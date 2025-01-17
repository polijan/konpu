#ifndef  KONPU_BITS_H
#define  KONPU_BITS_H
#include "c.h"

//------------------------------------------------------------------------------
// Chunk Terminology in ilo Konpu:
//------------------------------------------------------------------------------
// A "Chunk"   is a certain number of bits contained within an unsigned integer.
// A "Byte"    is an 8-bits chunk (value is in [0-255]).
// A "Nibble"  is a  4-bits chunk (value is in [0-15]).
// A "Quarter" is a  2-bits chunk (value is in [0-3]).
// A "Bit"     is a  1-bit  chunk (value is {0,1}).
//------------------------------------------------------------------------------


// BITS(...) allows to write binary literals. It accepts from 1 to 8 arguments
// written as byte in binary and returns a value (of type `unsigned int`).
//
// Examples:
//   BITS(10) => 2
//   BITS(10011100) => 156
//   BITS(11001001,01101011) => as an unsigned int
//   BITS(11011001,01001001,01101011,00010111)
//       => as an unsigned int or uint32_t (whatever type is bigger)
//
// The type of the value is as follow:
// 1 or 2 args => returns an unsigned int
// 3 or 4 args => returns a  uint32_t (or unsigned int, whatever is bigger)
// 5+ args     => returns a  uint64_t (or unsigned int, whatever is bigger)
//
// Note: C23 introduced binary literals, but using the BITS macro allows
//       something (a little bit) similar with prior versions of C too.
#define BITS(...) \
   UTIL_OVERLOAD(BITS, __VA_ARGS__)
#  define BITS_0x_(b)  0x##b##u
#  define BITS_1_(binary_lit)                            ( \
      ((BITS_0x_(binary_lit) & 0x0000000Fu) ?   1u : 0u) | \
      ((BITS_0x_(binary_lit) & 0x000000F0u) ?   2u : 0u) | \
      ((BITS_0x_(binary_lit) & 0x00000F00u) ?   4u : 0u) | \
      ((BITS_0x_(binary_lit) & 0x0000F000u) ?   8u : 0u) | \
      ((BITS_0x_(binary_lit) & 0x000F0000u) ?  16u : 0u) | \
      ((BITS_0x_(binary_lit) & 0x00F00000u) ?  32u : 0u) | \
      ((BITS_0x_(binary_lit) & 0x0F000000u) ?  64u : 0u) | \
      ((BITS_0x_(binary_lit) & 0xF0000000u) ? 128u : 0u) )
#  define BITS_2_(b1, b2) \
      ((BITS_1_(b1) << 8) | BITS_1_(b2))
#  define BITS_4_(b1, b2, b3, b4) \
      (((uint32_t)BITS_2_(b1, b2) << 16) | BITS_2_(b3, b4))
#  define BITS_8_(b1, b2, b3, b4, b5, b6, b7, b8) \
      (((uint64_t)BITS_4_(b1, b2, b3, b4) << 32) | BITS_4_(b5, b6, b7, b8))
#  define BITS_3_(b1, b2, b3)                  BITS_4_(0, b1, b2, b3)
#  define BITS_5_(b1, b2, b3, b4, b5)          BITS_8_(0,0,0, b1,b2,b3,b4,b5)
#  define BITS_6_(b1, b2, b3, b4, b5, b6)      BITS_8_(0,0, b1,b2,b3,b4,b5,b6)
#  define BITS_7_(b1, b2, b3, b4, b5, b6, b7)  BITS_8_(0, b1,b2,b3,b4,b5,b6,b7)

// Same as the BITS() macro but given the result is reverse, it is as if the
// binary literal is given from the most to the least significant bits.
#define BITS_REVERSED(...) \
   UTIL_OVERLOAD(BITS_REVERSED, __VA_ARGS__)
#  define BITS_REVERSED_1_(binary_lit)                   ( \
      ((BITS_0x_(binary_lit) & 0x0000000Fu) ? 128u : 0u) | \
      ((BITS_0x_(binary_lit) & 0x000000F0u) ?  64u : 0u) | \
      ((BITS_0x_(binary_lit) & 0x00000F00u) ?  32u : 0u) | \
      ((BITS_0x_(binary_lit) & 0x0000F000u) ?  16u : 0u) | \
      ((BITS_0x_(binary_lit) & 0x000F0000u) ?   8u : 0u) | \
      ((BITS_0x_(binary_lit) & 0x00F00000u) ?   4u : 0u) | \
      ((BITS_0x_(binary_lit) & 0x0F000000u) ?   2u : 0u) | \
      ((BITS_0x_(binary_lit) & 0xF0000000u) ?   1u : 0u) )
#  define BITS_REVERSED_2_(b1, b2)                         \
      ((BITS_REVERSED_1_(b2) << 8) | BITS_REVERSED_1_(b1))
#  define BITS_REVERSED_4_(b1, b2, b3, b4)                 \
      (((uint32_t)BITS_REVERSED_2_(b3, b4) << 16) | BITS_REVERSED_2_(b1, b2))
#  define BITS_REVERSED_8_(b1, b2, b3, b4, b5, b6, b7, b8) \
      (((uint64_t)BITS_REVERSED_4_(b5, b6, b7, b8) << 32)  \
               |  BITS_REVERSED_4_(b1, b2, b3, b4))
#  define BITS_REVERSED_3_(b1, b2, b3)                  BITS_REVERSED_4_(0, b1, b2, b3)
#  define BITS_REVERSED_5_(b1, b2, b3, b4, b5)          BITS_REVERSED_8_(0,0,0, b1,b2,b3,b4,b5)
#  define BITS_REVERSED_6_(b1, b2, b3, b4, b5, b6)      BITS_REVERSED_8_(0,0, b1,b2,b3,b4,b5,b6)
#  define BITS_REVERSED_7_(b1, b2, b3, b4, b5, b6, b7)  BITS_REVERSED_8_(0, b1,b2,b3,b4,b5,b6,b7)


// Unsigned integers as "chunk" arrays

// Those macros evaluate their paramaters only once:
#define BITS_BIT_AT(bits, index)            (((bits) >> ((index)     )) & 1u)
#define BITS_QUARTER_AT(bits, index)        (((bits) >> ((index) << 1)) & 3u)
#define BITS_NIBBLE_AT(bits, index)         (((bits) >> ((index) << 2)) & 0xFu)
#define BITS_BYTE_AT(bits, index)           (((bits) >> ((index) << 3)) & 0xFFu)
// `chunk_bit_sz` (the size of the chunk in bit) is evaluated twice:
#define BITS_CHUNK_AT(bits, index, chunk_bit_sz) \
   (((bits) >> ((chunk_bit_sz) * (index))) & ((1u << ((chunk_bit_sz) + 1)) - 1))

// Deprecated names: TODO remove
#define BitsByteAt(bits, index)             BITS_BYTE_AT((bits), (index))
#define BitsNibbleAt(bits, index)           BITS_NIBBLE_AT((bits), (index))
#define BitsQuarterAt(bits, index)          BITS_QUARTER_AT((bits), (index))
#define BitsBitAt(bits, index)              BITS_BIT_AT((bits), (index))


#endif //KONPU_BITS_H
