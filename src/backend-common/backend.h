#ifndef BACKEND_COMMON_INTERFACE_H_
#define BACKEND_COMMON_INTERFACE_H_
#include "core84/c.h"

//------------------------------------------------------------------------------
// Backend systems Init and Drop functions
//------------------------------------------------------------------------------

int BackendInitPlatform(void);
int BackendDropPlatform(void);

// Initialize Graphics.
// User can specify a desired zoom level by which to magnify Konpu's framebuffer
// on the real screen. If desired zoom level is 0 or negative, then user wishes
// to initialize full screen graphics instead of windowed.
// The backend is free to ignore this parameter, but it it does consider it,
// magnification level should be as close as possible to the given parameter
// without exceeding it.
int BackendInitGraphics(C_ATTRIBUTE_MAYBE_UNUSED int desired_zoom_max);
int BackendDropGraphics(void);

int BackendInitAudio(void);
int BackendDropAudio(void);

int BackendInitTime(void);
int BackendDropTime(void);

int BackendInitKeyboard(void);
int BackendDropKeyboard(void);

int BackendInitMouse(void);
int BackendDropMouse(void);

int BackendInitPrinter(void);
int BackendDropPrinter(void);

//------------------------------------------------------------------------------

// A `main`-like entry point for the backend.
// Notes:
// - The default-provided implementation calls the BackendInit* and BackendDrop*
//   functions, so if you use it, you need not call those functions yourself.
// - If you use a C main function as the entry point, it could just be this:
//   `int main(int argc, char *argv[]) { return BackendMain(argc,argv) ? EXIT_FAILURE : EXIT_SUCCESS; }`
int BackendMain(int argc, char *argv[]);

void BackendUpdate(void);

void BackendEvent(const void *event_ctx);

#endif //include guard
