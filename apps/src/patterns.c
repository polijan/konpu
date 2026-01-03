#include <konpu.h>

static const uint64_t pattern[] = {
   GLYPH64_PATTERN_DITHER_BAYER_DITHER00, GLYPH64_PATTERN_DITHER_BAYER_DITHER01,
   GLYPH64_PATTERN_DITHER_BAYER_DITHER02, GLYPH64_PATTERN_DITHER_BAYER_DITHER03,
   GLYPH64_PATTERN_DITHER_BAYER_DITHER04, GLYPH64_PATTERN_DITHER_BAYER_DITHER05,
   GLYPH64_PATTERN_DITHER_BAYER_DITHER06, GLYPH64_PATTERN_DITHER_BAYER_DITHER07,
   GLYPH64_PATTERN_DITHER_BAYER_DITHER08, GLYPH64_PATTERN_DITHER_BAYER_DITHER09,
   GLYPH64_PATTERN_DITHER_BAYER_DITHER10, GLYPH64_PATTERN_DITHER_BAYER_DITHER11,
   GLYPH64_PATTERN_DITHER_BAYER_DITHER12, GLYPH64_PATTERN_DITHER_BAYER_DITHER13,
   GLYPH64_PATTERN_DITHER_BAYER_DITHER14, GLYPH64_PATTERN_DITHER_BAYER_DITHER15,
   GLYPH64_PATTERN_DITHER_BAYER_DITHER16,
};
#define PATTERN_COUNT  C_COUNTOF(pattern)

//#define SOWELI     GLYPH64(001515204054403E)
#define TOKI    \
   GLYPH64(     \
      10010010, \
      01010100, \
      00000000, \
      00111000, \
      01000100, \
      01000100, \
      00111000, \
      00000000  \
   )
#define SOWELI  TOKI

static int Show(int number_of_planes)
{
   // Set Video Mode and Clear Screen
   //VideoMode(VIDEO_MODE_GLYPH_ATTRIBUTES(64, ATTRIBUTE8_2x4));
   //VideoMode(VIDEO_MODE_GLYPH(64));
   VideoMode(VIDEO_MODE_GLYPH_PLANES(64, number_of_planes));

   Printer("%d plane(s) -> %d x %d  (color=%d)\n", number_of_planes, VIDEO_WIDTH, VIDEO_HEIGHT, 1 << ColorDepth());

   Video.border = 29;
   VideoGlyphSetAll();
   VideoAttributeSetAll(0x01);

   // Set something in half of the second plane (plane #1)
   //TODO: VideoGlyphSetAll(GLYPH64_PATTERN_ARCHITECTURE_BRICK_SHADED, 1);
   //for now, manually

   //memset(VIDEO_BUFFER + VIDEO_SIZE / 2, 0xCC, VIDEO_SIZE/2);

   if (!VideoModeHasAttributes() && VideoModeLowNibble() > 1) {
      int plane_offset = VideoPlaneOffset();
      for (int i = 0; i < plane_offset / C_SIZEOF(Glyph64); i++) {
         ((Glyph64*)(Video.frame + plane_offset))[i] = GLYPH64_PATTERN_FULL;
      }
      // VideoPlaneAs(Glyph64, 1)[i] = GLYPH64_PATTERN_HALF_BOTTOM;
      //GLYPH64_PATTERN_ARCHITECTURE_ARBOR;
   }

   //*VideoGlyph64(2,2) = SOWELI; VideoRender(); TimeSleep(4000); //Just to show

   // Set some Pattern as initial glyphs and show it
//   VideoGlyphSetAll(GLYPH64_PATTERN_WAVES_ZIGZAG_BOLD_SHADED);
//   VideoRender();
//   TimeSleep(1000);


   // Cycle through patterns
   for (int i = 0; i < 5 * PATTERN_COUNT; i++) {
      // Select a Glyph (go forth and back through the patterns)
      int n = i % (2 * PATTERN_COUNT);
      int idx = (n < PATTERN_COUNT)? n : (2 * PATTERN_COUNT - n - 1);
      Glyph64 g = pattern[idx];

      // Write the glyph pattern and some "soweli" glyphs
      for (int j = 0; j < VIDEO_COUNT_GLYPH64; j++) {
         Video.glyph64[j] = (j % 7)? g : (g ^ SOWELI);
      }
      VideoRender();
      TimeSleep(100);
   }

   TimeSleep(1000);
   return 0;
}

int AppInit(void) {
   for (int plane = 1; plane <= 8 ; plane ++)
      Show(plane);

   return 0;
}
