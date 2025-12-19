#include "konpu.h"


// Linear interpolation (aka "lerp") utility function
// The returned value varies linearly from a (when f=0) to b (when f=1)
static inline float lerpf(float a, float b, float f)
{ return a + f * (b - a); }

// In 256 colors pixel video mode (mode 123), paint a gradient (top-to-bottom)
// between the two given RGB888 colors.
static void gradient(uint32_t rgb_start, uint32_t rgb_end)
{
   struct ColorLABf lab1 = ColorRGBiToLABf( ColorRGBi(rgb_start) );
   struct ColorLABf lab2 = ColorRGBiToLABf( ColorRGBi(rgb_end) );

   for (int y = 0; y < VIDEO_HEIGHT; y++) {
      float t = (float)y / VIDEO_HEIGHT;
      struct ColorLABf gradient = { .L = lerpf(lab1.L, lab2.L, t),
                                    .a = lerpf(lab1.a, lab2.a, t),
                                    .b = lerpf(lab1.b, lab2.b, t), };
      int color = ColorFromLABf(gradient);

      // In 256 color pixel mode, strip = pixel. Paint horizontal lines.
      VideoArray pixel = VIDEO_STRIP();
      for (int x = 0; x < VIDEO_WIDTH; x++)  pixel[y][x] = color;
   }
}


int AppInit(void)
{
   VideoMode(123);
   Video.border = COLOR_CSS_BLACK;

   while (true) {
      gradient(UtilRandom32(0x1000000), UtilRandom32(0x1000000));
      VideoRender();
      for (int i = 0; i < 90; i++) {
         KeyboardUpdate();
         if (KEY_IS_ANY_DOWN()) return 0;
         UtilSleep(10);
      }
   }
}
