#include "glyph.h"

void PrivateGlyphClear_Rectangle_(Rectangle r)
{
   switch (GLYPH_DIMENSION) {
      case PIXELS_2x4:   PrivateGlyphSetAll_Rectangle_8_(r, 0);  break;
      case PIXELS_4x4:   PrivateGlyphSetAll_Rectangle_16_(r, 0); break;
      case PIXELS_4x8:   PrivateGlyphSetAll_Rectangle_32_(r, 0); break;
      case PIXELS_8x8:   PrivateGlyphSetAll_Rectangle_64_(r, 0); break;
      case PIXELS_8x16:  PrivateGlyphSetAll_Rectangle_128_(r, (Glyph128){0}); break;
      case PIXELS_16x16: PrivateGlyphSetAll_Rectangle_256_(r, (Glyph256){0}); break;

      case PIXELS_Nx1:
         // As we're supposed to have Glyphs, not Strip, thus we could fall to
         // unreachable(). But we shouldn't lead to UB like that, we can just
         // do nothing.
         break;

      default: unreachable();
   }
}