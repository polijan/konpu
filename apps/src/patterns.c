#include "konpu.h"
int AppInit(void); // TODO: add this in the generated konpu.h ???

// Booh, Ugly!
#include <stdlib.h>
#define SLEEP(n)                          \
   do {                                   \
      int ignored = system("sleep " #n);  \
      (void)ignored;                      \
   } while(0)


static const uint64_t pattern[] = {
   GLYPH64_PATTERN_DITHER_BAYER_DITHER00,
   GLYPH64_PATTERN_DITHER_BAYER_DITHER01,
   GLYPH64_PATTERN_DITHER_BAYER_DITHER02,
   GLYPH64_PATTERN_DITHER_BAYER_DITHER03,
   GLYPH64_PATTERN_DITHER_BAYER_DITHER04,
   GLYPH64_PATTERN_DITHER_BAYER_DITHER05,
   GLYPH64_PATTERN_DITHER_BAYER_DITHER06,
   GLYPH64_PATTERN_DITHER_BAYER_DITHER07,
   GLYPH64_PATTERN_DITHER_BAYER_DITHER08,
   GLYPH64_PATTERN_DITHER_BAYER_DITHER09,
   GLYPH64_PATTERN_DITHER_BAYER_DITHER10,
   GLYPH64_PATTERN_DITHER_BAYER_DITHER11,
   GLYPH64_PATTERN_DITHER_BAYER_DITHER12,
   GLYPH64_PATTERN_DITHER_BAYER_DITHER13,
   GLYPH64_PATTERN_DITHER_BAYER_DITHER14,
   GLYPH64_PATTERN_DITHER_BAYER_DITHER15,
   GLYPH64_PATTERN_DITHER_BAYER_DITHER16,
};

int AppInit(void)
{
   VideoSetMode(VIDEO_MODE_ATTRIBUTE(Glyph64, ATTRIBUTE_8x8_16));

   // Set some Pattern as initial glyphs and show it
   VideoAttributeSetAll();
   VideoGlyphSetAll(GLYPH64_PATTERN_WAVES_ZIGZAG_BOLD_SHADED);
   VideoRender();
   SLEEP(2);

   int number_of_glyphs = (VIDEO_WIDTH  >> VideoGlyphLog2Width())
                        * (VIDEO_HEIGHT >> VideoGlyphLog2Height());
   Glyph64 soweli = 0x001515204054403E;

   // Cycle through patterns
   for (int i = 0; i < 5 * UTIL_ARRAY_SIZE(pattern); i++) {

      // Select a Glyph pattern
      Glyph64 g;
      int n = i % (2 * UTIL_ARRAY_SIZE(pattern));
      if (n < UTIL_ARRAY_SIZE(pattern)) {
         g = pattern[n];
      } else {
         n -= UTIL_ARRAY_SIZE(pattern);
         g = pattern[UTIL_ARRAY_SIZE(pattern) - 1 - n];
      }

      // Write the glyph pattern and some "soweli" glyphs
      for (int j = 0; j < number_of_glyphs; j++) {
         if (j % 7) {
            VIDEO_FRAMEBUFFER_AS(Glyph64)[j] = g;
         } else {
            VIDEO_FRAMEBUFFER_AS(Glyph64)[j] = g ^ soweli;
         }
      }
      VideoRender();
      SLEEP(0.1);
   }

   SLEEP(2);
   return 0;
}