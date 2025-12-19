#ifndef  KONPU_TILE_H_
#define  KONPU_TILE_H_
#include "video_mode.h"

//------------------------------------------------------------------------------
// Tiles: C Types, Chunk/Color, and (full) Type
//------------------------------------------------------------------------------

// This table summarizes the relationships between a Tile's Pixels Chunk,
// its Tile<N> C type, its Dimension:
//
// Dimension|  Tile16    Tile32    Tile64    Tile128   Tile256
// ---------+-------------------------------------------------
//   2x4    |  Quarter   Nibble    Byte
//   4x4    |            Quarter   Nibble    Byte
//   4x8    |                      Quarter   Nibble    Byte
//   8x8    |                                Quarter   Nibble
//   8x16   |                                          Quarter

// C Types
typedef Form16  Tile16;
typedef Form32  Tile32;
typedef Form64  Tile64;
typedef Form128 Tile128;
typedef Form256 Tile256;

// The (full) type of a Tile
// It is in fact the Video.mode when using such a tile, thus combining:
// * its dimension (which can be PIXELS_<2x4|4x4|4x8|8x8|8x16>)
// * and its pixels' chunk id
//
// It is used to pass to the VIDEO_MODE_TILE(...) macro to give a Video mode
enum TileType {
   TILE16_2x4   = (PIXELS_2x4  << 4) | PIXEL_CHUNK_QUARTER,  //   4 colors
   TILE32_2x4   = (PIXELS_2x4  << 4) | PIXEL_CHUNK_NIBBLE,   //  16 colors
   TILE32_4x4   = (PIXELS_4x4  << 4) | PIXEL_CHUNK_QUARTER,  //   4 colors
   TILE64_2x4   = (PIXELS_2x4  << 4) | PIXEL_CHUNK_BYTE,     // 256 colors
   TILE64_4x4   = (PIXELS_4x4  << 4) | PIXEL_CHUNK_NIBBLE,   //  16 colors
   TILE64_4x8   = (PIXELS_4x8  << 4) | PIXEL_CHUNK_QUARTER,  //   4 colors
   TILE128_4x4  = (PIXELS_4x4  << 4) | PIXEL_CHUNK_BYTE,     // 256 colors
   TILE128_4x8  = (PIXELS_4x8  << 4) | PIXEL_CHUNK_NIBBLE,   //  16 colors
   TILE128_8x8  = (PIXELS_8x8  << 4) | PIXEL_CHUNK_QUARTER,  //   4 colors
   TILE256_4x8  = (PIXELS_4x8  << 4) | PIXEL_CHUNK_BYTE,     // 256 colors
   TILE256_8x8  = (PIXELS_8x8  << 4) | PIXEL_CHUNK_NIBBLE,   //  16 colors
   TILE256_8x16 = (PIXELS_8x16 << 4) | PIXEL_CHUNK_QUARTER,  //   4 colors
};

// Tiles' full type (value is as `enum TileType`)
#define TILE_TYPE          VIDEO_MODE

// Tiles' pixels chunck id (value as `enum PixelChunk`)
#define TILE_PIXEL_CHUNK   (TILE_TYPE & 0xF)

// Tiles' dimensions (value as `enum VideoElementDimension`).
// Possible values are PIXELS_<2x4|4x4|4x8|8x8|8x16>
#define TILE_DIMENSION     (TILE_TYPE >> 4)


//------------------------------------------------------------------------------
// Tile measurements: Simple macros giving their Width and Height (pixels),
// Size (in bytes), the numbers of Bit (ie = count of pixels), as well as the
// Log2 of those values.
//------------------------------------------------------------------------------

#define TILE_SIZE                  (1 << TILE_SIZE_LOG2)
#define TILE_WIDTH                 (2 << ((TILE_DIMENSION + 1) >> 1))
#define TILE_HEIGHT                (4 << (TILE_TYPE >> 5))
#define TILE_COUNT_BITS            (8 << TILE_SIZE_LOG2)
#define TILE_COUNT_PIXELS          (8 << TILE_DIMENSION)
#define TILE_COUNT_COLOR           (1 << TILE_COUNT_COLOR_LOG2)

#define TILE_SIZE_LOG2             ((TILE_TYPE >> 4) + (TILE_TYPE & 3))
#define TILE_WIDTH_LOG2            ((TILE_DIMENSION + 3) >> 1)
#define TILE_HEIGHT_LOG2           (2 + (TILE_TYPE >> 5))
#define TILE_COUNT_BITS_LOG2       (3 + (TILE_TYPE >> 4) + (TILE_TYPE & 3))
#define TILE_COUNT_PIXELS_LOG2     (TILE_DIMENSION + 3)
#define TILE_COUNT_COLOR_LOG2      (1 << (TILE_TYPE & 3))

// ^-- Implementation notes about the macros above:
//
// - Formula for H and log2(H) obtained by replacing Dimension=TILE_TYPE>>4 into
//   the Video Element formulas H=4<<(Dimension>>1) and log2(H)=2+(Dimension>>1)
//
// - log2(#bits) = log2(#pixels * color_depth)
//               = log2(#pixels) + log2(color_depth)
//               = (TILE_DIMENSION + 3) + log2(1 << (TILE_TYPE & 3))
//               = 3 + (TILE_TYPE >> 4) + (TILE_TYPE & 3)
// - log2(size) = log2(#bits) - 3


//------------------------------------------------------------------------------
// Tile measurement macros optimized for when the Tile<16|32|64|128|256> C type
// is known.
//------------------------------------------------------------------------------

#define TILE16_SIZE                2
#define TILE16_WIDTH               2
#define TILE16_HEIGHT              4
#define TILE16_COUNT_BITS          16
#define TILE16_COUNT_PIXELS        8
#define TILE16_COUNT_COLOR         4
#define TILE16_SIZE_LOG2           1
#define TILE16_WIDTH_LOG2          1
#define TILE16_HEIGHT_LOG2         2
#define TILE16_COUNT_BITS_LOG2     4
#define TILE16_COUNT_PIXELS_LOG2   3
#define TILE16_COUNT_COLOR_LOG2    2

#define TILE32_SIZE                4
#define TILE32_WIDTH               (2 << (TILE_TYPE == TILE32_4x4))
#define TILE32_HEIGHT              4
#define TILE32_COUNT_BITS          32
#define TILE32_COUNT_PIXELS        (8 << (TILE_TYPE == TILE32_4x4))
#define TILE32_COUNT_COLOR         (1 << TILE32_COUNT_COLOR_LOG2)
#define TILE32_SIZE_LOG2           2
#define TILE32_WIDTH_LOG2          (1 + (TILE_TYPE == TILE32_4x4))
#define TILE32_HEIGHT_LOG2         2
#define TILE32_COUNT_BITS_LOG2     5
#define TILE32_COUNT_PIXELS_LOG2   (3 + (TILE_TYPE == TILE32_4x4))
#define TILE32_COUNT_COLOR_LOG2    (2 << (TILE_TYPE == TILE32_2x4))

#define TILE64_SIZE                8
#define TILE64_WIDTH               (2 << (TILE_TYPE != TILE64_2x4))
#define TILE64_HEIGHT              (4 << (TILE_TYPE == TILE64_4x8))
#define TILE64_COUNT_BITS          64
#define TILE64_COUNT_PIXELS        TILE_COUNT_PIXELS
#define TILE64_COUNT_COLOR         TILE_COUNT_COLOR
#define TILE64_SIZE_LOG2           3
#define TILE64_WIDTH_LOG2          (1 + (TILE_TYPE != TILE64_2x4))
#define TILE64_HEIGHT_LOG2         (2 + (TILE_TYPE == TILE64_4x8))
#define TILE64_COUNT_BITS_LOG2     6
#define TILE64_COUNT_PIXELS_LOG2   TILE_COUNT_PIXELS_LOG2
#define TILE64_COUNT_COLOR_LOG2    TILE_COUNT_COLOR_LOG2

#define TILE128_SIZE               16
#define TILE128_WIDTH              (4 << (TILE_TYPE == TILE128_8x8))
#define TILE128_HEIGHT             (4 << (TILE_TYPE != TILE128_4x4))
#define TILE128_COUNT_BITS         128
#define TILE128_COUNT_PIXELS       TILE_COUNT_PIXELS
#define TILE128_COUNT_COLOR        TILE_COUNT_COLOR
#define TILE128_SIZE_LOG2          4
#define TILE128_WIDTH_LOG2         (2 + (TILE_TYPE == TILE128_8x8))
#define TILE128_HEIGHT_LOG2        (2 + (TILE_TYPE != TILE128_4x4))
#define TILE128_COUNT_BITS_LOG2    7
#define TILE128_COUNT_PIXELS_LOG2  TILE_COUNT_PIXELS_LOG2
#define TILE128_COUNT_COLOR_LOG2   TILE_COUNT_COLOR_LOG2

#define TILE256_SIZE               32
#define TILE256_WIDTH              (4 << (TILE_TYPE != TILE256_4x8))
#define TILE256_HEIGHT             (8 << (TILE_TYPE == TILE256_8x16))
#define TILE256_COUNT_BITS         256
#define TILE256_COUNT_PIXELS       TILE_COUNT_PIXELS
#define TILE256_COUNT_COLOR        TILE_COUNT_COLOR
#define TILE256_SIZE_LOG2          5
#define TILE256_WIDTH_LOG2         (2 + (TILE_TYPE != TILE256_4x8))
#define TILE256_HEIGHT_LOG2        (3 + (TILE_TYPE == TILE256_8x16))
#define TILE256_COUNT_BITS_LOG2    8
#define TILE256_COUNT_PIXELS_LOG2  TILE_COUNT_PIXELS_LOG2
#define TILE256_COUNT_COLOR_LOG2   TILE_COUNT_COLOR_LOG2


//------------------------------------------------------------------------------

/*
// _Generic statement for Tiles
// It selects `F<N>_` based on which Tile<N> type `tile` really is.
// Extra code may be be inserted in the _Generic via the ... parameter(s).
#define TILE_Generic(tile, F, ...)    \
   _Generic((tile)                 ,  \
      Tile16:  F##16_              ,  \
      Tile32:  F##32_              ,  \
      Tile64:  F##64_              ,  \
      Tile128: F##128_             ,  \
      Tile256: F##256_  __VA_OPT__(,) \
      __VA_ARGS__                     \
   )
*/

#endif //include guard
