// Ensure values for the KONPU_PLATFORM_* feature macros

#ifndef  KONPU_PLATFORM_H
#define  KONPU_PLATFORM_H
#include "config.h"


// Ensure a value for KONPU_PLATFORM_SDL2 (0 or 1)
// - Step 1) Normalize (i.e. set to 1 if the symbol is defined but has no value)
#if defined(KONPU_PLATFORM_SDL2) && ~(~KONPU_PLATFORM_SDL2 + 0) == 0 \
                                 && ~(~KONPU_PLATFORM_SDL2 + 1) == 1
#   undef  KONPU_PLATFORM_SDL2
#   define KONPU_PLATFORM_SDL2        1
#endif
// - Step 2) If a value was provided, check its validity
#if defined(KONPU_PLATFORM_SDL2) && (KONPU_PLATFORM_SDL2 != 0) && (KONPU_PLATFORM_SDL2 != 1)
#   error("KONPU_PLATFORM_SDL2 defined but not set to 0 or 1")
#endif
// - Step 3) Set up a default value if none was provided (default: 0)
#ifndef KONPU_PLATFORM_SDL2
#   define KONPU_PLATFORM_SDL2        0
#endif


// Ensure a value for KONPU_PLATFORM_POSIX (0 [=disabled] or >0) 
// - Step 1) Normalize (i.e. set to 1 if the symbol is defined but has no value)
#if defined(KONPU_PLATFORM_POSIX) && ~(~KONPU_PLATFORM_POSIX + 0) == 0 \
                                  && ~(~KONPU_PLATFORM_POSIX + 1) == 1
#   undef  KONPU_PLATFORM_POSIX
#   define KONPU_PLATFORM_POSIX       1
#endif
// - Step 2) If a value was provided, check its validity
//           Also check validity for _POSIX_C_SOURCE, if defined.
#if defined(KONPU_PLATFORM_POSIX) && (KONPU_PLATFORM_POSIX < 0)
#   error("invalid KONPU_PLATFORM_POSIX value (is < 0)")
#endif
#if defined(_POSIX_C_SOURCE) && ~(~_POSIX_C_SOURCE + 0) == 0 \
                             && ~(~_POSIX_C_SOURCE + 1) == 1
#   error("_POSIX_C_SOURCE is defined but has no value")
#endif
#if defined(_POSIX_C_SOURCE) && (_POSIX_C_SOURCE < 1)
#   error("invalid _POSIX_C_SOURCE value (is < 1)")
#endif
// - Step 3) Set up a default value if none was provided
//           (0 if SDL2 is enabled, or _POSIX_C_SOURCE if defined, otherwise 0)
#ifndef KONPU_PLATFORM_POSIX
#   if KONPU_PLATFORM_SDL2
#      define KONPU_PLATFORM_POSIX    0
#   elif defined(_POSIX_C_SOURCE)
#      define KONPU_PLATFORM_POSIX    _POSIX_C_SOURCE
#   else
#      define KONPU_PLATFORM_POSIX    0
#   endif
#endif


// Ensure a value for KONPU_PLATFORM_WINDOWS (0 or 1)
// - Step 1) Normalize (i.e. set to 1 if the symbol is defined but has no value)
#if defined(KONPU_PLATFORM_WINDOWS) && ~(~KONPU_PLATFORM_WINDOWS + 0) == 0 \
                                    && ~(~KONPU_PLATFORM_WINDOWS + 1) == 1
#   undef  KONPU_PLATFORM_WINDOWS
#   define KONPU_PLATFORM_WINDOWS     1
#endif
// - Step 2) If a value was provided, check its validity
#if defined(KONPU_PLATFORM_WINDOWS) && (KONPU_PLATFORM_WINDOWS != 0) \
                                    && (KONPU_PLATFORM_WINDOWS != 1)
#   error("KONPU_PLATFORM_WINDOWS defined but not set to 0 or 1")
#endif
// - Step 3) Set up a default value if none was provided (0 if SDL2 is enabled,
//                      otherwise 1 if we can auto-detect Windows, otherwise 0)
#ifndef KONPU_PLATFORM_WINDOWS
#   if KONPU_PLATFORM_SDL2
#      define KONPU_PLATFORM_WINDOWS  0
#   elif defined(_WIN32) && !defined(__CYGWIN__)
#      define KONPU_PLATFORM_WINDOWS  1
#   else
#      define KONPU_PLATFORM_WINDOWS  0
#   endif
#endif


// Ensure a value for KONPU_PLATFORM_LIBC (0 or 1)
// - Step 1) Normalize (i.e. set to 1 if the symbol is defined but has no value)
#if defined(KONPU_PLATFORM_LIBC) && ~(~KONPU_PLATFORM_LIBC + 0) == 0 \
                                 && ~(~KONPU_PLATFORM_LIBC + 1) == 1
#   undef  KONPU_PLATFORM_LIBC
#   define KONPU_PLATFORM_LIBC        1
#endif
// - Step 2) If a value was provided, check its validity
#if defined(KONPU_PLATFORM_LIBC) && (KONPU_PLATFORM_LIBC != 0) \
                                 && (KONPU_PLATFORM_LIBC != 1)
#   error("KONPU_PLATFORM_LIBC defined but not set to 0 or 1")
#endif
// - Step 3) Set up a default value if none was provided
//           (1 if POSIX or WINDOWS platform is enabled, otherwise 0)
#ifndef KONPU_PLATFORM_LIBC
#   if KONPU_PLATFORM_POSIX || KONPU_PLATFORM_WINDOWS
#      define KONPU_PLATFORM_LIBC     1
#   else
#      define KONPU_PLATFORM_LIBC     0
#   endif
#endif


#endif //KONPU_PLATFORM_H