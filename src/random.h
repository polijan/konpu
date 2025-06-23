#ifndef  KONPU_RANDOM_H_
#define  KONPU_RANDOM_H_
#include "c.h"

// void UtilInitRandom([[uint64_t seed], uint64_t seq])
// Re-Initialize the random generator
// the state of the random number generator, can be initialized in two parts,
// a seed and a sequence selection constant.
#define UtilInitRandom(...)    UTIL_OVERLOAD(UtilInitRandom, __VA_ARGS__)
   void UtilInitRandom_0_(void);
   void UtilInitRandom_1_(uint64_t seed);
   void UtilInitRandom_2_(uint64_t seed, uint64_t seq);

// uint32_t UtilRandom32([uint32_t bound])
// Return a 32-bit pseudo-random number.
// if bound is specified then number will be in [0, bound[
#define UtilRandom32(...)      UTIL_OVERLOAD(UtilRandom32, __VA_ARGS__)
   uint32_t UtilRandom32_0_(void);
   uint32_t UtilRandom32_1_(uint32_t bound);

// uint64_t UtilRandom64([uint64_t bound])
// Return a 64-bit pseudo-random number.
// if bound is specified then number will be in [0, bound[
#define UtilRandom64(...)      UTIL_OVERLOAD(UtilRandom64, __VA_ARGS__)
#  define   UtilRandom64_0_()  \
      ((uint64_t)UtilRandom32_0_() << 32 | UtilRandom32_0_())
   static inline uint64_t UtilRandom64_1_(uint64_t bound) {
      return (bound <= UINT32_MAX)
         ?  UtilRandom32_1_(bound)
         :  (uint64_t)UtilRandom32_1_(bound >> 32) << 32 | UtilRandom32_0_();
   }

// <stdlib.h>-like functionalities: RAND_MAX, rand(), srand()
#ifdef KONPU_H_
#  undef    RAND_MAX
#  undef    rand
#  if INT_WIDTH > 32
#    define RAND_MAX     UINT32_MAX
#    define rand()       ((int)UtilRandom32())
#  else
#    define RAND_MAX     INT_MAX
#    define rand()       ((int)UtilRandom32(RAND_MAX))
#  endif
#  undef    srand
#  define   srand(seed)  UtilInitRandom(seed, 0)
#endif

#endif //include guard
