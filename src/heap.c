// Konpu's heap allocator works from a fixed block of memory.
// It is based on/inspired by the MEMSYS5 allocator from SQLite (which is public
// domain). The allocator works using the Buddy algorithm and thus assumes the
// backing memory is a power-of-two in bytes.

#include "heap.h"
#include "init.h"
#include "var.h"
#include <string.h>


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

// Log base 2 of min allocation size in bytes
#define LOGMIN        3

// Log base 2 of max allocation size in bytes
// Maximum size of any allocation is ((1<<LOGMAX)*HEAP_ALLOC_MIN)
#define LOGMAX        16

// Masks used for HEAP_CTRL[] elements.
#define CTRL_LOGSIZE  0x1f    // Log2 Size of this block
#define CTRL_FREE     0x20    // True if not checked out

// Number of HEAP_ALLOC_MIN sized blocks in the heap available for allocation.
//#define NBLOCK  ((1<<19) / (HEAP_ALLOC_MIN + 1))
//#define NBLOCK  ((1 << LOGMAX)*HEAP_ALLOC_MIN / (HEAP_ALLOC_MIN + 1))
//#define NBLOCK (UINT32_C(1) << 16)
#define NBLOCK        65536
// ---> nBlock = (nByte / (HEAP_ALLOC_MIN+sizeof(uint8_t)));



// A minimum allocation is an instance of the following structure.
// Larger allocations are an array of these structures where the
// size of the array is a power of 2.
typedef struct HeapChunck {
   int32_t next;  // Index of next free chunk
   int32_t prev;  // Index of previous free chunk
} HeapChunck;
// The size of this object must be a power of two.
static_assert(sizeof(HeapChunck) == HEAP_ALLOC_MIN);

// Assuming HEAP_MEMORY is divided up into an array of HeapChunck
// structures, return a pointer to the idx-th such link.
#define HeapChunkAt(idx) ((HeapChunck *)(&HEAP_MEMORY[(idx) * HEAP_ALLOC_MIN]))


// Lists of free blocks. heap_freelist[0] is a list of free blocks of
// size HEAP_ALLOC_MIN.  heap_freelist[1] holds blocks of size HEAP_ALLOC_MIN*2.
// heap_freelist[2] holds free blocks of size HEAP_ALLOC_MIN*4. And so forth.
//static uint16_t heap_freelist[LOGMAX + 1]; //MARCHE PAS
static int32_t heap_freelist[LOGMAX + 1];


// Performance statistics
#if KONPU_PLATFORM_LIBC && !defined(NDEBUG)
#define HEAP_STATS
static struct {
   uint64_t nAlloc;        // Total number of calls to malloc
   uint64_t totalAlloc;    // Total of all malloc calls - includes internal frag
   uint64_t totalExcess;   // Total internal fragmentation
   uint32_t currentOut;    // Current checkout, including internal fragmentation
   uint32_t currentCount;  // Current number of distinct checkouts
   uint32_t maxOut;        // Maximum instantaneous currentOut
   uint32_t maxCount;      // Maximum instantaneous currentCount
   uint32_t maxRequest;    // Largest allocation (exclusive of internal frag)
} stats;

// TODO <-- this is to prevent -Wmissing-protopypes for now
void HeapDump(const char *zFilename);
#endif


// Unlink the i-th chunk from list it is currently on.
// It should be found on heap_freelist[iLogsize].
static void HeapUnlink(int32_t i, int iLogsize)
{
   assert(i >= 0 && i < NBLOCK);
   assert(iLogsize >= 0 && iLogsize <= LOGMAX);
   assert((HEAP_CTRL[i] & CTRL_LOGSIZE) == iLogsize);

   int32_t next = HeapChunkAt(i)->next;
   int32_t prev = HeapChunkAt(i)->prev;
   if (prev < 0) {
      heap_freelist[iLogsize] = next;
   } else {
      HeapChunkAt(prev)->next = next;
   }
   if (next >= 0) {
      HeapChunkAt(next)->prev = prev;
   }
}

// Link the n-th chunk so that is on the iLogsize free list.
static void HeapLink(int32_t n, int iLogsize)
{
   assert(n >= 0 && n < NBLOCK);
   assert(iLogsize >= 0 && iLogsize <= LOGMAX);
   assert((HEAP_CTRL[n] & CTRL_LOGSIZE) == iLogsize);

   int32_t x = HeapChunkAt(n)->next = heap_freelist[iLogsize];
   HeapChunkAt(n)->prev = -1;
   if (x >= 0) {
      assert(x < NBLOCK);
      HeapChunkAt(x)->prev = n;
   }
   heap_freelist[iLogsize] = n;
}

void HeapInit(void)
{
   for (int i = 0; i <= LOGMAX; i++) {
      heap_freelist[i] = -1;
   }
//   heap_freelist[LOGMAX] = 1; /// MARCHE PAS

   int32_t iOffset = 0;  // An offset into HEAP_CTRL[]
   for (int i = LOGMAX; i >=0; i--) {
      int32_t nAlloc = (1 << i);
      if ((iOffset + nAlloc) <= NBLOCK) {
         HEAP_CTRL[iOffset] = i | CTRL_FREE;
         HeapLink(iOffset, i);
         iOffset += nAlloc;
      }
      assert((iOffset + nAlloc) > NBLOCK);
   }
}


/*
// Return the size of an outstanding allocation, in bytes.
// This only works for chunks that are currently checked out.
int32_t HeapCapacityOf(void *p)
{
   assert( p!=0 );
   int32_t i = (int32_t)(((uint8_t *)p-HEAP_MEMORY)/HEAP_ALLOC_MIN);
   assert(i>=0 && i< NBLOCK);
   int32_t iSize = HEAP_ALLOC_MIN * (1 << (HEAP_CTRL[i]&CTRL_LOGSIZE));
   return iSize;
}
*/


// Return a block of memory of at least nBytes in size.
// Return NULL if unable.  Return NULL if nBytes<=0.
void *HeapMalloc(int32_t nByte)
{
   // nByte must be a positive
   if (nByte <= 0) return NULL;

   // No more than 512Kb per allocation
   if (nByte > ((INT32_C(1) << LOGMAX) * HEAP_ALLOC_MIN)) return NULL;

#ifdef HEAP_STATS
   // Keep track of the maximum allocation request.
   // Even unfulfilled requests are counted
   if ((uint32_t)nByte > stats.maxRequest) {
      stats.maxRequest = nByte;
   }
#endif

   // Round nByte up to the next valid power of two
   int32_t iFullSz; // Size of allocation rounded up to power of 2
   int iLogsize;    // Log2 of iFullSz/POW2_MIN
   for (iFullSz = HEAP_ALLOC_MIN, iLogsize = 0;
        iFullSz < nByte;
        iFullSz *= 2, iLogsize++) { }

   // Make sure heap_freelist[iLogsize] contains at least one free block.
   // If not, then split a block of the next larger power of two in order
   // to create a new free block of size iLogsize.
   int32_t iBin;  // Index into heap_freelist[]
   for (iBin = iLogsize; iBin <= LOGMAX && heap_freelist[iBin] < 0; iBin++) {}
   if (iBin > LOGMAX) return NULL;
   int32_t i = heap_freelist[iBin];  // Index of a chunck
   HeapUnlink(i, iBin);
   while (iBin > iLogsize) {
      int32_t newSize;

      iBin--;
      newSize = 1 << iBin;
      HEAP_CTRL[i + newSize] = CTRL_FREE | iBin;
      HeapLink(i + newSize, iBin);
   }
   HEAP_CTRL[i] = iLogsize;

#ifdef HEAP_STATS
   // Update allocator performance statistics.
   stats.nAlloc++;
   stats.totalAlloc += iFullSz;
   stats.totalExcess += iFullSz - nByte;
   stats.currentCount++;
   stats.currentOut += iFullSz;
   if (stats.maxCount < stats.currentCount) stats.maxCount = stats.currentCount;
   if (stats.maxOut   < stats.currentOut)   stats.maxOut = stats.currentOut;
#endif

#ifndef NDEBUG
   // Make sure the allocated memory does not assume that it is set to zero
   // or retains a value from a previous allocation
   memset(&HEAP_MEMORY[i*HEAP_ALLOC_MIN], 0xAA, iFullSz);
#endif

   // Return a pointer to the allocated memory.
   return (void*)&HEAP_MEMORY[i*HEAP_ALLOC_MIN];
}

// Free an outstanding memory allocation.
void HeapFree(void *pOld)
{
   if (pOld == NULL) return;

   // Set iBlock to the index of the block pointed to by pOld in
   // the array of HEAP_ALLOC_MIN byte blocks pointed to by HEAP_MEMORY.
   int32_t iBlock = (int32_t)(((uint8_t *)pOld-HEAP_MEMORY)/HEAP_ALLOC_MIN);

   // Check that the pointer pOld points to a valid, non-free block.
   assert(iBlock>=0 && iBlock < NBLOCK);
   assert(((uint8_t *)pOld-HEAP_MEMORY)%HEAP_ALLOC_MIN==0);
   assert((HEAP_CTRL[iBlock] & CTRL_FREE) == 0);

   uint32_t iLogsize = HEAP_CTRL[iBlock] & CTRL_LOGSIZE;
   uint32_t size = 1 << iLogsize;
   assert( iBlock+size-1<(uint32_t)NBLOCK );

   HEAP_CTRL[iBlock] |= CTRL_FREE;
   HEAP_CTRL[iBlock+size-1] |= CTRL_FREE;

#ifdef HEAP_STATS
   assert( stats.currentCount>0 );
   assert( stats.currentOut>=(size*HEAP_ALLOC_MIN) );
   stats.currentCount--;
   stats.currentOut -= size*HEAP_ALLOC_MIN;
   assert( stats.currentOut>0 || stats.currentCount==0 );
   assert( stats.currentCount>0 || stats.currentOut==0 );
#endif

   HEAP_CTRL[iBlock] = CTRL_FREE | iLogsize;
   while (true) { //  while( ALWAYS(iLogsize<LOGMAX) ) {
      int32_t iBuddy;
      if ((iBlock >> iLogsize) & 1) {
         iBuddy = iBlock - size;
         assert( iBuddy>=0 );
      } else {
         iBuddy = iBlock + size;
         if (iBuddy >= NBLOCK) break;
      }
      if (HEAP_CTRL[iBuddy] != (CTRL_FREE | iLogsize)) break;
      HeapUnlink(iBuddy, iLogsize);
      iLogsize++;
      if (iBuddy < iBlock) {
         HEAP_CTRL[iBuddy] = CTRL_FREE | iLogsize;
         HEAP_CTRL[iBlock] = 0;
         iBlock = iBuddy;
      } else {
         HEAP_CTRL[iBlock] = CTRL_FREE | iLogsize;
         HEAP_CTRL[iBuddy] = 0;
      }
      size *= 2;
   }

#ifndef NDEBUG
   // Overwrite freed memory with the 0x55 bit pattern to verify that it is
   // not used after being freed
   memset(&HEAP_MEMORY[iBlock*HEAP_ALLOC_MIN], 0x55, size);
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

//   int32_t nOld = HeapCapacityOf(pPrior);
   int32_t nOld = HeapCapacityAt(HeapAddress(pPrior));

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


#ifdef HEAP_STATS
#include <stdio.h>
#include <inttypes.h>

// Open the file indicated and write a log of all unfreed memory
// allocations into that log.
void HeapDump(const char *zFilename)
{
   FILE *out;
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

   fprintf(out, "freelists (size -> # free blocks): ");
   for (int i = 0; i <= LOGMAX && i + LOGMIN < 32; i++) {
      int n = 0;
      for (int j = heap_freelist[i]; j >= 0; j = HeapChunkAt(j)->next, n++) {}
      //fprintf(out, "freelist items of size %d: %d\n", HEAP_ALLOC_MIN << i, n);
      fprintf(out, "(%d -> %d) ", HEAP_ALLOC_MIN << i, n);
   }
   fprintf(out, "\nstats.nAlloc       = %" PRIu64 "\n", stats.nAlloc);
   fprintf(out, "stats.totalAlloc   = %"   PRIu64 "\n", stats.totalAlloc);
   fprintf(out, "stats.totalExcess  = %"   PRIu64 "\n", stats.totalExcess);
   fprintf(out, "stats.currentOut   = %"   PRIu32 "\n", stats.currentOut);
   fprintf(out, "stats.currentCount = %"   PRIu32 "\n", stats.currentCount);
   fprintf(out, "stats.maxOut       = %"   PRIu32 "\n", stats.maxOut);
   fprintf(out, "stats.maxCount     = %"   PRIu32 "\n", stats.maxCount);
   fprintf(out, "stats.maxRequest   = %"   PRIu32 "\n", stats.maxRequest);

   if (out == stdout) {
      fflush(stdout);
   } else {
      fclose(out);
   }
}
#endif //HEAP_STATS
