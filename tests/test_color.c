#include "test.h"
#include <math.h>

// Ensure the consistency of the sRGB vs OkLab color data in ROM
static const char *testColorRomData(void)
{
   for (int c = 0; c < 256; c++) {
      // Approximate OkLab data from ROM
      int L8 = COLOR_ROM(8*c) << 1;
      int a8 = (int8_t)(COLOR_ROM(8*c + 1)) + 12;
      int b8 = (int8_t)(COLOR_ROM(8*c + 2)) - 30;

      // sRGB data from ROM (create a "real" OkLab color from it)
      struct ColorRGBi rgb = {
         .r = COLOR_ROM(8*c + 5),
         .g = COLOR_ROM(8*c + 6),
         .b = COLOR_ROM(8*c + 7),
      };
      struct ColorLABf lab = ColorRGBiToLABf(rgb);

      // Compare
      TestAssert("L", fabs(510 * lab.L - L8) < 1.0f);
      TestAssert("a", fabs(510 * lab.a - a8) < 0.5f);
      TestAssert("b", fabs(510 * lab.b - b8) < 0.5f);
   }
   return 0;
}


// Test sRGB gamma and reverse-gamma are inverse functions.
static const char *testRGBGamma(void)
{
   for (int i = 0; i < 256; i++) {
      TestAssert("Gamma", ColorRGBGammaify(ColorRGBLinearize(i)) == i);
   }
   return 0;
}


TEST_BEGIN
   TestRun(testColorRomData);
   TestRun(testRGBGamma);
TEST_END
