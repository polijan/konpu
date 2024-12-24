#include "color.h"
#include "init.h"

#include <string.h>
void ColorInit(void)
{
   memcpy(COLOR_PALETTE128, KonpuROM + KONPU_ROM_PALETTE, 128);

   // should be in ROM anu seme?
   COLOR_BORDER = 1;
   COLOR_DEFAULT_BG = 0;
   COLOR_DEFAULT_FG = 45;
 }

/*
void ColorResetPalette(int n)
{
   assert(IS_A_POWER_OF_TWO((uint8_t)n));

   int offset = (n == 128)? 0 : (n - 2);
   memcpy(COLOR_PALETTE128 + offset, KonpuROM + KONPU_ROM_PALETTE + offset, n);
}
*/
