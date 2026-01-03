#include "core84/timing.h"
#include "backend-common/backend.h"

// Backend Includes
#include "platform_.h"

//------------------------------------------------------------------------------
// Implementation of Backend time functions
//------------------------------------------------------------------------------

int BackendInitTime(void)  { return 0; }
int BackendDropTime(void)  { return 0; }

//------------------------------------------------------------------------------
// Implementation of the Core's Time*() functions
//------------------------------------------------------------------------------

void TimeSleep(int ms) { if (ms > 0) SDL_Delay(ms); }
   // works for SDL1,2,3

// In SDL3           : GetTicks() returns a 64 bit value in SDL3()
// In SDL2(>= 2.0.18): GetTicks64() does that too, so it is used-
// Prior to that: GetTicks() is sued but return 32-bits, which means it wraps
//                around every ~49 days (which should be acceptable).
#if KONPU_PLATFORM_SDL2 && (SDL_MINOR_VERSION > 0 || SDL_PATCHLEVEL >= 18)
   int64_t Time(void)  { return SDL_GetTicks64(); }
#else
   int64_t Time(void)  { return SDL_GetTicks(); }
#endif




#if 0 // TODO: REMOVE



//------------------------------------------------------------------------------
// Implementation of Backend time functions
//------------------------------------------------------------------------------

#if   KONPU_PLATFORM_SDL_ANY
   void BackendInitTime(void) {}
#elif KONPU_PLATFORM_LIBC
   // Even if POSIX or Windows or other systems offer finer time control, let's
   // just use `clock()` from standard C in Konpu.
#  include <time.h>
   static clock_t start;
   void BackendInitTime()  { start = clock(); }
#else
   void BackendInitTime(void) {}
#endif

//------------------------------------------------------------------------------
// Implementation of the Core's Time*() functions
//------------------------------------------------------------------------------

// Core's TimeSleep()
#if   KONPU_PLATFORM_SDL_ANY
   void TimeSleep(int ms) { if (ms > 0) SDL_Delay(ms); }
#elif KONPU_PLATFORM_WINDOWS
   void TimeSleep(int ms) { if (ms > 0) Sleep(ms); }
#elif KONPU_PLATFORM_POSIX
#  if _POSIX_C_SOURCE >= 199309L
#     include <time.h>
#     include <threads.h>
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

// Core's Time()
#if KONPU_PLATFORM_SDL_ANY
   // In SDL3           : GetTicks() returns a 64 bit value in SDL3()
   // In SDL2(>= 2.0.18): GetTicks64() does that too, so it is used-
   // Prior to that: GetTicks() is sued but return 32-bits, which means it wraps
   //                around every ~49 days (which should be acceptable).
#  if KONPU_PLATFORM_SDL2 && (SDL_MINOR_VERSION > 0 || SDL_PATCHLEVEL >= 18)
      int64_t Time(void)  { return SDL_GetTicks64(); }
#  else
      int64_t Time(void)  { return SDL_GetTicks(); }
#  endif
#elif KONPU_PLATFORM_LIBC
   // Even if POSIX or Windows or other systems offer finer time control, let's
   // just use `clock()` from standard C in Konpu.
   int64_t Time(void) {
      // `clock_t` has an integer type in C99, but a floating point type in C11.
      // And in Posix/Unix systems, it might be either. Handling clock_t values
      // with floating point operations should be the way to handle that "mess".
      return ((double)(clock() - start)) / CLOCKS_PER_SEC * 1000;
   }
#else
   int64_t Time(void)  { return -1; }
#endif

#endif
