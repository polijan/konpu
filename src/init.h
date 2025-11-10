#ifndef KONPU_INIT_H_
#define KONPU_INIT_H_

//------------------------------------------------------------------------------
// Initialization and Deinitialization of Konpu's systems
//------------------------------------------------------------------------------

#include "c.h"

#if KONPU_PLATFORM_SDL1 || KONPU_PLATFORM_SDL2 || KONPU_PLATFORM_SDL3
#  define PlatformInit()  SDL_Init(0) // <- init SDL but with no subsystems
#  define PlatformDrop()  SDL_Quit()
   // SDL_Quit should always be invoked when leaving SDL, even when if every
   // subsystem has already been shutdown via SDL_QuitSubSystem().
#else
#  define PlatformInit()
#  define PlatformDrop()
#endif

void    TimeInit(void);
#define TimeDrop()

extern void UtilInitRandom_0_(void);
#define RandomInit() UtilInitRandom_0_()
#define RandomDrop()

void    VideoInit(void);
#define VideoDrop()
void    VideoReset(void); //TODO <-- should be public? (so move to video.h/c ?)

int     VideoBackendInit(int desired_zoom_max);
void    VideoBackendDrop(void);

#define KeyInit()
#define KeyDrop()

#define PrinterInit()
#define PrinterDrop()   KonpuDisconnectPrinter()

#define StackInit()
#define StackDrop()

void    HeapInit(void);
#define HeapDrop()


#endif //include guard
