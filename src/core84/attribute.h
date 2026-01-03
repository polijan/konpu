#ifndef  KONPU_ATTRIBUTE_H_
#define  KONPU_ATTRIBUTE_H_
#include "video_mode.h"
#include "bits.h"
#include "rectangle.h" // TODO: move to vide_mode.h or even to arch.h?

//------------------------------------------------------------------------------
// Attributes' Dimension, Color Type, and (full) Type
//------------------------------------------------------------------------------

// Attributes are Video elements that can be contained in the framebuffer
// alongside Glyphs or Strip8s. They carry one or two colors used to color their
// underlying bit pixels: a "pen" color (applied to the "1" bits) and/or a
// "paper" color (applied to 0 bits).
//
// In the C type system, they are represented as a byte (Attribute8: carrying
// either a true color pen, a true color paper, or a pair of two nibbles) or a
// pair of bytes (Attribute16: carrying two true color values).


// ATTRIBUTE8(value) or ATTRIBUTE8(pen, paper) - construct an Attribute8
#define ATTRIBUTE8(...) \
   UTIL_OVERLOAD(PRIVATE_ATTRIBUTE8, __VA_ARGS__)
#  define PRIVATE_ATTRIBUTE8_1_(value)       ((Attribute8)(value))
#  define PRIVATE_ATTRIBUTE8_2_(pen, paper)  ((Attribute8)(((pen) << 4) | (paper)))

// ATTRIBUTE16(pen, paper) - construct an Attribute16
#define ATTRIBUTE16(pen, paper)      ((Attribute16){ (pen), (paper) })

// The type of color(s) encoded by Attributes
// ATTRIBUTE8_COLOR  attributes are stored with the C type Attribute8 (=uint8_t)
// ATTRIBUTE16_COLOR attributes are stored with the C type Attribute16
enum AttributeColorType {
   ATTRIBUTE8_COLOR       = 0,  // 1 byte, nibbles specify two 16-colors (pen/paper)
   ATTRIBUTE8_COLOR_PEN   = 1,  // 1 byte, specifies one full color  (pen)
   ATTRIBUTE8_COLOR_PAPER = 2,  // 1 byte, specifies one full color  (paper)
   ATTRIBUTE16_COLOR      = 3,  // 2 bytes, specify  two full colors (pen/paper)
};

// The (full) type of an Attribute, combining its dimension and its color type
// a single nibble value:
// ```
//  .------ Attributes pixel size (value as enum VideoElementDimension)
//  |       values can be:PIXELS_2x4, PIXEL_4x4, or PIXELS_8x8
//  |
//  |  .--- Attributes color type (value as enum AttributeColorType, up to 8x8)
//  v  v
// |..|..|
// ```
//
// It is notably used as:
// - the low nibble of the Video.mode when the framebuffer contains Attributes
// - or to specify the desired Attributes in the `VIDEO_MODE_*_ATTRIBUTE(...)`
//   macro/functions
//
enum AttributeType {
   ATTRIBUTE8_2x4       = (PIXELS_2x4 << 2) | ATTRIBUTE8_COLOR,
   ATTRIBUTE8_2x4_PEN   = (PIXELS_2x4 << 2) | ATTRIBUTE8_COLOR_PEN,
   ATTRIBUTE8_2x4_PAPER = (PIXELS_2x4 << 2) | ATTRIBUTE8_COLOR_PAPER,
   ATTRIBUTE16_2x4      = (PIXELS_2x4 << 2) | ATTRIBUTE16_COLOR,
   ATTRIBUTE8_4x4       = (PIXELS_4x4 << 2) | ATTRIBUTE8_COLOR,
   ATTRIBUTE8_4x4_PEN   = (PIXELS_4x4 << 2) | ATTRIBUTE8_COLOR_PEN,
   ATTRIBUTE8_4x4_PAPER = (PIXELS_4x4 << 2) | ATTRIBUTE8_COLOR_PAPER,
   ATTRIBUTE16_4x4      = (PIXELS_4x4 << 2) | ATTRIBUTE16_COLOR,
   ATTRIBUTE8_4x8       = (PIXELS_4x8 << 2) | ATTRIBUTE8_COLOR,
   ATTRIBUTE8_4x8_PEN   = (PIXELS_4x8 << 2) | ATTRIBUTE8_COLOR_PEN,
   ATTRIBUTE8_4x8_PAPER = (PIXELS_4x8 << 2) | ATTRIBUTE8_COLOR_PAPER,
   ATTRIBUTE16_4x8      = (PIXELS_4x8 << 2) | ATTRIBUTE16_COLOR,
   ATTRIBUTE8_8x8       = (PIXELS_8x8 << 2) | ATTRIBUTE8_COLOR,
   ATTRIBUTE8_8x8_PEN   = (PIXELS_8x8 << 2) | ATTRIBUTE8_COLOR_PEN,
   ATTRIBUTE8_8x8_PAPER = (PIXELS_8x8 << 2) | ATTRIBUTE8_COLOR_PAPER,
   ATTRIBUTE16_8x8      = (PIXELS_8x8 << 2) | ATTRIBUTE16_COLOR,
};

// Attributes' dimensions (value as `enum VideoElementDimension`).
// Possible values are up to 8x8, i.e. PIXELS_<2x4|4x4|4x8|8x8>.
#define ATTRIBUTE_DIMENSION     ((VIDEO_MODE >> 2) & 3)

// Attributes' color type (value is as `enum AttributeColorType`)
#define ATTRIBUTE_COLOR_TYPE    (VIDEO_MODE & 3)

// Attributes' full type (value is as `enum AttributeType`)
#define ATTRIBUTE_TYPE          VideoModeLowNibble()


//------------------------------------------------------------------------------
// Attribute measurements: Simple macros giving their Width and Height (pixels),
// Size (in bytes), the numbers of Colors, as well as the Log2 of those values.
//------------------------------------------------------------------------------

#define ATTRIBUTE_SIZE               (1 << ATTRIBUTE_SIZE_LOG2)
#define ATTRIBUTE_WIDTH              (2 << ((ATTRIBUTE_DIMENSION + 1) >> 1))
#define ATTRIBUTE_HEIGHT             (4 << BITS_GET_BIT(VIDEO_MODE, 3))
#define ATTRIBUTE_COUNT_BITS         (8 << ATTRIBUTE_SIZE_LOG2)
#define ATTRIBUTE_COUNT_PIXELS       (8 << ATTRIBUTE_DIMENSION)
#define ATTRIBUTE_COUNT_COLOR        (1 << ATTRIBUTE_COUNT_COLOR_LOG2)

#define ATTRIBUTE_SIZE_LOG2          (ATTRIBUTE_COLOR_TYPE == ATTRIBUTE16_COLOR)
#define ATTRIBUTE_WIDTH_LOG2         ((ATTRIBUTE_DIMENSION + 3) >> 1)
#define ATTRIBUTE_HEIGHT_LOG2        (2 + BITS_GET_BIT(VIDEO_MODE, 3))
#define ATTRIBUTE_COUNT_BITS_LOG2    (3 + (ATTRIBUTE_COLOR_TYPE == ATTRIBUTE16_COLOR))
#define ATTRIBUTE_COUNT_PIXELS_LOG2  (ATTRIBUTE_DIMENSION + 3)
#define ATTRIBUTE_COUNT_COLOR_LOG2   (4 << (ATTRIBUTE_COLOR_TYPE != ATTRIBUTE8_COLOR))
   // Note: ATTRIBUTE_SIZE_LOG2 can also been seen as a boolean which is true
   //       iff Attributes have two bytes.

// ^ Implementation notes about the macros above:
//
// The Video.mode is as follow when it contains attributes:
//
//                    |Type |
//              |1|***|..|..|
//                     ^  ^
//      Dimension  <---'  '--->  ColorType
//  00: PIXELS_2x4 = 0       00: ATTRIBUTE8_COLOR       = 0
//  01: PIXELS_4x4 = 1       01: ATTRIBUTE8_COLOR_PEN   = 1
//  10: PIXELS_4x8 = 2       10: ATTRIBUTE8_COLOR_PAPER = 2
//  11: PIXELS_8x8 = 3       11: ATTRIBUTE16_COLOR      = 3
//
// * Notice that Height only depends on bit #3 of the Video.mode bit, thus:
//       Height       := 4 << BITS_GET_BIT(Mode, 3)
//       log2(Height) := 2 + BITS_GET_BIT(Mode, 3)
//
// * Attributes' Size depends on color type being ATTRIBUTE16_COLOR, thus:
//   log2(Size)   :=   ColorType == ATTRIBUTE16_COLOR
//
// * Attributes' Color depth is 4/8 depending on color type being ATTRIBUTE8_COLOR:
//   log2(#Color) :=   4 << (ColorType != ATTRIBUTE8_COLOR)


//------------------------------------------------------------------------------
// Attributes measurement macros optimized for when the Attribute<8|16> C type
// is known.
//------------------------------------------------------------------------------

#define ATTRIBUTE8_SIZE                1
#define ATTRIBUTE8_WIDTH               ATTRIBUTE_WIDTH
#define ATTRIBUTE8_HEIGHT              ATTRIBUTE_HEIGHT
#define ATTRIBUTE8_COUNT_BITS          8
#define ATTRIBUTE8_COUNT_PIXELS        ATTRIBUTE_COUNT_PIXELS
#define ATTRIBUTE8_COUNT_COLOR         (1 << ATTRIBUTE8_COUNT_COLOR_LOG2)
#define ATTRIBUTE8_SIZE_LOG2           0
#define ATTRIBUTE8_WIDTH_LOG2          ATTRIBUTE_WIDTH_LOG2
#define ATTRIBUTE8_HEIGHT_LOG2         ATTRIBUTE_HEIGHT_LOG2
#define ATTRIBUTE8_COUNT_BITS_LOG2     3
#define ATTRIBUTE8_COUNT_PIXELS_LOG2   ATTRIBUTE_COUNT_PIXELS_LOG2
#define ATTRIBUTE8_COUNT_COLOR_LOG2    (4 << (ATTRIBUTE_COLOR_TYPE != ATTRIBUTE8_COLOR))

#define ATTRIBUTE16_SIZE               2
#define ATTRIBUTE16_WIDTH              ATTRIBUTE_WIDTH
#define ATTRIBUTE16_HEIGHT             ATTRIBUTE_HEIGHT
#define ATTRIBUTE16_COUNT_BITS         16
#define ATTRIBUTE16_COUNT_PIXELS       ATTRIBUTE_COUNT_PIXELS
#define ATTRIBUTE16_COUNT_COLOR        256
#define ATTRIBUTE16_SIZE_LOG2          1
#define ATTRIBUTE16_WIDTH_LOG2         ATTRIBUTE_WIDTH_LOG2
#define ATTRIBUTE16_HEIGHT_LOG2        ATTRIBUTE_HEIGHT_LOG2
#define ATTRIBUTE16_COUNT_BITS_LOG2    4
#define ATTRIBUTE16_COUNT_PIXELS_LOG2  ATTRIBUTE_COUNT_PIXELS_LOG2
#define ATTRIBUTE16_COUNT_COLOR_LOG2   8


//------------------------------------------------------------------------------

/*

Use or not?

// _Generic statement for Attibutes
// It selects `F<8|16>_` based on which Attribute<8|16> type `glyph` really is.
// Extra code may be be inserted in the _Generic via the ... parameter(s).
#define ATTRIBUTE_Generic(attr, F, ...)  \
   _Generic((attr)                    ,  \
      Attribute16: F##16_             ,  \
      Attribute8:  F##8_   __VA_OPT__(,) \
      __VA_ARGS__                        \
   )

Probably, it should be more like this below,
to allow

// _Generic statement for Attibutes
// It selects `F16_` if `attr` is of type Attribute16, or selects `F8_`
// otherwise. Extra code may be inserted in the _Generic via the ...
// parameter(s).
#define ATTRIBUTE_Generic(attr, F, ...)  \
   _Generic((attr)                    ,  \
      Attribute16: F##16_             ,  \
      __VA_ARGS__          __VA_OPT__(,) \
      default:     F##8_                 \
   )


*/

//------------------------------------------------------------------------------
// Attributes Access: Get(), Set(), ...
//
// Attributes are manipulated here via byte pointers to the framebuffer
//------------------------------------------------------------------------------

// void AttributeSetAll([Video|Rectangle r], Attribute<8|16> attribute);
// Set all the Attributes in the surface.
#define AttributeSetAll(...) \
   UTIL_OVERLOAD(PRIVATE_AttributeSetAll, __VA_ARGS__)
#  define PRIVATE_AttributeSetAll_1_(attribute)  \
      PRIVATE_AttributeSetAll_Rectangle_(Video.active, (attribute))
#  define PRIVATE_AttributeSetAll_2_(surface, attribute)                       \
      _Generic((surface)                                                     , \
         VideoSurface_: PRIVATE_AttributeSetAll_Video_((attribute))          , \
         Rectangle:     PRIVATE_AttributeSetAll_Rectangle_(RECTANGLE((surface)), (attribute))  \
      )
#  define PRIVATE_AttributeSetAll_Video_(attribute)                            \
      _Generic((attribute)                                                   , \
         Attribute16: PRIVATE_AttributeSetAll_Video_16_                      , \
         default:     PRIVATE_AttributeSetAll_Video_8_                         \
      )((attribute))
#  define PRIVATE_AttributeSetAll_Rectangle_(rectangle, attribute)             \
      _Generic((attribute)                                                   , \
         Attribute16: PRIVATE_AttributeSetAll_Rectangle_16_                  , \
         default:     PRIVATE_AttributeSetAll_Rectangle_8_                     \
      )((rectangle), (attribute))
   static inline void PRIVATE_AttributeSetAll_Video_8_(int a8) {
      assert(ATTRIBUTE_COLOR_TYPE != ATTRIBUTE16_COLOR);
      memset(VIDEO_ATTRIBUTE_START_, a8, VIDEO_ATTRIBUTES_SIZE);
   }
   static inline void PRIVATE_AttributeSetAll_Video_16_(Attribute16 a16) {
      assert(ATTRIBUTE_COLOR_TYPE == ATTRIBUTE16_COLOR);
      for (int i = 0; i < VIDEO_COUNT_ATTRIBUTE16; i++)
         VIDEO_ATTRIBUTE16_START_[i] = a16;
   }
   static inline void
   PRIVATE_AttributeSetAll_Rectangle_8_(Rectangle r, int a8) {
      assert(ATTRIBUTE_COLOR_TYPE != ATTRIBUTE16_COLOR);
      r = RectangleIntersection(r,
         (Rectangle){ 0,0, VIDEO_WIDTH_ATTRIBUTE8, VIDEO_HEIGHT_ATTRIBUTE8 }
      );
      VideoArray attr = VIDEO_ATTRIBUTE(8, r.x0, r.y0);
      for (int y = 0; y < r.height; y++)
         for (int x = 0; x < r.width; x++) {
            attr[y][x] = a8;
         }
   }
   static inline void
   PRIVATE_AttributeSetAll_Rectangle_16_(Rectangle r, Attribute16 a16) {
      assert(ATTRIBUTE_COLOR_TYPE == ATTRIBUTE16_COLOR);
      r = RectangleIntersection(r,
         (Rectangle){ 0,0, VIDEO_WIDTH_ATTRIBUTE16, VIDEO_HEIGHT_ATTRIBUTE16 }
      );
      VideoArray attr = VIDEO_ATTRIBUTE(16, r.x0, r.y0);
      for (int y = 0; y < r.height; y++)
         for (int x = 0; x < r.width; x++)
            attr[y][x] = a16;
   }

// TODO: Deprecate?
//
// For Raw access,  prefer:
//   VIDEO_ATTRIBUTE(<8|16>)[y][x];  // but this requires you know the type)
// For Safe access, prefer:
//   AttributeGet(Video, x,y) or AttributeSet(Video, x,y, ...);
 //  ^ TODO: implement those.
//
// Return pointer to attribute at (x,y)
// No bound checking, x/y MUST be positive and < VIDEO_WIDTH/HEIGHT_ATTRIBUTE
static inline uint8_t *VideoAttributeAt_(int x, int y)
{
   assert(x >= 0 && x < VIDEO_WIDTH_ATTRIBUTE);
   assert(y >= 0 && y < VIDEO_HEIGHT_ATTRIBUTE);
   int index = x + y * VIDEO_WIDTH_ATTRIBUTE;
   return VIDEO_ATTRIBUTE_START_ + (index << ATTRIBUTE_SIZE_LOG2);
}


// Return the pen color of an Attribute
static inline uint8_t AttributeGetPen(const uint8_t *attr)
{
   switch (ATTRIBUTE_COLOR_TYPE) {
      case ATTRIBUTE16_COLOR:      // fallthrough
      case ATTRIBUTE8_COLOR_PEN:   return *attr;
      case ATTRIBUTE8_COLOR:       return *attr >> 4;
      case ATTRIBUTE8_COLOR_PAPER: return Video.attr_default_pen;
      default: unreachable();
   }
}

// Set the pen color of an Attribute
static inline void AttributeSetPen(uint8_t *attr, uint8_t color)
{
   switch (ATTRIBUTE_COLOR_TYPE) {
      case ATTRIBUTE16_COLOR:      // fallthrough
      case ATTRIBUTE8_COLOR_PEN:   *attr = color; break;
      case ATTRIBUTE8_COLOR:       *attr = (color << 4) | (*attr & 0xF); break;
      case ATTRIBUTE8_COLOR_PAPER: break;
      default: unreachable();
   }
}

// Return the paper color of an Attribute
static inline uint8_t AttributeGetPaper(const uint8_t *attr)
{
   switch (ATTRIBUTE_COLOR_TYPE) {
      case ATTRIBUTE8_COLOR:       return *attr & 0xF;
      case ATTRIBUTE8_COLOR_PAPER: return *attr;
      case ATTRIBUTE16_COLOR:      return attr[1];
      case ATTRIBUTE8_COLOR_PEN:   return Video.attr_default_paper;
      default: unreachable();
   }
}

// Set the paper color of an Attribute
static inline void AttributeSetPaper(uint8_t *attr, uint8_t color)
{
   switch (ATTRIBUTE_COLOR_TYPE) {
      case ATTRIBUTE8_COLOR:       *attr = (*attr & 0xF0) | color; break;
      case ATTRIBUTE8_COLOR_PAPER: *attr = color;   break;
      case ATTRIBUTE16_COLOR:      attr[1] = color; break;
      case ATTRIBUTE8_COLOR_PEN:   break;
      default: unreachable();
   }
}

// Set the pen and paper colors of an Attribute (when possible)
static inline void
AttributeSet(uint8_t *attr, uint8_t pen_color, uint8_t paper_color)
{
   switch (ATTRIBUTE_COLOR_TYPE) {
      case ATTRIBUTE16_COLOR:       attr[1] = paper_color; // fallthrough
      case ATTRIBUTE8_COLOR_PEN:   *attr = pen_color;   break;
      case ATTRIBUTE8_COLOR_PAPER: *attr = paper_color; break;
      case ATTRIBUTE8_COLOR: *attr = (pen_color << 4) | paper_color; break;
      default: unreachable();
   }
}

// Swap the pen and paper colors of an attribute (when possible)
static inline void AttributeReverse(uint8_t *attr)
{
   switch (ATTRIBUTE_COLOR_TYPE) {
      case ATTRIBUTE8_COLOR:
         *attr = (*attr << 4) | (*attr >> 4);
         break;
      case ATTRIBUTE16_COLOR:
         { uint8_t tmp = attr[0]; attr[1] = attr[0]; attr[0] = tmp; }
         break;
      // We cannot video-reverse those:
      case ATTRIBUTE8_COLOR_PEN:   break;
      case ATTRIBUTE8_COLOR_PAPER: break;
      default: unreachable();
   }
}

#endif //include guard
