// choose one platform for rendering (or control it from your build system)
#define  KONPU_PLATFORM_SDL2       // will use SDL2
//#define  KONPU_PLATFORM_POSIX  // will output on terminal

#define  KONPU_RES_MODE 1      // <-- some default resolution
#define  KONPU_IMPLEMENTATION  // <-- must be defined once to include the code

#include <stdlib.h>
#include "konpu.h"

canvas screen;

////////////////////////////////////////////////////////////////////////////////

static void quitOnError(void) {
   if (renderer_getError()) {
      renderer_drop();
      exit(1);
   }
}

static void quit(void) {
   quitOnError();
   renderer_drop();
   exit(0);
}

static void waitForAKey(void) {
#if RENDERER_SDL2
   // TODO: HERE, WE CHEAT for now, by using SDL directly !!!
   //       The plan is for konpu to have its own simple input system
   //       to associate with a renderer. (probably covering just basic
   //       keyboard input)
   if (renderer_getId() == RENDERER_SDL2) {
      SDL_Event event;
      while (true) {
         while(SDL_PollEvent(&event)) {
            switch(event.type) {
               case SDL_QUIT   : quit();
               case SDL_KEYDOWN: return;
            }
         }
         sleep_ms(10);
      }
   }
#elif KONPU_PLATFORM_POSIX
   // for now, just have a delay
   sleep_ms(300);
#endif
}


////////////////////////////////////////////////////////////////////////////////

#define EXAMPLE   GLYPH(7C022A0204A8A800)  // soweli glyph

// print message and glyph
static void show(const char* msg, uint64_t glyph)
{
   canvas_set(screen, 0);
   print_quadrant(screen, 0,0, msg);
   canvas_glyph(screen, 1,2) = EXAMPLE;

   #define CHECKER  GLYPH(aa55aa55aa55aa55)
   canvas_glyph(screen, 3,1) = CHECKER & GLYPH_BOTTOM_RIGHT; canvas_glyph(screen, 4,1) = CHECKER & GLYPH_BOTTOM; canvas_glyph(screen, 5,1) = CHECKER & GLYPH_BOTTOM_LEFT;
   canvas_glyph(screen, 3,2) = CHECKER & GLYPH_RIGHT       ; canvas_glyph(screen, 4,2) = glyph                 ; canvas_glyph(screen, 5,2) = CHECKER & GLYPH_LEFT;
   canvas_glyph(screen, 3,3) = CHECKER & GLYPH_TOP_RIGHT   ; canvas_glyph(screen, 4,3) = CHECKER & GLYPH_TOP   ; canvas_glyph(screen, 5,3) = CHECKER & GLYPH_TOP_LEFT;

   print_quadrant(screen, 0,10, "PRESS ANY KEY");

   // render screen:
#if RENDERER_PSEUDOGRAPHICS
   // TODO: we must have this in the PSEUDOGRAPHICS renderer and remove the #ifdef
   if (renderer_getId() == RENDERER_PSEUDOGRAPHICS)
      RENDERER_PSEUDOGRAPHICS_TTY_CLEAR();
#endif
   render();
   quitOnError();
   waitForAKey();
}






int main(int argc, char **argv)
{  (void)argc; (void)argv; // not using argc/argv


   // init renderer
#if RENDERER_SDL2
   //if (rendererSDL2_init("Glyphs Operations Demo", 640, 480)) return 1;
   if (rendererSDL2_init("Glyphs Operations Demo", 2 * video.width_px, 2 * video.height_px))
      return 1;
#elif RENDERER_PSEUDOGRAPHICS
   if (rendererPseudoGraphics_init(RENDERER_PSEUDOGRAPHICS_MODE_2x2)) return 1;
#else
#  error("no suitable renderer")
#endif

   screen = video_getScreenCanvas();
   uint64_t g = EXAMPLE;
   while(true) {
      //fixme: print! eats up the last character
      show("normal "        , g);
      show("~ inverse "     , ~g);
      show("mirror "        , glyph_mirror(g));
      show("flip "          , glyph_flip(g));
      show("rotate 90 "     , glyph_rotate90(g));
      show("rotate 180 "    , glyph_rotate180(g));
      show("rotate 270 "    , glyph_rotate270(g));
      show("transpose "     , glyph_transpose(g));
   }
}
