#ifndef KONPU_VIDEO_FRAMEBUFFER_H
#define KONPU_VIDEO_FRAMEBUFFER_H
#include "util.h"
#include "attribute.h"
#include "glyph.h"
#include "color.h"

//------------------------------------------------------------------------------
// The Video Framebuffer
//------------------------------------------------------------------------------

// The video framebuffer
#define VIDEO_FRAMEBUFFER (KonpuMemory + VIDEO_FRAMEBUFFER_ADDRESS)

// The video framebuffer, but cast as a pointer to the given type.
// The intent of this cast should be to access Glyphs or Tiles video elements
// present in the framebuffer in accordance to the VIDEO_MODE.
#define VIDEO_FRAMEBUFFER_AS(TYPE) ((TYPE *)VIDEO_FRAMEBUFFER)

// Assert that the given pointer indicates a location inside the framebuffer
static inline void VideoAssertInFramebuffer(void *pointer)
{
   assert((uint8_t *)(pointer) >= VIDEO_FRAMEBUFFER &&
          (uint8_t *)(pointer) < (VIDEO_FRAMEBUFFER + VIDEO_SIZE));
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
   return VIDEO_FRAMEBUFFER + n * (VIDEO_SIZE / VideoModeLowNibble());
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
// - Otherwise, return the offset from the start of VIDEO_FRAMEBUFFER where the
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

// Return the offset in the VIDEO_FRAMEBUFFER where the attributes start, or
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
   assert(x >= 0 && x < (VIDEO_WIDTH >> AttributeWidthLog2()));
   assert(y >= 0 && y < (VIDEO_HEIGHT >> AttributeHeightLog2()));

   int index = x + y * (VIDEO_WIDTH >> AttributeWidthLog2());
   return VIDEO_FRAMEBUFFER + VideoAttributeOffset() + (index << AttributeHasTwoBytes());
}

// uint8_t *VideoAttribute(int x, int y);
// Return the Attribute pointer at glyph coordinates (x,y) in the video
// framebuffer.
// - The video framebuffer should be in attribute modes.
// - Bounds checking: if the (x,y) attribute coordinates are outside the bounds
//   of the screen, they will be clamped to the nearest valid edge.
static inline uint8_t *VideoAttribute(int x, int y)
{
   UtilClampCoordinate(&x, VIDEO_WIDTH >> AttributeWidthLog2());
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
// - no arguments: set to default fg and bg
// - one argument: set to the given byte (Attribute must point to a single byte)
// - two arguments: set to the given fg and bg colors.
#define VideoAttributeSetAll(...) \
   UTIL_OVERLOAD(VideoAttributeSetAll, __VA_ARGS__)
static inline void VideoAttributeSetAll_1_(int byte)
{
   int start = VideoAttributeOffset();
   memset(VIDEO_FRAMEBUFFER + start, byte, VIDEO_SIZE - start);
}
static inline void VideoAttributeSetAll_2_(int fg, int bg)
{
   if (AttributeHasTwoBytes()) {
      union {
         uint16_t u8[8];
         uint64_t u64;
      } mem = {.u8 = {fg, bg, fg, bg, fg, bg, fg, bg}};
      int offset = VideoAttributeOffset();
      uint8_t *attributes = VIDEO_FRAMEBUFFER + offset;
      int end = (VIDEO_SIZE - offset) >> 3;
      for (int i = 0; i < end; i++)
         ((uint64_t *)attributes)[i] = mem.u64;
   } else {
      VideoAttributeSetAll_1_(fg << 4 | bg);
   }
}
#define VideoAttributeSetAll_0_() \
   VideoAttributeSetAll_2_(COLOR_DEFAULT_FG, COLOR_DEFAULT_BG)

//------------------------------------------------------------------------------
// Access to Glyphs
//------------------------------------------------------------------------------

// In glyph modes, return the log2 of a glyph's size in bytes.
// Note: This is same as element descriptor in the video mode.
#define VideoGlyphLog2Sizeof() ((VIDEO_MODE >> 4) & 7)

// In glyph modes, return the log2 of a glyph's pixel width.
// Thus, the width of a glyph is: 1 << VideoGlyphLog2Width().
#define VideoGlyphLog2Width() (((VIDEO_MODE >> 4 & 7) + 3) >> 1)
//                      VIDEO_MODE's element descriptor: |0|1|2|3|4|5|
//                      log2(glyph width):               |1|2|2|3|3|4|
//                      thus, we apply (x+3)/2 to the element descriptor

// In glyph modes, return the log2 of a glyph's pixel height.
// Thus, the height of a glyph is: 1 << VideoGlyphLog2Height().
#define VideoGlyphLog2Height() (2 + ((VIDEO_MODE >> 4 & 7) >> 1))
//                      VIDEO_MODE's element descriptor: |0|1|2|3|4|5|
//                      log2(glyph height):              |2|2|3|3|4|4|
//                      thus, we apply 2+x/2 to the element descriptor
// This also works and can be one op less:
// #define VideoGlyphLog2Height()  (2 + (VIDEO_MODE >> 5 & 3))
// However, it's perhaps best to have the same operations than in the element
// descriptor and Log2Width, so the compiler sees similarites and optimize?

// Glyph *VideoGlyph(int x, int y, [plane[=0]]);
// Return the Glyph at glyph coordinates (x,y) in the video framebuffer.
// - The video framebuffer should contain Glyph elements.
// - For glyph bitplanes modes, a third parameter is accepted (defaults to 0)
//   which indicates the bit plane index (zero-indexed).
// - Bounds checking: if the (x,y) glyph coordinates are outside the bounds of
//   the screen, they will be clamped to the nearest valid edge. And likewise
//   the plane will be clamped to [0, max numbeof of planes[
#define VideoGlyph(...) UTIL_OVERLOAD(VideoGlyph, __VA_ARGS__)
// Same as function `VideoGlyph()`, but no bounds checking.
#define VideoGlyph_(...) UTIL_OVERLOAD(VideoGlyph_, __VA_ARGS__)
#define VideoGlyph__2_(x, y) VideoGlyph__3_((x), (y), 0)
static inline Glyph *VideoGlyph__3_(int x, int y, int plane)
{
   // Coordinates (x,y) must be inside the glyph grid:
   assert(x >= 0 && x < (VIDEO_WIDTH >> VideoGlyphLog2Width()));
   assert(y >= 0 && y < (VIDEO_HEIGHT >> VideoGlyphLog2Height()));
   int index = x + y * (VIDEO_WIDTH >> VideoGlyphLog2Width());
   return VideoPlane(plane) + (index << VideoModeElementDescriptor());
}
#define VideoGlyph_2_(x, y) VideoGlyph_3_((x), (y), 0)
static inline Glyph *VideoGlyph_3_(int x, int y, int plane)
{
   UtilClampCoordinate(&x, VIDEO_WIDTH >> VideoGlyphLog2Width());
   UtilClampCoordinate(&y, VIDEO_HEIGHT >> VideoGlyphLog2Height());
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
//   the plane will be clamped to [0, max numbeof of planes[
#define VideoGlyph8(...) UTIL_OVERLOAD(VideoGlyph8, __VA_ARGS__)
// Same as function `VideoGlyph8()`, but no bounds checking.
#define VideoGlyph8_(...) UTIL_OVERLOAD(VideoGlyph8_, __VA_ARGS__)
#define VideoGlyph8__2_(x, y) VideoGlyph8__3_((x), (y), 0)
static inline Glyph8 *VideoGlyph8__3_(int x, int y, int plane)
{
   assert(x >= 0 && x < (VIDEO_WIDTH  >> GLYPH8_WIDTH_LOG2));
   assert(y >= 0 && y < (VIDEO_HEIGHT >> GLYPH8_HEIGHT_LOG2));
   int offset = x + y * (VIDEO_WIDTH  >> GLYPH8_WIDTH_LOG2);
   return VideoPlane(plane) + offset;
}
#define VideoGlyph8_2_(x, y) VideoGlyph8_3_((x), (y), 0)
static inline Glyph8 *VideoGlyph8_3_(int x, int y, int plane)
{
   UtilClampCoordinate(&x, VIDEO_WIDTH  >> GLYPH8_WIDTH_LOG2);
   UtilClampCoordinate(&y, VIDEO_HEIGHT >> GLYPH8_HEIGHT_LOG2);
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
//   the plane will be clamped to [0, max numbeof of planes[
#define VideoGlyph16(...) UTIL_OVERLOAD(VideoGlyph16, __VA_ARGS__)
// Same as function `VideoGlyph16()`, but no bounds checking.
#define VideoGlyph16_(...) UTIL_OVERLOAD(VideoGlyph16_, __VA_ARGS__)
#define VideoGlyph16__2_(x, y) VideoGlyph16__3_((x), (y), 0)
static inline Glyph16 *VideoGlyph16__3_(int x, int y, int plane)
{
   assert(x >= 0 && x < (VIDEO_WIDTH >> GLYPH16_WIDTH_LOG2));
   assert(y >= 0 && y < (VIDEO_HEIGHT >> GLYPH16_HEIGHT_LOG2));
   int offset = x + y * (VIDEO_WIDTH >> GLYPH16_WIDTH_LOG2);
   return VideoPlaneAs(Glyph16, plane) + offset;
}
#define VideoGlyph16_2_(x, y) VideoGlyph16_3_((x), (y), 0)
static inline Glyph16 *VideoGlyph16_3_(int x, int y, int plane)
{
   UtilClampCoordinate(&x, VIDEO_WIDTH >> GLYPH16_WIDTH_LOG2);
   UtilClampCoordinate(&y, VIDEO_HEIGHT >> GLYPH16_HEIGHT_LOG2);
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
//   the plane will be clamped to [0, max numbeof of planes[
#define VideoGlyph32(...) UTIL_OVERLOAD(VideoGlyph32, __VA_ARGS__)
// Same as function `VideoGlyph32()`, but no bounds checking.
#define VideoGlyph32_(...) UTIL_OVERLOAD(VideoGlyph32_, __VA_ARGS__)
#define VideoGlyph32__2_(x, y) VideoGlyph32__3_((x), (y), 0)
static inline Glyph32 *VideoGlyph32__3_(int x, int y, int plane)
{
   assert(x >= 0 && x < (VIDEO_WIDTH >> GLYPH32_WIDTH_LOG2));
   assert(y >= 0 && y < (VIDEO_HEIGHT >> GLYPH32_HEIGHT_LOG2));
   int offset = x + y * (VIDEO_WIDTH >> GLYPH32_WIDTH_LOG2);
   return VideoPlaneAs(Glyph32, plane) + offset;
}
#define VideoGlyph32_2_(x, y) VideoGlyph32_3_((x), (y), 0)
static inline Glyph32 *VideoGlyph32_3_(int x, int y, int plane)
{
   UtilClampCoordinate(&x, VIDEO_WIDTH >> GLYPH32_WIDTH_LOG2);
   UtilClampCoordinate(&y, VIDEO_HEIGHT >> GLYPH32_HEIGHT_LOG2);
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
//   the plane will be clamped to [0, max numbeof of planes[
#define VideoGlyph64(...) UTIL_OVERLOAD(VideoGlyph64, __VA_ARGS__)
// Same as function `VideoGlyph64()`, but no bounds checking.
#define VideoGlyph64_(...) UTIL_OVERLOAD(VideoGlyph64_, __VA_ARGS__)
#define VideoGlyph64__2_(x, y) VideoGlyph64__3_((x), (y), 0)
static inline Glyph64 *VideoGlyph64__3_(int x, int y, int plane)
{
   assert(x >= 0 && x < (VIDEO_WIDTH >> GLYPH64_WIDTH_LOG2));
   assert(y >= 0 && y < (VIDEO_HEIGHT >> GLYPH64_HEIGHT_LOG2));
   int offset = x + y * (VIDEO_WIDTH >> GLYPH64_WIDTH_LOG2);
   return VideoPlaneAs(Glyph64, plane) + offset;
}
#define VideoGlyph64_2_(x, y) VideoGlyph64_3_((x), (y), 0)
static inline Glyph64 *VideoGlyph64_3_(int x, int y, int plane)
{
   UtilClampCoordinate(&x, VIDEO_WIDTH >> GLYPH64_WIDTH_LOG2);
   UtilClampCoordinate(&y, VIDEO_HEIGHT >> GLYPH64_HEIGHT_LOG2);
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
//   the plane will be clamped to [0, max numbeof of planes[
#define VideoGlyph128(...) UTIL_OVERLOAD(VideoGlyph128, __VA_ARGS__)
// Same as function `VideoGlyph128()`, but no bounds checking.
#define VideoGlyph128_(...) UTIL_OVERLOAD(VideoGlyph128_, __VA_ARGS__)
#define VideoGlyph128__2_(x, y) VideoGlyph128__3_((x), (y), 0)
static inline Glyph128 *VideoGlyph128__3_(int x, int y, int plane)
{
   assert(x >= 0 && x < (VIDEO_WIDTH >> GLYPH128_WIDTH_LOG2));
   assert(y >= 0 && y < (VIDEO_HEIGHT >> GLYPH128_HEIGHT_LOG2));
   int offset = x + y * (VIDEO_WIDTH >> GLYPH128_WIDTH_LOG2);
   return VideoPlaneAs(Glyph128, plane) + offset;
}
#define VideoGlyph128_2_(x, y) VideoGlyph128_3_((x), (y), 0)
static inline Glyph128 *VideoGlyph128_3_(int x, int y, int plane)
{
   UtilClampCoordinate(&x, VIDEO_WIDTH >> GLYPH128_WIDTH_LOG2);
   UtilClampCoordinate(&y, VIDEO_HEIGHT >> GLYPH128_HEIGHT_LOG2);
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
//   the plane will be clamped to [0, max numbeof of planes[
#define VideoGlyph256(...) UTIL_OVERLOAD(VideoGlyph256, __VA_ARGS__)
// Same as function `VideoGlyph256()`, but no bounds checking.
#define VideoGlyph256_(...) UTIL_OVERLOAD(VideoGlyph256_, __VA_ARGS__)
#define VideoGlyph256__2_(x, y) VideoGlyph256__3_((x), (y), 0)
static inline Glyph256 *VideoGlyph256__3_(int x, int y, int plane)
{
   assert(x >= 0 && x < (VIDEO_WIDTH >> GLYPH256_WIDTH_LOG2));
   assert(y >= 0 && y < (VIDEO_HEIGHT >> GLYPH256_HEIGHT_LOG2));
   int offset = x + y * (VIDEO_WIDTH >> GLYPH256_WIDTH_LOG2);
   return VideoPlaneAs(Glyph256, plane) + offset;
}
#define VideoGlyph256_2_(x, y) VideoGlyph256_3_((x), (y), 0)
static inline Glyph256 *VideoGlyph256_3_(int x, int y, int plane)
{
   UtilClampCoordinate(&x, VIDEO_WIDTH >> GLYPH256_WIDTH_LOG2);
   UtilClampCoordinate(&y, VIDEO_HEIGHT >> GLYPH256_HEIGHT_LOG2);
   UtilClampCoordinate(&plane, VideoModeLowNibble());
   return VideoGlyph256__3_(x, y, plane);
}

// Clear the glyphs in the Framebuffer (ie set them all to zero)
#define VideoGlyphClear() \
   memset(VIDEO_FRAMEBUFFER, 0, VideoAttributeOffset());

// TODO, make it: void VideoGlyphSetAll([Glyph<...> glyph]), [int plane = 0]]);
//       it could accept up to two args.
//
// void VideoGlyphSetAll([Glyph glyph)
// Set all the glyphs of the video framebuffer to the given one.
// If no argument is given, "clear" all the glyphs (ie. then set them all to 0).
#define VideoGlyphSetAll(...) \
   UTIL_OVERLOAD(VideoGlyphSetAll, __VA_ARGS__)
#define VideoGlyphSetAll_0_() \
   memset(VIDEO_FRAMEBUFFER, 0, VideoAttributeOffset())
#define VideoGlyphSetAll_1_(glyph)        \
   _Generic((glyph),                      \
       uint8_t: VideoGlyphSetAll_1_8_,    \
       uint16_t: VideoGlyphSetAll_1_16_,  \
       uint32_t: VideoGlyphSetAll_1_32_,  \
       uint64_t: VideoGlyphSetAll_1_64_,  \
       Glyph128: VideoGlyphSetAll_1_128_, \
       Glyph256: VideoGlyphSetAll_1_256_)((glyph))
static inline void VideoGlyphSetAll_1_64_(uint64_t g)
{
   int end = VideoAttributeOffset() >> PIXELS_8x8;
   for (int i = 0; i < end; i++)
      VIDEO_FRAMEBUFFER_AS(uint64_t)[i] = g;
}
static inline void VideoGlyphSetAll_1_16_(uint16_t g)
{
   union {
      uint16_t u16[4];
      uint64_t u64;
   } mem = {.u16 = {g, g, g, g}};
   VideoGlyphSetAll_1_64_(mem.u64);
}
static inline void VideoGlyphSetAll_1_32_(uint32_t g)
{
   union {
      uint16_t u32[2];
      uint64_t u64;
   } mem = {.u32 = {g, g}};
   VideoGlyphSetAll_1_64_(mem.u64);
}
static inline void VideoGlyphSetAll_1_8_(uint8_t g)
{
   // Would calling VideoGlyphSetAll_1_64_ be faster or slower than memset?
   memset(VIDEO_FRAMEBUFFER, g, VideoAttributeOffset());
}
static inline void VideoGlyphSetAll_1_128_(Glyph128 g)
{
   // We're not 100% sure to have an exact number of Glyph128
   int end = VideoAttributeOffset() >> PIXELS_8x16;
   for (int i = 0; i < end; i++)
      VIDEO_FRAMEBUFFER_AS(Glyph128)[i] = g;
}
static inline void VideoGlyphSetAll_1_256_(Glyph256 g)
{
   // We're not 100% sure to have an exact number of Glyph256
   int end = VideoAttributeOffset() >> PIXELS_16x16;
   for (int i = 0; i < end; i++)
      VIDEO_FRAMEBUFFER_AS(Glyph256)[i] = g;
}

//------------------------------------------------------------------------------
// Tile Modes (TODO)
//------------------------------------------------------------------------------

// Tile *VideoTile(int x, int y, [int plane[=0]]);

// VideoTileSetAll(...)


//------------------------------------------------------------------------------
// Access to Pixels bytes (in pixel modes)
//------------------------------------------------------------------------------

#ifdef KONPU_OPTION_OPTIMIZE_VIDEO_MODE
// Same as function `VideoGetPixel`, but no bounds checking.
#define VideoGetPixel_(x, y) VideoGetPixel_inline_((x), (y))
// Same as function `VideoSetPixel`, but no bounds checking.
#define VideoSetPixel_(x, y, color) VideoSetPixel_inline_((x), (y), (color))
#else
// Same as function `VideoGetPixel`, but no bounds checking.
int VideoGetPixel_(int x, int y);
// Same as function `VideoSetPixel`, but no bounds checking.
void VideoSetPixel_(int x, int y, int color);
#endif
// Use `VideoGetPixel` instead.
static C_HINT_ALWAYS_INLINE
int VideoGetPixel_inline_(int x, int y)
{
   // Use the safe function for proper bounds checking.
   assert(x >= 0 && x < VIDEO_WIDTH);
   assert(y >= 0 && y < VIDEO_HEIGHT);

   // How to react on an error condition (presumably this can only happen if the
   // user has somehow inputed some invalid video mode with a poke).
#ifndef KONPU_COMPILE_OPTION_USER_MUST_NOT_POKE_AROUND
#  define VIDEO_ERROR()  return 0
#else
#  define VIDEO_ERROR()  unreachable()
#endif

   enum VideoElementPixelSize elem = VideoModeElementDescriptor();
   int low_nibble = VideoModeLowNibble();

   // Glyph:
   //
   // Elem. Descript| where is the Glyph in
   //---------------|-----------------------------------------------------
   // 0 PIXELS_2x4  | FRAMEBUFFER_AS(uint8_t)[x/2 + y/4 * VIDEO_WIDTH/4]
   // 1 PIXELS_4x4  | FRAMEBUFFER_AS(uint16_t)[x/4 + y/4 * VIDEO_WIDTH/4]
   // 2 PIXELS_4x8  | FRAMEBUFFER_AS(uint32_t)[x/4 + y/8 * VIDEO_WIDTH/8]
   // 3 PIXELS_8x8  | FRAMEBUFFER_AS(uint64_t)[x/8 + y/8 * VIDEO_WIDTH/8]
   // 4 PIXELS_8x16 | FRAMEBUFFER_AS(Glyph128)[x/8 + y/16 *VIDEO_WIDTH/16]
   // 5 PIXELS_16x16| FRAMEBUFFER_AS(Glyph256)[x/16 +y/16 *VIDEO_WIDTH/16]

   // 1       2x4     8
   // 2       4x4    16
   //

   // - chunky-pixels or just 1 bitplane:
   //
   // low| chunk size  |  pixel is at this byte  | chunk index for the
   // nib|  (aka bpp)  |  in the framebuffer     | pixel in that byte
   // ---|-------------|-------------------------|--------------------
   //  1 | 1 (bit)     | x / 8 + y * VIDEO_WIDTH |   x % 8
   //  9 | 2 (quarter) | x / 4 + y * VIDEO_WIDTH |   x % 4
   // 10 | 4 (nibble)  | x / 2 + y * VIDEO_WIDTH |   x % 2
   // 11 | 8 (byte)    |   x   + y * VIDEO_WIDTH |     0
   //
   // - several bitplanes:
   // access is same as 1 bit, but we have to iterate through every plane and
   // reconstruct the pixel color bit by bit. (hopefully loops are unrolled)

   // Attribute modes
   if (VideoModeHasAttributes())
   {
#if 0
#define ls elem // same as VideoGlyphLog2Sizeof();
         int lw = (ls + 3) >> 1;  // same as VideoGlyphLog2Width();
         int lh = 2 + (ls >> 1);  // same as VideoGlyphLog2Height();

         // in the glyph grid, the glyph over pixel (x,y) would be at
         // coordinates (x/w, y/h). The glyph grid is (VIDEO_GLYPH/w) wide,
         int offset = (x>>lw) + (y>>lh) * (VIDEO_GLYPH>>lw);

         // ... and the bit for the pixel would be at coordinates (w%w, y%h) in
         // that glyph, ie that is bit at index: bit = x%w + y%h * w,
         // transforming the modulo because of power of 2: x%w = x & (w-1)
         int bit =  (x & ((1<<lw) - 1))         // <--   x%w
                 + ((y & ((1<<lh) - 1)) >> lw); // <-- + y%h * w


         int px = BitsBitAt(VIDEO_FRAMEBUFFER + (offset << ls), )
#endif

      // Look the Attribute and Glyph pixel under the given pixel:
      // Elem   | Glyph corresponding to pixel (x,y)  | Bit for the pixel|
      // Descrip|                                     | in that Glyph    |
      //--------|-------------------------------------|------------------|
      // 0 2x4  | V<uint8_t >[x/2 + y/4 * V_WIDTH/2]  | x%2 + 2*y%4      |
      // 1 4x4  | V<uint16_t>[x/4 + y/4 * V_WIDTH/4]  | x%4 + 4*y%4      |
      // 2 4x8  | V<uint32_t>[x/4 + y/8 * V_WIDTH/8]  | x%4 + 4*y%8      |
      // 3 8x8  | V<uint64_t>[x/8 + y/8 * V_WIDTH/8]  | x%8 + 8*y%8      |
      // 4 8x16 | V<Glyph128>[x/8 + y/16 * V_WIDTH/16]| x%8 + 8*y%16     |
      // 5 16x16| V<Glyph256>[x/16 +y/16 * V_WIDTH/16]| x%16 + 16*y%16   |

      // Look the Attribute and Glyph pixel under the given pixel:
      // Elem   | Glyph corresponding to pixel (x,y)  | Bit for the pixel|
      // Descrip|                                     | in that Glyph    |
      //--------|-------------------------------------|------------------|
      // 0 2x4  | V<uint8_t >[x/2 + y/4 * V_WIDTH/2]  | x%2 , y%4      |
      // 1 4x4  | V<uint16_t>[x/4 + y/4 * V_WIDTH/4]  | x%4 , y%4      |
      // 2 4x8  | V<uint32_t>[x/4 + y/8 * V_WIDTH/8]  | x%4 , y%8      |
      // 3 8x8  | V<uint64_t>[x/8 + y/8 * V_WIDTH/8]  | x%8 , y%8      |
      // 4 8x16 | V<Glyph128>[x/8 + y/16 * V_WIDTH/16]| x%8 , y%16     |
      // 5 16x16| V<Glyph256>[x/16 +y/16 * V_WIDTH/16]| x%16, y%16     |
      // 7 8x1  | V[x/8 + y * V_WIDTH/8]

      // int px = GlyphPixelAt(VideoGlyph_(x,y), x%glyph_width, y%glyph_height);

      int px;          // the bit pixel
      int w;           // width of the glyph grid
      int h;           // height of the glyph grid
      int attr_offset; // offset for the start of the attributes
      switch (elem) {
         case PIXELS_2x4:
            w = VIDEO_WIDTH  >> 1;
            h = VIDEO_HEIGHT >> 2;
            attr_offset = (w * h) << PIXELS_2x4;
            px = GlyphPixelAt_8_(VIDEO_FRAMEBUFFER
                  [(x >> 1) + (y >> 2) * w], x & 1, x & 3);
            break;
         case PIXELS_4x4:
            w = VIDEO_WIDTH  >> 2;
            h = VIDEO_HEIGHT >> 2;
            attr_offset = (w * h) << PIXELS_4x4;
            px = GlyphPixelAt_16_(VIDEO_FRAMEBUFFER_AS(Glyph16)
                  [(x >> 2) + (y >> 2) * w], x & 3, x & 3);
            break;
         case PIXELS_4x8:
            w = VIDEO_WIDTH  >> 2;
            h = VIDEO_HEIGHT >> 3;
            attr_offset = (w * h) << PIXELS_4x8;
            px = GlyphPixelAt_32_(VIDEO_FRAMEBUFFER_AS(Glyph32)
                  [(x >> 2) + (y >> 3) * w], x & 3, y & 7);
            break;
         case PIXELS_8x8:
            w = VIDEO_WIDTH  >> 3;
            h = VIDEO_HEIGHT >> 3;
            attr_offset = (w * h) << PIXELS_8x8;
            px = GlyphPixelAt_64_(VIDEO_FRAMEBUFFER_AS(Glyph64)
                  [(x >> 3) + (y >> 3) * w], x & 7, y & 7);
            break;
         case PIXELS_8x16:
            w = VIDEO_WIDTH  >> 3;
            h = VIDEO_HEIGHT >> 4;
            attr_offset = (w * h) << PIXELS_8x16;
            px = GlyphPixelAt_128_(VIDEO_FRAMEBUFFER_AS(Glyph128)
                  [(x >> 3) + (y >> 4) * w], x & 7, y & 15);
            break;
         case PIXELS_16x16:
            w = VIDEO_WIDTH  >> 4;
            h = VIDEO_HEIGHT >> 4;
            attr_offset = (w * h) << PIXELS_16x16;
            px = GlyphPixelAt_256_(VIDEO_FRAMEBUFFER_AS(Glyph256)
                  [(x >> 4) + (y >> 4) * w], x & 15, y & 15);
            break;
         case PIXELS:
            w = VIDEO_WIDTH >> 3;
            h = VIDEO_HEIGHT;
            attr_offset = w * h;
            px = BitsBitAt(VIDEO_FRAMEBUFFER[(x >> 3) + y * w], x & 7);
            break;
         default:
            VIDEO_ERROR();
      }

      // Look for the attribute under the pixel
      w = AttributeWidthLog2();
      x >>= w;
      y >>= AttributeHeightLog2();
      uint8_t *attr = VIDEO_FRAMEBUFFER + attr_offset
                    + ((x + y * (VIDEO_WIDTH >> w)) << AttributeHasTwoBytes());

      // Return its fg or bg color depending on whether px is on or off.
      return (px) ? AttributeGetForeground(attr)
                  : AttributeGetBackground(attr);
   }

   // Pixel modes:
   //
   // - chunky-pixels or just 1 bitplane:
   //
   // low| chunk size  |  pixel is at this byte  | chunk index for the
   // nib|  (aka bpp)  |  in the framebuffer     | pixel in that byte
   // ---|-------------|-------------------------|--------------------
   //  1 | 1 (bit)     | x / 8 + y * VIDEO_WIDTH |   x % 8
   //  9 | 2 (quarter) | x / 4 + y * VIDEO_WIDTH |   x % 4
   // 10 | 4 (nibble)  | x / 2 + y * VIDEO_WIDTH |   x % 2
   // 11 | 8 (byte)    |   x   + y * VIDEO_WIDTH |     0
   //
   // - several bitplanes:
   // access is same as 1 bit, but we have to iterate through every plane and
   // reconstruct the pixel color bit by bit. (hopefully loops are unrolled)
   if (elem == PIXELS) {
      uint8_t *ptr = VIDEO_FRAMEBUFFER + y * VIDEO_WIDTH;
      switch (low_nibble) {
         // Single-plane bit Pixels or Chunk Pixels
         case  1: return BitsBitAt(ptr[x >> 3], x & 7);
         case  9: return BitsQuarterAt(ptr[x >> 2], x & 3);
         case 10: return BitsNibbleAt(ptr[x >> 1], x & 1);
         case 11: return ptr[x];

         // Several bitplanes
         case  2: // fallthrough
         case  3: // fallthrough
         case  4: // fallthrough
         case  5: // fallthrough
         case  6: // fallthrough
#if (VIDEO_SIZE_FACTOR_ % 7 == 0)
         case  7: // fallthrough
#endif
         case  8: ;
            int color = 0;
            int plane_size = VIDEO_SIZE / low_nibble;
            for (int plane = 0; plane < low_nibble; plane++) {
               color |= BitsBitAt(ptr[x >> 3], x & 7) << plane;
               ptr += plane_size;
            }
            return color;

         default: VIDEO_ERROR();
      }
   }

   // Glyph bitplanes / Tile modes:
   switch (low_nibble) {
      // One single glyph plane
      case  1: return 0; // TODO

      // Several glyph bit planes (construct glyph)
      case  2: // fallthrough
      case  3: // fallthrough
      case  4: // fallthrough
      case  5: // fallthrough
      case  6: // fallthrough
#if (VIDEO_SIZE_FACTOR_ % 7 == 0)
      case  7: // fallthrough
#endif
      case  8: ;
         return 0; // TODO

      // Tiles modes:
      case  9: return 0; // TODO
      case 10: return 0; // TODO
      case 11: return 0; // TODO

      default: VIDEO_ERROR();
   }

#undef VIDEO_ERROR
}

// Internal only - Use or VideoSetPixel [or VideoSetPixel_] instead.
static inline void VideoSetPixel_inline_(int x, int y, int color)
{
   (void)x;
   (void)y;
   (void)color; /* TODO*/
}

// Return the color of the pixel (x,y).
// if (x,y) is out of the screen, return the default background color instead.
static inline int VideoGetPixel(int x, int y)
{
   return (x < 0 || x >= VIDEO_WIDTH || y < 0 || y >= VIDEO_HEIGHT) ? COLOR_DEFAULT_BG : VideoGetPixel_(x, y);
   // We could return the border color? It could make sense, however the
   // border color is always in 256 color mode, but the framebuffer isn't.
}

// Set the color of the pixel (x,y).
// If (x,y) is out of the screen, do nothing.
// TODO: update the description:
// In attribute modes other than "bg256", it colors the foreground attribute at
// the (x,y) pixel coordinates with the given color
// In fg256 attribute mode, it colors ththis could color the foreground attribute with this color
static inline void VideoSetPixel(int x, int y, int color)
{
   if (x < 0 || x >= VIDEO_WIDTH || y < 0 || y >= VIDEO_HEIGHT)
      return;
   VideoSetPixel_(x, y, color);
}

//------------------------------------------------------------------------------
// Rendering the Framebuffer
//------------------------------------------------------------------------------

// The number of errors when rendering the video framebuffer on screen.
// If an error occurs, the `VideoRender?? function *may* increase this counter.
// (Note: an error at every frame at 60FPS takes >2.2 years to overflow it)
#define VIDEO_RENDER_ERRORS \
   (*(const uint32_t *)(KonpuMemory + VIDEO_RENDER_ERRORS_ADDRESS))

// Render the video framebuffer on screen.
// Return value may be non-zero on error (in which case VIDEO_RENDER_ERRRORS
// counter will also be increased by one).
int VideoRender(void);

// Render the video framebuffer into a memory buffer. The graphics are rendered
// in ARGB form as 8bits-gamma-encoded sRGB color + the given alpha component.
//
static inline void VideoRenderToARGB(uint32_t *frame_out, int alpha)
{
   const uint32_t A = (uint32_t)alpha << 24;
   const uint8_t *palette = ColorPalette();

   for (int y = 0; y < VIDEO_HEIGHT; y++) {
      for (int x = 0; x < VIDEO_WIDTH; x++) {
         const int color = VideoGetPixel_(x, y);
         const uint8_t *rgb = COLOR_RGB((palette != NULL) ? palette[color] : color);
         *frame_out++ = A | (rgb[0] << 16) | (rgb[1] << 8) | rgb[2];
      }
   }
}
/* TODO: uint32_t??? This invites endianness issues?
static inline void VideoRenderToARGB_(void *buffer, int alpha)
{
   uint8_t *frame = buffer;
   const int A = (uint32_t)alpha << 24;
   const uint8_t *palette = ColorPalette();

   for (int y = 0; y < VIDEO_HEIGHT; y++) {
      for (int x = 0; x < VIDEO_WIDTH; x++) {
         const int color = VideoGetPixel_(x,y);
         const uint8_t *rgb = COLOR_RGB((palette != NULL) ? palette[color] : color);
         *frame = A;
         memcpy(frame + 1, rgb, 3);
         frame += (1+3);
      }
   }
}*/

static inline void VideoRenderToRGB(void *buffer)
{
   uint8_t *frame = buffer;
   const uint8_t *palette = ColorPalette();

   for (int y = 0; y < VIDEO_HEIGHT; y++) {
      for (int x = 0; x < VIDEO_WIDTH; x++) {
         const int color = VideoGetPixel_(x, y);
         const uint8_t *rgb = COLOR_RGB((palette != NULL) ? palette[color] : color);
         memcpy(frame, rgb, 3);
         frame += 3;
      }
   }
}

#endif // KONPU_VIDEO_FRAMEBUFFER_H
