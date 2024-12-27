//------------------------------------------------------------------------------
// Prints details for all video modes on the printer.
// Run it as: konpu --novideo --stdout --app <this application>
//------------------------------------------------------------------------------
#include "konpu.h"
int AppInit(void); // TODO: add this in the generated konpu.h ???


int AppInit(void)
{
   const char *attr_type[] = {"fg16/bg16", "fg256", "bg256", "fg256+bg256"};

   int valid = 0;
   Printer("Mode Sz8x8   Res.  #Colors Description\n");
   for (int i = 0; i < 255; i++) {
      int nbytes = VideoModeResolution(i);
      if (nbytes == 0) continue;

      valid++;
      VideoSetMode(i);
      Printer("%3d   %2d   %3dx%3d   %3d   ",
         i, nbytes, VIDEO_WIDTH, VIDEO_HEIGHT, 1 << VideoColorDepth());


      unsigned elem_size = VideoModeElementDescriptor();
      const char *elem_size_str = "";
      switch (elem_size) {
         case PIXELS_2x4:   elem_size_str = "2x4";   break;
         case PIXELS_4x4:   elem_size_str = "4x4";   break;
         case PIXELS_4x8:   elem_size_str = "4x8";   break;
         case PIXELS_8x8:   elem_size_str = "8x8";   break;
         case PIXELS_8x16:  elem_size_str = "8x16";  break;
         case PIXELS_16x16: elem_size_str = "16x16"; break;
      }

      if (VideoModeHasAttributes()) {
         Printer("glyphs(%d) & attributes(%d / %s)",
                  8 * (1 << elem_size),
                  8 * (1 << AttributePixelSize()),
                  attr_type[AttributeColorType()]
               );

      } else if ((VIDEO_MODE & 0xF) > 8) {

         // Chunky Modes:
         int bpp = 1 << ((VIDEO_MODE & 0xF) - 8);
         const char *chunk = "";
         switch (bpp) {
            case 2: chunk = "Quarter"; break;
            case 4: chunk = "Nibble";  break;
            case 8: chunk = "Byte";    break;
            default: assert(0); // wrong chuck
         }
         if (elem_size == PIXELS) {
            Printer("Chunky Pixels");
            Printer(" (%d bpp (\"%s\") -> %d colors)", bpp, chunk, 1 << bpp);
         } else {
            Printer("Tile%s%d", chunk, 8 * bpp * (1 << elem_size));
            Printer("(%s px, %d colors)", elem_size_str, 1 << bpp);
         }

      } else {

         // Planar Modes:
         Printer("planar(x%d) ", VIDEO_MODE & 0xF);
         if (elem_size == PIXELS) {
            Printer("pixels");
         } else {
            Printer("glyphs(%d)", 8 * (1 << elem_size));
         }
      }
      Printer("\n");
   }

   Printer("\nNumber of valid video modes: %d\n", valid);
   Printer("Default video mode: %d\n", VIDEO_MODE_DEFAULT);
   return 0;
}
