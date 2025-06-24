#ifndef  KONPU_TIME_H_
#define  KONPU_TIME_H_
#include "c.h"

// Try to do nothing for the specified number of milliseconds.
// Depending on the underlying platform:
// - Possiblity the delay might be longer (due to OS scheduling).
// - Possiblity this is not supported in which case it does nothing (unlikely)
// - Possiblity the delay would be shorter (unlikely)
void UtilSleep(int milliseconds);

// Return the number of milliseconds since the start of ilo Konpu,
// or a negative number if that information is unavailable.
int64_t UtilTime(void);


// <time.h>-like interface
#ifdef KONPU_H_
   typedef int64_t  time_t;

   // time_t time(time_t *t);
   // Returns the time as the number of seconds since the start of ilo Konpu
   // (Konpu doesn't keep time in between runs and has no concept of an 'epoch')
   // If t is not a null pointer, store the time there.
   #  define time(tptr_or_null)   c_time_(tptr_or_null)
#endif
static inline int64_t c_time_(int64_t *t) {
   int64_t seconds = UtilTime() / 1000;
   if (t) *t = seconds;
   return seconds;
}

#endif //include guard
