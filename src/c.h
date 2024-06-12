// - Include "config.h" and "platform.h" headers
// - Check for standard C11 (or later) compliance
// - Check for "common" assumptions that Konpu relies upon
//   Hint: `grep` Konpu's sources for the word "ASSUMEs" to list them all.
// - Include the following "free-standing" C headers:
//   <float.h>, <limits.h>, <stdarg.h>, <stddef.h>, <stdint.h>
// - When possible, define "polyfills" to bring C23 functionality to C11/C17
//   * unreachable()
//   * static_assert(...)
//   * C11/C17: include the free-standing <stdalign.h> and <stdbool.h>
//   * misc...
// - Include functionalities similar to other (non-freestanding) C headers
//   * assert(...) similar to <assert.h>
//   * some of the mem* functions similar to <string.h>
// - Include common extensions:
//   * macros UTIL_OVERLOAD for variadic overloading based on number of args.
//   * ASSUME(...)/likely(...)/unlikely(...) macros to give optimization hints

#ifndef  KONPU_C_H
#define  KONPU_C_H

/* Konpu ASSUMEs C11 or later */
#if !defined(__STDC_VERSION__) || (__STDC_VERSION__ < 201112L)
#   error "Konpu requires a compiler which conforms to C11 (or later) standard."
#endif

// Config and platform includes
#include "config.h"
#include "platform.h"

// Includes for compiler builtins/intrinsics
#if defined(_MSC_VER)   // Microsoft Visual C++
#   include <intrin.h>  // list of intrinsics: https://learn.microsoft.com/en-us/cpp/intrinsics/compiler-intrinsics?view=msvc-170
#endif

// Free-standing header: variable arguments
#include <stdarg.h>

// Free-standing header: limits of floating-point types
#include <float.h>

// Free-standing header: range of integer types
#include <limits.h>
#if CHAR_BIT != 8  // Konpu ASSUMEs 8-bits bytes (and so do POSIX and Windows)
#   error "bytes do not have an 8-bits width"
#endif
// Expands to the number of bits for a number 2^n - 1 (n < 2040)
#define UINT_COUNT_WIDTH_OF_MAX(m) \
        ((m)/((m)%255+1) / 255%255*8 + 7-86/((m)%255+12))
        // Used to pass the MAX of an integer type and get that type's width as
        // a macro constant. This trick is from:
        // https://stackoverflow.com/questions/3957252/is-there-any-way-to-compute-the-width-of-an-integer-type-at-compile-time
        // The credit for the macros goes to Hallvard B. Furuseth who had posted
        // them (under the name IMAX_BITS) in the comp.lang.c Usenet newsgroup,
        // see: https://groups.google.com/g/comp.lang.c/c/NfedEFBFJ0k
        // and: https://groups.google.com/group/comp.lang.c/msg/e998153ef07ff04b
        // also, see explanation at: https://web.archive.org/web/20150403064546/http://coding.derkeiler.com/Archive/C_CPP/comp.lang.c/2009-01/msg02242.html
// "Polyfill" definitions for C prior to C23:
#if __STDC_VERSION__ <= 201710L
#   define CHAR_WIDTH          CHAR_BIT
#   define UCHAR_WIDTH         CHAR_BIT
#   define SCHAR_WIDTH         CHAR_BIT
#   define USHRT_WIDTH         UINT_COUNT_WIDTH_OF_MAX(USHRT_MAX)
#   define UINT_WIDTH          UINT_COUNT_WIDTH_OF_MAX(UINT_MAX)
#   define ULONG_WIDTH         UINT_COUNT_WIDTH_OF_MAX(ULONG_MAX)
#   define ULLONG_WIDTH        UINT_COUNT_WIDTH_OF_MAX(ULLONG_MAX)
#   define SHRT_WIDTH          USHRT_WIDTH
#   define INT_WIDTH           UINT_WIDTH
#   define LONG_WIDTH          ULONG_WIDTH
#   define LLONG_WIDTH         ULLONG_WIDTH
#endif

// C11/C17-specific free-standing headers (deprecated by C23)
#if __STDC_VERSION__ <= 201710L
#   include <stdalign.h>  // alignas/alignof (keywords in C23)
#   include <stdbool.h>   // macros for bool/true/false (keywords in C23)
//  Note: we don't include <stdnoreturn.h> (noreturn is an attribute in C23)
#endif

// Free-standing header: common macro definitions
#include <stddef.h>
#if __STDC_VERSION__ <= 201710L
    // Define C's unreachable() macro prior to C23
    // Reminder: unreachable() is a macro resulting in Undefined Behaviour if
    // executed. Its purpose is to be used as a hint for compiler optimization.
    //
    // For example, if we know *for sure*, 3 < val <= 0, we can do:
    // switch (val) {
    //   case  0: ...  break;
    //   case  1: ...  break;
    //   case  2: ...  break;
    //   default: unreachable();
    // }
#   if defined(__GNUC__)
#      define unreachable()   (__builtin_unreachable())
#   elif defined(_MSC_VER)
#      define unreachable()   (__assume(false))
#   else
#      define unreachable()   ((void)0)
#   endif
#endif


//--- Variadic macros and Overloading ------------------------------------------
// Utilities using variadic macros (see also "util.h"). The most important one
// here is UTIL_OVERLOAD, which aims to support macros/functions "overloading"
// based on the number of parameters.

// UTIL_STRINGIFY: Trigger macro expansion and turn into a string constant.
#define UTIL_STRINGIFY(x)               UTIL_STRINGIFY_(x)
        #define UTIL_STRINGIFY_(x)      #x

// UTIL_CAT: Trigger macro expansion and concatenate its parameters.
//           It is useful for creating other macros.
#define UTIL_CAT(item, ...)             UTIL_CAT_(item, __VA_ARGS__)
        #define UTIL_CAT_(item, ...)    item##__VA_ARGS__

// UTIL_ARG_COUNT: expands to the number of its arguments (works up to 50 args)
#define UTIL_ARG_COUNT(...)                                                   \
        UTIL_ARG_AT_50_(__VA_ARGS__,    50, 49, 48, 47, 46, 45, 44, 43, 42,   \
        41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25,   \
        24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10,  9,  8,   \
        7,   6,  5,  4,  3,  2,  1,  0, UTIL_ARG_COUNT__TOO_MANY_ARGS)
#       define UTIL_ARG_AT_50_(  n01, n02, n03, n04, n05, n06, n07, n08, n09, \
        n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23, \
        n24, n25, n26, n27, n28, n29, n30, n31, n32, n33, n34, n35, n36, n37, \
        n38, n39, n40, n41, n42, n43, n44, n45, n46, n47, n48, n49, n50, n,   \
        ...)  n

// UTIL_OVERLOAD: macro to perform "variadic overloading"
// Here's an example how to use it:
// >   #define MyFunction(...)  UTIL_OVERLOAD(MyFunction, __VA_ARGS__)
// >   int MyFunction_2_(int a, int b);        // function with two parameters
// >   int MyFunction_3_(int a, int b, int c); // function with three parameters
// This allows the following:
// >   MyFunction(10,20);    // expand to MyFunction_2_(10,20)
// >   MyFunction(10,20,30); // expand to MyFunction_3_(10,20,30)
#define UTIL_OVERLOAD(function, ...) \
        UTIL_CAT(UTIL_CAT(function##_, UTIL_ARG_COUNT(__VA_ARGS__)), _)(__VA_ARGS__)
//------------------------------------------------------------------------------


// void ASSUME(condition...); function-like macro
// The boolean condition passed to this macro is defined to be true.
// If the condition is violated during execution, the behavior is undefined.
// The condition itself is never evaluated (contrarily to C's `assert`)
// This macro is meant to provide additional information about the state of the
// program to the compiler which then might be able to make optimizations.
//
// Implementation inspired by: https://stackoverflow.com/questions/63493968/reproducing-clangs-builtin-assume-for-gcc
#ifdef __has_cpp_attribute  // C++23 has [[assume]] attribute
#     if __has_cpp_attribute(assume) >= 202207L
#        define ASSUME(...) [[assume(__VA_ARGS__)]]
#     endif
#elif defined(__clang__)
#     define ASSUME(...)    do { __builtin_assume(__VA_ARGS__); } while(0)
#elif defined(_MSC_VER)
#     define ASSUME(...)    do { __assume(__VA_ARGS__); } while(0)
#elif defined(__GNUC__)
#     if __GNUC__ >= 13
#        define ASSUME(...) __attribute__((__assume__(__VA_ARGS__)))
#     endif
#else
#     define ASSUME(...)
#endif

// The `assert(...)` macro (similar to <assert.h>)
// The macro (may) evaluate its arguments and terminates the program if the
// evaluation is zero.
#ifdef NDEBUG
    // In release mode, turn assertions in ASSUMEptions, so that arguments
    // never get evaluated, and may be used for potential further optimization.
#   define assert(...)      ASSUME(__VA_ARGS__)
#elif KONPU_PLATFORM_LIBC
#   include <assert.h>      // libc's assert
#elif defined(__GNUC__)     // with no platform, try to stop the execution
#   define assert(...)      if (!(__VA_ARGS__))  __builtin_trap()
#elif _MSC_VER
#   define assert(...)      if (!(__VA_ARGS__))  __debugbreak()
#else
#   define assert(...)      ((void)0)
    // C cannot portably stop the program execution, although one might wish
    // to trigger a fault to do that in practice, for example:
    // #define assert(...)  if (!(__VA_ARGS__))  *(volatile int *)0 = 0
#endif

// The `static_assert(condition, [message])` keyword/macro
#if __STDC_VERSION__ <= 201710L
    // C23: nothing to do (`static_assert` is a C keyword)
    // C11 and C17 (and gcc) have _Static_assert(expression, message)
#   undef  static_assert // <-- undef as this has been defined by <assert.h>
#   define static_assert(...)   UTIL_OVERLOAD(STATIC_ASSERT, __VA_ARGS__)
#          define STATIC_ASSERT_2_(condition, msg)  _Static_assert(condition, msg)
#          define STATIC_ASSERT_1_(condition)       _Static_assert(condition, UTIL_STRINGIFY(__LINE__))
#endif


// Free-standing header: fixed-width integer types
#include  <stdint.h> 
// Konpu ASSUMEs the existence of (u)int8_t, (u)int16_t, (u)int32_t, (u)int64_t
static_assert(sizeof( int_least8_t ) == 1);
static_assert(sizeof(uint_least8_t ) == 1);
static_assert(sizeof( int_least16_t) == 2);
static_assert(sizeof(uint_least16_t) == 2);
static_assert(sizeof( int_least32_t) == 4);
static_assert(sizeof(uint_least32_t) == 4);
static_assert(sizeof( int_least64_t) == 8);
static_assert(sizeof(uint_least64_t) == 8);
// "Polyfill" definitions for C prior to C23
#if __STDC_VERSION__ <= 201710L
#   define INT8_WIDTH            8
#   define INT16_WIDTH          16
#   define INT32_WIDTH          32
#   define INT64_WIDTH          64
#   define INT_LEAST8_WIDTH      8
#   define INT_LEAST16_WIDTH    16
#   define INT_LEAST32_WIDTH    32
#   define INT_LEAST64_WIDTH    64
#   define UINT8_WIDTH           8
#   define UINT16_WIDTH         16
#   define UINT32_WIDTH         32
#   define UINT64_WIDTH         64
#   define UINT_LEAST8_WIDTH     8
#   define UINT_LEAST16_WIDTH   16
#   define UINT_LEAST32_WIDTH   32
#   define UINT_LEAST64_WIDTH   64
#endif

// likely(condition) / unlikely(condition)
//
// likely and unlikely are not defined in C, but are used in many projects as
// optimization hints (to tell the compiler to favour the "likely" side of a
// branch).
// Note that [[likely]] and [[unlikely]] are defined as attributes in C++20.
#if defined(__GNUC__)
#   define likely(...)          (__builtin_expect((__VA_ARGS__), 1))
#   define unlikely(...)        (__builtin_expect((__VA_ARGS__), 0))
#else
#   define likely(...)          (__VA_ARGS__)
#   define unlikely(...)        (__VA_ARGS__)
#endif


// Other functions from the standard C library. Making sure we have those:

// memset, memcpy (as in <string.h>)
#ifdef __GNUC__
   // GCC includes built-in versions of many of the functions in the standard C
   // library. Many of these are only optimized in certain cases; if they aren't
   // optimized in a particular case, a call to the library function is emitted.
#  define memset(dest, byte, count)   __builtin_memset(dest, byte, count)
#  define memcpy(dest, src, count)    __builtin_memcpy(dest, src, count)
#elif KONPU_PLATFORM_LIBC
   extern void *memset(void *dest, int byte, size_t count);
   extern void *memcpy(void *restrict dest, const void *restrict src, size_t count);
#else // naive/sub-optimal implementation ...
   static inline void*
   memset(void *dest, int byte, size_t count) {
      char *d = dest;
      while (count--) *d++ = byte;
      return dest;
   }
   static inline void*
   memcpy(void *restrict dest, const void *restrict src, size_t count) {
      char *restrict d = dest;
      const char *restrict s = src;
      while (count--) *d++ = *s++;
      return dest;
   }
#endif

#endif //KONPU_C_H
