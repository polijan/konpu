#ifndef  KONPU_FORM_H_
#define  KONPU_FORM_H_
#include "arch.h"

// Form Elements are the main elements in the framebuffer which contain Pixels
// They can be Glyphs or Tiles or Strips.
// ( When the pixels in those elements are bits, they are organized in one or
//   more bit Planes or complemented by Attributes. )


// Generic version of the Form element
typedef union {
   Form8   *form8,   *glyph8 ,  *strip;
   Form16  *form16,  *glyph16,  *tile16;
   Form32  *form32,  *glyph32,  *tile32;
   Form64  *form64,  *glyph64,  *tile64;
   Form128 *form128, *glyph128, *tile128;
   Form256 *form256, *glyph256;
   void    *ptr;
} Form;

// The Dimension of Form elements is given by the Video mode
#define FORM_DIMENSION    VideoModeDimension()

// true if Form's pixels are bits (Strip8 or Glyphs),
// false for "chunky" elements (Strip<1|2|4> or Tiles)
#define FORM_HAS_BITS_    (VideoModeHasAttributes() || VideoModeLowNibble() < 9)

// log2 of the number of bits per pixels
#define FORM_BPP_LOG2_    (FORM_HAS_BITS_ ? 0 : (VIDEO_MODE & 3))

// true iff for a Form elements are Strips, false if they are Glyphs or Tiles
#define FORM_IS_STRIP_    (VideoModeDimension() == PIXELS_Nx1)


//------------------------------------------------------------------------------
// Element measurement macros when we know that Form elements are either
// Glyph or Tiles(aka a "chunky Glyphs"), but not a Strip
//
// Glyphs and Tiles can be considered a single type of element. Glyphs' pixels
// are bits, wile Tiles pixels are quarter/nibble/bytes.
//------------------------------------------------------------------------------

#define GLYPH_OR_TILE_SIZE               (1 << GLYPH_OR_TILE_SIZE_LOG2)
#define GLYPH_OR_TILE_WIDTH              GLYPH_WIDTH
#define GLYPH_OR_TILE_HEIGHT             GLYPH_HEIGHT
#define GLYPH_OR_TILE_COUNT_BITS         (8 << GLYPH_OR_TILE_SIZE_LOG2)
#define GLYPH_OR_TILE_COUNT_PIXELS       GLYPH_COUNT_PIXELS
#define GLYPH_OR_TILE_COUNT_COLOR        (1 << GLYPH_OR_TILE_COUNT_COLOR_LOG2)
#define GLYPH_OR_TILE_SIZE_LOG2          (VideoModeDimension() + FORM_BPP_LOG2_)
#define GLYPH_OR_TILE_WIDTH_LOG2         GLYPH_WIDTH_LOG2
#define GLYPH_OR_TILE_HEIGHT_LOG2        GLYPH_HEIGHT_LOG2
#define GLYPH_OR_TILE_COUNT_BITS_LOG2    (3 + GLYPH_OR_TILE_SIZE_LOG2)
#define GLYPH_OR_TILE_COUNT_PIXELS_LOG2  GLYPH_COUNT_PIXELS_LOG2
#define GLYPH_OR_TILE_COUNT_COLOR_LOG2   (1 << FORM_BPP_LOG2_)


//------------------------------------------------------------------------------
// Form elements measurement macros in the general case
//------------------------------------------------------------------------------

#define FORM_SIZE               (1 << FORM_SIZE_LOG2)
#define FORM_WIDTH              (FORM_IS_STRIP_ ? STRIP_WIDTH : GLYPH_OR_TILE_WIDTH)
#define FORM_HEIGHT             (FORM_IS_STRIP_ ? 1 : GLYPH_OR_TILE_HEIGHT)
#define FORM_COUNT_BITS         (8 << FORM_SIZE_LOG2)
#define FORM_COUNT_PIXELS       (FORM_IS_STRIP_ ? STRIP_COUNT_PIXELS : GLYPH_OR_TILE_COUNT_PIXELS)
#define FORM_COUNT_COLOR        (1 << FORM_COUNT_COLOR_LOG2)
#define FORM_SIZE_LOG2          (FORM_IS_STRIP_ ? 0 : GLYPH_OR_TILE_SIZE_LOG2)
#define FORM_WIDTH_LOG2         (FORM_IS_STRIP_ ? STRIP_WIDTH_LOG2 : GLYPH_OR_TILE_WIDTH_LOG2)
#define FORM_HEIGHT_LOG2        (FORM_IS_STRIP_ ? 0 : GLYPH_OR_TILE_HEIGHT_LOG2)
#define FORM_COUNT_BITS_LOG2    (3 + FORM_SIZE_LOG2)
#define FORM_COUNT_PIXELS_LOG2  (FORM_IS_STRIP_ ? STRIP_COUNT_PIXELS_LOG2 : GLYPH_OR_TILE_COUNT_PIXELS_LOG2)
#define FORM_COUNT_COLOR_LOG2   (1 << FORM_BPP_LOG2_)


//------------------------------------------------------------------------------
// Form elements measurement macros when the Form<N> C type is known
// (Hint: Form8 is Glyph8 or a Strip / Form16 is Glyph16 or Tile16_2x4 /
//        Form32 width can be optimized and Form32+ are Glyph or Tile=
//------------------------------------------------------------------------------

// Form8  = Glyph8 (constant) or Strip
#define FORM8_SIZE                 1
#define FORM8_WIDTH                (FORM_IS_STRIP_ ? STRIP_WIDTH : GLYPH8_WIDTH)
#define FORM8_HEIGHT               (FORM_IS_STRIP_ ? 1 : GLYPH8_HEIGHT)
#define FORM8_COUNT_BITS           8
#define FORM8_COUNT_PIXELS         (8 >> FORM_BPP_LOG2_)
#define FORM8_COUNT_COLOR          FORM_COUNT_COLOR
#define FORM8_SIZE_LOG2            0
#define FORM8_WIDTH_LOG2           (FORM_IS_STRIP_ ? STRIP_WIDTH_LOG2 : GLYPH8_WIDTH_LOG2)
#define FORM8_HEIGHT_LOG2          (FORM_IS_STRIP_ ? 0 : GLYPH8_HEIGHT_LOG2)
#define FORM8_COUNT_BITS_LOG2      3
#define FORM8_COUNT_PIXELS_LOG2    (3 - FORM_BPP_LOG2_)
#define FORM8_COUNT_COLOR_LOG2     FORM_COUNT_COLOR_LOG2

// Form16 = Glyph16 or Tile16_2x4
#define FORM16_SIZE                2
#define FORM16_WIDTH               (2 << (VIDEO_MODE != TILE16_2x4))
#define FORM16_HEIGHT              4  // common Glyph16/Tile16
#define FORM16_COUNT_BITS          16
#define FORM16_COUNT_PIXELS        (8 << (VIDEO_MODE != TILE16_2x4))
#define FORM16_COUNT_COLOR         (2 << (VIDEO_MODE == TILE16_2x4))
#define FORM16_SIZE_LOG2           1
#define FORM16_WIDTH_LOG2          (1 << (VIDEO_MODE != TILE16_2x4))
#define FORM16_HEIGHT_LOG2         2 // common Glyph16/Tile16
#define FORM16_COUNT_BITS_LOG2     4
#define FORM16_COUNT_PIXELS_LOG2   (3 + (VIDEO_MODE != TILE16_2x4))
#define FORM16_COUNT_COLOR_LOG2    (1 << (VIDEO_MODE == TILE16_2x4))

#define FORM32_SIZE                4
#define FORM32_WIDTH               (2 << (VIDEO_MODE != TILE32_2x4))
#define FORM32_HEIGHT              GLYPH_OR_TILE_HEIGHT
#define FORM32_COUNT_BITS          32
#define FORM32_COUNT_PIXELS        GLYPH_OR_TILE_COUNT_PIXELS
#define FORM32_COUNT_COLOR         GLYPH_OR_TILE_COUNT_COLOR
#define FORM32_SIZE_LOG2           2
#define FORM32_WIDTH_LOG2          (1 + (VIDEO_MODE != TILE32_2x4))
#define FORM32_HEIGHT_LOG2         GLYPH_OR_TILE_HEIGHT_LOG2
#define FORM32_COUNT_BITS_LOG2     5
#define FORM32_COUNT_PIXELS_LOG2   GLYPH_OR_TILE_COUNT_PIXELS_LOG2
#define FORM32_COUNT_COLOR_LOG2    GLYPH_OR_TILE_COUNT_COLOR_LOG2

#define FORM64_SIZE                8
#define FORM64_WIDTH               GLYPH_OR_TILE_WIDTH
#define FORM64_HEIGHT              GLYPH_OR_TILE_HEIGHT
#define FORM64_COUNT_BITS          64
#define FORM64_COUNT_PIXELS        GLYPH_OR_TILE_COUNT_PIXELS
#define FORM64_COUNT_COLOR         GLYPH_OR_TILE_COUNT_COLOR
#define FORM64_SIZE_LOG2           3
#define FORM64_WIDTH_LOG2          GLYPH_OR_TILE_WIDTH_LOG2
#define FORM64_HEIGHT_LOG2         GLYPH_OR_TILE_HEIGHT_LOG2
#define FORM64_COUNT_BITS_LOG2     6
#define FORM64_COUNT_PIXELS_LOG2   GLYPH_OR_TILE_COUNT_PIXELS_LOG2
#define FORM64_COUNT_COLOR_LOG2    GLYPH_OR_TILE_COUNT_COLOR_LOG2

#define FORM128_SIZE               16
#define FORM128_WIDTH              GLYPH_OR_TILE_WIDTH
#define FORM128_HEIGHT             GLYPH_OR_TILE_HEIGHT
#define FORM128_COUNT_BITS         128
#define FORM128_COUNT_PIXELS       GLYPH_OR_TILE_COUNT_PIXELS
#define FORM128_COUNT_COLOR        GLYPH_OR_TILE_COUNT_COLOR
#define FORM128_SIZE_LOG2          4
#define FORM128_WIDTH_LOG2         GLYPH_OR_TILE_WIDTH_LOG2
#define FORM128_HEIGHT_LOG2        GLYPH_OR_TILE_HEIGHT_LOG2
#define FORM128_COUNT_BITS_LOG2    7
#define FORM128_COUNT_PIXELS_LOG2  GLYPH_OR_TILE_COUNT_PIXELS_LOG2
#define FORM128_COUNT_COLOR_LOG2   GLYPH_OR_TILE_COUNT_COLOR_LOG2

#define FORM256_SIZE               32
#define FORM256_WIDTH              GLYPH_OR_TILE_WIDTH
#define FORM256_HEIGHT             GLYPH_OR_TILE_HEIGHT
#define FORM256_COUNT_BITS         256
#define FORM256_COUNT_PIXELS       GLYPH_OR_TILE_COUNT_PIXELS
#define FORM256_COUNT_COLOR        GLYPH_OR_TILE_COUNT_COLOR
#define FORM256_SIZE_LOG2          5
#define FORM256_WIDTH_LOG2         GLYPH_OR_TILE_WIDTH_LOG2
#define FORM256_HEIGHT_LOG2        GLYPH_OR_TILE_HEIGHT_LOG2
#define FORM256_COUNT_BITS_LOG2    8
#define FORM256_COUNT_PIXELS_LOG2  GLYPH_OR_TILE_COUNT_PIXELS_LOG2
#define FORM256_COUNT_COLOR_LOG2   GLYPH_OR_TILE_COUNT_COLOR_LOG2


//------------------------------------------------------------------------------

// void FormClear([Video|Rectangle]);
// Set all the Forms to 0 in the given or active surface
// TODO: void GlyphForms([Video|Rectangle r], [int plane]);
#define FormClear(...)             UTIL_OVERLOAD(PrivateFormClear, __VA_ARGS__)
#  define PrivateFormClear_0_()    PrivateFormClear_1_(Video.active)
#  define PrivateFormClear_1_(surface)                                         \
      _Generic((surface)                                                     , \
         VideoSurface_: memset(Video.frame, 0, VIDEO_FORMS_SIZE)             , \
         Rectangle:     PrivateFormClear_Rectangle_(RECTANGLE((surface)))      \
      )
   void PrivateFormClear_Rectangle_(Rectangle r); // TODO

//------------------------------------------------------------------------------
/*
// Raw form access
Form VideoFormAt_(int x, int y) {
//   Video.frame
//   + (x + y * ELEMENT_WIDTH) << FORM_SIZE_LOG2
}
*/


#endif //include guard
