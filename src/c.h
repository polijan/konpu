// - Include "config.h" and "platform.h" headers
// - Check for standard C11 (or later) compliance
// - Check for "common" assumptions that Konpu relies upon
//   Hint: `grep` Konpu's sources for the word "ASSUMEs" to list them all.
// - Include the following "free-standing" C headers:
//   <float.h>, <limits.h>, <stdarg.h>, <stddef.h>, <stdint.h>
// - Define "polyfills" (or alternatives) to bring many C23 functionalities:
//   * unreachable() macro
//   * static_assert(...) keyword (with one or two argument(s))
//   * alignas/alignof keywords (<stdalign.h> in C11/17)
//   * bool/true/false type/keywords (<stdbool.h> in C11/17)
//   * most functionalities from C23's <stdbit.h> header (TODO)
//   * UTIL_VA_OPT_COMMA(...) as a functional equivalent to __VA_OPT__(,)
//   * misc...
// - Include functionalities similar to other (non-freestanding) C headers
//   * assert(...) similar to <assert.h>
//   * some of the mem* functions similar to <string.h>
// - Include common extensions:
//   * macros UTIL_OVERLOAD for variadic overloading based on number of args.
//   * C_HINT_* to give more context to the compiler (usually to give it the
//     possibility to do more optimization or provide warnings).

#ifndef  KONPU_C_H
#define  KONPU_C_H

//TODO:
// - maybe C23 standard attribute are C_ATTRIBUTE_* and others attributes
//   (or similar things) should be C_HINT_*
// - maybe have C_HINT_GCC_PURE, C_HINT_GCC_... for GCC attributes (const, pure, ...)
// - is `ssizeof` name OK? Or should it be C_SSIZEOF() / c_ssizeof



/* Konpu ASSUMEs C11 or later */
#if !defined(__STDC_VERSION__) || (__STDC_VERSION__ < 201112L)
#   error "Konpu requires a compiler which conforms to C11 (or later) standard."
#endif

// Config and platform includes
#include "config.h"
#include "platform.h"

// Control Test Macros to add/prevent extras when using various C libraries
// TODO: actually, it feels like stuff that should be defined from the Makefile
#ifndef NDEBUG
#   define _FORTIFY_SOURCE  3
    // ^ in GNU's C library, adds security checks to various library functions.
    //   (may have a performance overhead)
#endif
// For GNU C library, prevent too much stuff, see:
// https://www.gnu.org/software/libc/manual/html_node/Feature-Test-Macros.html
#undef _GNU_SOURCE
#undef _DEFAULT_SOURCE
#undef _ATFILE_SOURCE
#undef _DYNAMIC_STACK_SIZE_SOURCE
// Prevent inclusion of POSIX C extra functions, outside of Konpu implementation
// #undef _POSIX_SOURCE
// #undef _POSIX_C_SOURCE
// #undef _XOPEN_SOURCE
// #undef _XOPEN_SOURCE_EXTENDED

// Includes for compiler builtins/intrinsics --> TODO: is this needed?
//#if defined(_MSC_VER)   // Microsoft Visual C++
//#   include <intrin.h>  // list of intrinsics: https://learn.microsoft.com/en-us/cpp/intrinsics/compiler-intrinsics?view=msvc-170
//#endif

// Free-standing header: variable arguments
#include <stdarg.h>

// Free-standing header: limits of floating-point types
#include <float.h>

// Free-standing header: range of integer types
#include <limits.h>
#if CHAR_BIT != 8  // Konpu ASSUMEs 8-bits bytes (and so do POSIX and Windows)
#   error "bytes do not have an 8-bits width"
#endif
// Return the number of bits for an integer of the form: 2^n - 1 (with n < 2040)
#define UTIL_COUNT_BITS_OF_2N1_(x) \
   ((x)/((x)%255+1) / 255%255*8 + 7-86/((x)%255+12))
   // Used to pass the MAX of an integer type and get that type's width as
   // a macro constant. This trick is from:
   // https://stackoverflow.com/questions/3957252/is-there-any-way-to-compute-the-width-of-an-integer-type-at-compile-time
   // The credit for the macros goes to Hallvard B. Furuseth who had posted them
   // (under the name IMAX_BITS) in the comp.lang.c Usenet newsgroup,
   // see: https://groups.google.com/g/comp.lang.c/c/NfedEFBFJ0k
   // and: https://groups.google.com/group/comp.lang.c/msg/e998153ef07ff04b
   // also, see explanation at: https://web.archive.org/web/20150403064546/http://coding.derkeiler.com/Archive/C_CPP/comp.lang.c/2009-01/msg02242.html
// "Polyfill" definitions for C prior to C23:
#if __STDC_VERSION__ <= 201710L
#   define CHAR_WIDTH          CHAR_BIT
#   define UCHAR_WIDTH         CHAR_BIT
#   define SCHAR_WIDTH         CHAR_BIT
#   define USHRT_WIDTH         UTIL_COUNT_BITS_OF_2N1_(USHRT_MAX)
#   define UINT_WIDTH          UTIL_COUNT_BITS_OF_2N1_(UINT_MAX)
#   define ULONG_WIDTH         UTIL_COUNT_BITS_OF_2N1_(ULONG_MAX)
#   define ULLONG_WIDTH        UTIL_COUNT_BITS_OF_2N1_(ULLONG_MAX)
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
#if KONPU_PLATFORM_LIBC && (!defined(NDEBUG))
    // In debug mode and have the "real" assert from C, then
    // then (re)define C's unreachable() as `assert(0)` (even for C23).
#   undef  unreachable
#   define unreachable()  assert(0)
#elif __STDC_VERSION__ <= 201710L
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


//------------------------------------------------------------------------------
// Variadic macros and Overloading
//------------------------------------------------------------------------------
// Utilities using variadic macros (see also "util.h").
// The most important ones here are:
// - UTIL_OVERLOAD(function, ...), which helps to support a form of function or
//   macro "overloading" based on the number of parameters.
// - UTIL_VA_OPT_COMMA(__VA_ARGS__), which is the same as __VA_OPT__(,)
//   without requiring C23.
//------------------------------------------------------------------------------

// UTIL_STRINGIFY: Trigger macro expansion and turn into a string constant.
#define UTIL_STRINGIFY(x)          UTIL_STRINGIFY_(x)
   #define UTIL_STRINGIFY_(x)      #x

// UTIL_CAT: Trigger macro expansion and concatenate its parameters.
//           It is useful for creating other macros.
#define UTIL_CAT(item, ...)        UTIL_CAT_(item, __VA_ARGS__)
   #define UTIL_CAT_(item, ...)    item##__VA_ARGS__

// UTIL_IF: Expands differently depending on the condition.
// Note: the condition should expand to exactly the token 0 or 1, nothing else.
#define UTIL_IF(condition_0_or_1, case_when_1, case_when_0) \
   UTIL_CAT(UTIL_CAT(UTIL_IF_, condition_0_or_1), _)(case_when_1, case_when_0)
#  define UTIL_IF_0_(case_when_1, case_when_0)  case_when_0
#  define UTIL_IF_1_(case_when_1, case_when_0)  case_when_1

// UTIL_HAS_COMMA: Expands to 1 if there is comma in the arguments, 0 otherwise.
// (might be useful on its own, but is mostly used as helper for UTIL_IS_EMPTY)
#define UTIL_HAS_COMMA(...)         \
   UTIL_ARG_AT_50_(__VA_ARGS__,     \
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
      1, 1, 1, 1, 1, 1, 1, 1, 1, 0, UTIL_HAS_COMMA__TOO_MANY_ARGUMENTS_)

// UTIL_IS_EMPTY: expands to 0 if the macro has no arguments, 1 otherwise.
#define UTIL_IS_EMPTY(...)                                                     \
   /* Macro adapted from P99(https://gitlab.inria.fr/gustedt/p99), MIT License \
    * A great explanation of how this works can be found here:                 \
    * https://gustedt.wordpress.com/2010/06/08/detect-empty-macro-arguments */ \
   UTIL_IS_EMPTY_(                                                             \
      /* test if there is just one argument, eventually an empty one */        \
      UTIL_HAS_COMMA(__VA_ARGS__),                                             \
      /* test if TRIGGER_PARENTHESIS together with the argument adds a comma */\
      UTIL_HAS_COMMA(UTIL_TRIGGER_PARENTHESIS_ __VA_ARGS__),                   \
      /* test if the argument together with a parenthesis adds a comma */      \
      UTIL_HAS_COMMA(__VA_ARGS__ (/*empty*/)),                                 \
      /* test if placing it between TRIGGER_PARENTHESIS and the                \
         parenthesis adds a comma */                                           \
      UTIL_HAS_COMMA(UTIL_TRIGGER_PARENTHESIS_ __VA_ARGS__ (/*empty*/))        )
#  define UTIL_TRIGGER_PARENTHESIS_(...) ,
#  define UTIL_PASTE5_(_0, _1, _2, _3, _4) _0 ## _1 ## _2 ## _3 ## _4 ## _
#  define UTIL_IS_EMPTY_CASE_0001_ ,
#  define UTIL_IS_EMPTY_(_0, _1, _2, _3) \
      UTIL_HAS_COMMA(UTIL_PASTE5_(UTIL_IS_EMPTY_CASE_, _0, _1, _2, _3))

// UTIL_COUNT_ARGUMENTS: Expands to the number of its arguments (works up to 50)
#define  UTIL_COUNT_ARGUMENTS(...)                 \
   UTIL_IF(UTIL_IS_EMPTY(__VA_ARGS__),             \
      UTIL_COUNT_ARGUMENTS_ZERO_,    /* 0 */       \
      UTIL_COUNT_ARGUMENTS_NONZERO_  /*see below*/ \
   )(__VA_ARGS__)
#  define UTIL_COUNT_ARGUMENTS_ZERO_()    0
   // This is what I used before, but it also outputs 1 in case of zero args, so
   // it's now supplemented by an IS_EMPTY utility borrowed from P99 (see below)
#  define UTIL_COUNT_ARGUMENTS_NONZERO_(...)                                   \
      UTIL_ARG_AT_50_(__VA_ARGS__,    50, 49, 48, 47, 46, 45, 44, 43, 42, 41,  \
      40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23,  \
      22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  \
      4 ,  3,  2,  1,  0, UTIL_COUNT_ARGUMENTS__TOO_MANY_ARGUMENTS_)
#     define UTIL_ARG_AT_50_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, \
         n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23, n24, \
         n25, n26, n27, n28, n29, n30, n31, n32, n33, n34, n35, n36, n37, n38, \
         n39, n40, n41, n42, n43, n44, n45, n46, n47, n48, n49, n50, n, ...) n

// UTIL_OVERLOAD: macro to perform "variadic overloading"
// Here's an example how one might use it:
// >   #define MyFunction(...)  UTIL_OVERLOAD(MyFunction, __VA_ARGS__)
// >   int MyFunction_2_(int a, int b);        // Function with two parameters
// >   int MyFunction_3_(int a, int b, int c); // Function with three parameters
// This allows the following:
// >   MyFunction(10, 20);      // Expands to: MyFunction_2_(10, 20);
// >   MyFunction(10, 20, 30);  // Expands to: MyFunction_3_(10, 20, 30);
#define UTIL_OVERLOAD(function, ...)                                     \
   UTIL_CAT(UTIL_CAT(function##_, UTIL_COUNT_ARGUMENTS(__VA_ARGS__)), _) \
   (__VA_ARGS__)

// UTIL_VA_OPT_COMMA(__VA_ARGS__): Expands to nothing if there's no arguments,
// otherwise, expands to a comma. It serves as the equivalent of __VA_OPT__(,)
// without requiring C23... or maybe it still can't be used well, keep reading:
// IMPORTANT: Up to C17, the "..." in a variadic macro theoretically requires at
// least one argument. A very strict compiler might not let __VA_ARGS__ be empty
// (example: `-pedantic` gives a warning in GCC), thus perhaps limiting the
// usage of this macro.
#define UTIL_VA_OPT_COMMA(...) \
   UTIL_CAT(UTIL_CAT(UTIL_VA_OPT_COMMA_, UTIL_IS_EMPTY(__VA_ARGS__)), _)
#  define UTIL_VA_OPT_COMMA_0_ ,
#  define UTIL_VA_OPT_COMMA_1_


//------------------------------------------------------------------------------

// void C_HINT_ASSUME(condition...); function-like macro
// The boolean condition passed to this macro is defined to be true.
// If the condition is violated during execution, the behavior is undefined.
// The condition itself is never evaluated (contrarily to C's `assert`)
// This macro is meant to provide additional information about the state of the
// program to the compiler which then might be able to make optimizations.
//
// Implementation inspired by: https://stackoverflow.com/questions/63493968/reproducing-clangs-builtin-assume-for-gcc
//#if defined(__has_cpp_attribute) && (__has_cpp_attribute(assume) >= 202207L)
//      // C++23 has [[assume]] attribute
//#     define C_HINT_ASSUME(...) [[assume(__VA_ARGS__)]]
#if defined(__clang__)
#     define C_HINT_ASSUME(...)   do { __builtin_assume(__VA_ARGS__); } while(0)
#elif defined(_MSC_VER)
#     define C_HINT_ASSUME(...)   do { __assume(__VA_ARGS__); } while(0)
#elif defined(__GNUC__) && (__GNUC__ >= 13)
#     define C_HINT_ASSUME(...)   __attribute__((__assume__(__VA_ARGS__)))
#else
#     define C_HINT_ASSUME(...)
#endif

// The `assert(...)` macro (similar to <assert.h>)
// The macro (may) evaluate its arguments and terminates the program if the
// evaluation is zero.
#ifdef NDEBUG
    // In release build, turn assertions in ASSUMEption HINTs, so that arguments
    // never get evaluated, and may be used for potential further optimization.
    // This requires assert() statements to be free from side effects, which
    // is not force by C but is supposed to be that way (otherwise assertions
    // could never be disabled)
#   define assert(...)      C_HINT_ASSUME(__VA_ARGS__)
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
#          define STATIC_ASSERT_2_(condition, string_lit)  \
                  _Static_assert((condition), string_lit)
#          define STATIC_ASSERT_1_(condition) \
                  _Static_assert((condition), UTIL_STRINGIFY(__LINE__))
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

// We provide macros for C23 standard attributes and try to provide fallback
// for C11/C17
#if __STDC_VERSION__ >= 202311L
#   define C_ATTRIBUTE_DEPRECATED        [[deprecated]]
#   define C_ATTRIBUTE_FALLTHROUGH       [[fallthrough]]
#   define C_ATTRIBUTE_NODISCARD         [[nodiscard]]
#   define C_ATTRIBUTE_MAYBE_UNUSED      [[maybe_unused]]
#   define C_ATTRIBUTE_NORETURN          [[noreturn]]
#   define C_ATTRIBUTE_UNSEQUENCED       [[unsequenced]]
#   define C_ATTRIBUTE_REPRODUCIBLE      [[reproducible]]
#else
#   define C_ATTRIBUTE_NORETURN          _Noreturn
#   if defined(__GNUC__)
       // [[nodiscard]]: ‘warn_unused_result’ attribute is similar but only
       // applies to functions, where as nodiscard can also apply to structs,
       // unions, enums.
       //
       // https://www.open-std.org/jtc1/sc22/wg14/www/docs/n2956.htm#some-differences-with-gcc-const-and-pure
       // - `pure` is more relaxed than [[reproducible]], so we can use it,
       // - but `const` is more strict than [[unsequenced]], so we can't use.
#      define C_ATTRIBUTE_DEPRECATED     __attribute__((deprecated))
#      define C_ATTRIBUTE_FALLTHROUGH    __attribute__((fallthrough))
#      define C_ATTRIBUTE_MAYBE_UNUSED   __attribute__((unused))
#      define C_ATTRIBUTE_REPRODUCIBLE   __attribute__((pure))
#   else
#      define C_ATTRIBUTE_DEPRECATED
#      define C_ATTRIBUTE_FALLTHROUGH
#      define C_ATTRIBUTE_MAYBE_UNUSED
#      define C_ATTRIBUTE_REPRODUCIBLE
#   endif
#   define C_ATTRIBUTE_NODISCARD
#ifndef    C_ATTRIBUTE_UNSEQUENCED
           // [[unsequenced]] implies [[reproducible]], thus:
#   define C_ATTRIBUTE_UNSEQUENCED   C_ATTRIBUTE_REPRODUCIBLE
#endif
#endif

// C_HINT_ALWAYS_INLINE
// When used instead of the keyword `ìnline`, force (or strongly suggest) to the
// compiler to inline a function.
// Note: typically you should just use `inline` (if you think it's worth it) and
// trust the compiler to do the right thing. Abusing ALWAYS_INLINE can result in
// code bloat and no performance gain.
#if defined(__GNUC__)
#   define C_HINT_ALWAYS_INLINE    __attribute__((always_inline))
#elif defined(_MSC_VER)
#   define C_HINT_ALWAYS_INLINE    __forceinline
#else
#   define C_HINT_ALWAYS_INLINE    inline
#endif

// C_HINT_LIKELY(condition) / C_HINT_UNLIKELY(condition)
// likely and unlikely are not defined in C, but are used in many projects as
// optimization hints (to tell the compiler to favour the "likely" side of a
// branch). Note that [[likely]] and [[unlikely]] are defined as attributes in
// C++20 (perhaps it will come in some future C version)
#if defined(__GNUC__)
#   define C_HINT_LIKELY(...)      (__builtin_expect((__VA_ARGS__), 1))
#   define C_HINT_UNLIKELY(...)    (__builtin_expect((__VA_ARGS__), 0))
#else
#   define C_HINT_LIKELY(...)      (__VA_ARGS__)
#   define C_HINT_UNLIKELY(...)    (__VA_ARGS__)
#endif

// C23 <stdbit.h>-like functionalities:
//
// The following macros/functions are available in Konpu C for
// uint<8,16,32,64>_t (even when Konpu is compiled with C11 or C17):
// stdc_bit_ceil, stdc_bit_floor, stdc_bit_width, stdc_count_ones,
// stdc_count_zeros, stdc_first_leading_one, stdc_first_leading_zero,
// stdc_first_trailing_one, stdc_first_trailing_zero, stdc_has_single_bit,
// stdc_leading_ones, stdc_leading_zeros, stdc_trailing_ones,
// stdc_trailing_zeros.
//
// The following macro constants are also available:
// __STDC_ENDIAN_BIG__, __STDC_ENDIAN_LITTLE__, __STDC_ENDIAN_NATIVE__
//
#if __STDC_VERSION__ > 201710L
#   include <stdbit.h>
#else
    // TODO: define stdc_bit_ceil, stdc_bit_floor, stdc_bit_width,
    //       stdc_count_ones, stdc_count_zeros, stdc_first_leading_one,
    //       stdc_first_leading_zero, stdc_first_trailing_one,
    //       stdc_first_trailing_zero, stdc_has_single_bit, stdc_leading_ones,
    //       stdc_leading_zeros, stdc_trailing_ones, stdc_trailing_zeros.
#endif
#ifndef __STDC_ENDIAN_NATIVE__
#   if defined(__GNUC__)
#      define __STDC_ENDIAN_BIG__          __ORDER_BIG_ENDIAN__
#      define __STDC_ENDIAN_LITTLE__       __ORDER_LITTLE_ENDIAN__
#      define __STDC_ENDIAN_NATIVE__       __BYTE_ORDER__
#   else
// TODO/FIXME: yeah, defining -DKONPU_ENDIAN_ for the Makefile won't work
//             when we generate include/konpu.h.
#      define __STDC_ENDIAN_BIG__          4321
#      define __STDC_ENDIAN_LITTLE__       1234
#      ifdef KONPU_ENDIAN_
#         define __STDC_ENDIAN_NATIVE__    KONPU_ENDIAN_
#      else
#         define __STDC_ENDIAN_NATIVE__    0  // unknown
#         error "For C prior to C23 without gcc/clang compiler, you need to define KONPU_ENDIAN_ from your build system (see tools/endian)."
#      endif
#   endif
#endif


// a macro to use like the `sizeof` operator but returning a *signed* size (as a
// int32_t) ... because it is good to use signed types for indexing and length.
#define ssizeof(type_or_expression)    ((int32_t)sizeof(type_or_expression))
// Note: a int32_t can fit a size up to 2Gb, anything more (or even close to
// that would be totally unreasonnable for Konpu).

// Memory functions from <string.h>. Making sure we have those:
extern void *memset(void *dst, int ch, size_t count);
extern void *memcpy(void *restrict dst, const void *restrict src, size_t count);
// maybe memmove too?
// extern void *memmove(void *dst, const void *src, size_t count);

/*
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
*/

#endif //KONPU_C_H
