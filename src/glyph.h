#ifndef  KONPU_GLYPH_H
#define  KONPU_GLYPH_H
#include "bits.h"
#include "video_mode.h"

//------------------------------------------------------------------------------
// Glyphs types and typedefs
//------------------------------------------------------------------------------

//  Glyph terminology:        Glyph8      Glyph32         Glyph64
//  ------------------          . .       . . . .     . . . . . . . .
// .----------.---------.       . .       . . . .     . . . . . . . .
// | TYPE     |  BITMAP |       . .       . . . .     . . . . . . . .
// |----------|---------|       . .       . . . .     . . . . . . . .
// | Glyph8   |  2 x 4  |                 . . . .     . . . . . . . .
// | Glyph16  |  4 x 4  |     . . . .     . . . .     . . . . . . . .
// | Glyph32  |  4 x 8  |     . . . .     . . . .     . . . . . . . .
// | Glyph64  |  8 x 8  |     . . . .     . . . .     . . . . . . . .
// | Glyph128 |  8 x 16 |     . . . .
// | Glyph256 | 16 x 16 |     Glyph16     Glyphs128 and Glyph256 are
// '----------'---------'                 made of Glyph64s:
typedef uint8_t   Glyph8;     //
typedef uint16_t  Glyph16;    //    top         top_left | top_right
typedef uint32_t  Glyph32;    //   ------    ------------|------------
typedef uint64_t  Glyph64;    //   bottom    bottom_left | bottom_right
typedef struct    Glyph128 { Glyph64 top,
                                     bottom; } Glyph128;
typedef struct    Glyph256 { Glyph64 top_left   , top_right,
                                     bottom_left, bottom_right; } Glyph256;
// When size is not specified, glyphs can be passed as a void pointer.
typedef void      Glyph;

// // Misc.:
// bool GlyphIsEqual(glyph1, glyph2)
//
// // Measurements:
// int GlyphWidth(glyph)
// int GlyphHeight(glyph)
// int GlyphWidthLog2(glyph)
// // int GlyphHeightLog2(glyph)
// int GlyphRealHeight(glyph)
// int GlyphRealWidth(glyph)
// int GlyphMargin<Left|Right|Top|Bottom>(g)
//
// // Access to Pixels
// int   GlyphPixelAt(glyph, x,y)
// bool? GlyphPixelIsSet(glyph, x,y)
// glyph GlyphPixelSet(glyph, x,y, [value=1])
// glyph GlyphPixelUnset(glyph, x,y)
//
// // Bitwise Not, And, Or, Xor:
// glyph GlyphNot(glyph)
// glyph Glyph<And|Or|Xor>(glyph1, glyph2)
//
// // Shifts and Cycles:
// glyph GlyphShift<Up|Down|Left|Right>(glyph, int n) => TODO
// glyph GlyphCycle<Up|Down|Left|Right>(glyph, int n) => TODO
//
// // Flip, Mirror, Rotate, Transpose:
// glyph GlyphFlip(glyph)
// glyph GlyphMirror(glyph)
// glyph GlyphTranspose(glyph)
// glyph GlyphRotate<90|180|270>(glyph)
//
// // Access to Lines:
// unsigned int GlyphLineAt(glyph)
// unsigned int GlyphFlatten(glyph)
//
// // Half/ Doubling glyphs
// glyph GlyphHalf(glyph, int offset)
// glyph GlyphSwapHalves(glyph)
// glyph GlyphFromHalves(glyph left_or_top, glyph right_or_bottom)
// glyph GlyphUpscale(glyph)
//------------------------------------------------------------------------------

// Return true iff glyph1 and glyph2 are the same glyph.
// Both glyph must be of the same glyph type, otherwise comparison is false.
#define GlyphIsEqual(glyph1, glyph2)        ( \
        (sizeof(glyph1) == sizeof(glyph2)) && \
        _Generic((glyph1)                   , \
           uint8_t:  GlyphIsEqual_8_        , \
           uint16_t: GlyphIsEqual_16_       , \
           uint32_t: GlyphIsEqual_32_       , \
           uint64_t: GlyphIsEqual_64_       , \
           Glyph128: GlyphIsEqual_128_      , \
           Glyph256: GlyphIsEqual_256_        \
        )((glyph1), (glyph2))                 )
        static inline bool GlyphIsEqual_8_( uint8_t  g, uint8_t  f)  { return g == f; }
        static inline bool GlyphIsEqual_16_(uint16_t g, uint16_t f)  { return g == f; }
        static inline bool GlyphIsEqual_32_(uint32_t g, uint64_t f)  { return g == f; }
        static inline bool GlyphIsEqual_64_(uint64_t g, uint64_t f)  { return g == f; }
        static inline bool GlyphIsEqual_128_(Glyph128 g, Glyph128 f)
        { return (g.top == f.top) && (g.bottom == f.bottom); }
        static inline bool GlyphIsEqual_256_(Glyph256 g, Glyph256 f) {
           return (g.top_left == f.top_left)
               && (g.top_right == f.top_right)
               && (g.bottom_left == f.bottom_left)
               && (g.bottom_right == f.bottom_right);
        }

//------------------------------------------------------------------------------
// Measurements
//------------------------------------------------------------------------------

// Width and Height constants:
#define GLYPH8_WIDTH          2
#define GLYPH8_HEIGHT         4
#define GLYPH16_WIDTH         4
#define GLYPH16_HEIGHT        4
#define GLYPH32_WIDTH         4
#define GLYPH32_HEIGHT        8
#define GLYPH64_WIDTH         8
#define GLYPH64_HEIGHT        8
#define GLYPH128_WIDTH        8
#define GLYPH128_HEIGHT       16
#define GLYPH256_WIDTH        16
#define GLYPH256_HEIGHT       16
// Log2 constants of Width and Height:
#define GLYPH8_WIDTH_LOG2     1
#define GLYPH8_HEIGHT_LOG2    2
#define GLYPH16_WIDTH_LOG2    2
#define GLYPH16_HEIGHT_LOG2   2
#define GLYPH32_WIDTH_LOG2    2
#define GLYPH32_HEIGHT_LOG2   3
#define GLYPH64_WIDTH_LOG2    3
#define GLYPH64_HEIGHT_LOG2   3
#define GLYPH128_WIDTH_LOG2   3
#define GLYPH128_HEIGHT_LOG2  4
#define GLYPH256_WIDTH_LOG2   4
#define GLYPH256_HEIGHT_LOG2  4

#define GlyphWidth(glyph)  _Generic((glyph), \
        uint8_t:  GLYPH8_WIDTH             , \
        uint16_t: GLYPH16_WIDTH            , \
        uint32_t: GLYPH32_WIDTH            , \
        uint64_t: GLYPH64_WIDTH            , \
        Glyph128: GLYPH128_WIDTH           , \
        Glyph256: GLYPH256_WIDTH             )

#define GlyphHeight(glyph)  _Generic((glyph), \
        uint8_t:  GLYPH8_HEIGHT             , \
        uint16_t: GLYPH16_HEIGHT            , \
        uint32_t: GLYPH32_HEIGHT            , \
        uint64_t: GLYPH64_HEIGHT            , \
        Glyph128: GLYPH128_HEIGHT           , \
        Glyph256: GLYPH256_HEIGHT             )

#define GlyphWidthLog2(glyph)  _Generic((glyph), \
        uint8_t:  GLYPH8_WIDTH_LOG2            , \
        uint16_t: GLYPH16_WIDTH_LOG2           , \
        uint32_t: GLYPH32_WIDTH_LOG2           , \
        uint64_t: GLYPH64_WIDTH_LOG2           , \
        Glyph128: GLYPH128_WIDTH_LOG2          , \
        Glyph256: GLYPH256_WIDTH_LOG2            )

#define GlyphHeightLog2(glyph)  _Generic((glyph), \
        uint8_t:  GLYPH8_HEIGHT_LOG2            , \
        uint16_t: GLYPH16_HEIGHT_LOG2           , \
        uint32_t: GLYPH32_HEIGHT_LOG2           , \
        uint64_t: GLYPH64_HEIGHT_LOG2           , \
        Glyph128: GLYPH128_HEIGHT_LOG2          , \
        Glyph256: GLYPH256_HEIGHT_LOG2            )

#define GlyphIsSquare(glyph)  _Generic((glyph), \
        uint8_t:  false                       , \
        uint16_t: true                        , \
        uint32_t: false                       , \
        uint64_t: true                        , \
        Glyph128: false                       , \
        Glyph256: true                          )



// Construct Glyphs from literals (in hexadecimal):
#define GLYPH8( hex_lit)     ((uint8_t) 0x##hex_lit##U)
#define GLYPH16(hex_lit)     ((uint16_t)0x##hex_lit##U)
#define GLYPH32(hex_lit)     ((uint32_t)0x##hex_lit##U)
#define GLYPH64(hex_lit)     ((uint64_t)0x##hex_lit##U)
#define GLYPH128(hex_bottom_lit, hex_top_lit)         \
        ((Glyph128){ 0x##hex_top_lit, 0x##hex_bottom_lit })
#define GLYPH256(hex_bottom_right_lit, hex_bottom_left_lit,          \
                 hex_top_right_lit,    hex_top_left_lit)             \
        ((Glyph256){ .bottom_right = GLYPH64(hex_bottom_right_lit) , \
                     .bottom_left  = GLYPH64(hex_bottom_left_lit)  , \
                     .top_right    = GLYPH64(hex_top_right_lit)    , \
                     .top_left     = GLYPH64(hex_top_left_lit)     } )


//------------------------------------------------------------------------------
// Access to Pixels
// GlyphPixelAt(glyph, x,y)
// GlyphPixelIsSet(glyph, x,y)
// GlyphPixelSet(glyph, x,y, [value=1])
// GlyphPixelUnset(glyph, x,y)
//------------------------------------------------------------------------------

// Pixel index
#define GLYPH8_INDEX(x,y)    ((x) + ((y) << GLYPH8_WIDTH_LOG2))
#define GLYPH16_INDEX(x,y)   ((x) + ((y) << GLYPH16_WIDTH_LOG2))
#define GLYPH32_INDEX(x,y)   ((x) + ((y) << GLYPH32_WIDTH_LOG2))
#define GLYPH64_INDEX(x,y)   ((x) + ((y) << GLYPH64_WIDTH_LOG2))
#define GLYPH128_INDEX(x,y)  ((x) + ((y) << GLYPH128_WIDTH_LOG2))
#define GLYPH256_INDEX(x,y)  ((x) + ((y) << GLYPH256_WIDTH_LOG2))
// #define GLYPH_INDEX(glyph, x,y)   ((x) + ((y) << GlyphWidthLog2(glyph)))

#if 0
// TODO: should I also do const void* ???
// Return the value (0 or 1) of the pixel (x,y) of the given glyph
#define GlyphPixelAt(glyph, x,y)           \
        _Generic((glyph)                 , \
           uint8_t:  GlyphPixelAt_8_     , \
           uint16_t: GlyphPixelAt_16_    , \
           uint32_t: GlyphPixelAt_32_    , \
           uint64_t: GlyphPixelAt_64_    , \
           Glyph128: GlyphPixelAt_128_   , \
           Glyph256: GlyphPixelAt_256_   , \
           void*   : GlyphPixelAt_dynamic_ \
        )((glyph), (x), (y))
        static inline uint8_t  GlyphPixelAt_8_(uint8_t g, int x, int y)
        { assert(x >= 0 && x < GLYPH8_WIDTH);
          assert(y >= 0 && y < GLYPH8_HEIGHT);
          return (g >> GLYPH8_INDEX(x,y)) & 1U;
        }
        static inline uint16_t GlyphPixelAt_16_(uint16_t g, int x, int y)
        { assert(x >= 0 && x < GLYPH16_WIDTH);
          assert(y >= 0 && y < GLYPH16_HEIGHT);
          return (g >> GLYPH16_INDEX(x,y)) & 1U;
        }
        static inline uint32_t GlyphPixelAt_32_(uint32_t g, int x, int y)
        { assert(x >= 0 && x < GLYPH32_WIDTH);
          assert(y >= 0 && y < GLYPH32_HEIGHT);
          return (g >> GLYPH32_INDEX(x,y)) & 1U;
        }
        static inline uint64_t GlyphPixelAt_64_(uint64_t g, int x, int y)
        { assert(x >= 0 && x < GLYPH64_WIDTH);
          assert(y >= 0 && y < GLYPH64_HEIGHT);
          return (g >> GLYPH64_INDEX(x,y)) & 1U;
        }
        static inline uint64_t GlyphPixelAt_128_(Glyph128 g, int x, int y)
        { assert(x >= 0 && x < GLYPH128_WIDTH);
          assert(y >= 0 && y < GLYPH128_HEIGHT);
          if (y < 8)
             return GlyphPixelAt_64_(g.top, x, y);
          else
             return GlyphPixelAt_64_(g.bottom, x, y - 8);
        }
        static inline uint64_t GlyphPixelAt_256_(Glyph256 g, int x, int y)
        { assert(x >= 0 && x < GLYPH256_WIDTH);
          assert(y >= 0 && y < GLYPH256_HEIGHT);

/*
          if (x < 8)  return (y < 8) ?
                             GlyphPixelAt_64_(g.top_right,    x - 8, y)    :
                             GlyphPixelAt_64_(g.bottom_right, x - 8, y - 8);
          else        return (y < 8) ?
                             GlyphPixelAt_64_(g.top_right   , x - 8, y)    :
                             GlyphPixelAt_64_(g.bottom_right, x - 8, y - 8);
*/


          if (x < 8) {
             if (y < 8)
                return GlyphPixelAt_64_(g.top_left, x, y);
             else
                return GlyphPixelAt_64_(g.bottom_left, x, y - 8);
          } else {
            if (y < 8)
                return GlyphPixelAt_64_(g.top_right, x - 8, y);
             else
                return GlyphPixelAt_64_(g.bottom_right, x - 8, y - 8);
          }
        }
        // TODO: should it be const void* ???
        static inline uint64_t GlyphPixelAt_dynamic_(const void *g, int x, int y)
        {
           switch (VideoModeElementDescriptor()) {
               case PIXELS_2x4  : return GlyphPixelAt(*(uint8_t *)g, x, y);
               case PIXELS_4x4  : return GlyphPixelAt(*(uint16_t*)g, x, y);
               case PIXELS_4x8  : return GlyphPixelAt(*(uint32_t*)g, x, y);
               case PIXELS_8x8  : return GlyphPixelAt(*(uint64_t*)g, x, y);
               case PIXELS_8x16 : return GlyphPixelAt(*(Glyph128*)g, x, y);
               case PIXELS_16x16: return GlyphPixelAt(*(Glyph256*)g, x, y);
               default: unreachable();
           }
        }
#endif
// TODO: should I also do const void* ???
// Return the value (0 or 1) of the pixel (x,y) of the given glyph
#define GlyphPixelAt(glyph, x,y)           \
   _Generic((glyph)                 , \
      uint8_t:  GlyphPixelAt_8_     , \
      uint16_t: GlyphPixelAt_16_    , \
      uint32_t: GlyphPixelAt_32_    , \
      uint64_t: GlyphPixelAt_64_    , \
      Glyph128: GlyphPixelAt_128_   , \
      Glyph256: GlyphPixelAt_256_   , \
      default : GlyphPixelAt_dynamic_ \
   )((glyph), (x), (y))
   static inline uint8_t  GlyphPixelAt_8_(uint8_t g, int x, int y)
   {  assert(x >= 0 && x < GLYPH8_WIDTH);
      assert(y >= 0 && y < GLYPH8_HEIGHT);
      return (g >> GLYPH8_INDEX(x,y)) & 1U;
   }
   static inline uint16_t GlyphPixelAt_16_(uint16_t g, int x, int y)
   {  assert(x >= 0 && x < GLYPH16_WIDTH);
      assert(y >= 0 && y < GLYPH16_HEIGHT);
      return (g >> GLYPH16_INDEX(x,y)) & 1U;
   }
   static inline uint32_t GlyphPixelAt_32_(uint32_t g, int x, int y)
   {  assert(x >= 0 && x < GLYPH32_WIDTH);
      assert(y >= 0 && y < GLYPH32_HEIGHT);
      return (g >> GLYPH32_INDEX(x,y)) & 1U;
   }
   static inline uint64_t GlyphPixelAt_64_(uint64_t g, int x, int y)
   {  assert(x >= 0 && x < GLYPH64_WIDTH);
      assert(y >= 0 && y < GLYPH64_HEIGHT);
      return (g >> GLYPH64_INDEX(x,y)) & 1U;
   }
   static inline uint64_t GlyphPixelAt_128_(Glyph128 g, int x, int y)
   {  assert(x >= 0 && x < GLYPH128_WIDTH);
      assert(y >= 0 && y < GLYPH128_HEIGHT);
      if (y < 8)
         return GlyphPixelAt_64_(g.top, x, y);
      else
         return GlyphPixelAt_64_(g.bottom, x, y - 8);
   }
   static inline uint64_t GlyphPixelAt_256_(Glyph256 g, int x, int y)
   {  assert(x >= 0 && x < GLYPH256_WIDTH);
      assert(y >= 0 && y < GLYPH256_HEIGHT);
      if (x < 8) {
         if (y < 8)
            return GlyphPixelAt_64_(g.top_left, x, y);
         else
            return GlyphPixelAt_64_(g.bottom_left, x, y - 8);
      } else {
         if (y < 8)
            return GlyphPixelAt_64_(g.top_right, x - 8, y);
         else
            return GlyphPixelAt_64_(g.bottom_right, x - 8, y - 8);
      }
   }
   static inline uint64_t GlyphPixelAt_dynamic_(const void *g, int x, int y)
   {
      assert(g != NULL);
      switch (VideoModeElementDescriptor()) {
         case PIXELS_2x4  : return GlyphPixelAt(*(const uint8_t *)g, x, y);
         case PIXELS_4x4  : return GlyphPixelAt(*(const uint16_t*)g, x, y);
         case PIXELS_4x8  : return GlyphPixelAt(*(const uint32_t*)g, x, y);
         case PIXELS_8x8  : return GlyphPixelAt(*(const uint64_t*)g, x, y);
         case PIXELS_8x16 : return GlyphPixelAt(*(const Glyph128*)g, x, y);
         case PIXELS_16x16: return GlyphPixelAt(*(const Glyph256*)g, x, y);
         default: unreachable();
      }
   }

// Return a non-zero value iff the pixel (x,y) of the glyph is set
// Note about PixelIsSet vs PixelAt in tests:
//      At() is `(x >> n) & 1` where as IsSet() is `x & (1 << n)`.
//      If n is a constant, IsSet might take one op less.
#define GlyphPixelIsSet(glyph, x,y)        \
        _Generic((glyph)                 , \
           uint8_t:  GlyphPixelIsSet_8_  , \
           uint16_t: GlyphPixelIsSet_16_ , \
           uint32_t: GlyphPixelIsSet_32_ , \
           uint64_t: GlyphPixelIsSet_64_ , \
           Glyph128: GlyphPixelIsSet_128_, \
           Glyph256: GlyphPixelIsSet_256_  \
        )((glyph), (x), (y))
        static inline uint8_t  GlyphPixelIsSet_8_(uint8_t g, int x, int y)
        { assert(x >= 0 && x < GLYPH8_WIDTH);
          assert(y >= 0 && y < GLYPH8_HEIGHT);
          return g & (1U << GLYPH8_INDEX(x,y));
        }
        static inline uint16_t GlyphPixelIsSet_16_(uint16_t g, int x, int y)
        { assert(x >= 0 && x < GLYPH16_WIDTH);
          assert(y >= 0 && y < GLYPH16_HEIGHT);
          return g & (1U << GLYPH16_INDEX(x,y));
        }
        static inline uint32_t GlyphPixelIsSet_32_(uint32_t g, int x, int y)
        { assert(x >= 0 && x < GLYPH32_WIDTH);
          assert(y >= 0 && y < GLYPH32_HEIGHT);
          return g & (UINT32_C(1) << GLYPH32_INDEX(x,y));
        }
        static inline uint64_t GlyphPixelIsSet_64_(uint64_t g, int x, int y)
        { assert(x >= 0 && x < GLYPH64_WIDTH);
          assert(y >= 0 && y < GLYPH64_HEIGHT);
          return g & (UINT64_C(1) << GLYPH64_INDEX(x,y));
        }
        static inline uint64_t GlyphPixelIsSet_128_(Glyph128 g, int x, int y)
        { assert(x >= 0 && x < GLYPH128_WIDTH);
          assert(y >= 0 && y < GLYPH128_HEIGHT);
          if (y < 8)
             return GlyphPixelIsSet_64_(g.top, x, y);
          else
             return GlyphPixelIsSet_64_(g.bottom, x, y - 8);
        }
        static inline uint64_t GlyphPixelIsSet_256_(Glyph256 g, int x, int y)
        { assert(x >= 0 && x < GLYPH256_WIDTH);
          assert(y >= 0 && y < GLYPH256_HEIGHT);
          if (x < 8) {
             if (y < 8)
                return GlyphPixelIsSet_64_(g.top_left, x, y);
             else
                return GlyphPixelIsSet_64_(g.bottom_left, x, y - 8);
          } else {
            if (y < 8)
                return GlyphPixelIsSet_64_(g.top_right, x - 8, y);
             else
                return GlyphPixelIsSet_64_(g.bottom_right, x - 8, y - 8);
          }
        }

//------------------------------------------------------------------------------
// Bitwise operations (not, and, or, xor) are similar to the native C operators,
// but will ensure that return is also of the same type (no integer promotion).
// For and/or/xor, both glyphs should be of the same type.
//------------------------------------------------------------------------------

// Bitwise Not (i.e. inverse) of a glyph
#define GlyphNot(glyph)             \
        _Generic((glyph)          , \
           uint8_t:  GlyphNot_8_  , \
           uint16_t: GlyphNot_16_ , \
           uint32_t: GlyphNot_32_ , \
           uint64_t: GlyphNot_64_ , \
           Glyph128: GlyphNot_128_, \
           Glyph256: GlyphNot_256_  \
        )((glyph))
        static inline uint8_t  GlyphNot_8_( uint8_t  g) {return ~g;}
        static inline uint16_t GlyphNot_16_(uint16_t g) {return ~g;}
        static inline uint32_t GlyphNot_32_(uint32_t g) {return ~g;}
        static inline uint64_t GlyphNot_64_(uint64_t g) {return ~g;}
        static inline Glyph128 GlyphNot_128_(Glyph128 g)
        { return (Glyph128){ ~g.top, ~g.bottom }; }
        static inline Glyph256 GlyphNot_256_(Glyph256 g)
        { return (Glyph256){ ~g.top_left   , ~g.top_right,
                             ~g.bottom_left, ~g.bottom_right }; }

// Bitwise And between two glyphs
#define GlyphAnd(glyph1, glyph2)    \
        _Generic((glyph1)         , \
           uint8_t:  GlyphAnd_8_  , \
           uint16_t: GlyphAnd_16_ , \
           uint32_t: GlyphAnd_32_ , \
           uint64_t: GlyphAnd_64_ , \
           Glyph128: GlyphAnd_128_, \
           Glyph256: GlyphAnd_256_  \
        )((glyph1), (glyph2))
        static inline uint8_t  GlyphAnd_8_( uint8_t  g, uint8_t  f) {return g&f;}
        static inline uint16_t GlyphAnd_16_(uint16_t g, uint16_t f) {return g&f;}
        static inline uint32_t GlyphAnd_32_(uint32_t g, uint32_t f) {return g&f;}
        static inline uint64_t GlyphAnd_64_(uint64_t g, uint64_t f) {return g&f;}
        static inline Glyph128 GlyphAnd_128_(Glyph128 g, Glyph128 f)
        { return (Glyph128){ g.top&f.top, g.bottom&f.bottom }; }
        static inline Glyph256 GlyphAnd_256_(Glyph256 g, Glyph256 f)
        { return (Glyph256){ g.top_left&f.top_left, g.top_right&f.top_right,
                             g.bottom_left&f.bottom_left, g.bottom_right&f.bottom_left }; }

// Bitwise Or (i.e. union) of two glyphs
#define GlyphOr(glyph1, glyph2)    \
        _Generic((glyph1)        , \
           uint8_t:  GlyphOr_8_  , \
           uint16_t: GlyphOr_16_ , \
           uint32_t: GlyphOr_32_ , \
           uint64_t: GlyphOr_64_ , \
           Glyph128: GlyphOr_128_, \
           Glyph256: GlyphOr_256_  \
        )((glyph1), (glyph2))
        static inline uint8_t  GlyphOr_8_( uint8_t  g, uint8_t  f) {return g|f;}
        static inline uint16_t GlyphOr_16_(uint16_t g, uint16_t f) {return g|f;}
        static inline uint32_t GlyphOr_32_(uint32_t g, uint32_t f) {return g|f;}
        static inline uint64_t GlyphOr_64_(uint64_t g, uint64_t f) {return g|f;}
        static inline Glyph128 GlyphOr_128_(Glyph128 g, Glyph128 f)
        { return (Glyph128){ g.top|f.top, g.bottom|f.bottom }; }
        static inline Glyph256 GlyphOr_256_(Glyph256 g, Glyph256 f)
        { return (Glyph256){ g.top_left|f.top_left, g.top_right|f.top_right,
                             g.bottom_left|f.bottom_left, g.bottom_right|f.bottom_left }; }

// Bitwise Xor between two glyphs
#define GlyphXor(glyph1, glyph2)    \
        _Generic((glyph1)         , \
           uint8_t:  GlyphXor_8_  , \
           uint16_t: GlyphXor_16_ , \
           uint32_t: GlyphXor_32_ , \
           uint64_t: GlyphXor_64_ , \
           Glyph128: GlyphXor_128_, \
           Glyph256: GlyphXor_256_  \
        )((glyph1), (glyph2))
        static inline uint8_t  GlyphXor_8_( uint8_t  g, uint8_t  f) {return g^f;}
        static inline uint16_t GlyphXor_16_(uint16_t g, uint16_t f) {return g^f;}
        static inline uint32_t GlyphXor_32_(uint32_t g, uint32_t f) {return g^f;}
        static inline uint64_t GlyphXor_64_(uint64_t g, uint64_t f) {return g^f;}
        static inline Glyph128 GlyphXor_128_(Glyph128 g, Glyph128 f)
        { return (Glyph128){ g.top^f.top, g.bottom^f.bottom }; }
        static inline Glyph256 GlyphXor_256_(Glyph256 g, Glyph256 f)
        { return (Glyph256){ g.top_left^f.top_left, g.top_right^f.top_right,
                             g.bottom_left^f.bottom_left, g.bottom_right^f.bottom_left }; }

//------------------------------------------------------------------------------
// Functions dealing with glyph lines
//------------------------------------------------------------------------------

// Return an unsigned value representing the n-th line of the given glyph
#define GlyphLineAt(glyph, n)          \
        _Generic((glyph)             , \
           uint8_t:  GlyphLineAt_8_  , \
           uint16_t: GlyphLineAt_16_ , \
           uint32_t: GlyphLineAt_32_ , \
           uint64_t: GlyphLineAt_64_ , \
           Glyph128: GlyphLineAt_128_, \
           Glyph256: GlyphLineAt_256_  \
        )((glyph))
        static inline unsigned GlyphLineAt_8_( uint8_t  g, int n)
        { assert(n >= 0 && n < GLYPH8_HEIGHT);  return BitsQuarterAt(g, n); }
        static inline unsigned GlyphLineAt_16_(uint16_t g, int n)
        { assert(n >= 0 && n < GLYPH16_HEIGHT); return BitsNibbleAt(g, n); }
        static inline unsigned GlyphLineAt_32_(uint32_t g, int n)
        { assert(n >= 0 && n < GLYPH32_HEIGHT); return BitsNibbleAt(g, n); }
        static inline unsigned GlyphLineAt_64_(uint64_t g, int n)
        { assert(n >= 0 && n < GLYPH64_HEIGHT); return BitsByteAt(g, n); }
        static inline unsigned GlyphLineAt_128_(Glyph128 g, int n)
        { assert(n >= 0 && n < GLYPH128_HEIGHT);
          return (n < 8)? GlyphLineAt_64_(g.top, n) :
                          GlyphLineAt_64_(g.bottom, n - 8); }
        static inline unsigned GlyphLineAt_256_(Glyph256 g, int n)
        { assert(n >= 0 && n < GLYPH256_HEIGHT);
          if (n < 8) {
             return (GlyphLineAt_64_(g.top_left, n) << 8) | GlyphLineAt_64_(g.top_right, n);
          } else {
             n -= 8;
             return (GlyphLineAt_64_(g.bottom_left, n) << 8) | GlyphLineAt_64_(g.bottom_right, n);
          }
        }

// Combine all the lines of the glyph into a single value using a binary-or
#define GlyphFlatten(glyph)             \
        _Generic((glyph)              , \
           uint8_t:  GlyphFlatten_8_  , \
           uint16_t: GlyphFlatten_16_ , \
           uint32_t: GlyphFlatten_32_ , \
           uint64_t: GlyphFlatten_64_ , \
           Glyph128: GlyphFlatten_128_, \
           Glyph256: GlyphFlatten_256_  \
        )((glyph))
        static inline unsigned GlyphFlatten_8_(uint8_t g)
        { g |= g >> 4;
          g |= g >> 2;
          return g & 3u; }
        static inline unsigned GlyphFlatten_16_(uint16_t g)
        { g |= g >> 8;
          g |= g >> 4;
          return g & 0xFu; }
        static inline unsigned GlyphFlatten_32_(uint32_t g)
        { g |= g >> 16;
          g |= g >>  8;
          g |= g >>  4;
          return g & 0xFu; }
        static inline unsigned GlyphFlatten_64_(uint64_t g)
        { g |= g >> 32;
          g |= g >> 16;
          g |= g >>  8;
          return g & 0xFFu; }
        static inline unsigned GlyphFlatten_128_(Glyph128 g)
        { return GlyphFlatten_64_(g.top) | GlyphFlatten_64_(g.bottom); }
        static inline unsigned GlyphFlatten_256_(Glyph256 g)
        { unsigned left  = GlyphFlatten_64_(g.top_left)  | GlyphFlatten_64_(g.bottom_left);
          unsigned right = GlyphFlatten_64_(g.top_right) | GlyphFlatten_64_(g.bottom_right);
          return (left << 8) | right;
        }

// Return a glyph of type GLYPH_TYPE (for example GLYPH64) which is made of n
// lines on the left. n must be in [0, width of the glyph type].

#define GlyphLinesLeft(GLYPH_TYPE, n)  \
        UTIL_CAT(UTIL_CAT(GlyphLinesLeft_, GLYPH_TYPE), _)(n)
        static inline uint8_t GlyphLinesLeft_GLYPH8_(int n) {
           assert(n >= 0 && n <= GLYPH8_WIDTH);
           return ((1u << n) - 1) * GLYPH8(55);
        }
        static inline uint16_t GlyphLinesLeft_GLYPH16_(int n) {
           assert(n >= 0 && n <= GLYPH32_WIDTH);
           return ((1u << n) - 1) * GLYPH16(1111);
        }
        static inline uint32_t GlyphLinesLeft_GLYPH32_(int n) {
           assert(n >= 0 && n <= GLYPH32_WIDTH);
           return ((1u << n) - 1) * GLYPH32(11111111);
        }
        static inline uint64_t GlyphLinesLeft_GLYPH64_(int n) {
           assert(n >= 0 && n <= GLYPH64_WIDTH);
           return ((1u << n) - 1) * GLYPH64(0101010101010101);
        }
        static inline Glyph128 GlyphLinesLeft_GLYPH128_(int n) {
           uint64_t g = GlyphLinesLeft_GLYPH64_(n);
           return (Glyph128){ g, g };
        }
        static inline Glyph256 GlyphLinesLeft_GLYPH256_(int n) {
           assert(n >= 0 && n <= GLYPH256_WIDTH);
           if (n < 8) {
              uint64_t g = GlyphLinesLeft_GLYPH64_(n);
              return (Glyph256){ .top_left = g, .bottom_left = g };
           } else {
              uint64_t g = GlyphLinesLeft_GLYPH64_(n - 8);
              return (Glyph256){
                 .top_left    = GLYPH64(ffffffffffffffff), .top_right = g,
                 .bottom_left = GLYPH64(ffffffffffffffff), .bottom_right = g
              };
           }
        }

#define GlyphLinesRight(GLYPH_TYPE, n)  \
        GlyphNot(GlyphLinesLeft(GLYPH_TYPE, UTIL_CAT(GLYPH_TYPE,_WIDTH) - (n)))

//------------------------------------------------------------------------------
// Shifts and Cycles:
// The behavior is undefined if n is negative or is greater or equal the number
// of bits of the glyph.
//------------------------------------------------------------------------------

// Return the glyph shifted n pixels upward (n must be in [0..GLYPH_HEIGHT[)
#define GlyphShiftUp(glyph, n)          \
        _Generic((glyph)              , \
           uint8_t:  GlyphShiftUp_8_  , \
           uint16_t: GlyphShiftUp_16_ , \
           uint32_t: GlyphShiftUp_32_ , \
           uint64_t: GlyphShiftUp_64_ , \
           Glyph128: GlyphShiftUp_128_, \
           Glyph256: GlyphShiftUp_256_  \
        )((glyph), (n))
        static inline uint8_t  GlyphShiftUp_8_(uint8_t g, int n) {
            assert(n >= 0 && n < GLYPH8_HEIGHT);
            return g >> (n << GLYPH8_WIDTH_LOG2);
        }
        static inline uint16_t GlyphShiftUp_16_(uint16_t g, int n) {
            assert(n >= 0 && n < GLYPH16_HEIGHT);
            return g >> (n << GLYPH16_WIDTH_LOG2);
        }
        static inline uint32_t GlyphShiftUp_32_(uint32_t g, int n) {
            assert(n >= 0 && n < GLYPH32_HEIGHT);
            return g >> (n << GLYPH32_WIDTH_LOG2);
        }
        static inline uint64_t GlyphShiftUp_64_(uint64_t g, int n) {
            assert(n >= 0 && n < GLYPH64_HEIGHT);
            return g >> (n << GLYPH64_WIDTH_LOG2);
        }
        static inline Glyph128 GlyphShiftUp_128_(Glyph128 g, int n) {
            assert(n >= 0 && n < GLYPH128_HEIGHT);
            if (n == 0) {
               // Do nothing. This case (n==0) is handled separately, as the
               // code below for (n < 8) would lead to do <<64 (which is UB)
            } else if (n < GLYPH64_HEIGHT) {
               g.top    = GlyphShiftUp_64_(g.top, n) |
                          (g.bottom << (64 - (n << GLYPH64_WIDTH_LOG2)));
               g.bottom = GlyphShiftUp_64_(g.bottom, n);
            } else {
               g.top    = GlyphShiftUp_64_(g.bottom, n - GLYPH64_HEIGHT);
               g.bottom = 0;
            }
            return g;
        }
        static inline Glyph256 GlyphShiftUp_256_(Glyph256 g, int n) {
            assert(n >= 0 && n < GLYPH256_HEIGHT);
            if (n == 0) {
               // Nothing to do. This case (n==0) is handled separately, as the
               // code below for (n < 8) would lead to do <<64 (which is UB)
               return g;
            } else if (n < GLYPH64_HEIGHT) {
               int off = 64 - (n << GLYPH64_WIDTH_LOG2);
               return (Glyph256){
                  .top_left  = GlyphShiftUp_64_(g.top_left, n)  | (g.bottom_left  << off),
                  .top_right = GlyphShiftUp_64_(g.top_right, n) | (g.bottom_right << off),
                  .bottom_left  = GlyphShiftUp_64_(g.bottom_left, n),
                  .bottom_right = GlyphShiftUp_64_(g.bottom_right, n),
               };
            } else {
               n -= GLYPH64_HEIGHT;
               return (Glyph256){
                  GlyphShiftUp_64_(g.bottom_left, n), GlyphShiftUp_64_(g.bottom_right, n),
                  0                                 , 0
               };
            }
        }

// Return the glyph shifted n pixels downward (n must be in [0..GLYPH_HEIGHT[)
#define GlyphShiftDown(glyph, n)          \
        _Generic((glyph)                , \
           uint8_t:  GlyphShiftDown_8_  , \
           uint16_t: GlyphShiftDown_16_ , \
           uint32_t: GlyphShiftDown_32_ , \
           uint64_t: GlyphShiftDown_64_ , \
           Glyph128: GlyphShiftDown_128_, \
           Glyph256: GlyphShiftDown_256_  \
        )((glyph), (n))
        static inline uint8_t  GlyphShiftDown_8_(uint8_t g, int n)
        { assert(n >= 0 && n < GLYPH8_HEIGHT);
          return g << (n << GLYPH8_WIDTH_LOG2);
        }
        static inline uint16_t GlyphShiftDown_16_(uint16_t g, int n)
        { assert(n >= 0 && n < GLYPH16_HEIGHT);
          return g << (n << GLYPH16_WIDTH_LOG2);
        }
        static inline uint32_t GlyphShiftDown_32_(uint32_t g, int n)
        { assert(n >= 0 && n < GLYPH32_HEIGHT);
          return g << (n << GLYPH32_WIDTH_LOG2);
        }
        static inline uint64_t GlyphShiftDown_64_(uint64_t g, int n)
        { assert(n >= 0 && n < GLYPH64_HEIGHT);
          return g << (n << GLYPH64_WIDTH_LOG2);
        }
        static inline Glyph128 GlyphShiftDown_128_(Glyph128 g, int n) {
            assert(n >= 0 && n < GLYPH128_HEIGHT);
            if (n == 0) {
               // Do nothing. This case (n==0) is handled separately, as the
               // code below for (n < 8) would lead to do >>64 (which is UB)
            } else if (n < GLYPH64_HEIGHT) {
               g.bottom = GlyphShiftDown_64_(g.bottom, n) |
                          (g.top >> (64 - (n << GLYPH64_WIDTH_LOG2)));
               g.top    = GlyphShiftDown_64_(g.top, n);
            } else {
               g.bottom = GlyphShiftDown_64_(g.top, n - GLYPH64_HEIGHT);
               g.top = 0;
            }
            return g;
        }
        static inline Glyph256 GlyphShiftDown_256_(Glyph256 g, int n) {
            assert(n >= 0 && n < GLYPH256_HEIGHT);
            if (n == 0) {
               // Nothing to do. This case (n==0) is handled separately, as the
               // code below for (n < 8) would lead to do >>64 (which is UB)
               return g;
            } else if (n < GLYPH64_HEIGHT) {
               int off = 64 - (n << GLYPH64_WIDTH_LOG2);
               return (Glyph256){
                  .bottom_left  = GlyphShiftDown_64_(g.bottom_left, n)  | (g.top_left  >> off),
                  .bottom_right = GlyphShiftDown_64_(g.bottom_right, n) | (g.top_right >> off),
                  .top_left  = GlyphShiftDown_64_(g.top_left, n),
                  .top_right = GlyphShiftDown_64_(g.top_right, n),
               };
            } else {
               n -= GLYPH64_HEIGHT;
               return (Glyph256){
                  0                                , 0,
                  GlyphShiftDown_64_(g.top_left, n), GlyphShiftDown_64_(g.top_right, n)
               };
            }
        }

#define GlyphShiftLeft(glyph, n)          \
        _Generic((glyph)                , \
           uint8_t:  GlyphShiftLeft_8_  , \
           uint16_t: GlyphShiftLeft_16_ , \
           uint32_t: GlyphShiftLeft_32_ , \
           uint64_t: GlyphShiftLeft_64_ , \
           Glyph128: GlyphShiftLeft_128_, \
           Glyph256: GlyphShiftLeft_256_  \
        )((glyph), (n))
        static inline uint8_t GlyphShiftLeft_8_(uint8_t g, int n) {
           assert(n >= 0 && n < GLYPH8_WIDTH);
           return (g >> n) & GlyphLinesLeft(GLYPH8, GLYPH8_WIDTH - n);
        }
        static inline uint16_t GlyphShiftLeft_16_(uint16_t g, int n) {
           assert(n >= 0 && n < GLYPH16_WIDTH);
           return (g >> n) & GlyphLinesLeft(GLYPH16, GLYPH16_WIDTH - n);
        }
        static inline uint32_t GlyphShiftLeft_32_(uint32_t g, int n) {
           assert(n >= 0 && n < GLYPH32_WIDTH);
           return (g >> n) & GlyphLinesLeft(GLYPH32, GLYPH32_WIDTH - n);
        }
        static inline uint64_t GlyphShiftLeft_64_(uint64_t g, int n) {
           assert(n >= 0 && n < GLYPH64_WIDTH);
           return (g >> n) & GlyphLinesLeft(GLYPH64, GLYPH64_WIDTH - n);
        }
        static inline Glyph128 GlyphShiftLeft_128_(Glyph128 g, int n) {
           assert(n >= 0 && n < GLYPH128_WIDTH);
           return (Glyph128){ GlyphShiftLeft_64_(g.top, n),
                              GlyphShiftLeft_64_(g.bottom, n) };
        }
        static inline Glyph256 GlyphShiftLeft_256_(Glyph256 g, int n) {
           assert(n >= 0 && n < GLYPH256_WIDTH);
           if (n < 8) {
              g.top_left    = GlyphShiftLeft_64_(g.top_left, n);
              g.bottom_left = GlyphShiftLeft_64_(g.bottom_left, n);
              // ^ TODO: we also need to "add" the left column of the right glyphs
              // from n in 1-7,
              g.top_right    = GlyphShiftLeft_64_(g.top_right, n);
              g.bottom_right = GlyphShiftLeft_64_(g.bottom_right, n);
              return g;
           } else {
              n -= 8;
              return (Glyph256){ GlyphShiftLeft_64_(g.top_right, n)   , 0,
                                 GlyphShiftLeft_64_(g.bottom_right, n), 0 };
           }
        }

#define GlyphShiftRight(glyph, n)          \
        _Generic((glyph)                 , \
           uint8_t:  GlyphShiftRight_8_  , \
           uint16_t: GlyphShiftRight_16_ , \
           uint32_t: GlyphShiftRight_32_ , \
           uint64_t: GlyphShiftRight_64_ , \
           Glyph128: GlyphShiftRight_128_, \
           Glyph256: GlyphShiftRight_256_  \
        )((glyph), (n))
        static inline uint8_t GlyphShiftRight_8_(uint8_t g, int n) {
           assert(n >= 0 && n < GLYPH8_WIDTH);
           return (g << n) & GlyphLinesRight(GLYPH8, GLYPH8_WIDTH - n);
        }
        static inline uint16_t GlyphShiftRight_16_(uint16_t g, int n) {
           assert(n >= 0 && n < GLYPH16_WIDTH);
           return (g << n) & GlyphLinesRight(GLYPH16, GLYPH16_WIDTH - n);
        }
        static inline uint32_t GlyphShiftRight_32_(uint32_t g, int n) {
           assert(n >= 0 && n < GLYPH32_WIDTH);
           return (g << n) & GlyphLinesRight(GLYPH32, GLYPH32_WIDTH - n);
        }
        static inline uint64_t GlyphShiftRight_64_(uint64_t g, int n) {
           assert(n >= 0 && n < GLYPH64_WIDTH);
           return (g << n) & GlyphLinesRight(GLYPH64, GLYPH64_WIDTH - n);
        }
        static inline Glyph128 GlyphShiftRight_128_(Glyph128 g, int n) {
           assert(n >= 0 && n < GLYPH128_WIDTH);
           return (Glyph128){ GlyphShiftRight_64_(g.top, n),
                              GlyphShiftRight_64_(g.bottom, n) };
        }
        static inline Glyph256 GlyphShiftRight_256_(Glyph256 g, int n) {
           assert(n >= 0 && n < GLYPH256_WIDTH);
           if (n < 8) {
              g.top_left    = GlyphShiftRight_64_(g.top_left, n);
              g.bottom_left = GlyphShiftRight_64_(g.bottom_left, n);
              // ^ TODO: we also need to "add" the left column of the right glyphs
              // from n in 1-7,
              g.top_right    = GlyphShiftRight_64_(g.top_right, n);
              g.bottom_right = GlyphShiftRight_64_(g.bottom_right, n);
              return g;
           } else {
              n -= 8;
              return (Glyph256){ GlyphShiftRight_64_(g.top_right, n)   , 0,
                                 GlyphShiftRight_64_(g.bottom_right, n), 0 };
           }
        }

// Return the glyph cycled n pixels upward (n must be in [0..GLYPH_HEIGHT[)
#define GlyphCycleUp(glyph, n)          \
        _Generic((glyph)              , \
           uint8_t:  GlyphCycleUp_8_  , \
           uint16_t: GlyphCycleUp_16_ , \
           uint32_t: GlyphCycleUp_32_ , \
           uint64_t: GlyphCycleUp_64_ , \
           Glyph128: GlyphCycleUp_128_, \
           Glyph256: GlyphCycleUp_256_  \
        )((glyph), (n))
        static inline uint8_t  GlyphCycleUp_8_(uint8_t g, int n)
        { assert(n >= 0 && n < GLYPH8_HEIGHT);
          if (n == 0) return g; // because otherwise we get g<<8 which is UB
          n <<= GLYPH8_WIDTH_LOG2;
          return (g >> n) | (g << (8 - n));
        }
        static inline uint16_t GlyphCycleUp_16_(uint16_t g, int n)
        { assert(n >= 0 && n < GLYPH16_HEIGHT);
          if (n == 0) return g;
          n <<= GLYPH16_WIDTH_LOG2;
          return (g >> n) | (g << (16 - n));
        }
        static inline uint32_t GlyphCycleUp_32_(uint32_t g, int n)
        { assert(n >= 0 && n < GLYPH32_HEIGHT);
          if (n == 0) return g;
          n <<= GLYPH32_WIDTH_LOG2;
          return (g >> n) | (g << (32 - n));
        }
        static inline uint64_t GlyphCycleUp_64_(uint64_t g, int n)
        { assert(n >= 0 && n < GLYPH64_HEIGHT);
          if (n == 0) return g;
          n <<= GLYPH64_WIDTH_LOG2;
          return (g >> n) | (g << (64 - n));
        }
        static inline Glyph128 GlyphCycleUp_128_(Glyph128 g, int n)
        { assert(n >= 0 && n < GLYPH128_HEIGHT);
          g = GlyphShiftUp(g, n); //TODO
          return g;
        }
        static inline Glyph256 GlyphCycleUp_256_(Glyph256 g, int n)
        { assert(n >= 0 && n < GLYPH256_HEIGHT);
          (void)g; return (Glyph256){0}; // TODO
        }

// Return the glyph cycled n pixels downward (n must be in [0..GLYPH_HEIGHT[)
#define GlyphCycleDown(glyph, n)          \
        _Generic((glyph)                , \
           uint8_t:  GlyphCycleDown_8_  , \
           uint16_t: GlyphCycleDown_16_ , \
           uint32_t: GlyphCycleDown_32_ , \
           uint64_t: GlyphCycleDown_64_ , \
           Glyph128: GlyphCycleDown_128_, \
           Glyph256: GlyphCycleDown_256_  \
        )((glyph), (n))
        static inline uint8_t  GlyphCycleDown_8_(uint8_t g, int n)
        { assert(n >= 0 && n < GLYPH8_HEIGHT);
          if (n == 0) return g;
          n <<= GLYPH8_WIDTH_LOG2;
          return (g << n) | (g >> (8 - n));
        }
        static inline uint16_t GlyphCycleDown_16_(uint16_t g, int n)
        { assert(n >= 0 && n < GLYPH16_HEIGHT);
          if (n == 0) return g;
          n <<= GLYPH16_WIDTH_LOG2;
          return (g << n) | (g >> (16 - n));
        }
        static inline uint32_t GlyphCycleDown_32_(uint32_t g, int n)
        { assert(n >= 0 && n < GLYPH32_HEIGHT);
          if (n == 0) return g;
          n <<= GLYPH32_WIDTH_LOG2;
          return (g << n) | (g >> (32 - n));
        }
        static inline uint64_t GlyphCycleDown_64_(uint64_t g, int n)
        { assert(n >= 0 && n < GLYPH64_HEIGHT);
          if (n == 0) return g;
          n <<= GLYPH64_WIDTH_LOG2;
          return (g << n) | (g >> (64 - n));
        }
        static inline Glyph128 GlyphCycleDown_128_(Glyph128 g, int n)
        { assert(n >= 0 && n < GLYPH128_HEIGHT);
          (void)g; return (Glyph128){0}; // TODO
        }
        static inline Glyph256 GlyphCycleDown_256_(Glyph256 g, int n)
        { assert(n >= 0 && n < GLYPH256_HEIGHT);
          (void)g; return (Glyph256){0}; // TODO
        }

#define GlyphCycleLeft(glyph, n)                                        \
        /* It would be UNSAFE to evaluate `glyph` and `n`            */ \
        /* in a macro, but otherwise we could just write:            */ \
        /* ((n == 0)? glyph :                                        */ \
        /*            GlyphOr(GlyphShiftLeft(glyph, n),              */ \
        /*            GlyphShiftRight(glyph, GlyphWidth(glyph) - n)) */ \
        _Generic((glyph)                , \
           uint8_t:  GlyphCycleLeft_8_  , \
           uint16_t: GlyphCycleLeft_16_ , \
           uint32_t: GlyphCycleLeft_32_ , \
           uint64_t: GlyphCycleLeft_64_ , \
           Glyph128: GlyphCycleLeft_128_, \
           Glyph256: GlyphCycleLeft_256_  \
        )((glyph), (n))
        static inline uint8_t GlyphCycleLeft_8_(uint8_t g, int n) {
           assert(n >= 0 && n < GLYPH8_WIDTH);
           if (n == 0) return g;
           return GlyphShiftLeft(g, n) | GlyphShiftRight(g, GLYPH8_WIDTH - n);
        }
        static inline uint16_t GlyphCycleLeft_16_(uint16_t g, int n) {
           assert(n >= 0 && n < GLYPH16_WIDTH);
           if (n == 0) return g;
           return GlyphShiftLeft(g, n) | GlyphShiftRight(g, GLYPH16_WIDTH - n);
        }
        static inline uint32_t GlyphCycleLeft_32_(uint32_t g, int n) {
           assert(n >= 0 && n < GLYPH32_WIDTH);
           if (n == 0) return g;
           return GlyphShiftLeft(g, n) | GlyphShiftRight(g, GLYPH32_WIDTH - n);
        }
        static inline uint64_t GlyphCycleLeft_64_(uint64_t g, int n) {
           assert(n >= 0 && n < GLYPH64_WIDTH);
           if (n == 0) return g;
           return GlyphShiftLeft(g, n) | GlyphShiftRight(g, GLYPH64_WIDTH - n);
        }
        static inline Glyph128 GlyphCycleLeft_128_(Glyph128 g, int n) {
           assert(n >= 0 && n < GLYPH128_WIDTH);
           if (n == 0) return g;
           return GlyphOr(GlyphShiftLeft(g, n), GlyphShiftRight(g, GLYPH128_WIDTH - n));
        }
        static inline Glyph256 GlyphCycleLeft_256_(Glyph256 g, int n) {
           assert(n >= 0 && n < GLYPH256_WIDTH);
           if (n == 0) return g;
           return GlyphOr(GlyphShiftLeft(g, n), GlyphShiftRight(g, GLYPH256_WIDTH - n));
        }

#define GlyphCycleRight(glyph, n)                                      \
        /* It would be UNSAFE to evaluate `glyph` and `n`           */ \
        /* in a macro, but otherwise we could just write:           */ \
        /* ((n == 0)? glyph :                                       */ \
        /*            GlyphOr(GlyphShiftRight(glyph, n),            */ \
        /*            GlyphShiftLeft(glyph, GlyphWidth(glyph) - n)) */ \
        _Generic((glyph)                , \
           uint8_t:  GlyphCycleRight_8_  , \
           uint16_t: GlyphCycleRight_16_ , \
           uint32_t: GlyphCycleRight_32_ , \
           uint64_t: GlyphCycleRight_64_ , \
           Glyph128: GlyphCycleRight_128_, \
           Glyph256: GlyphCycleRight_256_  \
        )((glyph), (n))
        static inline uint8_t GlyphCycleRight_8_(uint8_t g, int n) {
           assert(n >= 0 && n < GLYPH8_WIDTH);
           if (n == 0) return g;
           return GlyphShiftRight(g, n) | GlyphShiftLeft(g, GLYPH8_WIDTH - n);
        }
        static inline uint16_t GlyphCycleRight_16_(uint16_t g, int n) {
           assert(n >= 0 && n < GLYPH16_WIDTH);
           if (n == 0) return g;
           return GlyphShiftRight(g, n) | GlyphShiftLeft(g, GLYPH16_WIDTH - n);
        }
        static inline uint32_t GlyphCycleRight_32_(uint32_t g, int n) {
           assert(n >= 0 && n < GLYPH32_WIDTH);
           if (n == 0) return g;
           return GlyphShiftRight(g, n) | GlyphShiftLeft(g, GLYPH32_WIDTH - n);
        }
        static inline uint64_t GlyphCycleRight_64_(uint64_t g, int n) {
           assert(n >= 0 && n < GLYPH64_WIDTH);
           if (n == 0) return g;
           return GlyphShiftRight(g, n) | GlyphShiftLeft(g, GLYPH64_WIDTH - n);
        }
        static inline Glyph128 GlyphCycleRight_128_(Glyph128 g, int n) {
           assert(n >= 0 && n < GLYPH128_WIDTH);
           if (n == 0) return g;
           return GlyphOr(GlyphShiftRight(g, n), GlyphShiftLeft(g, GLYPH128_WIDTH - n));
        }
        static inline Glyph256 GlyphCycleRight_256_(Glyph256 g, int n) {
           assert(n >= 0 && n < GLYPH256_WIDTH);
           if (n == 0) return g;
           return GlyphOr(GlyphShiftRight(g, n), GlyphShiftLeft(g, GLYPH256_WIDTH - n));
        }



#if 0
//------------------------------------------------------------------------------
// transformations: *shift* and *cycle*
//------------------------------------------------------------------------------
// TODO?: maybe there's a way to do cycleLeft/cycleRight more efficiently?

/// @brief return the glyph shifted n pixels to the left (n must be: 0 <= n < 8)
static inline  uint64_t  glyph_shiftLeft(uint64_t glyph, unsigned n)
{ return (glyph >> n) & glyph_nLinesLeft(8 - n); }

/// @brief return the glyph shifted n pixels to the right (n must be: 0 <= n < 8)
static inline  uint64_t  glyph_shiftRight(uint64_t glyph, unsigned n)
{ return (glyph << n) & ~glyph_nLinesLeft(n); }

/// @brief return the glyph shift cycled n pixels left (n must be: 0 <= n < 8)
static inline uint64_t  glyph_cycleLeft(uint64_t glyph, unsigned n)
{ return glyph_shiftLeft(glyph, n) | glyph_shiftRight(glyph, 8 - n); }

/// @brief return the glyph shift cycled n pixels right (n must be: 0 <= n < 8)
static inline uint64_t  glyph_cycleRight(uint64_t glyph, unsigned n)
{ return glyph_shiftRight(glyph, n) | glyph_shiftLeft(glyph, 8 - n); }

#endif



//------------------------------------------------------------------------------
// Flip, Mirror, Rotate, Transpose glyphs
// Note: Rotate and Transpose only work on squared-size glyphs: Glyph<16,64,256>
// Inspiration for implementation: Hacker's Delight +
// https://stackoverflow.com/questions/1667591/rotating-a-bitmap-90-degrees
//------------------------------------------------------------------------------

// Return the given glyph flipped along the horizontal vertical axis (|)
#define GlyphFlip(glyph)             \
        _Generic((glyph)           , \
           uint8_t : GlyphFlip_8_  , \
           uint16_t: GlyphFlip_16_ , \
           uint32_t: GlyphFlip_32_ , \
           uint64_t: GlyphFlip_64_ , \
           Glyph128: GlyphFlip_128_, \
           Glyph256: GlyphFlip_256_  \
        )((glyph))
        static inline uint8_t  GlyphFlip_8_(uint8_t g)
        { // Reverse the ordering of 2bits
          g = ((g & 0xf0U) >>  4) | ((g & 0x0fU) <<  4);
          g = ((g & 0xccU) >>  2) | ((g & 0x33U) <<  2);
          return g;
        }
        static inline uint16_t GlyphFlip_16_(uint16_t g)
        { // Reverse the ordering of nibbles
          g = ((g & 0xff00U) >>  8) | ((g & 0x00ffU) <<  8);
          g = ((g & 0xf0f0U) >>  4) | ((g & 0x0f0fU) <<  4);
          return g;
        }
        static inline uint32_t GlyphFlip_32_(uint32_t g)
        { // Reverse the ordering of nibbles
          g = ((g & 0xffff0000U) >> 16) | ((g & 0x0000ffffU) << 16);
          g = ((g & 0xff00ff00U) >>  8) | ((g & 0x00ff00ffU) <<  8);
          g = ((g & 0xf0f0f0f0U) >>  4) | ((g & 0x0f0f0f0fU) <<  4);
          return g;
        }
        static inline uint64_t GlyphFlip_64_(uint64_t g)
        { // Reverse the ordering of bytes
#         ifdef __GNUC__
             return __builtin_bswap64(g);
#         else
             g = ((g & 0xffffffff00000000U) >> 32) | ((g & 0x00000000ffffffffU) << 32);
             g = ((g & 0xffff0000ffff0000U) >> 16) | ((g & 0x0000ffff0000ffffU) << 16);
             g = ((g & 0xff00ff00ff00ff00U) >>  8) | ((g & 0x00ff00ff00ff00ffU) <<  8);
             return g;
#         endif
        }
        static inline Glyph128 GlyphFlip_128_(Glyph128 g)
        { return (Glyph128){GlyphFlip_64_(g.bottom), GlyphFlip_64_(g.top)}; }
        static inline Glyph256 GlyphFlip_256_(Glyph256 g)
        { return (Glyph256){
            GlyphFlip_64_(g.bottom_left), GlyphFlip_64_(g.bottom_right),
            GlyphFlip_64_(g.top_left),    GlyphFlip_64_(g.top_right)
          };
        }

// Return the given glyph flipped along the vertical axis (|)
#define GlyphMirror(glyph)             \
        _Generic((glyph)             , \
           uint8_t : GlyphMirror_8_  , \
           uint16_t: GlyphMirror_16_ , \
           uint32_t: GlyphMirror_32_ , \
           uint64_t: GlyphMirror_64_ , \
           Glyph128: GlyphMirror_128_, \
           Glyph256: GlyphMirror_256_  \
        )((glyph))
        static inline uint8_t GlyphMirror_8_(uint8_t g)
        { // Reverse the bits order of every 2bit in the glyph
          return ((g & 0xaaU) >> 1) | ((g & 0x55U) << 1);
        }
        static inline uint16_t GlyphMirror_16_(uint16_t g)
        { // Reverse the bits order of every nibble in the glyph
          g = ((g & 0xccccU) >> 2) | ((g & 0x3333U) << 2);
          g = ((g & 0xaaaaU) >> 1) | ((g & 0x5555U) << 1);
          return g;
        }
        static inline uint32_t GlyphMirror_32_(uint32_t g)
        { // Reverse the bits order of every nibble in the glyph
          g = ((g & 0xccccccccU) >> 2) | ((g & 0x33333333U) << 2);
          g = ((g & 0xaaaaaaaaU) >> 1) | ((g & 0x55555555U) << 1);
          return g;
        }
        static inline uint64_t GlyphMirror_64_(uint64_t g)
        { // Reverse the bits order of every byte in the glyph
          g = ((g & 0xf0f0f0f0f0f0f0f0U) >> 4) | ((g & 0x0f0f0f0f0f0f0f0fU) << 4);
          g = ((g & 0xccccccccccccccccU) >> 2) | ((g & 0x3333333333333333U) << 2);
          g = ((g & 0xaaaaaaaaaaaaaaaaU) >> 1) | ((g & 0x5555555555555555U) << 1);
          return g;
        }
        static inline Glyph128 GlyphMirror_128_(Glyph128 g)
        { return (Glyph128){GlyphMirror_64_(g.top),GlyphMirror_64_(g.bottom)}; }
        static inline Glyph256 GlyphMirror_256_(Glyph256 g)
        { return (Glyph256){
            GlyphMirror_64_(g.top_right),    GlyphMirror_64_(g.top_left),
            GlyphMirror_64_(g.bottom_right), GlyphMirror_64_(g.bottom_left)
          };
        }

// Return the given squared-size (16|64|256) glyph flipped along the \ diagonal.
#define GlyphTranspose(square_glyph)     \
        _Generic((square_glyph)        , \
           uint16_t: GlyphTranspose_16_, \
           uint64_t: GlyphTranspose_64_, \
           Glyph256: GlyphTranspose_256_ \
        )((square_glyph))
        static inline uint16_t GlyphTranspose_16_(uint16_t g)
        { // inspired by https://stackoverflow.com/questions/65612229/flip-4x4-bit-grid-horizontally-vertically-diagonally
          return  (g & 0x8421U) | // main diagonal
                  ((g & 0x4210U) >> 3) | ((g & 0x2100U) >> 6) | ((g & 0x1000U) >> 9) |  // <- upper diagonals.
                  ((g & 0x0842U) << 3) | ((g & 0x0084U) << 6) | ((g & 0x0008U) << 9) ;  // <- lower diagonals
        }
        static inline uint64_t GlyphTranspose_64_(uint64_t g)
        { // Based on "Hacker's Delight" (second edition) by Henry S. Warren, Jr.
          // https://web.archive.org/web/20190915025154/http://www.hackersdelight.org/
          // https://en.wikipedia.org/wiki/Hacker%27s_Delight
          g = ( g        & 0xaa55aa55aa55aa55U       ) |
              ((g        & 0x00aa00aa00aa00aaU) << 7 ) |
              ((g >> 7)  & 0x00aa00aa00aa00aaU       ) ;
          g = ( g        & 0xcccc3333cccc3333U       ) |
              ((g        & 0x0000cccc0000ccccU) << 14) |
              ((g >> 14) & 0x0000cccc0000ccccU       ) ;
          g = ( g        & 0xf0f0f0f00f0f0f0fU       ) |
              ((g        & 0x00000000f0f0f0f0U) << 28) |
              ((g >> 28) & 0x00000000f0f0f0f0U)        ;
          return g;
        }
        static inline Glyph256 GlyphTranspose_256_(Glyph256 g)
        { return (Glyph256){
            GlyphTranspose_64_(g.top_left),  GlyphTranspose_64_(g.bottom_left),
            GlyphTranspose_64_(g.top_right), GlyphTranspose_64_(g.bottom_right)
          };
        }

// Return the given Glyph rotated  90 degrees (counterclockwise)
#define GlyphRotate90(square_glyph)    GlyphTranspose(GlyphMirror(square_glyph))

// Return the given Glyph rotated 180 degrees
#define GlyphRotate180(square_glyph)        GlyphMirror(GlyphFlip(square_glyph))

// Return the given Glyph rotated 270 degrees (= 90 degrees clockwise)
#define GlyphRotate270(square_glyph)     GlyphTranspose(GlyphFlip(square_glyph))


//------------------------------------------------------------------------------
// Quadrant Constants
// GLYPH<n>[_<TOP|BOTTOM>][_<LEFT|RIGHT>] / GLYPH<n>_FULL
//------------------------------------------------------------------------------

#define GLYPH8_TOP_LEFT        GLYPH8(a0)
#define GLYPH8_TOP_RIGHT       GLYPH8(50)
#define GLYPH8_BOTTOM_LEFT     GLYPH8(0a)
#define GLYPH8_BOTTOM_RIGHT    GLYPH8(05)
#define GLYPH8_FULL            GLYPH8(ff)
#define GLYPH8_TOP             ((Glyph8)(GLYPH8_TOP_LEFT|GLYPH8_TOP_RIGHT))
#define GLYPH8_BOTTOM          ((Glyph8)(GLYPH8_BOTTOM_LEFT|GLYPH8_BOTTOM_RIGHT))
#define GLYPH8_LEFT            ((Glyph8)(GLYPH8_TOP_LEFT|GLYPH8_BOTTOM_LEFT))
#define GLYPH8_RIGHT           ((Glyph8)(GLYPH8_TOP_RIGHT|GLYPH8_BOTTOM_RIGHT))

#define GLYPH16_TOP_LEFT       GLYPH16(0033)
#define GLYPH16_TOP_RIGHT      GLYPH16(00cc)
#define GLYPH16_BOTTOM_LEFT    GLYPH16(3300)
#define GLYPH16_BOTTOM_RIGHT   GLYPH16(cc00)
#define GLYPH16_FULL           GLYPH16(ffff)
#define GLYPH16_TOP            ((Glyph16)(GLYPH16_TOP_LEFT|GLYPH16_TOP_RIGHT))
#define GLYPH16_BOTTOM         ((Glyph16)(GLYPH16_BOTTOM_LEFT|GLYPH16_BOTTOM_RIGHT))
#define GLYPH16_LEFT           ((Glyph16)(GLYPH16_TOP_LEFT|GLYPH16_BOTTOM_LEFT))
#define GLYPH16_RIGHT          ((Glyph16)(GLYPH16_TOP_RIGHT|GLYPH16_BOTTOM_RIGHT))

#define GLYPH32_TOP_LEFT       GLYPH32(00003333)
#define GLYPH32_TOP_RIGHT      GLYPH32(0000cccc)
#define GLYPH32_BOTTOM_LEFT    GLYPH32(33330000)
#define GLYPH32_BOTTOM_RIGHT   GLYPH32(cccc0000)
#define GLYPH32_FULL           GLYPH32(ffffffff)
#define GLYPH32_TOP            ((Glyph32)(GLYPH32_TOP_LEFT|GLYPH32_TOP_RIGHT))
#define GLYPH32_BOTTOM         ((Glyph32)(GLYPH32_BOTTOM_LEFT|GLYPH32_BOTTOM_RIGHT))
#define GLYPH32_LEFT           ((Glyph32)(GLYPH32_TOP_LEFT|GLYPH32_BOTTOM_LEFT))
#define GLYPH32_RIGHT          ((Glyph32)(GLYPH32_TOP_RIGHT|GLYPH32_BOTTOM_RIGHT))

#define GLYPH64_TOP_LEFT       GLYPH64(000000000f0f0f0f)
#define GLYPH64_TOP_RIGHT      GLYPH64(00000000f0f0f0f0)
#define GLYPH64_BOTTOM_LEFT    GLYPH64(0f0f0f0f00000000)
#define GLYPH64_BOTTOM_RIGHT   GLYPH64(f0f0f0f000000000)
#define GLYPH64_FULL           GLYPH64(ffffffffffffffff)
#define GLYPH64_TOP            ((Glyph64)(GLYPH64_TOP_LEFT|GLYPH64_TOP_RIGHT))
#define GLYPH64_BOTTOM         ((Glyph64)(GLYPH64_BOTTOM_LEFT|GLYPH64_BOTTOM_RIGHT))
#define GLYPH64_LEFT           ((Glyph64)(GLYPH64_TOP_LEFT|GLYPH64_BOTTOM_LEFT))
#define GLYPH64_RIGHT          ((Glyph64)(GLYPH64_TOP_RIGHT|GLYPH64_BOTTOM_RIGHT))

#define GLYPH128_TOP_LEFT      ((Glyph128){.top    = GLYPH64_LEFT})
#define GLYPH128_TOP_RIGHT     ((Glyph128){.top    = GLYPH64_RIGHT})
#define GLYPH128_TOP           ((Glyph128){.top    = GLYPH64_FULL})
#define GLYPH128_BOTTOM_LEFT   ((Glyph128){.bottom = GLYPH64_LEFT})
#define GLYPH128_BOTTOM_RIGHT  ((Glyph128){.bottom = GLYPH64_RIGHT})
#define GLYPH128_BOTTOM        ((Glyph128){.bottom = GLYPH64_FULL})
#define GLYPH128_LEFT          ((Glyph128){.top = GLYPH64_LEFT , .bottom = GLYPH64_LEFT})
#define GLYPH128_RIGHT         ((Glyph128){.top = GLYPH64_RIGHT, .bottom = GLYPH64_RIGHT})
#define GLYPH128_FULL          ((Glyph128){.top = GLYPH64_FULL , .bottom = GLYPH64_FULL})

#define GLYPH256_TOP_LEFT      ((Glyph256){.top_left     = GLYPH64_FULL})
#define GLYPH256_TOP_RIGHT     ((Glyph256){.top_right    = GLYPH64_FULL})
#define GLYPH256_BOTTOM_LEFT   ((Glyph256){.bottom_left  = GLYPH64_FULL})
#define GLYPH256_BOTTOM_RIGHT  ((Glyph256){.bottom_right = GLYPH64_FULL})
#define GLYPH256_TOP           ((Glyph256){.top_left     = GLYPH64_FULL, .top_right    = GLYPH64_FULL})
#define GLYPH256_BOTTOM        ((Glyph256){.bottom_left  = GLYPH64_FULL, .bottom_right = GLYPH64_FULL})
#define GLYPH256_LEFT          ((Glyph256){.top_left     = GLYPH64_FULL, .bottom_left  = GLYPH64_FULL})
#define GLYPH256_RIGHT         ((Glyph256){.top_right    = GLYPH64_FULL, .bottom_right = GLYPH64_FULL})
#define GLYPH256_FULL          ((Glyph256){.top_left     = GLYPH64_FULL, .top_right    = GLYPH64_FULL,\
                                           .bottom_left  = GLYPH64_FULL, .bottom_right = GLYPH64_FULL})

//------------------------------------------------------------------------------
// Doubling and Halving Glyphs
//------------------------------------------------------------------------------

// Extract a glyph of half size.
// The offset controls at which line or column we start.
// uint32_t offset 0-->4
// uint64_t offset 0-->4
#define GlyphHalf(glyph, offset)      \
        _Generic((glyph)            , \
           uint16_t: GlyphHalf_16_  , \
           uint32_t: GlyphHalf_32_  , \
           uint64_t: GlyphHalf_64_  , \
           Glyph128: GlyphHalf_128_ , \
           Glyph256: GlyphHalf_256_   \
        )((glyph), (offset))
        static inline uint8_t  GlyphHalf_16_(uint16_t glyph, int offset) {
           assert(offset >= 0 && offset <= (GLYPH16_WIDTH / 2));
           unsigned half = 0;
           // Extract every second 2bit chunck from the glyph
           for (int i = 0; i < 4; i++) {
              uint32_t nibble = (glyph >> (i * 4 + offset)) & 0x3u;
              // Place the extracted nibble in the correct position in the result
              half |= nibble << (i * 2);
           }
           return half;
        }
        static inline uint16_t GlyphHalf_32_(uint32_t glyph, int offset) {
           assert(offset >= 0 && offset <= (GLYPH32_HEIGHT / 2));
           return (uint16_t) GlyphShiftUp_32_(glyph, offset);
        }
        static inline uint32_t GlyphHalf_64_(uint64_t glyph, int offset) {
           assert(offset >= 0 && offset <= (GLYPH64_WIDTH / 2));
           uint32_t half = 0;
           // Extract every second nibble from the glyph
           for (int i = 0; i < 8; i++) {
              // Shift right by 8 bits each iteration to move to the next pair of nibbles
              // Use bitwise AND with 0xF0 to extract the second nibble
              uint32_t nibble = (glyph >> (i * 8 + offset)) & 0xFu;
              // Place the extracted nibble in the correct position in the result
              half |= nibble << (i * 4);
           }
           return half;
        }
        static inline uint64_t GlyphHalf_128_(Glyph128 glyph, int offset) {
           assert(offset >= 0 && offset <= (GLYPH128_HEIGHT / 2));
           return GlyphShiftUp_128_(glyph, offset).top;
        }
        static inline Glyph128 GlyphHalf_256_(Glyph256 glyph, int offset) {
           assert(offset >= 0 && offset <= (GLYPH256_WIDTH / 2));
// TODO / FIXME: uncomment -->   glyph = GlyphShiftLeft_256_(glyph, offset);
           return (Glyph128){glyph.top_left, glyph.bottom_left};
        }

// Return the given glyph with both halves swapped
#define GlyphSwapHalves(glyph)              \
        _Generic((glyph)                  , \
           uint8_t:  GlyphSwapHalves_8_   , \
           uint16_t: GlyphSwapHalves_16_  , \
           uint32_t: GlyphSwapHalves_32_  , \
           uint64_t: GlyphSwapHalves_64_  , \
           Glyph128: GlyphSwapHalves_128_ , \
           Glyph256: GlyphSwapHalves_256_   \
        )((glyph))
        static inline uint8_t GlyphSwapHalves_8_(uint8_t g)
        { return (g << 4) | (g >> 4); }
        static inline uint16_t GlyphSwapHalves_16_(uint16_t g)
        { return ((g & 0xccccU) >> 2) | ((g & 0x3333U) << 2); }
        static inline uint32_t GlyphSwapHalves_32_(uint32_t g)
        { return (g << 16) | (g >> 16); }
        static inline uint64_t GlyphSwapHalves_64_(uint64_t g)
        { return ((g & 0xf0f0f0f0f0f0f0f0U) >> 4) | ((g & 0x0f0f0f0f0f0f0f0fU) << 4); }
        static inline Glyph128 GlyphSwapHalves_128_(Glyph128 g)
        { return (Glyph128){ g.bottom, g.top }; }
        static inline Glyph256 GlyphSwapHalves_256_(Glyph256 g)
        { return (Glyph256){ g.top_right   , g.top_left,
                             g.bottom_right, g.bottom_left }; }

// Create a glyph from two glyphs of half size (must be of the same type)
// The two halves will be arranged either horizontally or vertically
// depending on the size.
#define GlyphFromHalves(left_or_top, right_or_bottom) \
        _Generic((left_or_top)                      , \
           uint8_t:  GlyphFromHalves_8_             , \
           uint16_t: GlyphFromHalves_16_            , \
           uint32_t: GlyphFromHalves_32_            , \
           uint64_t: GlyphFromHalves_64_            , \
           Glyph128: GlyphFromHalves_128_             \
        )((left_or_top), (right_or_bottom))
        static inline uint16_t GlyphFromHalves_8_1_(uint8_t half)
        { // Return a glyph16 from a glyph8 placed on the left side.
          // (Hint: << 2 to place it on the right instead)

          // Interleave the byte's bits value xxxxxxxx as xx00xx00xx00xx00
          unsigned g = half;
          g  =   (g | (g << 4)) & 0x0F0Fu;
          return (g | (g << 2)) & 0x3333u;
        }
        static inline uint16_t GlyphFromHalves_8_(uint8_t l, uint8_t r)
        { return GlyphFromHalves_8_1_(l) | (GlyphFromHalves_8_1_(r) << 2); }
        static inline uint32_t GlyphFromHalves_16_(uint16_t t, uint16_t b)
        { return (uint32_t)t | ((uint32_t)b << 16); }
        static inline uint64_t GlyphFromHalves_32_1_(uint32_t half)
        { // Return a glyph64 from a glyp32 placed on the left side.
          // (Hint: << 4 to place it on the right instead)

          // This interleaves the nibbles: nnnnnnnn -> n0n0n0n0n0n0n0n0
          uint64_t g = half;
          g  =   (g | (g << 16)) & 0x0000FFFF0000FFFFu;
          g  =   (g | (g <<  8)) & 0x00FF00FF00FF00FFu;
          return (g | (g <<  4)) & 0x0F0F0F0F0F0F0F0Fu;
        }
        static inline uint64_t GlyphFromHalves_32_(uint32_t l, uint32_t r)
        { return GlyphFromHalves_32_1_(l) | (GlyphFromHalves_32_1_(r) << 4); }
        static inline Glyph128 GlyphFromHalves_64_(uint64_t t, uint64_t b)
        { return (Glyph128){ t, b }; }
        static inline Glyph256 GlyphFromHalves_128_(Glyph128 l, Glyph128 r)
        { return (Glyph256){ l.top, r.top, l.bottom, r.bottom }; }


// Return a bigger glyph based on the given glyph. This will double up the size
// of each pixel either horizontally (for example for Glyph16 -> Glyph32) or
// vertically (for example for Glyph32->Glyph64).
// Note: It works with Glyph8-128 but Glyph256s cannot be upscaled.
#define GlyphUpscale(glyph)             \
        _Generic((glyph)              , \
           uint8_t:  GlyphUpscale_8_  , \
           uint16_t: GlyphUpscale_16_ , \
           uint32_t: GlyphUpscale_32_ , \
           uint64_t: GlyphUpscale_64_ , \
           Glyph128: GlyphUpscale_128_  \
        )((glyph))
        static inline uint16_t GlyphUpscale_8_(uint8_t g) {
           // interleave bits of g with itself, use the 64-bit multiplication
           // technique described at:
           // https://graphics.stanford.edu/~seander/bithacks.html#Interleave64bitOps
           uint64_t t = (g * 0x0101010101010101u & 0x8040201008040201u) * 0x0102040810204081u;
           return ((t >> 49) & 0x5555u) | ((t >> 48) & 0xAAAAu);
        }
        static inline uint32_t GlyphUpscale_16_(uint16_t g) {
           // Extract each line (nibble) from the glyph
           uint32_t i = (g >> 12) & 0xF;
           uint32_t j = (g >> 8)  & 0xF;
           uint32_t k = (g >> 4)  & 0xF;
           uint32_t l =  g        & 0xF;
           // Construct the 32-bit result with doubled nibbles: iijjkkll
           return (i<<28)|(i<<24) | (j<<20)|(j<<16) | (k<<12)|(k<<8) | (l<<4)|l;
        }
/* nope
        static inline uint64_t GlyphUpscale_32_(uint32_t g)
        {
           // Interleave the glyph bytes with itself using this technique:
           // https://graphics.stanford.edu/~seander/bithacks.html#InterleaveBMN
           g = (g | (g << 8)) & 0x00FF00FFu;
           g = (g | (g << 4)) & 0x0F0F0F0Fu;
           g = (g | (g << 2)) & 0x33333333u;
           g = (g | (g << 1)) & 0x55555555u;
           return g | (g << 1);
        }
*/
        static inline uint64_t GlyphUpscale_32_(uint32_t g)
        {
/* No?:
           uint64_t z = 0;
           for (int i = 0; i < 32; i++) {
              uint64_t t = (g & 1U << i) << i;
              z |= t | (t << 1);
           }
           return z;
*/

/* OK:
           uint64_t g64 = 0;
           for (int i = 0; i < 32; i++) {
              // Extract the i-th bit from g
              uint64_t bit = (g >> i) & 1;
              // Place bit it in the position (2*i) and (2*i+1)
              g64 |= bit << (2 * i) | bit << (2 * i + 1);
           }
           return g64;
*/


           uint64_t g64 = 0;
           for (int i = 0; i < 32; i++) {
              // Extract the i-th bit from g
              uint64_t bit = (g >> i) & 1;
              // Place bit it in the position (2*i) and (2*i+1)
              g64 |= bit << (2 * i) | bit << (2 * i + 1);
           }
           return g64;

        }
        static inline Glyph128 GlyphUpscale_64_(uint64_t g) {
           // Extract each line (byte) from half of the glyph and start to
           // construct the Glyph128
           Glyph128 g128;
           uint64_t i = (g >> 56) & 0xFF;
           uint64_t j = (g >> 48) & 0xFF;
           uint64_t k = (g >> 40) & 0xFF;
           uint64_t l = (g >> 32) & 0xFF;
           g128.bottom = (i<<56)|(i<<48) | (j<<40)|(j<<32) | (k<<24)|(k<<16) | (l<<8)|l;

           // Same for the other half
           i = (g >> 24) & 0xFF;
           j = (g >> 16) & 0xFF;
           k = (g >>  8) & 0xFF;
           l =  g        & 0xFF;
           g128.top = (i<<56)|(i<<48) | (j<<40)|(j<<32) | (k<<24)|(k<<16) | (l<<8)|l;
           return g128;
        }
        static inline Glyph256 GlyphUpscale_128_(Glyph128 g) {
           return (Glyph256){
              .top_left     = GlyphUpscale_32_(GlyphHalf_64_(g.top, 0)),
              .top_right    = GlyphUpscale_32_(GlyphHalf_64_(g.top, 4)),
              .bottom_left  = GlyphUpscale_32_(GlyphHalf_64_(g.bottom, 0)),
              .bottom_right = GlyphUpscale_32_(GlyphHalf_64_(g.bottom, 4)),
           };
        }

#if 0
            GlyphHalf(g.top, 4);


// for top
// extract left half -> top_left become the upscale32->64 of it
// extract right half -> top_right become the upscale32->64 of it
// same for bottom


           Glyph256 g256 = {0};
/*
           int i = 0, j = 0;
           for (int i = 0; i < 64; i++) {
              if (j > 4) { j = 0;  }
              // Extract the i-th bit from g
              uint64_t bit = (g.top >> i) & 1;
              // Place bit it in the position (2*i) and (2*i+1)
              g256.top_left |= bit << (2 * i) | bit << (2 * i + 1);
           }
           for (int i = 32; i < 64; i++) {
              // Extract the i-th bit from g
              uint64_t bit = (g.top >> i) & 1;
              // Place bit it in the position (2*i) and (2*i+1)
              g256.top_right |= bit << (2 * i) | bit << (2 * i + 1);
*/

/*
           for (int i = 0; i < 32; i++) {
              // Extract the i-th bit from g
              uint64_t bit = (g.top >> i) & 1;
              // Place bit it in the position (2*i) and (2*i+1)
              g256.top_left |= bit << (2 * i) | bit << (2 * i + 1);
           }
           for (int i = 32; i < 64; i++) {
              // Extract the i-th bit from g
              uint64_t bit = (g.top >> i) & 1;
              // Place bit it in the position (2*i) and (2*i+1)
              g256.top_right |= bit << (2 * i) | bit << (2 * i + 1);
           }
*/

           for (int i = 0; i < 32; i++) {
              // Extract the i-th bit from g
              uint64_t bit = (g.bottom >> i) & 1;
              // Place bit it in the position (2*i) and (2*i+1)
              g256.bottom_left |= bit << (2 * i) | bit << (2 * i + 1);
           }
           for (int i = 32; i < 64; i++) {
              // Extract the i-th bit from g
              uint64_t bit = (g.bottom >> i) & 1;
              // Place bit it in the position (2*i) and (2*i+1)
              g256.bottom_right |= bit << (2 * i) | bit << (2 * i + 1);
           }
           return g256;
        }
#endif



#if 0
// Construct a Glyph containing a single pixel (x,y)
static inline Glyph8    Glyph8FromPixel(int x, int y)
{  assert(x >= 0);  assert(x < GLYPH8_WIDTH);
   assert(y >= 0);  assert(y < GLYPH8_HEIGHT);
   return 1U << (x + y << GLYPH8_WIDTH_LOG2); }
static inline Glyph16   Glyph16FromPixel(int x, int y)
{  assert(x >= 0);  assert(x < GLYPH16_WIDTH);
   assert(y >= 0);  assert(y < GLYPH16_HEIGHT);
   return 1U << (x + y << GLYPH16_WIDTH_LOG2); }
static inline Glyph32   Glyph32FromPixel(int x, int y)
{  assert(x >= 0);  assert(x < GLYPH32_WIDTH);
   assert(y >= 0);  assert(y < GLYPH32_HEIGHT);
   return UINT32_C(1) << (x + y << GLYPH32_WIDTH_LOG2); }
static inline Glyph64   Glyph64FromPixel(int x, int y)
{  assert(x >= 0);  assert(x < GLYPH64_WIDTH);
   assert(y >= 0);  assert(y < GLYPH64_HEIGHT);
   return UINT64_C(1) << (x + y << GLYPH64_WIDTH_LOG2); }
static inline Glyph128  Glyph128FromPixel(int x, int y)
{  assert(x >= 0);  assert(x < GLYPH128_WIDTH);
   assert(y >= 0);  assert(y < GLYPH128_HEIGHT);
   return (y < GLYPH64_HEIGHT) ?
          (Glyph128) {.top    = Glyph64FromPixel(x, y)} :
          (Glyph128) {.bottom = Glyph64FromPixel(x, y - GLYPH64_HEIGHT)} ;}
static inline Glyph128  Glyph256FromPixel(int x, int y)
{  assert(x >= 0);  assert(x < GLYPH256_WIDTH);
   assert(y >= 0);  assert(y < GLYPH256_HEIGHT);
   if (x < GLYPH64_WIDTH) {
      return (y < GLYPH64_HEIGHT) ?
             (Glyph256) {.topLeft     = Glyph64FromPixel(x, y)} :
             (Glyph256) {.bottomLeft  = Glyph64FromPixel(x, y - GLYPH64_HEIGHT)};
   } else {
      return (y < GLYPH64_HEIGHT) ?
             (Glyph256) {.topRight    = Glyph64FromPixel(x - GLYPH64_WIDTH, y)} :
             (Glyph256) {.bottomRight = Glyph64FromPixel(x - GLYPH64_WIDTH, y - GLYPH64_HEIGHT)};
   }
}

/// @brief return the value (0 or 1) of pixel (x,y) in the given quadrant
#define GlyphPixelAt(glyph, x,y)


/// @brief return the value (0 or 1) of pixel (x,y) in the given quadrant
#define QuadrantPixelAt(quadrant, x,y)  uint_bitAt((quadrant), GLYPH16_INDEX((x), (y)))
/// @brief return the value (0 or 1) of pixel (x,y) in the given half
#define HalfPixelAt(half, x,y)          uint_bitAt((half), GLYPH32_INDEX((x), (y)))
/// @brief return the value (0 or 1) of pixel (x,y) in the given glyph
#define GlyphPixelAt(glyph, x,y)        uint_bitAt((glyph), GLYPH64_INDEX((x), (y)))


/* just some compile-time basic tests */
static_assert(QuadrantPixelAt(quadrant_fromPixel(2,3), 2,3) == 1);
static_assert(QuadrantPixelAt(quadrant_fromPixel(0,1), 2,3) == 0);
static_assert(HalfPixelAt(half_fromPixel(2,6), 2,6) == 1);
static_assert(HalfPixelAt(half_fromPixel(1,2), 3,6) == 0);
static_assert(GlyphPixelAt(glyph_fromPixel(6,7), 6,7) == 1);
static_assert(GlyphPixelAt(glyph_fromPixel(2,3), 4,5) == 0);
#endif


#if 0
//------------------------------------------------------------------------------
// glyph quadrants and eighth lines
//------------------------------------------------------------------------------

static inline uint64_t glyph_nLinesTop(unsigned n)
{ uint64_t x = GLYPH(1) << ((n << GLYPH64_WIDTH_LOG2) - 1); return x | (x - 1); }
// x | (x - 1) is a trick to right-propagate the left-most bit  ---^

static inline uint64_t glyph_nLinesBottom(unsigned n)
{ return ~glyph_nLinesTop(8 - n); }

static inline uint64_t glyph_nLinesLeft(unsigned n)
{  // there might be a way to do this faster
   uint64_t g = 0;
   for(unsigned i = 0; i < n; i++)
      g |= GLYPH(0101010101010101) << i;
   return g;
}

static inline uint64_t glyph_nLinesRight(unsigned n)
{ return ~glyph_nLinesLeft(8 - n); }

//------------------------------------------------------------------------------

// glyph measurement (return a value [0-8])
static inline  int       glyph_marginTop   (uint64_t glyph); ///< # of upper empty lines
static inline  int       glyph_marginBottom(uint64_t glyph); ///< # of lower empty lines
static inline  int       glyph_marginLeft  (uint64_t glyph); ///< # of left empty columns
static inline  int       glyph_marginRight (uint64_t glyph); ///< # of right empty columns
//TODO: maybe name height and width are confusing,
//      those are not GLYPH_WIDTH and GLYPH_HEIGHT
static inline  int       glyph_height      (uint64_t glyph); // height of a glyph
static inline  int       glyph_width       (uint64_t glyph); // width of a glyph
// notes: - the coordinates of the upper-left non-empty pixel of a glyph is:
//          (glyph_marginLeft(glyph), glyph_marginTop(glyph))
//        - so sometimes, you might be interested in "8 - ...margin..." instead
//          of the true width/height of a glyph.

static inline int glyph_marginTop(uint64_t glyph)
{ return (glyph)? (uint64_ctz(glyph) / GLYPH64_HEIGHT) : GLYPH64_HEIGHT; }

static inline int glyph_marginBottom(uint64_t glyph)
{ return (glyph)? (uint64_clz(glyph) / GLYPH64_HEIGHT) : GLYPH64_HEIGHT; }

static inline int glyph_height(uint64_t glyph)
{ return (glyph)? (GLYPH64_HEIGHT - (uint64_clz(glyph) / GLYPH64_HEIGHT) - (uint64_ctz(glyph) / GLYPH64_HEIGHT)) : 0; }

static inline int glyph_marginLeft(uint64_t glyph)
{ return (glyph)? uint8_ctz(glyph_flatten(glyph)) : 0; }

static inline int glyph_marginRight(uint64_t glyph)
{ return (glyph)? uint8_clz(glyph_flatten(glyph)) : 0; }

static inline int glyph_width(uint64_t glyph)
{
   if (glyph) {
      unsigned char line = glyph_flatten(glyph);
      return GLYPH64_WIDTH - uint8_clz(line) - uint8_ctz(line);
   } else {
      return 0;
   }
}
#endif

#endif //KONPU_GLYPH_H
