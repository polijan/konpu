#ifndef KONPU_PIXEL_H_
#define KONPU_PIXEL_H_
#include "video_mode.h"
#include "glyph.h"
#include "attribute.h"

// A "strip" is a byte representing a small horizontal line of 1,2,4,or 8
// pixels. When the framebuffer contains pixels, they are stored as strips.

//------------------------------------------------------------------------------
// PixelStrip: Dimensions and Properties
//----------------------------..----------.---------.--------.------------------
// PixelStrip                 // Pixel    |Dimension|  # of  | Video.mode's
typedef uint8_t  PixelStrip;  // Chunk(s) |in Pixels| Colors | Low Nibble
//----------------------------//----------|---------|--------|------------------
typedef uint8_t  PixelStrip8; // Bits     |  8 x 1  |    2   |    1
typedef uint8_t  PixelStrip4; // Quarters |  4 x 1  |    4   |    9
typedef uint8_t  PixelStrip2; // Nibbles  |  2 x 1  |   16   |   10
typedef uint8_t  PixelStrip1; // Byte     |  1 x 1  |  256   |   11
//----------------------------''----------'---------'--------'------------------

#define PIXELSTRIP_WIDTH        (1 << PIXELSTRIP_WIDTH_LOG2)
#define PIXELSTRIP_HEIGHT       1
#define PIXELSTRIP1_WIDTH       1
#define PIXELSTRIP2_WIDTH       2
#define PIXELSTRIP4_WIDTH       4
#define PIXELSTRIP8_WIDTH       8
#define PIXELSTRIP1_WIDTH_LOG2  0
#define PIXELSTRIP2_WIDTH_LOG2  1
#define PIXELSTRIP4_WIDTH_LOG2  2
#define PIXELSTRIP8_WIDTH_LOG2  3
#define PIXELSTRIP_HEIGHT_LOG2  0
#define PIXELSTRIP_WIDTH_LOG2   ((VIDEO_MODE & 8)? (11 - VideoModeLowNibble()) : 3)
   //   ^ Based on table above, we can infer PIXELSTRIP_WIDTH is the following
   //     power of 2 based on Video.mode's low nibble n: (n==1) ? 3 : 11-n
   //     Furthermore, checking Video.mode's low nibble being 1 vs 9/10/11,
   //     could further simplify into checking (Video.mode & 8)

#define PIXELSTRIP_COLOR_DEPTH \
   ((VIDEO_MODE & 8) ? (1 << (VideoModeLowNibble() - 8)) : 1)

//------------------------------------------------------------------------------
// Access to PixelStrip elements
//------------------------------------------------------------------------------

// uint8_t *VideoPixelStripAt_(int x,y, [int plane])
// Access a PixelStrip in the video framebuffer. No bounds checking!
#define VideoPixelStripAt_(...) \
   UTIL_OVERLOAD(PixelStripAt_, __VA_ARGS__)
#  define VideoPixelStripAt__2_(x,y)   VideoPixelStripAt__3_((x), (y), 0)
   static inline uint8_t* VideoPixelStripAt__3_(int x, int y, int p) {
      assert(x >= 0 && x < VIDEO_WIDTH_PIXELSTRIP);
      assert(y >= 0 && y < VIDEO_HEIGHT_PIXELSTRIP);
      return Video.pixel + p * VIDEO_SIZE_PLANE + x + y * VIDEO_WIDTH_PIXELSTRIP;
   }


//------------------------------------------------------------------------------
// Unsafe MACROS for access to individual Pixels of the video framebuffer.
//
// Those are UNSAFE as that they do NOT do *any* bound checking and may evaluate
// their arguments more than once. To manipulate pixels, it is more convenient
// and safer to use the PixelGet() and PixelSet() functions.
//------------------------------------------------------------------------------
//
// GET: Return the value of pixel (x,y) in the framebuffer (at the given plane)
// uint8_t  VIDEO_GET_PIXEL__GLYPH_(<8|16|32|64|128|256>, int x,y,plane);
// uint8_t  VIDEO_GET_PIXEL__TILE<TODO>(int x,y);
// uint8_t  VIDEO_GET_PIXEL__PIXELSTRIP<1|2|4>_(int x,y);
// uint8_t  VIDEO_GET_PIXEL__PIXELSTRIP8_(int x,y,plane);
//
// SET: Set the value of pixel (x,y) in the framebuffer (at the given plane)
// void  VIDEO_SET_PIXEL__GLYPH_(<8|16|32|64|128|256>, int x,y,plane, uint8_t bit);
// void  VIDEO_SET_PIXEL__TILE<TODO>(int x,y, ...);
// void  VIDEO_SET_PIXEL__PIXELSTRIP<1|2|4>_(int x,y, uint8_t chunk);
// void  VIDEO_SET_PIXEL__PIXELSTRIP8_(int x,y,plane, uint8_t bit);
//
// For those functions to make sense, the framebuffer should be in a mode such
// that it contains the specified element, and the value for x,y, and plane,
// must not exceed the limits. None of this is enforced by the macros.
//------------------------------------------------------------------------------


// Implementation notes for _PIXELSTRIP<N>_ macros:
//
// - The individual pixel (x,y) in the framebuffer containing PixelStrips is the
//   chunk at position `x % PIXELSTRIP_WIDTH` in the PixelStrip located at
//   (`x / PIXELSTRIP_WIDTH`, y).
//   Because PIXELSTRIP_WIDTH is a power of 2, this means: pixel (x,y) is the
//   chunk at position `x & (PIXELSTRIP_WIDTH - 1)` in the PixelStrip located at
//   (`x >> PIXELSTRIP_WIDTH_LOG2`, y).
// - Only bit pixels can be arranged in bit planes, so we accept a `plane`
//   parameter only when element is PIXELSTRIP8.

#define VIDEO_GET_PIXEL__PIXELSTRIP1_(x,y)                                     \
   (Video.pixel[(x) + (y) * VIDEO_WIDTH])

#define VIDEO_GET_PIXEL__PIXELSTRIP2_(x,y)                     BITS_GET_NIBBLE(\
   Video.pixel[ ((x)>>PIXELSTRIP2_WIDTH_LOG2) + (y)*VIDEO_WIDTH_PIXELSTRIP2 ] ,\
   (x) & (PIXELSTRIP2_WIDTH - 1)                                               )

#define VIDEO_GET_PIXEL__PIXELSTRIP4_(x,y)                    BITS_GET_QUARTER(\
   Video.pixel[ ((x)>>PIXELSTRIP4_WIDTH_LOG2) + (y)*VIDEO_WIDTH_PIXELSTRIP4 ] ,\
   (x) & (PIXELSTRIP4_WIDTH - 1)                                               )

#define VIDEO_GET_PIXEL__PIXELSTRIP8_(x,y, plane)                 BITS_GET_BIT(\
   Video.pixel[(plane) * VIDEO_SIZE_PLANE +                                    \
      ((x) >> PIXELSTRIP8_WIDTH_LOG2) + (y) * VIDEO_WIDTH_PIXELSTRIP8 ]       ,\
   (x) & (PIXELSTRIP8_WIDTH - 1)                                               )

#define VIDEO_SET_PIXEL__PIXELSTRIP1_(x,y, byte)                               \
   Video.pixel[(x) + (y) * VIDEO_WIDTH] = (byte)

#define VIDEO_SET_PIXEL__PIXELSTRIP2_(x, y, nibble)                            \
   do {                                                                        \
      uint8_t *strip = Video.pixel +                                           \
         ((x) >> PIXELSTRIP2_WIDTH_LOG2) + (y) * VIDEO_WIDTH_PIXELSTRIP2;      \
      *strip = BITS_SET_NIBBLE(*strip, (x) & (PIXELSTRIP2_WIDTH - 1), (nibble)); \
   } while(0)

#define VIDEO_SET_PIXEL__PIXELSTRIP4_(x, y, quarter)                           \
   do {                                                                        \
      uint8_t *strip = Video.pixel +                                           \
         ((x) >> PIXELSTRIP4_WIDTH_LOG2) + (y) * VIDEO_WIDTH_PIXELSTRIP4;      \
      *strip = BITS_SET_QUARTER(*strip, (x) & (PIXELSTRIP4_WIDTH - 1), (quarter)); \
   } while(0)


#define VIDEO_SET_PIXEL__PIXELSTRIP8_(x, y, plane, bit)                        \
   do {                                                                        \
      uint8_t *strip = Video.pixel + (plane) * VIDEO_SIZE_PLANE +              \
         ((x) >> PIXELSTRIP8_WIDTH_LOG2) + (y) * VIDEO_WIDTH_PIXELSTRIP8;      \
      *strip = BITS_SET_BIT(*strip, (x) & (PIXELSTRIP8_WIDTH - 1), (bit));     \
   } while(0)

#if 0 // Enable or NOT?
static inline int VIDEO_GET_PIXEL__PIXELSTRIP_(int x, int y, int plane)
{
   assert(x >= 0 && x < VIDEO_WIDTH);
   assert(y >= 0 && y < VIDEO_HEIGHT);
   uint8_t *strip = Video.pixel + plane * VIDEO_SIZE_PLANE
      + (x << PIXELSTRIP_WIDTH_LOG2) + y * VIDEO_WIDTH_PIXELSTRIP;
   int chunk_sz = PIXELSTRIP_COLOR_DEPTH; //(evaluated twice by the macro below)
   return BITS_GET_CHUNK(*strip, x & (PIXELSTRIP_WIDTH - 1), chunk_sz);
}

static inline void VIDEO_SET_PIXEL__PIXELSTRIP_(int x, int y, int plane, unsigned chunk)
{
   assert(x >= 0 && x < VIDEO_WIDTH);
   assert(y >= 0 && y < VIDEO_HEIGHT);
   uint8_t *strip = Video.pixel + plane * VIDEO_SIZE_PLANE
      + (x << PIXELSTRIP_WIDTH_LOG2) + y * VIDEO_WIDTH_PIXELSTRIP;
   int chunk_sz = PIXELSTRIP_COLOR_DEPTH; //(evaluated twice by the macro below)
   // Bit function BITS_SET_CHUNK(..) is not implemented
   *strip = BITS_SET_CHUNK(*strip, x & (PIXELSTRIP_WIDTH - 1), chunk_sz, chunk);
}
#endif

// Implementation notes for _GLYPH_ macros:
//
// - The pixel (x,y) in the framebuffer containing Glyphs is the bit
// at position (x % GLYPH_WIDTH, y % GLYPH_HEIGHT)
//   within the glyph at position (x / GLYPH_WIDTH, y / GLYPH_HEIGHT).
// - Because Glyphs width (and similarly for height) are powers of 2, the
//   following optimizations can be used:
//   x / glyph_width, becomes: `x >> GLYPH_WIDTH_LOG2`
//   x % glyph_width, becomes: `x & (GLYPH_WIDTH - 1)`

#define VIDEO_GET_PIXEL__GLYPH_(N, x,y, plane)                                 \
   GlyphGetPixel_##N##_(                                                       \
      VideoPlaneAt__2_(Glyph##N, (plane))[                                     \
         ((x) >> UTIL_CAT(GLYPH##N, _WIDTH_LOG2))                              \
         + ((y) >> UTIL_CAT(GLYPH##N, _HEIGHT_LOG2)) * UTIL_CAT(VIDEO_WIDTH_, GLYPH##N) \
         ],                                                                    \
         (x) & (UTIL_CAT(GLYPH##N, _WIDTH)  - 1),                              \
         (y) & (UTIL_CAT(GLYPH##N, _HEIGHT) - 1)                               \
   )

#define VIDEO_SET_PIXEL__GLYPH_(N, x,y, plane, bit)                            \
   do {                                                                        \
      Glyph##N *glyph = VideoPlaneAt__2_(Glyph##N, (plane))                    \
         + ((x) >> UTIL_CAT(GLYPH##N, _WIDTH_LOG2))                            \
         + ((y) >> UTIL_CAT(GLYPH##N, _HEIGHT_LOG2)) * UTIL_CAT(VIDEO_WIDTH_, GLYPH##N); \
      *glyph = GlyphSetPixel_##N##_(*glyph,                                    \
         (x) & (UTIL_CAT(GLYPH##N, _WIDTH)  - 1),                              \
         (y) & (UTIL_CAT(GLYPH##N, _HEIGHT) - 1),                              \
         (bit) );                                                              \
   } while(0)


//------------------------------------------------------------------------------
// Access to individual Pixels in the framebuffer: PixelGet(), PixelSet(), etc.
// whatever the framebuffer elements actually are.
//------------------------------------------------------------------------------

// Internal only - Use or PixelGet [or VideoGet_] instead.
static C_HINT_ALWAYS_INLINE int PixelGet_internal_(int x, int y)
{
   assert(x >= 0 && x < VIDEO_WIDTH);
   assert(y >= 0 && y < VIDEO_HEIGHT);

   //---------------------------------------------------------------------------
   // Attribute modes
   //---------------------------------------------------------------------------
   enum VideoElementDimension dimension = VideoModeDimension();
   if (VideoModeHasAttributes()) {
      // Step 1: Fetch the bit value for the pixel(x,y).
      int px;
      switch (dimension) {
         case PIXELS_2x4:   px = VIDEO_GET_PIXEL__GLYPH_(  8, x,y,0);  break;
         case PIXELS_4x4:   px = VIDEO_GET_PIXEL__GLYPH_( 16, x,y,0);  break;
         case PIXELS_4x8:   px = VIDEO_GET_PIXEL__GLYPH_( 32, x,y,0);  break;
         case PIXELS_8x8:   px = VIDEO_GET_PIXEL__GLYPH_( 64, x,y,0);  break;
         case PIXELS_8x16:  px = VIDEO_GET_PIXEL__GLYPH_(128, x,y,0);  break;
         case PIXELS_16x16: px = VIDEO_GET_PIXEL__GLYPH_(256, x,y,0);  break;
         case PIXELS_Nx1:   px = VIDEO_GET_PIXEL__PIXELSTRIP8_(x,y,0); break;
         default: unreachable();
      }

      // Step 2: Get the attribute under the pixel(x,y)
      uint8_t *attr = VideoAttributeAt_(
         x >> ATTRIBUTE_WIDTH_LOG2, y >> ATTRIBUTE_HEIGHT_LOG2);

      // Step 3: Return the attribute's pen/paper depending on px on/off state.
      return px ? AttributeGetPen(attr) : AttributeGetPaper(attr);
   }

   //---------------------------------------------------------------------------
   // Pixel modes (chunky or Bit planes)
   //---------------------------------------------------------------------------
   int low_nibble = VideoModeLowNibble();
   if (dimension == PIXELS_Nx1) {
      switch (low_nibble) {
         // Chunk Pixels or single-plane bit-Pixels:
         case 11: return VIDEO_GET_PIXEL__PIXELSTRIP1_(x,y);
         case 10: return VIDEO_GET_PIXEL__PIXELSTRIP2_(x,y);
         case  9: return VIDEO_GET_PIXEL__PIXELSTRIP4_(x,y);
         case  1: return VIDEO_GET_PIXEL__PIXELSTRIP8_(x,y,0);
                  // ^-- single plane pixel mode could just fallthrough, but let
                  //     make sure it's optimized and doesn't go through a loop

         // Several Bit-pixel Planes: iterate through every plane and
         //                           reconstruct the pixel color bit by bit.
         case  2: // fallthrough
         case  3: // fallthrough
         case  4: // fallthrough
         case  5: // fallthrough
         case  6: // fallthrough
#if (VIDEO_FACTOR_ % 7 == 0)
         case  7: // fallthrough
#endif
         case  8: ;
            unsigned color = 0;
            for (int plane = 0; plane < low_nibble; plane++) {
               color |= VIDEO_GET_PIXEL__PIXELSTRIP8_(x,y,plane) << plane;
            }
            return color;

         default: unreachable();
      }
   }

   //---------------------------------------------------------------------------
   // Tile modes (low_nibble = 9/10/11) or Glyph Planes modes
   //---------------------------------------------------------------------------
   switch (low_nibble) {
      default: unreachable();

      // Tiles: TODO!
      case 11: return 0; // TODO: 8bpp tiles
      case 10: return 0; // TODO: 4bpp tiles
      case  9: return 0; // TODO: 2bpp tiles

      // Glyphs Planes (low_nibble is the number of planes)
      case  1: // fallthrough  // <-- single-plane Glyphs

      case  2: // fallthrough  // <-- several glyph bit planes
      case  3: // fallthrough
      case  4: // fallthrough
      case  5: // fallthrough
      case  6: // fallthrough
#if (VIDEO_FACTOR_ % 7 == 0)
      case  7: // fallthrough
#endif
      case  8: ;
         int px = 0; // color of the pixel(x,y)
         for (int p = 0; p < low_nibble; p++) {
            switch (dimension) {
               case PIXELS_2x4:   px |= VIDEO_GET_PIXEL__GLYPH_(  8, x,y, p) << p; break;
               case PIXELS_4x4:   px |= VIDEO_GET_PIXEL__GLYPH_( 16, x,y, p) << p; break;
               case PIXELS_4x8:   px |= VIDEO_GET_PIXEL__GLYPH_( 32, x,y, p) << p; break;
               case PIXELS_8x8:   px |= VIDEO_GET_PIXEL__GLYPH_( 64, x,y, p) << p; break;
               case PIXELS_8x16:  px |= VIDEO_GET_PIXEL__GLYPH_(128, x,y, p) << p; break;
               case PIXELS_16x16: px |= VIDEO_GET_PIXEL__GLYPH_(256, x,y, p) << p; break;
               default: unreachable();
            }
         }
         return px;
   }
}


// Internal only - Use or PixelSetPixel [or PixelSet_] instead.
static C_HINT_ALWAYS_INLINE void PixelSet_internal_(int x, int y, uint8_t color)
{
   assert(x >= 0 && x < VIDEO_WIDTH);
   assert(y >= 0 && y < VIDEO_HEIGHT);

   //---------------------------------------------------------------------------
   // Attribute modes
   //---------------------------------------------------------------------------
   enum VideoElementDimension dimension = VideoModeDimension();
   if (VideoModeHasAttributes()) {
      // Step 1: Get the attribute under the pixel(x,y)
      uint8_t *attr = VideoAttributeAt_(
         x >> ATTRIBUTE_WIDTH_LOG2, y >> ATTRIBUTE_HEIGHT_LOG2);

      // Step 2: Does the required color exist in the attribute?
      // 1. Yes, it is the pen color    => we'll set the pixel(x,y) bit to 1
      // 2. Yes, it is the paper color  => we'll set the pixel(x,y) bit to 0
      // 3. No => we set the pen color and we'll set the pixel(x,y) bit to 1
      int px = 1;
      if (AttributeGetPaper(attr) == color) {
         px = 0;
      } else if (AttributeGetPen(attr) != color) {
         AttributeSetPen(attr, color);
      }

      // Step 3: Set the value for the pixel(x,y).
      switch (dimension) {
         case PIXELS_2x4:   VIDEO_SET_PIXEL__GLYPH_(  8, x,y,0, px);  return;
         case PIXELS_4x4:   VIDEO_SET_PIXEL__GLYPH_( 16, x,y,0, px);  return;
         case PIXELS_4x8:   VIDEO_SET_PIXEL__GLYPH_( 32, x,y,0, px);  return;
         case PIXELS_8x8:   VIDEO_SET_PIXEL__GLYPH_( 64, x,y,0, px);  return;
         case PIXELS_8x16:  VIDEO_SET_PIXEL__GLYPH_(128, x,y,0, px);  return;
         case PIXELS_16x16: VIDEO_SET_PIXEL__GLYPH_(256, x,y,0, px);  return;
         case PIXELS_Nx1:   VIDEO_SET_PIXEL__PIXELSTRIP8_(x,y,0, px); return;
         default: unreachable();
      }
   }

   //---------------------------------------------------------------------------
   // Pixel modes (chunky or Bit planes)
   //---------------------------------------------------------------------------
   int low_nibble = VideoModeLowNibble();
   if (dimension == PIXELS_Nx1) {
      switch (low_nibble) {
         // Chunk Pixels or single-plane bit-Pixels:
         case 11: VIDEO_SET_PIXEL__PIXELSTRIP1_(x,y, color);   return;
         case 10: VIDEO_SET_PIXEL__PIXELSTRIP2_(x,y, color);   return;
         case  9: VIDEO_SET_PIXEL__PIXELSTRIP4_(x,y, color);   return;
         case  1: VIDEO_SET_PIXEL__PIXELSTRIP8_(x,y,0, color); return;
                  // ^-- single plane pixel mode could just fallthrough, but let
                  //     make sure it's optimized and doesn't go through a loop

         // Several Bit-pixel Planes: iterate through every plane and
         //                           reconstruct the pixel color bit by bit.
         case  2: // fallthrough
         case  3: // fallthrough
         case  4: // fallthrough
         case  5: // fallthrough
         case  6: // fallthrough
#if (VIDEO_FACTOR_ % 7 == 0)
         case  7: // fallthrough
#endif
         case 8:
            for (int p = 0; p < low_nibble; p++) {
               VIDEO_SET_PIXEL__PIXELSTRIP8_(x,y,p, BITS_GET_BIT(color, p));
            }
            return;

         default: unreachable();
      }
   }

   //---------------------------------------------------------------------------
   // Tile modes (low_nibble = 9/10/11) or Glyph Planes modes
   //---------------------------------------------------------------------------
   switch (low_nibble) {
      default: unreachable();

      // Tiles: TODO!
      case 11: return; // TODO: 8bpp tiles
      case 10: return; // TODO: 4bpp tiles
      case  9: return; // TODO: 2bpp tiles

      // Glyphs Planes (low_nibble is the number of planes)
      case  1: // fallthrough  // <-- single-plane Glyphs

      case  2: // fallthrough  // <-- several glyph bit planes
      case  3: // fallthrough
      case  4: // fallthrough
      case  5: // fallthrough
      case  6: // fallthrough
#if (VIDEO_FACTOR_ % 7 == 0)
      case  7: // fallthrough
#endif
      case  8: ;
         for (int p = 0; p < low_nibble; p++) {
            switch (dimension) {
               case PIXELS_2x4:   VIDEO_SET_PIXEL__GLYPH_(  8,x,y,p, BITS_GET_BIT(color, p)); break;
               case PIXELS_4x4:   VIDEO_SET_PIXEL__GLYPH_( 16,x,y,p, BITS_GET_BIT(color, p)); break;
               case PIXELS_4x8:   VIDEO_SET_PIXEL__GLYPH_( 32,x,y,p, BITS_GET_BIT(color, p)); break;
               case PIXELS_8x8:   VIDEO_SET_PIXEL__GLYPH_( 64,x,y,p, BITS_GET_BIT(color, p)); break;
               case PIXELS_8x16:  VIDEO_SET_PIXEL__GLYPH_(128,x,y,p, BITS_GET_BIT(color, p)); break;
               case PIXELS_16x16: VIDEO_SET_PIXEL__GLYPH_(256,x,y,p, BITS_GET_BIT(color, p)); break;
               default: unreachable();
            }
         }
         return;
   }
}


#ifdef KONPU_OPTION_OPTIMIZE_VIDEO_MODE
   // Same as function `PixelGet`, but no bounds checking.
#  define PixelGet_(x, y)          PixelGet_internal_((x), (y))
   // Same as function `PixelSet`, but no bounds checking.
#  define PixelSet_(x, y, color)   PixelSet_internal_((x), (y), (color))
#else
   // Same as function `PixelGet()`, but no bounds checking.
   int PixelGet_(int x, int y);
   // Same as function `PixelSet()`, but no bounds checking.
   void PixelSet_(int x, int y, uint8_t color);
#endif

// Return the color of the pixel (x,y).
// if (x,y) is out of the screen, return the default background color instead.
static inline int PixelGet(int x, int y)
{
   return (x < 0 || x >= VIDEO_WIDTH || y < 0 || y >= VIDEO_HEIGHT) ?
      Video.stray_pixel_strip : PixelGet_(x, y);
      // ^ FIXME: no, we need to return a stray color
}

// Set the the pixel (x,y) to the given color.
// If (x,y) is out of the screen, do nothing.
// Note: in attribute modes, the whole attribute under the pixel
//       would get painted with the given color as its foreground
// TODO: check with FG256 attributes!!!
static inline void PixelSet(int x, int y, uint8_t color)
{
   if (x < 0 || x >= VIDEO_WIDTH || y < 0 || y >= VIDEO_HEIGHT)  return;
   PixelSet_(x, y, color);
}

#endif //include guard
