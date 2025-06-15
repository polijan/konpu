#ifndef  KONPU_BITS_H_
#define  KONPU_BITS_H_
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

//------------------------------------------------------------------------------
// Unsigned integers as arrays of chunks
//------------------------------------------------------------------------------
// * indicates the type of chunk, it can be BIT, QUARTER, NIBBLE, BYTE,
//   (or sometimes CHUNK in which case the macro also takes the chunk bit size
//    as an extra parameter)
//
// BITS_GET_*(bits, index) --> get a chunk element from bits
// BITS_SET_*(bits, index, value) --> return bits with chunk set to value
// BITS_SWAP_*(bits) --> return bits array with chunk place is reversed
//     return type is same as what  was input
// BITS_REV_*(bits) --> return the array but the bits inside each chunk are
//                      reversed (TODO... or not?)
//------------------------------------------------------------------------------

#define BITS_GET_BIT(uint, index)           (((uint) >> ((index)     )) & 1u)
#define BITS_GET_QUARTER(uint, index)       (((uint) >> ((index) << 1)) & 3u)
#define BITS_GET_NIBBLE(uint, index)        (((uint) >> ((index) << 2)) & 0xFu)
#define BITS_GET_BYTE(uint, index)          (((uint) >> ((index) << 3)) & 0xFFu)
#define BITS_GET_CHUNK(uint, index, chunk_bit_sz) \
   (((uint) >> ((chunk_bit_sz) * (index))) & ((1u << ((chunk_bit_sz) + 1)) - 1))

#define BITS_SET_BIT(uint, index, bit) \
   _Generic((uint)                   , \
      uint32_t: BITS_SET_BIT_u32_    , \
      uint64_t: BITS_SET_BIT_u64_    , \
      default : BITS_SET_BIT_u_        \
   )((uint), (index), (bit))
   static inline unsigned BITS_SET_BIT_u_(unsigned x, unsigned n, unsigned bit)
   { return (x & ~(1u << n)) | (bit << n); }
   static inline uint32_t BITS_SET_BIT_u32_(uint32_t x, unsigned n, uint32_t bit)
   { return (x & ~(UINT32_C(1) << n)) | (bit << n); }
   static inline uint64_t BITS_SET_BIT_u64_(uint64_t x, unsigned n, uint64_t bit)
   { return (x & ~(UINT64_C(1) << n)) | (bit << n); }

#define BITS_SET_QUARTER(uint, index, quarter) \
   _Generic((uint)                           , \
      uint32_t: BITS_SET_QUARTER_u32_        , \
      uint64_t: BITS_SET_QUARTER_u64_        , \
      default : BITS_SET_QUARTER_u_           \
   )((uint), (index), (quarter))
   static inline unsigned BITS_SET_QUARTER_u_(unsigned x, unsigned n, unsigned quarter)
   { n <<= 1; return (x & ~(0x3u << n)) | (quarter << n); }
   static inline uint32_t BITS_SET_QUARTER_u32_(uint32_t x, unsigned n, uint32_t quarter)
   { n <<= 1; return (x & ~(UINT32_C(3) << n)) | (quarter << n); }
   static inline uint64_t BITS_SET_QUARTER_u64_(uint64_t x, unsigned n, uint64_t quarter)
   { n <<= 1; return (x & ~(UINT64_C(3) << n)) | (quarter << n); }

#define BITS_SET_NIBBLE(uint, index, nibble) \
   _Generic((uint)                         , \
      uint32_t: BITS_SET_NIBBLE_u32_       , \
      uint64_t: BITS_SET_NIBBLE_u64_       , \
      default : BITS_SET_NIBBLE_u_           \
   )((uint), (index), (nibble))
   static inline unsigned BITS_SET_NIBBLE_u_(unsigned x, unsigned n, unsigned nibble)
   { n <<= 2; return (x & ~(0xFu << n)) | (nibble << n); }
   static inline uint32_t BITS_SET_NIBBLE_u32_(uint32_t x, unsigned n, uint32_t nibble)
   { n <<= 2; return (x & ~(UINT32_C(0xF) << n)) | (nibble << n); }
   static inline uint64_t BITS_SET_NIBBLE_u64_(uint64_t x, unsigned n, uint64_t nibble)
   { n <<= 2; return (x & ~(UINT64_C(0xF) << n)) | (nibble << n); }

#define BITS_SET_BYTE(uint, index, byte)                      \
   _Generic((uint)                                          , \
      uint8_t : (byte)                                      , \
      uint16_t: BITS_SET_BYTE_u_((uint), (index), (byte))   , \
      uint32_t: BITS_SET_BYTE_u32_((uint), (index), (byte)) , \
      uint64_t: BITS_SET_BYTE_u64_((uint), (index), (byte))   )
   static inline unsigned BITS_SET_BYTE_u_(unsigned x, unsigned n, unsigned byte)
   { n <<= 3; return (x & ~(0xFFu << n)) | (byte << n); }
   static inline uint32_t BITS_SET_BYTE_u32_(uint32_t x, unsigned n, uint32_t byte)
   { n <<= 3; return (x & ~(UINT32_C(0xFF) << n)) | (byte << n); }
   static inline uint64_t BITS_SET_BYTE_u64_(uint64_t x, unsigned n, uint64_t byte)
   { n <<= 3; return (x & ~(UINT64_C(0xFF) << n)) | (byte << n); }



// TODO
#define BITS_SWAP_BIT(uint)          \
   _Generic((uint)                 , \
      uint64_t: BITS_SWAP_BIT_u64_   \
   )(uint)
   static inline uint64_t BITS_SWAP_BIT_u64_(uint64_t x) {
      x = (x >> 1  & 0x5555555555555555u) | (x << 1  & 0xaaaaaaaaaaaaaaaau); // swap adjacent bits
      x = (x >> 2  & 0x3333333333333333u) | (x << 2  & 0xccccccccccccccccu); // swap 2-bit pairs
      x = (x >> 4  & 0x0F0F0F0F0F0F0F0Fu) | (x << 4  & 0xF0F0F0F0F0F0F0F0u); // swap 4-bit nibbles
      x = (x >> 8  & 0x00FF00FF00FF00FFu) | (x << 8  & 0xFF00FF00FF00FF00u); // swap 8-bit bytes
      x = (x >> 16 & 0x0000FFFF0000FFFFu) | (x << 16 & 0xFFFF0000FFFF0000u); // swap 16-bit halves
      x = (x >> 32 & 0x00000000FFFFFFFFu) | (x << 32 & 0xFFFFFFFF00000000u); // swap 32-bit quarters
      return x;
   }

#define BITS_SWAP_QUARTER(uint) //TODO

#define BITS_SWAP_NIBBLE(uint)          \
   _Generic((uint)                    , \
      uint8_t:  BITS_SWAP_NIBBLE_u8_  , \
      uint16_t: BITS_SWAP_NIBBLE_u16_ , \
      uint32_t: BITS_SWAP_NIBBLE_u32_ , \
      uint64_t: BITS_SWAP_NIBBLE_u64_   \
   )(uint)
   static inline unsigned BITS_SWAP_NIBBLE_u8_(uint8_t x)
   { return x >> 4 | (x & 0x0Fu) << 4; }
   static inline unsigned BITS_SWAP_NIBBLE_u16_(uint16_t x) {
      x =  x            >> 8 | (x &   0xFFu) << 8;
      x = (x & 0xF0F0u) >> 4 | (x & 0x0F0Fu) << 4;
      return x;
   }
   static inline uint32_t BITS_SWAP_NIBBLE_u32_(uint32_t x) {
      x =  x                >> 16 | (x &     0xFFFFu) << 16;
      x = (x & 0xFF00FF00u) >>  8 | (x & 0x00FF00FFu) <<  8;
      x = (x & 0xF0F0F0F0u) >>  4 | (x & 0x0F0F0F0Fu) <<  4;
      return x;
   }
   static inline uint64_t BITS_SWAP_NIBBLE_u64_(uint64_t x) {
      x =  x                        >> 32 | (x &         0xFFFFFFFFu) << 32;
      x = (x & 0xFFFF0000FFFF0000u) >> 16 | (x & 0x0000FFFF0000FFFFu) << 16;
      x = (x & 0xFF00FF00FF00FF00u) >>  8 | (x & 0x00FF00FF00FF00FFu) <<  8;
      x = (x & 0xF0F0F0F0F0F0F0F0u) >>  4 | (x & 0x0F0F0F0F0F0F0F0Fu) <<  4;
      return x;
   }

#ifdef __GNUC__
#  define BITS_SWAP_BYTE(uint)               \
   _Generic((uint)                         , \
      uint8_t:                    (uint)   , \
      uint16_t: __builtin_bswap16((uint))  , \
      uint32_t: __builtin_bswap32((uint))  , \
      uint64_t: __builtin_bswap64((uint))  )
#else
#  define BITS_SWAP_BYTE(uint)               \
   _Generic((uint)                         , \
      uint8_t:                      (uint) , \
      uint16_t: BITS_SWAP_BYTE_u16_((uint)), \
      uint32_t: BITS_SWAP_BYTE_u32_((uint)), \
      uint64_t: BITS_SWAP_BYTE_u64_((uint))  \
   )(uint)
   static inline uint16_t BITS_SWAP_BYTE_u16_(uint16_t x)
   { return x << 8 | x >> 8; }
   static inline uint32_t BITS_SWAP_BYTE_u32_(uint32_t x)
   { return x << 24 | x >> 24 | ((x << 8) & 0xFF0000u) | ((x >> 8) & 0xFF00u); }
   static inline uint64_t BITS_SWAP_BYTE_u64_(uint64_t x) {
      x = x >> 32 | x << 32;
      x = (x & 0xFFFF0000FFFF0000u) >> 16 | (x & 0x0000FFFF0000FFFFu) << 16;
      x = (x & 0xFF00FF00FF00FF00u) >>  8 | (x & 0x00FF00FF00FF00FFu) <<  8;
      return x;
   }
#endif


//------------------------------------------------------------------------------
// Extra functionality for bit manipulation
// (inspiration: https://catonmat.net/bit-hacks-header-file)
//------------------------------------------------------------------------------

// return true if x has a single bit set to 1
#define BITS_HAS_A_SINGLE_ONE(x)                ((x) && !((x) & ((x) - 1)))

// return true if x is zero or has single bit set to 1
#define BITS_HAS_AT_MOST_A_SINGLE_ONE(x)        (!((x) & ((x) - 1)))

// Return x with the right-most 0-bit set to 1
// (ex: 0100[0]111 => 0100[1]111)
#define BITS_SET_RIGHTMOST_ZERO(x)              ((x) | ((x) + 1))

// return x with the right-most 1-bit unset
// (ex: 010010[1]0 => 010010[0]0)
#define BITS_UNSET_RIGHTMOST_ONE(x)             ((x) & ((x) - 1))

// return x with all bits set to 0 except the right-most 1 bit
// (ex: 010010[1]0 => 0000000[1]0)
#define BITS_ISOLATE_RIGHTMOST_ONE(x)           ((x) & -(x))

// return x with all bits set to 0 except the right-most 0 bit which is set to 1
// (ex: 0101[0]111 => 0000[1]000)
#define BITS_ISOLATE_AND_SET_RIGHTMOST_ZERO(x)  (~(x) & ((x) + 1))

// return x with all bits to right of the right-most 1-bit also set to 1
// (ex: 0100[1]000 => 0100[1111])
#define BITS_RIGHT_PROPAGATE_RIGHTMOST_ONE(x)   ((x) | ((x) - 1))


#endif //include guard
