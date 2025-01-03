#include "time.h"
#include "platform_include.h"

#define DEFINE  void TimeSleep(int ms)
#if   KONPU_PLATFORM_SDL1 || KONPU_PLATFORM_SDL2 || KONPU_PLATFORM_SDL3
   DEFINE { if (ms > 0) SDL_Delay(ms); }
#elif KONPU_PLATFORM_WINDOWS
   DEFINE { if (ms > 0) Sleep(ms); }
#elif KONPU_PLATFORM_POSIX
#  if _POSIX_C_SOURCE >= 199309L
#     include <time.h>
      DEFINE { if (ms > 0) usleep(ms * 1000); }
#  else
#     include <unistd.h>
      DEFINE {
         if (ms <= 0) return;
         struct timespec ts = { .tv_sec  =  ms / 1000,
                                .tv_nsec = (ms % 1000) * 1000000L };
         nanosleep(&ts, NULL);
      }
#  endif
#elif KONPU_PLATFORM_LIBC && \
      (__STDC_VERSION__ >= 201112L) && !defined(__STDC_NO_THREADS__)
   // Standard C thread API since C11 allows a sleep
   DEFINE {
      if (ms <= 0) return;
      struct timespec ts = { .tv_sec  =  ms / 1000,
                             .tv_nsec = (ms % 1000) * 1000000L };
      thrd_sleep(&ts, NULL);
   }
#else
   DEFINE { (void)ms; }
#endif
#undef DEFINE
