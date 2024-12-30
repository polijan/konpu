// Functionalities to query properties based on the video mode
#ifndef  KONPU_VIDEO_MODE_H
#define  KONPU_VIDEO_MODE_H
#include "memory.h"

//------------------------------------------------------------------------------
// Enums
//------------------------------------------------------------------------------

// This enum is used to indicate the size in pixels of video elements (ie
// (Glyphs, Tiles, Attributes, Pixels) encoded in the video framebuffer.
enum VideoElementPixelSize {
   // Indicates the size in pixels of Glyph, Tiles, or Attributes.
   PIXELS_2x4   = 0,
   PIXELS_4x4   = 1,
   PIXELS_4x8   = 2,
   PIXELS_8x8   = 3, // <- not valid for 8bpp tiles
   PIXELS_8x16  = 4, // <- not valid for attributes or >=4bpp tiles
   PIXELS_16x16 = 5, // <- not valid for attributes or tiles
   // Note: For glyphs, this represents the log2 of the size in bytes.

   // Indicates that elements in the framebuffer are Pixels.
   PIXELS       = 7,
};

// This enum is used to indicate the type of colors encoded by attributes in the
// video framebuffer.           // Byte|
enum AttributeColorType {       // size| Description
   ATTRIBUTE_COLORS_16    = 0,  //   1 | Nibbles specify a fg and bg 16-color
   ATTRIBUTE_COLORS_FG256 = 1,  //   1 | Byte specifies one foreground 256-color
   ATTRIBUTE_COLORS_BG256 = 2,  //   1 | Byte specifies one background 256-color
   ATTRIBUTE_COLORS_256   = 3   //   2 | Bytes specify a fg and bg 256-color
};

// The full list of the 16 possible Attributes type. It is nibble value which is
// composed of two quarters: an VideoElementPixelSize (max. 8x8) and an
// AttributeColorType.
//                       .------ attributes px size (enum VideoElementPixelSize)
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
//    |  .--- Element Descriptor (enum VideoElementPixelSize)
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
// * Element Descriptor: is an enum VideoElementPixelSize which describes the
//   size of Glyphs or Tiles or alternatively the presence of pixels.
//
// * Low Nibble:
//
//   - If the Has Attribute bit is set, the low nibble describes an attribute
//     type (enum AttributeType). Reminder: this value is itself composed of two
//     quarters:
//                       .------ attributes px size (enum VideoElementPixelSize)
//                       |  .--- attributes color type (enum AttributeColorType)
//                       v  v
//                      |..|..|
//
//   - Othwerwise, the low nibble can (only) take the following values:
//
//     1...8  -> indicates a planar mode (either with glyph or pixels)
//               and give the number of bitplanes.
//
//     9...11 -> indicate a packed pixel mode (if Element descriptor is PIXELS)
//               or a tile mode, and gives the bits per pixels for the element
//               as follow:   9 -> 2bpp pixels or tiles (4 color)
//                           10 -> 4bpp pixels or tiles (16 colors)
//                           11 -> 8bpp pixels or tiles (256 colors)
//               Note: 1 << (low nibble - 8) is the number of bits per pixel,
//                     1 << (1 << (low nibble - 8)) is the nmuber of colors
//------------------------------------------------------------------------------

// The default Video Mode (= glyphs + 16-color attributes, each 4x8-sized)
#define VIDEO_MODE_DEFAULT   168

#ifndef KONPU_OPTION_OPTIMIZE_VIDEO_MODE
#  define VIDEO_MODE    (*(const uint8_t*)(KonpuMemory + VIDEO_MODE_ADDRESS))
#else
   // Normalize (to default mode) if the symbol is defined but has no value
#  if ~(~KONPU_OPTION_OPTIMIZE_VIDEO_MODE + 0) == 0 && \
      ~(~KONPU_OPTION_OPTIMIZE_VIDEO_MODE + 1) == 1
#     undef  KONPU_OPTION_OPTIMIZE_VIDEO_MODE
#     define KONPU_OPTION_OPTIMIZE_VIDEO_MODE  VIDEO_MODE_DEFAULT
#  endif
#  define VIDEO_MODE    KONPU_OPTION_OPTIMIZE_VIDEO_MODE
#endif //KONPU_OPTION_OPTIMIZE_VIDEO_MODE

//------------------------------------------------------------------------------
// The three main parts in the VIDEO_MODE (as explained above)
//------------------------------------------------------------------------------

// Return VIDEO_MODE's attribute bit / ie. whether framebuffer has Attributes
#define VideoModeHasAttributes()   (VIDEO_MODE >> 7)

// Return VIDEO_MODE's low nibble
//#define VideoModeLowNibble()       (VIDEO_MODE & 0xF)
static inline int VideoModeLowNibble(void)    { return VIDEO_MODE & 0xF; }

// Return VIDEO_MODE's element descriptor
// (whether elements are pixels or their pixel size)
static inline enum VideoElementPixelSize
VideoModeElementDescriptor(void)
{ return (VIDEO_MODE >> 4) & 7; }


//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------

// Return the number of bytes required to encode an 8x8 pixel area in the given
// mode. If the mode is invalid, return 0 (thus this function can also be used
// in a boolean test to checking mode's validity).
//
// The resolution of the framebuffer depends on how many bytes are required
// to encode an 8x8 pixel area (see: tools/resfinder).
int VideoModeResolution(uint8_t mode);

// Set the video to the given mode.
// Return 0 if mode is invalid,
// and otherwise, the same non-zero value as function `VideoModeResolution`
#ifndef KONPU_OPTION_OPTIMIZE_VIDEO_MODE
   int VideoSetMode(uint8_t mode);
#else
   static inline int VideoSetMode(uint8_t mode)
   { return (mode == VIDEO_MODE)? VideoModeResolution(mode) : 0; }
   //        Mode should not be changed in optimized parts ---^
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
// Pixels      | BIT-PIXEL PLANES | PIXELS            | BIT-PIXELS + ATTRIBUTES
//-------------'------------------'-------------------'-------------------------

//------------------------------------------------------------------------------
// Various macros to create a video mode
//------------------------------------------------------------------------------
// VIDEO_MODE_FROM_PARTS_(int attribute_bit,
//       enum VideoElementPixelSize element descriptor, int low_nibble)
//
// VIDEO_MODE_GLYPH(GLYPH_TYPE)
// VIDEO_MODE_GLYPH_PLANES(GLYPH_TYPE, int number_of_planes)
// VIDEO_MODE_GLYPH_ATTRIBUTES(GLYPH_TYPE, [enum AttributeType])
//
// VIDEO_MODE_PIXEL(int depth)                   // 1,2,4,8
// VIDEO_MODE_PIXEL_PLANES(int number_of_plane)  // 1,2,3,4,5,6,8
// VIDEO_MODE_PIXEL_ATTRIBUTES(enum AttributeType)
//
// VIDEO_MODE_TILE(TILE_TYPE)
//------------------------------------------------------------------------------

// Return a video mode based on its three parts (as explained in the doc):
// - the attribute bit,
// - the element descriptor (enum VideoElementPixelSize)
// - the low_nibble
#define VIDEO_MODE_FROM_PARTS_(attribute_bit, element_descriptor, low_nibble) \
   ((int)((attribute_bit) << 7 | (element_descriptor) << 4 | (low_nibble)))

// int VIDEO_MODE_GLYPH(GLYPH_TYPE)
// Return a mode number based on a Glyph type with a single bit plane
#define VIDEO_MODE_GLYPH(GLYPH_TYPE) \
   VIDEO_MODE_GLYPH_PLANES(GLYPH_TYPE, 1)

// int VIDEO_MODE_GLYPH_PLANES(GLYPH_TYPE)
// Return a mode number based on a Glyph type with the given amount of planes
// The number of planes should be 1,2,3,4,5,6,or 8.
#define VIDEO_MODE_GLYPH_PLANES(GLYPH_TYPE, number_of_planes) \
   VIDEO_MODE_FROM_PARTS_(                                    \
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

// int VIDEO_MODE_GLYPH_ATTRIBUTES(GLYPH_TYPE, [enum AttributeType] attr_type);
// Return a mode number based on a Glyph type and an enum AttributeType
// If the Attribute if left unspecified, it will default to 16-color attributes
// whose size in pixels is either same as the glyphs or at max 8x8.
#define VIDEO_MODE_GLYPH_ATTRIBUTES(...)                                    \
   UTIL_OVERLOAD(VIDEO_MODE_GLYPH_ATTRIBUTES, __VA_ARGS__)
   #define VIDEO_MODE_GLYPH_ATTRIBUTES_2_(GLYPH_TYPE, enum_attribute_type)  \
      VIDEO_MODE_FROM_PARTS_(                                               \
         1,                                                                 \
         _Generic((GLYPH_TYPE){0},                                          \
            Glyph8:   PIXELS_2x4,                                           \
            Glyph16:  PIXELS_4x4,                                           \
            Glyph32:  PIXELS_4x8,                                           \
            Glyph64:  PIXELS_8x8,                                           \
            Glyph128: PIXELS_8x16,                                          \
            Glyph256: PIXELS_16x16                                          \
         ),                                                                 \
         (enum_attribute_type)                                              \
      )
   #define VIDEO_MODE_GLYPH_ATTRIBUTES_1_(GLYPH_TYPE)                       \
      VIDEO_MODE_GLYPH_ATTRIBUTES_2_(GLYPH_TYPE, ATTRIBUTE_COLORS_16 |       \
         _Generic((GLYPH_TYPE){0},                                          \
            Glyph8:   PIXELS_2x4,                                           \
            Glyph16:  PIXELS_4x4,                                           \
            Glyph32:  PIXELS_4x8,                                           \
            Glyph64:  PIXELS_8x8,                                           \
            Glyph128: PIXELS_8x8,                                           \
            Glyph256: PIXELS_8x8                                            \
         ) << 2                                                             \
      )

// int VIDEO_MODE_PIXEL(int depth)
// Return a mode number with chunk pixels (or a single plane of bit pixels)
// based on the depth, which should be 1,2,4,8
#define VIDEO_MODE_PIXEL(bit_depth)                        \
   VIDEO_MODE_FROM_PARTS_(0, PIXELS,                       \
      ((bit_depth) == 1) ? 1 : (9 + (bits_per_pixel >> 2)) )

// int VIDEO_MODE_PIXEL_PLANES(number_of_planes)
// Return a mode number based on bit pixels with the given amount of planes
// The number of planes should be 1,2,3,4,5,6,or 8.
#define VIDEO_MODE_PIXEL_PLANES(number_of_planes)      \
   VIDEO_MODE_FROM_PARTS_(0, PIXELS, (number_of_planes))

// int VIDEO_MODE_PIXEL_ATTRIBUTES(enum AttributeType attr_type);
// Return a mode number with bit pixels and the bigven attribute type.
#define VIDEO_MODE_PIXEL_ATTRIBUTES(enum_attribute_type)  \
   VIDEO_MODE_FROM_PARTS_(1, PIXELS, (enum_attribute_type))

// Return a Tile mode with the given tile type
#define VIDEO_MODE_TILE(TILE_TYPE)   // TODO
/*
static inline
int VIDEO_MODE_TILE(enum VideoElementPixelSize tile_size, int bits_per_pixel) {
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

// Return the color depth of the video framebuffer,
// ie. the log2 of the max. number of colors that can be in the framebuffer
static inline int VideoColorDepth()
{
   // Attribute modes
   if (VideoModeHasAttributes())
      return (AttributeColorType() == ATTRIBUTE_COLORS_16)? 4 : 8; // 16 or 256

   // Non-Attributes modes
   int n = VideoModeLowNibble();
   return (n <= 8)? n : (1 << (n - 8));
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

//------------------------------------------------------------------------------
// Meh?
//------------------------------------------------------------------------------

// Return whether the framebuffer has pixels (either packed/chunky or as bitplanes)
#define VideoModeHasPixels() \
   (VideoModeElementDescriptor() == PIXELS)

// Return whether the framebuffer has either Glyphs or Tiles
#define VideoModeHasGlyphsOrTiles() \
   (VideoModeElementDescriptor() <= PIXELS_16x16)

// Return whether the framebuffer is in a Bitplane mode
// (the mode may be encoding either Glyphs or Pixels)
#define VideoModeHasBitPlanes() \
   (VideoModeHasAttributes() ? 0 : (VideoModeLowNibble() <= 8))

// Return whether the framebuffer is in a Glyph Bitplane mode
#define VideoModeHasGlyphBitPlanes() \
   (VideoModeHasBitPlanes() && VideoModeHasGlyphsOrTiles())

// Return whether the framebuffer is in a Pixel Bitplane mode
#define VideoModeHasPixelBitPlanes() \
   (VIDEO_MODE >= 113 && VIDEO_MODE <= 120)
   // faster than: (VideoModeHasBitPlanes() && VideoModeHasPixels())

// Return whether the framebuffer is in packed/chunky mode (either as Tiles or Pixels)
#define VideoModeHasChunks() \
   (VideoModeHasAttributes() ? 0 : (VideoModeLowNibble() > 8))

// Return whether the framebuffer is in Tile mode
#define VideoModeHasTiles() \
   (VideoModeHasGlyphsOrTiles() && VideoModeHasChunks())

// Return whether the frambuffer is in packed/chunky pixels mode
#define VideoModeHasChunkyPixels() \
   (VIDEO_MODE >= 121 && VIDEO_MODE <= 123)
   // faster than: (VideoModeHasPixels() && VideoModeHasChunks())

// Return whether the framebuffer encodes pixels but without the need of several
// bitplanes, ie, it return true iff it has chunky pixels or ONE bitplane of
// pixels.
#define VideModeHasSimplePixels() \
   (VIDEO_MODE == 113 || VideoModeHasChunkyPixels())

// Return whether the framebuffer encodes pixels over MORE than 1 bitplane
#define VideModeHasSplitPixels() \
   (VIDEO_MODE > 113 && VIDEO_MODE <= 120)


// Return VIDEO_MODE's bit per pixel element
// Notes: only valid in bitplanes modes (Glyph bitplanes or Pixels bitplanes)
static inline int VideoModeNumberOfPlanes(void)
{  assert(VideoModeHasBitPlanes());
   return VIDEO_MODE & 0xF; }

// Return VIDEO_MODE's log2 of bit per pixel of element
// Notes: only valid in chunky modes (Tiles or Pixels)
static inline int VideoModeBitsPerPixelLog2(void)
{ return (VIDEO_MODE & 0xF) - 8; }


//------------------------------------------------------------------------------
// Definition for VIDEO_WIDTH* / VIDEO_HEIGHT*
//------------------------------------------------------------------------------

// VIDEO_MODE_WIDTH_MAX / VIDEO_MODE_HEIGHT_MAX
// The maximum possible framebuffer width and height from any video mode
#if VIDEO_SIZE_FACTOR_ == 7
#   define VIDEO_MODE_WIDTH_MAX   480
#   define VIDEO_MODE_HEIGHT_MAX  336
#elif VIDEO_SIZE_FACTOR_ == 6
#   define VIDEO_MODE_WIDTH_MAX   480
#   define VIDEO_MODE_HEIGHT_MAX  288
#elif VIDEO_SIZE_FACTOR_ == 5
#   define VIDEO_MODE_WIDTH_MAX   400
#   define VIDEO_MODE_HEIGHT_MAX  288
#elif VIDEO_SIZE_FACTOR_ == 4
#   define VIDEO_MODE_WIDTH_MAX   320
#   define VIDEO_MODE_HEIGHT_MAX  288
#elif VIDEO_SIZE_FACTOR_ == 3
#   define VIDEO_MODE_WIDTH_MAX   320
#   define VIDEO_MODE_HEIGHT_MAX  216
#elif VIDEO_SIZE_FACTOR_ == 2
#   define VIDEO_MODE_WIDTH_MAX   288
#   define VIDEO_MODE_HEIGHT_MAX  160
#else
#   error "VIDEO_SIZE_FACTOR_ does not have a correct value!"
#endif

// VIDEO_WIDTH
#ifdef KONPU_OPTION_FORCE_VIDEO_MODE_WIDTH
#  define VIDEO_WIDTH    KONPU_OPTION_FORCE_VIDEO_MODE_WIDTH
#elif !defined(KONPU_OPTION_OPTIMIZE_VIDEO_MODE)
#  define VIDEO_WIDTH    (*(const int16_t*)(KonpuMemory + VIDEO_WIDTH_ADDRESS))
#endif

// VIDEO_HEIGHT
#ifdef KONPU_OPTION_FORCE_VIDEO_MODE_HEIGHT
#  define VIDEO_HEIGHT   KONPU_OPTION_FORCE_VIDEO_MODE_HEIGHT
#elif !defined(KONPU_OPTION_OPTIMIZE_VIDEO_MODE)
#  define VIDEO_HEIGHT   (*(const int16_t*)(KonpuMemory + VIDEO_HEIGHT_ADDRESS))
#endif

// This will define VIDEO_WIDTH and VIDEO_WEIGHT when not "FORCED" and in the
// case where the OPTIMIZE_VIDEO_MODE option is on.
#include "video_mode_auto.h"


#endif //KONPU_VIDEO_MODE_H
