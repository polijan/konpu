#ifndef  KONPU_TILE_H
#define  KONPU_TILE_H
#include "glyph.h"

//------------------------------------------------------------------------------
// Tiles typedefs
//------------------------------------------------------------------------------

// Names with only the size in bytes.

typedef uint16_t  Tile16;   //  2x4 2bpp
typedef uint32_t  Tile32;   //  4x4 2bpp, or 2x4 4bpp
typedef uint64_t  Tile64;   //  4x8 2bpp, or 4x4 4bpp, or 2x4 8bpp
typedef Glyph128  Tile128;  //  8x8 2bpp, or 4x8 4bpp, or 4x4 8bpp
typedef Glyph256  Tile256;  // 8x16 2bpp, or 8x8 4bpp, or 4x8 8bpp

// names with the chunck (Quarter = 2bpp, Nibble = 4bpp, Byte = 8bpp)

//TileQuarter2x4
//TileQuarter4x4
//TileQuarter4x8
//TileQuarter8x8
//TileQuarter8x16
//TileNibble2x4
//TileNibble4x4
//TileNibble4x8
//TileNibble8x8
//TileByte2x4
//TileByte4x4
//TileByte4x8

//Tile2x4_2bpp;
//TileQuarter4x4;
//TileQuarter4x8;
//TileQuarter8x8;
//TileQuarter8x16;

typedef uint16_t  TileQuarter16;   //  2x4 2-bits (= 4 colors) tile
typedef uint32_t  TileQuarter32;   //  4x4 2-bits (= 4 colors) tile
typedef uint32_t  TileQuarter64;   //  4x8 2-bits (= 4 colors) tile
typedef Glyph128  TileQuarter128;  //  8x8 2-bits (= 4 colors) tile
typedef Glyph256  TileQuarter256;  // 8x16 2-bits (= 4 colors) tile

typedef uint32_t  TileNibble32;    //  2x4 4-bits (= 16 colors) tile
typedef uint64_t  TileNibble64;    //  4x4 4-bits (= 16 colors) tile
typedef Glyph128  TileNibble128;   //  4x8 4-bits (= 16 colors) tile
typedef Glyph256  TileNibble256;   //  8x8 4-bits (= 16 colors) tile

typedef uint64_t  TileByte64;      //  2x4 8-bits (=256 colors) tile
typedef Glyph128  TileByte128;     //  4x4 8-bits (=256 colors) tile
typedef Glyph256  TileByte256;     //  4x8 8-bits (=256 colors) tile


//------------------------------------------------------------------------------
// Measurements
//------------------------------------------------------------------------------

// Width and Height constants:
#define TILE_QUARTER16_WIDTH  2
#define TILE_QUARTER32_WIDTH  4
// TODO ...

//------------------------------------------------------------------------------

// Tile<Quarter|Nibble|Byte><Shift|Cycle><Up|Down|Left|Right>
// Tile<Quarter|Nibble|Byte>Rotate<90|180|270>
// Tile<>Rotate<90|180|270>
// ...

#endif //KONPU_TILE_H
