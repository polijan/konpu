#include "konpu.h"
int AppInit(void); // TODO: add this in the generated konpu.h ???

// Booh, Ugly!
#include <stdlib.h>
#define SLEEP(n)                          \
   do {                                   \
      int ignored = system("sleep " #n);  \
      (void)ignored;                      \
   } while(0)

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

   SLEEP(2);
   return 0;
}
