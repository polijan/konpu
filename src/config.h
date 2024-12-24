#ifndef  KONPU_CONFIG_H
#define  KONPU_CONFIG_H

// This file would be included in every .h and .c in Konpu.
// In here, you can define what you wish (for example, you may override the
// -D... definitions passed by the build system).

#undef KONPU_PLATFORM_SDL1
#undef KONPU_PLATFORM_SDL2
#undef KONPU_PLATFORM_SDL3
#undef KONPU_PLATFORM_POSIX
#undef KONPU_PLATFORM_WINDOWS
#undef KONPU_PLATFORM_LIBC

#define KONPU_PLATFORM_SDL2
#define KONPU_PLATFORM_POSIX
#define KONPU_PLATFORM_LIBC


// Override the Video RAM "factor" (ie the size of the video framebuffer is
// factor * 2880 bytes)
// #define VIDEO_SIZE_FACTOR_  3

#endif //KONPU_CONFIG_H
