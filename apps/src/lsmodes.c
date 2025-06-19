//------------------------------------------------------------------------------
// Prints details for all video modes on the printer.
// Run it as: konpu --novideo --stdout --app <this application>
//------------------------------------------------------------------------------
#include "konpu.h"

static const char *attr_color_type_str[] = {"16", "FG256", "BG256", "256"};
static const char *dimension_str[] = {"2x4","4x4","4x8","8x8","8x16","16x16"};
static const char *bpp_chunk_str[] = {"Quarter", "Nibble", "Byte"};

int AppInit(void)
{
   int valid = 0;
   Printer("Mode Sz8x8   Res.  #Colors Framebuffer content and layout\n");
   for (int i = 0; i < 255; i++) {
      int nbytes = VideoMode_(i);
      if (nbytes == 0) continue;

      valid++;
      VideoMode(i);
      Printer("%3d   %2d   %3dx%3d   %3d   ",
         i, nbytes, VIDEO_WIDTH, VIDEO_HEIGHT, 1 << ColorDepth());

      enum VideoElementDimension elem_dimension = VideoModeDimension();
      int low_nibble = VideoModeLowNibble();

      // Attribute Modes -------------------------------------------------------
      if (VideoModeHasAttributes()) {
         if (elem_dimension == PIXELS) {
            Printer("Bit-Pixels (as %dx%d bytes)",
               VIDEO_WIDTH >> 3, VIDEO_HEIGHT);
         } else {
            Printer("Glyph%d (%dx%d)",
               8 << elem_dimension, VIDEO_WIDTH_GLYPH, VIDEO_HEIGHT_GLYPH);
         }
         Printer(" + ATTRIBUTE_%s_%s (%dx%d @offset %d)",
            dimension_str[AttributeDimension()],
            attr_color_type_str[AttributeColorType()],
            VIDEO_WIDTH_ATTRIBUTE, VIDEO_HEIGHT_ATTRIBUTE,
            VideoAttributeOffset());

      // Chunky Modes ----------------------------------------------------------
      } else if (low_nibble > 8) {
         int bpp = 1 << (low_nibble - 8);
         const char *chunk = bpp_chunk_str[low_nibble - 9];
         if (elem_dimension == PIXELS) {
            Printer("%s-Pixels (as %dx%d bytes)",
              chunk, VIDEO_WIDTH * bpp / 8, VIDEO_HEIGHT);
         } else {
            Printer("Tile%s%d[=%spx] (%dx%d)",
               chunk, 8 * bpp * (1 << elem_dimension),
               dimension_str[elem_dimension],
               // TODO: we probably want VIDEO_WIDTH_TILE, VIDEO_HEIGHT_TILE
               //       name. In any case, tile's width and height depends
               //       on the same dimension indicator in the VIDEO_MOVE,
               //       so it's in fact same implementation as glyphs:
               VIDEO_WIDTH_GLYPH, VIDEO_HEIGHT_GLYPH);
         }

      // Planar Modes ----------------------------------------------------------
      } else {
         if (low_nibble > 1) Printer("%d ", low_nibble);
         if (elem_dimension == PIXELS) {
            Printer("Bit-Pixel%s(as %dx%d bytes",
               (low_nibble > 1)? " Planes ":"s ",
               VIDEO_WIDTH_PIXELBYTE_8x1, VIDEO_HEIGHT_PIXELBYTE);
         } else {
            Printer("Glyph%d%s (%dx%d",
               8 << elem_dimension, (low_nibble > 1)? " Planes":"",
               VIDEO_WIDTH_GLYPH, VIDEO_HEIGHT_GLYPH);
         }
         if (low_nibble > 1) {
            Printer(" @offsets n x %d)", VIDEO_SIZE / low_nibble);
         } else {
            Printer(") [single bitplane]");
         }

      //------------------------------------------------------------------------
      }
      Printer("\n");
   }
   Printer("\nNumber of valid video modes: %d\n", valid);
   Printer("Default video mode: %d\n", VIDEO_MODE_DEFAULT);
   return 0;
}
