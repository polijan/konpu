#ifndef KONPU_PIXEL_H_
#define KONPU_PIXEL_H_
#include "util.h"
#include "attribute.h"
#include "glyph.h"
#include "color.h"
#include "video.h"

//------------------------------------------------------------------------------
// Access to Pixels
//------------------------------------------------------------------------------

// A byte made of chunks containing a line of one or several pixels
//
// Chunk    |#Pixels| #Colors
//----------|-------|---------
// Bits     |  8x1  |    2
// Quarter  |  4x1  |    4
// Nibble   |  2x1  |   16
// Byte     |  1x1  |  256
typedef uint8_t PixelByte;


// PixelByteAt(x,y, [plane=0]) gives a lvalue to the byte containing the
// pixel(s) at coordinate (x,y) (in the given plane).
//
// - Video must be in a pixel mode.
// - Bounds checking: if the pixel (x,y) is outside the bounds of the screen,
//   coordinates will be clamped to the nearest valid edge. And likewise
//   the plane will be clamped to [0, max number of planes[
// - To forgo bounds checking, use PixelByteAt_
#define PixelByteAt(...)                UTIL_OVERLOAD(PixelByteAt, __VA_ARGS__)
#  define PixelByteAt_(...)             UTIL_OVERLOAD(PixelByteAt_, __VA_ARGS__)
#  define PixelByteAt__3_(x,y, plane)   *PixelByteAt__function_((x),(y),(plane))
#  define PixelByteAt__2_(x,y)          *PixelByteAt__function_((x),(y), 0)
#  define PixelByteAt_3_(x,y, plane)    *PixelByteAt_function_((x),(y),(plane))
#  define PixelByteAt_2_(x,y)           *PixelByteAt_function_((x),(y), 0)
   static inline uint8_t *PixelByteAt__function_(int x, int y, int plane) {
      assert(x >= 0 && x < VIDEO_WIDTH);
      assert(y >= 0 && y < VIDEO_HEIGHT);
      int n = VideoModeLowNibble();
      assert(plane >= 0 && plane < n);
      // n <= 8  => planes of bytes which contains 8 (= 2^3) bit colors
      // n == 9  => each byte contains 4 (= 2^2) quarter colors
      // n == 10 => each byte contains 2 (= 2^1) nibble colors
      // n == 11 => each byte contains 1 (= 2^0) color
      return VIDEO_BUFFER + y * VIDEO_WIDTH
         + ((n <= 8) ? ((x >> 3) + VideoPlaneOffset(plane)) : (x >> (11 - n)));
   }
   static inline uint8_t *PixelByteAt_function_(int x, int y, int plane) {
      UtilClampCoordinate(&x, VIDEO_WIDTH);
      UtilClampCoordinate(&y, VIDEO_HEIGHT);
      int n = VideoModeLowNibble();
      UtilClampCoordinate(&plane, n);
      return VIDEO_BUFFER + y * VIDEO_WIDTH
         + ((n <= 8) ? ((x >> 3) + VideoPlaneOffset(plane)) : (x >> (11 - n)));
   }


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
      uint_fast32_t w; // video width  measured in Glyphs (= VIDEO_WIDHT_GLYPH)
      uint_fast32_t h; // video height measured in Glyphs (= VIDEO_HEIGHT_GLYPH)
      int_fast32_t  attr_offset; // offset in the video buffer to the start of attributes

      // Step 1: Fetch `px`, the bit value for the pixel(x,y).
      // - The pixel is at position (x % GLYPH_WIDTH, y % GLYPH_HEIGHT) within
      //   the glyph at position (x / GLYPH_WIDTH, y / GLYPH_HEIGHT).
      // - Also because the glyph's and width and height dimension is always a
      //   power of 2, we can perform the following optimizations:
      //   number / dimension, becomes: x >> log2(dimension)
      //   number % dimension, becomes: x & (dimension - 1)
      int px;
      switch (dimension) {
         case PIXELS_2x4:
            w = VIDEO_WIDTH_GLYPH8;
            h = VIDEO_HEIGHT_GLYPH8;
            attr_offset = (w * h) << PIXELS_2x4;
            px = GlyphGetPixel(
               VIDEO_GLYPH8[(x >> GLYPH8_WIDTH_LOG2) + w * (y >> GLYPH8_HEIGHT_LOG2)],
               x & (GLYPH8_WIDTH - 1), y & (GLYPH8_HEIGHT - 1) );
            break;
         case PIXELS_4x4:
            w = VIDEO_WIDTH_GLYPH16;
            h = VIDEO_HEIGHT_GLYPH16;
            attr_offset = (w * h) << PIXELS_4x4;
            px = GlyphGetPixel(
               VIDEO_GLYPH16[(x >> GLYPH16_WIDTH_LOG2) + w * (y >> GLYPH16_HEIGHT_LOG2)],
               x & (GLYPH16_WIDTH - 1), y & (GLYPH16_HEIGHT - 1) );
            break;
         case PIXELS_4x8:
            w = VIDEO_WIDTH_GLYPH32;
            h = VIDEO_HEIGHT_GLYPH32;
            attr_offset = (w * h) << PIXELS_4x8;
            px = GlyphGetPixel(
               VIDEO_GLYPH32[(x >> GLYPH32_WIDTH_LOG2) + w * (y >> GLYPH32_HEIGHT_LOG2)],
               x & (GLYPH32_WIDTH - 1), y & (GLYPH32_HEIGHT - 1) );
            break;
         case PIXELS_8x8:
            w = VIDEO_WIDTH_GLYPH64;
            h = VIDEO_HEIGHT_GLYPH64;
            attr_offset = (w * h) << PIXELS_8x8;
            px = GlyphGetPixel(
               VIDEO_GLYPH64[(x >> GLYPH64_WIDTH_LOG2) + w * (y >> GLYPH64_HEIGHT_LOG2)],
               x & (GLYPH64_WIDTH - 1), y & (GLYPH64_HEIGHT - 1) );
            break;
         case PIXELS_8x16:
            w = VIDEO_WIDTH_GLYPH128;
            h = VIDEO_HEIGHT_GLYPH128;
            attr_offset = (w * h) << PIXELS_8x16;
            px = GlyphGetPixel(
               VIDEO_GLYPH128[(x >> GLYPH128_WIDTH_LOG2) + w * (y >> GLYPH128_HEIGHT_LOG2)],
               x & (GLYPH128_WIDTH - 1), y & (GLYPH128_HEIGHT - 1) );
            break;
         case PIXELS_16x16:
            w = VIDEO_WIDTH_GLYPH256;
            h = VIDEO_HEIGHT_GLYPH256;
            attr_offset = (w * h) << PIXELS_16x16;
            px = GlyphGetPixel(
               VIDEO_GLYPH256[(x >> GLYPH256_WIDTH_LOG2) + w * (y >> GLYPH256_HEIGHT_LOG2)],
               x & (GLYPH256_WIDTH - 1), y & (GLYPH256_HEIGHT - 1) );
            break;
         case PIXELS: // bit-pixels are similar to 8x1 glyphs
            w = VIDEO_WIDTH >> 3;
            h = VIDEO_HEIGHT;
            attr_offset = w * h;
            px = BITS_GET_BIT(VIDEO_BUFFER[(x >> 3) + y * w], x & 7);
            break;
         default:
            unreachable();
      }

      // Step 2: Get the attribute under the pixel(x,y)
      int w_log2 = AttributeWidthLog2();
      x >>= w_log2;
      y >>= AttributeHeightLog2();
      uint8_t *attr = VIDEO_BUFFER + attr_offset
         + ((x + y * (VIDEO_WIDTH >> w_log2)) << AttributeHasTwoBytes());

      // Step 3: Return the attribute's fg or bg color
      //         depending on whether px is on or off.
      return (px) ? AttributeGetForeground(attr)
                  : AttributeGetBackground(attr);
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
   if (dimension == PIXELS) {
      uint8_t *ptr = VIDEO_BUFFER + y * VIDEO_WIDTH;
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
   // Tile modes (LowNibble = 9/10/11) or Glyph BitPlanes modes
   //---------------------------------------------------------------------------
   switch (low_nibble) {
      // Tiles modes:
      case 11: return 0; // TODO
      case 10: return 0; // TODO
      case  9: return 0; // TODO

      // Single-plane Glyphs
      case  1: // fallthrough  // single-plane Glyphs
      // Several glyph bit planes (construct glyph)
      case  2: // fallthrough
      case  3: // fallthrough
      case  4: // fallthrough
      case  5: // fallthrough
      case  6: // fallthrough
#if (VIDEO_FACTOR_ % 7 == 0)
      case  7: // fallthrough
#endif
      case  8: ;
         int px = 0; // color of the pixel(x,y)
         int w;      // video width measured in Glyphs (= VIDEO_WIDHT_GLYPH)
         switch (dimension) {
            case PIXELS_2x4:
               w = VIDEO_WIDTH_GLYPH8;
               for (int plane = 0; plane < low_nibble; plane++)
                  px |= GlyphGetPixel(
                     VIDEO_GLYPH8[(x >> GLYPH8_WIDTH_LOG2) + w * (y >> GLYPH8_HEIGHT_LOG2)],
                     x & (GLYPH8_WIDTH - 1), y & (GLYPH8_HEIGHT - 1) ) << plane;
               return px;
            case PIXELS_4x4:
               w = VIDEO_WIDTH_GLYPH16;
               for (int plane = 0; plane < low_nibble; plane++)
                  px |= GlyphGetPixel(
                     VIDEO_GLYPH16[(x >> GLYPH16_WIDTH_LOG2) + w * (y >> GLYPH16_HEIGHT_LOG2)],
                     x & (GLYPH16_WIDTH - 1), y & (GLYPH16_HEIGHT - 1) ) << plane;
               return px;
            case PIXELS_4x8:
               w = VIDEO_WIDTH_GLYPH32;
               for (int plane = 0; plane < low_nibble; plane++)
                  px |= GlyphGetPixel(
                     VIDEO_GLYPH32[(x >> GLYPH32_WIDTH_LOG2) + w * (y >> GLYPH32_HEIGHT_LOG2)],
                     x & (GLYPH32_WIDTH - 1), y & (GLYPH32_HEIGHT - 1) ) << plane;
               return px;
            case PIXELS_8x8:
               w = VIDEO_WIDTH_GLYPH64;
               for (int plane = 0; plane < low_nibble; plane++)
                  px |= GlyphGetPixel(
                     VIDEO_GLYPH64[(x >> GLYPH64_WIDTH_LOG2) + w * (y >> GLYPH64_HEIGHT_LOG2)],
                     x & (GLYPH64_WIDTH - 1), y & (GLYPH64_HEIGHT - 1) ) << plane;
               return px;
            case PIXELS_8x16:
               w = VIDEO_WIDTH_GLYPH128;
               for (int plane = 0; plane < low_nibble; plane++)
                  px |= GlyphGetPixel(
                     VIDEO_GLYPH128[(x >> GLYPH128_WIDTH_LOG2) + w * (y >> GLYPH128_HEIGHT_LOG2)],
                     x & (GLYPH128_WIDTH - 1), y & (GLYPH128_HEIGHT - 1) ) << plane;
               return px;
            case PIXELS_16x16:
               w = VIDEO_WIDTH_GLYPH256;
               for (int plane = 0; plane < low_nibble; plane++)
                  px |= GlyphGetPixel(
                     VIDEO_GLYPH256[(x >> GLYPH256_WIDTH_LOG2) + w * (y >> GLYPH256_HEIGHT_LOG2)],
                     x & (GLYPH256_WIDTH - 1), y & (GLYPH256_HEIGHT - 1) ) << plane;
               return px;
            default: unreachable();
         }
      default: unreachable();
   }
}

// Internal only - Use or PixelSetPixel [or PixelSet_] instead.
static C_HINT_ALWAYS_INLINE void PixelSet_internal_(int x, int y, int color)
{
   assert(x >= 0 && x < VIDEO_WIDTH);
   assert(y >= 0 && y < VIDEO_HEIGHT);

   //---------------------------------------------------------------------------
   // Attribute modes
   //---------------------------------------------------------------------------
   enum VideoElementDimension dimension = VideoModeDimension();
   if (VideoModeHasAttributes()) {
      /* TODO
      uint_fast32_t w; // video width  measured in Glyphs (= VIDEO_WIDHT_GLYPH)
      uint_fast32_t h; // video height measured in Glyphs (= VIDEO_HEIGHT_GLYPH)
      int_fast32_t  attr_offset; // offset in the video buffer to the start of attributes

      int w_log2 = AttributeWidthLog2();
      x >>= w_log2;
      y >>= AttributeHeightLog2();
      uint8_t *attr = VIDEO_BUFFER + attr_offset
         + ((x + y * (VIDEO_WIDTH >> w_log2)) << AttributeHasTwoBytes());


      // Step 1: Set bit pixel(x,y)
      switch (dimension) {
         case PIXELS_2x4:
            w = VIDEO_WIDTH_GLYPH8;
            h = VIDEO_HEIGHT_GLYPH8;
            attr_offset = (w * h) << PIXELS_2x4;
            GlyphSetPixel(
               VIDEO_GLYPH8[(x >> GLYPH8_WIDTH_LOG2) + w * (y >> GLYPH8_HEIGHT_LOG2)],
               x & (GLYPH8_WIDTH - 1), y & (GLYPH8_HEIGHT - 1), 1);
            break;
         case PIXELS_4x4:
            w = VIDEO_WIDTH_GLYPH16;
            h = VIDEO_HEIGHT_GLYPH16;
            attr_offset = (w * h) << PIXELS_4x4;
            px = GlyphPixelAt(
               VIDEO_GLYPH16[(x >> GLYPH16_WIDTH_LOG2) + w * (y >> GLYPH16_HEIGHT_LOG2)],
               x & (GLYPH16_WIDTH - 1), y & (GLYPH16_HEIGHT - 1) );
            break;
         case PIXELS_4x8:
            w = VIDEO_WIDTH_GLYPH32;
            h = VIDEO_HEIGHT_GLYPH32;
            attr_offset = (w * h) << PIXELS_4x8;
            px = GlyphPixelAt(
               VIDEO_GLYPH32[(x >> GLYPH32_WIDTH_LOG2) + w * (y >> GLYPH32_HEIGHT_LOG2)],
               x & (GLYPH32_WIDTH - 1), y & (GLYPH32_HEIGHT - 1) );
            break;
         case PIXELS_8x8:
            w = VIDEO_WIDTH_GLYPH64;
            h = VIDEO_HEIGHT_GLYPH64;
            attr_offset = (w * h) << PIXELS_8x8;
            px = GlyphPixelAt(
               VIDEO_GLYPH64[(x >> GLYPH64_WIDTH_LOG2) + w * (y >> GLYPH64_HEIGHT_LOG2)],
               x & (GLYPH64_WIDTH - 1), y & (GLYPH64_HEIGHT - 1) );
            break;
         case PIXELS_8x16:
            w = VIDEO_WIDTH_GLYPH128;
            h = VIDEO_HEIGHT_GLYPH128;
            attr_offset = (w * h) << PIXELS_8x16;
            px = GlyphPixelAt(
               VIDEO_GLYPH128[(x >> GLYPH128_WIDTH_LOG2) + w * (y >> GLYPH128_HEIGHT_LOG2)],
               x & (GLYPH128_WIDTH - 1), y & (GLYPH128_HEIGHT - 1) );
            break;
         case PIXELS_16x16:
            w = VIDEO_WIDTH_GLYPH256;
            h = VIDEO_HEIGHT_GLYPH256;
            attr_offset = (w * h) << PIXELS_16x16;
            px = GlyphPixelAt(
               VIDEO_GLYPH256[(x >> GLYPH256_WIDTH_LOG2) + w * (y >> GLYPH256_HEIGHT_LOG2)],
               x & (GLYPH256_WIDTH - 1), y & (GLYPH256_HEIGHT - 1) );
            break;
         case PIXELS:
            w = VIDEO_WIDTH >> 3;
            h = VIDEO_HEIGHT;
            attr_offset = w * h;
            BITS_SET_BIT(VIDEO_BUFFER[(x >> 3) + y * w], x & 7, 1);
            break;
         default:
            unreachable();
      }

      // Step 2: Get the attribute under the pixel(x,y) and set its foreground
      int w_log2 = AttributeWidthLog2();
      x >>= w_log2;
      y >>= AttributeHeightLog2();
      uint8_t *attr = VIDEO_BUFFER + attr_offset
         + ((x + y * (VIDEO_WIDTH >> w_log2)) << AttributeHasTwoBytes());
      AttributeSetForeground(attr, color);
   */
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
   if (dimension == PIXELS) {

      uint8_t *ptr = VIDEO_BUFFER + y * VIDEO_WIDTH;
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
   void PixelSet_(int x, int y, int color);
#endif

// Return the color of the pixel (x,y).
// if (x,y) is out of the screen, return the default background color instead.
static inline int PixelGet(int x, int y)
{
   return (x < 0 || x >= VIDEO_WIDTH || y < 0 || y >= VIDEO_HEIGHT) ?
      COLOR_DEFAULT_BG : PixelGet_(x, y);
   //TODO: FIXME
   // We could return the border color? It could make sense, however the
   // border color is always in 256 color mode, but the framebuffer isn't.
   // What about COLOR_DEFAULT_BG, it's the same thing or what?
}

// Set the the pixel (x,y) to the given color.
// If (x,y) is out of the screen, do nothing.
// Note: in attribute modes, the whole attribute under the pixel
//       would get painted with the given color as its foreground
// TODO: check with FG256 attributes!!!
static inline void PixelSet(int x, int y, int color)
{
   if (x < 0 || x >= VIDEO_WIDTH || y < 0 || y >= VIDEO_HEIGHT)  return;
   PixelSet_(x, y, color);
}

#endif //include guard
