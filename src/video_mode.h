#ifndef  KONPU_VIDEO_MODE_H_
#define  KONPU_VIDEO_MODE_H_
#include "c.h"

//------------------------------------------------------------------------------
// Video Card Internals
//------------------------------------------------------------------------------

// Videocard Registers (Read-only from Konpu and not memory-mapped to RAM,
// but they are mapped to: VIDEO_MODE, _HEIGHT, _WIDTH for easy querying)
extern const uint8_t   *VIDEO_REGISTER_MODE_;
extern const int16_t   *VIDEO_REGISTER_WIDTH_;
extern const int16_t   *VIDEO_REGISTER_HEIGHT_;
extern const uint32_t  *VIDEO_REGISTER_ERRORS_;

// Videocard internal configuration (see: tools/resfinder)
#define VIDEO_FACTOR_  4 // implemented: 2,3,4,5,6,7
#define VIDEO_LCM_SZ_  2880

//------------------------------------------------------------------------------

// Size in bytes of the video framebuffer
#define VIDEO_SIZE     (VIDEO_FACTOR_ * VIDEO_LCM_SZ_)

// Current Video Width in Pixels
#define VIDEO_WIDTH    (*VIDEO_REGISTER_WIDTH_)

// Current Video Height in Pixels
#define VIDEO_HEIGHT   (*VIDEO_REGISTER_HEIGHT_)

#if (INT_WIDTH >= 32) || (VIDEO_FACTOR_ == 1)
//  Number of pixels in the framebuffer
#   define VIDEO_WxH_  (VIDEO_WIDTH * VIDEO_HEIGHT)
#else
#   define VIDEO_WxH_  ((int32_t)VIDEO_WIDTH * (int32_t)VIDEO_HEIGHT)
#endif

// Count of video errors when rendering the framebuffer
// (Note: an error at every frame at 60FPS takes >2.2 years to overflow it)
#define VIDEO_ERRORS   (*VIDEO_REGISTER_ERRORS_)

// Max possible framebuffer width/height achievable from any video mode
#if VIDEO_FACTOR_ == 7
#   define VIDEO_MAX_WIDTH   480
#   define VIDEO_MAX_HEIGHT  336
#elif VIDEO_FACTOR_ == 6
#   define VIDEO_MAX_WIDTH   480
#   define VIDEO_MAX_HEIGHT  288
#elif VIDEO_FACTOR_ == 5
#   define VIDEO_MAX_WIDTH   400
#   define VIDEO_MAX_HEIGHT  288
#elif VIDEO_FACTOR_ == 4
#   define VIDEO_MAX_WIDTH   320
#   define VIDEO_MAX_HEIGHT  288
#elif VIDEO_FACTOR_ == 3
#   define VIDEO_MAX_WIDTH   320
#   define VIDEO_MAX_HEIGHT  216
#elif VIDEO_FACTOR_ == 2
#   define VIDEO_MAX_WIDTH   288
#   define VIDEO_MAX_HEIGHT  160
#else
#   error "VIDEO_FACTOR_ invalid / not implemented"
#endif

// Current Video Mode
#define VIDEO_MODE    (*VIDEO_REGISTER_MODE_)
#ifdef KONPU_OPTION_OPTIMIZE_VIDEO_MODE
   // Normalize (to default mode) if the symbol is defined but has no value
#  if ~(~KONPU_OPTION_OPTIMIZE_VIDEO_MODE + 0) == 0 && \
      ~(~KONPU_OPTION_OPTIMIZE_VIDEO_MODE + 1) == 1
#     undef  KONPU_OPTION_OPTIMIZE_VIDEO_MODE
#     define KONPU_OPTION_OPTIMIZE_VIDEO_MODE  VIDEO_MODE_DEFAULT
#  endif
#  undef  VIDEO_MODE
#  define VIDEO_MODE  KONPU_OPTION_OPTIMIZE_VIDEO_MODE
#endif

//------------------------------------------------------------------------------
// Enums
//------------------------------------------------------------------------------

// Indicate the dimension in pixels of a video element
// (Glyphs, Tiles, Attributes, or Pixels)
enum VideoElementDimension {
   PIXELS_2x4   = 0,
   PIXELS_4x4   = 1,
   PIXELS_4x8   = 2,
   PIXELS_8x8   = 3, // value not possible for 8bpp Tiles
   PIXELS_8x16  = 4, // value not possible for Attributes or >=4bpp Tiles
   PIXELS_16x16 = 5, // value only possible for Glyphs (Glyph256)
   PIXELS       = 7, // value only possible for Pixels

   // note: when elements are glyphs, the value also represents the log2 of the
   //       size in bytes.
};

// Indicate the type of colors encoded by attributes in the framebuffer.
enum AttributeColorType {       //Bytes| Description
   ATTRIBUTE_COLORS_16    = 0,  //   1 | Nibbles specify a fg and bg 16-color
   ATTRIBUTE_COLORS_FG256 = 1,  //   1 | Byte specifies one foreground 256-color
   ATTRIBUTE_COLORS_BG256 = 2,  //   1 | Byte specifies one background 256-color
   ATTRIBUTE_COLORS_256   = 3   //   2 | Bytes specify a fg and bg 256-color
};

// The full list of the all 16 possible Attributes type.
// It is nibble value which is composed of two quarters:
// - an VideoElementDimension (max. 8x8)
// - and an AttributeColorType.
//                       .------ attributes px size (enum VideoElementDimension)
//                       |  .--- attributes color type (enum AttributeColorType)
//                       v  v
//                      |..|..|
//
// In attribute modes, it is the low nibble of the video mode.
enum AttributeType {
  ATTRIBUTE_2x4_16    = ((PIXELS_2x4 << 2) | ATTRIBUTE_COLORS_16),
  ATTRIBUTE_4x4_16    = ((PIXELS_4x4 << 2) | ATTRIBUTE_COLORS_16),
  ATTRIBUTE_4x8_16    = ((PIXELS_4x8 << 2) | ATTRIBUTE_COLORS_16),
  ATTRIBUTE_8x8_16    = ((PIXELS_8x8 << 2) | ATTRIBUTE_COLORS_16),
  ATTRIBUTE_2x4_FG256 = ((PIXELS_2x4 << 2) | ATTRIBUTE_COLORS_FG256),
  ATTRIBUTE_4x4_FG256 = ((PIXELS_4x4 << 2) | ATTRIBUTE_COLORS_FG256),
  ATTRIBUTE_4x8_FG256 = ((PIXELS_4x8 << 2) | ATTRIBUTE_COLORS_FG256),
  ATTRIBUTE_8x8_FG256 = ((PIXELS_8x8 << 2) | ATTRIBUTE_COLORS_FG256),
  ATTRIBUTE_2x4_BG256 = ((PIXELS_2x4 << 2) | ATTRIBUTE_COLORS_BG256),
  ATTRIBUTE_4x4_BG256 = ((PIXELS_4x4 << 2) | ATTRIBUTE_COLORS_BG256),
  ATTRIBUTE_4x8_BG256 = ((PIXELS_4x8 << 2) | ATTRIBUTE_COLORS_BG256),
  ATTRIBUTE_8x8_BG256 = ((PIXELS_8x8 << 2) | ATTRIBUTE_COLORS_BG256),
  ATTRIBUTE_2x4_256   = ((PIXELS_2x4 << 2) | ATTRIBUTE_COLORS_256),
  ATTRIBUTE_4x4_256   = ((PIXELS_4x4 << 2) | ATTRIBUTE_COLORS_256),
  ATTRIBUTE_4x8_256   = ((PIXELS_4x8 << 2) | ATTRIBUTE_COLORS_256),
  ATTRIBUTE_8x8_256   = ((PIXELS_8x8 << 2) | ATTRIBUTE_COLORS_256)
};

//------------------------------------------------------------------------------
// Definition of the VIDEO_MODE
//------------------------------------------------------------------------------
// The video mode describes the layout of the video framebuffer.
// It is encoded by a single byte with the following binary representation:
//
//    .-- Has Attributes (bit)
//    |
//    |  .--- Dimension  (enum VideoElementDimension)
//    |  |
//    |  |   .--- Low Nibble (nibble)
//    v  v   v
//   |.|...|....|
//
//
// The different parts of the video mode are:
//
// * Has Attributes: if this bit is set, the framebuffer is in attribute mode.
//
// * Dimension: an `enum VideoElementDimension` which describes the dimension in
//   pixels of the non-attribute elements of the framebuffer (ie. Glyphs, Tiles,
//   or indicate the presence of Pixels).
//
// * Low Nibble:
//
//   - If the Has Attribute bit is set, the low nibble describes an attribute
//     type (enum AttributeType). Reminder: this value is itself composed of two
//     quarters:
//                    .------ attributes dimension  (enum VideoElementDimension)
//                    |  .--- attributes color type (enum AttributeColorType)
//                    v  v
//                   |..|..|
//
//   - Othwerwise, the low nibble can (only) take the following values:
//
//     1...8  -> indicate a "planar" mode (with Glyphs or bit-Pixels)
//               and give the number of bitplanes.
//
//     9...11 -> indicate a "chunky" mode (with Tiles or (non-bit)-Pixels)
//               and the chunk and bpp/color depth of the element,
//               as follow:   9 -> quarter Pixels or Tiles (4 colors)
//                           10 -> nibble  Pixels or Tiles (16 colors)
//                           11 -> byte    Pixels or Tiles (256 colors)
//               Note: 1 << (low nibble - 8) is the number of bits per pixel,
//                     1 << (1 << (low nibble - 8)) is the number of colors
//------------------------------------------------------------------------------

// Return VIDEO_MODE's attribute bit. (ie. whether framebuffer has Attributes)
#define VideoModeHasAttributes()               (VIDEO_MODE >> 7)

// Return VIDEO_MODE's dimension (ie, the dimension in pixels of the framebuffer
// 's non-attribute elements) as an `enum VideoElementDimension`.
static inline enum VideoElementDimension
VideoModeDimension(void)                       { return (VIDEO_MODE >> 4) & 7; }

// Return VIDEO_MODE's low nibble
#define VideoModeLowNibble(void)               (VIDEO_MODE & 0xF)

//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------

// Same as the VideoMode() function, but do NOT change the video mode.
// It can be used for example to test if mode would be a valid video mode.
int VideoMode_(int mode);

// Set the video mode to the given mode (if valid)
// Return Value:
// - If the mode is invalid, return 0.
// - Otherwise a non-zero return value (which is the number of bytes required to
//   encode an 8x8 pixel area in the given mode).
#ifndef KONPU_OPTION_OPTIMIZE_VIDEO_MODE
   int VideoMode(int mode);
#else
   static inline int VideoMode(int mode)
   { return (mode == VIDEO_MODE)? VideoMode_(mode) : 0; }
   // When optimized, the video mode must not be changed.
#endif



//------------------------------------------------------------------------------
// Functions or macros to easily create a value that could be assigned to the
// video mode.
//------------------------------------------------------------------------------
//
//                              Layout chosen for coloring
//             .------------------.-------------------.-------------------------
//             | BitPlanes        | "Chunky" element  | Attributes
//-------------|------------------|-------------------|-------------------------
// Glyph/Tiles | GLYPH PLANES     | TILES             | GLYPHS + ATTRIBUTES
// Pixels      | bit-PIXEL PLANES | (non-bit)PIXELS   | bit-PIXELS + ATTRIBUTES
//-------------'------------------'-------------------'-------------------------

//------------------------------------------------------------------------------
// VIDEO_MODE*: macros or functions to specify a video mode value
//------------------------------------------------------------------------------
// VIDEO_MODE_DEFAULT
// VIDEO_MODE_(attribute_bit, dimension, low_nibble)
//
// VIDEO_MODE_GLYPH(GLYPH_TYPE)
// VIDEO_MODE_GLYPH_PLANES(GLYPH_TYPE, number_of_planes)
// VIDEO_MODE_GLYPH_ATTRIBUTES(GLYPH_TYPE, [enum AttributeType])
//
// VIDEO_MODE_PIXEL(bits_per_pixels)         // 1,2,4,8
// VIDEO_MODE_PIXEL_PLANES(number_of_plane)  // 1,2,3,4,5,6,8
// VIDEO_MODE_PIXEL_ATTRIBUTES(enum AttributeType)
//
// VIDEO_MODE_TILE(TILE_TYPE)
//------------------------------------------------------------------------------

// The default Video Mode (= Glyphs + 16-color Attributes, each 4x8-sized)
#define VIDEO_MODE_DEFAULT   168

// Return a video mode based on its three parts (as explained in the doc):
// - the attribute bit,
// - the dimension (enum VideoElementDimension)
// - the low_nibble
#define VIDEO_MODE_(attribute_bit, dimension, low_nibble) \
   ((int)((attribute_bit) << 7 | (dimension) << 4 | (low_nibble)))

// int VIDEO_MODE_GLYPH(GLYPH_TYPE)
// Return a video mode number based on a Glyph type with a single bit plane
#define VIDEO_MODE_GLYPH(GLYPH_TYPE) \
   VIDEO_MODE_GLYPH_PLANES(GLYPH_TYPE, 1)

// int VIDEO_MODE_GLYPH_PLANES(GLYPH_TYPE, int number_of_planes)
// Return a mode based on a Glyph type with the given amount of planes
// The number of planes should be 1,2,3,4,5,6,or 8.
#define VIDEO_MODE_GLYPH_PLANES(GLYPH_TYPE, number_of_planes) \
   VIDEO_MODE_(                                               \
      0,                                                      \
      _Generic((GLYPH_TYPE){0} ,                              \
         Glyph8:   PIXELS_2x4  ,                              \
         Glyph16:  PIXELS_4x4  ,                              \
         Glyph32:  PIXELS_4x8  ,                              \
         Glyph64:  PIXELS_8x8  ,                              \
         Glyph128: PIXELS_8x16 ,                              \
         Glyph256: PIXELS_16x16                               \
      ),                                                      \
      (number_of_planes)                                      \
   )

// int VIDEO_MODE_GLYPH_ATTRIBUTES(GLYPH_TYPE, [enum AttributeType attr_type]);
// Return a mode number based on a Glyph type and an enum AttributeType
// If the Attribute if left unspecified, it will default to 16-color attributes
// whose size in pixels is either same as the glyphs or at max 8x8.
#define VIDEO_MODE_GLYPH_ATTRIBUTES(...)                                       \
   UTIL_OVERLOAD(VIDEO_MODE_GLYPH_ATTRIBUTES, __VA_ARGS__)
   #define VIDEO_MODE_GLYPH_ATTRIBUTES_2_(GLYPH_TYPE, enum_attribute_type)     \
      VIDEO_MODE_(                                                             \
         1,                                                                    \
         _Generic((GLYPH_TYPE){0},                                             \
            Glyph8:   PIXELS_2x4,                                              \
            Glyph16:  PIXELS_4x4,                                              \
            Glyph32:  PIXELS_4x8,                                              \
            Glyph64:  PIXELS_8x8,                                              \
            Glyph128: PIXELS_8x16,                                             \
            Glyph256: PIXELS_16x16                                             \
         ),                                                                    \
         (enum_attribute_type)                                                 \
      )
   #define VIDEO_MODE_GLYPH_ATTRIBUTES_1_(GLYPH_TYPE)                          \
      VIDEO_MODE_GLYPH_ATTRIBUTES_2_(GLYPH_TYPE, ATTRIBUTE_COLORS_16 |         \
         _Generic((GLYPH_TYPE){0},                                             \
            Glyph8:   PIXELS_2x4,                                              \
            Glyph16:  PIXELS_4x4,                                              \
            Glyph32:  PIXELS_4x8,                                              \
            Glyph64:  PIXELS_8x8,                                              \
            Glyph128: PIXELS_8x8,                                              \
            Glyph256: PIXELS_8x8                                               \
         ) << 2                                                                \
      )

// Return a mode number with chunk pixels or (a single plane of) bit pixels
// based on the given bits per pixel, which should be 1,2,4,8.
static inline int VIDEO_MODE_PIXEL(int bits_per_pixel)
{ return VIDEO_MODE_(0, PIXELS,
      (bits_per_pixel == 1) ? 1 : (9 + (bits_per_pixel >> 2)) ); }

// Return a mode number based on bit pixels with the given amount of planes
// The number of planes should be 1,2,3,4,5,6,or 8.
static inline int VIDEO_MODE_PIXEL_PLANES(int number_of_planes)
{ return VIDEO_MODE_(0, PIXELS, (number_of_planes)); }

// Return a mode number with bit pixels and the bigven attribute type.
static inline int VIDEO_MODE_PIXEL_ATTRIBUTES(enum AttributeType attr_type)
{ return VIDEO_MODE_(1, PIXELS, attr_type); }

// Return a Tile mode with the given tile type
#define VIDEO_MODE_TILE(TILE_TYPE)   // TODO
/*
static inline
int VIDEO_MODE_TILE(enum VideoElementDimension tile_size, int bits_per_pixel) {
   assert( bits_per_pixel == 1                                   ||
          (bits_per_pixel == 2 && bits_per_pixel <= PIXELS_8x16) ||
          (bits_per_pixel == 4 && bits_per_pixel <= PIXELS_8x8)  ||
          (bits_per_pixel == 8 && bits_per_pixel <= PIXELS_4x8)  );
   return (bits_per_pixel == 1) ?
          VIDEO_MODE_GLYPH_BITPLANES(tile_size, 1) :
          VIDEO_MODE_GENERIC_(0, tile_size, bits_per_pixel + 7);
}
*/



//------------------------------------------------------------------------------
// This would be better some place else, but because of the way we construct
// the consolidated konpu.h, it needs to be here.
//------------------------------------------------------------------------------

// In attribute modes, return the VIDEO_MODE attributes' type
static inline enum AttributeColorType
AttributeColorType(void) {
   // assert(VideoModeHasAttributes());
   return VIDEO_MODE & 3;
}

// TODO: may I move this to color.h???
// Return the current color depth,
// ie. the log2 of the max. number of colors that can be in the framebuffer
static inline int ColorDepth()
{
   // Attribute modes
   if (VideoModeHasAttributes())
      return (AttributeColorType() == ATTRIBUTE_COLORS_16)? 4 : 8; // 16 or 256

   // Non-Attributes modes
   int n = VideoModeLowNibble();
   return (n <= 8)? n : 1 << (n - 8);
   // ^ This is because:
   //
   // Low   | Chunk and Number of Colors  | How that's expressed in terms of
   // Nibble|                             | the Low Nibble n.
   // ------|-----------------------------|----------------------------------
   //   1-8 | planar mode give 2^n color  --> = 1 << n
   //     9 | quarter chunk ->   4 colors --> = 1 << (1<<1) = 1 << (1 << n-8)
   //    10 | nibble chunk  ->  16 colors --> = 1 << (1<<2) = 1 << (1 << n-8)
   //    11 | byte          -> 256 colors --> = 1 << (1<<3) = 1 << (1 << n-8)
   // }
}


// This will define VIDEO_WIDTH and VIDEO_WEIGHT when not "FORCED" and in the
// case where the OPTIMIZE_VIDEO_MODE option is on.
#include "video_mode_auto.h"

#endif //include guard
