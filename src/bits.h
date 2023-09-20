#ifndef  KONPU_BITS_H
#define  KONPU_BITS_H
#include "platform.h"
#include "c.h"

#if KONPU_PLATFORM_LIBC && (__STDC_VERSION__ > 201710L)  // C23 or later
#   include <stdbit.h>  // note: this should define __STDC_VERSION_STDBIT_H__
#endif

//===< bits manipulation macros and function-like >=============================
// in all those:
// - 'x' *MUST* be of a unsigned integer type (otherwise undefined behaviour)
// - 'n' should be >= 0 and less than the number of bits of the type of x.
// we consider bits starting as 0 from the least-significant bit (rightmost)

// those three macros returns an unsigned int, a uint32_t, or a uint64_t
// respectively with only the n-th bit set
#define uint_fromBit(n)                                  (1U << (n))
#define uint32_fromBit(n)                       (UINT32_C(1) << (n))
#define uint64_fromBit(n)                       (UINT64_C(1) << (n))

// return the value 0 or 1 of the nth-bit of x
// (type of the returned value is same as x or promoted to unsigned int)
#define uint_bitValue(x, n)                     (((x) >> (n)) & 1U)

// return x with all bits set to 0 the n-th bit which is left untouched
#define   byte_isolateBit(x, n)                 ((x) &   uint_fromBit((n)))
#define uint16_isolateBit(x, n)                 ((x) &   uint_fromBit((n)))
#define uint32_isolateBit(x, n)                 ((x) & uint32_fromBit((n)))
#define uint64_isolateBit(x, n)                 ((x) & uint64_fromBit((n)))

// return a non-zero value iff the nth-bit of x is set
// notes:
// * this is intended to use as boolean, but really the type of the
//        returned value is same as x or promoted to unsigned int
// * it's generally better to use isSetBit than bitValue as boolean/tests
//   because bitValue is `(x >> n) & 1`,
//   where as isSetBit is `x & (1 << n)`,
//   and if n is a constant, that can take one op less.
#define   byte_isSetBit(x, n)                     byte_isolateBit((x), (n))
#define uint16_isSetBit(x, n)                   uint16_isolateBit((x), (n))
#define uint32_isSetBit(x, n)                   uint32_isolateBit((x), (n))
#define uint64_isSetBit(x, n)                   uint64_isolateBit((x), (n))

// return x with the n-th bit set to 1
#define uint_setBit(x, n)                       ((x) | uint_fromBit((n)))
#define uint32_setBit(x, n)                     ((x) | uint32_fromBit((n)))
#define uint64_setBit(x, n)                     ((x) | uint64_fromBit((n)))

// return x with the n-th bit unset (set to 0)
#define uint_unsetBit(x, n)                     ((x) & (~uint_fromBit((n))))
#define uint32_unsetBit(x, n)                   ((x) & (~uint32_fromBit((n))))
#define uint64_unsetBit(x, n)                   ((x) & (~uint64_fromBit((n))))

// return x with the n-th bit value changed
#define uint_toggleBit(x, n)                    ((x) ^ uint_fromBit((n)))
#define uint32_toggleBit(x, n)                  ((x) ^ uint32_fromBit((n)))
#define uint64_toggleBit(x, n)                  ((x) ^ uint64_fromBit((n)))


// return x with the right-most 0-bit set to 1
// (ex: 0100[0]111 => 0100[1]111)
#define UINT_SET_RIGHTMOST_ZERO(x)              ((x) | ((x) + 1))

// return x with the right-most 1-bit unset
// (ex: 010010[1]0 => 010010[0]0)
#define UINT_UNSET_RIGHTMOST_ONE(x)             ((x) & ((x) - 1))

// return x with all bits set to 0 except the right-most 1 bit
// (ex: 010010[1]0 => 0000000[1]0)
#define UINT_ISOLATE_RIGHTMOST_ONE(x)           ((x) & -(x))

// return x with all bits set to 0 except the right-most 0 bit which is set to 1
// (ex: 0101[0]111 => 0000[1]000)
#define UINT_ISOLATE_AND_SET_RIGHTMOST_ZERO(x)  (~(x) & ((x) + 1))

// return x with all bits to right of the right-most 1-bit also set to 1
// (ex: 0100[1]000 => 0100[1111])
#define UINT_RIGHT_PROPAGATE_RIGHTMOST_ONE(x)   ((x) | ((x) - 1))

//===</ bits manipulation macros and function-like >============================



//===< bytes manipulation macros and function-like >============================

#define uint_byteValue(x, n)                    (((x) >> 8*(n)) & 0xffU)

// this macro merges bits from two unsigned integers according to a mask.
// the mask should contains 0 where bits from x are selected, 1 where from y.
#define UINT_MERGE(x, y, mask)      ((x) ^ (((x) ^ (y)) & (mask)))
        // the obvious way would be (x & ~mask) | (y & mask), but this may
        // achieve the same with one operation less.
        // Based on "Bit Twiddling Hacks" by Sean Eron Anderson
        // https://graphics.stanford.edu/~seander/bithacks.html#MaskedMerge
        // (code snippet is in the public domain)   (sent by Ron Jeffery)

// reverse bits in a byte
#define byte_reverse(byte) \
        (((byte) * UINT64_C(0x0202020202) & UINT64_C(0x010884422010)) % 1023)
        // Based on "Bit Twiddling Hacks" by Sean Eron Anderson
        // https://graphics.stanford.edu/~seander/bithacks.html#ReverseByteWith64BitsDiv
        // (code snippet is in the public domain)
        // This uses three 64-bits operations (&, *, and %)
        // The method was attributed to Rich Schroeppel in the Programming Hacks
        // section of Beeler, M., Gosper, R. W., and Schroeppel, R. HAKMEM.
        // MIT AI Memo 239, Feb. 29, 1972.

//===</ bytes manipulation macros and function-like >===========================



//===< functions/macros that are (will be) similar to C23's <stdbit.h> =========

// (similar to stdc_leading_zeros*())
// int <T>_clz(T x): return number of trailing 0-bits in the value x
//                   (but result is UNDEFINED if x == 0)
#if defined(__GNUC__)
#   define      byte_clz(x)    (__builtin_clz( (x)  << (  UINT_WIDTH -  8)))
#   define    uint16_clz(x)    (__builtin_clz( (x)  << (  UINT_WIDTH - 16)))
#   if (UINT_WIDTH >= 32)
#      define uint32_clz(x)    (__builtin_clz(  (x) << (  UINT_WIDTH - 32)))
#   else
#      define uint32_clz(x)    (__builtin_clzl( (x) << ( ULONG_WIDTH - 32)))
#   endif
#   if (UINT_WIDTH >= 64)
#      define uint64_clz(x)    (__builtin_clz(  (x) << (  UINT_WIDTH - 64)))
#   elif (ULONG_WIDTH >= 64)
#      define uint64_clz(x)    (__builtin_clzl( (x) << ( ULONG_WIDTH - 64)))
#   else
#      define uint64_clz(x)    (__builtin_clzll((x) << (ULLONG_WIDTH - 64)))
#   endif
// #elif -> TODO: maybe intrinsic MSVC
#elif defined(__STDC_VERSION_STDBIT_H__) // C23 or later
#   define      byte_clz(x)    stdc_leading_zeros(( uint8_t)(x))
#   define    uint16_clz(x)    stdc_leading_zeros((uint16_t)(x))
#   define    uint32_clz(x)    stdc_leading_zeros((uint32_t)(x))
#   define    uint64_clz(x)    stdc_leading_zeros((uint64_t)(x))
#else
    # error "clz rely on GCC/CLANG builtins or on C23 or later"
    // TODO: provide portable C99 implementation
#endif


// (similar to stdc_trailing_zeros*())
// int <T>_ctz(T x): return number of trailing 0-bits in the value x
//                   (but result is UNDEFINED if x == 0)
#if defined(__GNUC__)
#   define      byte_ctz(x)    __builtin_ctz((x))
#   define    uint16_ctz(x)    __builtin_ctz((x))
#   if (UINT_WIDTH >= 32)
#      define uint32_ctz(x)    __builtin_ctz((x))
#   else
#      define uint32_ctz(x)    __builtin_ctzl((x))
#   endif
#   if (UINT_WIDTH >= 64)
#      define uint64_ctz(x)    __builtin_ctz((x))
#   elif (ULONG_WIDTH >= 64)
#      define uint64_ctz(x)    __builtin_ctzl((x))
#   else
#      define uint64_ctz(x)    __builtin_ctzll((x))
#   endif
// #elif -> TODO: maybe intrinsic MSVC
#elif defined __STDC_VERSION_STDBIT_H__ // C23 or later
#   define      byte_ctz(x)    stdc_trailing_zeros(( uint8_t)(x))
#   define    uint16_ctz(x)    stdc_trailing_zeros((uint16_t)(x))
#   define    uint32_ctz(x)    stdc_trailing_zeros((uint32_t)(x))
#   define    uint64_ctz(x)    stdc_trailing_zeros((uint64_t)(x))
#else
    # error "ctz rely on GCC/CLANG builtins or on C23 or later"
    // TODO: provide portable C99 implementation
#endif


// int <T>_count_ones(T x)
// return the number of 1-bits in the value x
#if defined(__GNUC__)
#   define      byte_count_ones(x)     __builtin_popcount((x))
#   define    uint16_count_ones(x)     __builtin_popcount((x))
#   if (UINT_WIDTH >= 32)
#      define uint32_count_ones(x)     __builtin_popcount((x))
#   else
#      define uint32_count_ones(x)     __builtin_popcountl((x))
#   endif
#   if (UINT_WIDTH >= 64)
#      define uint64_count_ones(x)     __builtin_popcount((x))
#   elif (ULONG_WIDTH >= 64)
#      define uint64_count_ones(x)     __builtin_popcountl((x))
#   else
#      define uint64_count_ones(x)     __builtin_popcountll((x))
#   endif
#elif defined(_MSC_VER) && defined(_WIN64)
#   static inline int   byte_count_ones(unsigned x)   { return __popcnt16(x); }
#   static inline int uint16_count_ones(unsigned x)   { return __popcnt16(x); }
#   define            uint32_count_ones(uint32_t x)   __popcnt(x);
#   static inline int uint64_count_ones(uint64_t x)   { return __popcnt64(x); }
#elif defined __STDC_VERSION_STDBIT_H__
#   define      byte_count_ones(x)     stdc_count_ones((unsigned)(x))
#   define    uint16_count_ones(x)     stdc_count_ones((unsigned)(x))
#   define    uint32_count_ones(x)     stdc_count_ones((uint32_t)(x))
#   define    uint64_count_ones(x)     stdc_count_ones((uint64_t)(x))
#else
    // Adapted and Based on from "Bit Twiddling Hacks" by Sean Eron Anderson
    // https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
    static inline int
    byte_count_ones(unsigned v)
    {
       v -= (v >> 1) & 0x55u);
       v  = (v & 0x33u) + ((v >> 2) & 0x33u);
       v  = (v + (v >> 4)) & 0x0Fu;
       return v;
    }
    static inline int
    uint16_count_ones(unsigned v)
    {
       v -= (v >> 1) & 0x5555u);
       v  = (v & 0x3333u) + ((v >> 2) & 0x3333u);
       v  = (v + (v >> 4)) & 0x0F0Fu;
       return (v * 0x0101u) >> 8;
    }
    static inline int
    uint32_count_ones(uint32_t v)
    {
       v -= (v >> 1) & UINT32_C(0x55555555));
       v  = (v & UINT32_C(0x33333333)) + ((v >> 2) & UINT32_C(0x33333333));
       v  = (v + (v >> 4)) & UINT32_C(0x0F0F0F0F);
       return (v * UINT32_C(0x01010101)) >> 24;

    }
    static inline int
    uint64_count_ones(uint64_t v)
    {
       v -= (v >> 1) & UINT64_C(0x5555555555555555);
       v  = (v & UINT64_C(0x3333333333333333)) + ((v >> 2) & UINT64_C(0x3333333333333333));
       v  = (v + (v >> 4)) & UINT64_C(0x0f0f0f0f0f0f0f0f);
       return (v * UINT64_C(0x0101010101010101)) >> 56;
    }
#endif


// int <T>_count_zeros(T x);
// return the number of 0-bits in the value x
#define   byte_count_zeros(x)          ( 8 -   byte_count_ones((x)))
#define uint16_count_zeros(x)          (16 - uint16_count_ones((x)))
#define uint32_count_zeros(x)          (32 - uint32_count_ones((x)))
#define uint64_count_zeros(x)          (64 - uint64_count_ones((x)))

//===</ functions/macros that are (will be) similar to C23's <stdbit.h> ========



//==============================================================================

//bswap   : swap bits  /  reverse : reverse every bit
//GCC:
//#define uint32_bswap(x)          __builtin_bswap32(x)
//#define uint64_bswap(x)          __builtin_bswap64(x)


// int <T>_hamming_distance(T x, T y)
// return the hamming distance between x and y
// the "hamming distance" of two bit sequences counts positions at which the
// bits differ, ie: the min. number of substitutions required to change one
// sequence into the other. It can provide a way to tell "how similar" two
// sequences of bytes are.
#define   byte_hamming_distance(x, y)    byte_count_ones((x) ^ (y))
#define uint16_hamming_distance(x, y)  uint16_count_ones((x) ^ (y))
#define uint32_hamming_distance(x, y)  uint32_count_ones((x) ^ (y))
#define uint64_hamming_distance(x, y)  uint64_count_ones((x) ^ (y))

//==============================================================================

#endif //KONPU_BITS_H
