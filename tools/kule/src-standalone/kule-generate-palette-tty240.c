// Print the typical last 240 colors of a terminal emulator palette.
// Colors 0-15 are left to black.

#include <stdio.h>
#include <stdlib.h>

int main(void)
{
   int i = 0;

   // Colors 0-15 do vary (we put those to black, edit as needed)
   for (; i < 16; i++) {
      printf("000000\n");
   }

   // Colors 16-231 correspond to colors with r,g,b components in 0..5
   for (; i < 232; i++) {
      // A Color of index c has r,g,b components [0..5] such that:
      // c = 16 + 36*r + 6*g + b
      unsigned c = i - 16;
      unsigned b = c % 6;
      c /= 6;
      unsigned r = c / 6;
      unsigned g = c % 6;

      // Components 0..5 are scaled as such:
      if (r != 0) { r = 40 * r + 55; }
      if (g != 0) { g = 40 * g + 55; }
      if (b != 0) { b = 40 * b + 55; }

      printf("%02x%02x%02x\n", r,g,b);
   }

   // Colors 232-255 form a grayscale ramp
   for (; i < 256; i++) {
      // Grayscale ramp has intensity of 8 + 10*n (n in 0..23).
      unsigned g = 10 * i - 2312; // <- 8 + 10 * (i - 232)
      printf("%02x%02x%02x\n", g,g,g);
   }

   return EXIT_SUCCESS;
}
