#ifndef  KONPU_RANDOM_H_
#define  KONPU_RANDOM_H_
#include "c.h"

// void RandomInit([[uint64_t seed], uint64_t seq]);
// Re-Initialize the Random Number Generator (RNG)
// The state of the RNG can be initialized in two parts, a seed and a sequence
// selection constant.
#define RandomInit(...) \
   UTIL_OVERLOAD(PrivateRandomInit, __VA_ARGS__)
   void PrivateRandomInit_0_(void);
   void PrivateRandomInit_1_(uint64_t seed);
   void PrivateRandomInit_2_(uint64_t seed, uint64_t seq);

// uint8_t Random8([uint8_t bound]);
// Return an 8-bit pseudo-random number.
// if bound is specified then number will be in [0, bound[
#define Random8(...) \
   UTIL_OVERLOAD(PrivateRandom8, __VA_ARGS__)
#  define PrivateRandom8_0_()            Random32(UINT8_MAX)
#  define PrivateRandom8_1_(bound8)      Random32((uint8_t)(bound8))

// uint16_t Random16([uint16_t bound]);
// Return a 16-bit pseudo-random number.
// if bound is specified then number will be in [0, bound[
#define Random16(...) \
   UTIL_OVERLOAD(PrivateRandom16, __VA_ARGS__)
#  define PrivateRandom16_0_()           Random32(UINT16_MAX)
#  define PrivateRandom16_1_(bound16)    Random32((uint16_t)(bound16))

// uint32_t Random32([uint32_t bound]);
// Return a 32-bit pseudo-random number.
// if bound is specified then number will be in [0, bound[
#define Random32(...) \
   UTIL_OVERLOAD(PrivateRandom32, __VA_ARGS__)
   uint32_t PrivateRandom32_0_(void);
   uint32_t PrivateRandom32_1_(uint32_t bound);

// uint64_t Random64([uint64_t bound]);
// Return a 64-bit pseudo-random number.
// if bound is specified then number will be in [0, bound[
#define Random64(...) \
   UTIL_OVERLOAD(PrivateRandom64, __VA_ARGS__)
#  define PrivateRandom64_0_()  \
      ((uint64_t)PrivateRandom32_0_() << 32 | PrivateRandom32_0_())
   static inline uint64_t PrivateRandom64_1_(uint64_t bound) {
      return (bound <= UINT32_MAX)
         ?  PrivateRandom32_1_(bound)
         :  (uint64_t)PrivateRandom32_1_(bound >> 32) << 32 | PrivateRandom32_0_();
   }

// <stdlib.h>-like functionalities: RAND_MAX, rand(), srand()
#ifdef KONPU_H_
#  undef    RAND_MAX
#  undef    rand
#  if INT_WIDTH > 32
#    define RAND_MAX     UINT32_MAX
#    define rand()       ((int)Random32())
#  else
#    define RAND_MAX     INT_MAX
#    define rand()       ((int)Random32(RAND_MAX))
#  endif
#  undef    srand
#  define   srand(seed)  RandomInit(seed, 0)
#endif

#endif //include guard
