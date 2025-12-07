// FIXME: Seeting up the VIDEO_MODE doesn't work. Find Out why.
// #define KONPU_OPTION_OPTIMIZE_VIDEO_MODE 221

#include "konpu.h"

int AppInit(void)
{
   // 1.5 second of "tv static" effect...
   Video.border = 0;
   VideoMode(VIDEO_MODE_GLYPH(64));
   for (int i = 0; i < 120; i++) {
      for (int n = 0; n < VIDEO_COUNT_GLYPH64; n++) {
         Video.glyph64[n] = UtilRandom64();
      }
      VideoRender();
   }

   Glyph64  soweli    = 0x001515204054403E;
   Glyph128 soweli128 = GlyphUpscale(soweli);
   Glyph256 soweli256 = GlyphUpscale(soweli128);
   Glyph256 sstoki    = { 0xC131496A320418E0, 0x1F20404040201807,
                          0x00F00C028241C141, 0x00030C1013242724 };


//   VideoMode(VIDEO_MODE_GLYPH_ATTRIBUTES(256, ATTRIBUTE8_8x8_PEN));
   VideoMode(VIDEO_MODE_GLYPH_ATTRIBUTES(256, ATTRIBUTE8_2x4)); // Crash, why?


 //VideoMode(VIDEO_MODE_GLYPH(128));
//VideoMode(217);
//VideoMode(223); // weird modes with 24x15 glyph256

//VideoMode(209);

//   VideoMode(185);
//   VideoMode(176);
//   VideoMode(147); // why does it fail? does it? TODO!

   Printer("Factor:%d Mode:%d -> %dx%d [out of max. %dx%d]\n",
      VIDEO_FACTOR_, VIDEO_MODE,
      VIDEO_WIDTH, VIDEO_HEIGHT,
      8 * Rom.resolution_8x8[0], 8 * Rom.resolution_8x8[1]);
   Printer("ColorDepth: %d\n", ColorDepth());

   Printer("Attributes: starts at %d\n", VideoAttributeOffset());
   Printer("  Log2(width) = %d, Log2(height) = %d, Log2Sizeof ) %d\n",
      ATTRIBUTE_WIDTH_LOG2, ATTRIBUTE_HEIGHT_LOG2, ATTRIBUTE_SIZE_LOG2);
   Printer("End       : %d\n", VIDEO_SIZE);


   VideoGlyphSetAll(soweli);
   for (int n = VideoAttributeOffset(); n < VIDEO_SIZE; n++) {
      Video.frame[n] = n % 256;
   }
   VideoRender();

   Video.border = 55;
   for (int n = 0; n < VIDEO_COUNT_GLYPH256; n++) {
      Video.glyph256[n] = (n%2)? soweli256:sstoki;
      if (n%8) VideoRender();
   }

   Video.border = 70;
   assert(
      ((VIDEO_WIDTH_ATTRIBUTE * VIDEO_HEIGHT_ATTRIBUTE) >> ATTRIBUTE_SIZE_LOG2)
      == VIDEO_ATTRIBUTES_SIZE
   );
   for (int n = 0; n < VIDEO_ATTRIBUTES_SIZE; n++) {
      Video.frame[n + VideoAttributeOffset()] = (n % 16) << 4;
      if (n % VIDEO_WIDTH_ATTRIBUTE == 0) { // <-- once per line
         VideoRender(); UtilSleep(100);
      }
      assert(n + VideoAttributeOffset() < VIDEO_SIZE);
   }

   Video.border = 40;
   for (int i = 0; i < 40; i++) {
//      *(uint64_t*)VideoGlyph(0,0) = GlyphRotate270(*(uint64_t*)VideoGlyph(0,0));

      *VideoGlyph256(0,0) = GlyphRotate270(*VideoGlyph256(0,0));
      *VideoAttribute(4,4) = i%256;

      VideoRender(); UtilSleep(100);
      Video.border++;
   }
   return 0;
}
