#include "glyph_tools.h"

int main(int argc, char *argv[])
{
   if (argc != 2) goto usage;

   Symbol s = { .size = atoi(argv[1]) / 8 };
   for (int i = 0; i < 256; i++) {
      switch(s.size) {
         case 1:
            s.glyph8 = i;
            break;
         case 2:
            s.glyph16 = 0;
            if (i & 1)    s.glyph16 |= 0x0003;
            if (i & 2)    s.glyph16 |= 0x000c;
            if (i & 4)    s.glyph16 |= 0x0030;
            if (i & 8)    s.glyph16 |= 0x00c0;
            if (i & 16)   s.glyph16 |= 0x0300;
            if (i & 32)   s.glyph16 |= 0x0c00;
            if (i & 64)   s.glyph16 |= 0x3000;
            if (i & 128)  s.glyph16 |= 0xc000;
            break;
         case 4:
            s.glyph32 = 0;
            if (i & 1)    s.glyph32 |= 0x00000033;
            if (i & 2)    s.glyph32 |= 0x000000cc;
            if (i & 4)    s.glyph32 |= 0x00003300;
            if (i & 8)    s.glyph32 |= 0x0000cc00;
            if (i & 16)   s.glyph32 |= 0x00330000;
            if (i & 32)   s.glyph32 |= 0x00cc0000;
            if (i & 64)   s.glyph32 |= 0x33000000;
            if (i & 128)  s.glyph32 |= 0xcc000000;
            break;
         case 8:
            s.glyph64 = 0;
            if (i & 1)    s.glyph64 |= 0x0000000000000F0F;
            if (i & 2)    s.glyph64 |= 0x000000000000F0F0;
            if (i & 4)    s.glyph64 |= 0x000000000F0F0000;
            if (i & 8)    s.glyph64 |= 0x00000000F0F00000;
            if (i & 16)   s.glyph64 |= 0x00000F0F00000000;
            if (i & 32)   s.glyph64 |= 0x0000F0F000000000;
            if (i & 64)   s.glyph64 |= 0x0F0F000000000000;
            if (i & 128)  s.glyph64 |= 0xF0F0000000000000;
            break;
         case 16:
            s.glyph128 = (Glyph128){0};
            if (i & 1)    s.glyph128.top    |= 0x0000000000000F0F;
            if (i & 2)    s.glyph128.top    |= 0x000000000000F0F0;
            if (i & 4)    s.glyph128.top    |= 0x000000000F0F0000;
            if (i & 8)    s.glyph128.top    |= 0x00000000F0F0F000;
            if (i & 16)   s.glyph128.bottom |= 0x00000F0F0F0F0000;
            if (i & 32)   s.glyph128.bottom |= 0x0000F0F0F0F00000;
            if (i & 64)   s.glyph128.bottom |= 0x0F0F0F0F00000000;
            if (i & 128)  s.glyph128.bottom |= 0xF0F0F0F000000000;
            break;
         case 32:
            s.glyph256 = (Glyph256){0};
            if (i & 1)    s.glyph256.top_left     |= 0x00000000FFFFFFFF;
            if (i & 2)    s.glyph256.top_right    |= 0x00000000FFFFFFFF;
            if (i & 4)    s.glyph256.top_left     |= 0xFFFFFFFF00000000;
            if (i & 8)    s.glyph256.top_right    |= 0xFFFFFFFF00000000;
            if (i & 16)   s.glyph256.bottom_left  |= 0x00000000FFFFFFFF;
            if (i & 32)   s.glyph256.bottom_right |= 0x00000000FFFFFFFF;
            if (i & 64)   s.glyph256.bottom_left  |= 0xFFFFFFFF00000000;
            if (i & 128)  s.glyph256.bottom_right |= 0xFFFFFFFF00000000;
            break;
         default: goto usage;
      }

      printf("%05x   ", UnicodeOctants[i]);
      SymbolPrintBits(s);
      printf ("  ");
      UnicodePutChar(UnicodeOctants[i]);
      putchar('\n');
   }

   return EXIT_SUCCESS;

usage:
   fprintf(stderr,
      "Usage: %s [8|16|32|64|128|256]\n"
      "Print octant font for the given type of glyph.\n"
      , argv[0]);
   return EXIT_FAILURE;
}
