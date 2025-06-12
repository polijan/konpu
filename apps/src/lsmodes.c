//------------------------------------------------------------------------------
// Prints details for all video modes on the printer.
// Run it as: konpu --novideo --stdout --app <this application>
//------------------------------------------------------------------------------
#include "konpu.h"
int AppInit(void); // TODO: add this in the generated konpu.h ???


static const char *attr_color_type_str[] = {"16", "FG256", "BG256", "256"};
static const char *elem_pixel_size_str[] = {"2x4", "4x4", "4x8", "8x8", "8x16", "16x16"};
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

      enum VideoElementDimension elem_size = VideoModeDimension();
      int low_nibble = VideoModeLowNibble();

      // Attribute Modes -------------------------------------------------------
      if (VideoModeHasAttributes()) {
         if (elem_size == PIXELS) {
            Printer("Bit-Pixels (as %dx%d bytes)",
               VIDEO_WIDTH >> 3, VIDEO_HEIGHT);
         } else {
            Printer("Glyph%d (%dx%d)",
               8 << elem_size,
               VIDEO_WIDTH  >> VideoGlyphLog2Width(),
               VIDEO_HEIGHT >> VideoGlyphLog2Height());
         }
         Printer(" + ATTRIBUTE_%s_%s (%dx%d @offset %d)",
            elem_pixel_size_str[AttributeDimension()] ,
            attr_color_type_str[AttributeColorType()] ,
            VIDEO_WIDTH  >> AttributeWidthLog2(),
            VIDEO_HEIGHT >> AttributeHeightLog2(),
            VideoAttributeOffset());

      // Chunky Modes ----------------------------------------------------------
      } else if (low_nibble > 8) {
         int bpp = 1 << (low_nibble - 8);
         const char *chunk = bpp_chunk_str[low_nibble - 9];
         if (elem_size == PIXELS) {
            Printer("%s-Pixels (as %dx%d bytes)",
              chunk, VIDEO_WIDTH * bpp / 8, VIDEO_HEIGHT);
         } else {
            Printer("Tile%s%d[=%spx] (%dx%d)",
               chunk, 8 * bpp * (1 << elem_size),
               elem_pixel_size_str[elem_size],
               // Tile width and height depends on the same element descriptor
               // it's same as glyphs:
               VIDEO_WIDTH  >> VideoGlyphLog2Width(),
               VIDEO_HEIGHT >> VideoGlyphLog2Height());
         }

      // Planar Modes ----------------------------------------------------------
      } else {
         if (low_nibble > 1) Printer("%d ", low_nibble);
         if (elem_size == PIXELS) {
            Printer("Bit-Pixel%s(as %dx%d bytes",
               (low_nibble > 1)? " Planes ":"s ",
               VIDEO_WIDTH >> 3, VIDEO_HEIGHT);
         } else {
            Printer("Glyph%d%s (%dx%d",
               8 << elem_size, (low_nibble > 1)? " Planes":"",
               VIDEO_WIDTH  >> VideoGlyphLog2Width(),
               VIDEO_HEIGHT >> VideoGlyphLog2Height());
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
