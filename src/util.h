// Various small Functions and utility MACROs

#ifndef KONPU_UTIL_H
#define KONPU_UTIL_H
#include "c.h" // c.h also defines many UTIL_* macros

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

// Return the (signed) size of a variable with an array type.
// Note / Warning: Only works for arrays which have NOT decayed to a pointer.
#define UTIL_ARRAY_SIZE(array)      ((int32_t)(sizeof(array) / sizeof *(array)))
// TODO: convert to a signed int32_t

// Return the (signed) length of a C string literal.
#define UTIL_STRING_LITERAL_LENGTH(literal_string) \
   (ssizeof(literal_string) - 1)

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

#endif //KONPU_UTIL_H
