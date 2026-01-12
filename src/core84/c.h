/* First thing first: Konpu ASSUMEs C11 or later */
#ifndef KONPU_C_H_
#  if !defined(__STDC_VERSION__) || (__STDC_VERSION__ < 201112L)
#     error "Konpu requires a compiler conforming to C11 (or later) standard."
#  endif
#endif

// - Check for standard C11 (or later) compliance
// - Check for "common" assumptions that Konpu relies on
//   Hint: `grep` Konpu's sources for the word "ASSUMEs" to list them all.
// - Include "config.h"
// - Include the following "free-standing" C headers:
//   <float.h>, <limits.h>, <stdarg.h>, <stddef.h>, <stdint.h>
// - Define "polyfills" (or alternatives) to bring many C23 functionalities:
//   * unreachable() macro
//   * static_assert(...) keyword (with one or two argument(s))
//   * alignas/alignof keywords (<stdalign.h> in C11/17)
//   * bool/true/false type/keywords (<stdbool.h> in C11/17)
//   * bits functionalities like C23 (actually defined in the file "bits.h")
//     - macros like in the <stdbit.h> header
//     - the BITS(...) macro to write binary literals
//   * TODO(review)   UTIL_VA_OPT_COMMA(...) as a functional equivalent to __VA_OPT__(,)
//   * misc...
// - Declare (or even implement) some functionalities similar to
//   (non-freestanding) C headers:
//   * assert(...) similar to <assert.h>
//   * some of the mem* functions similar to <string.h>
// - Common extensions, such as:
//   * macros UTIL_OVERLOAD for variadic overloading based on number of args.
//   * C_HINT_* to give more context to the compiler (usually to give it the
//     possibility to do more optimization or provide warnings).

#ifndef  KONPU_C_H_
#define  KONPU_C_H_

// Check VLA support (specifically for C11/C17)
// C supports Variable Length Arrays (VLAs) starting from the C99 standard, but
// they are optional in C11 and mandatory again in C23. Therefore, whether VLAs
// are accepted depends on the specific C standard being used and the compiler's
// support for it.
// Note: Konpu uses variable-sized types to simplify its public API, but never
//       uses stack-based VLAs (as it is almost always a bad idea)
#ifdef __STDC_NO_VLA__
#  error "Konpu requires a C compiler with VLA support."
#endif

//TODO:
// - maybe C23 standard attribute are C_ATTRIBUTE_* and others attributes
//   (or similar things) should be C_HINT_*
// - maybe have C_HINT_GCC_PURE, C_HINT_GCC_... for GCC attributes (const, pure, ...)

// Config and platform includes
#include "config.h"
//#include "platform.h" <--- TODO:REMOVE
#define KONPU_PLATFORM_LIBC 1 //TODO: CHECK, REMOVE


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

//------------------------------------------------------------------------------
// FUNCTIONALITY NOT SUPPORTED (at least for now)
//------------------------------------------------------------------------------

// Non of the functionalities provided by the following headers are supported:
// <complex.h>  Complex number arithmetic (optional in C)
// <setjmp.h>   Nonlocal jumps
// <signal.h>   Signal handling
// <tgmath.h>   Type-generic math
// <wchar.h>    (C95)Extended multibyte and wide character utilities
// <wctype.h>   (C95)Functions to determine the type contained in wide character data


// Utility to help ban the usage of a function
// (eg. make a compile error if we try to use it)
#define C_BANNED(func)   KONPU_SHOULD_NOT_USE_THE_FUNCTION_##func##_


//------------------------------------------------------------------------------
// C (Variadic) OVERLOADING / Variadic macros utilities
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
#  define UTIL_STRINGIFY_(x)       #x

// UTIL_CAT: Trigger macro expansion and concatenate its parameters.
//           It is useful for creating other macros.
#define UTIL_CAT(item, ...)        UTIL_CAT_(item, __VA_ARGS__)
#  define UTIL_CAT_(item, ...)     item##__VA_ARGS__

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

// TODO: remove or review/improve or what?
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
// ATTRIBUTES AND HINTS: C_HINT_*
// Konpu provides C_HINT_* macros to tip the compiler into optimizing the code
// in certain ways or use implementation-defined features. It is similar to
// C (C23) attributes for which the next sections defines C_ATTRIBUTE_* macros.
//------------------------------------------------------------------------------

// C_HINT_ALWAYS_INLINE: when used *instead* of the keyword `inline`, it
// *STRONGLY* indicates (much more so than the standard `inline` keyword) to the
// compiler that we wish to inline a function.
// Note: typically you should just use `inline` (when you think it's worth it)
//       and trust the compiler to do the right thing. Abusing ALWAYS_INLINE may
//       result in code bloat and no performance gain.
#ifdef __GNUC__
#  define C_HINT_ALWAYS_INLINE    inline __attribute__((always_inline))
#elif defined(_MSC_VER)
#  define C_HINT_ALWAYS_INLINE    inline __forceinline
#else
#  define C_HINT_ALWAYS_INLINE    inline
#endif

// C_HINT_LIKELY(condition) / C_HINT_UNLIKELY(condition):
// Tell the compiler to optimize in order to favor the "likely" side of a
// branch. If the user prediction is wrong, this will be a performance penalty.
// Note: [[likely]] and [[unlikely]] are standard attributes in C++20.
//       (perhaps it will come in some future C version ?)
#ifdef __GNUC__
#  define C_HINT_LIKELY(...)      (__builtin_expect((__VA_ARGS__), 1))
#  define C_HINT_UNLIKELY(...)    (__builtin_expect((__VA_ARGS__), 0))
#else
#  define C_HINT_LIKELY(...)      (__VA_ARGS__)
#  define C_HINT_UNLIKELY(...)    (__VA_ARGS__)
#endif

// void C_HINT_ASSUME(condition...); function-like macro
// The boolean condition passed to this macro is assume to be true.
// If the condition is violated during execution, the behavior is undefined.
// The condition itself is never evaluated (contrarily to C's `assert`)
// This macro is meant to provide additional information about the state of the
// program to the compiler which then might be able to make optimizations.
// Note: [[assume]] is a standard attributes in C++23
//       (perhaps it will come in some future C version ?)
// Implementation inspired by:
// https://stackoverflow.com/questions/63493968/reproducing-clangs-builtin-assume-for-gcc
//#if defined(__has_cpp_attribute) && (__has_cpp_attribute(assume) >= 202207L)
//      // C++23 has [[assume]] attribute
//#     define C_HINT_ASSUME(...) [[assume(__VA_ARGS__)]]
#if defined(__clang__)
#  define C_HINT_ASSUME(...)   do { __builtin_assume(__VA_ARGS__); } while(0)
#elif defined(__GNUC__) && (__GNUC__ >= 13)
#  define C_HINT_ASSUME(...)   __attribute__((__assume__(__VA_ARGS__)))
#elif defined(_MSC_VER)
#  define C_HINT_ASSUME(...)   do { __assume(__VA_ARGS__); } while(0)
#else
#  define C_HINT_ASSUME(...)
#endif

//------------------------------------------------------------------------------
// ATTRIBUTES AND HINTS: C_ATTRIBUTE_*
//
// We provide C_ATTRIBUTE_* macros for all C standard attributes (from C23),
// so that in some case ti can also be used from C11/C17.
// (specifically for C_ATTRIBUTE_NO_RETURN, check the next section below)
//
// see: https://en.cppreference.com/w/c/language/attributes.html
//------------------------------------------------------------------------------
#if __STDC_VERSION__ >= 202311L
#  define C_ATTRIBUTE_DEPRECATED     [[deprecated]]
#  define C_ATTRIBUTE_FALLTHROUGH    [[fallthrough]]
#  define C_ATTRIBUTE_NODISCARD      [[nodiscard]]
#  define C_ATTRIBUTE_MAYBE_UNUSED   [[maybe_unused]]
#  define C_ATTRIBUTE_REPRODUCIBLE   [[reproducible]]
#  define C_ATTRIBUTE_UNSEQUENCED    [[unsequenced]]
#elif defined(__GNUC__)
   //
   // https://www.open-std.org/jtc1/sc22/wg14/www/docs/n2956.htm#some-differences-with-gcc-const-and-pure
   // - `pure` is more relaxed than [[reproducible]], so we can use it,
   // - but `const` is more strict than [[unsequenced]], so we can't use.
#  define C_ATTRIBUTE_DEPRECATED     __attribute__((deprecated))
#  define C_ATTRIBUTE_FALLTHROUGH    __attribute__((fallthrough))
   // [[nodiscard]]: `warn_unused_result` attribute is similar but only
   // applies to functions, where as nodiscard can also apply to structs,
   // unions, enums.
#  define C_ATTRIBUTE_NODISCARD      __attribute__((warn_unused_result))
#  define C_ATTRIBUTE_MAYBE_UNUSED   __attribute__((unused))
#  define C_ATTRIBUTE_REPRODUCIBLE   __attribute__((pure))
   // [[unsequenced]] implies [[reproducible]], thus:
#  define C_ATTRIBUTE_UNSEQUENCED    C_ATTRIBUTE_REPRODUCIBLE
#else
#  define C_ATTRIBUTE_DEPRECATED
#  define C_ATTRIBUTE_FALLTHROUGH
#  define C_ATTRIBUTE_NODISCARD
#  define C_ATTRIBUTE_MAYBE_UNUSED
#  define C_ATTRIBUTE_REPRODUCIBLE
#  define C_ATTRIBUTE_UNSEQUENCED
#endif


//------------------------------------------------------------------------------
// <stdnoreturn.h> - The `noreturn` macro ... available on user's request
//
// - The C23's [[noreturn]] attribute or C11/C17's _Noreturn keyword MUST NOT
//   be used in Konpu C.
// - The definition of a `noreturn` macro (as in C11/C17) may be requested
//   (via a KONPU_OPTION) to show an intent of not returning from a function,
//   but in actually does nothing. Alternatively, the C_ATTRIBUTE_NORETURN macro
//   is equivalent in Konpu C to show the same intent (and also
//   expands to nothing).
//
// see: https://en.cppreference.com/w/c/header/stdnoreturn.html
//      https://en.cppreference.com/w/c/language/noreturn.html
//      https://en.cppreference.com/w/c/language/attributes/noreturn
//------------------------------------------------------------------------------
#ifdef KONPU_OPTION_NORETURN
#  define noreturn
#endif
#ifdef KONPU_H_
#  define C_ATTRIBUTE_NORETURN  // expands to nothing
#elif __STDC_VERSION__ >= 202311L
#  define C_ATTRIBUTE_NORETURN  [[noreturn]]
#else
#  define C_ATTRIBUTE_NORETURN  _Noreturn
#endif

//------------------------------------------------------------------------------
// <stdbool.h> - Boolean type support
//
// see: https://en.cppreference.com/w/c/header/stdbool.html
//------------------------------------------------------------------------------
// The header is deprecated in C23 (bool/true/false are builtins). We could have
// included it (for < C17) as it is fully freestanding, but as it should only
// contain a few defines, Konpu implements it.
#if __STDC_VERSION__ < 202311L
#  define bool    _Bool
#  define true    1
#  define false   0
#endif
#ifndef   __bool_true_false_are_defined
#  define __bool_true_false_are_defined  1
#endif

//------------------------------------------------------------------------------
// <stdalign.h> - alignas and alignof
//
// see: https://en.cppreference.com/w/c/header/stdalign.html
//      https://en.cppreference.com/w/c/language/alignas.html
//      https://en.cppreference.com/w/c/language/alignof.html
//------------------------------------------------------------------------------
// The header is deprecated in C23 (alignas/aligno supported as keywords). It
// could have been include it (for < C17) as it is a fully freestanding, but as
// it should only contain a few defines, Konpu implements it.
#if __STDC_VERSION__ < 202311L
#  define alignas  _Alignas
#  define alignof  _Alignof
#endif
#ifndef   __alignas_is_defined
#  define __alignas_is_defined  1
#endif
#ifndef   __alignof_is_defined
#  define __alignof_is_defined  1
#endif

//------------------------------------------------------------------------------
// <iso646.h> - Alternative operator spellings ... available on user's request
//
// see: https://en.cppreference.com/w/c/header/iso646.html
//      https://en.cppreference.com/w/c/language/operator_alternative.html
//------------------------------------------------------------------------------
// We could include the header as it is fully freestanding header, but as it
// should only contain a few defines, Konpu implements it.
#ifdef KONPU_OPTION_ISO646
#  define and     &&
#  define and_eq  &=
#  define bitand  &
#  define bitor   |
#  define compl   ~
#  define not     !
#  define not_eq  !=
#  define or      ||
#  define or_eq   |=
#  define xor     ^
#  define xor_eq  ^=
#endif

//------------------------------------------------------------------------------
// <stddef.h> - Common basic macro and type definitions
//
// see: https://en.cppreference.com/w/c/header/stddef
//------------------------------------------------------------------------------
// Include the header as it is fully freestanding.
#include <stddef.h>

// Making sure NULL is not defined as simply 0... which might happens sometimes
//                               (mostly because (void*)0 isn't C++ compatible)
#undef    NULL
#if __STDC_VERSION__ < 202311L
#  define NULL  ((void*)0)
#else
#  define NULL  nullptr
#endif

#if KONPU_PLATFORM_LIBC && (!defined(NDEBUG))
   // In debug mode and have the "real" assert from C, then
   // then (re)define C's unreachable() as `assert(0)` (even for C23).
#  undef  unreachable
#  define unreachable()  assert(0)
#elif __STDC_VERSION__ < 202311L
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
#  ifdef __GNUC__
#     define unreachable()   (__builtin_unreachable())
#  elif defined(_MSC_VER)
#     define unreachable()   (__assume(false))
#  else
#     define unreachable()   ((void)0)
#  endif
#endif

//------------------------------------------------------------------------------
// <stdarg.h> - Variable arguments functionalities
//
// see: https://en.cppreference.com/w/c/header/stdarg.html
//------------------------------------------------------------------------------
// Include the header as it is fully freestanding.
#include <stdarg.h>

//------------------------------------------------------------------------------
// <assert.h> - assert / static_assert macros
//------------------------------------------------------------------------------

// The `assert(...)` macro (similar to <assert.h>)
// The macro (may) evaluate its arguments and terminates the program if the
// evaluation is zero.
#ifdef NDEBUG
   // In release build, turn assertions in ASSUMEption HINTs, so that arguments
   // never get evaluated, and may be used for potential further optimization.
   // This requires assert() statements to be free from side effects, which
   // is not forced by C but is supposed to be that way (otherwise assertions
   // could never be disabled)
#  define assert(...)      C_HINT_ASSUME(__VA_ARGS__)
#elif KONPU_PLATFORM_LIBC
#  include <assert.h>      // libc's assert
#elif defined(__GNUC__)    // with no platform, try to stop the execution
#  define assert(...)      if (!(__VA_ARGS__))  __builtin_trap()
#elif _MSC_VER
#  define assert(...)      if (!(__VA_ARGS__))  __debugbreak()
#else
#  define assert(...)      ((void)0)
   // C cannot portably stop the program execution, although one might wish
   // to trigger a fault to do that in practice, for example:
   // #define assert(...)  if (!(__VA_ARGS__))  *(volatile int *)0 = 0
#endif

// The `static_assert(condition, [message])` keyword/macro
#if __STDC_VERSION__ < 202311L
   // C23: nothing to do (`static_assert` is a C keyword)
   // C11 and C17 (and gcc) have _Static_assert(expression, message)
#  undef  static_assert // <-- undef as this has been defined by <assert.h>
#  define static_assert(...)   UTIL_OVERLOAD(PRIVATE_STATIC_ASSERT, __VA_ARGS__)
#     define PRIVATE_STATIC_ASSERT_2_(condition, string_literal)               \
         _Static_assert((condition), string_literal)
#     define PRIVATE_STATIC_ASSERT_1_(condition)                               \
         _Static_assert((condition), UTIL_STRINGIFY(__LINE__))
#endif

//------------------------------------------------------------------------------
// <limits.h> - Range of integer types
//
// see: https://en.cppreference.com/w/c/header/limits.html
//------------------------------------------------------------------------------
// Include the header as it is fully freestanding.
#include <limits.h>
#if CHAR_BIT != 8  // Konpu ASSUMEs 8-bits bytes (and so do POSIX and Windows)
#  error "byte width is not 8-bits"
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
   // also, see explanations at:
   // https://web.archive.org/web/20150403064546/http://coding.derkeiler.com/Archive/C_CPP/comp.lang.c/2009-01/msg02242.html
// "Polyfill" definitions for C prior to C23:
#if __STDC_VERSION__ < 202311L
#  define CHAR_WIDTH         CHAR_BIT
#  define UCHAR_WIDTH        CHAR_BIT
#  define SCHAR_WIDTH        CHAR_BIT
#  define USHRT_WIDTH        UTIL_COUNT_BITS_OF_2N1_(USHRT_MAX)
#  define UINT_WIDTH         UTIL_COUNT_BITS_OF_2N1_(UINT_MAX)
#  define ULONG_WIDTH        UTIL_COUNT_BITS_OF_2N1_(ULONG_MAX)
#  define ULLONG_WIDTH       UTIL_COUNT_BITS_OF_2N1_(ULLONG_MAX)
#  define SHRT_WIDTH         USHRT_WIDTH
#  define INT_WIDTH          UINT_WIDTH
#  define LONG_WIDTH         ULONG_WIDTH
#  define LLONG_WIDTH        ULLONG_WIDTH
   // (note: C23 also has BOOL_WIDTH [and a few more])
#endif
// bit-precise integers (may work starting from C23)
#ifndef   BITINT_MAXWIDTH     // Test BITINT_MAXWIDTH >= n to know if the type
#  define BITINT_MAXWIDTH  0  // _BitInt(n) can be used.
#endif

//------------------------------------------------------------------------------
// <stdint.h> - Fixed-width integer types
//
// see: https://en.cppreference.com/w/c/header/stdint.html
//------------------------------------------------------------------------------
// Include the header as it is fully freestanding.
#include <stdint.h>
// Konpu ASSUMEs the existence of [u]int<8|16|32|64>_t
static_assert(sizeof( int_least8_t ) == 1);
static_assert(sizeof(uint_least8_t ) == 1);
static_assert(sizeof( int_least16_t) == 2);
static_assert(sizeof(uint_least16_t) == 2);
static_assert(sizeof( int_least32_t) == 4);
static_assert(sizeof(uint_least32_t) == 4);
static_assert(sizeof( int_least64_t) == 8);
static_assert(sizeof(uint_least64_t) == 8);
// "Polyfill" definitions for C prior to C23
#if __STDC_VERSION__ < 202311L
#  define INT8_WIDTH            8
#  define INT16_WIDTH          16
#  define INT32_WIDTH          32
#  define INT64_WIDTH          64
#  define INT_LEAST8_WIDTH      8
#  define INT_LEAST16_WIDTH    16
#  define INT_LEAST32_WIDTH    32
#  define INT_LEAST64_WIDTH    64
#  define INT_FAST8_WIDTH      UTIL_COUNT_BITS_OF_2N1_(INT_FAST8_MAX)
#  define INT_FAST16_WIDTH     UTIL_COUNT_BITS_OF_2N1_(INT_FAST16_MAX)
#  define INT_FAST32_WIDTH     UTIL_COUNT_BITS_OF_2N1_(INT_FAST32_MAX)
#  define INT_FAST64_WIDTH     UTIL_COUNT_BITS_OF_2N1_(INT_FAST64_MAX)
#  define UINT8_WIDTH           8
#  define UINT16_WIDTH         16
#  define UINT32_WIDTH         32
#  define UINT64_WIDTH         64
#  define UINT_LEAST8_WIDTH     8
#  define UINT_LEAST16_WIDTH   16
#  define UINT_LEAST32_WIDTH   32
#  define UINT_LEAST64_WIDTH   64
#  define UINT_FAST8_WIDTH     UTIL_COUNT_BITS_OF_2N1_(UINT_FAST8_MAX)
#  define UINT_FAST16_WIDTH    UTIL_COUNT_BITS_OF_2N1_(UINT_FAST16_MAX)
#  define UINT_FAST32_WIDTH    UTIL_COUNT_BITS_OF_2N1_(UINT_FAST32_MAX)
#  define UINT_FAST64_WIDTH    UTIL_COUNT_BITS_OF_2N1_(UINT_FAST64_MAX)
//
#  define SIZE_WIDTH           UTIL_COUNT_BITS_OF_2N1_(SIZE_MAX)
#  define INTMAX_WIDTH         UTIL_COUNT_BITS_OF_2N1_(INTMAX_MAX)
#  define UINTMAX_WIDTH        UTIL_COUNT_BITS_OF_2N1_(UINTMAX_MAX)
#  define PTRDIFF_WIDTH        UTIL_COUNT_BITS_OF_2N1_(PTRDIFF_MAX)
#  ifdef INTPTR_MAX            // optional type
#     define INTPTR_WIDTH      UTIL_COUNT_BITS_OF_2N1_(INTPTR_MAX)
#  endif
#  ifdef  UINTPTR_MAX          // optional type
#     define UINTPTR_WIDTH     UTIL_COUNT_BITS_OF_2N1_(UINTPTR_MAX)
#  endif
#endif

//------------------------------------------------------------------------------
// <stdckdint.h> - Checked integer arithmetic
// The checked integer arithmetic ckd_add/sub/mul are provided even to C11/C17.
//
// see: https://en.cppreference.com/w/c/header/stdckdint.html
//
// When option KONPU_OPTION_PREFER_JTCKDINT is defined:
//   Prior to C23 and even when using GCC or Clang, expose the checked
//   arithmetic macros as those from the jtckdint project (see: external/).
//   They will also be used for all Konpu's static functions defined in konpu.h.
//------------------------------------------------------------------------------
#if __STDC_VERSION__ >= 202311L
// The header is available (starting with C23) and freestanding
#  include <stdckdint.h>
#elif defined(__GNUC__) && !defined(KONPU_OPTION_PREFER_JTCKDINT)
// Intrinsics: https://gcc.gnu.org/onlinedocs/gcc/Integer-Overflow-Builtins.html
#  define ckd_add(R, A, B)  __builtin_add_overflow((A), (B), (R))
#  define ckd_sub(R, A, B)  __builtin_sub_overflow((A), (B), (R))
#  define ckd_mul(R, A, B)  __builtin_mul_overflow((A), (B), (R))
#else
#  include "../external/jtckdint-1.0/jtckdint.h"
#endif


//------------------------------------------------------------------------------
// <stdbit.h> - working with bytes and bits
//
// Konpu C provides the macros for endianness of scalar types, ie:
//      __STDC_ENDIAN_BIG__, __STDC_ENDIAN_LITTLE__, __STDC_ENDIAN_NATIVE__
// and the type generic bit macros from C23 working for uint<8|16|32|64>_t, ie:
//     stdc_bit_ceil, stdc_bit_floor, stdc_bit_width, stdc_count_ones,
//     stdc_count_zeros, stdc_first_leading_one, stdc_first_leading_zero,
//     stdc_first_trailing_one, stdc_first_trailing_zero, stdc_has_single_bit,
//     stdc_leading_ones, stdc_leading_zeros, stdc_trailing_ones,
//     stdc_trailing_zeros.
//
// One should use the type generic macros only and NOT the stdc_*_u[c|s|i|l|ll].
// Konpu only garantuees that macros work on parameters of type
// uint<8|16|32|64>_t.
//
// See: https://en.cppreference.com/w/c/header/stdbit.html
// For details and implementation, see: the "bits.h" header.
//------------------------------------------------------------------------------

// Implemented in the file "bits.h" (along with other bit utilities)


//------------------------------------------------------------------------------
// <inttypes.h> - format conversion of integer types
//
// see: https://en.cppreference.com/w/c/header/inttypes.html
//
// Note:
// - The `imaxdiv_t` type, and `imaxabs`, `imaxdiv` and `strtoimax`/`strtoumax`
//   functions are defined (along similar types and functions) in the
//   <stdlib.h> section below.
// - `wcstoimax`/`wcstoumax` are not defined as Konpu doesn't support wchar_t.
// - Thus all we define in this section are the stdio format macro.
//------------------------------------------------------------------------------

// <inttypes.h> is not freestanding (it easily could, because usually it doesn't
// make much sense to included it out of <stdio.h>), so Konpu must implement it.


#if   INT_WIDTH   == 16
#  define C_INTTYPES_PREFIX16_
#elif INT_WIDTH   == 32
#  define C_INTTYPES_PREFIX32_
#elif INT_WIDTH   == 64
#  define C_INTTYPES_PREFIX64_
#endif
#if   LONG_WIDTH  == 32 && !defined(C_INTTYPES_PREFIX32_)
#  define C_INTTYPES_PREFIX32_  "l"
#elif LONG_WIDTH  == 64 && !defined(C_INTTYPES_PREFIX64_)
#  define C_INTTYPES_PREFIX64_  "l"
#endif
#if   LLONG_WIDTH == 64 && !defined(C_INTTYPES_PREFIX64_)
#  define C_INTTYPES_PREFIX64_  "ll"
#endif
#if   SHRT_WIDTH  == 16 && !defined(C_INTTYPES_PREFIX16_)
#  define C_INTTYPES_PREFIX16_  "h"
#elif SHRT_WIDTH  == 32 && !defined(C_INTTYPES_PREFIX32_)
#  define C_INTTYPES_PREFIX32_  "h"
#elif SHRT_WIDTH  == 64 && !defined(C_INTTYPES_PREFIX64_)
#  define C_INTTYPES_PREFIX64_  "h"
#endif
#ifndef C_INTTYPES_PREFIX16_
#  error "inttypes: cannot define C_INTTYPES_PREFIX16_"
#endif
#ifndef C_INTTYPES_PREFIX32_
#  error "inttypes: cannot define C_INTTYPES_PREFIX32_"
#endif
#ifndef C_INTTYPES_PREFIX64_
#  error "inttypes: cannot define C_INTTYPES_PREFIX64_"
#endif

#ifdef KONPU_H_
#
#  // PRN*<n> for fixed width
#  define PRId8          "d"
#  define PRIi8          "i"
#  define PRIo8          "o"
#  define PRIu8          "u"
#  define PRIx8          "x"
#  define PRIX8          "X"
#  define PRId16         "d"
#  define PRIi16         "i"
#  define PRIo16         "o"
#  define PRIu16         "u"
#  define PRIx16         "x"
#  define PRIX16         "X"
#  define PRId32         C_INTTYPES_PREFIX32_ "d"
#  define PRIi32         C_INTTYPES_PREFIX32_ "i"
#  define PRIo32         C_INTTYPES_PREFIX32_ "o"
#  define PRIu32         C_INTTYPES_PREFIX32_ "u"
#  define PRIx32         C_INTTYPES_PREFIX32_ "x"
#  define PRIX32         C_INTTYPES_PREFIX32_ "X"
#  define PRId64         C_INTTYPES_PREFIX64_ "d"
#  define PRIi64         C_INTTYPES_PREFIX64_ "i"
#  define PRIo64         C_INTTYPES_PREFIX64_ "o"
#  define PRIu64         C_INTTYPES_PREFIX64_ "u"
#  define PRIx64         C_INTTYPES_PREFIX64_ "x"
#  define PRIX64         C_INTTYPES_PREFIX64_ "X"
#
#  // SCN*<n> for fixed width
#  define SCNd8          "hhd"
#  define SCNi8          "hhi"
#  define SCNo8          "hho"
#  define SCNu8          "hhu"
#  define SCNx8          "hhx"
#  define SCNd16         C_INTTYPES_PREFIX16_ "d"
#  define SCNi16         C_INTTYPES_PREFIX16_ "i"
#  define SCNo16         C_INTTYPES_PREFIX16_ "o"
#  define SCNu16         C_INTTYPES_PREFIX16_ "u"
#  define SCNx16         C_INTTYPES_PREFIX16_ "x"
#  define SCNd32         C_INTTYPES_PREFIX32_ "d"
#  define SCNi32         C_INTTYPES_PREFIX32_ "i"
#  define SCNo32         C_INTTYPES_PREFIX32_ "o"
#  define SCNu32         C_INTTYPES_PREFIX32_ "u"
#  define SCNx32         C_INTTYPES_PREFIX32_ "x"
#  define SCNd64         C_INTTYPES_PREFIX64_ "d"
#  define SCNi64         C_INTTYPES_PREFIX64_ "i"
#  define SCNo64         C_INTTYPES_PREFIX64_ "o"
#  define SCNu64         C_INTTYPES_PREFIX64_ "u"
#  define SCNx64         C_INTTYPES_PREFIX64_ "x"
#
#  // *MAX: why does C define this? Just use 'j' which is the prefix for intmax.
#  define PRIdMAX        "jd"
#  define PRIiMAX        "ji"
#  define PRIoMAX        "jo"
#  define PRIuMAX        "ju"
#  define PRIxMAX        "jx"
#  define PRIXMAX        "jX"
#  define SCNdMAX        "jd"
#  define SCNiMAX        "ji"
#  define SCNoMAX        "jo"
#  define SCNuMAX        "ju"
#  define SCNxMAX        "jx"
#
#  // *PTR: Konpu assumes pointer to be 32 or 64 bits. (Rationale: any pointer
#  //       fits in a var thus is at most 64 bits; it can address all of Konpu's
#  //       memory, thus is >=20 bits)
#  if UINTPTR_MAX == UINT64_MAX
#     define PRIdPTR     PRId64
#     define PRIiPTR     PRIi64
#     define PRIoPTR     PRIo64
#     define PRIuPTR     PRIu64
#     define PRIxPTR     PRIx64
#     define PRIXPTR     PRIX64
#     define SCNdPTR     SCNd64
#     define SCNiPTR     SCNi64
#     define SCNoPTR     SCNo64
#     define SCNuPTR     SCNu64
#     define SCNxPTR     SCNx64
#  elif UINTPTR_MAX == UINT32_MAX
#     define PRIdPTR     PRId32
#     define PRIiPTR     PRIi32
#     define PRIoPTR     PRIo32
#     define PRIuPTR     PRIu32
#     define PRIxPTR     PRIx32
#     define PRIXPTR     PRIX32
#     define SCNdPTR     SCNd32
#     define SCNiPTR     SCNi32
#     define SCNoPTR     SCNo32
#     define SCNuPTR     SCNu32
#     define SCNxPTR     SCNx32
#  else
#     error "Konpu wrongly assumed pointers are 32 or 64 bits."
#  endif
#
#  // *LEAST*: In Konpu, uint_least*_t is assumed to be same as uint*_t.
#  define PRIdLEAST8     PRId8
#  define PRIiLEAST8     PRIi8
#  define PRIoLEAST8     PRIo8
#  define PRIuLEAST8     PRIu8
#  define PRIxLEAST8     PRIx8
#  define PRIXLEAST8     PRIX8
#  define SCNdLEAST8     SCNd8
#  define SCNiLEAST8     SCNi8
#  define SCNoLEAST8     SCNo8
#  define SCNuLEAST8     SCNu8
#  define SCNxLEAST8     SCNx8
#  define PRIdLEAST16    PRId16
#  define PRIiLEAST16    PRIi16
#  define PRIoLEAST16    PRIo16
#  define PRIuLEAST16    PRIu16
#  define PRIxLEAST16    PRIx16
#  define PRIXLEAST16    PRIX16
#  define SCNdLEAST16    SCNd16
#  define SCNiLEAST16    SCNi16
#  define SCNoLEAST16    SCNo16
#  define SCNuLEAST16    SCNu16
#  define SCNxLEAST16    SCNx16
#  define PRIdLEAST32    PRId32
#  define PRIiLEAST32    PRIi32
#  define PRIoLEAST32    PRIo32
#  define PRIuLEAST32    PRIu32
#  define PRIxLEAST32    PRIx32
#  define PRIXLEAST32    PRIX32
#  define SCNdLEAST32    SCNd32
#  define SCNiLEAST32    SCNi32
#  define SCNoLEAST32    SCNo32
#  define SCNuLEAST32    SCNu32
#  define SCNxLEAST32    SCNx32
#  define PRIdLEAST64    PRId64
#  define PRIiLEAST64    PRIi64
#  define PRIoLEAST64    PRIo64
#  define PRIuLEAST64    PRIu64
#  define PRIxLEAST64    PRIx64
#  define PRIXLEAST64    PRIX64
#  define SCNdLEAST64    SCNd64
#  define SCNiLEAST64    SCNi64
#  define SCNoLEAST64    SCNo64
#  define SCNuLEAST64    SCNu64
#  define SCNxLEAST64    SCNx64

// FAST*: NOT IMPLEMENTED YET / TODO


#  if   UINT_FAST64_MAX == UINT64_MAX
#     define C_INTTYPES_PREFIX_FAST64_ PREFIX"hh"
#  elif UINT_FAST64_MAX == USHRT_MAX
#     define C_INTTYPES_PREFIX_FAST8_ "h"
#  elif UINT_FAST8_MAX == UINT_MAX
#     define C_INTTYPES_PREFIX_FAST8_
#  elif UINT_FAST8_MAX == ULONG_MAX
#     define C_INTTYPES_PREFIX_FAST8_ "l"
#  elif UINT_FAST8_MAX == ULLONG_MAX
#     define C_INTTYPES_PREFIX_FAST8_ "ll"
#  else
#     error "uint_fast8_t is not unsigned char/short/int/long/long long."
#  endif
#
#  // FAST* // TODO: check uchar/ushort for fast8/fast16!
#
#endif //KONPU_H_






//------------------------------------------------------------------------------
// <stdlib.h>
//
// Konpu does NOT include <stdlib.h> but provides it own implementation for a
// few functions from it:
// `abs`, ...
//------------------------------------------------------------------------------

// Numeric conversion functions (atoX, strtoX, and strfromX): TODO
// In C23, when __STDC_IEC_60559_BFP__ or __STDC_IEC_60559_DFP__ are predefined,
// those functions are freestanding (although required to be well-defined only
// if a program does not set the state of the FENV_ACCESS pragma to ON)
double atof(const char* nptr);
int atoi(const char* nptr);
long atol(const char* nptr);
long long atoll(const char* nptr);
int strfromd(char* restrict s, size_t n, const char* restrict format, double fp);
int strfromf(char* restrict s, size_t n, const char* restrict format, float fp);
int strfroml(char* restrict s, size_t n, const char* restrict format, long double fp);
double strtod(const char* restrict nptr, char** restrict endptr);
float strtof(const char* restrict nptr, char** restrict endptr);
long double strtold(const char* restrict nptr, char** restrict endptr);
long strtol(const char* restrict nptr, char** restrict endptr, int base);
long long strtoll(const char* restrict nptr, char** restrict endptr, int base);
unsigned long strtoul(const char* restrict nptr, char** restrict endptr, int base);
unsigned long long strtoull(const char* restrict nptr, char** restrict endptr, int base);

// `strtoimax`/`strtoumax` (from inttypes.h)
#  if (INTMAX_MAX > LLONG_MAX) || (INTMAX_MIN < LLONG_MIN) || (UINTMAX_MAX > ULLONG_MAX)
#     define strtoimax     strtoll
#     define strtoumax     strtoull
#  else
#     define strtoimax     KONPU_DOES_NOT_SUPPORT_STRTOIMAX_
#     define strtoumax     KONPU_DOES_NOT_SUPPORT_STRTOUMAX_
#  endif


// System functions:
// - char* getenv(const char* name);
// - int system(const char* string); TODO
#ifdef KONPU_H_
#  define getenv(name)     c_getenv_(name)
#  define system(command)  c_system_(command)
#endif
static inline char *c_getenv_(const char *name)
{ (void)name; return NULL;} // for now, no support for env. variables
static inline int c_system_(const char* command) {
   // When command is null, return 0 to indicate a shell is available
   if (!command) return 0;
   // TODO: Otherwise pass command to Waso interpreter and return the value
   return 1;
}

// Integer functions ("div" and "abs", also those <inttypes.h>)
// -  int abs(int j);
// -  long labs(long j);
// -  long long llabs(long long j);
// -   intmax_t imaxabs( intmax_t n ); [from <inttypes.h>]
// ^- Note: taking the absolute value of the most negative integer isn't defined
// -  div_t     div(int numer, int denom);
// -  ldiv_t    ldiv(long numer, long denom);
// -  lldiv_t   lldiv(long long numer, long long denom);
// -  imaxdiv_t imaxdiv( intmax_t x, intmax_t y ); [from <inttypes.h>]
#ifdef KONPU_H_
#  define div_t              c_div_t_
#  define ldiv_t             c_ldiv_t_
#  define lldiv_t            c_lldiv_t_
#  define imaxdiv_t          c_imaxdiv_t_
#  define abs(n)             c_abs_(n)
#  define labs(n)            c_labs_(n)
#  define llabs(n)           c_llabs_(n)
#  define imaxabs(n)         c_imaxabs_(n)
#  define div(num, den)      c_div_((num), (den))
#  define ldiv(num, den)     c_ldiv_((num), (den))
#  define lldiv(num, den)    c_lldiv_((num), (den))
#  define imaxdiv(num, den)  c_imaxdiv_((num), (den))
#endif
typedef struct { int       quot, rem; }  c_div_t_;
typedef struct { long      quot, rem; }  c_ldiv_t_;
typedef struct { long long quot, rem; }  c_lldiv_t_;
typedef struct { intmax_t  quot, rem; }  c_imaxdiv_t_;
static inline int          c_abs_(int n)            { return (n < 0 ? -n : n); }
static inline long         c_labs_(long n)          { return (n < 0 ? -n : n); }
static inline long long    c_llabs_(long long n)    { return (n < 0 ? -n : n); }
static inline intmax_t     c_imaxabs_(intmax_t n)   { return (n < 0 ? -n : n); }
static inline c_div_t_     c_div_(int num, int den)                { return (c_div_t_)    { num / den, num % den }; }
static inline c_ldiv_t_    c_ldiv_(long num, long den)             { return (c_ldiv_t_)   { num / den, num % den }; }
static inline c_lldiv_t_   c_lldiv_(long long num, long long den)  { return (c_lldiv_t_)  { num / den, num % den }; }
static inline c_imaxdiv_t_ c_imaxdiv_(intmax_t num, intmax_t den)  { return (c_imaxdiv_t_){ num / den, num % den }; }

// Algorithms (Search and Sort): TODO
// -  QVoid* bsearch(const void* key, QVoid* base, size_t nmemb, size_t size,
//                   int (*compar)(const void* , const void* ));
// -  void qsort(void* base, size_t nmemb, size_t size,
//               int (*compar)(const void* , const void* ));

// Allocation functions: Konpu implements functions it provides:
// [ ]   void* malloc(size_t size);
// [ ]   void* calloc(size_t nmemb, size_t size);
// [ ]   void* realloc(void* ptr, size_t size);
// [ ]   void  free(void* ptr);
// (no)  void* aligned_alloc(size_t alignment, size_t size);
// (no)  void  free_sized(void* ptr, size_t size);
// (no)  void  free_aligned_sized(void* ptr, size_t alignment, size_t size);
#ifdef KONPU_H_
#  define malloc(size)        c_malloc_(size)
#  define calloc(n, size)     c_calloc_(n, size)
#  define realloc(ptr, size)  c_realloc_(ptr, size)
#  define free(ptr)           c_free_(ptr)
#endif

// Macro constants
#ifdef KONPU_H_
#  define EXIT_SUCCESS  0
#  define EXIT_FAILURE  1
#endif

// Random numbers functions:
// Konpu provides an implementation of `int rand(void)` and
// `void srand(unsigned int seed)` in random.h

// Exit functions. Konpu MAY later provide its implementation for some of
//                 those functions:
// -  [[noreturn]] void abort(void);
// -  int atexit(void (*func)(void));
// -  int at_quick_exit(void (*func)(void));
// -  [[noreturn]] void exit(int status);
// -  [[noreturn]] void _Exit(int status);
// -  [[noreturn]] void quick_exit(int status);

// Threads functions, Konpu does NOT provide those.
// call_once

// Multibytes string functions, Konpu does NOT provide those:
// mblen, mbtowc, wctomb, mbstowcs, wcstombs


//------------------------------------------------------------------------------
// Other C standard headers
//------------------------------------------------------------------------------
// <time.h> (very partial support)
// type `time_t` and function `time_t time(time_t *t)` define in "timing.h"
//
// <stdio.h> (most functions are supported)
//------------------------------------------------------------------------------
// type `time_t` and function `time_t time(time_t *t)` define in "timing.h"

//------------------------------------------------------------------------------
// <ctype.h> - character type features
//
// Functions: isalnum, isalpha, islower, isupper, isdigit, isxdigit, iscntrl,
//            isgraph, isspace, isblank, isprint, ispunct,
//            tolower, toupper
//
// see:
//------------------------------------------------------------------------------

// implemented in char.h (along with other character functions)


//------------------------------------------------------------------------------
// <wchar.h>    wide character utilities
// <wctype.h>   wide character features - not supported.
// <uchar.h>    UTF-16 and UTF-32 character utilities
//               '--> we'd like at least some partial support for this
//------------------------------------------------------------------------------

// a byte in UTF-8 encoding
typedef unsigned char char8_t;

// 16 bit characters encoding UTF-16
typedef uint16_t      char16_t;

// Unicode codepoint / UTF-32 character
typedef uint32_t      char32_t;

// typedef struct mbstate_t mbstate_t;
// #define MB_LEN_MAX <-- <limits.h>
// #define MB_CUR_MAX <-- <stdlib.h>
// mblen(), mbrlen(), mbsinit() ...

// wchar types and limits were defined by stddef, etc., but remove them:
#ifdef KONPU_H_
#  undef  WCHAR_MIN
#  undef  WCHAR_MAX
#  undef  wchar_t
#  define wchar_t   KONPU_DOES_NOT_USE_WCHAR_T_
#endif

/*
size_t c8rtomb( char* restrict s, char8_t  c8 , mbstate_t* restrict ps);
size_t c16rtomb(char* restrict s, char16_t c16, mbstate_t* restrict ps);
size_t c32rtomb(char* restrict s, char32_t c32, mbstate_t* restrict ps);
size_t mbrtoc8( char8_t*  restrict pc8 , const char* restrict s, size_t n, mbstate_t* restrict ps);
size_t mbrtoc16(char16_t* restrict pc16, const char* restrict s, size_t n, mbstate_t* restrict ps);
size_t mbrtoc32(char32_t* restrict pc32, const char* restrict s, size_t n, mbstate_t* restrict ps);
*/


//------------------------------------------------------------------------------
// <string.h> - null-terminated byte strings
//
// - null-terminated C string utility functions,
// - strerror function (see "errno section")
// - mem* functions
//
// see: https://en.cppreference.com/w/c/header/string.html
//      https://en.cppreference.com/w/c/string/byte.html
//------------------------------------------------------------------------------

// Declaration for function we absolutely need:
extern void *memset(void *dst, int ch, size_t count);
extern void *memcpy(void *restrict dst, const void *restrict src, size_t count);
// memmove : to be provided
// memchr  : to be provided [C99, but provide it with an interface a la C23]
// memset_explicit (C23) - hmm, no?
// memccpy (C23) - hmmm, no or maybe to be provided later



#if __STDC_VERSION__ >= 202311L
// In C23, <string.h> became partially freestanding.
// https://cppreference.com/w/c/language/conformance.html
// Implement or mark as banned (so we don't use them), the string functions not
// required by a freestanding implementation, ie: strdup, strndup, strxfrm,
// strtok, strerror.
#  include <string.h>
#  ifndef KONPU_H_
      // Return a new string which is a duplicate of the given string. Memory
      // for the new string is obtained with `malloc` (and thus should be
      // properly `free`-d by the user). Return NULL if allocation fails.
#     define strdup(cstring)   c_strdup_(string)
         static inline char *c_strdup_(const char *str) {
            size_t size = strlen(str) + 1;
            char *s = malloc(size);
            if (s)  memcpy(s, str, size);
            return s;
         }
#     define strdup(string)             C_BANNED(strdup)
#     undef  strndup
#     define strndup(string, size)      C_BANNED(strndup)
#     undef  strxfrm
#     define strxfrm(dest, src, n)      C_BANNED(strxfrm)
#     undef  strtok
#     define strtok(string, str_delim)  C_BANNED(strtok)
#     undef  strerror
#     define strerror(errnum)           C_BANNED(errnum)
#  endif
#else
#endif

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

//------------------------------------------------------------------------------
// <float.h> - Limits of floating-point types
//
// see: https://en.cppreference.com/w/c/header/float.html
//------------------------------------------------------------------------------
// Include the header as it is fully freestanding.
#include <float.h>

//------------------------------------------------------------------------------
// <complex.h> - complex number arithmetic
//
// As of now, Konpu C does NOT support complex numbers.
// If complex numbers are needed, users need to implement those by themselves,
// which is not too hard (but of course cannot be made to support the + - * /
// operators like a builtin C type could)
//
// see: https://en.cppreference.com/w/c/numeric/complex.html
//      https://en.cppreference.com/w/c/header/complex.html
//------------------------------------------------------------------------------
#ifdef KONPU_H_
#  undef  __STDC_NO_COMPLEX__
#  define __STDC_NO_COMPLEX__    KONPU_C_DOES_NOT_SUPPORT_COMPLEX_NUMBERS_
#endif

//------------------------------------------------------------------------------
// <tgmath.h> - type generic macros for <math.h> functions ...
//              ... available on user's request (via a KONPU_OPTION)
//
// As Konpu C doesn't have complex numbers, type generic macros only dispatch
// calls between the double (= no prefix), float (= prefix `f`), or long double
// (= prefix `l`) versions of <math.h> functions.
//
// see: https://en.cppreference.com/w/c/header/tgmath.html
//      https://en.cppreference.com/w/c/numeric/tgmath.html
//------------------------------------------------------------------------------

#define KONPU_TGMATHIFY_1_(func)  \
   _Generic((x)                 , \
      float      : func##f      , \
      double     : func         , \
      long double: func##l        \
   )((x))

#ifdef KONPU_OPTION_TGMATH
/* TODO
#define __STDC_VERSION_TGMATH_H__  // TODO / 202311L  ??
#define acos(x)           KONPU_TGMATHIFY_1_(acos)
#define acosh             KONPU_TGMATHIFY_1_(acosh)
#define acospi            // TODO
#define asin              // TODO
#define asinh             // TODO
#define asinpi            // TODO
#define atan              // TODO
#define atan2             // TODO
#define atan2pi           // TODO
#define atanh             // TODO
#define atanpi            // TODO
#define cbrt              // TODO
#define ceil              // TODO
#define compoundn         // TODO
#define copysign          // TODO
#define cos               // TODO
#define cosh              // TODO
#define cospi             // TODO
#define dadd              // TODO
#define ddiv              // TODO
#define dfma              // TODO
#define dmul              // TODO
#define dsqrt             // TODO
#define dsub              // TODO
#define erf               // TODO
#define erfc              // TODO
#define exp               // TODO
#define exp10             // TODO
#define exp10m1           // TODO
#define exp2              // TODO
#define exp2m1            // TODO
#define expm1             // TODO
#define fabs              // TODO
#define fadd              // TODO
#define fdim              // TODO
#define fdiv              // TODO
#define ffma              // TODO
#define floor             // TODO
#define fma               // TODO
#define fmax              // TODO
#define fmaximum          // TODO
#define fmaximum_mag      // TODO
#define fmaximum_mag_num  // TODO
#define fmaximum_num      // TODO
#define fmin              // TODO
#define fminimum          // TODO
#define fminimum_mag      // TODO
#define fminimum_mag_num  // TODO
#define fminimum_num      // TODO
#define fmod              // TODO
#define fmul              // TODO
#define frexp             // TODO
#define fromfp            // TODO
#define fromfpx           // TODO
#define fsqrt             // TODO
#define fsub              // TODO
#define hypot             // TODO
#define ilogb             // TODO
#define ldexp             // TODO
#define lgamma            // TODO
#define llogb             // TODO
#define llrint            // TODO
#define llround           // TODO
#define log               // TODO
#define log10             // TODO
#define log10p1           // TODO
#define log1p             // TODO
#define log2              // TODO
#define log2p1            // TODO
#define logb              // TODO
#define logp1             // TODO
#define lrint             // TODO
#define lround            // TODO
#define nearbyint         // TODO
#define nextafter         // TODO
#define nextdown          // TODO
#define nexttoward        // TODO
#define nextup            // TODO
#define pow               // TODO
#define pown              // TODO
#define powr              // TODO
#define remainder         // TODO
#define remquo            // TODO
#define rint              // TODO
#define rootn             // TODO
#define round             // TODO
#define roundeven         // TODO
#define rsqrt             // TODO
#define scalbln           // TODO
#define scalbn            // TODO
#define sin               // TODO
#define sinh              // TODO
#define sinpi             // TODO
#define sqrt              // TODO
#define tan               // TODO
#define tanh              // TODO
#define tanpi             // TODO
#define tgamma            // TODO
#define trunc             // TODO
#define ufromfp           // TODO
#define ufromfpx          // TODO
*/
#endif //KONPU_OPTION_TGMATH

//------------------------------------------------------------------------------
// <threads.h> / <stdatomic.h> - concurrency support
//
// As of now, Konpu does NOT support concurrency and thus defines almost nothing
// from those headers. This may change later. Also later, Konpu may define
// "coroutines" (which can look a lot like threads).
//
// see: https://en.cppreference.com/w/c/thread.html
//      https://en.cppreference.com/w/c/header/threads.html
//      https://en.cppreference.com/w/c/header/stdatomic.html
//------------------------------------------------------------------------------
#if __STDC_VERSION__ < 202311L
// As of today there's no need to use thread_local in Konpu.
// (but adding it anyway as it is unconditionally a C keyword)
#  define thread_local _Thread_local
#endif
#ifdef KONPU_H_
#  undef  __STDC_NO_THREADS__
#  define __STDC_NO_THREADS__  KONPU_C_DOES_NOT_SUPPORT_CONCURRENCY_
#  undef  __STDC_NO_ATOMICS__
#  define __STDC_NO_ATOMICS__  KONPU_C_DOES_NOT_SUPPORT_CONCURRENCY_
#endif

//------------------------------------------------------------------------------
// <stdcountof.h> (future C standard beyond C23) - countof macro/keyword
//
// Future C2Y standard may include the `_Countof` keyword and `countof` as a
// macro (before some future C revision would make it a real keyword)
//------------------------------------------------------------------------------

// Return (as a size_t) the number of elements within an array type.
// Warning: Only works for arrays (which have NOT decayed to a pointer)
#define countof(array)      (sizeof(array) / sizeof(0[(array)]))
// ^-- Maybe TODO later, but as of now, this doesn't detect if something else
//     than an array is passed to it, so be careful.


//------------------------------------------------------------------------------
// Signed vs Unsigned Integers in ilo Konpu
//------------------------------------------------------------------------------
// Konpu prefers to use signed integers for indices, sizes, and quantities.
// Unsigned integers are used for performing bits operations. This approach
// helps to prevent the subtle bugs that may occur during mixed unsigned-signed
// calculations and comparisons.
//
// As Konpu sizes and indices are not meant to exceed 1048575 (2^20-1), they are
// usually represented as `int32_t` or when a quantity (such as coordinates) is
// not meant to go over 32767, they might be represented as `int`.
//------------------------------------------------------------------------------

// Similar to the `sizeof` operator, but return a signed result (`int32_t`)
#define C_SIZEOF(type_or_expression)       ((int32_t)sizeof(type_or_expression))

// Similar to the `offsetof` operator, but return a signed result (`int32_t`)
#define C_OFFSETOF(T, Member)                     ((int32_t)offsetof(T, Member))

// Similar to the `alignof` operator, but return a signed result (`int32_t`)
#define C_ALIGNOF(T)                                       ((int32_t)alignof(T))

// same as `countof`, but returns a signed result (`int32_t`)
#define C_COUNTOF(array)                               ((int32_t)countof(array))


// Cast a signed integer to a `size_t`
// Note: Konpu prefers to use signed sizes, so this is specifically for when
//       there's a need to interact with outside parts which expect a `size_t`
#define UTIL_UNSIGNED_SIZE(signed_integer)  ((size_t)(1? (signed_integer) : -1))

// Cast a `size_t` to a Konpu size (`int32_t`)
#define UTIL_SIGNED_SIZE(size)       (_Generic((size), size_t: (int32_t)(size)))


#endif //include guard
