//------------------------------------------------------------------------------
// Implementations "inherited" from backend-common or backend-libc
//------------------------------------------------------------------------------

#include "platform_.h"
#define  OPTIONS_STRCHR(string, c)                  SDL_strchr((string), (c))
#define  OPTIONS_STRNCMP(string1, string2, maxlen)  SDL_strncmp((string1), (string2), (maxlen))
#include "backend-common/options.c"

// TODO later: avoid libc?
#include "backend-libc/printer.c"

#include "backend-libc/main.c"
#if KONPU_PLATFORM_LIBC && !(KONPU_PLATFORM_POSIX)
   #include <stdlib.h>
   int main(int argc, char *argv[])
   { return BackendMain(argc,argv) ? EXIT_FAILURE : EXIT_SUCCESS; }
#else
   int main(int argc, char *argv[])
   { return BackendMain(argc,argv); }
#endif
