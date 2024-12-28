#ifndef KONPU_INIT_H
#define KONPU_INIT_H

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

void    ColorInit(void);
#define ColorDrop()

void    VideoInit(void);
#define VideoDrop()
void    VideoReset(void); //TODO <-- should be public? (so move to video.h/c ?)

int     VideoBackendInit(int desired_zoom_max);
void    VideoBackendDrop(void);

#define PrinterInit()
#define PrinterDrop()   KonpuDisconnectPrinter()

#define StackInit()
#define StackDrop()

void    HeapInit(void);
#define HeapDrop()


#endif //KONPU_INIT_H
