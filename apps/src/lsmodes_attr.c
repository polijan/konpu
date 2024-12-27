//------------------------------------------------------------------------------
// Prints details for all attribute video modes on the printer.
// Run it as: konpu --novideo --stdout --app <this application>
//------------------------------------------------------------------------------
#include "konpu.h"
int AppInit(void); // TODO: add this in the generated konpu.h ???

static const char *attr_color_type[] = {"16", "FG256", "BG256", "256"};
static const char *elem_size_str[] = {"2x4", "4x4", "4x8", "8x8", "8x16", "16x16"};

int AppInit(void)
{
   Printer("Mo- |Resolution |     Glyphs       |     Attributes\n");
   Printer("de  |Sz8  Pixels|  Res.    Type    |Offset  Res.   Type\n");
   for (int i = 128; i < 255; i++) {
      int nbytes = VideoSetMode(i);
      if (nbytes == 0) continue;

      enum VideoElementPixelSize elem_sz = VideoModeElementDescriptor();
      Printer("%3d  %2d  %3dx%3d  %3dx%2d  Glyph%d%s  %5d  %3dx%2d  ATTRIBUTE_%s_%s",
         i, nbytes,
         VIDEO_WIDTH, VIDEO_HEIGHT,
         VIDEO_WIDTH  >> VideoGlyphLog2Width(), VIDEO_HEIGHT >> VideoGlyphLog2Height(),
         8 * (1 << elem_sz), (elem_sz == 0)? "  " : ((elem_sz < 4)? " " : ""),
         VideoAttributeOffset(),
         VIDEO_WIDTH  >> AttributeWidthLog2(), VIDEO_HEIGHT >> AttributeHeightLog2(),
         elem_size_str[AttributePixelSize()],
         attr_color_type[AttributeColorType()]
      );

      if ( (VIDEO_WIDTH  % (1 << VideoGlyphLog2Width())) ||
           (VIDEO_HEIGHT % (1 << VideoGlyphLog2Height())) )
         Printer("*\n");
      else
         Printer("\n");

   }
   return 0;
}