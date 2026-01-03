#include <konpu.h>

int AppInit(void)
{
   VideoMode(VIDEO_MODE_STRIP(2));

   while (!KEY_IS_ANY_DOWN()) {
      int x      = Random32(VIDEO_WIDTH);
      int y      = Random32(VIDEO_HEIGHT);
      int color  = Random32(1 << ColorDepth());
      int radius = 6 + Random32(UTIL_MIN(VIDEO_WIDTH, VIDEO_HEIGHT) / 4);

      DrawCircleFilled(x,y, radius, color);
      VideoRender();
      KeyboardUpdate();
   }
   return 0;
}
