// This App cycles through different modes and to show about big is the border

#include <konpu.h>

static void SHOW(int mode, const char *msg)
{
   int sz = VideoMode(mode);
   if (sz == 0) {
      Printer("Mode %d is invalid - %s\n", mode, msg);
      return;
   }
   Printer("%3d x %3d - %2d bytes per 8x8px - %s\n",
      VIDEO_WIDTH, VIDEO_HEIGHT, sz, msg);
   VideoRender();
   UtilSleep(2800);
}

int AppInit(void)
{
   COLOR_BORDER = 128;
   SHOW(  1, "2 color modes");  // 8
   SHOW(140, "8x8-attributes"); // 9
   SHOW(136, "4x8-attributes (default) / 8x8-double-attributes");  // 10
   SHOW(132, "4x4-attributes / 4x8-double-attributes");  // 12
   SHOW(121, "4 color modes | 2x4-attributes / 4x4-double-attributes");  // 16
   SHOW(131, "8 color modes (bitplanes) | 2x4-double-attributes (256 colors)");  // 24
   SHOW(122, "16 color non-attribute modes");   // 32
   SHOW(  5, "32 color modes (bitplanes)");     // 40
   SHOW(  6, "64 color modes (bitplanes)");     // 48
#if (VIDEO_FACTOR_ % 7 == 0)
   SHOW(  7, "128 color modes (bitplanes)");    // 56
#endif
   SHOW(123, "256 color non-attribute modes");  // 64
   return 0;
}
