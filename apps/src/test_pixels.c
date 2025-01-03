#include "konpu.h"
int AppInit(void); // TODO: add this in the generated konpu.h ???

int AppInit(void)
{
   if (VideoSetMode(123) == 0)
      return 1;
   Printer("Factor:%d Mode:%d -> %dx%d [out of max. %dx%d]\n",
      VIDEO_SIZE_FACTOR_, VIDEO_MODE,
      VIDEO_WIDTH, VIDEO_HEIGHT,
      VIDEO_MODE_WIDTH_MAX, VIDEO_MODE_HEIGHT_MAX);

   COLOR_BORDER = 45;

   for (int j = 0; j < 360; j++) {
      for (int i = 0; i < VIDEO_SIZE; i++) {
         VIDEO_FRAMEBUFFER[i] = (2*i*j + j) % 256;
      }
      VideoRender();
   }

   for(int i = 0; i < VIDEO_HEIGHT; i++) {
      VIDEO_FRAMEBUFFER[i + i*VIDEO_WIDTH] = i % 256;
      VideoRender();
   }

   TimeSleep(2000);
   return 0;
}
