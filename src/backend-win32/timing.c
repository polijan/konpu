#include "backend-common/backend.h"
#include "core84/timing.h"

#define  WIN32_LEAN_AND_MEAN
#include <windows.h>

//------------------------------------------------------------------------------
// Implementation of Backend time functions
//------------------------------------------------------------------------------

void BackendInitTime(void) { }
void BackendInitDrop(void) { }

//------------------------------------------------------------------------------
// Implementation of the Core's Time*() functions
//------------------------------------------------------------------------------

void TimeSleep(int ms) { if (ms > 0) Sleep(ms); }

// TODO
int64_t Time(void)  { return -1; }
