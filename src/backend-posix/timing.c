#include "backend-common/backend.h"
#include "core84/timing.h"
#include <time.h>

//------------------------------------------------------------------------------
// Implementation of Backend time functions
//------------------------------------------------------------------------------

static struct timespec start;

int BackendInitTime()  { return clock_gettime(CLOCK_MONOTONIC, &start); }
int BackendDropTime()  { return 0; }

//------------------------------------------------------------------------------
// Implementation of the Core's Time*() functions
//------------------------------------------------------------------------------

void TimeSleep(int ms)
{
   if (ms <= 0) return;
   struct timespec ts = {  .tv_sec  =  ms / 1000,
                           .tv_nsec = (ms % 1000) * 1000000L };
      nanosleep(&ts, NULL);
}

int64_t Time(void)
{
   struct timespec now;
   clock_gettime(CLOCK_MONOTONIC, &now);

   uint64_t sec  = now.tv_sec  - start.tv_sec;
   int64_t  nsec = now.tv_nsec - start.tv_nsec;
   if (nsec < 0) {
      sec--;
      nsec += 1000000000L;
   }

   return sec * 1000 + nsec / 1000000;
}
