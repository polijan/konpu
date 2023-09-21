#define  KONPU_PLATFORM_LIBC
#define  KONPU_IMPLEMENTATION
#include <konpu.h>

int main(int argc, char **argv)
{  (void)argc; (void)argv;  // not using argc/arg

   rendererPPM_init(4, 4);
   random_init(1234);

   canvas screen = video_getScreenCanvas();

#  define FRAMES 300
   for (int i=0; i < FRAMES; i++) {
      for (int y = 0; y < screen.height; y++)
         for (int x = 0; x < screen.width; x++)
            canvas_glyph(screen, x,y) = random();

      print(screen, 0,0, "Random Glyphs ...");
      render();
   }

   renderer_drop();
   return 0;
}
