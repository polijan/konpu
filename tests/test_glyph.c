#include "test.h"

// Test: For each video mode, verify that the overall size of the elements that
//       should be the framebuffer correspond to VIDEO_SIZE.

// A loop creating random glyph<8|16|...|256> named g<8|16|...|256>
#define RANDOM_GLYPHS_BEGIN                                             \
        for (int i_ = 0; i_ < 100; i_++) {                              \
            uint8_t  g8   = Random();                                   \
            uint16_t g16  = Random();                                   \
            uint32_t g32  = Random();                                   \
            uint64_t g64  = Random();                                   \
            Glyph128 g128 = { Random(), Random() };                     \
            Glyph256 g256 = { Random(), Random(), Random(), Random() };

#define RANDOM_GLYPHS_END   }


// Various check for IsEqual
static const char *testIsEqual() {
   TestAssert("comparison of different types is false",
              GlyphIsEqual((uint8_t) 0, (uint16_t) 0) == false);

   RANDOM_GLYPHS_BEGIN
      TestAssert("Glyph8:   IsEqual(g,g) should be true", GlyphIsEqual(g8, g8));
      TestAssert("Glyph16:  IsEqual(g,g) should be true", GlyphIsEqual(g16, g16));
      TestAssert("Glyph32:  IsEqual(g,g) should be true", GlyphIsEqual(g32, g32));
      TestAssert("Glyph64:  IsEqual(g,g) should be true", GlyphIsEqual(g64, g64));
      TestAssert("Glyph128: IsEqual(g,g) should be true", GlyphIsEqual(g128, g128));
      TestAssert("Glyph256: IsEqual(g,g) should be true", GlyphIsEqual(g256, g256));
   RANDOM_GLYPHS_END
   return 0;
}

// Check that applying bitwise Not twice gives back the original glyph.
static const char *testNotNot() {
   RANDOM_GLYPHS_BEGIN
      TestAssert("Glyph8  : Not Not = identity",
         GlyphIsEqual(g8,   GlyphNot(GlyphNot(g8))));
      TestAssert("Glyph16 : Not Not = identity",
         GlyphIsEqual(g16,  GlyphNot(GlyphNot(g16))));
      TestAssert("Glyph32 : Not Not = identity",
         GlyphIsEqual(g32,  GlyphNot(GlyphNot(g32))));
      TestAssert("Glyph64 : Not Not = identity",
         GlyphIsEqual(g64,  GlyphNot(GlyphNot(g64))));
      TestAssert("Glyph128: Not Not = identity",
         GlyphIsEqual(g128, GlyphNot(GlyphNot(g128))));
      TestAssert("Glyph256: Not Not = identity",
         GlyphIsEqual(g256, GlyphNot(GlyphNot(g256))));
   RANDOM_GLYPHS_END
   return 0;
}

// Check that applying Mirror twice gives back the original glyph.
static const char *testMirrorMirror() {
   RANDOM_GLYPHS_BEGIN
      TestAssert("Glyph8  : Mirror Mirror = identity",
         GlyphIsEqual(g8,   GlyphMirror(GlyphMirror(g8))));
      TestAssert("Glyph16 : Mirror Mirror = identity",
         GlyphIsEqual(g16,  GlyphMirror(GlyphMirror(g16))));
      TestAssert("Glyph32 : Mirror Mirror = identity",
         GlyphIsEqual(g32,  GlyphMirror(GlyphMirror(g32))));
      TestAssert("Glyph64 : Mirror Mirror = identity",
         GlyphIsEqual(g64,  GlyphMirror(GlyphMirror(g64))));
      TestAssert("Glyph128: Mirror Mirror = identity",
         GlyphIsEqual(g128, GlyphMirror(GlyphMirror(g128))));
      TestAssert("Glyph256: Mirror Mirror = identity",
         GlyphIsEqual(g256, GlyphMirror(GlyphMirror(g256))));
   RANDOM_GLYPHS_END
   return 0;
}

// Check that applying Flip twice gives back the original glyph.
static const char *testFlipFlip() {
   RANDOM_GLYPHS_BEGIN
      TestAssert("Glyph8  : Flip Flip = identity",
         GlyphIsEqual(g8,   GlyphFlip(GlyphFlip(g8))));
      TestAssert("Glyph16 : Flip Flip = identity",
         GlyphIsEqual(g16,  GlyphFlip(GlyphFlip(g16))));
      TestAssert("Glyph32 : Flip Flip = identity",
         GlyphIsEqual(g32,  GlyphFlip(GlyphFlip(g32))));
      TestAssert("Glyph64 : Flip Flip = identity",
         GlyphIsEqual(g64,  GlyphFlip(GlyphFlip(g64))));
      TestAssert("Glyph128: Flip Flip = identity",
         GlyphIsEqual(g128, GlyphFlip(GlyphFlip(g128))));
      TestAssert("Glyph256: Flip Flip = identity",
         GlyphIsEqual(g256, GlyphFlip(GlyphFlip(g256))));
   RANDOM_GLYPHS_END
   return 0;
}

// Check that applying Rotate180 twice gives back the original glyph.
static const char *testRotate180180() {
   RANDOM_GLYPHS_BEGIN
      TestAssert("Glyph8  : Rotate 180 twice = identity",
         GlyphIsEqual(g8,   GlyphRotate180(GlyphRotate180(g8))));
      TestAssert("Glyph16 : Rotate 180 twice = identity",
         GlyphIsEqual(g16,  GlyphRotate180(GlyphRotate180(g16))));
      TestAssert("Glyph32 : Rotate 180 twice = identity",
         GlyphIsEqual(g32,  GlyphRotate180(GlyphRotate180(g32))));
      TestAssert("Glyph64 : Rotate 180 twice = identity",
         GlyphIsEqual(g64,  GlyphRotate180(GlyphRotate180(g64))));
      TestAssert("Glyph128: Rotate 180 twice = identity",
         GlyphIsEqual(g128, GlyphRotate180(GlyphRotate180(g128))));
      TestAssert("Glyph256: Rotate 180 twice = identity",
         GlyphIsEqual(g256, GlyphRotate180(GlyphRotate180(g256))));
   RANDOM_GLYPHS_END
   return 0;
}

// For square glyphs, check that applying Transpose twice gives back the
// original glyph.
static const char *testTransposeTranspose() {
   RANDOM_GLYPHS_BEGIN
      (void)g8, (void)g32, (void)g128; // Only square glyphs
      TestAssert("Glyph16 : Transpose Transpose = identity",
         GlyphIsEqual(g16,  GlyphRotate270(GlyphRotate90(g16))));
      TestAssert("Glyph64 : Transpose Transpose = identity",
         GlyphIsEqual(g64,  GlyphRotate270(GlyphRotate90(g64))));
      TestAssert("Glyph256: Transpose Transpose = identity",
         GlyphIsEqual(g256, GlyphRotate270(GlyphRotate90(g256))));
   RANDOM_GLYPHS_END
   return 0;
}

// For square glyphs, check that applying Rotate270 and then Rotate90 gives
// back the original glyph.
static const char *testRotate90270() {
   RANDOM_GLYPHS_BEGIN
      (void)g8, (void)g32, (void)g128; // Only square glyphs
      TestAssert("Glyph16 : Rotate 90 + 270 = identity",
         GlyphIsEqual(g16,  GlyphRotate270(GlyphRotate90(g16))));
      TestAssert("Glyph64 : Rotate 90 + 270 = identity",
         GlyphIsEqual(g64,  GlyphRotate270(GlyphRotate90(g64))));
      TestAssert("Glyph256: Rotate 90 + 270 = identity",
         GlyphIsEqual(g256, GlyphRotate270(GlyphRotate90(g256))));
   RANDOM_GLYPHS_END
   return 0;
}

// Check that SwapHalves applied twice is same as the original glyph
static const char *testSwapHalvesSwapHalves() {
   RANDOM_GLYPHS_BEGIN
      TestAssert("Glyph8  : SwapHalves twice = identity",
         GlyphIsEqual(g8,  GlyphSwapHalves(GlyphSwapHalves(g8))));
      TestAssert("Glyph16 : SwapHalves twice = identity",
         GlyphIsEqual(g16,  GlyphSwapHalves(GlyphSwapHalves(g16))));
      TestAssert("Glyph32 : SwapHalves twice = identity",
         GlyphIsEqual(g32,  GlyphSwapHalves(GlyphSwapHalves(g32))));
      TestAssert("Glyph64 : SwapHalves twice = identity",
         GlyphIsEqual(g64,  GlyphSwapHalves(GlyphSwapHalves(g64))));
      TestAssert("Glyph128: SwapHalves twice = identity",
         GlyphIsEqual(g128, GlyphSwapHalves(GlyphSwapHalves(g128))));
      TestAssert("Glyph256: SwapHalves twice = identity",
         GlyphIsEqual(g256, GlyphSwapHalves(GlyphSwapHalves(g256))));
   RANDOM_GLYPHS_END
   return 0;
}

// Extract two halves with GlyphHalf, rebuild a glyph from those two with
// GlyphFromHalves, and check it's same as the original glyph
static const char *testFromHalvesHalfHalf() {
   RANDOM_GLYPHS_BEGIN
      (void)g8; // We don't test on Glyph8
      TestAssert("Glyph16: FromHalf(Half(), Half()) = Identity",
         GlyphIsEqual(g16, GlyphFromHalves(GlyphHalf(g16, 0), GlyphHalf(g16, 2))));
      TestAssert("Glyph32: FromHalf(Half(), Half()) = Identity",
         GlyphIsEqual(g32, GlyphFromHalves(GlyphHalf(g32, 0), GlyphHalf(g32, 4))));
      TestAssert("Glyph64: FromHalf(Half(), Half()) = Identity",
         GlyphIsEqual(g64, GlyphFromHalves(GlyphHalf(g64, 0), GlyphHalf(g64, 4))));
      TestAssert("Glyph128: FromHalf(Half(), Half()) = Identity",
         GlyphIsEqual(g128, GlyphFromHalves(GlyphHalf(g128, 0), GlyphHalf(g128, 8))));
      (void)g256;// TODO: TestAssert("Glyph256: FromHalf(Half(), Half()) = Identity",
      //   GlyphIsEqual(g256, GlyphFromHalves(GlyphHalf(g256, 0), GlyphHalf(g256, 8))));
   RANDOM_GLYPHS_END
   return 0;
}


TEST_BEGIN
   TestRun(testIsEqual);

   // Test with some transformations and their inverse
   TestRun(testNotNot);
   TestRun(testMirrorMirror);
   TestRun(testFlipFlip);
   TestRun(testRotate180180);
   TestRun(testTransposeTranspose);
   TestRun(testRotate90270);

   TestRun(testSwapHalvesSwapHalves);
   TestRun(testFromHalvesHalfHalf);
TEST_END
