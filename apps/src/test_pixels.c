#include "konpu.h"
int AppInit(void); // TODO: add this in the generated konpu.h ???

int AppInit(void)
{
   VideoMode(123);
   COLOR_BORDER = 45;

   for (int j = 0; j < 360; j++) {
      for (int i = 0; i < VIDEO_SIZE; i++) {
         VIDEO_BUFFER[i] = (2*i*j + j) % 256;
      }
      VideoRender();
   }

   for(int i = 0; i < VIDEO_HEIGHT; i++) {
      VIDEO_BUFFER[i + i*VIDEO_WIDTH] = i % 256;
      VideoRender();
   }

   TimeSleep(2000);
   return 0;
}
