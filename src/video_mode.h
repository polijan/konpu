#ifndef  KONPU_VIDEO_MODE_H_
#define  KONPU_VIDEO_MODE_H_
#include "util.h"
#include "form.h"

// Konpu's Video Graphics Card whose memory and register are accessible via
// memory-mapped I/O. In Konpu API, `Video` can be passed everywhere a surface
// is expected.
#define Video  Ram.video


//------------------------------------------------------------------------------
// Possible Optimization options
//------------------------------------------------------------------------------

/* "OPTIMIZED" mode (TODO)

// Current Video Mode
#ifdef KONPU_OPTION_OPTIMIZE_VIDEO_MODE
   // Normalize (to default mode) if the symbol is defined but has no value
#  if ~(~KONPU_OPTION_OPTIMIZE_VIDEO_MODE + 0) == 0 && \
      ~(~KONPU_OPTION_OPTIMIZE_VIDEO_MODE + 1) == 1
#     undef  KONPU_OPTION_OPTIMIZE_VIDEO_MODE
#     define KONPU_OPTION_OPTIMIZE_VIDEO_MODE  168 // = VIDEO_MODE_DEFAULT
#  endif
#  undef  VIDEO_MODE
#  define VIDEO_MODE  KONPU_OPTION_OPTIMIZE_VIDEO_MODE
#endif
*/

// This will define VIDEO_WIDTH and VIDEO_WEIGHT when not "FORCED" and in the
// case where the OPTIMIZE_VIDEO_MODE option is on.
#include "video_mode_auto.h"

//------------------------------------------------------------------------------
// Element dimensions (in terms of pixels)
//------------------------------------------------------------------------------

// Value for "chunky" pixels when pixels aren't bits
enum PixelChunk {
   PIXEL_CHUNK_BYTE    = 11, // pixel is a byte representing a full color
   PIXEL_CHUNK_NIBBLE  = 10, // pixel is a nibble  and can have 16 colors
   PIXEL_CHUNK_QUARTER =  9, // pixel is a quarter and can have  4 colors

   // Note: We can observe that log2(#Colors) = 1 << (px_chunk - 8)
   //       or it can also be:  log2(#Colors) = 1 << (px_chunk & 3)
   //       |
   //       '--> and when the framebuffer contains contains chunky Forms, we can
   //            thus get Forms' bits-per-pixels with: 1 << (Video.mode & 3)
};

// Video framebuffer element dimensions in Pixels
// (Elements can be Forms (Glyphs, Tiles, Strips) or Attributes)
//
// When one knows an element is a Glyph, Tile, or Attribute, this enum can
// describe its dimensions exactly; otherwise, it can only indicate if the
// element is a Strip.
enum VideoElementDimension {
   PIXELS_2x4   = 0,
   PIXELS_4x4   = 1,
   PIXELS_4x8   = 2,
   PIXELS_8x8   = 3, // value not possible  for 8bpp Tiles
   PIXELS_8x16  = 4, // value not possible  for Attributes or >=4bpp Tiles
   PIXELS_16x16 = 5, // value only possible for Glyph256
   PIXELS_Nx1   = 7, // value only possible for Strips

   // Note: When an Video Element has a definite Dimension (i.e. 0..5), its
   //       Width and Height in pixels and its number of pixels can be obtained
   //       usging the following formulas:
   //
   //       #Pixels       =  8 << Dimension
   //       Width         =  2 << ((Dimension + 1) >> 1)
   //       Height        =  4 << (Dimension >> 1)
   //       log2(#Pixels) =  Dimension + 3
   //       log2(Width)   =  (Dimension + 3) >> 1
   //       log2(Height)  =  2 + (Dimension >> 1)
   //
   //       There are "measurement" macros (based on the Video.mode and those
   //       formula (or even optimized)) for every Element type.

   // Note 2: When elements are Glyphs, the value also represents the
   //         (log2 of) their size in bytes.

};


#define VIDEO_ELEMENT_2x4_WIDTH                2
#define VIDEO_ELEMENT_2x4_HEIGHT               4
#define VIDEO_ELEMENT_2x4_COUNT_PIXELS         8
#define VIDEO_ELEMENT_2x4_WIDTH_LOG2           1
#define VIDEO_ELEMENT_2x4_HEIGHT_LOG2          2
#define VIDEO_ELEMENT_2x4_COUNT_PIXELS_LOG2    3

#define VIDEO_ELEMENT_4x4_WIDTH                4
#define VIDEO_ELEMENT_4x4_HEIGHT               4
#define VIDEO_ELEMENT_4x4_COUNT_PIXELS         16
#define VIDEO_ELEMENT_4x4_WIDTH_LOG2           2
#define VIDEO_ELEMENT_4x4_HEIGHT_LOG2          2
#define VIDEO_ELEMENT_4x4_COUNT_PIXELS_LOG2    4

#define VIDEO_ELEMENT_4x8_WIDTH                4
#define VIDEO_ELEMENT_4x8_HEIGHT               8
#define VIDEO_ELEMENT_4x8_COUNT_PIXELS         32
#define VIDEO_ELEMENT_4x8_WIDTH_LOG2           2
#define VIDEO_ELEMENT_4x8_HEIGHT_LOG2          3
#define VIDEO_ELEMENT_4x8_COUNT_PIXELS_LOG2    5

#define VIDEO_ELEMENT_8x8_WIDTH                8
#define VIDEO_ELEMENT_8x8_HEIGHT               8
#define VIDEO_ELEMENT_8x8_COUNT_PIXELS         64
#define VIDEO_ELEMENT_8x8_WIDTH_LOG2           3
#define VIDEO_ELEMENT_8x8_HEIGHT_LOG2          3
#define VIDEO_ELEMENT_8x8_COUNT_PIXELS_LOG2    6

#define VIDEO_ELEMENT_8x16_WIDTH               8
#define VIDEO_ELEMENT_8x16_HEIGHT              16
#define VIDEO_ELEMENT_8x16_COUNT_PIXELS        128
#define VIDEO_ELEMENT_8x16_WIDTH_LOG2          3
#define VIDEO_ELEMENT_8x16_HEIGHT_LOG2         4
#define VIDEO_ELEMENT_8x16_COUNT_PIXELS_LOG2   7

#define VIDEO_ELEMENT_16x16_WIDTH              16
#define VIDEO_ELEMENT_16x16_HEIGHT             16
#define VIDEO_ELEMENT_16x16_COUNT_PIXELS       256
#define VIDEO_ELEMENT_16x16_WIDTH_LOG2         4
#define VIDEO_ELEMENT_16x16_HEIGHT_LOG2        4
#define VIDEO_ELEMENT_16x16_COUNT_PIXELS_LOG2  8

#define VIDEO_ELEMENT_Nx1_HEIGHT               1
#define VIDEO_ELEMENT_Nx1_HEIGHT_LOG2          0

//------------------------------------------------------------------------------
// Measurements of the Video framebuffer
//
// VIDEO_WIDTH_<ELEMENT>   width  of the framebuffer in terms of that element
// VIDEO_HEIGHT_<ELEMENT>  height of the framebuffer in terms of that element
// VIDEO_COUNT_<ELEMENT>   number of such elements in the framebuffer
//
// Video mode must be such that the framebuffer contains this given ELEMENT.
//
// VIDEO_WIDTH, VIDEO_HEIGHT
// Width/Height of the video framebuffer in pixels (This same as Video.width
// and Video.height unless some optimization option is on.
//------------------------------------------------------------------------------

// TODO: Maybe we want to add the suffix _PIXEL. This makes the name longer, but
//       also but more logical and consistent with the VIDEO_*_<ELEMENT>
//       convention. And when using VIDEO_WIDTH, we're foced to
#ifndef VIDEO_WIDTH
#define VIDEO_WIDTH   Video.width
#endif
#ifndef VIDEO_HEIGHT
#define VIDEO_HEIGHT  Video.height
#endif

#ifndef VIDEO_WIDTH_PIXEL
#define VIDEO_WIDTH_PIXEL           Video.width
#endif
#define VIDEO_WIDTH_FORM            (VIDEO_WIDTH >> FORM_WIDTH_LOG2)
#define VIDEO_WIDTH_FORM8           (VIDEO_WIDTH >> FORM8_WIDTH_LOG2)
#define VIDEO_WIDTH_FORM16          (VIDEO_WIDTH >> FORM16_WIDTH_LOG2)
#define VIDEO_WIDTH_FORM32          (VIDEO_WIDTH >> FORM32_WIDTH_LOG2)
#define VIDEO_WIDTH_FORM64          (VIDEO_WIDTH >> FORM64_WIDTH_LOG2)
#define VIDEO_WIDTH_FORM128         (VIDEO_WIDTH >> FORM128_WIDTH_LOG2)
#define VIDEO_WIDTH_FORM256         (VIDEO_WIDTH >> FORM256_WIDTH_LOG2)
#define VIDEO_WIDTH_FORM_2x4        (VIDEO_WIDTH >> VIDEO_ELEMENT_2x4_WIDTH_LOG2)
#define VIDEO_WIDTH_FORM_4x4        (VIDEO_WIDTH >> VIDEO_ELEMENT_4x4_WIDTH_LOG2)
#define VIDEO_WIDTH_FORM_4x8        (VIDEO_WIDTH >> VIDEO_ELEMENT_4x8_WIDTH_LOG2)
#define VIDEO_WIDTH_FORM_8x8        (VIDEO_WIDTH >> VIDEO_ELEMENT_8x8_WIDTH_LOG2)
#define VIDEO_WIDTH_FORM_8x16       (VIDEO_WIDTH >> VIDEO_ELEMENT_8x16_WIDTH_LOG2)
#define VIDEO_WIDTH_FORM_16x16      (VIDEO_WIDTH >> VIDEO_ELEMENT_16x16_WIDTH_LOG2)
#define VIDEO_WIDTH_FORM_Nx1        (VIDEO_WIDTH >> STRIP_WIDTH_LOG2)
#define VIDEO_WIDTH_GLYPH           (VIDEO_WIDTH >> GLYPH_WIDTH_LOG2)
#define VIDEO_WIDTH_GLYPH8          (VIDEO_WIDTH >> GLYPH8_WIDTH_LOG2)
#define VIDEO_WIDTH_GLYPH16         (VIDEO_WIDTH >> GLYPH16_WIDTH_LOG2)
#define VIDEO_WIDTH_GLYPH32         (VIDEO_WIDTH >> GLYPH32_WIDTH_LOG2)
#define VIDEO_WIDTH_GLYPH64         (VIDEO_WIDTH >> GLYPH64_WIDTH_LOG2)
#define VIDEO_WIDTH_GLYPH128        (VIDEO_WIDTH >> GLYPH128_WIDTH_LOG2)
#define VIDEO_WIDTH_GLYPH256        (VIDEO_WIDTH >> GLYPH256_WIDTH_LOG2)
#define VIDEO_WIDTH_TILE            (VIDEO_WIDTH >> TILE_WIDTH_LOG2)
#define VIDEO_WIDTH_TILE_2x4        (VIDEO_WIDTH >> VIDEO_ELEMENT_2x4_WIDTH_LOG2)
#define VIDEO_WIDTH_TILE_4x4        (VIDEO_WIDTH >> VIDEO_ELEMENT_4x4_WIDTH_LOG2)
#define VIDEO_WIDTH_TILE_4x8        (VIDEO_WIDTH >> VIDEO_ELEMENT_4x8_WIDTH_LOG2)
#define VIDEO_WIDTH_TILE_8x8        (VIDEO_WIDTH >> VIDEO_ELEMENT_8x8_WIDTH_LOG2)
#define VIDEO_WIDTH_TILE_8x16       (VIDEO_WIDTH >> VIDEO_ELEMENT_8x16_WIDTH_LOG2)
#define VIDEO_WIDTH_TILE16          (VIDEO_WIDTH >> TILE16_WIDTH_LOG2)
#define VIDEO_WIDTH_TILE32          (VIDEO_WIDTH >> TILE32_WIDTH_LOG2)
#define VIDEO_WIDTH_TILE64          (VIDEO_WIDTH >> TILE64_WIDTH_LOG2)
#define VIDEO_WIDTH_TILE128         (VIDEO_WIDTH >> TILE128_WIDTH_LOG2)
#define VIDEO_WIDTH_TILE256         (VIDEO_WIDTH >> TILE256_WIDTH_LOG2)
#define VIDEO_WIDTH_STRIP           (VIDEO_WIDTH >> STRIP_WIDTH_LOG2)
#define VIDEO_WIDTH_STRIP1          (VIDEO_WIDTH >> STRIP1_WIDTH_LOG2)
#define VIDEO_WIDTH_STRIP2          (VIDEO_WIDTH >> STRIP2_WIDTH_LOG2)
#define VIDEO_WIDTH_STRIP4          (VIDEO_WIDTH >> STRIP4_WIDTH_LOG2)
#define VIDEO_WIDTH_STRIP8          (VIDEO_WIDTH >> STRIP8_WIDTH_LOG2)
#define VIDEO_WIDTH_STRIP_CHUNKY    (VIDEO_WIDTH >> STRIP_WIDTH_CHUNKY_LOG2)
#define VIDEO_WIDTH_ATTRIBUTE       (VIDEO_WIDTH >> ATTRIBUTE_WIDTH_LOG2)
#define VIDEO_WIDTH_ATTRIBUTE8      (VIDEO_WIDTH >> ATTRIBUTE8_WIDTH_LOG2)
#define VIDEO_WIDTH_ATTRIBUTE16     (VIDEO_WIDTH >> ATTRIBUTE16_WIDTH_LOG2)
#define VIDEO_WIDTH_ATTRIBUTE_2x4   (VIDEO_WIDTH >> VIDEO_ELEMENT_2x4_WIDTH_LOG2)
#define VIDEO_WIDTH_ATTRIBUTE_4x4   (VIDEO_WIDTH >> VIDEO_ELEMENT_4x4_WIDTH_LOG2)
#define VIDEO_WIDTH_ATTRIBUTE_4x8   (VIDEO_WIDTH >> VIDEO_ELEMENT_4x8_WIDTH_LOG2)
#define VIDEO_WIDTH_ATTRIBUTE_8x8   (VIDEO_WIDTH >> VIDEO_ELEMENT_8x8_WIDTH_LOG2)

#ifndef VIDEO_HEIGHT_PIXEL
#define VIDEO_HEIGHT_PIXEL          Video.height
#endif
#define VIDEO_HEIGHT_FORM           (VIDEO_HEIGHT >> FORM_HEIGHT_LOG2)
#define VIDEO_HEIGHT_FORM8          (VIDEO_HEIGHT >> FORM8_HEIGHT_LOG2)
#define VIDEO_HEIGHT_FORM16         (VIDEO_HEIGHT >> FORM16_HEIGHT_LOG2)
#define VIDEO_HEIGHT_FORM32         (VIDEO_HEIGHT >> FORM32_HEIGHT_LOG2)
#define VIDEO_HEIGHT_FORM64         (VIDEO_HEIGHT >> FORM64_HEIGHT_LOG2)
#define VIDEO_HEIGHT_FORM128        (VIDEO_HEIGHT >> FORM128_HEIGHT_LOG2)
#define VIDEO_HEIGHT_FORM256        (VIDEO_HEIGHT >> FORM256_HEIGHT_LOG2)
#define VIDEO_HEIGHT_FORM_2x4       (VIDEO_HEIGHT >> VIDEO_ELEMENT_2x4_HEIGHT_LOG2)
#define VIDEO_HEIGHT_FORM_4x4       (VIDEO_HEIGHT >> VIDEO_ELEMENT_4x4_HEIGHT_LOG2)
#define VIDEO_HEIGHT_FORM_4x8       (VIDEO_HEIGHT >> VIDEO_ELEMENT_4x8_HEIGHT_LOG2)
#define VIDEO_HEIGHT_FORM_8x8       (VIDEO_HEIGHT >> VIDEO_ELEMENT_8x8_HEIGHT_LOG2)
#define VIDEO_HEIGHT_FORM_8x16      (VIDEO_HEIGHT >> VIDEO_ELEMENT_8x16_HEIGHT_LOG2)
#define VIDEO_HEIGHT_FORM_16x16     (VIDEO_HEIGHT >> VIDEO_ELEMENT_16x16_HEIGHT_LOG2)
#define VIDEO_HEIGHT_FORM_Nx1       VIDEO_HEIGHT
#define VIDEO_HEIGHT_GLYPH          (VIDEO_HEIGHT >> GLYPH_HEIGHT_LOG2)
#define VIDEO_HEIGHT_GLYPH8         (VIDEO_HEIGHT >> GLYPH8_HEIGHT_LOG2)
#define VIDEO_HEIGHT_GLYPH16        (VIDEO_HEIGHT >> GLYPH16_HEIGHT_LOG2)
#define VIDEO_HEIGHT_GLYPH32        (VIDEO_HEIGHT >> GLYPH32_HEIGHT_LOG2)
#define VIDEO_HEIGHT_GLYPH64        (VIDEO_HEIGHT >> GLYPH64_HEIGHT_LOG2)
#define VIDEO_HEIGHT_GLYPH128       (VIDEO_HEIGHT >> GLYPH128_HEIGHT_LOG2)
#define VIDEO_HEIGHT_GLYPH256       (VIDEO_HEIGHT >> GLYPH256_HEIGHT_LOG2)
#define VIDEO_HEIGHT_TILE           (VIDEO_HEIGHT >> TILE_HEIGHT_LOG2)
#define VIDEO_HEIGHT_TILE_2x4       (VIDEO_HEIGHT >> VIDEO_ELEMENT_2x4_HEIGHT_LOG2)
#define VIDEO_HEIGHT_TILE_4x4       (VIDEO_HEIGHT >> VIDEO_ELEMENT_4x4_HEIGHT_LOG2)
#define VIDEO_HEIGHT_TILE_4x8       (VIDEO_HEIGHT >> VIDEO_ELEMENT_4x8_HEIGHT_LOG2)
#define VIDEO_HEIGHT_TILE_8x8       (VIDEO_HEIGHT >> VIDEO_ELEMENT_8x8_HEIGHT_LOG2)
#define VIDEO_HEIGHT_TILE_8x16      (VIDEO_HEIGHT >> VIDEO_ELEMENT_8x16_HEIGHT_LOG2)
#define VIDEO_HEIGHT_TILE16         (VIDEO_HEIGHT >> TILE16_HEIGHT_LOG2)
#define VIDEO_HEIGHT_TILE32         (VIDEO_HEIGHT >> TILE32_HEIGHT_LOG2)
#define VIDEO_HEIGHT_TILE64         (VIDEO_HEIGHT >> TILE64_HEIGHT_LOG2)
#define VIDEO_HEIGHT_TILE128        (VIDEO_HEIGHT >> TILE128_HEIGHT_LOG2)
#define VIDEO_HEIGHT_TILE256        (VIDEO_HEIGHT >> TILE256_HEIGHT_LOG2)
#define VIDEO_HEIGHT_STRIP          (VIDEO_HEIGHT >> STRIP_HEIGHT_LOG2)
#define VIDEO_HEIGHT_STRIP1         (VIDEO_HEIGHT >> STRIP1_HEIGHT_LOG2)
#define VIDEO_HEIGHT_STRIP2         (VIDEO_HEIGHT >> STRIP2_HEIGHT_LOG2)
#define VIDEO_HEIGHT_STRIP4         (VIDEO_HEIGHT >> STRIP4_HEIGHT_LOG2)
#define VIDEO_HEIGHT_STRIP8         (VIDEO_HEIGHT >> STRIP8_HEIGHT_LOG2)
#define VIDEO_HEIGHT_STRIP_CHUNKY   (VIDEO_HEIGHT >> STRIP_HEIGHT_CHUNKY_LOG2)
#define VIDEO_HEIGHT_ATTRIBUTE      (VIDEO_HEIGHT >> ATTRIBUTE_HEIGHT_LOG2)
#define VIDEO_HEIGHT_ATTRIBUTE8     (VIDEO_HEIGHT >> ATTRIBUTE8_HEIGHT_LOG2)
#define VIDEO_HEIGHT_ATTRIBUTE16    (VIDEO_HEIGHT >> ATTRIBUTE16_HEIGHT_LOG2)
#define VIDEO_HEIGHT_ATTRIBUTE_2x4  (VIDEO_HEIGHT >> VIDEO_ELEMENT_2x4_HEIGHT_LOG2)
#define VIDEO_HEIGHT_ATTRIBUTE_4x4  (VIDEO_HEIGHT >> VIDEO_ELEMENT_4x4_HEIGHT_LOG2)
#define VIDEO_HEIGHT_ATTRIBUTE_4x8  (VIDEO_HEIGHT >> VIDEO_ELEMENT_4x8_HEIGHT_LOG2)
#define VIDEO_HEIGHT_ATTRIBUTE_8x8  (VIDEO_HEIGHT >> VIDEO_ELEMENT_8x8_HEIGHT_LOG2)


#if (INT_WIDTH >= 32) || (VIDEO_FACTOR_ == 1)
#   define VIDEO_COUNT_PIXELS       (VIDEO_WIDTH * VIDEO_HEIGHT)
#else
#   define VIDEO_COUNT_PIXELS       ((int32_t)VIDEO_WIDTH * (int32_t)VIDEO_HEIGHT)
#endif
#define VIDEO_COUNT_PLANE           VideoModeLowNibble()
#define VIDEO_COUNT_BITS            (8 * VIDEO_SIZE)
#define VIDEO_COUNT_COLOR           (1 << VIDEO_COUNT_COLOR_LOG2)
#define VIDEO_COUNT_COLOR_LOG2      ColorDepth()

#define VIDEO_COUNT_GLYPH           (VIDEO_COUNT_PIXELS >> GLYPH_COUNT_PIXELS_LOG2)
#define VIDEO_COUNT_GLYPH8          (VIDEO_COUNT_PIXELS >> GLYPH8_COUNT_PIXELS_LOG2)
#define VIDEO_COUNT_GLYPH16         (VIDEO_COUNT_PIXELS >> GLYPH16_COUNT_PIXELS_LOG2)
#define VIDEO_COUNT_GLYPH32         (VIDEO_COUNT_PIXELS >> GLYPH32_COUNT_PIXELS_LOG2)
#define VIDEO_COUNT_GLYPH64         (VIDEO_COUNT_PIXELS >> GLYPH64_COUNT_PIXELS_LOG2)
#define VIDEO_COUNT_GLYPH128        (VIDEO_COUNT_PIXELS >> GLYPH128_COUNT_PIXELS_LOG2)
#define VIDEO_COUNT_GLYPH256        (VIDEO_COUNT_PIXELS >> GLYPH256_COUNT_PIXELS_LOG2)
#define VIDEO_COUNT_TILE            (VIDEO_COUNT_PIXELS >> TILE_COUNT_PIXELS_LOG2)
#define VIDEO_COUNT_TILE_2x4        (VIDEO_COUNT_PIXELS >> VIDEO_ELEMENT_2x4_COUNT_PIXELS_LOG2)
#define VIDEO_COUNT_TILE_4x4        (VIDEO_COUNT_PIXELS >> VIDEO_ELEMENT_4x4_COUNT_PIXELS_LOG2)
#define VIDEO_COUNT_TILE_4x8        (VIDEO_COUNT_PIXELS >> VIDEO_ELEMENT_4x8_COUNT_PIXELS_LOG2)
#define VIDEO_COUNT_TILE_8x8        (VIDEO_COUNT_PIXELS >> VIDEO_ELEMENT_8x8_COUNT_PIXELS_LOG2)
#define VIDEO_COUNT_TILE_8x16       (VIDEO_COUNT_PIXELS >> VIDEO_ELEMENT_8x16_COUNT_PIXELS_LOG2)
#define VIDEO_COUNT_TILE16          (VIDEO_COUNT_PIXELS >> TILE16_COUNT_PIXELS_LOG2)
#define VIDEO_COUNT_TILE32          (VIDEO_COUNT_PIXELS >> TILE32_COUNT_PIXELS_LOG2)
#define VIDEO_COUNT_TILE64          (VIDEO_COUNT_PIXELS >> TILE64_COUNT_PIXELS_LOG2)
#define VIDEO_COUNT_TILE128         (VIDEO_COUNT_PIXELS >> TILE128_COUNT_PIXELS_LOG2)
#define VIDEO_COUNT_TILE256         (VIDEO_COUNT_PIXELS >> TILE256_COUNT_PIXELS_LOG2)
#define VIDEO_COUNT_STRIP           (VIDEO_COUNT_PIXELS >> STRIP_COUNT_PIXELS_LOG2)
#define VIDEO_COUNT_STRIP1          (VIDEO_COUNT_PIXELS >> STRIP1_COUNT_PIXELS_LOG2)
#define VIDEO_COUNT_STRIP2          (VIDEO_COUNT_PIXELS >> STRIP2_COUNT_PIXELS_LOG2)
#define VIDEO_COUNT_STRIP4          (VIDEO_COUNT_PIXELS >> STRIP4_COUNT_PIXELS_LOG2)
#define VIDEO_COUNT_STRIP8          (VIDEO_COUNT_PIXELS >> STRIP8_COUNT_PIXELS_LOG2)
#define VIDEO_COUNT_STRIP_CHUNKY    (VIDEO_COUNT_PIXELS >> STRIP_COUNT_PIXELS_CHUNKY_LOG2)
#define VIDEO_COUNT_ATTRIBUTE       (VIDEO_COUNT_PIXELS >> ATTRIBUTE_COUNT_PIXELS_LOG2)
#define VIDEO_COUNT_ATTRIBUTE_2x4   (VIDEO_COUNT_PIXELS >> VIDEO_ELEMENT_2x4_COUNT_PIXELS_LOG2)
#define VIDEO_COUNT_ATTRIBUTE_4x4   (VIDEO_COUNT_PIXELS >> VIDEO_ELEMENT_4x4_COUNT_PIXELS_LOG2)
#define VIDEO_COUNT_ATTRIBUTE_4x8   (VIDEO_COUNT_PIXELS >> VIDEO_ELEMENT_4x8_COUNT_PIXELS_LOG2)
#define VIDEO_COUNT_ATTRIBUTE_8x8   (VIDEO_COUNT_PIXELS >> VIDEO_ELEMENT_8x8_COUNT_PIXELS_LOG2)
#define VIDEO_COUNT_ATTRIBUTE8      (VIDEO_COUNT_PIXELS >> ATTRIBUTE8_COUNT_PIXELS_LOG2)
#define VIDEO_COUNT_ATTRIBUTE16     (VIDEO_COUNT_PIXELS >> ATTRIBUTE16_COUNT_PIXELS_LOG2)


//------------------------------------------------------------------------------
// Size in bytes of framebuffer regions containing a given Element type
//
// Remember that it only make sense to use a macro if the frambuffer actually
// contains the associated regions (for example, if video doesn't have planes,
// dont' use VIDEO_SIZE_PLANE /  or
// attributes, then VIDEO_SIZE_PLANE and VIDEO_SIZE_ATTRIBUTES aren't 0)
//------------------------------------------------------------------------------

#define VIDEO_FORMS_SIZE            (VIDEO_COUNT_PIXELS >> 3)
#define VIDEO_GLYPHS_SIZE           VIDEO_FORMS_SIZE
#define VIDEO_TILES_SIZE            VIDEO_SIZE // tiles never have attributes
#define VIDEO_STRIPS_SIZE           VIDEO_FORMS_SIZE

#define VIDEO_ATTRIBUTES_SIZE       (VIDEO_SIZE - VIDEO_FORMS_SIZE)

// Start of the Attributes in the Video framebuffer
// (only make sense if the framebuffer has attributes)
#define VIDEO_ATTRIBUTE_START_    (Video.frame + VIDEO_FORMS_SIZE)
#define VIDEO_ATTRIBUTE8_START_   VIDEO_ATTRIBUTE_START_
#define VIDEO_ATTRIBUTE16_START_  ((Attribute16 *)VIDEO_ATTRIBUTE_START_)

#define VIDEO_SIZE_PLANE            (VIDEO_SIZE / VIDEO_COUNT_PLANE)
   // WARNING: In video modes with AttributeType `0` (aka ATTRIBUTE8_2x4), the
   //          video mode low nibble is 0, which make this macro invokes a
   //          division by 0. Of course, one should use this macro only when bit
   //          planes are actually possible, but it is important to note this
   //          particularly nasty case.



//------------------------------------------------------------------------------
// Views to Video elements of the framebuffer as 2D variably sized arrays.
//
// The VIDEO_<ELEMENT>(...) functions/macros return a row-major 2D array, where
// the Element at position (x,y) is accessed as: VIDEO_<ELEMENT>(...)[y][x].
//------------------------------------------------------------------------------


// VideoArray  VIDEO_FORM(N, [int plane], [int x0, y0]);
// Return a 2D array of Form<N> giving an UNCHECKED access to the Forms elements
//  in the video framebuffer.
// - N: a literal <8|16|32|64|128|256> to specify the type of Form
// - plane: specify a plane (planar modes only)
// - x0,y0: specify an origin where the array starts
#define VIDEO_FORM(...) \
   UTIL_OVERLOAD(PRIVATE_VIDEO_FORM, __VA_ARGS__)
#  define PRIVATE_VIDEO_FORM_1_(N) \
      ((Form##N (*)[VIDEO_WIDTH_FORM##N])(Video.frame))
#  define PRIVATE_VIDEO_FORM_2_(N, plane)  (                                   \
      (Form##N (*)[VIDEO_WIDTH_FORM##N])                                       \
         (Video.frame + (plane) * VIDEO_SIZE_PLANE)                            \
   )
#  define PRIVATE_VIDEO_FORM_3_(N, x0,y0)  (                                   \
      (Form##N (*)[VIDEO_WIDTH_FORM##N])                                       \
         ( Video.frame                                                         \
         + ((x0) + (y0) * VIDEO_WIDTH_FORM##N) * sizeof(Form##N)               \
         )                                                                     \
   )
#  define PRIVATE_VIDEO_FORM_4_(N, plane, x0,y0)  (                            \
      (Form##N (*)[VIDEO_WIDTH_FORM##N])                                       \
         ( Video.frame                                                         \
         + (plane) * VIDEO_SIZE_PLANE                                          \
         + ((x0) + (y0) * VIDEO_WIDTH_FORM##N) * sizeof(Form##N)               \
         )                                                                     \
   )
   // Implementation note: one could have thought to only define VIDEO_FORM_4_
   // and do:   #define VIDEO_FORM_1_(N)          VIDEO_FORM_4_(N, 0, 0,0)
   //           #define VIDEO_FORM_2_(N, plane)   VIDEO_FORM_4_(N, plane, 0,0)
   //           #define VIDEO_FORM_3_(N, x0,y0)   VIDEO_FORM_4_(N, 0, x0,y0)
   // but we would risk potential division by 0 by invoking VIDEO_SIZE_PLANE in
   // a non-plane mode. Also expended macros are easier to understand.


// VideoArray  VIDEO_GLYPH(N, [int plane], [int x0, y0]);
// Return a 2D array of Glyph<N> giving an UNCHECKED access to the Glyphs in the
// video framebuffer.
// - N: a literal <8|16|32|64|128|256> to specify the type of Glyphs
// - plane: specify a plane (planar modes only)
// - x0,y0: specify an origin where the array starts
#define VIDEO_GLYPH(...) \
   UTIL_OVERLOAD(PRIVATE_VIDEO_GLYPH, __VA_ARGS__)
#  define PRIVATE_VIDEO_GLYPH_1_(N) \
      ((Glyph##N (*)[VIDEO_WIDTH_GLYPH##N])(Video.frame))
#  define PRIVATE_VIDEO_GLYPH_2_(N, plane)  (                                  \
      (Glyph##N (*)[VIDEO_WIDTH_GLYPH##N])                                     \
         (Video.frame + (plane) * VIDEO_SIZE_PLANE)                            \
   )
#  define PRIVATE_VIDEO_GLYPH_3_(N, x0,y0)  (                                  \
      (Glyph##N (*)[VIDEO_WIDTH_GLYPH##N])                                     \
         (  Video.frame                                                        \
         + ((x0) + (y0) * VIDEO_WIDTH_GLYPH##N) * sizeof(Glyph##N)             \
         )                                                                     \
   )
#  define PRIVATE_VIDEO_GLYPH_4_(N, plane, x0,y0)  (                           \
      (Glyph##N (*)[VIDEO_WIDTH_GLYPH##N])                                     \
         (  Video.frame                                                        \
         + (plane) * VIDEO_SIZE_PLANE                                          \
         + ((x0) + (y0) * VIDEO_WIDTH_GLYPH##N) * sizeof(Glyph##N)             \
         )                                                                     \
   )
   // Implementation note: one could have thought to only define VIDEO_GLYPH_4_
   // and do:   #define VIDEO_GLYPH_1_(N)          VIDEO_GLYPH_4_(N, 0, 0,0)
   //           #define VIDEO_GLYPH_2_(N, plane)   VIDEO_GLYPH_4_(N, plane, 0,0)
   //           #define VIDEO_GLYPH_3_(N, x0,y0)   VIDEO_GLYPH_4_(N, 0, x0,y0)
   // but we would risk potential division by 0 by invoking VIDEO_SIZE_PLANE in
   // a non-plane mode. Also expended macros are easier to understand.

// VideoArray  VIDEO_TILE(N, [int x0, y0]);
// Return a 2D array of Tile<N> giving an UNCHECKED access to the Glyphs in the
// video framebuffer.
// - N: a literal <16|32|64|128|256> to specify the C type of Tiles
// - x0,y0: specify an origin where the array starts
#define VIDEO_TILE(...) \
   UTIL_OVERLOAD(PRIVATE_VIDEO_TILE, __VA_ARGS__)
#  define PRIVATE_VIDEO_TILE_1_(N) \
      ((Tile##N (*)[VIDEO_WIDTH_TILE##N])(Video.frame))
#  define PRIVATE_VIDEO_TILE_3_(N, x0,y0)  (                                   \
      (Tile##N (*)[VIDEO_WIDTH_TILE##N])                                       \
         (Video.frame + ((x0) + (y0) * VIDEO_WIDTH_TILE##N) * sizeof(Tile##N)) \
   )

// VideoArray  VIDEO_STRIP([int plane], [int x0, y0]);
// Return a 2D array giving an UNCHECKED access to the Strips in the video
// framebuffer.
// - plane: specify a plane (only for Strip8 (aka bit Strips) in planar mode)
// - x0,y0: specify an origin where the array starts
#define VIDEO_STRIP(...) \
   UTIL_OVERLOAD(PRIVATE_VIDEO_STRIP, __VA_ARGS__)
#  define PRIVATE_VIDEO_STRIP_0_() \
      ((Strip (*)[VIDEO_WIDTH_STRIP]) (Video.strip))
#  define PRIVATE_VIDEO_STRIP_2_(x0, y0)  (                                    \
      (Strip (*)[VIDEO_WIDTH_STRIP])                                           \
         (Video.strip + (x0) + (y0) * VIDEO_WIDTH_STRIP)                       \
   )
#  define PRIVATE_VIDEO_STRIP_1_(plane)   (                                    \
      (Strip (*)[VIDEO_WIDTH_STRIP8])   /* Using planes, thus assume STRIP8 */ \
         (Video.strip + (plane) * VIDEO_SIZE_PLANE)                            \
   )
#  define PRIVATE_VIDEO_STRIP_3_(plane, x0, y0)  (                             \
      (Strip (*)[VIDEO_WIDTH_STRIP8])   /* Using planes, thus assume STRIP8 */ \
         ( Video.strip                                                         \
         + (plane) * VIDEO_SIZE_PLANE                                          \
         + (x0) + (y0) * VIDEO_WIDTH_STRIP8                                    \
         )                                                                     \
   )

// auto VIDEO_ATTRIBUTE(N, [int x0, y0])
// Return a 2D array of Attribute<N> giving an UNCHECKED access to the
// Attributes in the video framebuffer.
// - N: a literal <8|16> to specify the type of Attribute
// - x0,y0: specify an origin where the array starts
#define VIDEO_ATTRIBUTE(...) \
   UTIL_OVERLOAD(PRIVATE_VIDEO_ATTRIBUTE, __VA_ARGS__)
#  define PRIVATE_VIDEO_ATTRIBUTE_1_(N) \
      ((Attribute##N (*)[VIDEO_WIDTH_ATTRIBUTE]) VIDEO_ATTRIBUTE_START_)
#  define PRIVATE_VIDEO_ATTRIBUTE_3_(N, x0,y0)  (                              \
      (Attribute##N (*)[VIDEO_WIDTH_ATTRIBUTE])                                \
         ( VIDEO_ATTRIBUTE_START_                                              \
         + ((x0) + (y0) * VIDEO_WIDTH_ATTRIBUTE) * sizeof(Attribute##N)        \
         )                                                                     \
   )


#if __STDC_VERSION__ >= 202311L
   // Automatic-type for a 2D array with raw access to video elements in the
   // framebuffer. (Note: only use inside a scope, not for a global variable)
#  define VideoArray  auto
#elif defined(__GNUC__)
#  define VideoArray  __auto_type
#endif

// TODO LATER: If/When Konpu moves to C23, Remove this whole thing!
//
// When using the above functions to store a 2D array into a variable, it is
// easier to declare that variable as `VideoArray`. But this is only available
// in C23 or above; prior to C23, one can use the more specific VideoArray...
// types, but those MUST to be defined wihtin a scope (NOT at global level), one
// can use the VIDEO_USE_ARRAY_TYPES macro for this. Also make sure not to
// change the video mode in that scope.
#define VIDEO_USE_ARRAY_TYPES                                                  \
   /* Forms */                                                                 \
   typedef Form8    (*VideoArrayGlyph8) [VIDEO_WIDTH_FORM8];                   \
   typedef Form16   (*VideoArrayGlyph16)[VIDEO_WIDTH_FORM16];                  \
   typedef Form32   (*VideoArrayGlyph32)[VIDEO_WIDTH_FORM32];                  \
   typedef Form64   (*VideoArrayGlyph64)[VIDEO_WIDTH_FORM64];                  \
   typedef Form128  (*VideoArrayGlyph128)[VIDEO_WIDTH_FORM128];                \
   typedef Form256  (*VideoArrayGlyph256)[VIDEO_WIDTH_FORM256];                \
   /* Glyphs */                                                                \
   typedef Glyph8   (*VideoArrayGlyph8) [VIDEO_WIDTH_GLYPH8];                  \
   typedef Glyph16  (*VideoArrayGlyph16)[VIDEO_WIDTH_GLYPH16];                 \
   typedef Glyph32  (*VideoArrayGlyph32)[VIDEO_WIDTH_GLYPH32];                 \
   typedef Glyph64  (*VideoArrayGlyph64)[VIDEO_WIDTH_GLYPH64];                 \
   typedef Glyph128 (*VideoArrayGlyph128)[VIDEO_WIDTH_GLYPH128];               \
   typedef Glyph256 (*VideoArrayGlyph256)[VIDEO_WIDTH_GLYPH256];               \
   /* Tiles */                                                                 \
   typedef Tile16   (*VideoArrayTile16)[VIDEO_WIDTH_TILE16];                   \
   typedef Tile32   (*VideoArrayTile32)[VIDEO_WIDTH_TILE32];                   \
   typedef Tile64   (*VideoArrayTile64)[VIDEO_WIDTH_TILE64];                   \
   typedef Tile128  (*VideoArrayTile128)[VIDEO_WIDTH_TILE128];                 \
   typedef Tile256  (*VideoArrayTile256)[VIDEO_WIDTH_TILE256];                 \
   /* Strips */                                                                \
   typedef Strip    (*VideoArrayStrip)[VIDEO_WIDTH_STRIP];                     \
   /* Attributes */                                                            \
   typedef Attribute8  (*VideoArrayAttribute8) [VIDEO_WIDTH_ATTRIBUTE8];       \
   typedef Attribute16 (*VideoArrayAttribute16)[VIDEO_WIDTH_ATTRIBUTE16];

/*
   typedef Strip1 (*VideoArrayStrip1)[VIDEO_WIDTH_STRIP1];                     \
   typedef Strip2 (*VideoArrayStrip2)[VIDEO_WIDTH_STRIP2];                     \
   typedef Strip4 (*VideoArrayStrip4)[VIDEO_WIDTH_STRIP4];                     \
   typedef Strip8 (*VideoArrayStrip8)[VIDEO_WIDTH_STRIP8];                     \
*/


//------------------------------------------------------------------------------
// Definition of the Video.mode
//------------------------------------------------------------------------------
// The video mode describes the layout of the Video framebuffer.
// It is encoded by a single byte with the following binary representation:
//
//    .-- HasAttributes (single bit, 0 or 1)
//    |
//    |  .--- Dimension (enum VideoElementDimension)
//    |  |
//    |  |   .--- Low Nibble (nibble)
//    v  v   v
//   |.|...|....|
//
//
// The different parts of the video mode are:
//
// * HasAttributes: if this bit is set, the framebuffer also contains Attributes
//
// * Dimension: describes (as an `enum VideoElementDimension`) the dimensions of
//   the Form elements (ie non-Attributes) of the framebuffer. It gives the
//   exact size in pixels of Glyphs or Tiles or indicates the presence of Strips
//
// * Low Nibble:
//
//   - If the HasAttribute bit is set, the low nibble describes the Attributes
//     type (enum AttributeType). Reminder: this value is itself composed of two
//     quarters:
//                    .------ Attributes dimension  (enum VideoElementDimension)
//                    |  .--- Attributes color type (enum AttributeColorType)
//                    v  v
//                   |..|..|
//
//   - Othwerwise, the low nibble can (only) take the following values:
//
//     1...8  -> indicate a "planar" mode (with Glyphs or Strip8)
//               and give the number of bitplanes.
//
//     9...11 -> indicate a "chunky" mode (with Tiles or (non-bit)-Strips)
//               and the chunk and color depth of the element,
//               as follow:   9 -> quarter: Strip4 or Tiles (4 colors)
//                           10 -> nibble:  Strip2 or Tiles (16 colors)
//                           11 -> byte:    Strip1 or Tiles (256 colors)
//               Note: In that case, the bits-per-pixels can be expressed as
//                     1 << (low_nibble - 8)  or  1 << (low_nibble & 3)
//                                 which is also  1 << (Video.mode & 3)
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Note: Representing the Video mode with an 8-bit struct instead of a byte
//       would have simplified the code, but bit-fields are a bit "broken" in C
//       as alignment, field orders, and packing are implementation dependent.
//
//       // Works in GCC, CLANG, and many other compilers, but is not standard:
//       typedef struct alignas(uint8_t) __attribute__((packed)) {
//          uint8_t  has_attribute  : 1;
//          uint8_t  form_dimension : 3;
//          uint8_t  low_nibble     : 4;
//       } VideoMode;
//       static_assert( sizeof(struct VideoMode) ==  sizeof(uint8_t));
//       static_assert(alignof(struct VideoMode) == alignof(uint8_t));
//------------------------------------------------------------------------------

#ifndef   VIDEO_MODE
#  define VIDEO_MODE  Video.mode
#endif

// Return the Video Mode's HasAttribute bit.
// (ie. whether the framebuffer also contains Attributes)
#define VideoModeHasAttributes()               (VIDEO_MODE >> 7)

// Return the Video Mode's dimension (i.e., the dimension in pixels of the
// framebuffer's non-Attribute elements) as an `enum VideoElementDimension`.
#define VideoModeDimension()                   ((VIDEO_MODE >> 4) & 7)

// Return the Video Mode's low nibble. It may represent the Attributes' type or
// the number of bit planes, or indicate the type of chunky pixels used by Form
// elements. (see video mode description)
#define VideoModeLowNibble()                   (VIDEO_MODE & 0xF)


//------------------------------------------------------------------------------
// Functions or macros to easily create a value that could be assigned to the
// video mode.
//------------.-----------------------------------------------------------------
//            |                 Method used for coloring:
//    Bare    |-------------------.-------------------.-------------------------
//  Elements  |    Bit Planes     | "Chunky" Elements |       Attributes
//------------|-------------------|-------------------|-------------------------
// Glyph/Tile |  Glyph Planes     | Tiles             | Glyphs + Attributes
//------------|-------------------|-------------------|-------------------------
//   Strip    |  Strip8 Planes    | Strip<1|2|4>      | Strip8 (bit strips)
//            |  (bit strips)     | (chunky strips)   | + Attributes
//------------'-------------------'-------------------'-------------------------

//------------------------------------------------------------------------------
// VIDEO_MODE_*: functions/macros to construct that could be assigned to the
// video mode.
//------------------------------------------------------------------------------
// // The default Video mode
// #define VIDEO_MODE_DEFAULT  ...
//
// // Return a mode number for Glyph<N> video modes
// uint8_t VIDEO_MODE_GLYPH(literal N);
// uint8_t VIDEO_MODE_GLYPH_PLANES(literal N, int number_of_planes);
// uint8_t VIDEO_MODE_GLYPH_ATTRIBUTES(lit. N, [enum AttributeType attr_type]);
//
// // Return a mode number for Tile video modes
// uint8_t VIDEO_MODE_TILE(enum TileType tile_type);
//
// // Return a mode number for Strip<N> video modes
// uint8_t VIDEO_MODE_STRIP(literal N);
// uint8_t VIDEO_MODE_STRIP8_PLANES(int number_of_planes);
// uint8_t VIDEO_MODE_STRIP8_ATTRIBUTES(enum AttributeType attr_type);
//------------------------------------------------------------------------------

// The default Video Mode (= Glyph32 with ATTRIBUTE8_4x8)
#define VIDEO_MODE_DEFAULT   168

// Return a video mode based on its three parts (as explained in the doc):
// - the Attribute Bit  (0 or 1),
// - the Form Dimension (value as in `enum VideoElementDimension`)
// - the Low Nibble (Attributes type, # of Planes, or Forms' chunck ID)
#define VIDEO_MODE_FROM_PARTS_(attribute_bit, form_dimension, low_nibble)      \
   ((uint8_t)((attribute_bit) << 7 | (form_dimension) << 4 | (low_nibble)))

// uint8_t VIDEO_MODE_GLYPH(literal N);
// Return a video mode number based on a Glyph<N> type with a single bit plane
#define VIDEO_MODE_GLYPH(N) \
   VIDEO_MODE_GLYPH_PLANES(N, 1)

// uint8_t VIDEO_MODE_GLYPH_PLANES(literal N, int number_of_planes);
// Return a mode based on a Glyph<N> type with the given amount of planes
// The number of planes should be 1,2,3,4,5,6,or 8.
#define VIDEO_MODE_GLYPH_PLANES(N, number_of_planes)                           \
   VIDEO_MODE_FROM_PARTS_(                                                     \
      0,                                                                       \
      _Generic((Glyph##N){0}   ,                                               \
         Glyph8:   PIXELS_2x4  ,                                               \
         Glyph16:  PIXELS_4x4  ,                                               \
         Glyph32:  PIXELS_4x8  ,                                               \
         Glyph64:  PIXELS_8x8  ,                                               \
         Glyph128: PIXELS_8x16 ,                                               \
         Glyph256: PIXELS_16x16                                                \
      ),                                                                       \
      (number_of_planes)                                                       \
   )

// uint8_t VIDEO_MODE_GLYPH_ATTRIBUTES(literal N, [enum AttributeType type]);
// Return a mode number based on a Glyph<N> type and an enum AttributeType
// If the Attribute type is left unspecified, it will default to 16-color
// attributes whose size in pixels is either same as the glyphs or at max 8x8.
#define VIDEO_MODE_GLYPH_ATTRIBUTES(...)                                       \
   UTIL_OVERLOAD(PRIVATE_VIDEO_MODE_GLYPH_ATTRIBUTES, __VA_ARGS__)
   #define PRIVATE_VIDEO_MODE_GLYPH_ATTRIBUTES_2_(N, enum_attribute_type)      \
      VIDEO_MODE_FROM_PARTS_(                                                  \
         1,                                                                    \
         _Generic((Glyph##N){0},                                               \
            Glyph8:   PIXELS_2x4,                                              \
            Glyph16:  PIXELS_4x4,                                              \
            Glyph32:  PIXELS_4x8,                                              \
            Glyph64:  PIXELS_8x8,                                              \
            Glyph128: PIXELS_8x16,                                             \
            Glyph256: PIXELS_16x16                                             \
         ),                                                                    \
         (enum_attribute_type)                                                 \
      )
   #define PRIVATE_VIDEO_MODE_GLYPH_ATTRIBUTES_1_(N)                           \
      PRIVATE_VIDEO_MODE_GLYPH_ATTRIBUTES_2_(N, ATTRIBUTE8_COLOR |             \
         _Generic((Glyph##N){0},                                               \
            Glyph8:   PIXELS_2x4,                                              \
            Glyph16:  PIXELS_4x4,                                              \
            Glyph32:  PIXELS_4x8,                                              \
            Glyph64:  PIXELS_8x8,                                              \
            Glyph128: PIXELS_8x8,                                              \
            Glyph256: PIXELS_8x8                                               \
         ) << 2                                                                \
      )

// uint8_t VIDEO_MODE_TILE(enum TileType tile_type);
// Return a Tile mode with the given tile type
#define VIDEO_MODE_TILE(enum_TileType) \
   ((uint8_t)(enum_TileType))

// uint8_t VIDEO_MODE_STRIP(literal N);
// Return a Strip<N> mode, N should be <1|2|4|8>
#define VIDEO_MODE_STRIP(N)                                                    \
   VIDEO_MODE_FROM_PARTS_(0, PIXELS_Nx1, UTIL_ARG_AT(N,                        \
      1, PIXEL_CHUNK_BYTE, PIXEL_CHUNK_NIBBLE, 1, PIXEL_CHUNK_QUARTER, 1,1,1,1 ,1)
      // 1:byte, 2:nibble, 4:quarter, 0|3|5-8: single plane of bit-Strip (=1)

// uint8_t VIDEO_MODE_STRIP8_PLANES(int number_of_planes);
// Return a mode number for Strip8 (aka bit strips) arranged in planes
// (whose given amount be 1,2,3,4,5,6,or 8).
#define VIDEO_MODE_STRIP8_PLANES(number_of_planes) \
   VIDEO_MODE_FROM_PARTS_(0, PIXELS_Nx1, (number_of_planes))

// uint8_t VIDEO_MODE_STRIP8_ATTRIBUTES(enum AttributeType attr_type);
// Return a mode number for Strip8 (aka bit strips) colored by Attributes
#define VIDEO_MODE_STRIP8_ATTRIBUTES(enum_AttributeType) \
   VIDEO_MODE_FROM_PARTS_(1, PIXELS_Nx1, (enum_AttributeType))

//------------------------------------------------------------------------------
// Misc. Video functions
//------------------------------------------------------------------------------

// Rectangles filling the whole screen

#define VIDEO_RECTANGLE_PIXEL \
   ((Rectangle){ 0,0 , VIDEO_WIDTH, VIDEO_HEIGHT })
#define VIDEO_RECTANGLE_FORM \
   ((Rectangle){ 0,0 , VIDEO_WIDTH_FORM, VIDEO_HEIGHT_FORM })
#define VIDEO_RECTANGLE_GLYPH \
   ((Rectangle){ 0,0 , VIDEO_WIDTH_GLYPH, VIDEO_HEIGHT_GLYPH })
#define VIDEO_RECTANGLE_TILE  \
   ((Rectangle){ 0,0 , VIDEO_WIDTH_TILE, VIDEO_HEIGHT_TILE })
#define VIDEO_RECTANGLE_STRIP \
   ((Rectangle){ 0,0 , VIDEO_WIDTH_STRIP, VIDEO_HEIGHT_STRIP })
#define VIDEO_RECTANGLE_ATTRIBUTE \
   ((Rectangle){ 0,0 , VIDEO_WIDTH_ATTRIBUTE, VIDEO_HEIGHT_ATTRIBUTE })


//------------------------------------------------------------------------------
// Misc. Video functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Functions
//
// // Set the Video mode. (Return: 0 if mode is invalid, or the number of bytes
// //                      required to encode an 8x8 pixel area in the new mode)
// int VideoMode(int mode);             // Also clear the framebuffer
// int VideoMode_DoNotClear_(int mode); // Do not clear the framebuffer
// int VideoMode_DryRun_(int mode);     // Do nothing except return a value
//
// // Clear the frambuffer (or some elements of it):
// void VideoClear();
// void VideoClearGlyphs();
// void VideoClearTiles();
// void VideoClearStrips();
// void VideoClearAttributes();
// void VideoClearPlane(int plane);
//
// void VideoReset();
// void VideoRender();

//------------------------------------------------------------------------------

// Zero-out all Tiles of the Video framebuffer
static inline void VideoClearTiles(void)
{ memset(Video.frame, 0, VIDEO_TILES_SIZE); }

// TODO: Delete as this should be same as GlyphClear(Video);
// Zero-out all Glyphs of the Video framebuffer
static inline void VoidClearGlyphs(void)
{ memset(Video.frame, 0, VIDEO_GLYPHS_SIZE); }

// Zero-out all Strips of the Video framebuffer
static inline void VideoClearStrips(void)
{ memset(Video.frame, 0, VIDEO_STRIPS_SIZE); }

// Zero-out the given plane
static inline void VideoClearPlane(int plane) {
   if (plane >= 0 && plane < VIDEO_COUNT_PLANE)
      memset(Video.frame + plane * VIDEO_SIZE_PLANE, 0, VIDEO_SIZE_PLANE);
}

// Set all Attributes of the Video framebuffer to default pen and/or paper
void VideoClearAttributes(void);

// Clear out the framebuffer: Set non-Attributes elements to zero and all
// Attributes to the default pen and/or paper
void VideoClear(void);


// Reset color palette, the default mode, empty the framebuffer, etc.
void VideoReset(void);

// Render the video framebuffer on screen.
// Return value may be non-zero on error (in which case VIDEO_RENDER_ERRRORS
// counter will also be increased by one).
int VideoRender(void);

// Same as the VideoMode() but DO NOT actually change the Video mode nor clear
// the screen. It still is useful to get the return value. For example, it may
// be used to test if a given value would make a valid video mode.
int VideoMode_DryRun_(int mode);

#ifndef KONPU_OPTION_OPTIMIZE_VIDEO_MODE
   // Same as VideoMode(), but do NOT clear the framebuffer.
   int VideoMode_DoNotClear_(int mode);
#else
   static inline int VideoMode_DoNotClear_(int mode)
   { return (mode == VIDEO_MODE) ? VideoMode_DryRun_(mode) : 0; }
   // When optimized, the video mode must not be changed.
#endif

// Set the Video mode to the given mode (if valid) and clear the framebuffer.
// Return Value:
// - If the mode is invalid, return 0.
// - Otherwise a non-zero return value (which is the number of bytes required
//   to encode an 8x8 pixel area in the given mode).
static inline int VideoMode(int mode)
{
   int return_value = VideoMode_DoNotClear_(mode);
   VideoClear();
   return return_value;
}


#endif //include guard
