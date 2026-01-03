#include "test.h"

// A 100x loop in which random Glyphs named g<8|16|...|256> are available
#define RANDOM_GLYPHS_BEGIN                                                    \
   for (int i_ = 0; i_ < 100; i_++) {                                          \
      Glyph8   g8   = Random32(0x100);                                         \
      Glyph16  g16  = Random32(0x10000);                                       \
      Glyph32  g32  = Random32();                                              \
      Glyph64  g64  = Random64();                                              \
      Glyph128 g128 = { Random64(), Random64() };                              \
      Glyph256 g256 = { Random64(), Random64(), Random64(), Random64(), };

#define RANDOM_GLYPHS_END   }


//------------------------------------------------------------------------------


// Test for Glyph literals
static const char *testGlyphLiterals(void)
{
   static_assert(GLYPH8(D5) == GLYPH8(
      10,         // @ . \_ 1+4 -> 5
      10,         // @ . /
      10,         // @ . \_ 1+(4+8) -> D
      11          // @ @ /
   ));

   static_assert(GLYPH16(0225) == GLYPH16(
      1010,       // @ . @ .  5
      0100,       // . @ . .  2
      0100,       // . @ . .  2
      0000        // . . . .  0
   ));

   static_assert(GLYPH32(03465500) == GLYPH32(
      0000,       // . . . .  0
      0000,       // . . . .  0
      1010,       // @ . @ .  5
      1010,       // @ . @ .  5
      0110,       // . @ @ .  6
      0010,       // . . @ .  4
      1100,       // @ @ . .  3
      0           // . . . .  0
   ));

   static_assert(GLYPH64(001515204054403E) == GLYPH64(
      01111100,   // . @ @ @ @ @ . . 3E
      00000010,   // . . . . . . @ . 40
      00101010,   // . . @ . @ . @ . 54
      00000010,   // . . . . . . @ . 40
      00000100,   // . . . . . @ . . 20
      10101000,   // @ . @ . @ . . . 15
      10101000,   // @ . @ . @ . . . 15
      0           // . . . . . . . . 00
   ));

   // Testing GLYPH<8|16|32|64> could work with static_assert(), but results of
   // GLYPH<128|256> are compound literals, not integral constants, so we need
   // proper TestAsserts().

   Glyph128 g128_1 = { 0xE6EE7E2606060518, 0x081020604CDFC6C6 };
   Glyph128 g128_2 = GLYPH128(081020604CDFC6C6,E6EE7E2606060518);
   Glyph128 g128_3 = GLYPH128(
      00011000,   // . . . @ @ . . . 18
      10100000,   // @ . @ . . . . . 05
      01100000,   // . @ @ . . . . . 06
      01100000,   // . @ @ . . . . . 06
      01100100,   // . @ @ . . @ . . 26
      01111110,   // . @ @ @ @ @ @ . 7E
      01110111,   // . @ @ @ . @ @ @ EE
      01100111,   // ._@_@_._._@_@_@_E6
      01100011,   // . @ @ . . . @ @ C6
      01100011,   // . @ @ . . . @ @ C6
      11111011,   // @ @ @ @ @ . @ @ DF
      00110010,   // . . @ @ . . @ . 4C
      00000110,   // . . . . . @ @ . 60
      00000100,   // . . . . . @ . . 20
      00001000,   // . . . . @ . . . 10
      00010000    // . . . @ . . . . 08
   );
   TestAssert("Glyph128 literal (1)", GlyphIsEqual(g128_1, g128_2));
   TestAssert("Glyph128 literal (2)", GlyphIsEqual(g128_1, g128_3));
   TestAssert("Glyph128 literal (3)", GlyphIsEqual(g128_1, g128_3));

   Glyph256 g256_1 = {  0xC131496A320418E0, 0x1F20404040201807 ,
                        0x00F00C028241C141, 0x00030C1013242724 };
   Glyph256 g256_2 = GLYPH256(
      00000111,11100000,   // . . . . . @ @ @|@ @ @ . . . . .
      00011000,00011000,   // . . . @ @ . . .|. . . @ @ . . .
      00100000,00000100,   // . . @ . . . . .|. . . . . @ . .
      01001100,00000010,   // . @ . . @ @ . .|. . . . . . @ .
      01010110,00000010,   // . @ . @ . @ @ .|. . . . . . @ .
      10010010,00000010,   // @ . . @ . . @ .|. . . . . . @ .
      10001100,00000100,   // @ . . . @ @ . .|. . . . . @ . .
      10000011,11111000,   // @_._._._._._@_@|@_@_@_@_@_._._.
      10000010,00100100,   // @ . . . . . @ .|. . @ . . @ . .
      10000011,11100100,   // @ . . . . . @ @|@ @ @ . . @ . .
      10000010,00100100,   // @ . . . . . @ .|. . @ . . @ . .
      01000001,11001000,   // . @ . . . . . @|@ @ . . @ . . .
      01000000,00001000,   // . @ . . . . . .|. . . . @ . . .
      00110000,00110000,   // . . @ @ . . . .|. . @ @ . . . .
      00001111,11000000,   // . . . . @ @ @ @|@ @ . . . . . .
      00000000,00000000    // . . . . . . . .|. . . . . . . .
   );
   TestAssert("Glyph256 literal", GlyphIsEqual(g256_1, g256_2));

   return 0;
}


// Various check for IsEqual
static const char *testIsEqual(void) {
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
static const char *testNotNot(void) {
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
static const char *testMirrorMirror(void) {
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
static const char *testFlipFlip(void) {
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
static const char *testRotate180180(void) {
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
static const char *testTransposeTranspose(void) {
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
static const char *testRotate90270(void) {
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
static const char *testSwapHalvesSwapHalves(void) {
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
static const char *testFromHalvesHalfHalf(void) {
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
   TestRun(testGlyphLiterals);
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
