// Include system header files
// This file should only be included internally by Konpu's project .c files,
// as we do not wish to expose platforms functions in the Konpu API.

#ifndef KONPU_PLATFORM__H_
#define KONPU_PLATFORM__H_
#ifdef  KONPU_H_
#   error "This must not be included from konpu.h"
#endif
#include "platform.h"

//------------------------------------------------------------------------------
// POSIX platform
//------------------------------------------------------------------------------
// * We set up various "feature tests macros" that control what definitions are
//   being exposed. If POSIX platform is selected, then Konpu will ASSUME
//   POSIX.1-2001 as the minimum interface, and possibly POSIX.1-2008.
// * Does not include POSIX-specific header directly.
//------------------------------------------------------------------------------
#if KONPU_PLATFORM_POSIX
    // _POSIX_SOURCE: was used to enable POSIX.1 definitions.
    // It appears obsolete when _POSIX_C_SOURCE is defined > 0.
    // We define it anyway, just in case.
#   ifndef _POSIX_SOURCE
#      define _POSIX_SOURCE        1
#   endif

    // _POSIX_C_SOURCE macro: may specify/force      // Edition Year  Macro
    // conformance to POSIX. Format is a date        //    1    1988  (N/A)
    // corresponding to a revision of the standard.  //    2    1990  1
#   ifndef _POSIX_C_SOURCE                           //    3    1992  2
#      define _POSIX_C_SOURCE      200809L           //    4    1993  199309L
#   endif                                            //    5    1995  199506L
    // ensure minimum value:                         //    6    2001  200112L
#   if _POSIX_C_SOURCE < 200112L                     //    7    2008  200809L
#      undef     _POSIX_C_SOURCE                     //    8    2024  202405L
#      define    _POSIX_C_SOURCE   200112L           //         ...
#   endif

    // _XOPEN_SOURCE macro: may specify/force conformance to XSI (X/Open System
    // Interfaces Extension), which includes POSIX.
#   ifndef _XOPEN_SOURCE
#      define _XOPEN_SOURCE        700  // X/Open 7 (includes POSIX 2008)
#   endif
    // ensure minimum value:
#   if _XOPEN_SOURCE < 600              // X/Open 5 (includes POSIX 2004, ie:
#      undef     _XOPEN_SOURCE          //      a minor update of POSIX 2001)
#      define    _XOPEN_SOURCE     600
#   endif

    // POSIX-specific includes used in Konpu:
#   include <dlfcn.h>     // dynamic linking of shared libraries (-ldl)

#endif


//------------------------------------------------------------------------------
// Standard C library includes
//------------------------------------------------------------------------------
#if KONPU_PLATFORM_LIBC
#   include <errno.h>     // macros reporting error conditions
#   include <string.h>    // string handling + memory functions
#endif

/*
#   include <stdio.h>     // input/output


#   ifdef KONPU_PLATFORM_LIBC_WANT_STDLIB_
#      include <stdlib.h> // general utilities and allocation functions
#   endif
#elif __STDC_VERSION__ >= 202311L
//  <string.h> is a *partially* freestanding header since C23.
//  In freestanding mode, the following functions are NOT required:
//  strdup, strndup, strcoll, strxfrm, strtok, and strerror.
#   include <string.h>
#   undef  strtok // perhaps we should forbid it all the time.
#   define strtok(s, delim)  KONPU_PLATFORM_strtok_should_not_be_used_
#   undef  strerror
#   define strerror(errnum)  KONPU_PLATFORM_strerror_should_not_be_used_
//  <stdlib.h> is also a (very) *partially* freestanding header since C23,
//  see: https://en.cppreference.com/w/c/language/conformance.
//  But for now, we will not include it anyway.
#endif
// Remove some more <string.h> functionality which might be unavailable in
// freestanding mode and which we shouldn't use anyway because they're related
// to locales or obviously perform some allocations.
#undef  strdup
#undef  strdnup
#undef  strcoll
#undef  strxfrm
#define strdup(str)          KONPU_PLATFORM_strdup_should_not_be_used_
#define strndup(str, n)      KONPU_PLATFORM_strndup_should_not_be_used_
#define strcoll(str1, str2)  KONPU_PLATFORM_strcoll_should_not_be_used_
#define strxfrm(dst, src, n) KONPU_PLATFORM_strxfrm_should_not_be_used_
// Prevent accidental invocation of <stdlib.h>'s allocation functions
#undef  malloc
#undef  calloc
#undef  realloc
#undef  free
#undef  aligned_alloc
#undef  free_sized
#undef  free_aligned_sized
#define malloc(sz)           KONPU_PLATFORM_malloc_should_not_be_used_
#define calloc(n, sz)        KONPU_PLATFORM_calloc_should_not_be_used_
#define realloc(ptr, sz)     KONPU_PLATFORM_realloc_should_not_be_used_
#define free(ptr)            KONPU_PLATFORM_free_should_not_be_used_
#define aligned_alloc(a, sz) KONPU_PLATFORM_aligned_alloc_should_not_be_used_
#define free_sized(ptr, sz)  KONPU_PLATFORM_free_sized_should_not_be_used_
#define free_aligned_sized(ptr, a, sz) \
   KONPU_PLATFORM_free_aligned_sized_should_not_be_used_
*/

//------------------------------------------------------------------------------
// Includes for SDL platform
//------------------------------------------------------------------------------
#if KONPU_PLATFORM_SDL2
#   define KONPU_PLATFORM_SDL_NAME "SDL2"
//  Normally, the file to include is "SDL.h" (when the project is built with
//  `sdl2-config --libs` or `pkg-config sdl2 --libs`). But the header is often
//  located in /usr/include/SDL2/SDL.h and thus also often included as
//  <SDL2/SDL.h>, thus we try to include it if the main include fails (we use
//  `__has_include` which is present in many compilers and is required by C23).
#   if defined __has_include
#      if __has_include ("SDL.h")
#         include "SDL.h"
#      elif __has_include (<SDL2/SDL.h>)
#         include <SDL2/SDL.h>
#      else
#         error "Cannot find the header for SDL2"
#      endif
#   else
#     include "SDL.h"
#   endif
#elif KONPU_PLATFORM_SDL3
#   define KONPU_PLATFORM_SDL_NAME "SDL3"
#   if defined __has_include
#      if __has_include ("SDL.h")
#         include "SDL.h"
#      elif __has_include (<SDL3/SDL.h>)
#         include <SDL3/SDL.h>
#      else
#         error "Cannot find the header for SDL3"
#      endif
#   else
#     include "SDL.h"
#   endif
#elif KONPU_PLATFORM_SDL1
#   define KONPU_PLATFORM_SDL_NAME "SDL1"
#   if defined __has_include
#      if __has_include ("SDL.h")
#         include "SDL.h"
#      elif __has_include (<SDL/SDL.h>)
#         include <SDL/SDL.h>
#      else
#         error "Cannot find the header for SDL1.2"
#      endif
#   else
#     include "SDL.h"
#   endif
#endif //KONPU_PLATFORM_SDL*


//------------------------------------------------------------------------------
// Includes for Windows platform
//------------------------------------------------------------------------------
#if KONPU_PLATFORM_WINDOWS
#   define  WIN32_LEAN_AND_MEAN // <--  prevent the inclusion of many APIs
#   include <windows.h>
#endif


#endif //include guard
