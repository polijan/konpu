#include "test.h"
#include <math.h>

// Ensure the consistency of the sRGB vs OkLab color data in ROM
static const char *testColorRomData(void)
{
   for (int color = 0; color < 256; color++) {
      ColorInfo c = Rom.color[color];

      // Approximate OkLab data from ROM
      int L8 = c.L_half     * 2;
      int a8 = c.a_minus_12 + 12;
      int b8 = c.b_plus_30  - 30;

      // Create a float OkLab color from the ROM's sRGB data
      struct ColorRGBi rgb = { c.r, c.g, c.b };
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
