#include "test.h"

// Test: For each video mode, verify that the overall size of the elements that
//       should be the framebuffer correspond to VIDEO_SIZE.

// Various check for IsEqual
static const char *test_BITS(void)
{
   // We can test it all via static_asserts ...
   static_assert(BITS(0) == 0);
   static_assert(BITS(1) == 1);
   static_assert(BITS(10) == 2);
   static_assert(BITS(11) == 3);
   static_assert(BITS(111) == 7);
   static_assert(BITS(10,00000000) == 512);

   // Types for number of arguments to bit
   static_assert(sizeof(unsigned) == sizeof(BITS(01101011)));
   static_assert(sizeof(unsigned) == sizeof(BITS(01001001,01101011)));
#if UINT_WIDTH > 32
   static_assert(sizeof(unsigned) == sizeof(BITS(01001001,01101011,00010111)));
   static_assert(sizeof(unsigned) == sizeof(BITS(11011001,01001001,01101011,00010111)));
#else
   static_assert(sizeof(uint32_t) == sizeof(BITS(01001001,01101011,00010111)));
   static_assert(sizeof(uint32_t) == sizeof(BITS(11011001,01001001,01101011,00010111)));
#endif
#if UINT_WIDTH > 64
   static_assert(sizeof(unsigned) == sizeof(BITS(1,11011001,01001001,01101011,00010111)));
   static_assert(sizeof(unsigned) == sizeof(BITS(1,1,11011001,01001001,01101011,00010111)));
   static_assert(sizeof(unsigned) == sizeof(BITS(0,1,1,11011001,01001001,01101011,00010111)));
   static_assert(sizeof(unsigned) == sizeof(BITS(0,0,1,1,11011001,01001001,01101011,00010111)));
#else
   static_assert(sizeof(uint64_t) == sizeof(BITS(1,11011001,01001001,01101011,00010111)));
   static_assert(sizeof(uint64_t) == sizeof(BITS(1,1,11011001,01001001,01101011,00010111)));
   static_assert(sizeof(uint64_t) == sizeof(BITS(0,1,1,11011001,01001001,01101011,00010111)));
   static_assert(sizeof(uint64_t) == sizeof(BITS(0,0,1,1,11011001,01001001,01101011,00010111)));
#endif


   // As we only tested via static_assert, if it compiled, then we're all good:
   return 0;
}


TEST_BEGIN
   TestRun(test_BITS);

TEST_END
