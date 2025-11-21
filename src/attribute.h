#ifndef  KONPU_ATTRIBUTE_H_
#define  KONPU_ATTRIBUTE_H_

// Attributes are pieces of information contained in the video framebuffer which
// indicate the color of a glyph's pixels. They're as manipulated here via a
// byte pointer in the framebuffer and the attribute size is one or two bytes
// depending on the video mode (see: video_mode.h)
#include "video_mode.h"

//------------------------------------------------------------------------------
// Attributes Dimension
//------------------------------------------------------------------------------

// In attribute modes, returns the attributes pixel size.
// Note: Return value is one of: PIXELS_2x4, PIXELS_4x4, PIXELS_4x8, PIXELS_8x8.
static inline enum VideoElementDimension
AttributeDimension(void)         { return (VIDEO_MODE >> 2) & 3; }

// In attribute modes, returns the log2 of an attributes' pixel width.
// Thus, the width of an attribute is: 1 << AttributeWidthLog2().
#define AttributeWidthLog2()     (((VIDEO_MODE >> 2 & 3) + 3) >> 1)
   //                            VIDEO_MODE's attr dimension: |0|1|2|3|
   //                            log2(attribute height)     : |1|2|2|3|
   //                            thus, we apply (x+3)/2 to attributes' dimension

// In attribute modes, returns the log2 of an attributes' pixel height.
// Thus, the height of an attribute is: 1 << AttributeHeightLog2().
#define AttributeHeightLog2()    (2 + (VIDEO_MODE >> 3 & 1))
   //                            VIDEO_MODE's attr dimension: |0|1|2|3|
   //                            log2(attribute height):      |2|2|3|3|
   //                            thus, we apply 2+x/2 to attribtues' dimension

// In attribute modes, returns 0 if attributes point to one byte,
//                          or 1 if they point to two bytes.
// Note: The return value also represents the log2 of the size in bytes
//       that attributes point to.
#define AttributeHasTwoBytes()   ((VIDEO_MODE & 3) == ATTRIBUTE_COLORS_256)


////////////////////////////////////////////////////////////////////////////////
// TODO

// Return one of the following `enum VideoElementDimension`
// PIXELS_2x4, PIXELS_4x4, PIXELS_4x8, PIXELS_8x8.
#define ATTRIBUTE_DIMENSION      ((VIDEO_MODE >> 2) & 3)

#define ATTRIBUTE_WIDTH_LOG2     (((VIDEO_MODE >> 2 & 3) + 3) >> 1)
#define ATTRIBUTE_HEIGHT_LOG2    (2 + (VIDEO_MODE >> 3 & 1))
#define ATTRIBUTE_WIDTH          (1 << ATTRIBUTE_WIDTH_LOG2)
#define ATTRIBUTE_HEIGHT         (1 << ATTRIBUTE_HEIGHT_LOG2)
                                 //^also works: (4 << (ATTRIBUTE_DIMENSION > 1))

// attribute's dimension
//
// Dimension, D = |0|1|2|3|    Formula in D |
//----------------+-+-+-+-+------------------------------------
// width        = |2|4|4|8| =>

// height       = |4|4|8|8| => 4 << (D>1)
// log2(width)  = |1|2|2|3| => (D+3)/2
// log2(height) = |2|2|3|3| => 2 + D/2
//                            thus, we apply (x+3)/2 to attributes' dimension

// size of Attribute in bytes (one or two bytes)
#define ATTRIBUTE_SIZE           ((VIDEO_MODE & 3) - 1)


// TODO - Are those Needed???
// #define AttributeWidth()       ???
// #define AttributeHeight()     (4 << (VIDEO_MODE >> 3 & 1))
//                               // ^-- faster than 1 << AttributeHeightLog2()
////////////////////////////////////////////////////////////////////////////////





// In attribute modes, return the VIDEO_MODE attributes' type
static inline enum AttributeColorType
AttributeColorType(void) {
   // assert(VideoModeHasAttributes());
   return VIDEO_MODE & 3;
}



//------------------------------------------------------------------------------
// Attributes Access: Get(), Set(), ...
//------------------------------------------------------------------------------

// Offset Where the attributes start
#define VIDEO_ATTRIBUTE_OFFSET   (VIDEO_COUNT_PIXELS >> 3)

// Start of the attributes in the video framebuffer
// (only make sense if the video framebuffer has attributes)
#define VIDEO_ATTRIBUTE          (Video.frame + (VIDEO_COUNT_PIXELS >> 3))

// Return pointer to attribute at (x,y)
// No bound checking, x/y MUST be positive and < VIDEO_WIDTH/HEIGHT_ATTRIBUTE
static inline uint8_t *VideoAttributeAt_(int x, int y)
{
   assert(x >= 0 && x < VIDEO_WIDTH_ATTRIBUTE);
   assert(y >= 0 && y < VIDEO_HEIGHT_ATTRIBUTE);
   int index = x + y * VIDEO_WIDTH_ATTRIBUTE;
   return VIDEO_ATTRIBUTE + (index << AttributeHasTwoBytes());
}


// Return the foreground color of an attribute
static inline uint8_t AttributeGetPen(const uint8_t *attr)
{
   switch (AttributeColorType()) {
      case ATTRIBUTE_COLORS_256:      //return attr[0]; same as: // fallthrough
      case ATTRIBUTE_COLORS_PEN256:   return *attr;
      case ATTRIBUTE_COLORS_16:       return *attr >> 4;
      case ATTRIBUTE_COLORS_PAPER256: return Video.attr_default_pen;
      default: unreachable();
   }
}

// Set the foreground color of an attribute
static inline void AttributeSetPen(uint8_t *attr, uint8_t color)
{
   switch (AttributeColorType()) {
      case ATTRIBUTE_COLORS_256:    // attr[0] = color; same as: // fallthrough
      case ATTRIBUTE_COLORS_PEN256: *attr = color; break;
      case ATTRIBUTE_COLORS_16:     *attr = (color << 4) | (*attr & 0xF); break;
      case ATTRIBUTE_COLORS_PAPER256: break;
      default: unreachable();
   }
}

// Return the background color of an attribute
static inline uint8_t AttributeGetPaper(const uint8_t *attr)
{
   switch (AttributeColorType()) {
      case ATTRIBUTE_COLORS_16:       return *attr & 0xF;
      case ATTRIBUTE_COLORS_PAPER256: return *attr;
      case ATTRIBUTE_COLORS_256:      return attr[1];
      case ATTRIBUTE_COLORS_PEN256:   return Video.attr_default_paper;
      default: unreachable();
   }
}

// Set the background color of an attribute
static inline void AttributeSetPaper(uint8_t *attr, uint8_t color)
{
   switch (AttributeColorType()) {
      case ATTRIBUTE_COLORS_16:       *attr = (*attr & 0xF0) | color; break;
      case ATTRIBUTE_COLORS_PAPER256: *attr = color; break;
      case ATTRIBUTE_COLORS_256:      attr[1] = color; break;
      case ATTRIBUTE_COLORS_PEN256:   break;
      default: unreachable();
   }
}

// Set the foreground and background colors of an attribute
static inline void
AttributeSet(uint8_t *attr, uint8_t pen_color, uint8_t paper_color)
{
   switch (AttributeColorType()) {
      case ATTRIBUTE_COLORS_16: *attr = (pen_color << 4) | paper_color; break;
      case ATTRIBUTE_COLORS_PEN256:   *attr = pen_color; break;
      case ATTRIBUTE_COLORS_PAPER256: *attr = paper_color; break;
      case ATTRIBUTE_COLORS_256: attr[0] = pen_color; attr[1] = paper_color; break;
      default: unreachable();
   }
}

// Swap the foreground and background colors of an attribute (if it's possible)
static inline void AttributeReverse(uint8_t *attr)
{
   switch (AttributeColorType()) {
      case ATTRIBUTE_COLORS_16:
         *attr = (*attr << 4) | (*attr >> 4);
         break;
      case ATTRIBUTE_COLORS_256:
         ; uint8_t tmp = attr[0]; attr[1] = attr[0]; attr[0] = tmp;
         break;
      // We can't video reverse those:
      case ATTRIBUTE_COLORS_PEN256:   break;
      case ATTRIBUTE_COLORS_PAPER256: break;
      default: unreachable();
   }
}

#endif //include guard
