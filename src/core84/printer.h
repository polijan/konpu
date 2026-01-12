//TODO: `Printer` should become just a stream object and be manipulated with
//      stream/stdio functions

//------------------------------------------------------------------------------
// The Printer port (often just referred to as printer) is a fast interface
// through which ilo Konpu can send data out (to a printer or other device).
//------------------------------------------------------------------------------
#ifndef  KONPU_PRINTER_H_
#define  KONPU_PRINTER_H_
#include "c.h"

//------------------------------------------------------------------------------
// Printer port API - a single function similar to `printf`
//------------------------------------------------------------------------------

#if defined(__GNUC__)
   // This instructs GCC to perform checks for printf-like format, see:
   // https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#format
   __attribute__ ((format (printf, 1, 2)))
#endif
// Send data on the Printer interface as a `printf` like function.
// Upon success, return the number of bytes printed (excluding the null byte
// used to end output to strings). If an error is encountered, a negative
// value is returned.
int Printer(const char *restrict format, ...);

// TODO/MAYBE:
// rename Printer as :  int PrinterFormat(const char *format, ...);
// and also introduce:  int PrinterRaw(const void* data, size_t count);



//------------------------------------------------------------------------------
// KONPU_DEBUG (TODO / FIX ME)
//------------------------------------------------------------------------------

/*
#ifndef NDEBUG
#  define DEBUG(fmt, ...)                                       \
      Printer("%s:%d:%s(): " fmt, __FILE__, __LINE__, __func__  \
               UTIL_VA_OPT_COMMA(__VA_ARGS__) __VA_ARGS__)
#endif
*/


// UTIL_FIRST(...): Expands to its first argument.
#define UTIL_FIRST(...) \
        UTIL_FIRST_HELPER_(__VA_ARGS__, throw_away)
#       define UTIL_FIRST_HELPER_(first, ...) first

// UTIL_COMMA_REST(...): Expands to nothing if there's only ONE argument,
// Otherwise, expands to a comma followed by everything but the first argument.
// UTIL_COMMA(__VA_ARGS__) serves a similar purpose that __VA_OPT__(,) but
// without requiring C23. Works up to 50 arguments.
#define UTIL_COMMA_REST(...)   UTIL_OVERLOAD(UTIL_COMMA_REST, __VA_ARGS__)
#       define UTIL_COMMA_REST_1_(first)
#       define UTIL_COMMA_REST_2_(first, ...)   , __VA_ARGS__
#       define UTIL_COMMA_REST_3_(first, ...)   , __VA_ARGS__
#       define UTIL_COMMA_REST_4_(first, ...)   , __VA_ARGS__
#       define UTIL_COMMA_REST_5_(first, ...)   , __VA_ARGS__
#       define UTIL_COMMA_REST_6_(first, ...)   , __VA_ARGS__
#       define UTIL_COMMA_REST_7_(first, ...)   , __VA_ARGS__
#       define UTIL_COMMA_REST_8_(first, ...)   , __VA_ARGS__
#       define UTIL_COMMA_REST_9_(first, ...)   , __VA_ARGS__
#       define UTIL_COMMA_REST_10_(first, ...)  , __VA_ARGS__
#       define UTIL_COMMA_REST_11_(first, ...)  , __VA_ARGS__
#       define UTIL_COMMA_REST_12_(first, ...)  , __VA_ARGS__
#       define UTIL_COMMA_REST_13_(first, ...)  , __VA_ARGS__
#       define UTIL_COMMA_REST_14_(first, ...)  , __VA_ARGS__
#       define UTIL_COMMA_REST_15_(first, ...)  , __VA_ARGS__
#       define UTIL_COMMA_REST_16_(first, ...)  , __VA_ARGS__
#       define UTIL_COMMA_REST_17_(first, ...)  , __VA_ARGS__
#       define UTIL_COMMA_REST_18_(first, ...)  , __VA_ARGS__
#       define UTIL_COMMA_REST_19_(first, ...)  , __VA_ARGS__
#       define UTIL_COMMA_REST_20_(first, ...)  , __VA_ARGS__
#       define UTIL_COMMA_REST_21_(first, ...)  , __VA_ARGS__
#       define UTIL_COMMA_REST_22_(first, ...)  , __VA_ARGS__
#       define UTIL_COMMA_REST_23_(first, ...)  , __VA_ARGS__
#       define UTIL_COMMA_REST_24_(first, ...)  , __VA_ARGS__
#       define UTIL_COMMA_REST_25_(first, ...)  , __VA_ARGS__
#       define UTIL_COMMA_REST_26_(first, ...)  , __VA_ARGS__
#       define UTIL_COMMA_REST_27_(first, ...)  , __VA_ARGS__
#       define UTIL_COMMA_REST_28_(first, ...)  , __VA_ARGS__
#       define UTIL_COMMA_REST_29_(first, ...)  , __VA_ARGS__
#       define UTIL_COMMA_REST_30_(first, ...)  , __VA_ARGS__
#       define UTIL_COMMA_REST_31_(first, ...)  , __VA_ARGS__
#       define UTIL_COMMA_REST_32_(first, ...)  , __VA_ARGS__
#       define UTIL_COMMA_REST_33_(first, ...)  , __VA_ARGS__
#       define UTIL_COMMA_REST_34_(first, ...)  , __VA_ARGS__
#       define UTIL_COMMA_REST_35_(first, ...)  , __VA_ARGS__
#       define UTIL_COMMA_REST_36_(first, ...)  , __VA_ARGS__
#       define UTIL_COMMA_REST_37_(first, ...)  , __VA_ARGS__
#       define UTIL_COMMA_REST_38_(first, ...)  , __VA_ARGS__
#       define UTIL_COMMA_REST_39_(first, ...)  , __VA_ARGS__
#       define UTIL_COMMA_REST_40_(first, ...)  , __VA_ARGS__
#       define UTIL_COMMA_REST_41_(first, ...)  , __VA_ARGS__
#       define UTIL_COMMA_REST_42_(first, ...)  , __VA_ARGS__
#       define UTIL_COMMA_REST_43_(first, ...)  , __VA_ARGS__
#       define UTIL_COMMA_REST_44_(first, ...)  , __VA_ARGS__
#       define UTIL_COMMA_REST_45_(first, ...)  , __VA_ARGS__
#       define UTIL_COMMA_REST_46_(first, ...)  , __VA_ARGS__
#       define UTIL_COMMA_REST_47_(first, ...)  , __VA_ARGS__
#       define UTIL_COMMA_REST_48_(first, ...)  , __VA_ARGS__
#       define UTIL_COMMA_REST_49_(first, ...)  , __VA_ARGS__
#       define UTIL_COMMA_REST_50_(first, ...)  , __VA_ARGS__

// a DEBUG(fmt, ...) macro for fast and ugly debugging using the Printer.
// (Only active when NDEBUG isn't defined)
#ifndef NDEBUG
#   if __STDC_VERSION__ > 201710L
#      define DEBUG(fmt, ...)                       \
              Printer("%s:%d:%s(): " fmt,           \
                      __FILE__, __LINE__, __func__  \
                      __VA_OPT__(,) __VA_ARGS__)
#   else
#      define DEBUG(...)                                      \
              Printer("%s:%d:%s(): " UTIL_FIRST(__VA_ARGS__), \
                      __FILE__, __LINE__, __func__            \
                      UTIL_COMMA_REST(__VA_ARGS__))
#   endif
#else
#   define    DEBUG(...)
#endif


#endif //include guard
