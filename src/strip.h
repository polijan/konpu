#ifndef KONPU_STRIP_H_
#define KONPU_STRIP_H_
#include "video_mode.h"

//------------------------------------------------------------------------------
// Strip: Dimensions and Properties
//------------------------------------------------------------------------------

// A Strip is a byte representing a small horizontal line of N(=1|2|4|8) pixels.
// see: [packed pixels](https://en.wikipedia.org/wiki/Packed_pixel)

// The type of Strip is represented by the number of packed pixels:
//
//        |   |  Pixels | Pixel    |  # of  |
// Type   | N |  N x 1  | Chunk(s) | Colors | Video.mode's Low Nibble
// -------|---|---------|----------|--------|-----------------------------------
// Strip1 | 1 |  1 x 1  | Byte     |  256   | 11
// Strip2 | 2 |  2 x 2  | Nibbles  |   16   | 10
// Strip4 | 4 |  4 x 1  | Quarters |    4   |  9
// Strip8 | 8 |  8 x 1  | Bits     |    2   | number of Planes OR Attribute Type


enum StripVideoId_ {
   STRIP1_VIDEO_ID_ = 11,
   STRIP2_VIDEO_ID_ = 10,
   STRIP4_VIDEO_ID_ =  9,
   STRIP8_VIDEO_ID_ =  1, // Video.mode low nibble for a SINGLE Plane of Strip8
};

#define STRIP_DIMENSION   PIXELS_Nx1

// True for bit Strips (Strip8), false for "chunky" Strips (Strip<1|2|4>)
#define STRIP_HAS_BITS   (VideoModeHasAttributes() || VideoModeLowNibble() <= 8)



#define STRIP_SIZE               1
#define STRIP_WIDTH              (8 >> (STRIP_HAS_BITS ? 0 : (VIDEO_MODE & 3)))
#define STRIP_HEIGHT             1
#define STRIP_COUNT_BITS         8
#define STRIP_COUNT_PIXELS       STRIP_WIDTH
#define STRIP_COUNT_COLOR        (1 << STRIP_COUNT_COLOR_LOG2)

#define STRIP_SIZE_LOG2          0
#define STRIP_WIDTH_LOG2         (3 - (STRIP_HAS_BITS ? 0 : (VIDEO_MODE & 3)))
#define STRIP_HEIGHT_LOG2        0
#define STRIP_COUNT_BITS_LOG2    3
#define STRIP_COUNT_PIXELS_LOG2  STRIP_WIDTH_LOG2
#define STRIP_COUNT_COLOR_LOG2   (1 << (STRIP_HAS_BITS ? 0 : (VIDEO_MODE & 3)))


// Implementation Note: For "Chunky" Strips (Strip<1|2|4>), measure derive from
// the low nibble:      | 9 | 10 | 11 |
//  -----------------------------------
//  log2(log2(#colors)) =  1    2    3  =>       (MODE & 3)
//  log2(#colors)       =  2    4    8  =>  1 << (MODE & 3)
//  log2(width)         =  2    1    0  =>   3 - (MODE & 3)
//  width               =  4    2    1  =>  8 >> (MODE & 3)


// v-- Measurements when one know the Strip are "chunky", Strip<1|2|4>

#define STRIP_CHUNKY_SIZE               1
#define STRIP_CHUNKY_WIDTH              (8 >> (VIDEO_MODE & 3))
#define STRIP_CHUNKY_HEIGHT             1
#define STRIP_CHUNKY_COUNT_BITS         8
#define STRIP_CHUNKY_COUNT_PIXELS       STRIP_CHUNKY_WIDTH
#define STRIP_CHUNKY_COUNT_COLOR        (1 << STRIP_CHUNKY_COUNT_COLOR_LOG2)

#define STRIP_CHUNKY_SIZE_LOG2          0
#define STRIP_CHUNKY_WIDTH_LOG2         (3 - (VIDEO_MODE & 3))
#define STRIP_CHUNKY_HEIGHT_LOG2        0
#define STRIP_CHUNKY_COUNT_BITS_LOG2    3
#define STRIP_CHUNKY_COUNT_PIXELS_LOG2  STRIP_CHUNKY_WIDTH_LOG2
#define STRIP_CHUNKY_COUNT_COLOR_LOG2   (1 << (VIDEO_MODE & 3))


// v-- Measurements are literal constants for different of Strip types.

#define STRIP1_SIZE                     1
#define STRIP1_WIDTH                    1
#define STRIP1_HEIGHT                   1
#define STRIP1_COUNT_BITS               8
#define STRIP1_COUNT_PIXELS             1
#define STRIP1_COUNT_COLOR              256
#define STRIP1_SIZE_LOG2                0
#define STRIP1_WIDTH_LOG2               0
#define STRIP1_HEIGHT_LOG2              0
#define STRIP1_COUNT_BITS_LOG2          3
#define STRIP1_COUNT_PIXELS_LOG2        0
#define STRIP1_COUNT_COLOR_LOG2         8

#define STRIP2_SIZE                     1
#define STRIP2_WIDTH                    2
#define STRIP2_HEIGHT                   1
#define STRIP2_COUNT_BITS               8
#define STRIP2_COUNT_PIXELS             2
#define STRIP2_COUNT_COLOR              16
#define STRIP2_SIZE_LOG2                0
#define STRIP2_WIDTH_LOG2               1
#define STRIP2_HEIGHT_LOG2              0
#define STRIP2_COUNT_BITS_LOG2          3
#define STRIP2_COUNT_PIXELS_LOG2        1
#define STRIP2_COUNT_COLOR_LOG2         4

#define STRIP4_SIZE                     1
#define STRIP4_WIDTH                    4
#define STRIP4_HEIGHT                   1
#define STRIP4_COUNT_BITS               8
#define STRIP4_COUNT_PIXELS             4
#define STRIP4_COUNT_COLOR              4
#define STRIP4_SIZE_LOG2                0
#define STRIP4_WIDTH_LOG2               2
#define STRIP4_HEIGHT_LOG2              0
#define STRIP4_COUNT_BITS_LOG2          3
#define STRIP4_COUNT_PIXELS_LOG2        2
#define STRIP4_COUNT_COLOR_LOG2         2

#define STRIP8_SIZE                     1
#define STRIP8_WIDTH                    8
#define STRIP8_HEIGHT                   1
#define STRIP8_COUNT_BITS               8
#define STRIP8_COUNT_PIXELS             8
#define STRIP8_COUNT_COLOR              2
#define STRIP8_SIZE_LOG2                0
#define STRIP8_WIDTH_LOG2               3
#define STRIP8_HEIGHT_LOG2              0
#define STRIP8_COUNT_BITS_LOG2          3
#define STRIP8_COUNT_PIXELS_LOG2        3
#define STRIP8_COUNT_COLOR_LOG2         1

#endif //include guard
