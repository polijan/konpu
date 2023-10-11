/// @file provides a main function (for test purposes), unless NDEBUG is defined
#include "c.h"


// ISO C forbids an empty translation unit, so let's add something up...
static inline void konpu_empty_function_(void) { }

#ifndef NDEBUG
#   if KONPU_PLATFORM_LIBC
#      include <stdio.h>
#   endif //KONPU_PLATFORM_LIBC

int test_main(int argc, char* argv[])
{
   (void)argc; (void)argv;
#if KONPU_PLATFORM_LIBC
   printf("test_main: RETURNING 42\n");
#endif
   return 42;
}

//   KONPU_MAIN should expand to the name of a 'main()'-like function
#    ifndef KONPU_MAIN
#       define KONPU_MAIN  test_main // (default value if none provided)
#       if __STDC_VERSION__ >= 202311L || defined(__GNUC__)
#          warning "no MAIN was provided, using default value"
#       endif
#    endif //KONPU_MAIN
extern int KONPU_MAIN (int argc, char* argv[]);

int main(int argc, char* argv[])
{ return KONPU_MAIN (argc, argv); }

#endif //NDEBUG
