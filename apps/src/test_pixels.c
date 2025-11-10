#include "konpu.h"

int AppInit(void)
{
   VideoMode(123);
   Video.border = 45;

   for (int j = 0; j < 360; j++) {
      for (int i = 0; i < VIDEO_SIZE; i++) {
         Video.frame[i] = (2*i*j + j) % 256;
      }
      VideoRender();
   }

   for(int i = 0; i < VIDEO_HEIGHT; i++) {
      Video.frame[i + i*VIDEO_WIDTH] = i % 256;
      VideoRender();
   }

   UtilSleep(2000);
   return 0;
}
