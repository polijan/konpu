#include "konpu.h"

// Draw lines at random on the screen using a random color.
// This works in any mode. (Of course in attribute modes, it starts out well but
// "color clash" quickly becomes apparent).

//#define MODE   VIDEO_MODE_STRIP8_PLANES(3)
//#define MODE   VIDEO_MODE_STRIP(4)
//#define MODE   VIDEO_MODE_STRIP8_ATTRIBUTES(ATTRIBUTE16_4x4)

#define MODE   VIDEO_MODE_GLYPH_ATTRIBUTES(256, ATTRIBUTE8_2x4)

static void draw_random_line(void)
{
   int x1    = UtilRandom32(VIDEO_WIDTH);
   int y1    = UtilRandom32(VIDEO_HEIGHT);
   int x2    = UtilRandom32(VIDEO_WIDTH);
   int y2    = UtilRandom32(VIDEO_HEIGHT);
   int color = UtilRandom32(1 << ColorDepth());
   DrawLine(x1,y1, x2,y2, color);
}


int AppInit(void)
{
   VideoMode(MODE);

   while (!KEY_IS_ANY_DOWN()) {
      draw_random_line();

      VideoRender();
      KeyboardUpdate();
   }
   return 0;
}
