#ifndef  KONPU_PLATFORM_H
#define  KONPU_PLATFORM_H
#include "config.h"

//------------------------------------------------------------------------------
// KONPU_VERSION_*
// Note: It is also available in Konpu's ROM
//
// Ideally, the idea is for Konpu's version to follow the classic "semantic
// version" scheme (although there might be some exceptions... especially as
// long as both MAJOR and MINOR version are still 0)
//
// - MAJOR version: incompatible API changes
// - MINOR version: added functionality in a backward compatible manner
// - PATCH version: backward compatible fixes
//------------------------------------------------------------------------------

// Same as KONPU_VERSION_MAJOR, but it is constant
// (thus it could be used in the preprocessor)
#define KONPU_VERSION_MAJOR     0

// Same as KONPU_VERSION_MINOR, but it is constant
// (thus it could be used in the preprocessor)
#define KONPU_VERSION_MINOR     0

// Same as KONPU_VERSION_PATCH, but it is constant
// (thus it could be used in the preprocessor)
#define KONPU_VERSION_PATCH     1

//------------------------------------------------------------------------------
// Ensure values for the KONPU_PLATFORM_* feature macros
//------------------------------------------------------------------------------
// Eventually, we'd want to support
// - SDL2 and SDL3 (libsdl.org) for native desktop and mobile.
//   When using SDL3, no other platform will be used.
//   When using SDL2,
//
// - EMSCRIPTEN for The web and WASM runtimes  (use either SDL compiled to WASM
//              or the EmScripten Javascript reimplementation of the SDL1.2 API)
// - POSIX / Linux when SDL is not available
// - perhaps Windows (maybe/someday but not a priority)
//------------------------------------------------------------------------------

// Ensure a value for KONPU_PLATFORM_SDL3 (0 or 1)
// - Step 1) Normalize (i.e. set to 1 if the symbol is defined but has no value)
#if defined(KONPU_PLATFORM_SDL3) && ~(~KONPU_PLATFORM_SDL3 + 0) == 0 \
                                 && ~(~KONPU_PLATFORM_SDL3 + 1) == 1
#   undef  KONPU_PLATFORM_SDL3
#   define KONPU_PLATFORM_SDL3        1
#endif
// - Step 2) If a value was provided, check its validity
#if defined(KONPU_PLATFORM_SDL3) && (KONPU_PLATFORM_SDL3 != 0) && (KONPU_PLATFORM_SDL3 != 1)
#   error("KONPU_PLATFORM_SDL3 defined but not set to 0 or 1")
#endif
// - Step 3) Set up a default value if none was provided (default: 0)
#ifndef KONPU_PLATFORM_SDL3
#   define KONPU_PLATFORM_SDL3        0
#endif

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

// Ensure a value for KONPU_PLATFORM_SDL1 (0 or 1)
// - Step 1) Normalize (i.e. set to 1 if the symbol is defined but has no value)
#if defined(KONPU_PLATFORM_SDL1) && ~(~KONPU_PLATFORM_SDL1 + 0) == 0 \
                                 && ~(~KONPU_PLATFORM_SDL1 + 1) == 1
#   undef  KONPU_PLATFORM_SDL1
#   define KONPU_PLATFORM_SDL1        1
#endif
// - Step 2) If a value was provided, check its validity
#if defined(KONPU_PLATFORM_SDL1) && (KONPU_PLATFORM_SDL1 != 0) && (KONPU_PLATFORM_SDL1 != 1)
#   error("KONPU_PLATFORM_SDL2 defined but not set to 0 or 1")
#endif
// - Step 3) Set up a default value if none was provided (default: 0)
#ifndef KONPU_PLATFORM_SDL1
#   define KONPU_PLATFORM_SDL1        0
#endif

// KONPU_PLATFORM_SDL_ANY as a logical OR of all existing KONPU_PLATFORM_SDLx
#define KONPU_PLATFORM_SDL_ANY \
   (KONPU_PLATFORM_SDL3 || KONPU_PLATFORM_SDL2 || KONPU_PLATFORM_SDL1)


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
