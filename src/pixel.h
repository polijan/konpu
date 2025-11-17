#ifndef KONPU_PIXEL_H_
#define KONPU_PIXEL_H_
#include "util.h"
#include "video.h"

// A "strip" is a byte representing a small horizontal line of 1,2,4,or 8
// pixels. When the framebuffer contains pixels, they are stored as strips.

//------------------------------------------------------------------------------
// PixelStrip: Dimensions and Properties
//----------------------------..----------.---------.--------.------------------
// PixelStrip                 // Pixel    |Dimension|  # of  | Video.mode's
typedef uint8_t PixelStrip;   // Chunk(s) |in Pixels| Colors | Low Nibble
//----------------------------//----------|---------|--------|------------------
typedef uint8_t  PixelStrip8; // Bits     |  8 x 1  |    2   |    1
typedef uint8_t  PixelStrip4; // Quarters |  4 x 1  |    4   |    9
typedef uint8_t  PixelStrip2; // Nibbles  |  2 x 1  |   16   |   10
typedef uint8_t  PixelStrip1; // Byte     |  1 x 1  |  256   |   11
//---------------------------------------'---------'--------'-------------------

#define PIXELSTRIP_WIDTH       (1 << PIXELSTRIP_WIDTH_LOG2)
#define PIXELSTRIP_HEIGHT      1
#define PIXELSTRIP1_WIDTH      1
#define PIXELSTRIP2_WIDTH      2
#define PIXELSTRIP4_WIDTH      4
#define PIXELSTRIP8_WIDTH      8
#define PIXELSTRIP1_WIDTH_LOG2 0
#define PIXELSTRIP2_WIDTH_LOG2 1
#define PIXELSTRIP4_WIDTH_LOG2 2
#define PIXELSTRIP8_WIDTH_LOG2 3
#define PIXELSTRIP_HEIGHT_LOG2 0
#define PIXELSTRIP_WIDTH_LOG2  ((VIDEO_MODE&8)? (11 - VideoModeLowNibble()) : 3)
   //   ^ Based on table above, we can infer PIXELSTRIP_WIDTH is the following
   //     power of 2 based on Video.mode's low nibble n: (n==1) ? 3 : 11-n
   //     Furthermore, checking Video.mode's low nibble being 1 vs 9/10/11,
   //     could further simplify into checking (Video.mode & 8)

#define PIXELSTRIP_COLOR_DEPTH \
   ((VIDEO_MODE & 8) ? (1 << (VideoModeLowNibble() - 8)) : 1)


//------------------------------------------------------------------------------
// Access to PixelStrip elements: Get(), Set(), etc.
//------------------------------------------------------------------------------
// Access to PixelStrip can be done safely with those "functions":
//
// uint8_t PixelStripGet([Surface], int x,y, [int plane]);
// void    PixelStripSet([Surface], int x,y, uint8_t pixel_strip [int plane]);
//
// Note: When (x,y) is out-of-bounds with respect to the given Surface,
//       - Set(): doesn't do anything.
//       - Get(): all chunks of the returned PixelStrip are filled with the
//                stray color.
//------------------------------------------------------------------------------

// TODO: replace PixelStripAt() with PixelStripGet() and PixelStripSet()


// PixelStripAt(x,y, [plane=0]) gives an lvalue to the byte containing the
// pixel(s) at coordinate (x,y) (in the given plane).
//
// - Video must be in a pixel mode.
// - Bounds checking: if the pixel (x,y) is outside the bounds of the screen,
//   coordinates will be clamped to the nearest valid edge. And likewise
//   the plane will be clamped to [0, max number of planes[
// - To forgo bounds checking, use PixelStripAt_
#define PixelStripAt(...)              UTIL_OVERLOAD(PixelStripAt, __VA_ARGS__)
#  define PixelStripAt_(...)           UTIL_OVERLOAD(PixelStripAt_, __VA_ARGS__)
#  define PixelStripAt__3_(x,y, plane) *PixelStripAt__function_((x),(y),(plane))
#  define PixelStripAt__2_(x,y)        *PixelStripAt__function_((x),(y), 0)
#  define PixelStripAt_3_(x,y, plane)  *PixelStripAt_function_((x),(y),(plane))
#  define PixelStripAt_2_(x,y)         *PixelStripAt_function_((x),(y), 0)
   static inline uint8_t *PixelStripAt__function_(int x, int y, int plane) {
      assert(x >= 0 && x < VIDEO_WIDTH); // <--- well, that's wrong!
      assert(y >= 0 && y < VIDEO_HEIGHT);
      int n = VideoModeLowNibble();
      assert(plane >= 0 && plane < n);
      // n <= 8  => planes of bytes which contains 8 (= 2^3) bit colors
      // n == 9  => each byte contains 4 (= 2^2) quarter colors
      // n == 10 => each byte contains 2 (= 2^1) nibble colors
      // n == 11 => each byte contains 1 (= 2^0) color
      return Video.pixel + y * VIDEO_WIDTH
         + ((n <= 8) ? ((x >> 3) + VideoPlaneOffset(plane)) : (x >> (11 - n)));
   }
   static inline uint8_t *PixelStripAt_function_(int x, int y, int plane) {
      UtilClampCoordinate(&x, VIDEO_WIDTH);
      UtilClampCoordinate(&y, VIDEO_HEIGHT);
      int n = VideoModeLowNibble();
      UtilClampCoordinate(&plane, n);
      return Video.pixel + y * VIDEO_WIDTH
         + ((n <= 8) ? ((x >> 3) + VideoPlaneOffset(plane)) : (x >> (11 - n)));
   }



//------------------------------------------------------------------------------
// Access to individual Pixels in the framebuffer: PixelGet(), PixelSet(), etc.
// whatever the framebuffer elements actually are.
//------------------------------------------------------------------------------
#include "glyph.h"
#include "attribute.h"
#include "color.h"

// _PIXEL_GET_FROM_GLYPHS(N, int x,y, [int plane = 0])
// If the framebuffer has Glyph<N> elements, return the bit value of pixel(x,y)
#define _PIXEL_GET_FROM_GLYPHS(...)                                            \
   UTIL_OVERLOAD(_PIXEL_GET_FROM_GLYPHS, __VA_ARGS__)
   #define _PIXEL_GET_FROM_GLYPHS_3_(N, x,y)                                   \
      _PIXEL_GET_FROM_GLYPHS_4_(N,x,y,0)
   #define _PIXEL_GET_FROM_GLYPHS_4_(N, x,y, plane)                            \
      GlyphGetPixel_##N##_(                                                    \
         VideoPlaneAt__2_(Glyph##N, (plane))[                                 \
              ((x) >> UTIL_CAT(GLYPH##N, _WIDTH_LOG2))                         \
            + ((y) >> UTIL_CAT(GLYPH##N, _HEIGHT_LOG2)) * UTIL_CAT(VIDEO_WIDTH_, GLYPH##N) \
         ],                                                                     \
         (x) & (UTIL_CAT(GLYPH##N, _WIDTH)  - 1),                              \
         (y) & (UTIL_CAT(GLYPH##N, _HEIGHT) - 1)                               \
      )
   // This looks complex due to macros, but here is how pixel is fetched:
   // We call GlyphGetPixel on:
   // - The video pixel is at position (x % GLYPH_WIDTH, y % GLYPH_HEIGHT)
   //   within the glyph at position (x / GLYPH_WIDTH, y / GLYPH_HEIGHT).
   // - Because Glyphs width (and height) are powers of 2, the following
   //   optimizations are performed:
   //   x / glyph_width, becomes: x >> log2(glyph_width)
   //   x % glyph_width, becomes: x & (glyph_width - 1)
   // Implementation of PIXEL_SET_TO_GLYPH is similar below:

// _PIXEL_SET_TO_GLYPHS(N, int x,y, int bit_zero_or_one, [int plane = 0])
// If the framebuffer has Glyph<N> elements, set the value of bit pixel(x,y)
#define _PIXEL_SET_TO_GLYPHS(...)                                              \
   UTIL_OVERLOAD(_PIXEL_SET_TO_GLYPHS, __VA_ARGS__)
#  define _PIXEL_SET_TO_GLYPHS_4_(N, x,y, bit)                                 \
      _PIXEL_SET_TO_GLYPHS_5_(N,x,y, bit, 0)
#  define _PIXEL_SET_TO_GLYPHS_5_(N, x,y, bit, plane)                          \
      do {                                                                     \
         Glyph##N *glyph = VideoPlaneAt__2_(Glyph##N, (plane))                 \
            + ((x) >> UTIL_CAT(GLYPH##N, _WIDTH_LOG2))                         \
            + ((y) >> UTIL_CAT(GLYPH##N, _HEIGHT_LOG2)) * UTIL_CAT(VIDEO_WIDTH_, GLYPH##N); \
         *glyph = GlyphSetPixel_##N##_(*glyph,                                 \
            (x) & (UTIL_CAT(GLYPH##N, _WIDTH)  - 1),                           \
            (y) & (UTIL_CAT(GLYPH##N, _HEIGHT) - 1),                           \
            (bit) );                                                           \
      } while(0)



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
         case PIXELS_2x4:   px = _PIXEL_GET_FROM_GLYPHS(  8, x,y); break;
         case PIXELS_4x4:   px = _PIXEL_GET_FROM_GLYPHS( 16, x,y); break;
         case PIXELS_4x8:   px = _PIXEL_GET_FROM_GLYPHS( 32, x,y); break;
         case PIXELS_8x8:   px = _PIXEL_GET_FROM_GLYPHS( 64, x,y); break;
         case PIXELS_8x16:  px = _PIXEL_GET_FROM_GLYPHS(128, x,y); break;
         case PIXELS_16x16: px = _PIXEL_GET_FROM_GLYPHS(256, x,y); break;
         case PIXELS_Nx1:   // bit pixels strip (similar to 8x1 glyphs)
            px = BITS_GET_BIT(Video.pixel[(x >> 3) + (y >> 0) * (VIDEO_WIDTH >> 3)], x & 7);
         // px = BITS_GET_BIT(*PIXELSTRIP_AT_(BITS, x >> 3, y), x & 7);
            break;
         default: unreachable();
      }

      // Step 2: Get the attribute under the pixel(x,y)
      uint8_t *attr = VideoAttributeAt_(
         x >> ATTRIBUTE_WIDTH_LOG2, y >> ATTRIBUTE_HEIGHT_LOG2);
/*
      int w_log2 = AttributeWidthLog2();
      x >>= w_log2;
      y >>= AttributeHeightLog2();
      uint8_t *attr = VIDEO_ATTRIBUTE
         + ((x + y * (VIDEO_WIDTH >> w_log2)) << AttributeHasTwoBytes());
*/

      // Step 3: Return the attribute's pen/paper depending on px on/off state.
      return px ? AttributeGetPen(attr) : AttributeGetPaper(attr);
   }

   //---------------------------------------------------------------------------
   // Pixel modes
   //---------------------------------------------------------------------------
   // - Chunky-pixels or just 1 bitplane:
   //
   //   Low   | Chunk size  | The pixel is within this| Chunk index for the
   //   Nibble|  (aka bpp)  | byte in the video buffer| pixel in that byte
   //   ------|-------------|-------------------------|--------------------
   //      1  | 1 (bit)     | x / 8 + y * VIDEO_WIDTH |   x % 8
   //      9  | 2 (quarter) | x / 4 + y * VIDEO_WIDTH |   x % 4
   //     10  | 4 (nibble)  | x / 2 + y * VIDEO_WIDTH |   x % 2
   //     11  | 8 (byte)    |   x   + y * VIDEO_WIDTH |     0
   //
   // - Several BitPlanes:
   //   Access is same as 1 bit, but we must iterate through every plane and
   //   reconstruct the pixel color bit by bit.
   int low_nibble = VideoModeLowNibble();
   if (dimension == PIXELS_Nx1) {
      uint8_t *ptr = Video.pixel + y * VIDEO_WIDTH;
      switch (low_nibble) {
         // Chunk Pixels or single-plane bit-Pixels
         case 11: return ptr[x];
         case 10: return BITS_GET_NIBBLE( ptr[x >> 1], x & 1);
         case  9: return BITS_GET_QUARTER(ptr[x >> 2], x & 3);
         case  1: return BITS_GET_BIT(    ptr[x >> 3], x & 7);
                  // ^-- single plane pixel mode could just fallthrough, but let
                  //     make sure it's optimized and doesn't go through a loop

         // Several bitplanes
         case  2: // fallthrough
         case  3: // fallthrough
         case  4: // fallthrough
         case  5: // fallthrough
         case  6: // fallthrough
#if (VIDEO_FACTOR_ % 7 == 0)
         case  7: // fallthrough
#endif
         case  8: ;
            int color = 0;
            int plane_size = VIDEO_SIZE / low_nibble;
            for (int plane = 0; plane < low_nibble; plane++) {
               color |= BITS_GET_BIT(ptr[x >> 3], x & 7) << plane;
               ptr += plane_size;
            }
            return color;

         default: unreachable();
      }
   }

   //---------------------------------------------------------------------------
   // Tile modes (low_nibble = 9/10/11) or Glyph BitPlanes modes
   //---------------------------------------------------------------------------
   switch (low_nibble) {
      default: unreachable();

      // Tiles: TODO!
      case 11: return 0; // TODO
      case 10: return 0; // TODO
      case  9: return 0; // TODO

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
               case PIXELS_2x4:   px |= _PIXEL_GET_FROM_GLYPHS(  8, x,y, p) << p; break;
               case PIXELS_4x4:   px |= _PIXEL_GET_FROM_GLYPHS( 16, x,y, p) << p; break;
               case PIXELS_4x8:   px |= _PIXEL_GET_FROM_GLYPHS( 32, x,y, p) << p; break;
               case PIXELS_8x8:   px |= _PIXEL_GET_FROM_GLYPHS( 64, x,y, p) << p; break;
               case PIXELS_8x16:  px |= _PIXEL_GET_FROM_GLYPHS(128, x,y, p) << p; break;
               case PIXELS_16x16: px |= _PIXEL_GET_FROM_GLYPHS(256, x,y, p) << p; break;
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
         case PIXELS_2x4:   _PIXEL_SET_TO_GLYPHS(  8, x,y, px); return;
         case PIXELS_4x4:   _PIXEL_SET_TO_GLYPHS( 16, x,y, px); return;
         case PIXELS_4x8:   _PIXEL_SET_TO_GLYPHS( 32, x,y, px); return;
         case PIXELS_8x8:   _PIXEL_SET_TO_GLYPHS( 64, x,y, px); return;
         case PIXELS_8x16:  _PIXEL_SET_TO_GLYPHS(128, x,y, px); return;
         case PIXELS_16x16: _PIXEL_SET_TO_GLYPHS(256, x,y, px); return;
         case PIXELS_Nx1:   ; // 8x1 bit pixel strip
            uint8_t *strip = Video.pixel_strip8 + (x >> 3) + (y >> 0) * (VIDEO_WIDTH >> 3);
            *strip = BITS_SET_BIT(*strip, x & 7, px);

//            BITS_SET_BIT(Video.pixel[(x >> 3) + (y >> 0) * (VIDEO_WIDTH >> 3)], x & 7, px);
         // BITS_SET_BIT(*PIXELSTRIP_AT_(BITS, x >> 3, y), x & 7, px);
            return;
         default: unreachable();
      }
   }

   //---------------------------------------------------------------------------
   // Pixel modes
   //---------------------------------------------------------------------------
   // - Chunky-pixels or just 1 bitplane:
   //
   //   Low   | Chunk size  | The pixel is within this| Chunk index for the
   //   Nibble|  (aka bpp)  | byte in the video buffer| pixel in that byte
   //   ------|-------------|-------------------------|--------------------
   //      1  | 1 (bit)     | x / 8 + y * VIDEO_WIDTH |   x % 8
   //      9  | 2 (quarter) | x / 4 + y * VIDEO_WIDTH |   x % 4
   //     10  | 4 (nibble)  | x / 2 + y * VIDEO_WIDTH |   x % 2
   //     11  | 8 (byte)    |   x   + y * VIDEO_WIDTH |     0
   //
   // - Several BitPlanes:
   //   Access is same as 1 bit, but we must iterate through every plane and
   //   reconstruct the pixel color bit by bit.
   int low_nibble = VideoModeLowNibble();
   if (dimension == PIXELS_Nx1) {

      uint8_t *ptr = Video.pixel + y * VIDEO_WIDTH;
      switch (low_nibble) {
         // Chunk Pixels or single-plane bit-Pixels
         case 11: ptr[x] = color; return;
         case 10: ptr[x >> 1] = BITS_SET_NIBBLE(ptr[x >> 1], x & 1, color); return;
         case  9: ptr[x >> 2] = BITS_SET_QUARTER(ptr[x >> 2], x & 3, color); return;
         case  1: ptr[x >> 3] = BITS_SET_BIT(ptr[x >> 3], x & 7, color); return;
               // ^-- single plane pixel mode could just fallthrough, but let
               //     make sure it's optimized and doesn't go through a loop

         // Several bitplanes
         case  2: // fallthrough
         case  3: // fallthrough
         case  4: // fallthrough
         case  5: // fallthrough
         case  6: // fallthrough
#if (VIDEO_FACTOR_ % 7 == 0)
         case  7: // fallthrough
#endif
         case  8: ;
            int plane_size = VIDEO_SIZE / low_nibble;
            for (int plane = 0; plane < low_nibble; plane++) {
               ptr[x >> 3] = BITS_SET_BIT(ptr[x >> 3], x & 7, color);
               ptr += plane_size;
            }
            return;

         default: unreachable();
      }
   }

   //---------------------------------------------------------------------------
   // Tile modes (LowNibble = 9/10/11) or Glyph BitPlanes modes
   //---------------------------------------------------------------------------
   // TODO
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
