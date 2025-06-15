#ifndef KONPU_VIDEO_H_
#define KONPU_VIDEO_H_
#include "util.h"
#include "attribute.h"
#include "glyph.h"
#include "color.h"

//------------------------------------------------------------------------------
// Video Buffer, access and dimensions
//------------------------------------------------------------------------------

// Access to elements in the video buffer (only makes sene in video modes where
// such elements exist)
#define VIDEO_BUFFER                (RAM + RAM_VIDEO_BUFFER)
#define VIDEO_GLYPH                 ((void*)VIDEO_BUFFER)
#define VIDEO_GLYPH8                VIDEO_BUFFER
#define VIDEO_GLYPH16               ((uint16_t*)VIDEO_BUFFER)
#define VIDEO_GLYPH32               ((uint32_t*)VIDEO_BUFFER)
#define VIDEO_GLYPH64               ((uint64_t*)VIDEO_BUFFER)
#define VIDEO_GLYPH128              ((Glyph128*)VIDEO_BUFFER)
#define VIDEO_GLYPH256              ((Glyph256*)VIDEO_BUFFER)
#define VIDEO_ATTRIBUTE             (VIDEO_BUFFER + (VIDEO_COUNT_PIXELS_ >> 3)) //TODO: verify

// Number of such elements (when framebuffer contains element of that type)
#define VIDEO_COUNT_GLYPH           (VIDEO_COUNT_PIXELS_ >> (3 + VideoModeDimension()))
#define VIDEO_COUNT_GLYPH8          (VIDEO_COUNT_PIXELS_ >> (GLYPH8_WIDTH_LOG2   + GLYPH8_HEIGHT_LOG2))
#define VIDEO_COUNT_GLYPH16         (VIDEO_COUNT_PIXELS_ >> (GLYPH16_WIDTH_LOG2  + GLYPH16_HEIGHT_LOG2))
#define VIDEO_COUNT_GLYPH32         (VIDEO_COUNT_PIXELS_ >> (GLYPH32_WIDTH_LOG2  + GLYPH32_HEIGHT_LOG2))
#define VIDEO_COUNT_GLYPH64         (VIDEO_COUNT_PIXELS_ >> (GLYPH64_WIDTH_LOG2  + GLYPH64_HEIGHT_LOG2))
#define VIDEO_COUNT_GLYPH128        (VIDEO_COUNT_PIXELS_ >> (GLYPH128_WIDTH_LOG2 + GLYPH128_HEIGHT_LOG2))
#define VIDEO_COUNT_GLYPH256        (VIDEO_COUNT_PIXELS_ >> (GLYPH128_WIDTH_LOG2 + GLYPH128_HEIGHT_LOG2))
#define VIDEO_COUNT_TILE            (VIDEO_COUNT_PIXELS_ >> (3 + VideoModeDimension()))
#define VIDEO_COUNT_PIXELBYTE_8x1   (VIDEO_COUNT_PIXELS_ >> 3) // bit-pixels
#define VIDEO_COUNT_PIXELBYTE_4x1   (VIDEO_COUNT_PIXELS_ >> 2) // quarter-pixels
#define VIDEO_COUNT_PIXELBYTE_2x1   (VIDEO_COUNT_PIXELS_ >> 1) // nibble-pixels
#define VIDEO_COUNT_PIXELBYTE_1x1   VIDEO_COUNT_PIXELS_        // byte-pixels
#define VIDEO_COUNT_PIXELBYTE       //TODO. NO IT DEPENDS ON WHAT SORT OF PIXEL
#define VIDEO_COUNT_ATTRIBUTE       //TODO: does it make sense, because attributes is wide sometimes

// Width of the video framebuffer but in other "units" than pixels
// (framebuffer must contain element of the type)
#define VIDEO_WIDTH_GLYPH           (VIDEO_WIDTH >> GLYPH_WIDTH_LOG2)
#define VIDEO_WIDTH_GLYPH8          (VIDEO_WIDTH >> GLYPH8_WIDTH_LOG2)
#define VIDEO_WIDTH_GLYPH16         (VIDEO_WIDTH >> GLYPH16_WIDTH_LOG2)
#define VIDEO_WIDTH_GLYPH32         (VIDEO_WIDTH >> GLYPH32_WIDTH_LOG2)
#define VIDEO_WIDTH_GLYPH64         (VIDEO_WIDTH >> GLYPH64_WIDTH_LOG2)
#define VIDEO_WIDTH_GLYPH128        (VIDEO_WIDTH >> GLYPH128_WIDTH_LOG2)
#define VIDEO_WIDTH_GLYPH256        (VIDEO_WIDTH >> GLYPH256_WIDTH_LOG2)
#define VIDEO_WIDTH_ATTRIBUTE       (VIDEO_WIDTH >> AttributeWidthLog2())
#define VIDEO_WIDTH_ATTRIBUTE_2x4   (VIDEO_WIDTH >> 1)
#define VIDEO_WIDTH_ATTRIBUTE_4x4   (VIDEO_WIDTH >> 2)
#define VIDEO_WIDTH_ATTRIBUTE_4x8   (VIDEO_WIDTH >> 2)
#define VIDEO_WIDTH_ATTRIBUTE_8x8   (VIDEO_WIDTH >> 3)
#define VIDEO_WIDTH_PIXELBYTE_8x1   (VIDEO_WIDTH >> 3) // bit-pixel
#define VIDEO_WIDTH_PIXELBYTE_4x1   (VIDEO_WIDTH >> 2) // quarter-pixel
#define VIDEO_WIDTH_PIXELBYTE_2x1   (VIDEO_WIDTH >> 1) // nibble-pixel
#define VIDEO_WIDTH_PIXELBYTE_1x1   VIDEO_WIDTH        // byte-pixel

// Height of the video framebuffer but in other "units" than pixels
// (framebuffer must contain element of the type)
#define VIDEO_HEIGHT_GLYPH          (VIDEO_HEIGHT >> GLYPH_HEIGHT_LOG2)
#define VIDEO_HEIGHT_GLYPH8         (VIDEO_HEIGHT >> GLYPH8_HEIGHT_LOG2)
#define VIDEO_HEIGHT_GLYPH16        (VIDEO_HEIGHT >> GLYPH16_HEIGHT_LOG2)
#define VIDEO_HEIGHT_GLYPH32        (VIDEO_HEIGHT >> GLYPH32_HEIGHT_LOG2)
#define VIDEO_HEIGHT_GLYPH64        (VIDEO_HEIGHT >> GLYPH64_HEIGHT_LOG2)
#define VIDEO_HEIGHT_GLYPH128       (VIDEO_HEIGHT >> GLYPH128_HEIGHT_LOG2)
#define VIDEO_HEIGHT_GLYPH256       (VIDEO_HEIGHT >> GLYPH256_HEIGHT_LOG2)
#define VIDEO_HEIGHT_ATTRIBUTE      (VIDEO_HEIGHT >> AttributeHeightLog2())
#define VIDEO_HEIGHT_ATTRIBUTE_2x4  (VIDEO_HEIGHT >> 2)
#define VIDEO_HEIGHT_ATTRIBUTE_4x4  (VIDEO_HEIGHT >> 2)
#define VIDEO_HEIGHT_ATTRIBUTE_4x8  (VIDEO_HEIGHT >> 3)
#define VIDEO_HEIGHT_ATTRIBUTE_8x8  (VIDEO_HEIGHT >> 3)
#define VIDEO_HEIGHT_PIXELBYTE      VIDEO_HEIGHT



// Assert that the given pointer indicates a location inside the framebuffer
static inline void VideoAssertInFramebuffer(void *pointer)
{
   assert((uint8_t *)(pointer) >= VIDEO_BUFFER &&
          (uint8_t *)(pointer) < (VIDEO_BUFFER + VIDEO_SIZE));
   // Prevents the compiler to issue a "unused parameter" warning in case
   // `assert()` expands to nothing.
   (void)(pointer);
}

//------------------------------------------------------------------------------
// Access to BitPlanes
//------------------------------------------------------------------------------

// Return a pointer to the start of the n-th plane (zero-indexed)
// (Reminder: the total number of planes is given by VIDEO_MODE'ls low nibble)
static inline uint8_t *VideoPlane(int n)
{
   assert(n >= 0 && n < VideoModeLowNibble());
   return VIDEO_BUFFER + n * (VIDEO_SIZE / VideoModeLowNibble());
}


// Return the start of the n-th plane cast as the given Glyph type.
// Note: Video should be in bitplanes mode and n should be in [0, # of planes[
#define VideoPlaneAs(TYPE_NAME, n) ((TYPE_NAME *)VideoPlane((n)))


// Same as `VideoPlaneOffset`, but no bounds checking.
#define VideoPlaneOffset_(...) \
   UTIL_OVERLOAD(VideoPlaneOffset_, __VA_ARGS__)
   #define VideoPlaneOffset__0_(...)   (VIDEO_SIZE / VideoModeLowNibble())
   static inline int VideoPlaneOffset__1_(int n) {
      assert(n >= 0 && n < VideoModeLowNibble());
      return n * (VIDEO_SIZE / VideoModeLowNibble());
   }

// int VideoPlaneOffset(void);
// int VideoPlaneOffset(int plane_index);
//
// - Without no argument, return the length in bytes of a plane.
// - Otherwise, return the offset from the start of VIDEO_BUFFER where the
//   given (zero-based)index plane starts.
//
// Note on safety and bounds checking:
// - if not in planar mode, return 0.
// - plane will be clamped between [0, max number of plane[
// - Use `VidePlaneOffset_` for the same functionality with no bounds-checking
// - Reminder: total number of planes is given by the VIDEO_MODE's low nibble
#define VideoPlaneOffset(...) \
   UTIL_OVERLOAD(VideoPlaneOffset, __VA_ARGS__)
   #define VideoPlaneOffset_0_(...)   (VIDEO_SIZE / VideoModeLowNibble())
   static inline int VideoPlaneOffset_1_(int n) {
      UtilClampCoordinate(&n, VideoModeLowNibble());
      return VideoPlaneOffset__1_(n);
   }


//------------------------------------------------------------------------------
// Access to Attributes
//------------------------------------------------------------------------------

// Same as `VideoAttributeOffset()`, but assumes video is in attribute mode.
static inline int VideoAttributeOffset_(void)
{
   assert(VideoModeHasAttributes());
#if INT_WIDTH < 32
   return ((int32_t)VIDEO_WIDTH * (int32_t)VIDEO_HEIGHT) >> 3;
#else
   return (VIDEO_WIDTH * VIDEO_HEIGHT) >> 3;
#endif
}

// Return the offset in the VIDEO_BUFFER where the attributes start, or
// if not in attribute mode, return the end of the framebuffer.
// Note: in attribute mode, the return value also corresponds to size in bytes
//       occupied by all the Glyphs.
static inline int VideoAttributeOffset(void)
{
   return VideoModeHasAttributes() ? VideoAttributeOffset_() : VIDEO_SIZE;
}

// Same as function `VideoAttribute`, but no bounds checking.
static inline uint8_t *VideoAttribute_(int x, int y)
{
   assert(x >= 0 && x < (VIDEO_WIDTH  >> AttributeWidthLog2()));
   assert(y >= 0 && y < (VIDEO_HEIGHT >> AttributeHeightLog2()));

   int index = x + y * (VIDEO_WIDTH >> AttributeWidthLog2());
   return VIDEO_BUFFER + VideoAttributeOffset() + (index << AttributeHasTwoBytes());
}

// uint8_t *VideoAttribute(int x, int y);
// Return the Attribute pointer at glyph coordinates (x,y) in the video
// framebuffer.
// - The video framebuffer should be in attribute modes.
// - Bounds checking: if the (x,y) attribute coordinates are outside the bounds
//   of the screen, they will be clamped to the nearest valid edge.
static inline uint8_t *VideoAttribute(int x, int y)
{
   UtilClampCoordinate(&x, VIDEO_WIDTH  >> AttributeWidthLog2());
   UtilClampCoordinate(&y, VIDEO_HEIGHT >> AttributeHeightLog2());
   return VideoAttribute_(x, y);
}

// Same as function `VideoAttributeAtPixel`, but no bounds checking.
static inline uint8_t *VideoAttributeAtPixel_(int x, int y)
{
   assert(x >= 0 && x < VIDEO_WIDTH);
   assert(y >= 0 && y < VIDEO_HEIGHT);
   return VideoAttribute_(x >> AttributeWidthLog2(),
                          y >> AttributeHeightLog2());
}

// Return a pointer to the attribute located under the pixel (x,y). If the pixel
// (x,y) is out of the screen, it will first be clamped to the screen edge.
static inline uint8_t *VideoAttributeAtPixel(int x, int y)
{
   UtilClampCoordinate(&x, VIDEO_WIDTH);
   UtilClampCoordinate(&y, VIDEO_HEIGHT);
   return VideoAttributeAtPixel_(x, y);
}

// void VideoAttributeSetAll();
// void VideoAttributeSetAll(int attribute_byte);
// void VideoAttributeSetAll(int fg, int bg);
// Set all the attributes of the video framebuffer to a given value.
// - no  argument: set to default fg and bg
// - one argument: set to the given byte (Attribute must point to a single byte)
// - two arguments: set to the given fg and bg colors.
#define VideoAttributeSetAll(...) \
   UTIL_OVERLOAD(VideoAttributeSetAll, __VA_ARGS__)
   static inline void VideoAttributeSetAll_1_(int byte) {
      int start = VideoAttributeOffset();
      memset(VIDEO_BUFFER + start, byte, VIDEO_SIZE - start);
   }
   static inline void VideoAttributeSetAll_2_(int fg, int bg) {
      if (AttributeHasTwoBytes()) {
         union {
            uint16_t u8[8];
            uint64_t u64;
         } mem = {.u8 = {fg, bg, fg, bg, fg, bg, fg, bg}};
         int offset = VideoAttributeOffset();
         uint8_t *attributes = VIDEO_BUFFER + offset;
         int end = (VIDEO_SIZE - offset) >> 3;
         for (int i = 0; i < end; i++)
            ((uint64_t *)attributes)[i] = mem.u64;
      } else {
      VideoAttributeSetAll_1_(fg << 4 | bg);
      }
   }
#  define VideoAttributeSetAll_0_() \
      VideoAttributeSetAll_2_(COLOR_DEFAULT_FG, COLOR_DEFAULT_BG)

//------------------------------------------------------------------------------
// Access to Glyphs
//------------------------------------------------------------------------------

// Glyph *VideoGlyph(int x, int y, [plane[=0]]);
// Return the Glyph at glyph coordinates (x,y) in the video framebuffer.
// - The video framebuffer should contain Glyph elements.
// - For glyph bitplanes modes, a third parameter is accepted (defaults to 0)
//   which indicates the bit plane index (zero-indexed).
// - Bounds checking: if the (x,y) glyph coordinates are outside the bounds of
//   the screen, they will be clamped to the nearest valid edge. And likewise
//   the plane will be clamped to [0, max number of planes[
#define VideoGlyph(...) \
   UTIL_OVERLOAD(VideoGlyph, __VA_ARGS__)
// Same as function `VideoGlyph()`, but no bounds checking.
#define VideoGlyph_(...) \
   UTIL_OVERLOAD(VideoGlyph_, __VA_ARGS__)
#  define VideoGlyph__2_(x, y) \
      VideoGlyph__3_((x), (y), 0)
   static inline Glyph *VideoGlyph__3_(int x, int y, int plane) {
      // Coordinates (x,y) must be inside the glyph grid:
      assert(x >= 0 && x < VIDEO_WIDTH_GLYPH);
      assert(y >= 0 && y < VIDEO_HEIGHT_GLYPH);
      int index = x + y  * VIDEO_WIDTH_GLYPH;
      return VideoPlane(plane) + (index << VideoModeDimension());
   }
#  define VideoGlyph_2_(x, y) VideoGlyph_3_((x), (y), 0)
   static inline Glyph *VideoGlyph_3_(int x, int y, int plane) {
      UtilClampCoordinate(&x, VIDEO_WIDTH_GLYPH);
      UtilClampCoordinate(&y, VIDEO_HEIGHT_GLYPH);
      UtilClampCoordinate(&plane, VideoModeLowNibble());
      return VideoGlyph__3_(x, y, plane);
   }

// Glyph8 *VideoGlyph8(int x, int y, [plane[=0]]);
// Return the Glyph8 at glyph coordinates (x,y) in the video framebuffer.
// - The video framebuffer should contain Glyph8 elements.
// - For glyph bitplanes modes, a third parameter is accepted (defaults to 0)
//   which indicates the bit plane index (zero-indexed).
// - Bounds checking: if the (x,y) glyph coordinates are outside the bounds of
//   the screen, they will be clamped to the nearest valid edge. And likewise
//   the plane will be clamped to [0, max number of planes[
#define VideoGlyph8(...) \
   UTIL_OVERLOAD(VideoGlyph8, __VA_ARGS__)
// Same as function `VideoGlyph8()`, but no bounds checking.
#define VideoGlyph8_(...) \
   UTIL_OVERLOAD(VideoGlyph8_, __VA_ARGS__)
#  define VideoGlyph8__2_(x, y) VideoGlyph8__3_((x), (y), 0)
   static inline Glyph8 *VideoGlyph8__3_(int x, int y, int plane) {
      assert(x >= 0 && x < VIDEO_WIDTH_GLYPH8);
      assert(y >= 0 && y < VIDEO_HEIGHT_GLYPH8);
      int offset = x + y * VIDEO_WIDTH_GLYPH8;
      return VideoPlane(plane) + offset;
   }
#  define VideoGlyph8_2_(x, y) VideoGlyph8_3_((x), (y), 0)
   static inline Glyph8 *VideoGlyph8_3_(int x, int y, int plane) {
      UtilClampCoordinate(&x, VIDEO_WIDTH_GLYPH8);
      UtilClampCoordinate(&y, VIDEO_HEIGHT_GLYPH8);
      UtilClampCoordinate(&plane, VideoModeLowNibble());
      return VideoGlyph8__3_(x, y, plane);
   }

// Glyph16 *VideoGlyph16(int x, int y, [plane[=0]]);
// Return the Glyph16 at glyph coordinates (x,y) in the video framebuffer.
// - The video framebuffer should contain Glyph16 elements.
// - For glyph bitplanes modes, a third parameter is accepted (defaults to 0)
//   which indicates the bit plane index (zero-indexed).
// - Bounds checking: if the (x,y) glyph coordinates are outside the bounds of
//   the screen, they will be clamped to the nearest valid edge. And likewise
//   the plane will be clamped to [0, max number of planes[
#define VideoGlyph16(...) \
   UTIL_OVERLOAD(VideoGlyph16, __VA_ARGS__)
// Same as function `VideoGlyph16()`, but no bounds checking.
#define VideoGlyph16_(...) \
   UTIL_OVERLOAD(VideoGlyph16_, __VA_ARGS__)
#  define VideoGlyph16__2_(x, y) VideoGlyph16__3_((x), (y), 0)
   static inline Glyph16 *VideoGlyph16__3_(int x, int y, int plane) {
      assert(x >= 0 && x < VIDEO_WIDTH_GLYPH16);
      assert(y >= 0 && y < VIDEO_HEIGHT_GLYPH16);
      int offset = x + y * VIDEO_WIDTH_GLYPH16;
      return VideoPlaneAs(Glyph16, plane) + offset;
   }
#  define VideoGlyph16_2_(x, y) VideoGlyph16_3_((x), (y), 0)
   static inline Glyph16 *VideoGlyph16_3_(int x, int y, int plane) {
      UtilClampCoordinate(&x, VIDEO_WIDTH_GLYPH16);
      UtilClampCoordinate(&y, VIDEO_HEIGHT_GLYPH16);
      UtilClampCoordinate(&plane, VideoModeLowNibble());
      return VideoGlyph16__3_(x, y, plane);
   }

// Glyph32 *VideoGlyph32(int x, int y, [plane[=0]]);
// Return the Glyph32 at glyph coordinates (x,y) in the video framebuffer.
// - The video framebuffer should contain Glyph32 elements.
// - For glyph bitplanes modes, a third parameter is accepted (defaults to 0)
//   which indicates the bit plane index (zero-indexed).
// - Bounds checking: if the (x,y) glyph coordinates are outside the bounds of
//   the screen, they will be clamped to the nearest valid edge. And likewise
//   the plane will be clamped to [0, max number of planes[
#define VideoGlyph32(...) \
   UTIL_OVERLOAD(VideoGlyph32, __VA_ARGS__)
// Same as function `VideoGlyph32()`, but no bounds checking.
#define VideoGlyph32_(...) \
   UTIL_OVERLOAD(VideoGlyph32_, __VA_ARGS__)
#  define VideoGlyph32__2_(x, y) VideoGlyph32__3_((x), (y), 0)
   static inline Glyph32 *VideoGlyph32__3_(int x, int y, int plane) {
      assert(x >= 0 && x < VIDEO_WIDTH_GLYPH32);
      assert(y >= 0 && y < VIDEO_HEIGHT_GLYPH32);
      int offset = x + y * VIDEO_WIDTH_GLYPH32;
      return VideoPlaneAs(Glyph32, plane) + offset;
   }
#  define VideoGlyph32_2_(x, y) VideoGlyph32_3_((x), (y), 0)
   static inline Glyph32 *VideoGlyph32_3_(int x, int y, int plane) {
      UtilClampCoordinate(&x, VIDEO_WIDTH_GLYPH32);
      UtilClampCoordinate(&y, VIDEO_HEIGHT_GLYPH32);
      UtilClampCoordinate(&plane, VideoModeLowNibble());
      return VideoGlyph32__3_(x, y, plane);
   }

// Glyph64 *VideoGlyph64(int x, int y, [plane[=0]]);
// Return the Glyph64 at glyph coordinates (x,y) in the video framebuffer.
// - The video framebuffer should contain Glyph64 elements.
// - For glyph bitplanes modes, a third parameter is accepted (defaults to 0)
//   which indicates the bit plane index (zero-indexed).
// - Bounds checking: if the (x,y) glyph coordinates are outside the bounds of
//   the screen, they will be clamped to the nearest valid edge. And likewise
//   the plane will be clamped to [0, max number of planes[
#define VideoGlyph64(...) \
   UTIL_OVERLOAD(VideoGlyph64, __VA_ARGS__)
// Same as function `VideoGlyph64()`, but no bounds checking.
#define VideoGlyph64_(...) \
   UTIL_OVERLOAD(VideoGlyph64_, __VA_ARGS__)
#  define VideoGlyph64__2_(x, y) VideoGlyph64__3_((x), (y), 0)
   static inline Glyph64 *VideoGlyph64__3_(int x, int y, int plane) {
      assert(x >= 0 && x < VIDEO_WIDTH_GLYPH64);
      assert(y >= 0 && y < VIDEO_HEIGHT_GLYPH64);
      int offset = x + y * VIDEO_WIDTH_GLYPH64;
      return VideoPlaneAs(Glyph64, plane) + offset;
   }
#  define VideoGlyph64_2_(x, y) VideoGlyph64_3_((x), (y), 0)
   static inline Glyph64 *VideoGlyph64_3_(int x, int y, int plane) {
      UtilClampCoordinate(&x, VIDEO_WIDTH_GLYPH64);
      UtilClampCoordinate(&y, VIDEO_HEIGHT_GLYPH64);
      UtilClampCoordinate(&plane, VideoModeLowNibble());
      return VideoGlyph64__3_(x, y, plane);
   }

// Glyph128 *VideoGlyph128(int x, int y, [plane[=0]]);
// Return the Glyph128 at glyph coordinates (x,y) in the video framebuffer.
// - The video framebuffer should contain Glyph128 elements.
// - For glyph bitplanes modes, a third parameter is accepted (defaults to 0)
//   which indicates the bit plane index (zero-indexed).
// - Bounds checking: if the (x,y) glyph coordinates are outside the bounds of
//   the screen, they will be clamped to the nearest valid edge. And likewise
//   the plane will be clamped to [0, max number of planes[
#define VideoGlyph128(...) \
   UTIL_OVERLOAD(VideoGlyph128, __VA_ARGS__)
// Same as function `VideoGlyph128()`, but no bounds checking.
#define VideoGlyph128_(...) \
   UTIL_OVERLOAD(VideoGlyph128_, __VA_ARGS__)
#  define VideoGlyph128__2_(x, y) VideoGlyph128__3_((x), (y), 0)
   static inline Glyph128 *VideoGlyph128__3_(int x, int y, int plane) {
      assert(x >= 0 && x < VIDEO_WIDTH_GLYPH128);
      assert(y >= 0 && y < VIDEO_HEIGHT_GLYPH128);
      int offset = x + y * VIDEO_WIDTH_GLYPH128;
      return VideoPlaneAs(Glyph128, plane) + offset;
   }
#  define VideoGlyph128_2_(x, y) VideoGlyph128_3_((x), (y), 0)
   static inline Glyph128 *VideoGlyph128_3_(int x, int y, int plane) {
      UtilClampCoordinate(&x, VIDEO_WIDTH_GLYPH128);
      UtilClampCoordinate(&y, VIDEO_HEIGHT_GLYPH128);
      UtilClampCoordinate(&plane, VideoModeLowNibble());
      return VideoGlyph128__3_(x, y, plane);
   }

// Glyph256 *VideoGlyph256(int x, int y, [plane[=0]]);
// Return the Glyph256 at glyph coordinates (x,y) in the video framebuffer.
// - The video framebuffer should contain Glyph256 elements.
// - For glyph bitplanes modes, a third parameter is accepted (defaults to 0)
//   which indicates the bit plane index (zero-indexed).
// - Bounds checking: if the (x,y) glyph coordinates are outside the bounds of
//   the screen, they will be clamped to the nearest valid edge. And likewise
//   the plane will be clamped to [0, max number of planes[
#define VideoGlyph256(...) \
   UTIL_OVERLOAD(VideoGlyph256, __VA_ARGS__)
// Same as function `VideoGlyph256()`, but no bounds checking.
#define VideoGlyph256_(...) \
   UTIL_OVERLOAD(VideoGlyph256_, __VA_ARGS__)
#  define VideoGlyph256__2_(x, y) VideoGlyph256__3_((x), (y), 0)
   static inline Glyph256 *VideoGlyph256__3_(int x, int y, int plane) {
      assert(x >= 0 && x < VIDEO_WIDTH_GLYPH256);
      assert(y >= 0 && y < VIDEO_HEIGHT_GLYPH256);
      int offset = x + y * VIDEO_WIDTH_GLYPH256;
      return VideoPlaneAs(Glyph256, plane) + offset;
   }
#  define VideoGlyph256_2_(x, y) VideoGlyph256_3_((x), (y), 0)
   static inline Glyph256 *VideoGlyph256_3_(int x, int y, int plane) {
      UtilClampCoordinate(&x, VIDEO_WIDTH_GLYPH256);
      UtilClampCoordinate(&y, VIDEO_HEIGHT_GLYPH256);
      UtilClampCoordinate(&plane, VideoModeLowNibble());
      return VideoGlyph256__3_(x, y, plane);
   }


// Clear the glyphs in the Framebuffer (ie set them all to zero)
#define VideoGlyphClear() \
   memset(VIDEO_BUFFER, 0, VideoAttributeOffset())

// TODO, make it: void VideoGlyphSetAll([Glyph<...> glyph]), [int plane = 0]]);
//       it could accept up to two args.
//
// void VideoGlyphSetAll([Glyph glyph, [int plane]])
// Set all the glyphs of the video framebuffer to the given one.
// no argument: "clear" all the glyphs (ie. then set them all to 0).
// one argument: set all the glyphs to the given value.
// two arguments: set all the glyphs of the given plane to the given value
#define VideoGlyphSetAll(...) \
   UTIL_OVERLOAD(VideoGlyphSetAll, __VA_ARGS__)
#  define VideoGlyphSetAll_0_() \
      memset(VIDEO_BUFFER, 0, VideoAttributeOffset())
#  define VideoGlyphSetAll_1_(glyph)        \
      _Generic((glyph)                    , \
         uint8_t:  VideoGlyphSetAll_1_8_  , \
         uint16_t: VideoGlyphSetAll_1_16_ , \
         uint32_t: VideoGlyphSetAll_1_32_ , \
         uint64_t: VideoGlyphSetAll_1_64_ , \
         Glyph128: VideoGlyphSetAll_1_128_, \
         Glyph256: VideoGlyphSetAll_1_256_  \
      )((glyph))
   static inline void VideoGlyphSetAll_1_64_(uint64_t g) {
      int end = VideoAttributeOffset() >> PIXELS_8x8;
      for (int i = 0; i < end; i++)
         VIDEO_GLYPH64[i] = g;
   }
   static inline void VideoGlyphSetAll_1_16_(uint16_t g) {
      union {
         uint16_t u16[4];
         uint64_t u64;
      } mem = {.u16 = {g, g, g, g}};
      VideoGlyphSetAll_1_64_(mem.u64);
   }
   static inline void VideoGlyphSetAll_1_32_(uint32_t g) {
      union {
         uint16_t u32[2];
         uint64_t u64;
      } mem = {.u32 = {g, g}};
      VideoGlyphSetAll_1_64_(mem.u64);
   }
   static inline void VideoGlyphSetAll_1_8_(uint8_t g) {
      // Would calling VideoGlyphSetAll_1_64_ be faster or slower than memset?
      memset(VIDEO_BUFFER, g, VideoAttributeOffset());
   }
   static inline void VideoGlyphSetAll_1_128_(Glyph128 g) {
      // We're not 100% sure to have an exact number of Glyph128
      int end = VideoAttributeOffset() >> PIXELS_8x16;
      for (int i = 0; i < end; i++)
         VIDEO_GLYPH128[i] = g;
   }
   static inline void VideoGlyphSetAll_1_256_(Glyph256 g) {
      // We're not 100% sure to have an exact number of Glyph256
      int end = VideoAttributeOffset() >> PIXELS_16x16;
      for (int i = 0; i < end; i++)
         VIDEO_GLYPH256[i] = g;
   }
#  define VideoGlyphSetAll_2_(glyph, plane) \
      _Generic((glyph)                    , \
         uint8_t:  VideoGlyphSetAll_2_8_  , \
         uint16_t: VideoGlyphSetAll_2_16_ , \
         uint32_t: VideoGlyphSetAll_2_32_ , \
         uint64_t: VideoGlyphSetAll_2_64_ , \
         Glyph128: VideoGlyphSetAll_2_128_, \
         Glyph256: VideoGlyphSetAll_2_256_  \
      )((glyph), (plane))
   //TODO: implement VideoGlyphSetAll_2_*_



//------------------------------------------------------------------------------
// Rendering the Framebuffer
//------------------------------------------------------------------------------

// Render the video framebuffer on screen.
// Return value may be non-zero on error (in which case VIDEO_RENDER_ERRRORS
// counter will also be increased by one).
int VideoRender(void);


#endif //include guard
