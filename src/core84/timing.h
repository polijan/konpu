#ifndef  KONPU_TIME_H_
#define  KONPU_TIME_H_
#include "c.h"

// Return the number of milliseconds since the start of ilo Konpu,
// or a negative number if that information is unavailable.
int64_t Time(void);

// Try to do nothing for the specified number of milliseconds.
// Depending on the underlying platform:
// - Possiblity the delay might be longer (due to OS scheduling).
// - Possiblity this is not supported in which case it does nothing (unlikely)
// - Possiblity the delay would be shorter (unlikely)
void TimeSleep(int milliseconds);

// <time.h>-like interface
#ifdef KONPU_H_
#  define time_t          int64_t
#  define clock_t         double
#  define CLOCKS_PER_SEC  1000.0

   // time_t time(time_t *t);
   // Returns the time as the number of seconds since the start of ilo Konpu
   // (Konpu doesn't keep time in between runs and has no concept of an 'epoch')
   // If t is not a null pointer, store the time there.
#  define time(tptr_or_null)   c_time_(tptr_or_null)

   // clock_t clock(void);
   // Approximate processor time since the start of Konpu's
   // To convert to seconds, divide it by CLOCKS_PER_SEC.
#  define clock()    Time()

#endif
static inline int64_t c_time_(int64_t *t) {
   int64_t seconds = Time() / 1000;
   if (t) *t = seconds;
   return seconds;
}

#endif //include guard
