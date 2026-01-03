#include "test.h"

TEST_BEGIN
   // Nothing to do, everything is based on static_asserts.
   // As long as this file compiles, everything's good.
TEST_END



////////////////////////////////////////////////////////////////////////////////
// STATIC ASSERTS for measurement macros of the Video Elements
//
// IMPORTANT: They modify/override the VIDEO_MODE macro
////////////////////////////////////////////////////////////////////////////////


#define VIDEO_ELEMENT_WIDTH_(dimension)          (2 << (((dimension) + 1) >> 1))
#define VIDEO_ELEMENT_HEIGHT_(dimension)         (4 << ((dimension) >> 1))
#define VIDEO_ELEMENT_COUNT_PIXELS_(dimension)   (8 << (dimension))
#define VIDEO_ELEMENT_WIDTH_LOG2_(dimension)     (((dimension) + 3) >> 1)
#define VIDEO_ELEMENT_HEIGHT_LOG2_(dimension)    (2 + ((dimension) >> 1))
#define VIDEO_ELEMENT_COUNT_PIXELS_LOG2_(dimension)     ((dimension) + 3)

#define VIDEO_ASSERT_(P, W,H)                                     \
   static_assert(VIDEO_ELEMENT_WIDTH_(P) == W);                   \
   static_assert(VIDEO_ELEMENT_HEIGHT_(P) == H);                  \
   static_assert(VIDEO_ELEMENT_COUNT_PIXELS_(P) == W * H);        \
   static_assert(1 << VIDEO_ELEMENT_WIDTH_LOG2_(P)  == W);        \
   static_assert(1 << VIDEO_ELEMENT_HEIGHT_LOG2_(P) == H);        \
   static_assert(1 << VIDEO_ELEMENT_COUNT_PIXELS_LOG2_(P) == W * H)
VIDEO_ASSERT_(PIXELS_2x4,    2,4);
VIDEO_ASSERT_(PIXELS_4x4,    4,4);
VIDEO_ASSERT_(PIXELS_4x8,    4,8);
VIDEO_ASSERT_(PIXELS_8x8,    8,8);
VIDEO_ASSERT_(PIXELS_8x16,   8,16);
VIDEO_ASSERT_(PIXELS_16x16, 16,16);
#undef VIDEO_ASSERT_


//------------------------------------------------------------------------------
// Static Assert TESTS for Video element Measurements
//------------------------------------------------------------------------------

#define STATIC_ASSERT_ELEMENT(ELEM, N)                                         \
   static_assert(ELEM##_WIDTH  == 1 << ELEM##_WIDTH_LOG2);                     \
   static_assert(ELEM##_HEIGHT == 1 << ELEM##_HEIGHT_LOG2);                    \
   static_assert(ELEM##_SIZE   == 1 << ELEM##_SIZE_LOG2);                      \
   static_assert(ELEM##_COUNT_PIXELS == 1 << ELEM##_COUNT_PIXELS_LOG2);        \
   static_assert(ELEM##_COUNT_BITS   == 1 << ELEM##_COUNT_BITS_LOG2);          \
   static_assert(ELEM##_COUNT_COLOR  == 1 << ELEM##_COUNT_COLOR_LOG2);         \
                                                                               \
   static_assert(ELEM##N##_WIDTH   == 1 << ELEM##N##_WIDTH_LOG2);              \
   static_assert(ELEM##N##_HEIGHT == 1 << ELEM##N##_HEIGHT_LOG2);              \
   static_assert(ELEM##N##_SIZE   == 1 << ELEM##N##_SIZE_LOG2);                \
   static_assert(ELEM##N##_COUNT_PIXELS == 1 << ELEM##N##_COUNT_PIXELS_LOG2);  \
   static_assert(ELEM##N##_COUNT_BITS   == 1 << ELEM##N##_COUNT_BITS_LOG2);    \
   static_assert(ELEM##N##_COUNT_COLOR  == 1 << ELEM##N##_COUNT_COLOR_LOG2);   \
                                                                               \
   static_assert(ELEM##N##_WIDTH  == ELEM##_WIDTH);                            \
   static_assert(ELEM##N##_HEIGHT == ELEM##_HEIGHT);                           \
   static_assert(ELEM##N##_SIZE  == ELEM##_SIZE);                              \
   static_assert(ELEM##N##_COUNT_PIXELS == ELEM##_COUNT_PIXELS);               \
   static_assert(ELEM##N##_COUNT_BITS   == ELEM##_COUNT_BITS);                 \
   static_assert(ELEM##N##_COUNT_COLOR  == ELEM##_COUNT_COLOR);                \
                                                                               \
   static_assert(ELEM##N##_WIDTH_LOG2  == ELEM##_WIDTH_LOG2);                  \
   static_assert(ELEM##N##_HEIGHT_LOG2 == ELEM##_HEIGHT_LOG2);                 \
   static_assert(ELEM##N##_SIZE_LOG2   == ELEM##_SIZE_LOG2);                   \
   static_assert(ELEM##N##_COUNT_PIXELS_LOG2 == ELEM##_COUNT_PIXELS_LOG2);     \
   static_assert(ELEM##N##_COUNT_BITS_LOG2   == ELEM##_COUNT_BITS_LOG2);       \
   static_assert(ELEM##N##_COUNT_COLOR_LOG2  == ELEM##_COUNT_COLOR_LOG2);      \


//------------------------------------------------------------------------------
// Static Assert TESTS for Glyphs
//------------------------------------------------------------------------------

#define GLYPH_STATIC_ASSERT_(N)                                                \
   STATIC_ASSERT_ELEMENT(GLYPH, N)                                             \
   static_assert(GLYPH##N##_SIZE == sizeof(Glyph##N));                         \
   static_assert(GLYPH##N##_COUNT_BITS == CHAR_BIT * sizeof(Glyph##N));

// Glyphs<N> modes
#undef  VIDEO_MODE
#define VIDEO_MODE VIDEO_MODE_GLYPH(8)
GLYPH_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE VIDEO_MODE_GLYPH(16)
GLYPH_STATIC_ASSERT_(16)
#undef  VIDEO_MODE
#define VIDEO_MODE VIDEO_MODE_GLYPH(32)
GLYPH_STATIC_ASSERT_(32)
#undef  VIDEO_MODE
#define VIDEO_MODE VIDEO_MODE_GLYPH(64)
GLYPH_STATIC_ASSERT_(64)
#undef  VIDEO_MODE
#define VIDEO_MODE VIDEO_MODE_GLYPH(128)
GLYPH_STATIC_ASSERT_(128)
#undef  VIDEO_MODE
#define VIDEO_MODE VIDEO_MODE_GLYPH(256)
GLYPH_STATIC_ASSERT_(256)

// Glyphs 8 and Planes
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(8, 1)
GLYPH_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(8, 2)
GLYPH_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(8, 3)
GLYPH_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(8, 4)
GLYPH_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(8, 5)
GLYPH_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(8, 6)
GLYPH_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(8, 8)
GLYPH_STATIC_ASSERT_(8)
// Glyphs 16 and Planes
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(16, 1)
GLYPH_STATIC_ASSERT_(16)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(16, 2)
GLYPH_STATIC_ASSERT_(16)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(16, 3)
GLYPH_STATIC_ASSERT_(16)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(16, 4)
GLYPH_STATIC_ASSERT_(16)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(16, 5)
GLYPH_STATIC_ASSERT_(16)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(16, 6)
GLYPH_STATIC_ASSERT_(16)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(16, 8)
GLYPH_STATIC_ASSERT_(16)
// Glyphs 32 and Planes
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(32, 1)
GLYPH_STATIC_ASSERT_(32)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(32, 2)
GLYPH_STATIC_ASSERT_(32)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(32, 3)
GLYPH_STATIC_ASSERT_(32)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(32, 4)
GLYPH_STATIC_ASSERT_(32)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(32, 5)
GLYPH_STATIC_ASSERT_(32)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(32, 6)
GLYPH_STATIC_ASSERT_(32)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(32, 8)
GLYPH_STATIC_ASSERT_(32)
// Glyphs 64 and Planes
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(64, 1)
GLYPH_STATIC_ASSERT_(64)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(64, 2)
GLYPH_STATIC_ASSERT_(64)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(64, 3)
GLYPH_STATIC_ASSERT_(64)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(64, 4)
GLYPH_STATIC_ASSERT_(64)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(64, 5)
GLYPH_STATIC_ASSERT_(64)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(64, 6)
GLYPH_STATIC_ASSERT_(64)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(64, 8)
GLYPH_STATIC_ASSERT_(64)
// Glyphs 128 and Planes
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(128, 1)
GLYPH_STATIC_ASSERT_(128)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(128, 2)
GLYPH_STATIC_ASSERT_(128)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(128, 3)
GLYPH_STATIC_ASSERT_(128)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(128, 4)
GLYPH_STATIC_ASSERT_(128)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(128, 5)
GLYPH_STATIC_ASSERT_(128)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(128, 6)
GLYPH_STATIC_ASSERT_(128)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(128, 8)
GLYPH_STATIC_ASSERT_(128)
// Glyphs 256 and Planes
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(256, 1)
GLYPH_STATIC_ASSERT_(256)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(256, 2)
GLYPH_STATIC_ASSERT_(256)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(256, 3)
GLYPH_STATIC_ASSERT_(256)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(256, 4)
GLYPH_STATIC_ASSERT_(256)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(256, 5)
GLYPH_STATIC_ASSERT_(256)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(256, 6)
GLYPH_STATIC_ASSERT_(256)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_PLANES(256, 8)
GLYPH_STATIC_ASSERT_(256)

// Glyphs 8 and Attributes
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_ATTRIBUTES(8, 0)
GLYPH_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_ATTRIBUTES(8, 1)
GLYPH_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_ATTRIBUTES(8, 2)
GLYPH_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_ATTRIBUTES(8, 3)
GLYPH_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_ATTRIBUTES(8, 4)
GLYPH_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_ATTRIBUTES(8, 5)
GLYPH_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_ATTRIBUTES(8, 6)
GLYPH_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_ATTRIBUTES(8, 7)
GLYPH_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_ATTRIBUTES(8, 8)
GLYPH_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_ATTRIBUTES(8, 9)
GLYPH_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_ATTRIBUTES(8, 10)
GLYPH_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_ATTRIBUTES(8, 11)
GLYPH_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_ATTRIBUTES(8, 12)
GLYPH_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_ATTRIBUTES(8, 13)
GLYPH_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_ATTRIBUTES(8, 14)
GLYPH_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_GLYPH_ATTRIBUTES(8, 15)
GLYPH_STATIC_ASSERT_(8)



//------------------------------------------------------------------------------
// Static Assert TESTS for Tiles
//------------------------------------------------------------------------------

#define TILE_STATIC_ASSERT_(N)                                                 \
   STATIC_ASSERT_ELEMENT(TILE, N)                                              \
   static_assert(TILE##N##_SIZE == sizeof(Tile##N));                           \
   static_assert(TILE##N##_COUNT_BITS == CHAR_BIT * sizeof(Tile##N));


// Test all Tile Modes

#undef  VIDEO_MODE
#define VIDEO_MODE TILE16_2x4
TILE_STATIC_ASSERT_(16)

#undef  VIDEO_MODE
#define VIDEO_MODE TILE32_2x4
TILE_STATIC_ASSERT_(32)
#undef  VIDEO_MODE
#define VIDEO_MODE TILE32_4x4
TILE_STATIC_ASSERT_(32)

#undef  VIDEO_MODE
#define VIDEO_MODE  TILE64_2x4
TILE_STATIC_ASSERT_(64)
#undef  VIDEO_MODE
#define VIDEO_MODE  TILE64_4x4
TILE_STATIC_ASSERT_(64)
#undef  VIDEO_MODE
#define VIDEO_MODE  TILE64_4x8
TILE_STATIC_ASSERT_(64)

#undef  VIDEO_MODE
#define VIDEO_MODE  TILE128_4x4
TILE_STATIC_ASSERT_(128)
#undef  VIDEO_MODE
#define VIDEO_MODE  TILE128_4x8
TILE_STATIC_ASSERT_(128)
#undef  VIDEO_MODE
#define VIDEO_MODE  TILE128_8x8
TILE_STATIC_ASSERT_(128)

#undef  VIDEO_MODE
#define VIDEO_MODE  TILE256_4x8
TILE_STATIC_ASSERT_(256)
#undef  VIDEO_MODE
#define VIDEO_MODE  TILE256_8x8
TILE_STATIC_ASSERT_(256)
#undef  VIDEO_MODE
#define VIDEO_MODE  TILE256_8x16
TILE_STATIC_ASSERT_(256)

//------------------------------------------------------------------------------
// Static Assert TESTS for Strips
//------------------------------------------------------------------------------

#define STRIP_STATIC_ASSERT_(N)                                                \
   STATIC_ASSERT_ELEMENT(STRIP, N)                                             \
                                                                               \
   static_assert((N == 8) || !STRIP_HAS_BITS_);                                \
   static_assert((N != 8) || STRIP_HAS_BITS_);                                 \
                                                                               \
   static_assert(STRIP##N##_SIZE == sizeof(Strip));                            \
   static_assert(STRIP##N##_COUNT_BITS == CHAR_BIT * sizeof(Strip));

// Strip<N> mode for N in 1,2,4, and 8(simple)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_STRIP(1)
STRIP_STATIC_ASSERT_(1)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_STRIP(2)
STRIP_STATIC_ASSERT_(2)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_STRIP(4)
STRIP_STATIC_ASSERT_(4)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_STRIP(8)
STRIP_STATIC_ASSERT_(8)

// Strip8 and planes
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_STRIP8_PLANES(1)
STRIP_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_STRIP8_PLANES(2)
STRIP_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_STRIP8_PLANES(3)
STRIP_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_STRIP8_PLANES(4)
STRIP_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_STRIP8_PLANES(5)
STRIP_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_STRIP8_PLANES(6)
STRIP_STATIC_ASSERT_(8)
// #undef  VIDEO_MODE
// #define VIDEO_MODE  VIDEO_MODE_STRIP8_PLANES(8)
// STRIP_STATIC_ASSERT_(8)

// Strip 8 and Attributes (There are 16 attribute full type, 0..15, test all)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_STRIP8_ATTRIBUTES(0)
STRIP_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_STRIP8_ATTRIBUTES(1)
STRIP_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_STRIP8_ATTRIBUTES(2)
STRIP_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_STRIP8_ATTRIBUTES(3)
STRIP_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_STRIP8_ATTRIBUTES(4)
STRIP_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_STRIP8_ATTRIBUTES(5)
STRIP_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_STRIP8_ATTRIBUTES(6)
STRIP_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_STRIP8_ATTRIBUTES(7)
STRIP_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_STRIP8_ATTRIBUTES(8)
STRIP_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_STRIP8_ATTRIBUTES(9)
STRIP_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_STRIP8_ATTRIBUTES(10)
STRIP_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_STRIP8_ATTRIBUTES(11)
STRIP_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_STRIP8_ATTRIBUTES(12)
STRIP_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_STRIP8_ATTRIBUTES(13)
STRIP_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_STRIP8_ATTRIBUTES(14)
STRIP_STATIC_ASSERT_(8)
#undef  VIDEO_MODE
#define VIDEO_MODE  VIDEO_MODE_STRIP8_ATTRIBUTES(15)
STRIP_STATIC_ASSERT_(8)



