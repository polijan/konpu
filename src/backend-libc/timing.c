#include "core84/timing.h"
#include "backend-common/backend.h"
#include <time.h>

//------------------------------------------------------------------------------
// Implementation of Backend time functions
//------------------------------------------------------------------------------

static clock_t start;

int BackendInitTime()  { start = clock(); return 0; }
int BackendDropTime()  { return 0; }

//------------------------------------------------------------------------------
// Implementation of the Core's Time*() functions
//------------------------------------------------------------------------------

#ifndef __STDC_NO_THREADS__
#  include <threads.h>
   // Optional thread API (starting in C11) allows a portable implementation
   void TimeSleep(int ms) {
      if (ms <= 0) return;
      struct timespec ts = { .tv_sec  =  ms / 1000,
                             .tv_nsec = (ms % 1000) * 1000000L };
      thrd_sleep(&ts, NULL);
   }
#elif
#  if (__STDC_VERSION__ >= 202311L) || defined(__GNUC__)
#     warning "This C implementation has no threads"
#  endif
   void TimeSleep(int ms)  { (void)ms; }
#endif

int64_t Time(void)
{
   // Note: `clock_t` is defined as an arithmetic type in C99, but should be a
   //       real type starting from C11. On today's OS implementations, it still
   //       could be an integer value. Handling clock_t values with a conversion
   //       to double to handle that situation.
   return (double)(clock() - start) / CLOCKS_PER_SEC * 1000;
}
