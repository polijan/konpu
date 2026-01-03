#ifndef  KONPU_GLYPH_H_
#define  KONPU_GLYPH_H_
#include "video_mode.h"
#include "rectangle.h" //TODO: probably add this include into video_mode.h
#include "bits.h"


//------------------------------------------------------------------------------
// Glyphs Types
//------------------------------------------------------------------------------

//  Glyph terminology:                   Glyph8      Glyph32         Glyph64
//  ------------------                     . .       . . . .     . . . . . . . .
//-------------------------.---------.     . .       . . . .     . . . . . . . .
//              C Type     |Dimension|     . .       . . . .     . . . . . . . .
//-------------------------|---------|     . .       . . . .     . . . . . . . .
typedef Form8   Glyph8;   //  2 x 4  |               . . . .     . . . . . . . .
typedef Form16  Glyph16;  //  4 x 4  |   . . . .     . . . .     . . . . . . . .
typedef Form32  Glyph32;  //  4 x 8  |   . . . .     . . . .     . . . . . . . .
typedef Form64  Glyph64;  //  8 x 8  |   . . . .     . . . .     . . . . . . . .
typedef Form128 Glyph128; //  8 x 16 |   . . . .
typedef Form256 Glyph256; // 16 x 16 |   Glyph16     Glyph128 and Glyph256 are
//-------------------------'---------'               structs made of Glyph64s:
//
//                                         .top        .top_left | .top_right
//                                         ------    ------------|-------------
//                                        .bottom   .bottom_left | .bottom_right

// Generic Glyph, TODO: remove, or support, etc.
typedef void Glyph;

// Glyphs' Dimension can be PIXEL_<2x4|4x4|4x8|8x8|8x16|16x16>
#define GLYPH_DIMENSION   VideoModeDimension()

// Glyph Type: the number of pixels in the glyph.
// It can be <8|16|32|64|128|256>
#define GLYPH_TYPE        (8 << GLYPH_DIMENSION)

//------------------------------------------------------------------------------
// Glyph Operations
//------------------------------------------------------------------------------
//
// // Measurement macros: pixels Width and Height, byte Size, and more
// #define GLYPH[N]_<WIDTH|HEIGHT|SIZE|COUNT_...>[_LOG2]    ...
//
// // Get/Set Glyphs from/to Surface (Surface can be a Rectangle, Video, or can
// // be omitted (in which case it refers to the Video.active rectangle)
// void GlyphClear([Surface s]);
// void GlyphSetAll([Surface s], Glyph g);
// GlyphGet(N, [Surface s], int x,y, [int plane])
//
// // Construct Glyph<N> from literals
// #define GLYPH<N>(...)
//
// // "Real" Measurements:
// int GlyphWidth(glyph)
// int GlyphHeight(glyph)
// int GlyphWidthLog2(glyph)
// // int GlyphHeightLog2(glyph)
// int GlyphRealHeight(glyph)
// int GlyphRealWidth(glyph)
// int GlyphMargin<Left|Right|Top|Bottom>(g)
//
// // Bitwise Operations (==, Not, And, Or, Xor):
// bool   GlyphIsEqual(Glyph glyph1, Glyph glyph2);  // Compare two glyphs
// Glyph  GlyphNot(Glyph glyph);
// Glyph  Glyph<And|Or|Xor>(Glyph glyph1, Glyph glyph2);
//
// // Shifts and Cycles:
// Glyph  GlyphShift<Up|Down|Left|Right>(Glyph glyph, int n); => TODO
// Glyph  GlyphCycle<Up|Down|Left|Right>(Glyph glyph, int n); => TODO
//
// // Flip, Mirror, Rotate, Transpose:
// Glyph  GlyphFlip(Glyph glyph);
// Glyph  GlyphMirror(Glyph glyph);
// Glyph  GlyphTranspose(Glyph glyph);
// Glyph  GlyphRotate<90|180|270>(Glyph glyph);
//
// // Access to Lines:
// unsigned  GlyphGetLine(Glyph glyph, int8_t nth);
// Glyph     GlyphSetLine(Glyph glyph, int8_t nth, unsigned line);
// unsigned  GlyphFlatten(Glyph glyph);
// Glyph<N>  GlyphLines<Left|Right|Top|Bottom>(N, int n);
//
// // Access to Pixels within Glyphs:
// Glyph<N> GlyphFromPixel(N, x,y);
// int      GlyphGetPixel(Glyph glyph, int x,y);
// Glyph    GlyphSetPixel(Glyph glyph, int x,y, unsigned bit);
// bool?    GlyphPixelIsSet(Glyph glyph, int x,y);
// Glyph    GlyphPixelToggle(Glyph glyph, int x,y);
//
// // Half/ Doubling glyphs
// glyph GlyphHalf(glyph, int offset)
// glyph GlyphSwapHalves(glyph)
// glyph GlyphFromHalves(glyph left_or_top, glyph right_or_bottom)
// glyph GlyphUpscale(glyph)
//
// // Access to SubGlyphs ???
// Glyph  GlyphGetGlyph(Glyph dst_glyph, int x,y, GlyphType);
// Glyph  GlyphSetGlyph(Glyph dst_glyph, int x,y, Glyph smaller_glyph);
//
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Glyph measurements: Simple macros giving Glyphs' Width and Height (pixels),
// Size (in bytes), the numbers of Bit (ie = count of pixels), as well as the
// Log2 of those values.
// The macro are literal constants for defined Glyph<N> types, or otherwise
// simple operations on the video mode's dimension when N isn't specified.
//------------------------------------------------------------------------------

#define GLYPH_SIZE                  (1 << GLYPH_DIMENSION)
#define GLYPH_WIDTH                 (2 << ((GLYPH_DIMENSION + 1) >> 1))
#define GLYPH_HEIGHT                (4 << ((VIDEO_MODE >> 5) & 3)) /*see: note*/
#define GLYPH_COUNT_BITS            (8 << GLYPH_DIMENSION)
#define GLYPH_COUNT_PIXELS          (8 << GLYPH_DIMENSION)
#define GLYPH_COUNT_COLOR           2

#define GLYPH_SIZE_LOG2             GLYPH_DIMENSION
#define GLYPH_WIDTH_LOG2            ((GLYPH_DIMENSION + 3) >> 1)
#define GLYPH_HEIGHT_LOG2           (2 + ((VIDEO_MODE >> 5) & 3))  /*see: note*/
#define GLYPH_COUNT_BITS_LOG2       (GLYPH_DIMENSION + 3)
#define GLYPH_COUNT_PIXELS_LOG2     (GLYPH_DIMENSION + 3)
#define GLYPH_COUNT_COLOR_LOG2      1

// ^-- Implementation note about the macros above:
// As noted in "video.h", the Video Element formulas for Height and its log2 are
// Height := 4 << (Dimension>>1) and log2(Height) := 2 + (Dimension>>1)
// For Glyphs, Dimension is the VIDEO_MODE bits 5/6/7, thus we simplified.


//------------------------------------------------------------------------------
// Glyph measurement macros become constants when the Glyph type is known
// Glyph<8|16|32|64|128|256>
//------------------------------------------------------------------------------

#define GLYPH8_SIZE                 1
#define GLYPH8_WIDTH                2
#define GLYPH8_HEIGHT               4
#define GLYPH8_COUNT_BITS           8
#define GLYPH8_COUNT_PIXELS         8
#define GLYPH8_COUNT_COLOR          2
#define GLYPH8_SIZE_LOG2            0
#define GLYPH8_WIDTH_LOG2           1
#define GLYPH8_HEIGHT_LOG2          2
#define GLYPH8_COUNT_BITS_LOG2      3
#define GLYPH8_COUNT_PIXELS_LOG2    3
#define GLYPH8_COUNT_COLOR_LOG2     1

#define GLYPH16_SIZE                2
#define GLYPH16_WIDTH               4
#define GLYPH16_HEIGHT              4
#define GLYPH16_COUNT_BITS          16
#define GLYPH16_COUNT_PIXELS        16
#define GLYPH16_COUNT_COLOR         2
#define GLYPH16_SIZE_LOG2           1
#define GLYPH16_WIDTH_LOG2          2
#define GLYPH16_HEIGHT_LOG2         2
#define GLYPH16_COUNT_BITS_LOG2     4
#define GLYPH16_COUNT_PIXELS_LOG2   4
#define GLYPH16_COUNT_COLOR_LOG2    1

#define GLYPH32_SIZE                4
#define GLYPH32_WIDTH               4
#define GLYPH32_HEIGHT              8
#define GLYPH32_COUNT_BITS          32
#define GLYPH32_COUNT_PIXELS        32
#define GLYPH32_COUNT_COLOR         2
#define GLYPH32_SIZE_LOG2           2
#define GLYPH32_WIDTH_LOG2          2
#define GLYPH32_HEIGHT_LOG2         3
#define GLYPH32_COUNT_BITS_LOG2     5
#define GLYPH32_COUNT_PIXELS_LOG2   5
#define GLYPH32_COUNT_COLOR_LOG2    1

#define GLYPH64_SIZE                8
#define GLYPH64_WIDTH               8
#define GLYPH64_HEIGHT              8
#define GLYPH64_COUNT_BITS          64
#define GLYPH64_COUNT_PIXELS        64
#define GLYPH64_COUNT_COLOR         2
#define GLYPH64_SIZE_LOG2           3
#define GLYPH64_WIDTH_LOG2          3
#define GLYPH64_HEIGHT_LOG2         3
#define GLYPH64_COUNT_BITS_LOG2     6
#define GLYPH64_COUNT_PIXELS_LOG2   6
#define GLYPH64_COUNT_COLOR_LOG2    1

#define GLYPH128_SIZE               16
#define GLYPH128_WIDTH              8
#define GLYPH128_HEIGHT             16
#define GLYPH128_COUNT_BITS         128
#define GLYPH128_COUNT_PIXELS       128
#define GLYPH128_COUNT_COLOR        2
#define GLYPH128_SIZE_LOG2          4
#define GLYPH128_WIDTH_LOG2         3
#define GLYPH128_HEIGHT_LOG2        4
#define GLYPH128_COUNT_BITS_LOG2    7
#define GLYPH128_COUNT_PIXELS_LOG2  7
#define GLYPH128_COUNT_COLOR_LOG2   1

#define GLYPH256_SIZE               32
#define GLYPH256_WIDTH              16
#define GLYPH256_HEIGHT             16
#define GLYPH256_COUNT_BITS         256
#define GLYPH256_COUNT_PIXELS       256
#define GLYPH256_COUNT_COLOR        2
#define GLYPH256_SIZE_LOG2          5
#define GLYPH256_WIDTH_LOG2         4
#define GLYPH256_HEIGHT_LOG2        4
#define GLYPH256_COUNT_BITS_LOG2    8
#define GLYPH256_COUNT_PIXELS_LOG2  8
#define GLYPH256_COUNT_COLOR_LOG2   1


//------------------------------------------------------------------------------

// _Generic statement taylored for Glyphs
// It selects `F<N>_` based on which Glyph<N> type a `glyph` really is.
// Extra code may also be inserted in the _Generic via the ... parameter(s).
/* RE-ENABLE THIS
#define GLYPH_Generic(glyph, F, ...)   \
   _Generic((glyph)                 ,  \
      Glyph8:   F##8_               ,  \
      Glyph16:  F##16_              ,  \
      Glyph32:  F##32_              ,  \
      Glyph64:  F##64_              ,  \
      Glyph128: F##128_             ,  \
      Glyph256: F##256_  __VA_OPT__(,) \
      __VA_ARGS__                      \
   )
*/
#define GLYPH_Generic(glyph, F)   \
   _Generic((glyph)            ,  \
      Glyph8:   F##8_          ,  \
      Glyph16:  F##16_         ,  \
      Glyph32:  F##32_         ,  \
      Glyph64:  F##64_         ,  \
      Glyph128: F##128_        ,  \
      Glyph256: F##256_           \
   )


// void GlyphSetAll([Video|Rectangle r], Glyph<N> glyph);
// TODO: void GlyphSetAll([Video|Rectangle r], Glyph<N> glyph, [int plane]);

// Set all the Glyph in the given or active surface to the given ones
#define GlyphSetAll(...) \
   UTIL_OVERLOAD(PrivateGlyphSetAll, __VA_ARGS__)
#  define PrivateGlyphSetAll_1_(g) \
      GLYPH_Generic((g), PrivateGlyphSetAll_Rectangle_)(Video.active, (g))
#  define PrivateGlyphSetAll_2_(surface, glyph)                                \
      _Generic((surface)                                                     , \
         VideoSurface_: GLYPH_Generic((glyph), PrivateGlyphSetAll_Video_) ((glyph)) , \
         default:       GLYPH_Generic((glyph), PrivateGlyphSetAll_Rectangle_) (RECTANGLE((surface)), (glyph)) \
      )
#  define PRIVATE_IMPLEMENTATION_(N)                                           \
      static inline void PrivateGlyphSetAll_Video_##N##_(Glyph##N g) {         \
         for (int i = 0; i < VIDEO_COUNT_GLYPH##N; i++)                        \
            Video.glyph##N[i] = g;                                             \
      }                                                                        \
      static inline void                                                       \
      PrivateGlyphSetAll_Rectangle_##N##_(Rectangle r, Glyph##N g) {           \
         r =   RectangleIntersection(r, (Rectangle){                           \
                  0,0 , VIDEO_WIDTH_GLYPH##N, VIDEO_HEIGHT_GLYPH##N            \
               });                                                             \
         VideoArray glyph = VIDEO_GLYPH(N, r.x0, r.y0);                        \
         for (int y = 0; y < r.height; y++)                                    \
            for (int x = 0; x < r.width; x++)                                  \
               glyph[y][x] = g;                                                \
      }
      PRIVATE_IMPLEMENTATION_(8)
      PRIVATE_IMPLEMENTATION_(16)
      PRIVATE_IMPLEMENTATION_(32)
      PRIVATE_IMPLEMENTATION_(64)
      PRIVATE_IMPLEMENTATION_(128)
      PRIVATE_IMPLEMENTATION_(256)
#  undef PRIVATE_IMPLEMENTATION_


// void GlyphClear([Video|Rectangle]);
// Set all the Glyph to 0 in the given or active surface
// TODO: void GlyphClear([Video|Rectangle r], [int plane]);
#define GlyphClear(...)            UTIL_OVERLOAD(PrivateGlyphClear, __VA_ARGS__)
#  define PrivateGlyphClear_0_()   PrivateGlyphClear_1_(Video.active)
#  define PrivateGlyphClear_1_(surface)                                        \
      _Generic((surface)                                                     , \
         VideoSurface_: memset(Video.frame, 0, VIDEO_GLYPHS_SIZE)            , \
         Rectangle:     PrivateGlyphClear_Rectangle_(RECTANGLE((surface)))     \
      )
   void PrivateGlyphClear_Rectangle_(Rectangle r);


//------------------------------------------------------------------------------
// GLYPH<N>(...): Construct a Glyph<N> from literals
//
// GLYPH<N> for N in {8,16,32,64}
// - one argument: give the hexadecimal values of the glyph
// - several arguments: give the values as binary literal, one value per line.
// - can be used as integral constants
//
// GLYPH128
// -  2 arguments: give the hex value of the two glyph64 (bottom, then up)
// - 16 arguments: give the values as binary literal, one value per line.
//
// GLYPH256
// - 32 arguments: give the values as binary literal, 2 values per line.
//------------------------------------------------------------------------------

#define GLYPH8(...)                           UTIL_OVERLOAD(GLYPH8, __VA_ARGS__)
#  define GLYPH8_1_(hex_lit)                          ((uint8_t) 0x##hex_lit##U)
#  define GLYPH8_4_(quarter1, quarter2, quarter3, quarter4)        ((uint8_t)( \
      BITS_REVERSED_1_(quarter4)      | BITS_REVERSED_1_(quarter3) >> 2 |      \
      BITS_REVERSED_1_(quarter2) >> 4 | BITS_REVERSED_1_(quarter1) >> 6 )      )

#define GLYPH16(...)                         UTIL_OVERLOAD(GLYPH16, __VA_ARGS__)
#  define GLYPH16_1_(hex_lit)                         ((uint16_t)0x##hex_lit##U)
#  define GLYPH16_4_(nibble1, nibble2, nibble3, nibble4)          ((uint16_t)( \
      BITS_REVERSED_1_(nibble4) << 8 | BITS_REVERSED_1_(nibble3) << 4 |        \
      BITS_REVERSED_1_(nibble2)      | BITS_REVERSED_1_(nibble1) >> 4 )        )

#define GLYPH32(...)                         UTIL_OVERLOAD(GLYPH32, __VA_ARGS__)
#  define GLYPH32_1_(hex_lit)                         ((uint32_t)0x##hex_lit##U)
#  define GLYPH32_8_(nibble1, nibble2, nibble3, nibble4,                       \
                     nibble5, nibble6, nibble7, nibble8)          ((uint32_t)( \
      (uint32_t)BITS_REVERSED_1_(nibble8) << 24 |                              \
      (uint32_t)BITS_REVERSED_1_(nibble7) << 20 |                              \
      (uint32_t)BITS_REVERSED_1_(nibble6) << 16 |                              \
      (uint32_t)BITS_REVERSED_1_(nibble5) << 12 |                              \
      BITS_REVERSED_1_(nibble4) <<  8 | BITS_REVERSED_1_(nibble3) <<  4 |      \
      BITS_REVERSED_1_(nibble2)       | BITS_REVERSED_1_(nibble1) >>  4 )      )

#define GLYPH64(...)                         UTIL_OVERLOAD(GLYPH64, __VA_ARGS__)
#  define GLYPH64_1_(hex_lit)                         ((uint64_t)0x##hex_lit##U)
#  define GLYPH64_8_(byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8)   \
      ((uint64_t) BITS_REVERSED_8_( byte1, byte2, byte3, byte4,                \
                                    byte5, byte6, byte7, byte8 ))

#define GLYPH128(...)                       UTIL_OVERLOAD(GLYPH128, __VA_ARGS__)
#  define GLYPH128_2_(hex_bottom_lit,hex_top_lit)                              \
      ((Glyph128){ 0x##hex_top_lit##U, 0x##hex_bottom_lit##U })
#  define GLYPH128_16_(byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8, \
                       byte9, byteA, byteB, byteC, byteD, byteE, byteF, byteG) \
      ((Glyph128){                                                             \
         GLYPH64_8_(byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8),   \
         GLYPH64_8_(byte9, byteA, byteB, byteC, byteD, byteE, byteF, byteG)    \
      })

#define GLYPH256( byteL1, byteR1, byteL2, byteR2, byteL3, byteR3, byteL4,      \
                  byteR4, byteL5, byteR5, byteL6, byteR6, byteL7, byteR7,      \
                  byteL8, byteR8, byteL9, byteR9, byteLA, byteRA, byteLB,      \
                  byteRB, byteLC, byteRC, byteLD, byteRD, byteLE, byteRE,      \
                  byteLF, byteRF, byteLG, byteRG )                             \
   ((Glyph256){                                                                \
      GLYPH64_8_(byteL1,byteL2,byteL3,byteL4,byteL5,byteL6,byteL7,byteL8),     \
      GLYPH64_8_(byteR1,byteR2,byteR3,byteR4,byteR5,byteR6,byteR7,byteR8),     \
      GLYPH64_8_(byteL9,byteLA,byteLB,byteLC,byteLD,byteLE,byteLF,byteLG),     \
      GLYPH64_8_(byteR9,byteRA,byteRB,byteRC,byteRD,byteRE,byteRF,byteRG)      \
   })

//------------------------------------------------------------------------------
// Bitwise operations (==, not, and, or, xor) are similar to the native C
// operators, but will ensure that return is also of the same type (no integer
// promotion). For ==/and/or/xor, both glyphs should be of the same type.
//------------------------------------------------------------------------------

// Return true iff glyph1 and glyph2 are the same glyph.
// Note: if the glyphs are not of the same type, the comparison returns false.
#define GlyphIsEqual(glyph1, glyph2)                              \
   (  (sizeof(glyph1) == sizeof(glyph2)) &&                       \
      GLYPH_Generic((glyph1), GlyphIsEqual_)((glyph1), (glyph2))  \
   )
   static inline bool GlyphIsEqual_8_(Glyph8 g, Glyph8  f)    {return g == f;}
   static inline bool GlyphIsEqual_16_(Glyph16 g, Glyph16 f)  {return g == f;}
   static inline bool GlyphIsEqual_32_(Glyph32 g, Glyph32 f)  {return g == f;}
   static inline bool GlyphIsEqual_64_(Glyph64 g, Glyph64 f)  {return g == f;}
   static inline bool GlyphIsEqual_128_(Glyph128 g, Glyph128 f)
   { return (g.top == f.top) && (g.bottom == f.bottom); }
   static inline bool GlyphIsEqual_256_(Glyph256 g, Glyph256 f) {
      return (g.top_left == f.top_left) && (g.top_right == f.top_right) &&
         (g.bottom_left == f.bottom_left) && (g.bottom_right == f.bottom_right);
   }

// Bitwise Not (i.e. inverse) of a glyph
#define GlyphNot(glyph)   GLYPH_Generic((glyph), GlyphNot_)((glyph))
   static inline Glyph8   GlyphNot_8_(Glyph8  g)   { return ~g; }
   static inline Glyph16  GlyphNot_16_(Glyph16 g)  { return ~g; }
   static inline Glyph32  GlyphNot_32_(Glyph32 g)  { return ~g; }
   static inline Glyph64  GlyphNot_64_(Glyph64 g)  { return ~g; }
   static inline Glyph128 GlyphNot_128_(Glyph128 g)
   { return (Glyph128){ ~g.top, ~g.bottom }; }
   static inline Glyph256 GlyphNot_256_(Glyph256 g)
   { return (Glyph256){ ~g.top_left   , ~g.top_right,
                        ~g.bottom_left, ~g.bottom_right }; }

// Bitwise And between two glyphs
#define GlyphAnd(glyph1, glyph2) \
   GLYPH_Generic((glyph1), GlyphAnd_)((glyph1), (glyph2))
   static inline Glyph8  GlyphAnd_8_( Glyph8  g, Glyph8  f)  { return g&f; }
   static inline Glyph16 GlyphAnd_16_(Glyph16 g, Glyph16 f)  { return g&f; }
   static inline Glyph32 GlyphAnd_32_(Glyph32 g, Glyph32 f)  { return g&f; }
   static inline Glyph64 GlyphAnd_64_(Glyph64 g, Glyph64 f)  { return g&f; }
   static inline Glyph128 GlyphAnd_128_(Glyph128 g, Glyph128 f)
   { return (Glyph128){ g.top&f.top, g.bottom&f.bottom }; }
   static inline Glyph256 GlyphAnd_256_(Glyph256 g, Glyph256 f)
   { return (Glyph256){ g.top_left&f.top_left, g.top_right&f.top_right,
                        g.bottom_left&f.bottom_left, g.bottom_right&f.bottom_left }; }

// Bitwise Or (i.e. union) of two glyphs
#define GlyphOr(glyph1, glyph2) \
   GLYPH_Generic((glyph1), GlyphOr_)((glyph1), (glyph2))
   static inline Glyph8  GlyphOr_8_( Glyph8  g, Glyph8  f)  { return g|f; }
   static inline Glyph16 GlyphOr_16_(Glyph16 g, Glyph16 f)  { return g|f; }
   static inline Glyph32 GlyphOr_32_(Glyph32 g, Glyph32 f)  { return g|f; }
   static inline Glyph64 GlyphOr_64_(Glyph64 g, Glyph64 f)  { return g|f; }
   static inline Glyph128 GlyphOr_128_(Glyph128 g, Glyph128 f)
   { return (Glyph128){ g.top|f.top, g.bottom|f.bottom }; }
   static inline Glyph256 GlyphOr_256_(Glyph256 g, Glyph256 f)
   { return (Glyph256){ g.top_left|f.top_left, g.top_right|f.top_right,
                        g.bottom_left|f.bottom_left, g.bottom_right|f.bottom_left }; }

// Bitwise Xor between two glyphs
#define GlyphXor(glyph1, glyph2) \
   GLYPH_Generic((glyph1), GlyphXor_)((glyph1), (glyph2))
   static inline Glyph8  GlyphXor_8_( Glyph8  g, Glyph8  f)  { return g^f; }
   static inline Glyph16 GlyphXor_16_(Glyph16 g, Glyph16 f)  { return g^f; }
   static inline Glyph32 GlyphXor_32_(Glyph32 g, Glyph32 f)  { return g^f; }
   static inline Glyph64 GlyphXor_64_(Glyph64 g, Glyph64 f)  { return g^f; }
   static inline Glyph128 GlyphXor_128_(Glyph128 g, Glyph128 f)
   { return (Glyph128){ g.top^f.top, g.bottom^f.bottom }; }
   static inline Glyph256 GlyphXor_256_(Glyph256 g, Glyph256 f)
   { return (Glyph256){ g.top_left^f.top_left, g.top_right^f.top_right,
                        g.bottom_left^f.bottom_left, g.bottom_right^f.bottom_left }; }


//------------------------------------------------------------------------------
// Functions dealing with lines
//------------------------------------------------------------------------------

// Return an unsigned value representing the n-th horizontal line of a Glyph
#define GlyphGetLine(glyph, n) \
   GLYPH_Generic((glyph), GlyphGetLine_)((glyph), (n))
   static inline unsigned GlyphGetLine_8_(Glyph8  g, int n)
   { assert(n >= 0 && n < GLYPH8_HEIGHT);  return BITS_GET_QUARTER(g, n); }
   static inline unsigned GlyphGetLine_16_(Glyph16 g, int n)
   { assert(n >= 0 && n < GLYPH16_HEIGHT); return BITS_GET_NIBBLE(g, n); }
   static inline unsigned GlyphGetLine_32_(Glyph32 g, int n)
   { assert(n >= 0 && n < GLYPH32_HEIGHT); return BITS_GET_NIBBLE(g, n); }
   static inline unsigned GlyphGetLine_64_(Glyph64 g, int n)
   { assert(n >= 0 && n < GLYPH64_HEIGHT); return BITS_GET_BYTE(g, n); }
   static inline unsigned GlyphGetLine_128_(Glyph128 g, int n) {
      assert(n >= 0 && n < GLYPH128_HEIGHT);
      return (n < 8)? GlyphGetLine_64_(g.top, n) : GlyphGetLine_64_(g.bottom, n - 8);
   }
   static inline unsigned GlyphGetLine_256_(Glyph256 g, int n) {
      assert(n >= 0 && n < GLYPH256_HEIGHT);
      if (n < 8) {
         return (GlyphGetLine_64_(g.top_right, n) << 8) | GlyphGetLine_64_(g.top_left, n);
      } else {
         n -= 8;
         return (GlyphGetLine_64_(g.bottom_right, n) << 8) | GlyphGetLine_64_(g.bottom_left, n);
      }
   }

// Glyph GlyphSetLine(Glyph glyph, int8_t nth, unsigned line);
// Return the glyph but with the n-th horizontal line changed to the given value
#define GlyphSetLine(glyph, n, line) \
   GLYPH_Generic((glyph), GlyphSetLine_)((glyph), (n), (line))
   static inline C_ATTRIBUTE_NODISCARD
   Glyph8 GlyphSetLine_8_(Glyph8  g, int n, unsigned quarter) {
      assert(n >= 0 && n < GLYPH8_HEIGHT);
      return BITS_SET_QUARTER(g, n, quarter);
   }
   static inline C_ATTRIBUTE_NODISCARD
   Glyph16 GlyphSetLine_16_(Glyph16 g, int n, unsigned nibble) {
      assert(n >= 0 && n < GLYPH16_HEIGHT);
      return BITS_SET_NIBBLE(g, n, nibble);
   }
   static inline C_ATTRIBUTE_NODISCARD
   Glyph32 GlyphSetLine_32_(Glyph32 g, int n, unsigned nibble) {
      assert(n >= 0 && n < GLYPH32_HEIGHT);
      return BITS_SET_NIBBLE(g, n, nibble);
   }
   static inline C_ATTRIBUTE_NODISCARD
   Glyph64 GlyphSetLine_64_(Glyph64 g, int n, unsigned byte) {
      assert(n >= 0 && n < GLYPH64_HEIGHT);
      return BITS_SET_BYTE(g, n, byte);
   }
   static inline C_ATTRIBUTE_NODISCARD
   Glyph128 GlyphSetLine_128_(Glyph128 g, int n, unsigned byte) {
      assert(n >= 0 && n < GLYPH128_HEIGHT);
      if (n < 8) {
         g.top = GlyphSetLine_64_(g.top, n, byte);
      } else {
         g.bottom = GlyphSetLine_64_(g.bottom, n - 8, byte);
      }
      return g;
   }
   static inline C_ATTRIBUTE_NODISCARD
   Glyph256 GlyphSetLine_256_(Glyph256 g, int n, unsigned u16) {
      assert(n >= 0 && n < GLYPH256_HEIGHT);
      if (n < 8) {
         g.top_left  = GlyphSetLine_64_(g.top_left,  n, u16);
         g.top_right = GlyphSetLine_64_(g.top_right, n, u16 << 8);
      } else {
         n -= 8;
         g.bottom_left  = GlyphSetLine_64_(g.bottom_left,  n, u16);
         g.bottom_right = GlyphSetLine_64_(g.bottom_right, n, u16 << 8);
      }
      return g;
   }

// Combine all the lines of the glyph into a single value using a binary-or
#define GlyphFlatten(glyph) \
   GLYPH_Generic((glyph), GlyphFlatten_)((glyph))
   static inline unsigned GlyphFlatten_8_(Glyph8 g) {
      g |= g >> 4;
      g |= g >> 2;
      return g & 3u;
   }
   static inline unsigned GlyphFlatten_16_(Glyph16 g) {
      g |= g >> 8;
      g |= g >> 4;
      return g & 0xFu;
   }
   static inline unsigned GlyphFlatten_32_(Glyph32 g) {
      g |= g >> 16;
      g |= g >>  8;
      g |= g >>  4;
      return g & 0xFu;
   }
   static inline unsigned GlyphFlatten_64_(Glyph64 g) {
      g |= g >> 32;
      g |= g >> 16;
      g |= g >>  8;
      return g & 0xFFu;
   }
   static inline unsigned GlyphFlatten_128_(Glyph128 g)
   { return GlyphFlatten_64_(g.top) | GlyphFlatten_64_(g.bottom); }
   static inline unsigned GlyphFlatten_256_(Glyph256 g) {
      unsigned left  = GlyphFlatten_64_(g.top_left)  | GlyphFlatten_64_(g.bottom_left);
      unsigned right = GlyphFlatten_64_(g.top_right) | GlyphFlatten_64_(g.bottom_right);
      return (left << 8) | right;
   }

// Return a GlyphN (N in 8|16|32|64|128) made of n lines on the left.
// n must be in [0, width of the glyph type].
#define GlyphLinesLeft(N, n)  GlyphLinesLeft_##N##_((n))
   static inline Glyph8 GlyphLinesLeft_8_(int n) {
      assert(n >= 0 && n <= GLYPH8_WIDTH);
      return ((1u << n) - 1) * GLYPH8(55);
   }
   static inline Glyph16 GlyphLinesLeft_16_(int n) {
      assert(n >= 0 && n <= GLYPH32_WIDTH);
      return ((1u << n) - 1) * GLYPH16(1111);
   }
   static inline Glyph32 GlyphLinesLeft_32_(int n) {
      assert(n >= 0 && n <= GLYPH32_WIDTH);
      return ((1u << n) - 1) * GLYPH32(11111111);
   }
   static inline Glyph64 GlyphLinesLeft_64_(int n) {
      assert(n >= 0 && n <= GLYPH64_WIDTH);
      return ((1u << n) - 1) * GLYPH64(0101010101010101);
   }
   static inline Glyph128 GlyphLinesLeft_128_(int n) {
      Glyph64 g = GlyphLinesLeft_64_(n);
      return (Glyph128){ g, g };
   }
   static inline Glyph256 GlyphLinesLeft_256_(int n) {
      assert(n >= 0 && n <= GLYPH256_WIDTH);
      if (n < 8) {
         Glyph64 g = GlyphLinesLeft_64_(n);
         return (Glyph256){ .top_left = g, .bottom_left = g };
      } else {
         Glyph64 g = GlyphLinesLeft_64_(n - 8);
         return (Glyph256){
            .top_left    = GLYPH64(ffffffffffffffff), .top_right = g,
            .bottom_left = GLYPH64(ffffffffffffffff), .bottom_right = g
         };
      }
   }

// Return a glyphN (N in 8|16|32|64|128|256) made of n lines on the right.
// n must be in [0, width of the glyph type].
#define GlyphLinesRight(N, n)  GlyphNot(GlyphLinesLeft(N, (n)))

// GlyphNot(GlyphLinesLeft(GLYPH_TYPE, UTIL_CAT(GLYPH_TYPE,_WIDTH) - (n)))
// TODO: GlyphTop(GlyphType, n), GlyphBottom(GlyphType, n) ?


//------------------------------------------------------------------------------
// Shifts and Cycles:
// The behavior is undefined if n is negative or is greater or equal the number
// of bits of the glyph.
//------------------------------------------------------------------------------

// Return the glyph shifted n pixels upward (n must be in [0..GLYPH_HEIGHT[)
#define GlyphShiftUp(glyph, n) \
   GLYPH_Generic((glyph), GlyphShiftUp_)((glyph), (n))
   static inline Glyph8  GlyphShiftUp_8_(Glyph8 g, int n) {
      assert(n >= 0 && n < GLYPH8_HEIGHT);
      return g >> (n << GLYPH8_WIDTH_LOG2);
   }
   static inline Glyph16 GlyphShiftUp_16_(Glyph16 g, int n) {
      assert(n >= 0 && n < GLYPH16_HEIGHT);
      return g >> (n << GLYPH16_WIDTH_LOG2);
   }
   static inline Glyph32 GlyphShiftUp_32_(Glyph32 g, int n) {
      assert(n >= 0 && n < GLYPH32_HEIGHT);
      return g >> (n << GLYPH32_WIDTH_LOG2);
   }
   static inline Glyph64 GlyphShiftUp_64_(Glyph64 g, int n) {
      assert(n >= 0 && n < GLYPH64_HEIGHT);
      return g >> (n << GLYPH64_WIDTH_LOG2);
   }
   static inline Glyph128 GlyphShiftUp_128_(Glyph128 g, int n) {
      assert(n >= 0 && n < GLYPH128_HEIGHT);
      if (n == 0) {
         // Do nothing. This case (n==0) is handled separately, as the
         // code below for (n < 8) would lead to do <<64 (which is UB)
      } else if (n < GLYPH64_HEIGHT) {
         g.top    = GlyphShiftUp_64_(g.top, n) | (g.bottom << (64 - (n << GLYPH64_WIDTH_LOG2)));
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
#define GlyphShiftDown(glyph, n) \
   GLYPH_Generic((glyph), GlyphShiftDown_)((glyph), (n))
   static inline Glyph8  GlyphShiftDown_8_(Glyph8 g, int n) {
      assert(n >= 0 && n < GLYPH8_HEIGHT);
      return g << (n << GLYPH8_WIDTH_LOG2);
   }
   static inline Glyph16 GlyphShiftDown_16_(Glyph16 g, int n) {
      assert(n >= 0 && n < GLYPH16_HEIGHT);
      return g << (n << GLYPH16_WIDTH_LOG2);
   }
   static inline Glyph32 GlyphShiftDown_32_(Glyph32 g, int n) {
      assert(n >= 0 && n < GLYPH32_HEIGHT);
      return g << (n << GLYPH32_WIDTH_LOG2);
   }
   static inline Glyph64 GlyphShiftDown_64_(Glyph64 g, int n) {
      assert(n >= 0 && n < GLYPH64_HEIGHT);
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

#define GlyphShiftLeft(glyph, n)     \
   GLYPH_Generic((glyph), GlyphShiftLeft_)((glyph), (n))
   static inline Glyph8 GlyphShiftLeft_8_(Glyph8 g, int n) {
      assert(n >= 0 && n < GLYPH8_WIDTH);
      return (g >> n) & GlyphLinesLeft(8, GLYPH8_WIDTH - n);
   }
   static inline Glyph16 GlyphShiftLeft_16_(Glyph16 g, int n) {
      assert(n >= 0 && n < GLYPH16_WIDTH);
      return (g >> n) & GlyphLinesLeft(16, GLYPH16_WIDTH - n);
   }
   static inline Glyph32 GlyphShiftLeft_32_(Glyph32 g, int n) {
      assert(n >= 0 && n < GLYPH32_WIDTH);
      return (g >> n) & GlyphLinesLeft(32, GLYPH32_WIDTH - n);
   }
   static inline Glyph64 GlyphShiftLeft_64_(Glyph64 g, int n) {
      assert(n >= 0 && n < GLYPH64_WIDTH);
      return (g >> n) & GlyphLinesLeft(64, GLYPH64_WIDTH - n);
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

#define GlyphShiftRight(glyph, n)     \
   GLYPH_Generic((glyph), GlyphShiftRight_)((glyph), (n))
   static inline Glyph8 GlyphShiftRight_8_(Glyph8 g, int n) {
      assert(n >= 0 && n < GLYPH8_WIDTH);
      return (g << n) & GlyphLinesRight(8, GLYPH8_WIDTH - n);
   }
   static inline Glyph16 GlyphShiftRight_16_(Glyph16 g, int n) {
      assert(n >= 0 && n < GLYPH16_WIDTH);
      return (g << n) & GlyphLinesRight(16, GLYPH16_WIDTH - n);
   }
   static inline Glyph32 GlyphShiftRight_32_(Glyph32 g, int n) {
      assert(n >= 0 && n < GLYPH32_WIDTH);
      return (g << n) & GlyphLinesRight(32, GLYPH32_WIDTH - n);
   }
   static inline Glyph64 GlyphShiftRight_64_(Glyph64 g, int n) {
      assert(n >= 0 && n < GLYPH64_WIDTH);
      return (g << n) & GlyphLinesRight(64, GLYPH64_WIDTH - n);
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
#define GlyphCycleUp(glyph, n) \
   GLYPH_Generic((glyph), GlyphCycleUp_)((glyph), (n))
   static inline Glyph8  GlyphCycleUp_8_(Glyph8 g, int n) {
     assert(n >= 0 && n < GLYPH8_HEIGHT);
      if (n == 0) return g; // because otherwise we get g<<8 which is UB
      n <<= GLYPH8_WIDTH_LOG2;
      return (g >> n) | (g << (8 - n));
   }
   static inline Glyph16 GlyphCycleUp_16_(Glyph16 g, int n) {
      assert(n >= 0 && n < GLYPH16_HEIGHT);
      if (n == 0) return g;
      n <<= GLYPH16_WIDTH_LOG2;
      return (g >> n) | (g << (16 - n));
   }
   static inline Glyph32 GlyphCycleUp_32_(Glyph32 g, int n) {
      assert(n >= 0 && n < GLYPH32_HEIGHT);
      if (n == 0) return g;
      n <<= GLYPH32_WIDTH_LOG2;
      return (g >> n) | (g << (32 - n));
   }
   static inline Glyph64 GlyphCycleUp_64_(Glyph64 g, int n) {
      assert(n >= 0 && n < GLYPH64_HEIGHT);
      if (n == 0) return g;
      n <<= GLYPH64_WIDTH_LOG2;
      return (g >> n) | (g << (64 - n));
   }
   static inline Glyph128 GlyphCycleUp_128_(Glyph128 g, int n) {
      assert(n >= 0 && n < GLYPH128_HEIGHT);
      g = GlyphShiftUp(g, n); //TODO
      return g;
   }
   static inline Glyph256 GlyphCycleUp_256_(Glyph256 g, int n) {
      assert(n >= 0 && n < GLYPH256_HEIGHT);
      (void)g; return (Glyph256){0}; // TODO
   }

// Return the glyph cycled n pixels downward (n must be in [0..GLYPH_HEIGHT[)
#define GlyphCycleDown(glyph, n) \
   GLYPH_Generic((glyph), GlyphCycleDown_)((glyph), (n))
   static inline Glyph8  GlyphCycleDown_8_(Glyph8 g, int n) {
      assert(n >= 0 && n < GLYPH8_HEIGHT);
      if (n == 0) return g;
      n <<= GLYPH8_WIDTH_LOG2;
      return (g << n) | (g >> (8 - n));
   }
   static inline Glyph16 GlyphCycleDown_16_(Glyph16 g, int n) {
      assert(n >= 0 && n < GLYPH16_HEIGHT);
      if (n == 0) return g;
      n <<= GLYPH16_WIDTH_LOG2;
      return (g << n) | (g >> (16 - n));
   }
   static inline Glyph32 GlyphCycleDown_32_(Glyph32 g, int n) {
      assert(n >= 0 && n < GLYPH32_HEIGHT);
      if (n == 0) return g;
      n <<= GLYPH32_WIDTH_LOG2;
      return (g << n) | (g >> (32 - n));
   }
   static inline Glyph64 GlyphCycleDown_64_(Glyph64 g, int n) {
      assert(n >= 0 && n < GLYPH64_HEIGHT);
      if (n == 0) return g;
      n <<= GLYPH64_WIDTH_LOG2;
      return (g << n) | (g >> (64 - n));
   }
   static inline Glyph128 GlyphCycleDown_128_(Glyph128 g, int n) {
      assert(n >= 0 && n < GLYPH128_HEIGHT);
      (void)g; return (Glyph128){0}; // TODO
   }
   static inline Glyph256 GlyphCycleDown_256_(Glyph256 g, int n) {
      assert(n >= 0 && n < GLYPH256_HEIGHT);
      (void)g; return (Glyph256){0}; // TODO
   }

#define GlyphCycleLeft(glyph, n) \
   GLYPH_Generic((glyph), GlyphCycleLeft_)((glyph), (n))
#  define PRIVATE_IMPLEMENTATION_(N)                                           \
   static inline Glyph##N GlyphCycleLeft_##N##_(Glyph##N g, int n) {           \
      assert(n >= 0 && n < GLYPH##N##_WIDTH);                                  \
      if (n == 0) return g;                                                    \
      return GlyphOr(GlyphShiftLeft(g, n), GlyphShiftRight(g, GLYPH##N##_WIDTH - n));  \
   }
   PRIVATE_IMPLEMENTATION_(8)
   PRIVATE_IMPLEMENTATION_(16)
   PRIVATE_IMPLEMENTATION_(32)
   PRIVATE_IMPLEMENTATION_(64)
   PRIVATE_IMPLEMENTATION_(128)
   PRIVATE_IMPLEMENTATION_(256)
#  undef PRIVATE_IMPLEMENTATION_

#define GlyphCycleRight(glyph, n) \
   GLYPH_Generic((glyph), GlyphCycleRight_)((glyph), (n))
#  define PRIVATE_IMPLEMENTATION_(N)                                           \
   static inline Glyph##N GlyphCycleRight_##N##_(Glyph##N g, int n) {          \
      assert(n >= 0 && n < GLYPH##N##_WIDTH);                                  \
      if (n == 0) return g;                                                    \
      return GlyphOr(GlyphShiftRight(g, n), GlyphShiftLeft(g, GLYPH##N##_WIDTH - n)); \
   }
   PRIVATE_IMPLEMENTATION_(8)
   PRIVATE_IMPLEMENTATION_(16)
   PRIVATE_IMPLEMENTATION_(32)
   PRIVATE_IMPLEMENTATION_(64)
   PRIVATE_IMPLEMENTATION_(128)
   PRIVATE_IMPLEMENTATION_(256)
#  undef PRIVATE_IMPLEMENTATION_


//------------------------------------------------------------------------------
// Flip, Mirror, Rotate, Transpose glyphs
// Note: Rotate and Transpose only work on squared-size glyphs: Glyph<16,64,256>
// Inspiration for implementation: Hacker's Delight +
// https://stackoverflow.com/questions/1667591/rotating-a-bitmap-90-degrees
//------------------------------------------------------------------------------

// Return the given glyph flipped along the horizontal vertical axis (|)

#define GlyphFlip(glyph) \
   GLYPH_Generic((glyph), GlyphFlip_)((glyph))
   static inline Glyph8  GlyphFlip_8_(Glyph8 g) {
      // Reverse the ordering of 2bits
      g = ((g & 0xf0U) >>  4) | ((g & 0x0fU) <<  4);
      g = ((g & 0xccU) >>  2) | ((g & 0x33U) <<  2);
      return g;
   }
   static inline Glyph16 GlyphFlip_16_(Glyph16 g) {
      // Reverse the ordering of nibbles
      g = ((g & 0xff00U) >>  8) | ((g & 0x00ffU) <<  8);
      g = ((g & 0xf0f0U) >>  4) | ((g & 0x0f0fU) <<  4);
      return g;
   }
   static inline Glyph32 GlyphFlip_32_(Glyph32 g) {
      // Reverse the ordering of nibbles
      g = ((g & 0xffff0000U) >> 16) | ((g & 0x0000ffffU) << 16);
      g = ((g & 0xff00ff00U) >>  8) | ((g & 0x00ff00ffU) <<  8);
      g = ((g & 0xf0f0f0f0U) >>  4) | ((g & 0x0f0f0f0fU) <<  4);
      return g;
   }
   static inline Glyph64 GlyphFlip_64_(Glyph64 g)
   { // Reverse the ordering of bytes
#  ifdef __GNUC__
      return __builtin_bswap64(g);
#  else
      g = ((g & 0xffffffff00000000U) >> 32) | ((g & 0x00000000ffffffffU) << 32);
      g = ((g & 0xffff0000ffff0000U) >> 16) | ((g & 0x0000ffff0000ffffU) << 16);
      g = ((g & 0xff00ff00ff00ff00U) >>  8) | ((g & 0x00ff00ff00ff00ffU) <<  8);
      return g;
#  endif
   }
   static inline Glyph128 GlyphFlip_128_(Glyph128 g)
   { return (Glyph128){GlyphFlip_64_(g.bottom), GlyphFlip_64_(g.top)}; }
   static inline Glyph256 GlyphFlip_256_(Glyph256 g) {
      return (Glyph256){
         GlyphFlip_64_(g.bottom_left), GlyphFlip_64_(g.bottom_right),
         GlyphFlip_64_(g.top_left),    GlyphFlip_64_(g.top_right)
      };
   }

// Return the given glyph flipped along the vertical axis (|)
#define GlyphMirror(glyph) \
   GLYPH_Generic((glyph), GlyphMirror_)((glyph))
   static inline Glyph8 GlyphMirror_8_(Glyph8 g) {
      // Reverse the bits order of every 2bit in the glyph
      return ((g & 0xaaU) >> 1) | ((g & 0x55U) << 1);
   }
   static inline Glyph16 GlyphMirror_16_(Glyph16 g) {
      // Reverse the bits order of every nibble in the glyph
      g = ((g & 0xccccU) >> 2) | ((g & 0x3333U) << 2);
      g = ((g & 0xaaaaU) >> 1) | ((g & 0x5555U) << 1);
      return g;
   }
   static inline Glyph32 GlyphMirror_32_(Glyph32 g) {
      // Reverse the bits order of every nibble in the glyph
      g = ((g & 0xccccccccU) >> 2) | ((g & 0x33333333U) << 2);
      g = ((g & 0xaaaaaaaaU) >> 1) | ((g & 0x55555555U) << 1);
      return g;
   }
   static inline Glyph64 GlyphMirror_64_(Glyph64 g) {
      // Reverse the bits order of every byte in the glyph
      g = ((g & 0xf0f0f0f0f0f0f0f0U) >> 4) | ((g & 0x0f0f0f0f0f0f0f0fU) << 4);
      g = ((g & 0xccccccccccccccccU) >> 2) | ((g & 0x3333333333333333U) << 2);
      g = ((g & 0xaaaaaaaaaaaaaaaaU) >> 1) | ((g & 0x5555555555555555U) << 1);
      return g;
   }
   static inline Glyph128 GlyphMirror_128_(Glyph128 g)
   { return (Glyph128){GlyphMirror_64_(g.top),GlyphMirror_64_(g.bottom)}; }
   static inline Glyph256 GlyphMirror_256_(Glyph256 g) {
      return (Glyph256){
         GlyphMirror_64_(g.top_right),    GlyphMirror_64_(g.top_left),
         GlyphMirror_64_(g.bottom_right), GlyphMirror_64_(g.bottom_left),
      };
   }

// Return the given squared-size (16|64|256) glyph flipped along the \ diagonal.
#define GlyphTranspose(square_glyph) \
   _Generic((square_glyph)         , \
      Glyph16:  GlyphTranspose_16_ , \
      Glyph64:  GlyphTranspose_64_ , \
      Glyph256: GlyphTranspose_256_  \
   )((square_glyph))
   static inline Glyph16 GlyphTranspose_16_(Glyph16 g) {
      // inspired by https://stackoverflow.com/questions/65612229/flip-4x4-bit-grid-horizontally-vertically-diagonally
      return (g & 0x8421U) | // main diagonal
            ((g & 0x4210U) >> 3) | ((g & 0x2100U) >> 6) | ((g & 0x1000U) >> 9) |  // <- upper diagonals.
            ((g & 0x0842U) << 3) | ((g & 0x0084U) << 6) | ((g & 0x0008U) << 9) ;  // <- lower diagonals
   }
   static inline Glyph64 GlyphTranspose_64_(Glyph64 g) {
      // Based on "Hacker's Delight" (second edition) by Henry S. Warren, Jr.
      // https://web.archive.org/web/20190915025154/http://www.hackersdelight.org/
      // https://en.wikipedia.org/wiki/Hacker%27s_Delight
      g =   ( g        & 0xaa55aa55aa55aa55U       ) |
            ((g        & 0x00aa00aa00aa00aaU) << 7 ) |
            ((g >> 7)  & 0x00aa00aa00aa00aaU       ) ;
      g =   ( g        & 0xcccc3333cccc3333U       ) |
            ((g        & 0x0000cccc0000ccccU) << 14) |
            ((g >> 14) & 0x0000cccc0000ccccU       ) ;
      g =   ( g        & 0xf0f0f0f00f0f0f0fU       ) |
            ((g        & 0x00000000f0f0f0f0U) << 28) |
            ((g >> 28) & 0x00000000f0f0f0f0U)        ;
      return g;
   }
   static inline Glyph256 GlyphTranspose_256_(Glyph256 g) {
      return (Glyph256){
         GlyphTranspose_64_(g.top_left),  GlyphTranspose_64_(g.bottom_left),
         GlyphTranspose_64_(g.top_right), GlyphTranspose_64_(g.bottom_right),
      };
   }

// Return the given Glyph rotated  90 degrees (counterclockwise)
#define GlyphRotate90(square_glyph)    GlyphTranspose(GlyphMirror(square_glyph))

// Return the given Glyph rotated 180 degrees
#define GlyphRotate180(glyph)          GlyphMirror(GlyphFlip(glyph))

// Return the given Glyph rotated 270 degrees (= 90 degrees clockwise)
#define GlyphRotate270(square_glyph)   GlyphTranspose(GlyphFlip(square_glyph))


//------------------------------------------------------------------------------
// Access to Pixels
// Glyph<N> GlyphFromPixel(N, x,y)
// int      GlyphGetPixel(glyph, x,y)
// Glyph    GlyphSetPixel(glyph, x,y, bit)
// bool     GlyphPixelIsSet(glyph, x,y)
//------------------------------------------------------------------------------

// Index of the bit representing the pixel (x,y) in a glyph
#define GLYPH_INDEX(x,y)     ((x) + ((y) << GLYPH_WIDTH_LOG2))
#define GLYPH8_INDEX(x,y)    ((x) + ((y) << GLYPH8_WIDTH_LOG2))
#define GLYPH16_INDEX(x,y)   ((x) + ((y) << GLYPH16_WIDTH_LOG2))
#define GLYPH32_INDEX(x,y)   ((x) + ((y) << GLYPH32_WIDTH_LOG2))
#define GLYPH64_INDEX(x,y)   ((x) + ((y) << GLYPH64_WIDTH_LOG2))
#define GLYPH128_INDEX(x,y)  ((x) + ((y) << GLYPH128_WIDTH_LOG2))
#define GLYPH256_INDEX(x,y)  ((x) + ((y) << GLYPH256_WIDTH_LOG2))

// Glyph<N> GlyphFromPixel(N, x,y);
// where N is a literal <8|16|32|64|128|256>
// Construct a Glyph<N> containing a single pixel (x,y)
#define GlyphFromPixel(N, x,y) \
   GlyphFromPixel_##N##_((x), (y))
   static inline Glyph8 GlyphFromPixel_8_(int x, int y) {
      assert(x >= 0 && x < GLYPH8_WIDTH);
      assert(y >= 0 && y < GLYPH8_HEIGHT);
      return 1U << (x + (y << GLYPH8_WIDTH_LOG2));
   }
   static inline Glyph16 GlyphFromPixel_16_(int x, int y) {
      assert(x >= 0 && x < GLYPH16_WIDTH);
      assert(y >= 0 && y < GLYPH16_HEIGHT);
      return 1U << (x + (y << GLYPH16_WIDTH_LOG2));
   }
   static inline Glyph32 GlyphFromPixel_32_(int x, int y) {
      assert(x >= 0 && x < GLYPH32_WIDTH);
      assert(y >= 0 && y < GLYPH32_HEIGHT);
      return UINT32_C(1) << (x + (y << GLYPH32_WIDTH_LOG2));
   }
   static inline Glyph64 GlyphFromPixel_64_(int x, int y) {
      assert(x >= 0 && x < GLYPH64_WIDTH);
      assert(y >= 0 && y < GLYPH64_HEIGHT);
      return UINT64_C(1) << (x + (y << GLYPH64_WIDTH_LOG2));
   }
   static inline Glyph128 GlyphFromPixel_128_(int x, int y) {
      assert(x >= 0 && x < GLYPH128_WIDTH);
      assert(y >= 0 && y < GLYPH128_HEIGHT);
      return (y < GLYPH64_HEIGHT) ?
         (Glyph128){.top    = GlyphFromPixel(64, x, y)} :
         (Glyph128){.bottom = GlyphFromPixel(64, x, y - GLYPH64_HEIGHT)} ;
   }
   static inline Glyph256 GlyphFromPixel_256_(int x, int y) {
      assert(x >= 0 && x < GLYPH256_WIDTH);
      assert(y >= 0 && y < GLYPH256_HEIGHT);
      if (x < GLYPH64_WIDTH) {
         return (y < GLYPH64_HEIGHT) ?
            (Glyph256){.top_left    = GlyphFromPixel(64, x, y)} :
            (Glyph256){.bottom_left = GlyphFromPixel(64, x, y - GLYPH64_HEIGHT)};
      } else {
         return (y < GLYPH64_HEIGHT) ?
            (Glyph256){.top_right    = GlyphFromPixel(64, x - GLYPH64_WIDTH, y)} :
            (Glyph256){.bottom_right = GlyphFromPixel(64, x - GLYPH64_WIDTH, y - GLYPH64_HEIGHT)};
      }
   }

// TODO: should I also do const void* ???
// Return the value (0 or 1) of the pixel (x,y) of the given glyph
#define GlyphGetPixel(glyph, x,y)      \
   _Generic((glyph)                  , \
      Glyph8:   GlyphGetPixel_8_     , \
      Glyph16:  GlyphGetPixel_16_    , \
      Glyph32:  GlyphGetPixel_32_    , \
      Glyph64:  GlyphGetPixel_64_    , \
      Glyph128: GlyphGetPixel_128_   , \
      Glyph256: GlyphGetPixel_256_   , \
      default : GlyphGetPixel_dynamic_ \
   )((glyph), (x), (y))
   static inline unsigned GlyphGetPixel_8_(Glyph8 g, int x, int y) {
      assert(x >= 0 && x < GLYPH8_WIDTH);
      assert(y >= 0 && y < GLYPH8_HEIGHT);
      return BITS_GET_BIT(g, GLYPH8_INDEX(x,y));
   }
   static inline unsigned GlyphGetPixel_16_(Glyph16 g, int x, int y)
   {  assert(x >= 0 && x < GLYPH16_WIDTH);
      assert(y >= 0 && y < GLYPH16_HEIGHT);
      return BITS_GET_BIT(g, GLYPH16_INDEX(x,y));
   }
   static inline uint32_t GlyphGetPixel_32_(Glyph32 g, int x, int y) {
      assert(x >= 0 && x < GLYPH32_WIDTH);
      assert(y >= 0 && y < GLYPH32_HEIGHT);
      return BITS_GET_BIT(g, GLYPH32_INDEX(x,y));
   }
   static inline uint64_t GlyphGetPixel_64_(Glyph64 g, int x, int y) {
      assert(x >= 0 && x < GLYPH64_WIDTH);
      assert(y >= 0 && y < GLYPH64_HEIGHT);
      return BITS_GET_BIT(g, GLYPH64_INDEX(x,y));
   }
   static inline uint64_t GlyphGetPixel_128_(Glyph128 g, int x, int y) {
      assert(x >= 0 && x < GLYPH128_WIDTH);
      assert(y >= 0 && y < GLYPH128_HEIGHT);
      if (y < 8)
         return GlyphGetPixel_64_(g.top, x, y);
      else
         return GlyphGetPixel_64_(g.bottom, x, y - 8);
   }
   static inline uint64_t GlyphGetPixel_256_(Glyph256 g, int x, int y) {
      assert(x >= 0 && x < GLYPH256_WIDTH);
      assert(y >= 0 && y < GLYPH256_HEIGHT);
      if (x < 8) {
         if (y < 8)
            return GlyphGetPixel_64_(g.top_left, x, y);
         else
            return GlyphGetPixel_64_(g.bottom_left, x, y - 8);
      } else {
         if (y < 8)
            return GlyphGetPixel_64_(g.top_right, x - 8, y);
         else
            return GlyphGetPixel_64_(g.bottom_right, x - 8, y - 8);
      }
   }
   static inline uint64_t GlyphGetPixel_dynamic_(const void *g, int x, int y) {
      assert(g != NULL);
      switch (VideoModeDimension()) {
         case PIXELS_2x4  : return GlyphGetPixel(*(const Glyph8 *)g, x, y);
         case PIXELS_4x4  : return GlyphGetPixel(*(const Glyph16*)g, x, y);
         case PIXELS_4x8  : return GlyphGetPixel(*(const Glyph32*)g, x, y);
         case PIXELS_8x8  : return GlyphGetPixel(*(const Glyph64*)g, x, y);
         case PIXELS_8x16 : return GlyphGetPixel(*(const Glyph128*)g, x, y);
         case PIXELS_16x16: return GlyphGetPixel(*(const Glyph256*)g, x, y);
         default: unreachable();
      }
   }


// Return the glyph with the pixel (x,y) set to bit value (0 or 1)
#define GlyphSetPixel(glyph, x,y, bit) \
   _Generic((glyph)                  , \
      Glyph8:   GlyphSetPixel_8_     , \
      Glyph16:  GlyphSetPixel_16_    , \
      Glyph32:  GlyphSetPixel_32_    , \
      Glyph64:  GlyphSetPixel_64_    , \
      Glyph128: GlyphSetPixel_128_   , \
      Glyph256: GlyphSetPixel_256_   , \
      default : GlyphSetPixel_dynamic_ \
   )((glyph), (x), (y), (bit))
   static inline C_ATTRIBUTE_NODISCARD
   Glyph8  GlyphSetPixel_8_(Glyph8 g, int x, int y, int bit) {
      assert(x >= 0 && x < GLYPH8_WIDTH);
      assert(y >= 0 && y < GLYPH8_HEIGHT);
      assert(bit == 0 || bit == 1);
      return BITS_SET_BIT(g, GLYPH8_INDEX(x,y), bit);
   }
   static inline C_ATTRIBUTE_NODISCARD
   Glyph16  GlyphSetPixel_16_(Glyph16 g, int x, int y, int bit) {
      assert(x >= 0 && x < GLYPH16_WIDTH);
      assert(y >= 0 && y < GLYPH16_HEIGHT);
      assert(bit == 0 || bit == 1);
      return BITS_SET_BIT(g, GLYPH16_INDEX(x,y), bit);
   }
   static inline C_ATTRIBUTE_NODISCARD
   Glyph32  GlyphSetPixel_32_(Glyph32 g, int x, int y, int bit) {
      assert(x >= 0 && x < GLYPH32_WIDTH);
      assert(y >= 0 && y < GLYPH32_HEIGHT);
      assert(bit == 0 || bit == 1);
      return BITS_SET_BIT(g, GLYPH32_INDEX(x,y), bit);
   }
   static inline C_ATTRIBUTE_NODISCARD
   Glyph64  GlyphSetPixel_64_(Glyph64 g, int x, int y, int bit) {
      assert(x >= 0 && x < GLYPH64_WIDTH);
      assert(y >= 0 && y < GLYPH64_HEIGHT);
      assert(bit == 0 || bit == 1);
      return BITS_SET_BIT(g, GLYPH64_INDEX(x,y), bit);
   }
   static inline C_ATTRIBUTE_NODISCARD
   Glyph128  GlyphSetPixel_128_(Glyph128 g, int x, int y, int bit) {
      assert(x >= 0 && x < GLYPH128_WIDTH);
      assert(y >= 0 && y < GLYPH128_HEIGHT);
      if (y < 8)
         return (Glyph128) {
            .top    = GlyphSetPixel_64_(g.top, x, y, bit),
            .bottom = g.bottom
         };
      else
         return (Glyph128) {
            .top    = g.top,
            .bottom = GlyphSetPixel_64_(g.bottom, x, y - 8, bit)
         };
   }
   static inline C_ATTRIBUTE_NODISCARD
   Glyph256  GlyphSetPixel_256_(Glyph256 g, int x, int y, int bit) {
      assert(x >= 0 && x < GLYPH256_WIDTH);
      assert(y >= 0 && y < GLYPH256_HEIGHT);
      if (x < 8) {
         if (y < 8)
            return (Glyph256) {
               .top_left     = GlyphSetPixel_64_(g.top_left, x, y, bit),
               .top_right    = g.top_right,
               .bottom_left  = g.bottom_left,
               .bottom_right = g.bottom_right,
            };
         else
            return (Glyph256) {
               .top_left     = g.top_left,
               .top_right    = g.top_right,
               .bottom_left  = GlyphSetPixel_64_(g.bottom_left, x, y - 8, bit),
               .bottom_right = g.bottom_right,
            };
      } else {
         if (y < 8)
            return (Glyph256) {
               .top_left     = g.top_left,
               .top_right    = GlyphSetPixel_64_(g.top_right, x - 8, y, bit),
               .bottom_left  = g.bottom_left,
               .bottom_right = g.bottom_right,
            };
         else
            return (Glyph256) {
               .top_left     = g.top_left,
               .top_right    = g.top_right,
               .bottom_left  = g.bottom_left,
               .bottom_right = GlyphSetPixel_64_(g.bottom_right, x - 8, y - 8, bit),
            };
      }
   }
   static inline // Note that return value is VOID.
   void GlyphSetPixel_dynamic_(void *g, int x, int y, int bit) {
      assert(g != NULL);
      switch (VideoModeDimension()) {
         case PIXELS_2x4  : *(Glyph8 *)g = GlyphSetPixel(*(Glyph8 *)g, x, y, bit);  break;
         case PIXELS_4x4  : *(Glyph16*)g = GlyphSetPixel(*(Glyph16*)g, x, y, bit);  break;
         case PIXELS_4x8  : *(Glyph32*)g = GlyphSetPixel(*(Glyph32*)g, x, y, bit);  break;
         case PIXELS_8x8  : *(Glyph64*)g = GlyphSetPixel(*(Glyph64*)g, x, y, bit);  break;
         case PIXELS_8x16 : *(Glyph128*)g = GlyphSetPixel(*(Glyph128*)g, x, y, bit);  break;
         case PIXELS_16x16: *(Glyph256*)g = GlyphSetPixel(*(Glyph256*)g, x, y, bit);  break;
         default: unreachable();
      }
   }

// Return a non-zero value iff the pixel (x,y) of the glyph is set
// Note about PixelIsSet vs PixelAt in tests:
//      At() is `(x >> n) & 1` where as IsSet() is `x & (1 << n)`.
//      If n is a constant, IsSet might take one op less.
#define GlyphPixelIsSet(glyph, x,y)   \
   _Generic((glyph)                 , \
      Glyph8:   GlyphPixelIsSet_8_  , \
      Glyph16:  GlyphPixelIsSet_16_ , \
      Glyph32:  GlyphPixelIsSet_32_ , \
      Glyph64:  GlyphPixelIsSet_64_ , \
      Glyph128: GlyphPixelIsSet_128_, \
      Glyph256: GlyphPixelIsSet_256_  \
   )((glyph), (x), (y))
   static inline uint8_t  GlyphPixelIsSet_8_(Glyph8 g, int x, int y) {
      assert(x >= 0 && x < GLYPH8_WIDTH);
      assert(y >= 0 && y < GLYPH8_HEIGHT);
      return g & (1U << GLYPH8_INDEX(x,y));
   }
   static inline uint16_t GlyphPixelIsSet_16_(Glyph16 g, int x, int y) {
      assert(x >= 0 && x < GLYPH16_WIDTH);
      assert(y >= 0 && y < GLYPH16_HEIGHT);
      return g & (1U << GLYPH16_INDEX(x,y));
   }
   static inline uint32_t GlyphPixelIsSet_32_(Glyph32 g, int x, int y) {
      assert(x >= 0 && x < GLYPH32_WIDTH);
      assert(y >= 0 && y < GLYPH32_HEIGHT);
      return g & (UINT32_C(1) << GLYPH32_INDEX(x,y));
   }
   static inline uint64_t GlyphPixelIsSet_64_(Glyph64 g, int x, int y) {
      assert(x >= 0 && x < GLYPH64_WIDTH);
      assert(y >= 0 && y < GLYPH64_HEIGHT);
      return g & (UINT64_C(1) << GLYPH64_INDEX(x,y));
   }
   static inline uint64_t GlyphPixelIsSet_128_(Glyph128 g, int x, int y) {
      assert(x >= 0 && x < GLYPH128_WIDTH);
      assert(y >= 0 && y < GLYPH128_HEIGHT);
      if (y < 8)
         return GlyphPixelIsSet_64_(g.top, x, y);
      else
         return GlyphPixelIsSet_64_(g.bottom, x, y - 8);
   }
   static inline uint64_t GlyphPixelIsSet_256_(Glyph256 g, int x, int y) {
      assert(x >= 0 && x < GLYPH256_WIDTH);
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
// Doubling and Halving Glyphs
//------------------------------------------------------------------------------

// Extract a glyph of half size.
// The offset controls at which line or column we start.
// uint32_t offset 0-->4
// uint64_t offset 0-->4
#define GlyphHalf(glyph, offset) \
   _Generic((glyph)            , \
      Glyph16:  GlyphHalf_16_  , \
      Glyph32:  GlyphHalf_32_  , \
      Glyph64:  GlyphHalf_64_  , \
      Glyph128: GlyphHalf_128_ , \
      Glyph256: GlyphHalf_256_   \
   )((glyph), (offset))
   static inline uint8_t  GlyphHalf_16_(Glyph16 glyph, int offset) {
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
   static inline Glyph16 GlyphHalf_32_(Glyph32 glyph, int offset) {
      assert(offset >= 0 && offset <= (GLYPH32_HEIGHT / 2));
      return (uint16_t) GlyphShiftUp_32_(glyph, offset);
   }
   static inline Glyph32 GlyphHalf_64_(Glyph64 glyph, int offset) {
      assert(offset >= 0 && offset <= (GLYPH64_WIDTH / 2));
      uint32_t half = 0;
      // Extract every second nibble from the glyph
      for (int i = 0; i < 8; i++) {
         // Shift right by 8 bits each iteration to move to the next pair of
         // nibbles
         // Use bitwise AND with 0xF0 to extract the second nibble
         uint32_t nibble = (glyph >> (i * 8 + offset)) & 0xFu;
         // Place the extracted nibble in the correct position in the result
         half |= nibble << (i * 4);
      }
      return half;
   }
   static inline Glyph64 GlyphHalf_128_(Glyph128 glyph, int offset) {
      assert(offset >= 0 && offset <= (GLYPH128_HEIGHT / 2));
      return GlyphShiftUp_128_(glyph, offset).top;
   }
   static inline Glyph128 GlyphHalf_256_(Glyph256 glyph, int offset) {
      assert(offset >= 0 && offset <= (GLYPH256_WIDTH / 2));
// TODO / FIXME: uncomment -->   glyph = GlyphShiftLeft_256_(glyph, offset);
      return (Glyph128){glyph.top_left, glyph.bottom_left};
   }

// Return the given glyph with both halves swapped
#define GlyphSwapHalves(glyph)         \
   _Generic((glyph)                  , \
      Glyph8:   GlyphSwapHalves_8_   , \
      Glyph16:  GlyphSwapHalves_16_  , \
      Glyph32:  GlyphSwapHalves_32_  , \
      Glyph64:  GlyphSwapHalves_64_  , \
      Glyph128: GlyphSwapHalves_128_ , \
      Glyph256: GlyphSwapHalves_256_   \
   )((glyph))
   static inline Glyph8 GlyphSwapHalves_8_(Glyph8 g)
   { return (g << 4) | (g >> 4); }
   static inline Glyph16 GlyphSwapHalves_16_(Glyph16 g)
   { return ((g & 0xccccU) >> 2) | ((g & 0x3333U) << 2); }
   static inline Glyph32 GlyphSwapHalves_32_(Glyph32 g)
   { return (g << 16) | (g >> 16); }
   static inline Glyph64 GlyphSwapHalves_64_(Glyph64 g)
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
   _Generic((left_or_top)                           , \
      Glyph8:   GlyphFromHalves_8_                  , \
      Glyph16:  GlyphFromHalves_16_                 , \
      Glyph32:  GlyphFromHalves_32_                 , \
      Glyph64:  GlyphFromHalves_64_                 , \
      Glyph128: GlyphFromHalves_128_                  \
   )((left_or_top), (right_or_bottom))
   static inline Glyph16 GlyphFromHalves_8_1_(Glyph8 half) {
      // Return a glyph16 from a glyph8 placed on the left side.
      // (Hint: << 2 to place it on the right instead)

      // Interleave the byte's bits value xxxxxxxx as xx00xx00xx00xx00
      unsigned g = half;
      g  =   (g | (g << 4)) & 0x0F0Fu;
      return (g | (g << 2)) & 0x3333u;
   }
   static inline Glyph16 GlyphFromHalves_8_(Glyph8 l, Glyph8 r)
   { return GlyphFromHalves_8_1_(l) | (GlyphFromHalves_8_1_(r) << 2); }
   static inline Glyph32 GlyphFromHalves_16_(Glyph16 t, Glyph16 b)
   { return (uint32_t)t | ((uint32_t)b << 16); }
   static inline Glyph64 GlyphFromHalves_32_1_(Glyph32 half) {
      // Return a glyph64 from a glyp32 placed on the left side.
      // (Hint: << 4 to place it on the right instead)

      // This interleaves the nibbles: nnnnnnnn -> n0n0n0n0n0n0n0n0
      uint64_t g = half;
      g  =   (g | (g << 16)) & 0x0000FFFF0000FFFFu;
      g  =   (g | (g <<  8)) & 0x00FF00FF00FF00FFu;
      return (g | (g <<  4)) & 0x0F0F0F0F0F0F0F0Fu;
   }
   static inline Glyph64 GlyphFromHalves_32_(Glyph32 l, Glyph32 r)
   { return GlyphFromHalves_32_1_(l) | (GlyphFromHalves_32_1_(r) << 4); }
   static inline Glyph128 GlyphFromHalves_64_(Glyph64 t, Glyph64 b)
   { return (Glyph128){ t, b }; }
   static inline Glyph256 GlyphFromHalves_128_(Glyph128 l, Glyph128 r)
   { return (Glyph256){ l.top, r.top, l.bottom, r.bottom }; }


// Return a bigger glyph based on the given glyph. This will double up the size
// of each pixel either horizontally (for example for Glyph16 -> Glyph32) or
// vertically (for example for Glyph32->Glyph64).
// Note: It works with Glyph8-128 but Glyph256s cannot be upscaled.
#define GlyphUpscale(glyph)        \
   _Generic((glyph)              , \
      Glyph8:   GlyphUpscale_8_  , \
      Glyph16:  GlyphUpscale_16_ , \
      Glyph32:  GlyphUpscale_32_ , \
      Glyph64:  GlyphUpscale_64_ , \
      Glyph128: GlyphUpscale_128_  \
   )((glyph))
   static inline Glyph16 GlyphUpscale_8_(Glyph8 g) {
      // interleave bits of g with itself, use the 64-bit multiplication
      // technique described at:
      // https://graphics.stanford.edu/~seander/bithacks.html#Interleave64bitOps
      uint64_t t = (g * 0x0101010101010101u & 0x8040201008040201u) * 0x0102040810204081u;
      return ((t >> 49) & 0x5555u) | ((t >> 48) & 0xAAAAu);
   }
   static inline Glyph32 GlyphUpscale_16_(Glyph16 g) {
      // Extract each line (nibble) from the glyph
      uint32_t i = (g >> 12) & 0xF;
      uint32_t j = (g >> 8)  & 0xF;
      uint32_t k = (g >> 4)  & 0xF;
      uint32_t l =  g        & 0xF;
      // Construct the 32-bit result with doubled nibbles: iijjkkll
      return (i<<28)|(i<<24) | (j<<20)|(j<<16) | (k<<12)|(k<<8) | (l<<4)|l;
   }
/* nope
        static inline Glyph64 GlyphUpscale_32_(Glyph32 g)
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
   static inline Glyph64 GlyphUpscale_32_(Glyph32 g) {
/* No?:
           uint64_t z = 0;
           for (int i = 0; i < 32; i++) {
              uint64_t t = (g & 1U << i) << i;
              z |= t | (t << 1);
           }
           return z;
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
   static inline Glyph128 GlyphUpscale_64_(Glyph64 g) {
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

#endif //include guard
