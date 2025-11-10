#include "konpu.h"

int AppInit(void)
{
   VideoMode(VIDEO_MODE_PIXEL(4));

   while (!KEY_IS_ANY_DOWN()) {
      int x      = UtilRandom32(VIDEO_WIDTH);
      int y      = UtilRandom32(VIDEO_HEIGHT);
      int color  = UtilRandom32(1 << ColorDepth());
      int radius = 6 + UtilRandom32(UTIL_MIN(VIDEO_WIDTH, VIDEO_HEIGHT) / 4);

      DrawCircleFilled(x,y, radius, color);
      VideoRender();
      KeyboardUpdate();
   }
   return 0;
}
