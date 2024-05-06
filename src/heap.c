// Konpu's heap allocator works from a fixed block of memory.
// It is based on the MEMSYS5 allocator from SQLite (which is public domain).
// The allocator works using using the Buddy algorithm and thus assumes the
// backing memory is a power-of-two in bytes.

// TODO: from config.h or something?
#define KONPU_DEBUG
#define KONPU_TEST

#include "heap.h"


/* This memory allocator uses the following algorithm:
**
**   1.  All memory allocation sizes are rounded up to a power of 2.
**
**   2.  If two adjacent free blocks are the halves of a larger block,
**       then the two blocks are coalesced into the single larger block.
**
**   3.  New memory is allocated from the first available free block.
**
** This algorithm is described in: J. M. Robson. "Bounds for Some Functions
** Concerning Dynamic Storage Allocation". Journal of the Association for
** Computing Machinery, Volume 21, Number 8, July 1974, pages 491-499.
**
** Let n be the size of the largest allocation divided by the minimum
** allocation size (after rounding all sizes up to a power of 2.)  Let M
** be the maximum amount of memory ever outstanding at one time.  Let
** N be the total amount of memory available for allocation.  Robson
** proved that this memory allocator will never breakdown due to
** fragmentation as long as the following constraint holds:
**
**      N >=  M*(1 + log2(n)/2) - n + 1
**
** The sqlite3_status() logic tracks the maximum values of n and M so
** that an application can, at any time, verify this constraint.
*/


// A minimum allocation is an instance of the following structure.
// Larger allocations are an array of these structures where the
// size of the array is a power of 2.
//
// The size of this object must be a power of two.  That fact is
// verified in HeapInit().
typedef struct HeapList {
   int32_t next;  // Index of next free chunk
   int32_t prev;  // Index of previous free chunk
} HeapList;

// The size of a HeapList object must be a power of two.
// Verify that this is case.
static_assert( (sizeof(HeapList)&(sizeof(HeapList)-1))==0 );
// static_assert( alignof(HeapList) <= KONPU_DEFAULT_ALIGNMENT );


// Maximum size of any allocation is ((1<<LOGMAX)*heap.szAtom). Since
// heap.szAtom is always at least 8 and 32-bit integers are used,
// it is not actually possible to reach this limit.
#define LOGMAX 30

// Masks used for heap.aCtrl[] elements.
#define CTRL_LOGSIZE  0x1f    // Log2 Size of this block
#define CTRL_FREE     0x20    // True if not checked out

// All of the static variables used by this module are collected
// into a single structure named "heap".  This is to keep the
// static variables organized and to reduce namespace pollution
// when this module is combined with other in the amalgamation.
static struct Heap {
   // Memory available for allocation
   int32_t szAtom;      // Smallest possible allocation in bytes
   int32_t nBlock;      // Number of szAtom sized blocks in zPool
   uint8_t *zPool;  // Memory available to be allocated

#if defined(KONPU_DEBUG) || defined(KONPU_TEST)
   // Performance statistics
   uint64_t nAlloc;        // Total number of calls to malloc
   uint64_t totalAlloc;    // Total of all malloc calls - includes internal frag
   uint64_t totalExcess;   // Total internal fragmentation
   uint32_t currentOut;    // Current checkout, including internal fragmentation
   uint32_t currentCount;  // Current number of distinct checkouts
   uint32_t maxOut;        // Maximum instantaneous currentOut
   uint32_t maxCount;      // Maximum instantaneous currentCount
   uint32_t maxRequest;    // Largest allocation (exclusive of internal frag)
#endif

   // Lists of free blocks.  aiFreelist[0] is a list of free blocks of
   // size heap.szAtom.  aiFreelist[1] holds blocks of size szAtom*2.
   // aiFreelist[2] holds free blocks of size szAtom*4.  And so forth.
   int32_t aiFreelist[LOGMAX+1];

   // Space for tracking which blocks are checked out and the size of each block.
   // One byte per block.
   uint8_t *aCtrl;

} heap;


//------------------------------------

Address  HeapAddress(void *ptr)
{ return (uint8_t*)ptr - heap.zPool; }

void *HeapPointer(Address address)
{ return heap.zPool + address; }

//------------------------------------


// Assuming heap.zPool is divided up into an array of HeapList
// structures, return a pointer to the idx-th such link.
#define HEAPLIST(idx) ((HeapList *)(&heap.zPool[(idx) * heap.szAtom]))

// Unlink the chunk at heap.aPool[i] from list it is currently on.
// It should be found on heap.aiFreelist[iLogsize].
static void HeapUnlink(int32_t i, int iLogsize) {
   int32_t next, prev;
   assert( i>=0 && i<heap.nBlock );
   assert( iLogsize>=0 && iLogsize<=LOGMAX );
   assert( (heap.aCtrl[i] & CTRL_LOGSIZE)==iLogsize );

   next = HEAPLIST(i)->next;
   prev = HEAPLIST(i)->prev;
   if (prev < 0) {
      heap.aiFreelist[iLogsize] = next;
   } else {
      HEAPLIST(prev)->next = next;
   }
   if (next >= 0) {
      HEAPLIST(next)->prev = prev;
   }
}

// Link the chunk at heap.aPool[i] so that is on the iLogsize free list.
static void HeapLink(int32_t i, int iLogsize) {
//  assert( sqlite3_mutex_held(heap.mutex) );
   assert( i>=0 && i<heap.nBlock );
   assert( iLogsize>=0 && iLogsize<=LOGMAX );
   assert( (heap.aCtrl[i] & CTRL_LOGSIZE)==iLogsize );

   int32_t x = HEAPLIST(i)->next = heap.aiFreelist[iLogsize];
   HEAPLIST(i)->prev = -1;
   if (x >= 0) {
      assert( x<heap.nBlock );
      HEAPLIST(x)->prev = i;
   }
   heap.aiFreelist[iLogsize] = i;
}

// Return the size of an outstanding allocation, in bytes.
// This only works for chunks that are currently checked out.
int32_t HeapCapacityOf(void *p) {
   int32_t iSize, i;
   assert( p!=0 );
   i = (int32_t)(((uint8_t *)p-heap.zPool)/heap.szAtom);
   assert( i>=0 && i<heap.nBlock );
   iSize = heap.szAtom * (1 << (heap.aCtrl[i]&CTRL_LOGSIZE));
   return iSize;
}

// Return a block of memory of at least nBytes in size.
// Return NULL if unable.  Return NULL if nBytes<=0.
void *HeapMalloc(int32_t nByte) {
   int32_t i;       // Index of a heap.aPool[] slot
   int32_t iBin;    // Index into heap.aiFreelist[]
   int32_t iFullSz; // Size of allocation rounded up to power of 2
   int iLogsize;    // Log2 of iFullSz/POW2_MIN

   // nByte must be a positive
   if (nByte <= 0) return NULL;

   // No more than 1GiB per allocation
   if (nByte > 0x40000000) return 0;

#if defined(KONPU_DEBUG) || defined(KONPU_TEST)
   // Keep track of the maximum allocation request.
   // Even unfulfilled requests are counted
   if ((uint32_t)nByte > heap.maxRequest) {
      heap.maxRequest = nByte;
   }
#endif


   // Round nByte up to the next valid power of two
   for (iFullSz = heap.szAtom, iLogsize = 0;
        iFullSz < nByte;
        iFullSz *= 2, iLogsize++) { }

   // Make sure heap.aiFreelist[iLogsize] contains at least one free block.
   // If not, then split a block of the next larger power of two in order
   // to create a new free block of size iLogsize.
   for (iBin = iLogsize; iBin <= LOGMAX && heap.aiFreelist[iBin] < 0; iBin++) {}
   if (iBin > LOGMAX) { // error
      //testcase( sqlite3GlobalConfig.xLog!=0 );
      //sqlite3_log(SQLITE_NOMEM, "failed to allocate %u bytes", nByte);
      return 0;
   }
   i = heap.aiFreelist[iBin];
   HeapUnlink(i, iBin);
   while (iBin > iLogsize) {
      int32_t newSize;

      iBin--;
      newSize = 1 << iBin;
      heap.aCtrl[i + newSize] = CTRL_FREE | iBin;
      HeapLink(i + newSize, iBin);
   }
   heap.aCtrl[i] = iLogsize;

#if defined(KONPU_DEBUG) || defined(KONPU_TEST)
   // Update allocator performance statistics.
   heap.nAlloc++;
   heap.totalAlloc += iFullSz;
   heap.totalExcess += iFullSz - nByte;
   heap.currentCount++;
   heap.currentOut += iFullSz;
   if (heap.maxCount < heap.currentCount) heap.maxCount = heap.currentCount;
   if (heap.maxOut < heap.currentOut) heap.maxOut = heap.currentOut;
#endif

#ifdef KONPU_DEBUG
   // Make sure the allocated memory does not assume that it is set to zero
   // or retains a value from a previous allocation
   memset(&heap.zPool[i*heap.szAtom], 0xAA, iFullSz);
#endif

   // Return a pointer to the allocated memory.
   return (void*)&heap.zPool[i*heap.szAtom];
}

// Free an outstanding memory allocation.
void HeapFree(void *pOld) {
   if (pOld == NULL) return;

   uint32_t size, iLogsize;
   int32_t iBlock;

   // Set iBlock to the index of the block pointed to by pOld in
   // the array of heap.szAtom byte blocks pointed to by heap.zPool.
   iBlock = (int32_t)(((uint8_t *)pOld-heap.zPool)/heap.szAtom);

   // Check that the pointer pOld points to a valid, non-free block.
   assert( iBlock>=0 && iBlock<heap.nBlock );
   assert( ((uint8_t *)pOld-heap.zPool)%heap.szAtom==0 );
   assert( (heap.aCtrl[iBlock] & CTRL_FREE)==0 );

   iLogsize = heap.aCtrl[iBlock] & CTRL_LOGSIZE;
   size = 1<<iLogsize;
   assert( iBlock+size-1<(uint32_t)heap.nBlock );

   heap.aCtrl[iBlock] |= CTRL_FREE;
   heap.aCtrl[iBlock+size-1] |= CTRL_FREE;

#if defined(KONPU_DEBUG) || defined(KONPU_TEST)
   assert( heap.currentCount>0 );
   assert( heap.currentOut>=(size*heap.szAtom) );
   heap.currentCount--;
   heap.currentOut -= size*heap.szAtom;
   assert( heap.currentOut>0 || heap.currentCount==0 );
   assert( heap.currentCount>0 || heap.currentOut==0 );
#endif

   heap.aCtrl[iBlock] = CTRL_FREE | iLogsize;
   while (true) { //  while( ALWAYS(iLogsize<LOGMAX) ) {
      int32_t iBuddy;
      if ((iBlock >> iLogsize) & 1) {
         iBuddy = iBlock - size;
         assert( iBuddy>=0 );
      } else {
         iBuddy = iBlock + size;
         if (iBuddy >= heap.nBlock) break;
      }
      if (heap.aCtrl[iBuddy] != (CTRL_FREE | iLogsize)) break;
      HeapUnlink(iBuddy, iLogsize);
      iLogsize++;
      if (iBuddy < iBlock) {
         heap.aCtrl[iBuddy] = CTRL_FREE | iLogsize;
         heap.aCtrl[iBlock] = 0;
         iBlock = iBuddy;
      } else {
         heap.aCtrl[iBlock] = CTRL_FREE | iLogsize;
         heap.aCtrl[iBuddy] = 0;
      }
      size *= 2;
   }

#ifdef KONPU_DEBUG
   // Overwrite freed memory with the 0x55 bit pattern to verify that it is
   // not used after being freed
   memset(&heap.zPool[iBlock*heap.szAtom], 0x55, size);
#endif

   HeapLink(iBlock, iLogsize);
}

// Change the size of an existing memory allocation.
void *HeapRealloc(void *pPrior, int32_t nBytes)
{
   // If pPrior is NULL, realloc is equivalent to malloc
   if (pPrior == NULL)
      return HeapMalloc(nBytes);

   // For realloc(p, 0) with p non-NULL:
   // - Traditionally, it was (usually?) equivalent to free(p)
   // - C17 made it Implementation-Defined
   // - C23 made it Undefined Behaviour
   // Thus one should no longer use the "realloc(p, 0) as free" trick in C.
   // In order not to create surprise, ilo Konpu's implementation is gonna
   // stick to the "traditional" behaviour.
   if (nBytes == 0) {
      HeapFree(pPrior);
      return NULL;
   }

   assert( nBytes > 0 );
/*
   // TODO/TOFIX
   // nBytes is always a value obtained from a prior call to HeapRound().
   // Hence nBytes is always a non-negative power of two.
   // If nBytes==0 that means that an oversize allocation
   // (an allocation larger than 0x40000000) was requested and this
   // routine should return 0 without freeing pPrior.
   assert( (nBytes&(nBytes-1))==0 );  // EV: R-46199-30249
   assert( nBytes >= 0 );
   if (nBytes == 0) {
      return NULL;
   }
*/

   int32_t nOld = HeapCapacityOf(pPrior);
   if (nBytes <= nOld) {
      return pPrior;
   }

   void *p = HeapMalloc(nBytes);
   if (p) {
      memcpy(p, pPrior, nOld);
      HeapFree(pPrior);
   }
   return p;
}

// Round up a request size to the next valid allocation size.  If
// the allocation is too large to be handled by this allocation system,
// return 0.
//
// All allocations must be a power of two and must be expressed by a
// 32-bit signed integer.  Hence the largest allocation is 0x40000000
// or 1073741824 bytes.
static int32_t HeapRoundup(int32_t n) {
   int32_t iFullSz;
   if (n <= heap.szAtom * 2) {
      if (n <= heap.szAtom) return heap.szAtom;
         return heap.szAtom*2;
   }
   if (n > 0x10000000) {
      if (n > 0x40000000) return 0;
      if (n > 0x20000000) return 0x40000000;
      return 0x20000000;
   }
   for (iFullSz = heap.szAtom*8; iFullSz < n; iFullSz *= 4);
   if ((iFullSz / 2) >= (int64_t)n) return iFullSz/2;
   return iFullSz;
}

// Return the ceiling of the logarithm base 2 of iValue.
//
// Examples:   HeapLog(1) -> 0
//             HeapLog(2) -> 1
//             HeapLog(4) -> 2
//             HeapLog(5) -> 3
//             HeapLog(8) -> 3
//             HeapLog(9) -> 4
static int HeapLog(int32_t iValue) {
   int iLog;
   for (iLog=0;
        (iLog < (int)((sizeof(int) * 8) - 1)) && (1 << iLog) < iValue;
        iLog++);
   return iLog;
}



// Initialize the memory allocator.
//
// zByte: Memory usable by this allocator
// nByte: Number of bytes of memory available to this allocator
void HeapInit(uint8_t *zByte, Size nByte) {
   assert(zByte != NULL);
   assert(nByte > 0);

   int nMinLog = 1/*???*/;   /* Log base 2 of minimum allocation size in bytes */
   /* boundaries on sqlite3GlobalConfig.mnReq are enforced in sqlite3_config() */
   //nMinLog = HeapLog(sqlite3GlobalConfig.mnReq);
   heap.szAtom = (1 << nMinLog);
   while ((int32_t)sizeof(HeapList) > heap.szAtom) {
      heap.szAtom = heap.szAtom << 1;
   }

   heap.nBlock = (nByte / (heap.szAtom+sizeof(uint8_t)));
   heap.zPool = zByte;
   heap.aCtrl = (uint8_t *)&heap.zPool[heap.nBlock*heap.szAtom];

   for (int ii=0; ii<=LOGMAX; ii++) {
      heap.aiFreelist[ii] = -1;
   }

   int32_t iOffset = 0;  // An offset into heap.aCtrl[]
   for (int ii=LOGMAX; ii>=0; ii--) {
      int32_t nAlloc = (1<<ii);
      if ((iOffset + nAlloc) <= heap.nBlock) {
         heap.aCtrl[iOffset] = ii | CTRL_FREE;
         HeapLink(iOffset, ii);
         iOffset += nAlloc;
      }
      assert((iOffset + nAlloc) > heap.nBlock);
   }
}

// Deinitialize this module.
void HeapDrop(void)
{ /* nothing to do */ }


#ifdef KONPU_TEST
#include <stdio.h>
// Open the file indicated and write a log of all unfreed memory
// allocations into that log.
void HeapDump(const char *zFilename) {
   FILE *out;
   int i, j, n;
   int nMinLog;

   if (zFilename == 0 || zFilename[0] == 0) {
      out = stdout;
   } else {
      out = fopen(zFilename, "w");
      if (out == 0) {
        fprintf(stderr, "** Unable to output memory debug output log: %s **\n",
                        zFilename);
        return;
      }
   }

   nMinLog = HeapLog(heap.szAtom);
   for (i = 0; i <= LOGMAX && i + nMinLog < 32; i++) {
      for (n = 0, j = heap.aiFreelist[i]; j >= 0; j = HEAPLIST(j)->next, n++) {}
      fprintf(out, "freelist items of size %d: %d\n", heap.szAtom << i, n);
   }
   fprintf(out, "heap.nAlloc       = %llu\n", heap.nAlloc);
   fprintf(out, "heap.totalAlloc   = %llu\n", heap.totalAlloc);
   fprintf(out, "heap.totalExcess  = %llu\n", heap.totalExcess);
   fprintf(out, "heap.currentOut   = %u\n", heap.currentOut);
   fprintf(out, "heap.currentCount = %u\n", heap.currentCount);
   fprintf(out, "heap.maxOut       = %u\n", heap.maxOut);
   fprintf(out, "heap.maxCount     = %u\n", heap.maxCount);
   fprintf(out, "heap.maxRequest   = %u\n", heap.maxRequest);

   if (out == stdout) {
      fflush(stdout);
   } else {
      fclose(out);
   }
}
#endif
