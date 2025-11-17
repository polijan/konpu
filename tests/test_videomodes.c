// #define TEST_TRACE
#include "test.h"


// In glyph modes, return the log2 of a glyph's pixel width.
// Thus, the width of a glyph is: 1 << VideoGlyphLog2Width().
#define VideoGlyphLog2Width() (((VIDEO_MODE >> 4 & 7) + 3) >> 1)
//                      VIDEO_MODE's element descriptor: |0|1|2|3|4|5|
//                      log2(glyph width):               |1|2|2|3|3|4|
//                      thus, we apply (x+3)/2 to the element descriptor

// In glyph modes, return the log2 of a glyph's pixel height.
// Thus, the height of a glyph is: 1 << VideoGlyphLog2Height().
#define VideoGlyphLog2Height() (2 + ((VIDEO_MODE >> 4 & 7) >> 1))
//                      VIDEO_MODE's element descriptor: |0|1|2|3|4|5|
//                      log2(glyph height):              |2|2|3|3|4|4|
//                      thus, we apply 2+x/2 to the element descriptor



// Test: Check that the default video mode is 4x8 glyph with similarly sized
//       standard attribute.
static const char *testDefaultVideoMode(void)
{
   TestAssert("VIDEO_MODE_DEFAULT should be 4x8 glyphs with similar 16 color attributes",
      VIDEO_MODE_DEFAULT == VIDEO_MODE_GLYPH_ATTRIBUTES(Glyph32));
   return 0;
}


// Test: For each video mode, verify that the overall size of the elements that
//       should be the framebuffer correspond to VIDEO_SIZE.
static const char *testVideoModes(void)
{
   for (int i = 0; i < 255; i++) {
      if (! VideoMode(i)) continue;
      TestTrace("mode:%3d, res:%3dx%3d, ", i, VIDEO_WIDTH, VIDEO_HEIGHT);

      if (VideoModeHasAttributes()) { // glyph + attributes
         // framebuffer first stores the glyphs...
         int glyph_sz = (1 << VideoModeDimension()) // <- sizeof the glyph
                      * VIDEO_WIDTH  / (1 << VideoGlyphLog2Width())
                      * VIDEO_HEIGHT / (1 << VideoGlyphLog2Height());
         TestTrace("buffer: glyph%03d[%3dx%3d]:%5d ",
            8 * (1 << VideoModeDimension()),
            VIDEO_WIDTH  / (1 << VideoGlyphLog2Width()),
            VIDEO_HEIGHT / (1 << VideoGlyphLog2Height()),
            glyph_sz
         );
         // ... and then the attributes
         int attr_sz = (1 << AttributeHasTwoBytes()) // <- 1 or 2 bytes
                     * VIDEO_WIDTH   / (1 << AttributeWidthLog2())
                     * VIDEO_HEIGHT  / (1 << AttributeHeightLog2());
         TestTrace("+ attr%02d[%3dx%3d]x%d:%5d ",
            8 * (1 << AttributeDimension()),
            VIDEO_WIDTH  / (1 << AttributeWidthLog2()),
            VIDEO_HEIGHT / (1 << AttributeHeightLog2()),
            1 <<  AttributeHasTwoBytes(),
            attr_sz
         );

         TestTrace("=> %5d (vs %5d)\n", glyph_sz + attr_sz, VIDEO_SIZE);
         TestAssert("size of glyph & attr vs framebuffer size", glyph_sz + attr_sz == VIDEO_SIZE);
      } else {

         int planes = VIDEO_MODE & 0xF;
         if (planes > 8) {
            // packed pixels
            int bpp = 1 << ((VIDEO_MODE & 0xF) - 8);
            int pixel_sz = VIDEO_WIDTH * VIDEO_HEIGHT * bpp / 8;
            TestTrace("buffer: pixels(%d bpp) => %5d (vs %5d)\n", bpp, pixel_sz, VIDEO_SIZE);
            TestAssert("size of packed pixels vs framebuffer size", pixel_sz == VIDEO_SIZE);
         } else if (VideoModeDimension() == PIXELS_Nx1) {
            // planar pixels
            int pixel_sz = VIDEO_WIDTH * VIDEO_HEIGHT * planes / 8;
            TestTrace("planar pixels (x%d) => %5d (vs %5d)\n", planes, pixel_sz, VIDEO_SIZE);
            TestAssert("size of planar pixels vs framebuffer size", pixel_sz == VIDEO_SIZE);
         } else {
            // planar glyphs
            int glyph_sz = (1 << VideoModeDimension())
                         * VIDEO_WIDTH  / (1 << VideoGlyphLog2Width())
                         * VIDEO_HEIGHT / (1 << VideoGlyphLog2Height())
                         * planes;
            TestTrace("planar glyphs (%d x glyph%03d[%3dx%3d]) => %5d (vs %5d)\n",
                  planes,
                  8 * (1 << VideoModeDimension()),
                  VIDEO_WIDTH  / (1 << VideoGlyphLog2Width()),
                  VIDEO_HEIGHT / (1 << VideoGlyphLog2Height()),
                  glyph_sz, VIDEO_SIZE
            );
            TestAssert("size of planar glyphs vs framebuffer size", glyph_sz == VIDEO_SIZE);
         }
      }
   }
   return 0;
}


TEST_BEGIN
   TestRun(testDefaultVideoMode);
   TestRun(testVideoModes);
TEST_END
