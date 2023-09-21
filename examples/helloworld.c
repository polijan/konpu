// choose one platform for rendering (or control it from your build system)
#define  KONPU_PLATFORM_SDL2       // will use SDL2
//#define  KONPU_PLATFORM_POSIX    // will output on terminal

#define  KONPU_IMPLEMENTATION  // <-- must be defined once to include the code
#include "konpu.h"             // <-- tadaaa!

int main(int argc, char **argv)
{
   (void)argc; (void)argv;     // not using argc/argv
   renderer_initSimple();      // initialize a renderer

   // a very important message:
   canvas screen = video_getScreenCanvas();
   print_quadrant(screen, 0,0, "Hello, World ...");

   // RENDERER_PSEUDOGRAPHICS_TTY_CLEAR();
   render();                   // render the screen
   sleep_ms(2000);             // sleep 2 seconds

   renderer_drop();            // bye-bye!
   return 0;
}
