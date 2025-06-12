#include "konpu.h"
int AppInit(void); // TODO: add this in the generated konpu.h ???

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

   keyboard_draw();

   while (1) {
      for (int key = 0; key < KEY_COUNT; key++) {
         if (!KeyIsTriggered(key)) continue;
         switch (key) {
            case 41: goto MOVE;
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
   int X = VIDEO_WIDTH  >> VideoGlyphLog2Width();
   int Y = VIDEO_HEIGHT >> VideoGlyphLog2Height();
   int x = X / 2;
   int y = Y / 2;
   while (1) {
      for (int key = 0; key < KEY_COUNT; key++) {
         if (KeyIsUp(key)) continue;
         switch (key) {
            case 41: return 0;
            case 79: x += 1; if (x > X) x = X; break;
            case 80: x -= 1; if (x < 0) x = 0; break;
            case 81: y += 1; if (y > Y) y = Y; break;
            case 82: y -= 1; if (y < 0) y = 0; break;
         }
      }
      *VideoGlyph32(x,y) = GLYPH32_PATTERN_FULL;
      VideoRender();
      KeyUpdate();
      TimeSleep(30);
   }



   return 0;
}
