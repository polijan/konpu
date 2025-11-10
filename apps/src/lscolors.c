#include "konpu.h"

// Display colors 0..n of the given palette as a nx * ny grid
// if palette is null, show the given color.
static void kule(int n, int nx, int ny, uint8_t* palette)
{
   // assert(nx * ny == n);

   int color = 0;
   for (int y = 0; y < 32; y += 32/ny)
   for (int x = 0; x < 32; x += 32/nx) {
      for(int j = 0; j < 32/ny; j++)
      for(int i = 0; i < 32/nx; i++) {
         AttributeSetPaper(
            VideoAttribute(x+2+i, y+j),
            (palette)? palette[color] : color
         );
      }
      color++;
      if (color >= n) return;
   }
}

int AppInit(void)
{
   // According to Oklab, the closest sRGB color to gray-tone is #636363.
   // The closest Konpu color to that is:
   #define NEUTRAL 53

   VideoMode(VIDEO_MODE_GLYPH_ATTRIBUTES(Glyph64, ATTRIBUTE_8x8_256));
   VideoGlyphSetAll();
   VideoAttributeSetAll(NEUTRAL);
   Video.border = NEUTRAL;

   int color_log2 = 1;
   while (true) {
      switch (color_log2) {
         case  1: kule(  2,   2,  1, Video.palette2);  break;
         case  2: kule(  4,   2,  2, Video.palette4);  break;
         case  3: kule(  8,   4,  2, Video.palette8);  break;
         case  4: kule( 16,   4,  4, Video.palette16); break;
         case  5: kule( 32,   4,  8, Video.palette32); break;
         case  6: kule( 64,   8,  8, Video.palette64); break;
         default: kule(256,  16, 16, NULL); break;
      }
      VideoRender();
      UtilSleep(30);
      KeyboardUpdate();
      if (KEY_IS_DOWN(KEY_SCANCODE_ESCAPE)) {
         return 0;
      } else if (KEY_IS_TRIGGERED(KEY_SCANCODE_RIGHT)) {
         if (color_log2 < 7) color_log2++;
      } else if (KEY_IS_TRIGGERED(KEY_SCANCODE_LEFT)) {
         if (color_log2 > 1) color_log2--;
      } else {
         continue;
      }
   }
   return 0;
}
