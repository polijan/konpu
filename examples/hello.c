// choose one platform for rendering (or control it from your build system)
#define  KONPU_PLATFORM_SDL2       // will use SDL2
// #define  KONPU_PLATFORM_POSIX   // will output on terminal

#define  KONPU_RES_MODE 2      // <-- some default resolution
#define  KONPU_IMPLEMENTATION  // <-- must be defined once to include the code
#include "konpu.h"             // <-- tadaaa!


// paint: render the screen and sleep a bit
static void paint(int delay)
{

#if RENDERER_PSEUDOGRAPHICS
   // TODO: we must have this in the PSEUDOGRAPHICS renderer and remove the #ifdef
   if (renderer_getId() == RENDERER_PSEUDOGRAPHICS)
      RENDERER_PSEUDOGRAPHICS_TTY_CLEAR();
#endif
   render();
   if (rendererSingleton.error) {
      renderer_drop();
      exit(1);
   }

   // sleep some n * 60th of a seconds
   sleep_ms(delay * 1000/60);
}

////////////////////////////////////////////////////////////////////////////////

// some toki pona glyphs
#  define GLYPH_LI         GLYPH(6030180C18306000)
#  define GLYPH_SONA       GLYPH(9254007C44447C00)
#  define GLYPH_SOWELI     GLYPH(7C022A0204A8A800)
#  define GLYPH_SUNO       GLYPH(101028C628101000)
#  define GLYPH_JAN        GLYPH(386C6C6C386CC600) // bold

// we'll wait a bit before rendering a frame:
#  define DELAY            15 //milliseconds


int main(int argc, char **argv)
{  (void)argc; (void)argv; // not using argc/argv

   // init renderer
#if RENDERER_SDL2
   // if (rendererSDL2_init("hello, world!", 768, 432)) return 1;
   if (rendererSDL2_init("hello, world!", 2 * video.width_px, 2 * video.height_px))
      return 1;
#elif RENDERER_PSEUDOGRAPHICS
   if (rendererPseudoGraphics_init(RENDERER_PSEUDOGRAPHICS_MODE_2x4)) return 1;
#else
#  error("no suitable renderer")
#endif

   // init randomizer
   random_init(0xCAFE);

   while(true) {

#if RENDERER_SDL2
      // TODO: HERE, WE CHEAT for now, by using SDL directly !!!
      //       The plan is for konpu to have its own simple input system
      //       to associate with a renderer. (probably covering just basic
      //       keyboard input)
      if (renderer_getId() == RENDERER_SDL2) {
          SDL_Event event;
          while(SDL_PollEvent(&event)) {
             switch(event.type) {
                case SDL_QUIT: goto quit;
             }
          }
      }
#endif

      canvas screen = video_getScreenCanvas();
      canvas_set(screen, 0); paint(DELAY);

      canvas_glyph(screen, 0,0) = GLYPH_SOWELI;  paint(DELAY);

      canvas_glyph(screen, 1,0) = glyph_fromQuadrants(
                                     chr_quadrant('I'), chr_quadrant('L'),
                                     chr_quadrant('K'), chr_quadrant('O')) | GLYPH(ff);
      paint(DELAY);
      canvas_glyph(screen, 2,0) = glyph_fromQuadrants(
                                     chr_quadrant('O'), 0                ,
                                     chr_quadrant('N'), chr_quadrant('P'));
      paint(DELAY);
      canvas_glyph(screen, 3,0) = glyph_fromQuadrants(
                                     0                , chr_quadrant('&'),
                                     chr_quadrant('U'), 0);
      paint(DELAY);

      canvas_glyph(screen,  5,0) = chr('H');  paint(DELAY);
      canvas_glyph(screen,  6,0) = chr('E');  paint(DELAY);
      canvas_glyph(screen,  7,0) = chr('L');  paint(DELAY);
      canvas_glyph(screen,  8,0) = chr('L');  paint(DELAY);
      canvas_glyph(screen,  9,0) = chr('O');  paint(DELAY);

      canvas_glyph(screen, 0,2) |= glyph_rotate180(GLYPH_SOWELI);
      canvas_glyph(screen, 1,2) |= GLYPH_LI;
      canvas_glyph(screen, 2,2) |= glyph_flip(GLYPH_SONA);
      canvas_glyph(screen, 0,3) |= GLYPH_JAN;
      canvas_glyph(screen, 1,3) |= GLYPH_SONA;
      canvas_glyph(screen, 2,3) |= GLYPH_LI;
      canvas_glyph(screen, 3,3) |= GLYPH_SUNO;
      paint(DELAY);

      canvas_line(screen, -8,8, 100,64);
      paint(DELAY);

      // create another canvas in the screen:
      rect crop = {12,4, -20,80};
      canvas area = canvas_crop(screen, crop);
      canvas_set(area, GLYPH(5fbe5fbe5fbe5fbe));
      paint(DELAY);

      print_quadrant(area, 6,1, "ilo Konpu says this: Hello, World! Hello there, how Are you today? Has it been a fantastic day, or what?");
      paint(DELAY);

      for(int i = 0; i < 75; i++) {
         canvas_line(screen, random()%256,random()%144, random()%256,random()%144);
         paint(3);
      }

   }

quit:
   renderer_drop();
   return 0;
}
