#include "random.h"
#include "ram.h"
#include "rom.h"
#include "var.h"
#include "platform_.h"
#if KONPU_PLATFORM_LIBC
#   include <stdlib.h>
#   include <time.h>  // C's <time.h>
#else
#   include "time.h"  // Konpu's "time.h"
#endif
#include "init.h"


#if KONPU_PLATFORM_LIBC
   uint64_t Random(void)
   { return VarHash((var){.uint64 = rand()}); }

   void RandomInit(uint64_t seed)
   { srand(seed % UINT_MAX); }
#else
#  error "TODO: Konpu needs its own RNG"
#endif

void RandomInitAsRandom(uint64_t whatever)
{
   whatever +=
      // Add some pointer values that may vary in Konpu
         (uint64_t)(uintptr_t)ROM + (uint64_t)(uintptr_t)RAM
      // Then add CONSTANTS that comes from our config/build
      +  (uint64_t)__STDC_VERSION__
      +  KONPU_VERSION_PATCH +
      +  (KONPU_PLATFORM_SDL1 + KONPU_PLATFORM_SDL2 + KONPU_PLATFORM_SDL3) * 4
      +  KONPU_PLATFORM_POSIX * 9 + KONPU_PLATFORM_WINDOWS * 11;
#ifdef NDEBUG
   whatever += 7;
#endif
#ifdef __GNUC__
   whatever += 32;
#endif
#ifdef __clang__
   whatever += 64;
#endif
#ifdef _MSC_VER
   whatever += 128;
#endif
#ifdef SDL_PATCHLEVEL
   whatever += SDL_PATCHLEVEL;
#endif
#ifdef __GNUC_MINOR__
   whatever += __GNUC_MINOR__;
#endif

   // Finally add Runtime / Time stuff
#if KONPU_PLATFORM_LIBC
   whatever += (uint64_t)(time(NULL));
#else
   whatever += TimeTicks();
#endif

   // Initialize from a hash value...
   RandomInit( VarHash((var){.uint64 = whatever}) );
}