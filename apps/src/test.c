
// FIXME: Seeting up the VIDEO_MODE doesn't work. Find Out why.
// #define KONPU_OPTION_OPTIMIZE_VIDEO_MODE 221

#include "konpu.h"
int AppInit(void); // TODO: add this in the generated konpu.h ???

int AppInit(void)
{
   Glyph64  soweli    = 0x001515204054403E;
   Glyph128 soweli128 = GlyphUpscale(soweli);
   Glyph256 soweli256 = GlyphUpscale(soweli128);
   Glyph256 sstoki    = { 0xC131496A320418E0, 0x1F20404040201807,
                          0x00F00C028241C141, 0x00030C1013242724 };

VideoSetMode(VIDEO_MODE_GLYPH_ATTRIBUTES(Glyph256, ATTRIBUTE_8x8_FG256));
 //VideoSetMode(VIDEO_MODE_GLYPH(Glyph128));
//VideoSetMode(217);
//VideoSetMode(223); // weird modes with 24x15 glyph256

//VideoSetMode(209);

//   VideoSetMode(185);
//   VideoSetMode(176);
//   VideoSetMode(147); // why does it fail? does it? TODO!

   Printer("Factor:%d Mode:%d -> %dx%d [out of max. %dx%d]\n",
      VIDEO_SIZE_FACTOR_, VIDEO_MODE,
      VIDEO_WIDTH, VIDEO_HEIGHT,
      VIDEO_MODE_WIDTH_MAX, VIDEO_MODE_HEIGHT_MAX);
   Printer("ColorDepth: %d\n", VideoColorDepth());

   Printer("Attributes: starts at %d\n", VideoAttributeOffset());
   Printer("  Log2(width) = %d, Log2(height) = %d, Log2Sizeof ) %d\n",
      AttributeWidthLog2(), AttributeHeightLog2(), AttributeHasTwoBytes());
   Printer("End       : %d\n", VIDEO_SIZE);

   COLOR_BORDER = 45;
   VideoGlyphSetAll(GLYPH8_PATTERN_FULL);
   VideoGlyphSetAll(soweli);

   for (int n = VideoAttributeOffset(); n < VIDEO_SIZE; n++) {
      VIDEO_FRAMEBUFFER[n] = n % 256;
   }
   VideoRender();

   COLOR_BORDER = 55;
   int number_of_glyphs = (VIDEO_WIDTH  >> VideoGlyphLog2Width())
                        * (VIDEO_HEIGHT >> VideoGlyphLog2Height());
   //assert(number_of_glyphs == VideoGlyphCount()); FIXMEEEE!
   for (int n = 0; n < number_of_glyphs; n++) {
      VIDEO_FRAMEBUFFER_AS(Glyph256)[n] = (n%2)? soweli256:sstoki;
      if (n%8) VideoRender();
   }

   COLOR_BORDER = 70;
   int number_of_attributes_bytes = ( (VIDEO_WIDTH  >> AttributeWidthLog2())
                                    * (VIDEO_HEIGHT >> AttributeHeightLog2())
                                    ) >> AttributeHasTwoBytes();
   for (int n = 0; n < number_of_attributes_bytes; n++) {
      VIDEO_FRAMEBUFFER[n + VideoAttributeOffset()] = (n % 16) << 4;
      if (n % (VIDEO_WIDTH >> AttributeWidthLog2()) == 0) { // <-- once per line
         VideoRender(); TimeSleep(100);
      }
      assert(n + VideoAttributeOffset() < VIDEO_SIZE);
   }

   COLOR_BORDER = 40;
   for (int i = 0; i < 40; i++) {
//      *(uint64_t*)VideoGlyph(0,0) = GlyphRotate270(*(uint64_t*)VideoGlyph(0,0));

      *VideoGlyph256(0,0) = GlyphRotate270(*VideoGlyph256(0,0));
      *VideoAttribute(4,4) = i%256;

      VideoRender(); TimeSleep(100);
      COLOR_BORDER++;
   }
   return 0;
}
