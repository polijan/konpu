#ifndef  KONPU_VIDEO_MODE_H_
#define  KONPU_VIDEO_MODE_H_
#include "arch.h"

//------------------------------------------------------------------------------
// Enums
//------------------------------------------------------------------------------

// Indicate the dimension in pixels of a video element
// (Glyphs, Tiles, PixelStrips, or Attributes)
enum VideoElementDimension {
   PIXELS_2x4   = 0,
   PIXELS_4x4   = 1,
   PIXELS_4x8   = 2,
   PIXELS_8x8   = 3, // value not possible  for 8bpp Tiles
   PIXELS_8x16  = 4, // value not possible  for Attributes or >=4bpp Tiles
   PIXELS_16x16 = 5, // value only possible for Glyphs (Glyph256)
   PIXELS_Nx1   = 7, // value only possible for PixelStrips

   // Note: When elements are Glyphs, the value also represents the
   //       log2 of their size in bytes.
};

// Indicate the type of colors encoded by Attributes in the framebuffer.
enum AttributeColorType {      // Bytes| Description
   ATTRIBUTE_COLORS_16       = 0, // 1 | Nibbles specify 16-color pen and paper
   ATTRIBUTE_COLORS_PEN256   = 1, // 1 | Byte specifies the pen   as full color
   ATTRIBUTE_COLORS_PAPER256 = 2, // 1 | Byte specifies the paper as full color
   ATTRIBUTE_COLORS_256      = 3  // 2 | Bytes specify full color pen and paper
};

// The full list of the all 16 possible Attributes type.
// It is a nibble value composed of two quarters:
// - an VideoElementDimension (max. 8x8)
// - and an AttributeColorType.
//                       .------ attributes px size (enum VideoElementDimension)
//                       |  .--- attributes color type (enum AttributeColorType)
//                       v  v
//                      |..|..|
//
// In attribute modes, it is the low nibble of the video mode.
enum AttributeType {
   ATTRIBUTE_2x4_16       = ((PIXELS_2x4 << 2) | ATTRIBUTE_COLORS_16),
   ATTRIBUTE_2x4_PEN256   = ((PIXELS_2x4 << 2) | ATTRIBUTE_COLORS_PEN256),
   ATTRIBUTE_2x4_PAPER256 = ((PIXELS_2x4 << 2) | ATTRIBUTE_COLORS_PAPER256),
   ATTRIBUTE_2x4_256      = ((PIXELS_2x4 << 2) | ATTRIBUTE_COLORS_256),
   ATTRIBUTE_4x4_16       = ((PIXELS_4x4 << 2) | ATTRIBUTE_COLORS_16),
   ATTRIBUTE_4x4_PEN256   = ((PIXELS_4x4 << 2) | ATTRIBUTE_COLORS_PEN256),
   ATTRIBUTE_4x4_PAPER256 = ((PIXELS_4x4 << 2) | ATTRIBUTE_COLORS_PAPER256),
   ATTRIBUTE_4x4_256      = ((PIXELS_4x4 << 2) | ATTRIBUTE_COLORS_256),
   ATTRIBUTE_4x8_16       = ((PIXELS_4x8 << 2) | ATTRIBUTE_COLORS_16),
   ATTRIBUTE_4x8_PEN256   = ((PIXELS_4x8 << 2) | ATTRIBUTE_COLORS_PEN256),
   ATTRIBUTE_4x8_PAPER256 = ((PIXELS_4x8 << 2) | ATTRIBUTE_COLORS_PAPER256),
   ATTRIBUTE_4x8_256      = ((PIXELS_4x8 << 2) | ATTRIBUTE_COLORS_256),
   ATTRIBUTE_8x8_16       = ((PIXELS_8x8 << 2) | ATTRIBUTE_COLORS_16),
   ATTRIBUTE_8x8_PEN256   = ((PIXELS_8x8 << 2) | ATTRIBUTE_COLORS_PEN256),
   ATTRIBUTE_8x8_PAPER256 = ((PIXELS_8x8 << 2) | ATTRIBUTE_COLORS_PAPER256),
   ATTRIBUTE_8x8_256      = ((PIXELS_8x8 << 2) | ATTRIBUTE_COLORS_256)
};


//------------------------------------------------------------------------------
// Dimensions of the video framebuffer
//------------------------------------------------------------------------------

/*
// Current Video Mode
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
*/

#ifndef VIDEO_MODE
#define VIDEO_MODE    Video.mode
#endif
#ifndef VIDEO_WIDTH
#define VIDEO_WIDTH   Video.width
#endif
#ifndef VIDEO_HEIGHT
#define VIDEO_HEIGHT  Video.height
#endif



// VIDEO_WIDTH_<element>: Width of the video framebuffer but in other "units"
// than pixels (framebuffer must contain element of the type)
#define VIDEO_WIDTH_GLYPH           (VIDEO_WIDTH >> GLYPH_WIDTH_LOG2)
#define VIDEO_WIDTH_GLYPH8          (VIDEO_WIDTH >> GLYPH8_WIDTH_LOG2)
#define VIDEO_WIDTH_GLYPH16         (VIDEO_WIDTH >> GLYPH16_WIDTH_LOG2)
#define VIDEO_WIDTH_GLYPH32         (VIDEO_WIDTH >> GLYPH32_WIDTH_LOG2)
#define VIDEO_WIDTH_GLYPH64         (VIDEO_WIDTH >> GLYPH64_WIDTH_LOG2)
#define VIDEO_WIDTH_GLYPH128        (VIDEO_WIDTH >> GLYPH128_WIDTH_LOG2)
#define VIDEO_WIDTH_GLYPH256        (VIDEO_WIDTH >> GLYPH256_WIDTH_LOG2)
#define VIDEO_WIDTH_TILE            VIDEO_WIDTH_GLYPH
//TODO: VIDEO_WIDTH_TILExxx         ...
#define VIDEO_WIDTH_ATTRIBUTE       (VIDEO_WIDTH >> AttributeWidthLog2())
#define VIDEO_WIDTH_ATTRIBUTE_2x4   (VIDEO_WIDTH >> 1)
#define VIDEO_WIDTH_ATTRIBUTE_4x4   (VIDEO_WIDTH >> 2)
#define VIDEO_WIDTH_ATTRIBUTE_4x8   (VIDEO_WIDTH >> 2)
#define VIDEO_WIDTH_ATTRIBUTE_8x8   (VIDEO_WIDTH >> 3)
#define VIDEO_WIDTH_PIXELSTRIP      (VIDEO_WIDTH >> PIXELSTRIP_WIDTH_LOG2)
#define VIDEO_WIDTH_PIXELSTRIP1     (VIDEO_WIDTH >> PIXELSTRIP1_WIDTH_LOG2)
#define VIDEO_WIDTH_PIXELSTRIP2     (VIDEO_WIDTH >> PIXELSTRIP2_WIDTH_LOG2)
#define VIDEO_WIDTH_PIXELSTRIP4     (VIDEO_WIDTH >> PIXELSTRIP4_WIDTH_LOG2)
#define VIDEO_WIDTH_PIXELSTRIP8     (VIDEO_WIDTH >> PIXELSTRIP8_WIDTH_LOG2)

// VIDEO_HEIGHT_<element>: Width of the video framebuffer but in other "units"
// than pixels (framebuffer must contain element of the type)
#define VIDEO_HEIGHT_GLYPH          (VIDEO_HEIGHT >> GLYPH_HEIGHT_LOG2)
#define VIDEO_HEIGHT_GLYPH8         (VIDEO_HEIGHT >> GLYPH8_HEIGHT_LOG2)
#define VIDEO_HEIGHT_GLYPH16        (VIDEO_HEIGHT >> GLYPH16_HEIGHT_LOG2)
#define VIDEO_HEIGHT_GLYPH32        (VIDEO_HEIGHT >> GLYPH32_HEIGHT_LOG2)
#define VIDEO_HEIGHT_GLYPH64        (VIDEO_HEIGHT >> GLYPH64_HEIGHT_LOG2)
#define VIDEO_HEIGHT_GLYPH128       (VIDEO_HEIGHT >> GLYPH128_HEIGHT_LOG2)
#define VIDEO_HEIGHT_GLYPH256       (VIDEO_HEIGHT >> GLYPH256_HEIGHT_LOG2)
#define VIDEO_HEIGHT_TILE           VIDEO_HEIGHT_GLYPH
//TODO: VIDEO_HEIGHT_TILExxx        ...
#define VIDEO_HEIGHT_ATTRIBUTE      (VIDEO_HEIGHT >> AttributeHeightLog2())
#define VIDEO_HEIGHT_ATTRIBUTE_2x4  (VIDEO_HEIGHT >> 2)
#define VIDEO_HEIGHT_ATTRIBUTE_4x4  (VIDEO_HEIGHT >> 2)
#define VIDEO_HEIGHT_ATTRIBUTE_4x8  (VIDEO_HEIGHT >> 3)
#define VIDEO_HEIGHT_ATTRIBUTE_8x8  (VIDEO_HEIGHT >> 3)
#define VIDEO_HEIGHT_PIXELSTRIP     VIDEO_HEIGHT

// VIDEO_COUNT_<element>: The number of such elements in the framebuffer
// (video mode should be such that it really contains that type of element)
#if (INT_WIDTH >= 32) || (VIDEO_FACTOR_ == 1)
#   define VIDEO_COUNT_PIXELS       (VIDEO_WIDTH * VIDEO_HEIGHT)
#else
#   define VIDEO_COUNT_PIXELS       ((int32_t)VIDEO_WIDTH * (int32_t)VIDEO_HEIGHT)
#endif
#define VIDEO_COUNT_GLYPH           (VIDEO_COUNT_PIXELS >> (3 + VideoModeDimension()))
#define VIDEO_COUNT_GLYPH8          (VIDEO_COUNT_PIXELS >> (GLYPH8_WIDTH_LOG2   + GLYPH8_HEIGHT_LOG2))
#define VIDEO_COUNT_GLYPH16         (VIDEO_COUNT_PIXELS >> (GLYPH16_WIDTH_LOG2  + GLYPH16_HEIGHT_LOG2))
#define VIDEO_COUNT_GLYPH32         (VIDEO_COUNT_PIXELS >> (GLYPH32_WIDTH_LOG2  + GLYPH32_HEIGHT_LOG2))
#define VIDEO_COUNT_GLYPH64         (VIDEO_COUNT_PIXELS >> (GLYPH64_WIDTH_LOG2  + GLYPH64_HEIGHT_LOG2))
#define VIDEO_COUNT_GLYPH128        (VIDEO_COUNT_PIXELS >> (GLYPH128_WIDTH_LOG2 + GLYPH128_HEIGHT_LOG2))
#define VIDEO_COUNT_GLYPH256        (VIDEO_COUNT_PIXELS >> (GLYPH256_WIDTH_LOG2 + GLYPH256_HEIGHT_LOG2))
#define VIDEO_COUNT_TILE            VIDEO_COUNT_GLYPH // Tiles dimensions uses same indicator in the VIDEO_MODE as Glyphs.
//TODO: VIDEO_COUNT_TILExxx         ...
#define VIDEO_COUNT_PIXELSTRIP      (VIDEO_COUNT_PIXELS >> (PIXELSTRIP_WIDTH_LOG2  + PIXELSTRIP_HEIGHT_LOG2))
#define VIDEO_COUNT_PIXELSTRIP1     (VIDEO_COUNT_PIXELS >> (PIXELSTRIP1_WIDTH_LOG2 + PIXELSTRIP_HEIGHT_LOG2))
#define VIDEO_COUNT_PIXELSTRIP2     (VIDEO_COUNT_PIXELS >> (PIXELSTRIP1_WIDTH_LOG2 + PIXELSTRIP_HEIGHT_LOG2))
#define VIDEO_COUNT_PIXELSTRIP4     (VIDEO_COUNT_PIXELS >> (PIXELSTRIP1_WIDTH_LOG2 + PIXELSTRIP_HEIGHT_LOG2))
#define VIDEO_COUNT_PIXELSTRIP8     (VIDEO_COUNT_PIXELS >> (PIXELSTRIP1_WIDTH_LOG2 + PIXELSTRIP_HEIGHT_LOG2))
#define VIDEO_COUNT_ATTRIBUTE       //TODO: does it make sense, because attributes is wide sometimes
                                    // (VIDEO_SIZE_ATTRIBUTES >> ATTRIBUTE_SIZE_LOG2)
#define VIDEO_COUNT_PLANE           VideoModeLowNibble()

// Size in bytes of regions in the framebuffer.
// Remember that it only make sense to use a macro if the frembuffer actually
// contains the associated regions (for example, if video doesn't have planes or
// attributes, then VIDEO_SIZE_PLANE and VIDEO_SIZE_ATTRIBUTES aren't 0)
#define VIDEO_SIZE_PIXELS           ((VIDEO_COUNT_PIXELS >> 3) * PIXELSTRIP_COLOR_DEPTH) ///?
#define VIDEO_SIZE_GLYPHS           (VIDEO_COUNT_PIXELS >> 3)
#define VIDEO_SIZE_TILES            VIDEO_SIZE // tiles never have attributes
#define VIDEO_SIZE_ATTRIBUTES       (VIDEO_SIZE - (VIDEO_COUNT_PIXELS >> 3))
#define VIDEO_SIZE_PLANE            (VIDEO_SIZE / VIDEO_COUNT_PLANE)


//------------------------------------------------------------------------------
// Definition of the Video.mode
//------------------------------------------------------------------------------
// The video mode describes the layout of the video framebuffer.
// It is encoded by a single byte with the following binary representation:
//
//    .-- Has Attributes (single bit, 0 or 1)
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
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
// Note: A struct like this may have facilitated dealing with video mode, but
//       bit fields in C are a bit "broken" in that alignment and packing is
//       compiler dependent, so it might be that the following might not be like
//       a byte (altough it probably does in most compilers, and also compiler-
//       specifc attribute may ensure it is so).
//       MAYBE: change to that in the future?
//
//       typedef
//       struct __attribute__((packed, aligned(1))) VideoMode {
//          uint8_t  has_attribute  : 1;
//          uint8_t  elem_dimension : 3;
//          uint8_t  low_nibble     : 4;
//       } VideoMode;
//
//       static_assert(sizeof(struct VideoMode)  == sizeof(uint8_t));
//       static_assert(alignof(struct VideoMode) == alignof(uint8_t));
//
//------------------------------------------------------------------------------

// Return Video Mode's attribute bit. (ie. whether framebuffer has Attributes)
#define VideoModeHasAttributes()               (VIDEO_MODE >> 7)

// Return the Video Mode's dimension (ie, the dimension in pixels of the
// framebuffer's non-attribute elements) as an `enum VideoElementDimension`.
#define VideoModeDimension()                   ((VIDEO_MODE >> 4) & 7)

// Return the Video Mode's low nibble
#define VideoModeLowNibble()                   (VIDEO_MODE & 0xF)

//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------

// Same as the VideoMode() function, but do NOT change the video mode.
// It can be used for example to test if mode would be a valid video mode.
int VideoMode_(int mode);

#ifndef KONPU_OPTION_OPTIMIZE_VIDEO_MODE
   // Set the video mode to the given mode (if valid)
   // Return Value:
   // - If the mode is invalid, return 0.
   // - Otherwise a non-zero return value (which is the number of bytes required
   //   to encode an 8x8 pixel area in the given mode).
   int VideoMode(int mode);
#else
   static inline int VideoMode(int mode)
   { return (mode == VIDEO_MODE) ? VideoMode_(mode) : 0; }
   // When optimized, the video mode must not be changed.
#endif



//------------------------------------------------------------------------------
// Functions or macros to easily create a value that could be assigned to the
// video mode.
//------------.-----------------------------------------------------------------
//            |                 Method used for coloring:
// Framebuffer|-------------------.-------------------.-------------------------
// Elements   |    Bit Planes     | "Chunky" Elements |       Attributes
//------------|-------------------|-------------------|-------------------------
// Glyph/Tile | Glyph Planes      | Tiles             | Glyphs + Attributes
//------------|-------------------|-------------------|-------------------------
// PixelStrip |PixelStrip8 Planes | PixelStrip<1|2|4> | PixelStrip8 + Attributes
//            |(bit pixel)        |                   | (bit pixels)
//------------'-------------------'-------------------'-------------------------

//------------------------------------------------------------------------------
// VIDEO_MODE_*: macros (or functions) to specify a video mode value
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
// - the attribute bit (0 or 1),
// - the dimension     (enum VideoElementDimension)
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
{ return VIDEO_MODE_(0, PIXELS_Nx1,
      (bits_per_pixel == 1) ? 1 : (9 + (bits_per_pixel >> 2)) ); }

// Return a mode number based on bit pixels with the given amount of planes
// The number of planes should be 1,2,3,4,5,6,or 8.
static inline int VIDEO_MODE_PIXEL_PLANES(int number_of_planes)
{ return VIDEO_MODE_(0, PIXELS_Nx1, (number_of_planes)); }

// Return a mode number with bit pixels and the bigven attribute type.
static inline int VIDEO_MODE_PIXEL_ATTRIBUTES(enum AttributeType attr_type)
{ return VIDEO_MODE_(1, PIXELS_Nx1, attr_type); }

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

// TODO: Could I move this to color.h???
// Return the current color depth,
// ie. the log2 of the max. number of colors that can be in the framebuffer
static inline int ColorDepth()
{
   // Attribute modes
   if (VideoModeHasAttributes())
      return 4 << (AttributeColorType() != ATTRIBUTE_COLORS_16);
      //     ^--- 16 colors attributes => color depth is 4
      //          otherwise            => color depth is 8

   // Non-Attributes modes
   int n = VideoModeLowNibble();
   return (n <= 8) ? n : 1 << (n - 8);
      //  ^--- This is because:
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





//------------------------------------------------------------------------------
// Access to bit Planes
//
// Reminder:
// - Video elements are Glyph or PixelStrip8 (aka bit pixels)
// - Video has no attributes (Video.mode attribute bit is off)
// - Video.mode's low nibble (or VIDEO_COUNT_PLANE) gives the number of planes
//------------------------------------------------------------------------------

// TYPE *VideoPlaneAt_([TYPE], plane)
// Return a pointer (TYPE*) to the start of the n-th plane (zero-indexed)
// - TYPE : should normally be Glyph<n> or PixelStrip8 (bit pixels) as that's
//          what planes give colors too.
//          If omitted, return a `uint8_t *`
// - plane: MUST be positive and < VIDEO_COUNT_PLANE
#define VideoPlaneAt_(...)  \
   UTIL_OVERLOAD(VideoPlaneAt_, __VA_ARGS__)
#  define VideoPlaneAt__2_(TYPE, n)  ((TYPE *)VideoPlaneAt__1_((n)))
   static inline uint8_t *VideoPlaneAt__1_(int n) {
      assert(n >= 0 && n < VIDEO_COUNT_PLANE);
      return Video.frame + n * VIDEO_SIZE_PLANE;
   }

//------------------------------------------------------------------------------
// Misc. Video functions
//------------------------------------------------------------------------------

// Reset color, mode, empty framebuffer, etc.
void VideoReset(void);

// Render the video framebuffer on screen.
// Return value may be non-zero on error (in which case VIDEO_RENDER_ERRRORS
// counter will also be increased by one).
int VideoRender(void);


#endif //include guard
