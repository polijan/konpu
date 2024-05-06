// Konpu's heap allocator
// This allocator works from a fixed block of memory.

#ifndef  KONPU_HEAP_H
#define  KONPU_HEAP_H
#include "c.h"

// Size plays a similar role as `size_t` but signed
typedef int32_t  Size;

// Same as the `sizeof` operator but return a Size (instead of size_t)
#define SizeOf(T) \
        ((Size)sizeof(T))

// Heap alignement. All objects in the Heap will be aligned to this value.
// One should not use types with higher alignement requirements in this heap.
#define KONPU_DEFAULT_ALIGNMENT   8

// Address is an address in Konpu's heap.
// It plays a similar role as `intptr_t` or `ptrdiff_t`
typedef int32_t  Address;

static inline Address AddressAlignForward(Address address) {
   Size modulo = address & (KONPU_DEFAULT_ALIGNMENT - 1);
   if (modulo)   address += KONPU_DEFAULT_ALIGNMENT - modulo;
   return address;
}

static inline Address AddressAlignBackward(Address address)
{ return address - (address & (KONPU_DEFAULT_ALIGNMENT - 1)); }

//------------------------------------------------------------------------------



// Initialize the Heap.
// buffer: Block of memory usable by Konpu's heap
// size: Number of bytes of memory available
void     HeapInit(uint8_t *buffer, Size size);

// Deinitialize the Heap.
void     HeapDrop(void);

// Conversion between Addresses and Pointers
Address  HeapAddress(void *pointer);

// Given an address, return a pointer
//
void    *HeapPointer(Address address);

// Similar to <stdlib.h>'s `malloc` function:
// Return a block of memory of at least `size` bytes.
// Return NULL if unable or if size<=0.
void    *HeapMalloc(Size size);

// Similar to <stdlib.h>'s `realloc` function:
// Change the size of an existing memory allocation.
void    *HeapRealloc(void *ptr, Size new_size);

// Similar to <stdlib.h>'s `free` function:
// Free an outstanding memory allocation.
void     HeapFree(void* pointer);

// Return the size of an outstanding allocation, in bytes.
// This only works for chunks that are currently checked out.
Size     HeapCapacityOf(void* pointer);


// Setting: KONPU_SETTING_NO_STDLIB_INTERFACE
// if defined, do not override malloc/calloc/free with the Heap versions.
#ifndef    KONPU_SETTING_NO_STDLIB_INTERFACE
#   undef  malloc
#   define malloc(size)                 HeapMalloc(size)
#   undef  realloc
#   define realloc(pointer, new_size)   HeapRealloc(pointer, new_size)
#   undef  free
#   define free(sz)                     HeapFree(pointer)
#endif

#endif //KONPU_HEAP_H
