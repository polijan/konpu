// FIXME: Seeting up the VIDEO_MODE doesn't work. Find Out why.
// #define KONPU_OPTION_OPTIMIZE_VIDEO_MODE 221

#include "konpu.h"

int AppInit(void)
{
   // 1.5 second of "tv static" effect...
   COLOR_BORDER = 0;
   VideoMode(VIDEO_MODE_GLYPH(Glyph64));
   for (int i = 0; i < 120; i++) {
      for (int n = 0; n < VIDEO_COUNT_GLYPH64; n++) {
         VIDEO_GLYPH64[n] = UtilRandom64();
      }
      VideoRender();
   }


   Glyph64  soweli    = 0x001515204054403E;
   Glyph128 soweli128 = GlyphUpscale(soweli);
   Glyph256 soweli256 = GlyphUpscale(soweli128);
   Glyph256 sstoki    = { 0xC131496A320418E0, 0x1F20404040201807,
                          0x00F00C028241C141, 0x00030C1013242724 };


VideoMode(VIDEO_MODE_GLYPH_ATTRIBUTES(Glyph256, ATTRIBUTE_8x8_FG256));
// VideoMode(VIDEO_MODE_GLYPH_ATTRIBUTES(Glyph256, ATTRIBUTE_2x4_16)); <-- crash


 //VideoMode(VIDEO_MODE_GLYPH(Glyph128));
//VideoMode(217);
//VideoMode(223); // weird modes with 24x15 glyph256

//VideoMode(209);

//   VideoMode(185);
//   VideoMode(176);
//   VideoMode(147); // why does it fail? does it? TODO!

   Printer("Factor:%d Mode:%d -> %dx%d [out of max. %dx%d]\n",
      VIDEO_FACTOR_, VIDEO_MODE,
      VIDEO_WIDTH, VIDEO_HEIGHT,
      VIDEO_MAX_WIDTH, VIDEO_MAX_HEIGHT);
   Printer("ColorDepth: %d\n", ColorDepth());

   Printer("Attributes: starts at %d\n", VideoAttributeOffset());
   Printer("  Log2(width) = %d, Log2(height) = %d, Log2Sizeof ) %d\n",
      AttributeWidthLog2(), AttributeHeightLog2(), AttributeHasTwoBytes());
   Printer("End       : %d\n", VIDEO_SIZE);


   VideoGlyphSetAll(soweli);
   for (int n = VideoAttributeOffset(); n < VIDEO_SIZE; n++) {
      VIDEO_BUFFER[n] = n % 256;
   }
   VideoRender();

   COLOR_BORDER = 55;
   for (int n = 0; n < VIDEO_COUNT_GLYPH256; n++) {
      VIDEO_GLYPH256[n] = (n%2)? soweli256:sstoki;
      if (n%8) VideoRender();
   }

   COLOR_BORDER = 70;
   int number_of_attributes_bytes = ( (VIDEO_WIDTH  >> AttributeWidthLog2())
                                    * (VIDEO_HEIGHT >> AttributeHeightLog2())
                                    ) >> AttributeHasTwoBytes();
   for (int n = 0; n < number_of_attributes_bytes; n++) {
      VIDEO_BUFFER[n + VideoAttributeOffset()] = (n % 16) << 4;
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
