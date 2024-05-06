// Include system header files
// This file should only be included by .c files (implementations) and not by
// header files as we do not wish to expose the systems' APIs to Konpu's users.

#ifndef  KONPU_PLATFORM_INCLUDE_H
#define  KONPU_PLATFORM_INCLUDE_H
#include "c.h"


//-- POSIX platform ------------------------------------------------------------
// No header includes specific to the C POSIX library (the .c file would do it
// as needed). But we set up various "feature tests macros" that control what
// definitions are being exposed. If POSIX platform is selected, then Konpu
// ASSUMEs POSIX.1-2001 as the older minimum interface, or POSIX.1-2008 as the
// default.

#if KONPU_PLATFORM_POSIX
    // _POSIX_SOURCE: was used to enable POSIX.1 definitions
    // It appears obsolete when _POSIX_C_SOURCE is defined > 0.
    // We'll define it anyway just in case.
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
#      undef     _POSIX_C_SOURCE
#      define    _POSIX_C_SOURCE   200112L
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
#endif
//-- End POSIX platform --------------------------------------------------------


// Includes for Windows platform
#if KONPU_PLATFORM_WINDOWS
#   define  WIN32_LEAN_AND_MEAN // <--  exclude inclusion of many APIs
#   include <windows.h>
#endif


// Includes for SDL2 platform
#if KONPU_PLATFORM_SDL2
#   include <SDL2/SDL.h>  // might need to be: #include "SDL.h"
#endif


// "Basic" C library includes
#if KONPU_PLATFORM_LIBC
#   include <errno.h>     // macros reporting error conditions
#   include <string.h>    // string handling + memory functions
#   include <stdio.h>     // input/output
#   include <inttypes.h>  // format conversion of integer types
#   include <stdlib.h>    // general utilities, malloc & co...
    //  Disable stdlib's malloc:
#   if defined(KONPU_HEAP_H) && !defined(KONPU_SETTING_NO_STDLIB_INTERFACE)
#      define malloc(sz)        static_assert("stdlib malloc disabled" , false);
#      define calloc(n, sz)     static_assert("stdlib calloc disabled" , false);
#      define realloc(ptr, sz)  static_assert("stdlib realloc disabled", false);
#      define free(ptr)         static_assert("stdlib free disabled"   , false);
#   endif
#endif


#endif //KONPU_PLATFORM_INCLUDE_H
