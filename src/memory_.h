#ifndef  KONPU_MEMORY__H_
#define  KONPU_MEMORY__H_
#include "memory.h"
// Internal stuff only available to Konpu's .c files

// Expand to a few `static_assert` statements to ensure that the given address
// is a valid address in Konpu's RAM and is aligned with the given C type.
#define MEMORY_STATIC_ASSERT(ram_address, C_TYPENAME)                  \
        static_assert((ram_address) >= 0, "RAM address must be >= 0"); \
        static_assert((ram_address) < KONPU_MEMORY_SIZE,               \
                      "RAM address must be < KONPU_MEMORY_SIZE");      \
        static_assert((ram_address) % alignof(C_TYPENAME) == 0,        \
                      "RAM address alignement")


#endif //include guard
