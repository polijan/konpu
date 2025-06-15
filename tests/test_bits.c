#include "test.h"

// Print a number in binary
static void print_bin(uint64_t n) {
   if (n >> 1)  print_bin(n >> 1);
   putchar((n & 1)? '1' : '0');
}

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

static const char *testSwapByte(void) {
   uint8_t  u8  = 0xA3u;
   TestAssert("Byte Swap u8", BITS_SWAP_BYTE(u8) == 0xA3u);

   uint16_t u16 = 0xA2B5u;
   TestAssert("Byte Swap u16", BITS_SWAP_BYTE(u16) == 0xB5A2u);

   uint32_t u32 = 0x0245CAFEu;
   TestAssert("Byte Swap u32", BITS_SWAP_BYTE(u32) == 0xFECA4502u);

   uint64_t u64 = 0xFACADE07DEADBEEFu;
   TestAssert("Byte Swap u32", BITS_SWAP_BYTE(u64) == 0xEFBEADDE07DECAFAu);

   return 0;
}
static const char *testSwapNibble(void) {
   uint8_t  u8  = 0xA3u;
   TestAssert("Nibble Swap u8", BITS_SWAP_NIBBLE(u8) == 0x3Au);

   uint16_t u16 = 0xA2B5u;
   TestAssert("Nibble Swap u16", BITS_SWAP_NIBBLE(u16) == 0x5B2Au);

   uint32_t u32 = 0x0245CAFEu;
   TestAssert("Nibble Swap u32", BITS_SWAP_NIBBLE(u32) == 0xEFAC5420u);

   uint64_t u64 = 0xFACADE07DEADBEEFu;
   TestAssert("Nibble Swap u32", BITS_SWAP_NIBBLE(u64) == 0xFEEBDAED70EDACAFu);

   return 0;
}
static const char *testSwapBit(void) {
   uint64_t u64 = BITS(11000010,00111101,01110101,10111110,
                       01100101,11010101,11100111,00101111);
   TestAssert("Bit Swap u64", BITS_SWAP_BIT(u64) == BITS(
      11110100,11100111,10101011,10100110,
      01111101,10101110,10111100,01000011 ));

   return 0;
}

static const char *testBitsExtra(void)
{
   TestAssert("HAS_A_SINGLE_ONE (1)", BITS_HAS_A_SINGLE_ONE(1));
   TestAssert("HAS_A_SINGLE_ONE (2)", BITS_HAS_A_SINGLE_ONE(2));
   TestAssert("HAS_A_SINGLE_ONE (.)", BITS_HAS_A_SINGLE_ONE(1UL << 23));
   TestAssert("!HAS_A_SINGLE_ONE (0)", !BITS_HAS_A_SINGLE_ONE(0));
   TestAssert("!HAS_A_SINGLE_ONE (.)", !BITS_HAS_A_SINGLE_ONE(2343455UL));

   TestAssert("HAS_AT_MOST_A_SINGLE_ONE (0)", BITS_HAS_AT_MOST_A_SINGLE_ONE(0));
   TestAssert("HAS_AT_MOST_A_SINGLE_ONE (8)", BITS_HAS_AT_MOST_A_SINGLE_ONE(8));

   TestAssert("SET_RIGHTMOST_ZERO",
      BITS_SET_RIGHTMOST_ZERO(BITS(01000111)) == BITS(01001111));

   TestAssert("UNSET_RIGHTMOST_ZERO",
      BITS_UNSET_RIGHTMOST_ONE(BITS(01001010)) == BITS(01001000));

   TestAssert("BITS_ISOLATE_RIGHTMOST_ONE",
      BITS_ISOLATE_RIGHTMOST_ONE(BITS(01001010)) == BITS(000000010));

   TestAssert("BITS_ISOLATE_AND_SET_RIGHTMOST_ZERO",
         BITS_ISOLATE_AND_SET_RIGHTMOST_ZERO(BITS(01010111)) == BITS(00001000));

   TestAssert("BITS_RIGHT_PROPAGATE_RIGHTMOST_ONE",
      BITS_RIGHT_PROPAGATE_RIGHTMOST_ONE(BITS(01001000)) == BITS(01001111));

   return 0;
}

TEST_BEGIN
   TestRun(test_BITS);
   TestRun(testSwapBit);
   TestRun(testSwapNibble);
   TestRun(testSwapByte);
   TestRun(testBitsExtra);
TEST_END
