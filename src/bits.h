#ifndef  KONPU_BITS_H_
#define  KONPU_BITS_H_
#include "c.h"

// uint32_t or unsigned int, whatever is the larger type
#if UINT_WIDTH >= 32
   typedef unsigned int BITS_MAX32_T;
#else
   typedef uint32_t BITS_MAX32_T;
#endif

// uint64_t or unsigned int, whatever is the larger type
#if UINT_WIDTH >= 64
   typedef unsigned int BITS_MAX64_T;
#else
   typedef uint64_t BITS_MAX64_T;
#endif


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
// BITS_SET_*(bits, index, value) --> return an unsigned integer with same value
//                                 as bits but with chunk set to the given value
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
      uint8_t : BITS_SET_BIT_u_      , \
      uint16_t: BITS_SET_BIT_u_      , \
      uint32_t: BITS_SET_BIT_u32_    , \
      uint64_t: BITS_SET_BIT_u64_      \
   )((uint), (index), (bit))
   static inline unsigned BITS_SET_BIT_u_(unsigned x, unsigned n, unsigned bit)
   { return (x & ~(1u << n)) | (bit << n); }
   static inline BITS_MAX32_T BITS_SET_BIT_u32_(uint32_t x, unsigned n, BITS_MAX32_T bit)
   { return (x & ~(UINT32_C(1) << n)) | (bit << n); }
   static inline BITS_MAX64_T BITS_SET_BIT_u64_(uint64_t x, unsigned n, BITS_MAX64_T bit)
   { return (x & ~(UINT64_C(1) << n)) | (bit << n); }

#define BITS_SET_QUARTER(uint, index, quarter) \
   _Generic((uint)                           , \
      uint8_t : BITS_SET_QUARTER_u_          , \
      uint16_t: BITS_SET_QUARTER_u_          , \
      uint32_t: BITS_SET_QUARTER_u32_        , \
      uint64_t: BITS_SET_QUARTER_u64_          \
   )((uint), (index), (quarter))
   static inline unsigned BITS_SET_QUARTER_u_(unsigned x, unsigned n, unsigned quarter)
   { n <<= 1; return (x & ~(0x3u << n)) | (quarter << n); }
   static inline BITS_MAX32_T BITS_SET_QUARTER_u32_(uint32_t x, unsigned n, BITS_MAX32_T quarter)
   { n <<= 1; return (x & ~(UINT32_C(3) << n)) | (quarter << n); }
   static inline BITS_MAX64_T BITS_SET_QUARTER_u64_(uint64_t x, unsigned n, BITS_MAX64_T quarter)
   { n <<= 1; return (x & ~(UINT64_C(3) << n)) | (quarter << n); }

#define BITS_SET_NIBBLE(uint, index, nibble) \
   _Generic((uint)                         , \
      uint8_t : BITS_SET_NIBBLE_u_         , \
      uint16_t: BITS_SET_NIBBLE_u_         , \
      uint32_t: BITS_SET_NIBBLE_u32_       , \
      uint64_t: BITS_SET_NIBBLE_u64_         \
   )((uint), (index), (nibble))
   static inline unsigned BITS_SET_NIBBLE_u_(unsigned x, unsigned n, unsigned nibble)
   { n <<= 2; return (x & ~(0xFu << n)) | (nibble << n); }
   static inline BITS_MAX32_T BITS_SET_NIBBLE_u32_(uint32_t x, unsigned n, BITS_MAX32_T nibble)
   { n <<= 2; return (x & ~(UINT32_C(0xF) << n)) | (nibble << n); }
   static inline BITS_MAX64_T BITS_SET_NIBBLE_u64_(uint64_t x, unsigned n, BITS_MAX64_T nibble)
   { n <<= 2; return (x & ~(UINT64_C(0xF) << n)) | (nibble << n); }

#define BITS_SET_BYTE(uint, index, byte)                      \
   _Generic((uint)                                          , \
      uint8_t : (byte)                                      , \
      uint16_t: BITS_SET_BYTE_u_((uint), (index), (byte))   , \
      uint32_t: BITS_SET_BYTE_u32_((uint), (index), (byte)) , \
      uint64_t: BITS_SET_BYTE_u64_((uint), (index), (byte))   )
   static inline unsigned BITS_SET_BYTE_u_(unsigned x, unsigned n, unsigned byte)
   { n <<= 3; return (x & ~(0xFFu << n)) | (byte << n); }
   static inline BITS_MAX32_T BITS_SET_BYTE_u32_(uint32_t x, unsigned n, BITS_MAX32_T byte)
   { n <<= 3; return (x & ~(UINT32_C(0xFF) << n)) | (byte << n); }
   static inline BITS_MAX64_T BITS_SET_BYTE_u64_(uint64_t x, unsigned n, BITS_MAX64_T byte)
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
      // see: https://gcc.gnu.org/onlinedocs/gcc/Byte-Swapping-Builtins.html
#else
#  define BITS_SWAP_BYTE(uint)               \
   _Generic((uint)                         , \
      uint8_t:                      (uint) , \
      uint16_t: BITS_SWAP_BYTE_u16_((uint)), \
      uint32_t: BITS_SWAP_BYTE_u32_((uint)), \
      uint64_t: BITS_SWAP_BYTE_u64_((uint))  )
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
#if __STDC_VERSION__ < 202311L
#  define BITS_HAS_A_SINGLE_ONE(x)              ((x) && !((x) & ((x) - 1)))
#else
#  define BITS_HAS_A_SINGLE_ONE(x)              stdc_has_single_bit((x))
#endif

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


//------------------------------------------------------------------------------
// CLZ_ / CTZ_ / Popcount ... possibly very FAST (using builtins in GCC/CLANG)
//
// CLZ_/CTZ_ *MUST* only be used when INPUT IS NOT 0 (that requirement from
// the GCC builtins). For a safe function, handling zero too, use the <stdbit.h>
// macros: stdc_leading_zeros() / std_trailling_zeros()  [see: next section]
//
// GCC builtins, see:
// bits      : https://gcc.gnu.org/onlinedocs/gcc/Bit-Operation-Builtins.html
// byte swaps: https://gcc.gnu.org/onlinedocs/gcc/Byte-Swapping-Builtins.html
//------------------------------------------------------------------------------

// (shortcut / #undef'ed at the end of this section)
#define BITS_  static inline int C_ATTRIBUTE_UNSEQUENCED

// CTZ (= Count TRAILING Zeros) is functionally like stdc_trailing_zeros ...
// but is UNDEFINED when n is 0
#ifdef __GNUC__
#  define BitsCTZ_(n)                         \
      _Generic((n)                          , \
         unsigned char     : __builtin_ctz  , \
         unsigned short    : __builtin_ctz  , \
         unsigned int      : __builtin_ctz  , \
         unsigned long     : __builtin_ctzl , \
         unsigned long long: __builtin_ctzll  \
      )((n))
   // also define width-precise functions_:
   BITS_ BitsCTZ_8_( uint8_t  n) { return BitsCTZ_(n); }
   BITS_ BitsCTZ_16_(uint16_t n) { return BitsCTZ_(n); }
   BITS_ BitsCTZ_32_(uint32_t n) { return BitsCTZ_(n); }
   BITS_ BitsCTZ_64_(uint64_t n) { return BitsCTZ_(n); }

#elif __STDC_VERSION__ >= 202311L
#  define BitsCTZ_(n)      stdc_trailing_zeros((n))

#else // Pure C fallback: Binary Search Style (at most 6 operations)
#  define BitsCTZ_(n)             \
      _Generic((n)              , \
         uint8_t : BitsCTZ_8_   , \
         uint16_t: BitsCTZ_16_  , \
         uint32_t: BitsCTZ_32_  , \
         uint64_t: BitsCTZ_64_    \
      )((n))
      BITS_ BitsCTZ_8_(uint8_t n) {
         int count = 0;
         if ((n & 0x0F) == 0) { count += 4; n >>= 4; }
         if ((n & 0x03) == 0) { count += 2; n >>= 2; }
         if ((n & 0x01) == 0) { count += 1; }
         return count;
      }
      BITS_ BitsCTZ_16_(uint16_t n) {
         int count = 0;
         if ((n & 0x00FF) == 0) { count += 8; n >>= 8; }
         if ((n & 0x000F) == 0) { count += 4; n >>= 4; }
         if ((n & 0x0003) == 0) { count += 2; n >>= 2; }
         if ((n & 0x0001) == 0) { count += 1; }
         return count;
      }
      BITS_ BitsCTZ_32_(uint32_t n) {
         int count = 0;
         if ((n & 0x0000FFFF) == 0) { count += 16; n >>= 16; }
         if ((n & 0x000000FF) == 0) { count +=  8; n >>=  8; }
         if ((n & 0x0000000F) == 0) { count +=  4; n >>=  4; }
         if ((n & 0x00000003) == 0) { count +=  2; n >>=  2; }
         if ((n & 0x00000001) == 0) { count +=  1; }
         return count;
      }
      BITS_ BitsCTZ_64_(uint64_t n) {
         int count = 0;
         if ((n & 0x00000000FFFFFFFFULL) == 0) { count += 32; n >>= 32; }
         if ((n & 0x000000000000FFFFULL) == 0) { count += 16; n >>= 16; }
         if ((n & 0x00000000000000FFULL) == 0) { count +=  8; n >>=  8; }
         if ((n & 0x000000000000000FULL) == 0) { count +=  4; n >>=  4; }
         if ((n & 0x0000000000000003ULL) == 0) { count +=  2; n >>=  2; }
         if ((n & 0x0000000000000001ULL) == 0) { count +=  1; }
         return count;
      }
#endif


// CLZ (= Count LEADING zeros) is functionally like stdc_leading_zeros ...
// but is UNDEFINED when n is 0
#ifdef __GNUC__
#  define BitsCLZ_(n)                                                          \
      _Generic((n)                                                           , \
         /* builtin_clz is for int, char/short may have less leading 0s */     \
         unsigned char : (__builtin_clz((n)) - (UINT_WIDTH - UCHAR_WIDTH))   , \
         unsigned short: (__builtin_clz((n)) - (UINT_WIDTH - USHRT_WIDTH))   , \
         unsigned int      : __builtin_clz((n))                              , \
         unsigned long     : __builtin_clzl((n))                             , \
         unsigned long long: __builtin_clzll((n))                              \
      )
   // also define width-precise functions_:
   BITS_ BitsCLZ_8_( uint8_t  n) { return BitsCLZ_(n); }
   BITS_ BitsCLZ_16_(uint16_t n) { return BitsCLZ_(n); }
   BITS_ BitsCLZ_32_(uint32_t n) { return BitsCLZ_(n); }
   BITS_ BitsCLZ_64_(uint64_t n) { return BitsCLZ_(n); }

#elif __STDC_VERSION__ >= 202311L
#  define BitsCLZ_(n)      stdc_leading_zeros((n))

#else // Pure C fallback: Binary Search Style (at most 6 operations)
#  define BitsCLZ_(n)           \
      _Generic((n)            , \
         uint8_t : BitsCLZ_8_ , \
         uint16_t: BitsCLZ_16_, \
         uint32_t: BitsCLZ_32_, \
         uint64_t: BitsCLZ_64_  \
      )((n))
      BITS_ BitsCLZ_8_(uint8_t n) {
         int count = 0;
         if ((n & 0xF0) == 0) { count += 4; n <<= 4; }
         if ((n & 0xC0) == 0) { count += 2; n <<= 2; }
         if ((n & 0x80) == 0) { count += 1; }
         return count;
      }
      BITS_ BitsCLZ_16_(uint16_t n) {
         int count = 0;
         if ((n & 0xFF00) == 0) { count += 8; n <<= 8; }
         if ((n & 0xF000) == 0) { count += 4; n <<= 4; }
         if ((n & 0xC000) == 0) { count += 2; n <<= 2; }
         if ((n & 0x8000) == 0) { count += 1; }
         return count;
      }
      BITS_ BitsCLZ_32_(uint32_t n) {
         int count = 0;
         if ((n & 0xFFFF0000) == 0) { count += 16; n <<= 16; }
         if ((n & 0xFF000000) == 0) { count +=  8; n <<=  8; }
         if ((n & 0xF0000000) == 0) { count +=  4; n <<=  4; }
         if ((n & 0xC0000000) == 0) { count +=  2; n <<=  2; }
         if ((n & 0x80000000) == 0) { count +=  1; }
         return count;
      }
      BITS_ BitsCLZ_64_(uint64_t n) {
         int count = 0;
         if ((n & 0xFFFFFFFF00000000ULL) == 0) { count += 32; n <<= 32; }
         if ((n & 0xFFFF000000000000ULL) == 0) { count += 16; n <<= 16; }
         if ((n & 0xFF00000000000000ULL) == 0) { count +=  8; n <<=  8; }
         if ((n & 0xF000000000000000ULL) == 0) { count +=  4; n <<=  4; }
         if ((n & 0xC000000000000000ULL) == 0) { count +=  2; n <<=  2; }
         if ((n & 0x8000000000000000ULL) == 0) { count +=  1; }
         return count;
      }
#endif

// Popcount is functionally like stdc_count_ones
#ifdef __GNUC__
#  define BitsPopcount(n)                           \
      _Generic((n)                                , \
         unsigned char     : __builtin_popcount   , \
         unsigned short    : __builtin_popcount   , \
         unsigned int      : __builtin_popcount   , \
         unsigned long     : __builtin_popcountl  , \
         unsigned long long: __builtin_popcountll   \
      )((n))
   // also define those as they will be used in <stdbit> implementation
   BITS_ BitsPopcount_8_(uint8_t n)    { return BitsPopcount(n); }
   BITS_ BitsPopcount_16_(uint16_t n)  { return BitsPopcount(n); }
   BITS_ BitsPopcount_32_(uint32_t n)  { return BitsPopcount(n); }
   BITS_ BitsPopcount_64_(uint64_t n)  { return BitsPopcount(n); }

#elif __STDC_VERSION__ >= 202311L
#  define BitsPopcount(n)  stdc_count_ones((n))

#else // Pure C fallback if we don't have GCC builtins
#  define BitsPopcount(n)             \
      _Generic((n)                  , \
         uint8_t : BitsPopcount_8_  , \
         uint16_t: BitsPopcount_16_ , \
         uint32_t: BitsPopcount_32_ , \
         uint64_t: BitsPopcount_64_   \
      ((n)) // implemenation based on Hacker's Delight (by Henry S. Warren)
      BITS_ BitsPopcount_8_(uint8_t n) {
         n = n - ((n >> 1) & 0x55);
         n = (n & 0x33) + ((n >> 2) & 0x33);
         n = (n + (n >> 4)) & 0x0F;
         return n;
      }
      BITS_ BitsPopcount_16_(uint16_t n) {
         n = n - ((n >> 1) & 0x5555);
         n = (n & 0x3333) + ((n >> 2) & 0x3333);
         n = (n + (n >> 4)) & 0x0F0F;
         n = n + (n >> 8);
         return n & 0x1F;
      }
      BITS_ BitsPopcount_32_(uint32_t n) {
         n = n - ((n >> 1) & 0x55555555);
         n = (n & 0x33333333) + ((n >> 2) & 0x33333333);
         n = (n + (n >> 4)) & 0x0F0F0F0F;
         n = n + (n >> 8);
         n = n + (n >> 16);
         return n & 0x3F;
      }
      BITS_ BitsPopcount_64_(uint64_t n) {
         n = n - ((n >> 1) & 0x5555555555555555ULL);
         n = (n & 0x3333333333333333ULL) + ((n >> 2) & 0x3333333333333333ULL);
         n = (n + (n >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
         n = n + (n >> 8);
         n = n + (n >> 16);
         n = n + (n >> 32);
         return n & 0x7F;
      }
#endif //ifdef __GNUC__

#undef BITS_

//------------------------------------------------------------------------------
// <stdbit.h> - working with bytes and bits
//
// Konpu C garantuees the following from C23 (even for C11/C17):
//
// - Endianness of scalar types: __STDC_ENDIAN_LITTLE__, __STDC_ENDIAN_BIG__,
//   and __STDC_ENDIAN_NATIVE__ which *might* be one of the above
//
// - Type generic macros: stdc_leading_zeros, stdc_leading_ones,
//   stdc_trailing_zeros, stdc_trailing_ones, stdc_first_leading_zero,
//   stdc_first_leading_one, stdc_first_trailing_zero, stdc_first_trailing_one,
//   stdc_count_zeros, stdc_count_ones, stdc_has_single_bit, stdc_bit_width,
//   stdc_bit_floor, stdc_bit_ceil
//
// One should use the type generic macros only and NOT the stdc_*_u[c|s|i|l|ll].
// Konpu only garantuees that macros work on parameters of type
// uint<8|16|32|64>_t.
//
// see: https://en.cppreference.com/w/c/header/stdbit.html
//------------------------------------------------------------------------------

// stdc_*(n) macros:
//
// leading_zeros        count the number of consecutive ​0​ bits, starting from the most significant bit
// leading_ones         count the number of consecutive 1 bits, starting from the most significant bit
// trailing_zeros       count the number of consecutive ​0​ bits, starting from the least significant bit
// trailing_ones        count the number of consecutive 1 bits, starting from the least significant bit
// first_leading_zero   find the first position of ​0​ bit, starting from the most significant bit
// first_leading_one    find the first position of 1 bit, starting from the most significant bit
// first_trailing_zero  find the first position of ​0​ bit, starting from the least significant bit
// first_trailing_one   find the first position of 1 bit, starting from the least significant bit
// count_zeros          count the number of ​0​ bits
// count_ones           count the number of 1 bits
// has_single_bit       check if a number is an integral power of 2
// bit_width            find the smallest number of bits needed to represent the given value
// bit_floor            find the largest integral power of 2 not greater than the given value
// bit_ceil             find the smallest integral power of 2 not less than the given value


#if __STDC_VERSION__ >= 202311L
// Include the header as it's fully freestanding.
#  include <stdbit.h>
#else

#  ifdef __GNUC__
#     define    __STDC_ENDIAN_BIG__          __ORDER_BIG_ENDIAN__
#     define    __STDC_ENDIAN_LITTLE__       __ORDER_LITTLE_ENDIAN__
#     define    __STDC_ENDIAN_NATIVE__       __BYTE_ORDER__
#  else
#     define    __STDC_ENDIAN_BIG__          4321
#     define    __STDC_ENDIAN_LITTLE__       1234
#     ifdef KONPU_ENDIAN_
#        define __STDC_ENDIAN_NATIVE__       KONPU_ENDIAN_
         // TODO: "KONPU_ENDIAN_" is supposed to be defined from the Makefile
         //       when compiling Konpu. But when generating konpu.h, this
         //       needs to be done differently (maybe substitued)....
#     else
#        define __STDC_ENDIAN_NATIVE__       0  // unknown
#     endif
#  endif

   // shortcut / will be #undef'ed at the end of the section
#  define BITS_  static inline unsigned int C_ATTRIBUTE_UNSEQUENCED

#  define stdc_leading_zeros(n)                   \
      _Generic((n)                              , \
         uint8_t : Bits_stdc_leading_zeros_8_   , \
         uint16_t: Bits_stdc_leading_zeros_16_  , \
         uint32_t: Bits_stdc_leading_zeros_32_  , \
         uint64_t: Bits_stdc_leading_zeros_64_    \
      )((n))
      BITS_ Bits_stdc_leading_zeros_8_(uint8_t n)    { return n ? BitsCLZ_8_(n)  : UINT8_WIDTH; }
      BITS_ Bits_stdc_leading_zeros_16_(uint16_t n)  { return n ? BitsCLZ_16_(n) : UINT16_WIDTH; }
      BITS_ Bits_stdc_leading_zeros_32_(uint32_t n)  { return n ? BitsCLZ_32_(n) : UINT32_WIDTH; }
      BITS_ Bits_stdc_leading_zeros_64_(uint64_t n)  { return n ? BitsCLZ_64_(n) : UINT64_WIDTH; }

#  define stdc_trailing_zeros(n)                  \
      _Generic((n)                              , \
         uint8_t : Bits_stdc_trailing_zeros_8_  , \
         uint16_t: Bits_stdc_trailing_zeros_16_ , \
         uint32_t: Bits_stdc_trailing_zeros_32_ , \
         uint64_t: Bits_stdc_trailing_zeros_64_   \
      )((n))
      BITS_ Bits_stdc_trailing_zeros_8_(uint8_t n)    { return n ? BitsCTZ_8_(n)  : UINT8_WIDTH; }
      BITS_ Bits_stdc_trailing_zeros_16_(uint16_t n)  { return n ? BitsCTZ_16_(n) : UINT16_WIDTH; }
      BITS_ Bits_stdc_trailing_zeros_32_(uint32_t n)  { return n ? BitsCTZ_32_(n) : UINT32_WIDTH; }
      BITS_ Bits_stdc_trailing_zeros_64_(uint64_t n)  { return n ? BitsCTZ_64_(n) : UINT64_WIDTH; }

#  define stdc_leading_ones(n)                    \
      _Generic((n)                              , \
         uint8_t : Bits_stdc_leading_zeros_8_   , \
         uint16_t: Bits_stdc_leading_zeros_16_  , \
         uint32_t: Bits_stdc_leading_zeros_32_  , \
         uint64_t: Bits_stdc_leading_zeros_64_    \
      )(~(n))

#  define stdc_trailing_ones(n)                   \
      _Generic((n)                              , \
         uint8_t : Bits_stdc_trailing_zeros_8_  , \
         uint16_t: Bits_stdc_trailing_zeros_16_ , \
         uint32_t: Bits_stdc_trailing_zeros_32_ , \
         uint64_t: Bits_stdc_trailing_zeros_64_   \
      )(~(n))

#  define stdc_first_leading_zero(n)                  \
      _Generic((n)                                  , \
         uint8_t : Bits_stdc_first_leading_zero_8_  , \
         uint16_t: Bits_stdc_first_leading_zero_16_ , \
         uint32_t: Bits_stdc_first_leading_zero_32_ , \
         uint64_t: Bits_stdc_first_leading_zero_64_   \
      )((n))
      BITS_ Bits_stdc_first_leading_zero_8_(uint8_t n)    { return (n == UINT8_MAX)  ? 0 : 1 + stdc_leading_ones(n); }
      BITS_ Bits_stdc_first_leading_zero_16_(uint16_t n)  { return (n == UINT16_MAX) ? 0 : 1 + stdc_leading_ones(n); }
      BITS_ Bits_stdc_first_leading_zero_32_(uint32_t n)  { return (n == UINT32_MAX) ? 0 : 1 + stdc_leading_ones(n); }
      BITS_ Bits_stdc_first_leading_zero_64_(uint64_t n)  { return (n == UINT64_MAX) ? 0 : 1 + stdc_leading_ones(n); }

#  define stdc_first_leading_one(n)                   \
      _Generic((n)                                  , \
         uint8_t : Bits_stdc_first_leading_one_8_   , \
         uint16_t: Bits_stdc_first_leading_one_16_  , \
         uint32_t: Bits_stdc_first_leading_one_32_  , \
         uint64_t: Bits_stdc_first_leading_one_64_    \
      )((n))
      BITS_ Bits_stdc_first_leading_one_8_(uint8_t n)    { return n ?  1 + stdc_leading_zeros(n) : 0; }
      BITS_ Bits_stdc_first_leading_one_16_(uint16_t n)  { return n ?  1 + stdc_leading_zeros(n) : 0; }
      BITS_ Bits_stdc_first_leading_one_32_(uint32_t n)  { return n ?  1 + stdc_leading_zeros(n) : 0; }
      BITS_ Bits_stdc_first_leading_one_64_(uint64_t n)  { return n ?  1 + stdc_leading_zeros(n) : 0; }

#  define stdc_first_trailing_one(n)                  \
      _Generic((n)                                  , \
         uint8_t : Bits_stdc_first_trailing_one_8_  , \
         uint16_t: Bits_stdc_first_trailing_one_16_ , \
         uint32_t: Bits_stdc_first_trailing_one_32_ , \
         uint64_t: Bits_stdc_first_trailing_one_64_   \
      )((n))
      BITS_ Bits_stdc_first_trailing_one_8_(uint8_t  n)   { return n ? BitsCTZ_(n) : 0; }
      BITS_ Bits_stdc_first_trailing_one_16_(uint16_t n)  { return n ? BitsCTZ_(n) : 0; }
      BITS_ Bits_stdc_first_trailing_one_32_(uint32_t n)  { return n ? BitsCTZ_(n) : 0; }
      BITS_ Bits_stdc_first_trailing_one_64_(uint64_t n)  { return n ? BitsCTZ_(n) : 0; }

#  define stdc_first_trailing_zero(n)                 \
      _Generic((n)                                  , \
         uint8_t : Bits_stdc_first_trailing_one_8_  , \
         uint16_t: Bits_stdc_first_trailing_one_16_ , \
         uint32_t: Bits_stdc_first_trailing_one_32_ , \
         uint64_t: Bits_stdc_first_trailing_one_64_   \
      )((~n))

#  define stdc_count_ones(n)    ((unsigned)BitsPopcount((n)))

#  define stdc_count_zeros(n)   ((usigned))( \
      _Generic((n)                         , \
         uint8_t : BitsPopcount_8_         , \
         uint16_t: BitsPopcount_16_        , \
         uint32_t: BitsPopcount_32_        , \
         uint64_t: BitsPopcount_64_          \
      )(~(n)) )

#  define stdc_bit_width(n)                  \
      _Generic((n)                         , \
         uint8_t : Bits_stdc_bit_width_8_  , \
         uint16_t: Bits_stdc_bit_width_16_ , \
         uint32_t: Bits_stdc_bit_width_32_ , \
         uint64_t: Bits_stdc_bit_width_64_   \
      )((n))
      BITS_ Bits_stdc_bit_width_8_(uint8_t  n)   { return UINT8_WIDTH  - stdc_leading_zeros(n); }
      BITS_ Bits_stdc_bit_width_16_(uint16_t n)  { return UINT16_WIDTH - stdc_leading_zeros(n); }
      BITS_ Bits_stdc_bit_width_32_(uint32_t n)  { return UINT32_WIDTH - stdc_leading_zeros(n); }
      BITS_ Bits_stdc_bit_width_64_(uint64_t n)  { return UINT64_WIDTH - stdc_leading_zeros(n); }

// Contrarily to above, the following don't return unsigned int.
#  undef  BITS_
#  define BITS_(T) static inline T C_ATTRIBUTE_UNSEQUENCED

#  define stdc_has_single_bit(n)                       \
      _Generic((n)                                   , \
         unsigned char     : stdc_has_single_bit_uc  , \
         unsigned short    : stdc_has_single_bit_us  , \
         unsigned int      : stdc_has_single_bit_ui  , \
         unsigned long     : stdc_has_single_bit_ul  , \
         unsigned long long: stdc_has_single_bit_ull   \
      )((n))
      BITS_(bool) stdc_has_single_bit_uc(unsigned char n)        { return BITS_HAS_A_SINGLE_ONE(n); }
      BITS_(bool) stdc_has_single_bit_us(unsigned short n)       { return BITS_HAS_A_SINGLE_ONE(n); }
      BITS_(bool) stdc_has_single_bit_ui(unsigned int n)         { return BITS_HAS_A_SINGLE_ONE(n); }
      BITS_(bool) stdc_has_single_bit_ul(unsigned long n)        { return BITS_HAS_A_SINGLE_ONE(n); }
      BITS_(bool) stdc_has_single_bit_ull(unsigned long long n)  { return BITS_HAS_A_SINGLE_ONE(n); }

#  define stdc_bit_floor(n)                  \
      _Generic((n)                         , \
         uint8_t : Bits_stdc_bit_floor_8_  , \
         uint16_t: Bits_stdc_bit_floor_16_ , \
         uint32_t: Bits_stdc_bit_floor_32_ , \
         uint64_t: Bits_stdc_bit_floor_64_   \
      )((n))
      BITS_(uint8_t) Bits_stdc_bit_floor_8_(uint8_t n)
      { return n ? UINT8_C(1)  << (UINT8_WIDTH  - BitsCLZ_8_(n)  - 1)  : 0; }
      BITS_(uint16_t) Bits_stdc_bit_floor_16_(uint16_t n)
      { return n ? UINT16_C(1) << (UINT16_WIDTH - BitsCLZ_16_(n) - 1)  : 0; }
      BITS_(uint32_t) Bits_stdc_bit_floor_32_(uint32_t n)
      { return n ? UINT32_C(1) << (UINT32_WIDTH - BitsCLZ_32_(n) - 1)  : 0; }
      BITS_(uint64_t) Bits_stdc_bit_floor_64_(uint64_t n)
      { return n ? UINT64_C(1) << (UINT64_WIDTH - BitsCLZ_64_(n) - 1)  : 0; }

#  define stdc_bit_ceil(n)                  \
      _Generic((n)                        , \
         uint8_t : Bits_stdc_bit_ceil_8_  , \
         uint16_t: Bits_stdc_bit_ceil_16_ , \
         uint32_t: Bits_stdc_bit_ceil_32_ , \
         uint64_t: Bits_stdc_bit_ceil_64_   \
      )((n))
      BITS_(uint8_t) Bits_stdc_bit_ceil_8_(uint8_t n)
      { return n ? UINT8_C(1)  << (UINT8_WIDTH  - BitsCLZ_8_(n - 1))  : 1; }
      BITS_(uint16_t) Bits_stdc_bit_ceil_16_(uint16_t n)
      { return n ? UINT16_C(1) << (UINT16_WIDTH - BitsCLZ_16_(n - 1)) : 1; }
      BITS_(uint32_t) Bits_stdc_bit_ceil_32_(uint32_t n)
      { return n ? UINT32_C(1) << (UINT32_WIDTH - BitsCLZ_32_(n - 1)) : 1; }
      BITS_(uint64_t) Bits_stdc_bit_ceil_64_(uint64_t n)
      { return n ? UINT64_C(1) << (UINT64_WIDTH - BitsCLZ_64_(n - 1)) : 1; }

#endif //__STDC_VERSION__
#undef BITS_


#endif //include guard
