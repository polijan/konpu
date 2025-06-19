#include "error.h"
#include "platform_.h"


/*
#include <errno.h>
#include <string.h>

#if KONPU_PLATFORM_POSIX
#  include <dlfcn.h>
#endif
#if KONPU_PLATFORM_WINDOW
#  include <windows.h>
#endif
#if KONPU_PLATFORM_SDL2
#   include <SDL2/SDL.h>
#endif
*/

#include "literals.h"



// maybe we could have all the strings contiguously
// and do a linear search to find a
//
// another alt is to have 64 errors (8chars
static const char* error_message_[] = {
   // Special Error Message
   [0]  = ALE""LI""PONA" (No Error)",
   [1]  = "pakala a. ilo ante li pana e ona"" (Unknown Platform Error)",
   [2]  = "pakala a. mi sona ala e tan ona""(Unkown Error)",
   // We will use those for something else in the ROM
   [3]  = "",
   [4]  = "",
   [5]  = "",
   [6]  = "",
   [7]  = "",
   [8]  = "",
   [9]  = "",
   [10] = "",
   [11] = "",
   [12] = "",
   [13] = "",
   [14] = "",
   [15] = "",
   // Builtin error message >= 16
   [KONPU_ERROR_STACK_NOT_ENOUGH_ARGUMENTS] = "ijo namako li wile tawa pali ni (Not Enough Arguments)",
   [KONPU_ERROR_STACK_OVERFLOW] = ILO""WASO""LA""MUTE""IJO""LI""KAMA""SULI""IKE" (Stack Overflow)",

   //[KONPU_ERROR_HEAP_ALLOCATION] = "sona Konpu la mute ijo li kama suli (Heap Allocation Failure)",
};

const char *ErrorMessage(Error error)
{
   if (error <= KONPU_ERROR_PLATFORM_F) {
      switch (error) {
         case 0: return error_message_[0];
#if KONPU_PLATFORM_LIBC || KONPU_PLATFORM_POSIX
         case KONPU_ERROR_PLATFORM_C: return strerror(errno);
#endif
#if KONPU_PLATFORM_POSIX
         // TODO: dlerror returns the LAST dl error message, not something
         //       dependent on `error`
         case KONPU_ERROR_PLATFORM_A: return dlerror();
#endif
#if KONPU_PLATFORM_SDL2 || KONPU_PLATFORM_SDL3
         case KONPU_ERROR_PLATFORM_2: return SDL_GetError();
#endif
         default: return error_message_[1];
      }
   } else if (error < KONPU_ERROR_BUILTIN_COUNT_) {
      return error_message_[error];
//   } else if() {
//      error registry
//   }
   } else {
      return error_message_[2];
   }
}
