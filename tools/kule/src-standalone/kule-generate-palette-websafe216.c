// Print a Palette from the 216 "websafe" colors
// ie, a 6-level RGB palette where r,g,b varies uniformly in gamma sRGB space.
// See: https://en.wikipedia.org/wiki/Web_colors#Web-safe_colors

#include <stdio.h>
#include <stdlib.h>

int main(void)
{
   // Colors take r,g,b components take 6 levels
   for (int r = 0; r < 6; r++) {
      for (int g = 0; g < 6; g++) {
         for (int b = 0; b < 6; b++) {
            // Components 0..5 are scaled uniformly to 0..255
            printf("%02x%02x%02x\n", 51*r, 51*g, 51*b);
         }
      }
   }
   return EXIT_SUCCESS;
}
