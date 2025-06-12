// Various small Functions and utility MACROs

#ifndef KONPU_UTIL_H_
#define KONPU_UTIL_H_
#include "c.h" // c.h also defines many UTIL_* macros

// Similar to the `container_of` macro as popularized by the Linux kernel. I.e:
// Given the type T of a "container" struct and the name of the member the
// pointer refers to, returns a pointer to the struct that contains that member.
// For more info, see: https://radek.io/posts/magical-container_of-macro/
#define UTIL_CONTAINER_OF(pointer, T, member) \
   ((T*)((char*)(1 ? (pointer) : &((T*)0)->member) - offsetof(T, member)))

// Clamp the value of `n` into [0..dimension[
static inline void UtilClampCoordinate(int *n, int dimension) {
   if (*n < 0) {
      *n = 0;
   } else if (*n >= dimension) {
      *n = dimension - 1;
   }
}

// A comma. (Occasionally useful to pass a comma as a macro parameter)
#define UTIL_COMMA ,

// Return the (signed) number of elements within an array type.
// Note / Warning: Only works for arrays which have NOT decayed to a pointer.
#define UTIL_ARRAY_SIZE(array)      (C_SIZEOF(array) / C_SIZEOF(0[array]))

// Return the (signed) length of a C string literal.
#define UTIL_STRING_LITERAL_LENGTH(literal_string) \
   (C_SIZEOF(literal_string) - 1)

// Return non-zero iff the given integer is a power of two.
// Note: macro evaluates its argument several times.
#define UTIL_IS_POWER_OF_TWO(x)          (((x) > 0) && !((x) & ((x) - 1)))

// Return non-zero iff the given integer is a power of two or zero.
// Note: macro evaluates its argument several times.
#define UTIL_IS_POWER_OF_TWO_OR_ZERO(x)  (((x) >= 0) && !((x) & ((x) - 1)))

// Return the maximum of a and b
// Warning: No macro safety (a and b will be evaluated twice)
#define UTIL_MAX(a,b)         (((a) >= (b))? (a) : (b))

// Return the minimum of a and b
// Warning: No macro safety (a and b will be evaluated twice)
#define UTIL_MIN(a,b)         (((a) <= (b))? (a) : (b))

// Mathematical constant Pi (as a double)
#define UTIL_PI \
   3.1415926535897932384626433832795028841971693993751058209749445923078164062

// Mathematical constant Pi (as a float)
#define UTIL_PIf \
   3.1415926535897932384626433832795028841971693993751058209749445923078164062f

#endif //include guard
