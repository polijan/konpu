#include "konpu.h"
int AppInit(void); // TODO: add this in the generated konpu.h ???

// Booh, Ugly!
#include <stdlib.h>
#define SLEEP(n)                          \
   do {                                   \
      int ignored = system("sleep " #n);  \
      (void)ignored;                      \
   } while(0)


// According to Oklab, the closest sRGB color to gray-tone is #636363.
// The closest Konpu color to that is:
#define NEUTRAL 53


// Show the Konpu colors (256)
static void kule256(void)
{
   // Set a 256-color resolution.
   // We can use 285x256 and thus display all colors as 16x16 grid using 256x256
   // We use attribute backgrounds to easily display squares of colors.

   VideoSetMode(VIDEO_MODE_GLYPH_ATTRIBUTES(Glyph64, ATTRIBUTE_8x8_256)); 
   VideoGlyphSetAll();
   VideoAttributeSetAll(NEUTRAL);

   int color = 0;
   for (int y = 0; y < 32; y += 2) {
      for (int x = 0; x < 32; x += 2) {
         AttributeSetBackground(VideoAttribute(x + 2, y),     color);
         AttributeSetBackground(VideoAttribute(x + 3, y),     color);
         AttributeSetBackground(VideoAttribute(x + 2, y + 1), color);
         AttributeSetBackground(VideoAttribute(x + 3, y + 1), color);
         color++;
      }
   }
   assert(color == 256);
}

int AppInit(void)
{
   COLOR_BORDER = NEUTRAL;

   kule256(); 
   Printer("%d x %d  (color=%d)\n", VIDEO_WIDTH, VIDEO_HEIGHT, 1 << VideoColorDepth());
   VideoRender(); SLEEP(4);

   return 0;
}
