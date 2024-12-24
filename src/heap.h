// Konpu's Heap allocator
// This allocator works from a fixed block of memory.

#ifndef  KONPU_HEAP_H
#define  KONPU_HEAP_H
#include "memory.h"
#include "var.h"

// The size in bytes of the smallest possible allocation from the Heap.
#define HEAP_ALLOC_MIN   8

// All objects in the Heap will be at least aligned to this value.
// One should not try to store types with higher alignement requirements
// in the Heap.
#define HEAP_ALIGNMENT   8

#define HEAP_MEMORY           (KonpuMemory + HEAP_ADDRESS)
#define HEAP_CTRL             (KonpuMemory + HEAP_CTRL_ADDRESS)

// Returns a pointer from a 16-bit heap address
static inline
void *HeapAt(uint16_t heap_address)
{ return HEAP_MEMORY + heap_address * HEAP_ALLOC_MIN; }

// Returns a var pointer from a 16-bit heap address
static inline
var *HeapVarAt(uint16_t heap_address)
{ return (var*)HEAP_MEMORY + heap_address; }

// Returns the size of an outstanding allocation at this address, in bytes.
// This only works for address for which allocation occured
static inline
int32_t HeapCapacityAt(uint16_t heap_address)
{
#define CTRL_LOGSIZE  0x1f
   return HEAP_ALLOC_MIN * (INT32_C(1) << (HEAP_CTRL[heap_address] & CTRL_LOGSIZE));
#undef  CTRL_LOGSIZE
 }

// Returns an 16-bit heap address from a pointer
// (the pointer MUST points to inside the Heap memory)
static inline
uint16_t HeapAddress(void *pointer)
{  assert(pointer != NULL);
   assert( (uint8_t *)pointer >= HEAP_MEMORY );
   assert( (uint8_t *)pointer <  HEAP_CTRL );
   return ((uint8_t *)pointer -  HEAP_MEMORY) / HEAP_ALLOC_MIN;
}


#define HeapPointerCapacity(pointer)  HeapCapacityAt(HeapAddress(pointer))

/*
// Size plays a similar role as `size_t` but signed
typedef int32_t  Size;

// Same as the `sizeof` operator but return a Size (instead of size_t)
#define SizeOf(T) \
        ((Size)sizeof(T))
*/


// Similar to <stdlib.h>'s `malloc` function:
// Return a block of memory of at least `size` bytes.
// Return NULL if unable (or if size <= 0).
void *HeapMalloc(int32_t size);

// Similar to <stdlib.h>'s `realloc` function:
// Change the size of an existing memory allocation, in bytes.
// Return NULL if unable (or if size <= 0).
void *HeapRealloc(void *ptr, int32_t new_size);

// Similar to <stdlib.h>'s `free` function:
// Free an outstanding memory allocation.
void  HeapFree(void* pointer);


#endif //KONPU_HEAP_H
