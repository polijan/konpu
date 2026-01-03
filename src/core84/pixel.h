#ifndef KONPU_PIXEL_H_
#define KONPU_PIXEL_H_
#include "glyph.h"
#include "tile.h"
#include "strip.h"
#include "attribute.h"

//------------------------------------------------------------------------------
// Unsafe MACROS for access to individual Pixels of the video framebuffer.
//
// Those are UNSAFE as that they do NOT do *any* bound checking and may evaluate
// their arguments more than once. To manipulate pixels, it is more convenient
// and safer to use the PixelGet() and PixelSet() functions.
//------------------------------------------------------------------------------

// Implementation notes for VIDEO_<GET|SET>_PIXEL_<ELEMENT>_ macros:
//
// - The pixel (x,y) in the framebuffer is the bit/chunk at position
//   (x % <ELEMENT>_WIDTH, y % <ELEMENT>_HEIGHT) within the element at position
//   (x / <ELEMENT>_WIDTH, y / <ELEMENT>_HEIGHT).
// - As dimension are powers of 2, those optimizations can be used:
//   x / <ELEMENT>_WIDTH, becomes: `x >> <ELEMENT>_WIDTH_LOG2`
//   x % <ELEMENT>_WIDTHh, becomes: `x & (<ELEMENT>_WIDTH - 1)`

# define VIDEO_GET_PIXEL__GLYPH_(N, x,y)                                       \
   GlyphGetPixel_##N##_(                                                       \
      VIDEO_GLYPH(N) [(y) >> UTIL_CAT(GLYPH##N, _HEIGHT_LOG2)]                 \
                     [(x) >> UTIL_CAT(GLYPH##N, _WIDTH_LOG2)],                 \
      (x) & (UTIL_CAT(GLYPH##N, _WIDTH)  - 1),                                 \
      (y) & (UTIL_CAT(GLYPH##N, _HEIGHT) - 1)                                  \
   )

#define VIDEO_GET_PIXEL__GLYPH_IN_PLANE_(N, x,y, plane)                        \
   GlyphGetPixel_##N##_(                                                       \
      VIDEO_GLYPH(N, (plane)) [(y) >> UTIL_CAT(GLYPH##N, _HEIGHT_LOG2)]        \
                              [(x) >> UTIL_CAT(GLYPH##N, _WIDTH_LOG2)],        \
      (x) & (UTIL_CAT(GLYPH##N, _WIDTH)  - 1),                                 \
      (y) & (UTIL_CAT(GLYPH##N, _HEIGHT) - 1)                                  \
   )

#define VIDEO_SET_PIXEL__GLYPH_(N, x,y, bit)                                   \
   do {                                                                        \
      Glyph##N *glyph = & VIDEO_GLYPH(N)                                       \
         [(y) >> UTIL_CAT(GLYPH##N, _HEIGHT_LOG2)]                             \
         [(x) >> UTIL_CAT(GLYPH##N, _WIDTH_LOG2)];                             \
      *glyph = GlyphSetPixel_##N##_(*glyph,                                    \
         (x) & (UTIL_CAT(GLYPH##N, _WIDTH)  - 1),                              \
         (y) & (UTIL_CAT(GLYPH##N, _HEIGHT) - 1),                              \
         (bit) );                                                              \
   } while(0)

#define VIDEO_SET_PIXEL__GLYPH_IN_PLANE_(N, x,y, plane, bit)                   \
   do {                                                                        \
      Glyph##N *glyph = & VIDEO_GLYPH(N, (plane))                              \
         [(y) >> UTIL_CAT(GLYPH##N, _HEIGHT_LOG2)]                             \
         [(x) >> UTIL_CAT(GLYPH##N, _WIDTH_LOG2)];                             \
      *glyph = GlyphSetPixel_##N##_(*glyph,                                    \
         (x) & (UTIL_CAT(GLYPH##N, _WIDTH)  - 1),                              \
         (y) & (UTIL_CAT(GLYPH##N, _HEIGHT) - 1),                              \
         (bit) );                                                              \
   } while(0)


#define VIDEO_GET_PIXEL__STRIP1_(x,y)                                          \
   (Video.strip[(x) + (y) * VIDEO_WIDTH])

#define VIDEO_GET_PIXEL__STRIP2_(x,y)                                          \
   BITS_GET_NIBBLE(                                                            \
      Video.strip[((x)>>STRIP2_WIDTH_LOG2) + (y)*VIDEO_WIDTH_STRIP2],          \
      (x) & (STRIP2_WIDTH - 1)                                                 \
   )

#define VIDEO_GET_PIXEL__STRIP4_(x,y)                                          \
   BITS_GET_QUARTER(                                                           \
      Video.strip[((x)>>STRIP4_WIDTH_LOG2) + (y)*VIDEO_WIDTH_STRIP4],          \
      (x) & (STRIP4_WIDTH - 1)                                                 \
   )

#define VIDEO_GET_PIXEL__STRIP8_(x,y)                                          \
   BITS_GET_BIT(                                                               \
      Video.strip[((x) >> STRIP8_WIDTH_LOG2) + (y) * VIDEO_WIDTH_STRIP8],      \
      (x) & (STRIP8_WIDTH - 1)                                                 \
   )

#define VIDEO_GET_PIXEL__STRIP8_IN_PLANE_(x,y, plane)                          \
   BITS_GET_BIT(                                                               \
      Video.strip[(plane) * VIDEO_SIZE_PLANE +                                 \
         ((x) >> STRIP8_WIDTH_LOG2) + (y) * VIDEO_WIDTH_STRIP8 ],              \
      (x) & (STRIP8_WIDTH - 1)                                                 \
   )

#define VIDEO_SET_PIXEL__STRIP1_(x,y, byte)                                    \
   Video.strip[(x) + (y) * VIDEO_WIDTH] = (byte)

#define VIDEO_SET_PIXEL__STRIP2_(x, y, nibble)                                 \
   do {                                                                        \
      uint8_t *strip = Video.strip +                                           \
         ((x) >> STRIP2_WIDTH_LOG2) + (y) * VIDEO_WIDTH_STRIP2;                \
      *strip = BITS_SET_NIBBLE(*strip, (x) & (STRIP2_WIDTH - 1), (nibble));    \
   } while(0)

#define VIDEO_SET_PIXEL__STRIP4_(x, y, quarter)                                \
   do {                                                                        \
      uint8_t *strip = Video.strip +                                           \
         ((x) >> STRIP4_WIDTH_LOG2) + (y) * VIDEO_WIDTH_STRIP4;                \
      *strip = BITS_SET_QUARTER(*strip, (x) & (STRIP4_WIDTH - 1), (quarter));  \
   } while(0)

#define VIDEO_SET_PIXEL__STRIP8_(x, y, bit)                                    \
   do {                                                                        \
      uint8_t *strip = Video.strip +                                           \
         ((x) >> STRIP8_WIDTH_LOG2) + (y) * VIDEO_WIDTH_STRIP8;                \
      *strip = BITS_SET_BIT(*strip, (x) & (STRIP8_WIDTH - 1), (bit));          \
   } while(0)

#define VIDEO_SET_PIXEL__STRIP8_IN_PLANE_(x, y, plane, bit)                    \
   do {                                                                        \
      uint8_t *strip = Video.strip + (plane) * VIDEO_SIZE_PLANE +              \
         ((x) >> STRIP8_WIDTH_LOG2) + (y) * VIDEO_WIDTH_STRIP8;                \
      *strip = BITS_SET_BIT(*strip, (x) & (STRIP8_WIDTH - 1), (bit));          \
   } while(0)


//------------------------------------------------------------------------------
// Access to individual Pixels in the framebuffer: PixelGet(), PixelSet(), etc.
// whatever the framebuffer elements actually are.
//------------------------------------------------------------------------------

// Internal only - Use PixelGet [or PixelGet_] instead.
static C_HINT_ALWAYS_INLINE int PrivatePixelGet_(int x, int y)
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
         case PIXELS_2x4:   px = VIDEO_GET_PIXEL__GLYPH_(  8, x,y);  break;
         case PIXELS_4x4:   px = VIDEO_GET_PIXEL__GLYPH_( 16, x,y);  break;
         case PIXELS_4x8:   px = VIDEO_GET_PIXEL__GLYPH_( 32, x,y);  break;
         case PIXELS_8x8:   px = VIDEO_GET_PIXEL__GLYPH_( 64, x,y);  break;
         case PIXELS_8x16:  px = VIDEO_GET_PIXEL__GLYPH_(128, x,y);  break;
         case PIXELS_16x16: px = VIDEO_GET_PIXEL__GLYPH_(256, x,y);  break;
         case PIXELS_Nx1:   px = VIDEO_GET_PIXEL__STRIP8_(x,y); break;
         default:           unreachable();
      }

      // Step 2: Get the attribute under the pixel(x,y)
      uint8_t *attr = VideoAttributeAt_(
         x >> ATTRIBUTE_WIDTH_LOG2, y >> ATTRIBUTE_HEIGHT_LOG2);

      // Step 3: Return the attribute's pen/paper depending on px on/off state.
      return px ? AttributeGetPen(attr) : AttributeGetPaper(attr);
   }

   //---------------------------------------------------------------------------
   // Strip modes, chunky or bit planes
   //---------------------------------------------------------------------------
   int low_nibble = VideoModeLowNibble();
   if (dimension == PIXELS_Nx1) {
      switch (low_nibble) {
         // Chunky-pixel Strips
         case PIXEL_CHUNK_BYTE:    return VIDEO_GET_PIXEL__STRIP1_(x,y);
         case PIXEL_CHUNK_NIBBLE:  return VIDEO_GET_PIXEL__STRIP2_(x,y);
         case PIXEL_CHUNK_QUARTER: return VIDEO_GET_PIXEL__STRIP4_(x,y);

         // Single bit-Strip plane: we could just fallthrough, but let's make
         // sure it's optimized and doesn't go through a loop
         case 1: return VIDEO_GET_PIXEL__STRIP8_(x,y);

         // Several Strip8 Planes: iterate through every plane and reconstruct
         //                        the pixel color bit by bit.
         // TODO: wouldn't it be more efficient to construct the whole Strip8,
         //       and only then take the color?
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
               color |= VIDEO_GET_PIXEL__STRIP8_IN_PLANE_(x,y,plane) << plane;
            }
            return color;

         default: unreachable();
      }
   }

   //---------------------------------------------------------------------------
   // Tile modes or Glyph Planes modes
   //---------------------------------------------------------------------------
   switch (low_nibble) {
      default: unreachable();

      // Tiles (low_nibble indicates what pixel chunk is used)
      case PIXEL_CHUNK_BYTE:    return 0; // TODO: 8bpp tiles
      case PIXEL_CHUNK_NIBBLE:  return 0; // TODO: 4bpp tiles
      case PIXEL_CHUNK_QUARTER: return 0; // TODO: 2bpp tiles

      // Glyphs Plane(s) (low_nibble is the number of planes)
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
               case PIXELS_2x4:   px |= VIDEO_GET_PIXEL__GLYPH_IN_PLANE_(  8, x,y, p) << p; break;
               case PIXELS_4x4:   px |= VIDEO_GET_PIXEL__GLYPH_IN_PLANE_( 16, x,y, p) << p; break;
               case PIXELS_4x8:   px |= VIDEO_GET_PIXEL__GLYPH_IN_PLANE_( 32, x,y, p) << p; break;
               case PIXELS_8x8:   px |= VIDEO_GET_PIXEL__GLYPH_IN_PLANE_( 64, x,y, p) << p; break;
               case PIXELS_8x16:  px |= VIDEO_GET_PIXEL__GLYPH_IN_PLANE_(128, x,y, p) << p; break;
               case PIXELS_16x16: px |= VIDEO_GET_PIXEL__GLYPH_IN_PLANE_(256, x,y, p) << p; break;
               case PIXELS_Nx1:   unreachable();
               default:           unreachable();
            }
         }
         return px;
   }
}


// Internal only - Use PixelSet (or PixelSet_) instead.
static C_HINT_ALWAYS_INLINE void PrivatePixelSet_(int x, int y, uint8_t color)
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
         case PIXELS_2x4:   VIDEO_SET_PIXEL__GLYPH_(  8, x,y, px);  return;
         case PIXELS_4x4:   VIDEO_SET_PIXEL__GLYPH_( 16, x,y, px);  return;
         case PIXELS_4x8:   VIDEO_SET_PIXEL__GLYPH_( 32, x,y, px);  return;
         case PIXELS_8x8:   VIDEO_SET_PIXEL__GLYPH_( 64, x,y, px);  return;
         case PIXELS_8x16:  VIDEO_SET_PIXEL__GLYPH_(128, x,y, px);  return;
         case PIXELS_16x16: VIDEO_SET_PIXEL__GLYPH_(256, x,y, px);  return;
         case PIXELS_Nx1:   VIDEO_SET_PIXEL__STRIP8_(x,y, px); return;
         default:           unreachable();
      }
   }

   //---------------------------------------------------------------------------
   // Strip modes (Chunky Strips or Bit Strip8 planes)
   //---------------------------------------------------------------------------
   int low_nibble = VideoModeLowNibble();
   if (dimension == PIXELS_Nx1) {
      switch (low_nibble) {
         // Chunky-pixel Strips
         case PIXEL_CHUNK_BYTE:    VIDEO_SET_PIXEL__STRIP1_(x,y, color); return;
         case PIXEL_CHUNK_NIBBLE:  VIDEO_SET_PIXEL__STRIP2_(x,y, color); return;
         case PIXEL_CHUNK_QUARTER: VIDEO_SET_PIXEL__STRIP4_(x,y, color); return;

         // Single bit-Strip plane: we could just fallthrough, but let's make
         // sure it's optimized and doesn't go through a loop
         case 1: VIDEO_SET_PIXEL__STRIP8_(x,y, color); return;
               // ^-- single plane Strip8 mode could just fallthrough, but let
               //     make sure it's optimized and doesn't go through a loop

         // Several Strip8 Planes: set pixel bit by bit through every plane.
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
               VIDEO_SET_PIXEL__STRIP8_IN_PLANE_(x,y,p, BITS_GET_BIT(color, p));
            }
            return;

         default: unreachable();
      }
   }

   //---------------------------------------------------------------------------
   // Tile modes  or Glyph Planes
   //---------------------------------------------------------------------------
   switch (low_nibble) {
      default: unreachable();

      // Tiles (low_nibble indicates what pixel chunk is used)
      case PIXEL_CHUNK_BYTE:    return; // TODO: 8bpp tiles
      case PIXEL_CHUNK_NIBBLE:  return; // TODO: 4bpp tiles
      case PIXEL_CHUNK_QUARTER: return; // TODO: 2bpp tiles

      // Glyphs Plane(s) (low_nibble is the number of planes)
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
               case PIXELS_2x4:   VIDEO_SET_PIXEL__GLYPH_IN_PLANE_(  8,x,y,p, BITS_GET_BIT(color, p)); break;
               case PIXELS_4x4:   VIDEO_SET_PIXEL__GLYPH_IN_PLANE_( 16,x,y,p, BITS_GET_BIT(color, p)); break;
               case PIXELS_4x8:   VIDEO_SET_PIXEL__GLYPH_IN_PLANE_( 32,x,y,p, BITS_GET_BIT(color, p)); break;
               case PIXELS_8x8:   VIDEO_SET_PIXEL__GLYPH_IN_PLANE_( 64,x,y,p, BITS_GET_BIT(color, p)); break;
               case PIXELS_8x16:  VIDEO_SET_PIXEL__GLYPH_IN_PLANE_(128,x,y,p, BITS_GET_BIT(color, p)); break;
               case PIXELS_16x16: VIDEO_SET_PIXEL__GLYPH_IN_PLANE_(256,x,y,p, BITS_GET_BIT(color, p)); break;
               case PIXELS_Nx1:   unreachable();
               default:           unreachable();
            }
         }
         return;
   }
}

// Internal only - Use or PixelGetAtPlane instead.
static C_HINT_ALWAYS_INLINE int PrivatePixelGetAtPlane_(int x, int y, int plane)
{
   assert(x >= 0 && x < VIDEO_WIDTH);
   assert(y >= 0 && y < VIDEO_HEIGHT);
   assert(!VideoModeHasAttributes());
   assert(VideoModeLowNibble() <= 8);
   assert(plane >= 0 && plane < VideoModeLowNibble());

   switch (VideoModeDimension()) {
      case PIXELS_2x4:   return VIDEO_GET_PIXEL__GLYPH_IN_PLANE_(  8, x,y,plane);
      case PIXELS_4x4:   return VIDEO_GET_PIXEL__GLYPH_IN_PLANE_( 16, x,y,plane);
      case PIXELS_4x8:   return VIDEO_GET_PIXEL__GLYPH_IN_PLANE_( 32, x,y,plane);
      case PIXELS_8x8:   return VIDEO_GET_PIXEL__GLYPH_IN_PLANE_( 64, x,y,plane);
      case PIXELS_8x16:  return VIDEO_GET_PIXEL__GLYPH_IN_PLANE_(128, x,y,plane);
      case PIXELS_16x16: return VIDEO_GET_PIXEL__GLYPH_IN_PLANE_(256, x,y,plane);
      case PIXELS_Nx1:   return VIDEO_GET_PIXEL__STRIP8_IN_PLANE_(x,y,plane);
      default: unreachable();
   }
   return 0;
}
static C_HINT_ALWAYS_INLINE void PrivatePixelSetAtPlane_(int x, int y, int plane, int bit)
{
   assert(x >= 0 && x < VIDEO_WIDTH);
   assert(y >= 0 && y < VIDEO_HEIGHT);
   assert(!VideoModeHasAttributes());
   assert(VideoModeLowNibble() <= 8);
   assert(plane >= 0 && plane < VideoModeLowNibble());

   switch (VideoModeDimension()) {
      case PIXELS_2x4:   VIDEO_SET_PIXEL__GLYPH_IN_PLANE_(  8, x,y,plane, bit); break;
      case PIXELS_4x4:   VIDEO_SET_PIXEL__GLYPH_IN_PLANE_( 16, x,y,plane, bit); break;
      case PIXELS_4x8:   VIDEO_SET_PIXEL__GLYPH_IN_PLANE_( 32, x,y,plane, bit); break;
      case PIXELS_8x8:   VIDEO_SET_PIXEL__GLYPH_IN_PLANE_( 64, x,y,plane, bit); break;
      case PIXELS_8x16:  VIDEO_SET_PIXEL__GLYPH_IN_PLANE_(128, x,y,plane, bit); break;
      case PIXELS_16x16: VIDEO_SET_PIXEL__GLYPH_IN_PLANE_(256, x,y,plane, bit); break;
      case PIXELS_Nx1:   VIDEO_SET_PIXEL__STRIP8_IN_PLANE_(x,y,plane, bit); break;
      default: unreachable();
   }
}

#ifdef KONPU_OPTION_OPTIMIZE_VIDEO_MODE
   // Same as function `PixelGet`, but no bounds checking.
#  define PixelGet_(x,y)                     PrivatePixelGet_((x),(y))
   // Same as function `PixelSet`, but no bounds checking.
#  define PixelSet_(x,y, color)              PrivatePixelSet_((x),(y), (color))
   // Same as function `PixelGetAtPlane`, but no bounds checking.
#  define PixelGetAtPlane_(x,y, plane)       PrivatePixelGetAtPlane_((x),(y), (plane))
   // Same as function `PixelSetAtPlane`, but no bounds checking.
#  define PixelSetAtPlane_(x,y, plane, bit)  PrivatePixelSetAtPlane_((x),(y), (plane), (bit))
#else
   // Same as function `PixelGet()`, but no bounds checking.
   int PixelGet_(int x, int y);
   // Same as function `PixelSet()`, but no bounds checking.
   void PixelSet_(int x, int y, uint8_t color);
   // Same as function `PixelGetAtPlane`, but no bounds checking.
   int PixelGetAtPlane_(int x, int y, int plane);
   // Same as function `PixelSetAtPlane`, but no bounds checking.
   void PixelSetAtPlane_(int x, int y, int plane, int bit);
#endif

// Return the color of the pixel (x,y).
// if (x,y) is out of the screen, return the default background color instead.
static inline int PixelGet(int x, int y)
{
   return (x < 0 || x >= VIDEO_WIDTH || y < 0 || y >= VIDEO_HEIGHT) ?
      Video.stray_strip : PixelGet_(x, y);
      // ^ FIXME: no, we need to return a stray color
}

// Set the pixel (x,y) to the given color.
// If (x,y) is out of the screen, do nothing.
// Note: in attribute modes, the whole attribute under the pixel
//       would get painted with the given color as its foreground
// TODO: check with FG256 attributes!!!
static inline void PixelSet(int x, int y, uint8_t color)
{
   if (x < 0 || x >= VIDEO_WIDTH || y < 0 || y >= VIDEO_HEIGHT)  return;
   PixelSet_(x, y, color);
}

// Return the bit color of the pixel (x,y) at the given plane
// We assume to be in planar mode. If (x,y, plane) is out of the screen,
// return the default background color instead.
static inline int PixelGetAtPlane(int x, int y, int plane)
{
   return  (plane < 0 || plane >= VideoModeLowNibble() ||
            x < 0 || x >= VIDEO_WIDTH                  ||
            y < 0 || y >= VIDEO_HEIGHT                 )?
      Video.stray_strip : PixelGetAtPlane_(x, y, plane);
      // ^ FIXME: no, we need to return a stray color
}

// Set the pixel (x,y) in the given plane to the given bit
// If (x,y,plane) is out of the screen, do nothing.
static inline void PixelSetAtPlane(int x, int y, int plane, int bit)
{
   if (plane < 0 || plane >= VideoModeLowNibble() ||
       x < 0 || x >= VIDEO_WIDTH || y < 0 || y >= VIDEO_HEIGHT)  return;
   PixelSetAtPlane_(x, y, plane, bit);
}

// TODO?
// PixelToggleAtPlane(int x, int y, int plane);
// Toogle the pixel?

#endif //include guard
