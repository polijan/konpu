#ifndef  KONPU_TIME_H
#define  KONPU_TIME_H

// Try to do nothing for a specified number of milliseconds
// Depending on the underlying platform:
// - Possiblity the delay may will be longer due to OS scheduling.
// - Possiblity this is not supported in which case it does nothing (unlikely)
// - Possiblity the delay will be shorter (unlikely)
void TimeSleep(int milliseconds);

#endif //KONPU_TIME_H