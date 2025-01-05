#ifndef  KONPU_TIME_H
#define  KONPU_TIME_H
#include "c.h"

// Try to do nothing for the specified number of milliseconds.
// Depending on the underlying platform:
// - Possiblity the delay may would be longer due to OS scheduling.
// - Possiblity this is not supported in which case it does nothing (unlikely)
// - Possiblity the delay would be shorter (unlikely)
void TimeSleep(int milliseconds);

// Return the number of milliseconds since the start of ilo Konpu,
// or a negative number if that information is unavailable.
int64_t TimeTicks(void);

#endif //KONPU_TIME_H