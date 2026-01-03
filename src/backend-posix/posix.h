#ifndef  BACKEND_PLATFORM_POSIX_H_
#define  BACKEND_PLATFORM_POSIX_H_

// Konpu backend can use Posix
#undef  KONPU_PLATFORM_POSIX
#define KONPU_PLATFORM_POSIX  1

#undef  KONPU_PLATFORM_LIBC
#define KONPU_PLATFORM_LIBC   1

//------------------------------------------------------------------------------
// POSIX platform
//------------------------------------------------------------------------------
// Set up various "feature tests macros" that control what definitions are
// exposed in headers. If POSIX platform is selected, then Konpu ASSUMEs
// POSIX.1-2001 as the minimum interface, and possibly POSIX.1-2008.
//------------------------------------------------------------------------------

// _POSIX_SOURCE macro: used to enable POSIX.1 definitions.
// Obsolete when _POSIX_C_SOURCE is defined > 0, but define it just in case.
#undef  _POSIX_SOURCE
#define _POSIX_SOURCE  1

// _POSIX_C_SOURCE macro: specify/request conformance to POSIX.
// Format is a date corresponding to a revision of the standard.
//
// Edition Year  Macro
//    1    1988  (N/A)
//    2    1990  1
//    3    1992  2
//    4    1993  199309L
//    5    1995  199506L
//    6    2001  200112L
//    7    2008  200809L
//    8    2024  202405L
//         ...
#if !defined(_POSIX_C_SOURCE) || (~(~_POSIX_C_SOURCE + 0) == 0 && ~(~_POSIX_C_SOURCE + 1) == 1)
#  define _POSIX_C_SOURCE   200809L
#elif     _POSIX_C_SOURCE < 200112L
#  undef  _POSIX_C_SOURCE
#  define _POSIX_C_SOURCE   200112L
#endif

// _XOPEN_SOURCE macro: specify/request conformance to XSI (X/Open System
// Interfaces Extension), which includes POSIX.
#if !defined(_XOPEN_SOURCE) || (~(~_XOPEN_SOURCE + 0) == 0 && ~(~_XOPEN_SOURCE + 1) == 1)
#  define _XOPEN_SOURCE   700   // 700 = X/Open 7 (includes POSIX 2008)
#elif     _XOPEN_SOURCE < 600   // 600 = X/Open 5 (includes POSIX 2004, ie:
#  undef  _XOPEN_SOURCE         //            a minor update of POSIX 2001)
#  define _XOPEN_SOURCE   600
#endif

#endif //include guard
