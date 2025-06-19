// #define KONPU_OPTION_OPTIMIZE_VIDEO_MODE
#include "konpu.h"


#define K_LEFT   GLYPH32(0EEEEEE0)
#define K_RIGHT  GLYPH32(07777770)  // something like ]
#define K_FULL   GLYPH32_PATTERN_FULL
#define K_EMPTY  GLYPH32_PATTERN_EMPTY

static void keyboard_draw(void)
{
   int xstart = 1;
   int x = xstart, y = 1;
#define K    *VideoGlyph32(x++, y) = K_LEFT; *VideoGlyph32(x++, y) = K_RIGHT;
   K  x++; K K K K  x++; K K K K  x++; K K K K  x++; K K K K
   x = xstart; y+=2; K K K K K K K K K K K K K  x++; K K K  x++; K K K
#undef K
}


int AppInit(void)
{
   VideoMode(VIDEO_MODE_GLYPH_ATTRIBUTES(Glyph32, ATTRIBUTE_4x8_16));
   VideoGlyphSetAll();
   VideoAttributeSetAll(0x24);
   COLOR_BORDER = 255;

   keyboard_draw();

   while (1) {
      for (int key = 0; key < KEY_COUNT; key++) {
         if (!KEY_IS_DOWN(key)) continue;
         switch (key) {
            case KEY_SCANCODE_ESCAPE: goto MOVE;
            default: Printer("Key %d is pressed\n", key);
         }
      }
      VideoRender();
      KeyUpdate();
      TimeSleep(30);
   }

MOVE:
   TimeSleep(500);
   KeyUpdate(); // wait for reset
   int x = VIDEO_WIDTH_GLYPH  / 2;
   int y = VIDEO_HEIGHT_GLYPH / 2;
   while (1) {
      for (int key = 0; key < KEY_COUNT; key++) {
         if (KEY_IS_UP(key)) continue;
         switch (key) {
            case KEY_SCANCODE_ESCAPE: return 0;
            case KEY_SCANCODE_RIGHT : if (x < VIDEO_WIDTH_GLYPH-1)  x++; break;
            case KEY_SCANCODE_LEFT  : if (x > 0)                    x--; break;
            case KEY_SCANCODE_DOWN  : if (y < VIDEO_HEIGHT_GLYPH-1) y++; break;
            case KEY_SCANCODE_UP    : if (y > 0)                    y--; break;
         }
      }
      *VideoGlyph32(x,y) = GLYPH32_PATTERN_FULL;
      VideoRender();
      KeyUpdate();
      TimeSleep(30);
   }
   return 0;
}
