#include "time.h"

// TimeSleep()
#include "platform_.h"
#if   KONPU_PLATFORM_SDL1 || KONPU_PLATFORM_SDL2 || KONPU_PLATFORM_SDL3
   void TimeSleep(int ms) { if (ms > 0) SDL_Delay(ms); }
#elif KONPU_PLATFORM_WINDOWS
   void TimeSleep(int ms) { if (ms > 0) Sleep(ms); }
#elif KONPU_PLATFORM_POSIX
#  if _POSIX_C_SOURCE >= 199309L
#     include <time.h>
      void TimeSleep(int ms) {
         if (ms <= 0) return;
         struct timespec ts = { .tv_sec  =  ms / 1000,
                                .tv_nsec = (ms % 1000) * 1000000L };
         nanosleep(&ts, NULL);
      }
#  else
#     include <unistd.h>
      void TimeSleep(int ms) { if (ms > 0) usleep(ms * 1000); }
#  endif
#elif KONPU_PLATFORM_LIBC && \
      (__STDC_VERSION__ >= 201112L) && !defined(__STDC_NO_THREADS__)
   // Optional thread API in Standard C11 allows a portable implementation
   void TimeSleep(int ms) {
      if (ms <= 0) return;
      struct timespec ts = { .tv_sec  =  ms / 1000,
                             .tv_nsec = (ms % 1000) * 1000000L };
      thrd_sleep(&ts, NULL);
   }
#else
   void TimeSleep(int ms) { (void)ms; }
#endif

// TimeTicks() and TimeInit()
#include "init.h"
#if   KONPU_PLATFORM_SDL3
   void    TimeInit(void)  {}
   int64_t TimeTicks(void) { return SDL_GetTicks(); }
#elif KONPU_PLATFORM_SDL2 && (SDL_MINOR_VERSION > 0 || SDL_PATCHLEVEL >= 18)
   // SDL 2.0.18 has GetTicks64(), but prior versions do not.
   void    TimeInit(void)  {}
   int64_t TimeTicks(void) { return SDL_GetTicks64(); }
#elif KONPU_PLATFORM_LIBC
   // Even if POSIX or Windows or other systems offer finer time control, let's
   // just use `clock()` from standard C in Konpu.
#  include <time.h>
   static clock_t start;
   void    TimeInit()      { start = clock(); }
   int64_t TimeTicks(void) {
      // `clock_t` has an integer type in C99, but a floating point type in C11.
      // And in Posix/Unix systems, it might be either. Handling clock_t values
      // with floating point operations should be the way to handle that "mess".
      return ((double)(clock() - start)) / CLOCKS_PER_SEC * 1000;
   }
#elif KONPU_PLATFORM_SDL1 || KONPU_PLATFORM_SDL2
   // SDL1 (and early versions of SDL2) only returns a uint32_t value (although
   // Konpu will return it as a int64_t), which means it will wrap around every
   // ~49 days. Konpu should hopefully not be run for that long.
   void    TimeInit(void)  {}
   int64_t TimeTicks(void) { return SDL_GetTicks(); }
#else
   void    TimeInit(void)  {}
   int64_t TimeTicks(void) { return -1; }
#endif
