#ifndef  KONPU_GLYPH_H
#define  KONPU_GLYPH_H
#include "platform.h"
#include "c.h"
#include "bits.h"

// glyph terminology:                                              uint* glyphs:
// -----------------                                               -------------
// .----------------------.--------.----------.
// | GLYPH TYPE           | BITMAP |  C TYPE  |     quadrant     wide half
// |----------------------|--------|----------|     . . . .      . . . . . . . .
// | quadrant             |  4x4   | uint16_t |     . . . .      . . . . . . . .
// |----------------------|--------|----------|     . . . .      . . . . . . . .
// | half:                |        |          |     . . . .      . . . . . . . .
// |  - tall half         |  4x8   | uint32_t |
// |  - wide half         |  8x4   |          |
// |----------------------|--------|----------|     tall half    glyph
// | (standard) glyph     |  8x8   | uint64_t |     . . . .      . . . . . . . .
// |----------------------|--------|----------|     . . . .      . . . . . . . .
// | pair:                |        | (struct) |     . . . .      . . . . . . . .
// |  - tall pair         |  8x16  |   pair   |     . . . .      . . . . . . . .
// |  - wide pair         |  16x8  |          |     . . . .      . . . . . . . .
// |----------------------|--------|----------|     . . . .      . . . . . . . .
// | tetra                | 16x16  | (struct) |     . . . .      . . . . . . . .
// |                      |        |  tetra   |     . . . .      . . . . . . . .
// '----------------------'--------'----------'
//                                                          struct-based glyphs:
//                                                          --------------------
//
// tetra                              tall pair          wide pair
// (.top_left)        (.top_right)           (.first)
// . . . . . . . .|. . . . . . . .    . . . . . . . .    . . . . . . . .|. . . . . . . .
// . . . . . . . .|. . . . . . . .    . . . . . . . .    . . . . . . . .|. . . . . . . .
// . . . . . . . .|. . . . . . . .    . . . . . . . .    . . . . . . . .|. . . . . . . .
// . . . . . . . .|. . . . . . . .    . . . . . . . .    . . . . . . . .|. . . . . . . .
// . . . . . . . .|. . . . . . . .    . . . . . . . .    . . . . . . . .|. . . . . . . .
// . . . . . . . .|. . . . . . . .    . . . . . . . .    . . . . . . . .|. . . . . . . .
// . . . . . . . .|. . . . . . . .    . . . . . . . .    . . . . . . . .|. . . . . . . .
// ._._._._._._._.|._._._._._._._.    ._._._._._._._.    . . . . . . . .|. . . . . . . .
// . . . . . . . .|. . . . . . . .    . . . . . . . .    (.first)        (.second)
// . . . . . . . .|. . . . . . . .    . . . . . . . .
// . . . . . . . .|. . . . . . . .    . . . . . . . .
// . . . . . . . .|. . . . . . . .    . . . . . . . .
// . . . . . . . .|. . . . . . . .    . . . . . . . .
// . . . . . . . .|. . . . . . . .    . . . . . . . .
// . . . . . . . .|. . . . . . . .    . . . . . . . .
// . . . . . . . .|. . . . . . . .    . . . . . . . .
// (.bottom_left)  (.bottom_right)          (.second)

//------------------------------------------------------------------------------
// structures:

// construct non-struct glpyhs from C literals
#define QUADRANT(lit_hex)   UINT16_C(0x##lit_hex)
// #define HALF(lit_hex)    UINT32_C(0x##lit_hex)
#define TALLHALF(lit_hex)   UINT32_C(0x##lit_hex)
#define WIDEHALF(lit_hex)   UINT32_C(0x##lit_hex)
#define GLYPH(lit_hex)      UINT64_C(0x##lit_hex)

// pair glyphs (tall and wide pairs)
typedef struct pair {                     // anonymous unions are only supported
#if (__STDC_VERSION__ < 201112L)          // starting from C11, so to remain
           uint64_t first;                // portable with C99, only use .first
           uint64_t second;               // and .second as the names of fields,
#else                                     //
   union { uint64_t first,                // ... but ...
                    top, left, x; };      //
   union { uint64_t second,               // if C99 portability isn't needed,
                    bottom, right, y; };  // you may wish to refer to fields of:
#endif                                    // - a wide pair, as .left and .right,
} pair;                                   // - a tall pair, as .top  and .bottom

// tetra glyph:
typedef struct tetra {
   uint64_t top_left;     uint64_t top_right;
   uint64_t bottom_left;  uint64_t bottom_right;
} tetra;

// width/height constants
// TODO: we chose a glyph to be a uint64_t, so a 8x8 size is natrual and set in
// stone and there are raw "8" literals floating around in the source code.
// Let's try to use those macros instead in most cases.
#define GLYPH_WIDTH       8
#define GLYPH_HEIGHT      8
//
#define QUADRANT_WIDTH    (GLYPH_WIDTH  / 2)
#define QUADRANT_HEIGHT   (GLYPH_HEIGHT / 2)
#define TALLHALF_WIDTH    (GLYPH_WIDTH  / 2)
#define TALLHALF_HEIGHT   GLYPH_HEIGHT
#define WIDEHALF_WIDTH    GLYPH_WIDTH
#define WIDEHALF_HEIGHT   (GLYPH_HEIGHT / 2)
#define TALLPAIR_WIDTH    GLYPH_WIDTH
#define TALLPAIR_HEIGHT   (GLYPH_HEIGHT * 2)
#define WIDEPAIR_WIDTH    (GLYPH_WIDTH  * 2)
#define WIDEPAIR_HEIGHT   GLYPH_HEIGHT
#define TETRA_WIDTH       (GLYPH_WIDTH  * 2)
#define TETRA_HEIGHT      (GLYPH_HEIGHT * 2)


//------------------------------------------------------------------------------
// glyphs (unchecked) operations on individual pixel(s)
// fromPixel(x,y)     => return a glyph containing just the pixel(x,y)
// pixelValue(g, x,y) => return the value (0 or 1) of pixel(x,y) in the given glyph
// pixelIndex(x,y)    => return the index of the bit representing the pixel(x,y)

#define   glyph_fromPixel(x,y)                 (GLYPH(1) << glyph_pixelIndex((x),(y)))
#define   glyph_pixelValue(glyph, x,y)         uint_bitValue((glyph), glyph_pixelIndex((x),(y)))
#define   glyph_pixelIndex(x,y)                ((x) + GLYPH_WIDTH * (y))

#define   quadrant_fromPixel(x,y)              (QUADRANT(1) << quadrant_pixelIndex((x),(y)))
#define   quadrant_pixelValue(quadrant, x,y)   uint_bitValue((quadrant), quadrant_pixelIndex((x),(y)))
#define   quadrant_pixelIndex(x,y)             ((x) + QUADRANT_WIDTH * (y))

#define   tallhalf_fromPixel(x,y)              (TALLHALF(1) << tallhalf_pixelIndex((x),(y)))
#define   tallhalf_pixelValue(tallhalf, x,y)   uint_bitValue((tallhalf), tallhalf_pixelIndex((x),(y)))
#define   tallhalf_pixelIndex(x,y)             ((x) + TALLHALF_WIDTH * (y))

#define   widehalf_fromPixel(x,y)              (WIDEHALF(1) << widehalf_pixelIndex((x),(y)))
#define   widehalf_pixelValue(quandrant, x,y)  uint_bitValue((widehalf), widehalf_pixelIndex((x),(y)))
#define   widehalf_pixelIndex(x,y)             ((x) + WIDEHALF_WIDTH * (y))

pair      tallpair_fromPixel(int x, int y);
uint64_t  tallpair_pixelValue(pair tallpair, int x, int y);
#define   tallpair_pixelIndex(x,y)             ((x) + TALLPAIR_WIDTH * (y))

pair      widepair_fromPixel(int x, int y);
uint64_t  widepair_pixelValue(pair widepair, int x, int y);
#define   widepair_pixelIndex(x,y)             ((x) + WIDEPAIR_WIDTH * (y))

tetra     tetra_fromPixel(int x, int y);
uint64_t  tetra_pixelValue(tetra t, int x, int y);
#define   tetra_pixelIndex(x,y)                ((x) + TETRA_WIDTH * (y))


//------------------------------------------------------------------------------
// line and bits selection macros for quadrants, halves, and glyphs

// returns a 0x0N or 0xN0 (with the _H suffix) value,
// where N is the nibble representing a quadrant's line (line: 0-3)


#define quadrant_line(quadrant, n)    ((quadrant) >> (QUADRANT_WIDTH*(n)) & 0xF)
#define quadrant_line0(  quadrant)    ((quadrant)                       & 0x0F)
#define quadrant_line0_H(  quadrant)  ((quadrant) << (QUADRANT_WIDTH*1) & 0xF0)
#define quadrant_line1(  quadrant)    ((quadrant) >> (QUADRANT_WIDTH*1) & 0x0F)
#define quadrant_line1_H(quadrant)    ((quadrant)                       & 0xF0)
#define quadrant_line2(  quadrant)    ((quadrant) >> (QUADRANT_WIDTH*2) & 0x0F)
#define quadrant_line2_H(quadrant)    ((quadrant) >> (QUADRANT_WIDTH*1) & 0xF0)
#define quadrant_line3(  quadrant)    ((quadrant) >> (QUADRANT_WIDTH*3)       )
#define quadrant_line3_H(quadrant)    ((quadrant) >> (QUADRANT_WIDTH*2) & 0xF0)

// returns a 0x0N or 0xN0 (with the _H suffix) value,
// where N is the nibble representing a tall half's line (line: 0-7)
#define tallhalf_line(tallhalf, n)    ((tallhalf) >> (TALLHALF_WIDTH*(n)) & 0xF)
#define tallhalf_line0(  tallhalf)    ((tallhalf)                       & 0x0F)
#define tallhalf_line0_H(  tallhalf)  ((tallhalf) << (TALLHALF_WIDTH*1) & 0xF0)
#define tallhalf_line1(  tallhalf)    ((tallhalf) >> (TALLHALF_WIDTH*1) & 0x0F)
#define tallhalf_line1_H(tallhalf)    ((tallhalf)                       & 0xF0)
#define tallhalf_line2(  tallhalf)    ((tallhalf) >> (TALLHALF_WIDTH*2) & 0x0F)
#define tallhalf_line2_H(tallhalf)    ((tallhalf) >> (TALLHALF_WIDTH*1) & 0xF0)
#define tallhalf_line3(  tallhalf)    ((tallhalf) >> (TALLHALF_WIDTH*3) & 0x0F)
#define tallhalf_line3_H(tallhalf)    ((tallhalf) >> (TALLHALF_WIDTH*2) & 0xF0)
#define tallhalf_line4(  tallhalf)    ((tallhalf) >> (TALLHALF_WIDTH*4) & 0x0F)
#define tallhalf_line4_H(tallhalf)    ((tallhalf) >> (TALLHALF_WIDTH*3) & 0xF0)
#define tallhalf_line5(  tallhalf)    ((tallhalf) >> (TALLHALF_WIDTH*5) & 0x0F)
#define tallhalf_line5_H(tallhalf)    ((tallhalf) >> (TALLHALF_WIDTH*4) & 0xF0)
#define tallhalf_line6(  tallhalf)    ((tallhalf) >> (TALLHALF_WIDTH*6) & 0x0F)
#define tallhalf_line6_H(tallhalf)    ((tallhalf) >> (TALLHALF_WIDTH*5) & 0xF0)
#define tallhalf_line7(  tallhalf)    ((tallhalf) >> (TALLHALF_WIDTH*7)       )
#define tallhalf_line7_H(tallhalf)    ((tallhalf) >> (TALLHALF_WIDTH*6) & 0xF0)

// returns a 0x00-0xFF value representing a wide half's line (line: 0-3)
#define widehalf_line(widehalf, n)    uint_byteValue((widehalf), (n))

// returns a 0x00-0xFF value representing a glyph line (line: 0-7)
#define glyph_line(glyph, n)          uint_byteValue((glyph), (n))


//------------------------------------------------------------------------------
// constants / placeholders / masks

// - "placeholder" glyph (a solid block with margin 1)
#define QUADRANT_PLACEHOLDER   QUADRANT(0660)
#define TALLHALF_PLACEHOLDER   TALLHALF(06666660)
#define WIDEHALF_PLACEHOLDER   WIDEHALF(007e7e00)
#define GLYPH_PLACEHOLDER      GLYPH(007e7e7e7e7e7e00)  // alt: GLYPH(007e566a566a7e00)
#define TALLPAIR_PLACEHOLDER   ((pair){ .first        = GLYPH(007e7e7e7e7e7e7e) , \
                                        .second       = GLYPH(7e7e7e7e7e7e7e00) } )
#define WIDEPAIR_PLACEHOLDER   ((pair){ .first        = GLYPH(007f7f7f7f7f7f00) , \
                                        .second       = GLYPH(00fefefefefefe00) } )
#define TETRA_PLACEHOLDER      ((tetra){.top_left     = GLYPH(007f7f7f7f7f7f7f) , \
                                        .top_right    = GLYPH(00fefefefefefefe) , \
                                        .bottom_left  = GLYPH(7f7f7f7f7f7f7f00) , \
                                        .bottom_right = GLYPH(fefefefefefefe00) } )
// - a placeholder glyphs that spells the word "TODO"
#define GLYPH_TODO                                      GLYPH(ee4a4e00ceaace00)
#define TALLPAIR_TODO          ((pair){ .top          = GLYPH(0000000000000000) , \
                               /*TODO*/ .bottom       = GLYPH(0000000000000000) } )
#define WIDEPAIR_TODO          ((pair){ .top          = GLYPH(0000000000000000) , \
                               /*TODO*/ .bottom       = GLYPH(0000000000000000) } )
#define TETRA_TODO             ((tetra){.top_left     = GLYPH(7e5a181818183c00) , \
                                        .top_right    = GLYPH(386cc6c6c66c3800) , \
                                        .bottom_left  = GLYPH(f86c6666666cf800) , \
                                        .bottom_right = GLYPH(386cc6c6c66c3800) } )

// masks to that select 4x4 pixel parts of glyph with | ...
#define GLYPH_TOP_LEFT         GLYPH(000000000f0f0f0f)
#define GLYPH_TOP_RIGHT        GLYPH(00000000f0f0f0f0) 
#define GLYPH_BOTTOM_LEFT      GLYPH(0f0f0f0f00000000)
#define GLYPH_BOTTOM_RIGHT     GLYPH(f0f0f0f000000000)
#define GLYPH_TOP              (GLYPH_TOP_LEFT|GLYPH_TOP_RIGHT)
#define GLYPH_BOTTOM           (GLYPH_BOTTOM_LEFT|GLYPH_BOTTOM_RIGHT)
#define GLYPH_LEFT             (GLYPH_TOP_LEFT|GLYPH_BOTTOM_LEFT)
#define GLYPH_RIGHT            (GLYPH_TOP_RIGHT|GLYPH_BOTTOM_RIGHT)

// masks to select a part 4x4 pixel parts of a half with | ...
#define TALLHALF_TOP           TALLHALF(0000ffff)
#define TALLHALF_BOTTOM        TALLHALF(ffff0000)
#define WIDEHALF_LEFT          WIDEHALF(0f0f0f0f)
#define WIDEHALF_RIGHT         WIDEHALF(f0f0f0f0)

// masks to that select 2x2 pixel parts of quadrants | ...
#define QUADRANT_TOP_LEFT      QUADRANT(0033)
#define QUADRANT_TOP_RIGHT     QUADRANT(00cc)
#define QUADRANT_BOTTOM_LEFT   QUADRANT(3300)
#define QUADRANT_BOTTOM_RIGHT  QUADRANT(cc00)
#define QUADRANT_TOP           (QUADRANT_TOP_LEFT|QUADRANT_TOP_RIGHT)
#define QUADRANT_BOTTOM        (QUADRANT_BOTTOM_LEFT|QUADRANT_BOTTOM_RIGHT)
#define QUADRANT_LEFT          (QUADRANT_TOP_LEFT|QUADRANT_BOTTOM_LEFT)
#define QUADRANT_RIGHT         (QUADRANT_TOP_RIGHT|QUADRANT_BOTTOM_RIGHT)

//------------------------------------------------------------------------------
// make standard glyphes out of smaller components,
// makes halves out of quadrants

// halves from two quadrants:
static inline  uint32_t  tallhalf_fromQuadrants(uint16_t top,  uint16_t bottom);
static inline  uint32_t  widehalf_fromQuadrants(uint16_t left, uint16_t right);
// glyph from four quadrants:
static inline  uint64_t  glyph_fromQuadrants(uint16_t top_left   , uint16_t top_right,
                                             uint16_t bottom_left, uint16_t bottom_right);
// glyph from two halves:
static inline  uint64_t  glyph_fromWidehalves(uint32_t top,  uint32_t bottom);
static inline  uint64_t  glyph_fromTallhalves(uint32_t left, uint32_t right);

/* TODO: implement... OR NOT
// glyph from one half, and two quadrants
static inline  uint64_t  glyph3_left  (uint32_t tall_left,   uint16_t top_right,   uint16_t bottom_right);
static inline  uint64_t  glyph3_right (uint32_t tall_right,  uint16_t top_left,    uint16_t bottom_left);
static inline  uint64_t  glyph3_top   (uint32_t wide_top,    uint16_t bottom_left, uint16_t bottom_right);
static inline  uint64_t  glyph3_bottom(uint32_t wide_bottom, uint16_t top_left,    uint16_t top_right);

*/


// implementation

static inline  uint32_t  tallhalf_fromQuadrants(uint16_t top,  uint16_t bottom)
{ return (uint32_t)bottom << 16 | top; }

static inline  uint32_t  widehalf_fromQuadrants(uint16_t left, uint16_t right)
{ //      v-- just to make the computation happens in at least 32 bits
  //          as we shift by 24 bits ------------------------------------v
  return (uint32_t)( quadrant_line0(left) | quadrant_line0_H(right) ) << 0*WIDEHALF_WIDTH |
         (uint32_t)( quadrant_line1(left) | quadrant_line1_H(right) ) << 1*WIDEHALF_WIDTH |
         (uint32_t)( quadrant_line2(left) | quadrant_line2_H(right) ) << 2*WIDEHALF_WIDTH |
         (uint32_t)( quadrant_line3(left) | quadrant_line3_H(right) ) << 3*WIDEHALF_WIDTH ;
}

static inline  uint64_t  glyph_fromQuadrants(uint16_t top_left   , uint16_t top_right,
                                uint16_t bottom_left, uint16_t bottom_right)
{
  return (uint64_t)( quadrant_line0(top_left)    | quadrant_line0_H(top_right)    ) << 0*GLYPH_WIDTH |
         (uint64_t)( quadrant_line1(top_left)    | quadrant_line1_H(top_right)    ) << 1*GLYPH_WIDTH |
         (uint64_t)( quadrant_line2(top_left)    | quadrant_line2_H(top_right)    ) << 2*GLYPH_WIDTH |
         (uint64_t)( quadrant_line3(top_left)    | quadrant_line3_H(top_right)    ) << 3*GLYPH_WIDTH |
         (uint64_t)( quadrant_line0(bottom_left) | quadrant_line0_H(bottom_right) ) << 4*GLYPH_WIDTH |
         (uint64_t)( quadrant_line1(bottom_left) | quadrant_line1_H(bottom_right) ) << 5*GLYPH_WIDTH |
         (uint64_t)( quadrant_line2(bottom_left) | quadrant_line2_H(bottom_right) ) << 6*GLYPH_WIDTH |
         (uint64_t)( quadrant_line3(bottom_left) | quadrant_line3_H(bottom_right) ) << 7*GLYPH_WIDTH ;
}

static inline  uint64_t  glyph_fromWidehalves(uint32_t top,  uint32_t bottom)
{ return (uint64_t)bottom << 32 | top; }

static inline  uint64_t  glyph_fromTallhalves(uint32_t left, uint32_t right) {
  return (uint64_t)( tallhalf_line0(left) | tallhalf_line0_H(right) ) << 0*GLYPH_WIDTH |
         (uint64_t)( tallhalf_line1(left) | tallhalf_line1_H(right) ) << 1*GLYPH_WIDTH |
         (uint64_t)( tallhalf_line2(left) | tallhalf_line2_H(right) ) << 2*GLYPH_WIDTH |
         (uint64_t)( tallhalf_line3(left) | tallhalf_line3_H(right) ) << 3*GLYPH_WIDTH |
         (uint64_t)( tallhalf_line4(left) | tallhalf_line4_H(right) ) << 4*GLYPH_WIDTH |
         (uint64_t)( tallhalf_line5(left) | tallhalf_line5_H(right) ) << 5*GLYPH_WIDTH |
         (uint64_t)( tallhalf_line6(left) | tallhalf_line6_H(right) ) << 6*GLYPH_WIDTH |
         (uint64_t)( tallhalf_line7(left) | tallhalf_line7_H(right) ) << 7*GLYPH_WIDTH ;
}

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

/* flips and n*90-degrees rotations (angles are measured counterclockwise) */

// flip along the horizontal axis (-)
static inline  uint16_t  quadrant_flip      (uint16_t quadrant);
static inline  uint32_t  tallhalf_flip      (uint32_t tallhalf);
static inline  uint32_t  widehalf_flip      (uint32_t widehalf);
static inline  uint64_t     glyph_flip      (uint64_t glyph   );
static inline  pair      tallpair_flip      (pair     tallpair);
static inline  pair      widepair_flip      (pair     widepair);
static inline  tetra        tetra_flip      (tetra    ttra    );

// flip along the vertical axis (|)
static inline  uint16_t  quadrant_mirror    (uint16_t quadrant);
static inline  uint32_t  tallhalf_mirror    (uint32_t tallhalf);
static inline  uint32_t  widehalf_mirror    (uint32_t widehalf);
static inline  uint64_t     glyph_mirror    (uint64_t glyph   );
static inline  pair      tallpair_mirror    (pair     tallpair);
static inline  pair      widepair_mirror    (pair     widepair);
static inline  tetra        tetra_mirror    (tetra    ttra    );

// flip along the diagonal (\)
static inline  uint16_t  quadrant_transpose (uint16_t quadrant);
//static inline  uint32_t  tallhalf_transpose (uint32_t tallhalf);
//static inline  uint32_t  widehalf_transpose (uint32_t widehalf);
static inline  uint64_t     glyph_transpose (uint64_t glyph   );
static inline  /*w*/pair tallpair_transpose (pair     tallpair);
static inline  /*t*/pair widepair_transpose (pair     widepair);
static inline  tetra        tetra_transpose (tetra    ttra    );

// 90 degree (counterclockwise) rotation
static inline  uint16_t  quadrant_rotate90  (uint16_t quadrant);
//static inline  uint32_t  tallhalf_rotate90  (uint32_t tallhalf);
//static inline  uint32_t  widehalf_rotate90  (uint32_t widehalf);
static inline  uint64_t     glyph_rotate90  (uint64_t glyph   );
static inline  /*w*/pair tallpair_rotate90  (pair     tallpair);
static inline  /*t*/pair widepair_rotate90  (pair     widepair);
static inline  tetra        tetra_rotate90  (tetra    ttra    );

// 180 degree rotation
static inline  uint16_t  quadrant_rotate180 (uint16_t quadrant);
static inline  uint32_t  tallhalf_rotate180 (uint32_t tallhalf);
static inline  uint32_t  widehalf_rotate180 (uint32_t widehalf);
static inline  uint64_t  glyph_rotate180    (uint64_t glyph);
static inline  pair      tallpair_rotate180 (pair     tallpair);
static inline  pair      widepair_rotate180 (pair     widepair);
static inline  tetra     tetra_rotate180    (tetra    ttra);

// 270 degree (= 90 degrees clockwise) rotation
static inline  uint16_t  quadrant_rotate270 (uint16_t quadrant);
//static inline  uint32_t  tallhalf_rotate270 (uint32_t tallhalf);
//static inline  uint32_t  widehalf_rotate270 (uint32_t widehalf);
static inline  uint64_t  glyph_rotate270    (uint64_t glyph);
static inline  /*w*/pair tallpair_rotate270 (pair     tallpair);
static inline  /*t*/pair widepair_rotate270 (pair     widepair);
static inline  tetra     tetra_rotate270    (tetra    ttra);


// transformations: shifts
static inline uint64_t  glyph_shiftLeft     (uint64_t glyph, unsigned n);
static inline uint64_t  glyph_shiftRight    (uint64_t glyph, unsigned n);
static inline uint64_t  glyph_shiftTop      (uint64_t glyph, unsigned n);
static inline uint64_t  glyph_shiftBottom   (uint64_t glyph, unsigned n);

// transformations: cyclic shifts
//static inline uint64_t  glyph_cycleLeft   (uint64_t glyph, unsigned n); // TODO
//static inline uint64_t  glyph_cycleRight  (uint64_t glyph, unsigned n); // TODO
//static inline uint64_t  glyph_cycleTop    (uint64_t glyph, unsigned n); // TODO
//static inline uint64_t  glyph_cycleBottom (uint64_t glyph, unsigned n); // TODO


//--- inline implementation ----------------------------------------------------


/* glyph measurements */

static inline int glyph_marginTop(uint64_t glyph)
{ return (glyph)? (uint64_ctz(glyph) / GLYPH_HEIGHT) : GLYPH_HEIGHT; }

static inline int glyph_marginBottom(uint64_t glyph)
{ return (glyph)? (uint64_clz(glyph) / GLYPH_HEIGHT) : GLYPH_HEIGHT; }

static inline int glyph_height(uint64_t glyph)
{ return (glyph)? (GLYPH_HEIGHT - (uint64_clz(glyph) / GLYPH_HEIGHT) - (uint64_ctz(glyph) / GLYPH_HEIGHT)) : 0; }

// binary-OR combines all the line-bytes of the glyph into a byte
static inline unsigned char
glyph_flatten(uint64_t glyph) {
   return ((glyph  >>  GLYPH_WIDTH * 0) | (glyph  >>  GLYPH_WIDTH * 1) |
           (glyph  >>  GLYPH_WIDTH * 2) | (glyph  >>  GLYPH_WIDTH * 3) |
           (glyph  >>  GLYPH_WIDTH * 4) | (glyph  >>  GLYPH_WIDTH * 5) |
           (glyph  >>  GLYPH_WIDTH * 6) | (glyph  >>  GLYPH_WIDTH * 7) );
}

static inline int glyph_marginLeft(uint64_t glyph)
{ return (glyph)? byte_ctz(glyph_flatten(glyph)) : 0; }

static inline int glyph_marginRight(uint64_t glyph)
{ return (glyph)? byte_clz(glyph_flatten(glyph)) : 0; }

static inline int glyph_width(uint64_t glyph)
{
   if (glyph) {
      unsigned char line = glyph_flatten(glyph);
      return GLYPH_WIDTH - byte_clz(line) - byte_ctz(line);
   } else {
      return 0;
   }
}

/* flips / transposition / rotations */
// inspiration for implementation: Hacker's Delight +
// https://stackoverflow.com/questions/1667591/rotating-a-bitmap-90-degrees

static inline uint16_t quadrant_flip(uint16_t q)
{  q = ((q & QUADRANT(ff00)) >>  8) | ((q & QUADRANT(00ff)) <<  8);
   q = ((q & QUADRANT(f0f0)) >>  4) | ((q & QUADRANT(0f0f)) <<  4);
   return q;
}
static inline uint32_t tallhalf_flip(uint32_t t)
{  t = ((t & TALLHALF(ffff0000)) >> 16) | ((t & TALLHALF(0000ffff)) << 16);
   t = ((t & TALLHALF(ff00ff00)) >>  8) | ((t & TALLHALF(00ff00ff)) <<  8);
   t = ((t & TALLHALF(f0f0f0f0)) >>  4) | ((t & TALLHALF(0f0f0f0f)) <<  4);
   return t;
}
static inline uint32_t widehalf_flip(uint32_t w)
{  w = ((w & WIDEHALF(ffff0000)) >> 16) | ((w & WIDEHALF(0000ffff)) << 16);
   w = ((w & WIDEHALF(ff00ff00)) >>  8) | ((w & WIDEHALF(00ff00ff)) <<  8);
   return w;
}
static inline uint64_t glyph_flip(uint64_t g)
{  g = ((g & GLYPH(ffffffff00000000)) >> 32) | ((g & GLYPH(00000000ffffffff)) << 32);
   g = ((g & GLYPH(ffff0000ffff0000)) >> 16) | ((g & GLYPH(0000ffff0000ffff)) << 16);
   g = ((g & GLYPH(ff00ff00ff00ff00)) >>  8) | ((g & GLYPH(00ff00ff00ff00ff)) <<  8);
   return g;
}
static inline pair tallpair_flip(pair t)
{  uint32_t tmp = glyph_flip(t.first);
   t.first      = glyph_flip(t.second);
   t.second     = tmp;
   return t;
}
static inline pair widepair_flip(pair w)
{  w.first  = glyph_flip(w.first);
   w.second = glyph_flip(w.second);
   return w;
}
static inline tetra tetra_flip(tetra tt)
{  uint64_t tmp   = glyph_flip(tt.top_left);
   tt.top_left     = glyph_flip(tt.bottom_left);
   tt.bottom_left  = tmp;
   tmp             = glyph_flip(tt.top_right);
   tt.top_right    = glyph_flip(tt.bottom_right);
   tt.bottom_right = tmp;
   return tt;
}


static inline uint16_t quadrant_mirror(uint16_t q)
{  q = ((q & QUADRANT(cccc)) >> 2) | ((q & QUADRANT(3333)) << 2);
   q = ((q & QUADRANT(aaaa)) >> 1) | ((q & QUADRANT(5555)) << 1);
   return q;
}
static inline uint32_t tallhalf_mirror(uint32_t t)
{  t = ((t & TALLHALF(cccccccc)) >> 2) | ((t & TALLHALF(33333333)) << 2);
   t = ((t & TALLHALF(aaaaaaaa)) >> 1) | ((t & TALLHALF(55555555)) << 1);
   return t;
}
static inline uint32_t widehalf_mirror(uint32_t w)
{  w = ((w & WIDEHALF(f0f0f0f0)) >> 4) | ((w & WIDEHALF(0f0f0f0f)) << 4);
   w = ((w & WIDEHALF(cccccccc)) >> 2) | ((w & WIDEHALF(33333333)) << 2);
   w = ((w & WIDEHALF(aaaaaaaa)) >> 1) | ((w & WIDEHALF(55555555)) << 1);
   return w;
}
static inline uint64_t glyph_mirror(uint64_t glyph)
{  glyph = ((glyph & GLYPH(f0f0f0f0f0f0f0f0)) >> 4) | ((glyph & GLYPH(0f0f0f0f0f0f0f0f)) << 4);
   glyph = ((glyph & GLYPH(cccccccccccccccc)) >> 2) | ((glyph & GLYPH(3333333333333333)) << 2);
   glyph = ((glyph & GLYPH(aaaaaaaaaaaaaaaa)) >> 1) | ((glyph & GLYPH(5555555555555555)) << 1);
   return glyph;
}
static inline pair tallpair_mirror(pair t)
{  t.first  = glyph_mirror(t.first);
   t.second = glyph_mirror(t.second);
   return t;
}
static inline pair widepair_mirror(pair w)
{  uint64_t tmp = glyph_mirror(w.first);
   w.first      = glyph_mirror(w.second);
   w.second     = tmp;
   return w;
}
static inline tetra tetra_mirror(tetra tt)
{  uint64_t tmp    = glyph_mirror(tt.top_left);
   tt.top_left     = glyph_mirror(tt.top_right);
   tt.top_right    = tmp;
   tmp             = glyph_mirror(tt.bottom_left);
   tt.bottom_left  = glyph_mirror(tt.bottom_right);
   tt.bottom_right = tmp;
   return tt;
}

/*
static inline uint16_t  quadrant_transpose(uint16_t q) {  //   . a b c
   // TODO                                                //   A . d e
   uint16_t q2 = 0;                                       //   B D . f
                                                          //   C E F .

   for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
         q2 = uint_setBit(q & quadrant_fromPixel(j,i), quandrant_fromPixel(i,j))
         q2 |= ;

      }
}

   return q;
}*/

// https://stackoverflow.com/questions/65612229/flip-4x4-bit-grid-horizontally-vertically-diagonally
/*static inline uint16_t quadrant_transpose(uint16_t x) {
    return  // Main diagonal.
            (x & 0b1000'0100'0010'0001) |
           // Upper diagonals.
           ((x & 0b0100'0010'0001'0000) >> 3) |
           ((x & 0b0010'0001'0000'0000) >> 6) |
           ((x & 0b0001'0000'0000'0000) >> 9) |
           // Lower diagonals.
           ((x & 0b0000'1000'0100'0010) << 3) |
           ((x & 0b0000'0000'1000'0100) << 6) |
           ((x & 0b0000'0000'0000'1000) << 9);
}*/
static inline uint16_t quadrant_transpose(uint16_t x) {
    return  // Main diagonal.
            (x & 0x8421) |
           // Upper diagonals.
           ((x & 0x4210) >> 3) | ((x & 0x2100) >> 6) | ((x & 0x1000) >> 9) |
           // Lower diagonals.
           ((x & 0x0842) << 3) | ((x & 0x0084) << 6) | ((x & 0x0008) << 9);
}

static inline uint64_t  glyph_transpose(uint64_t glyph) {
   // Based on "Hacker's Delight" (second edition) by Henry S. Warren, Jr.
   // https://web.archive.org/web/20190915025154/http://www.hackersdelight.org/
   // https://en.wikipedia.org/wiki/Hacker%27s_Delight
   glyph = ( glyph        & GLYPH(aa55aa55aa55aa55)       ) |
           ((glyph        & GLYPH(00aa00aa00aa00aa)) << 7 ) |
           ((glyph >> 7)  & GLYPH(00aa00aa00aa00aa)       ) ;
   glyph = ( glyph        & GLYPH(cccc3333cccc3333)       ) |
           ((glyph        & GLYPH(0000cccc0000cccc)) << 14) |
           ((glyph >> 14) & GLYPH(0000cccc0000cccc)       ) ;
   glyph = ( glyph        & GLYPH(f0f0f0f00f0f0f0f)       ) |
           ((glyph        & GLYPH(00000000f0f0f0f0)) << 28) |
           ((glyph >> 28) & GLYPH(00000000f0f0f0f0))        ;
   return glyph;
}
static inline  /*w*/pair tallpair_transpose (pair     tallpair) {
   return tallpair_flip(tallpair_rotate90(tallpair));
}
static inline  /*t*/pair widepair_transpose (pair     widepair) {
   return widepair_flip(widepair_rotate90(widepair));
}
static inline tetra  tetra_transpose(tetra tt) {
   tt.top_left     = glyph_transpose(tt.top_left);
   tt.bottom_right = glyph_transpose(tt.bottom_right);
   uint64_t tmp    = glyph_transpose(tt.top_right);
   tt.top_right    = glyph_transpose(tt.bottom_left);
   tt.bottom_left  = tmp;
   return tt;
}

static inline  uint16_t  quadrant_rotate90  (uint16_t quadrant)   { return quadrant_transpose(quadrant_mirror(quadrant)); }
//static inline  uint32_t  tallhalf_rotate90  (uint32_t tallhalf)   { return tallhalf_transpose(tallhalf_mirror(tallhalf)); }
//static inline  uint32_t  widehalf_rotate90  (uint32_t widehalf)   { return widehalf_transpose(widehalf_mirror(widehalf)); }
static inline  uint64_t  glyph_rotate90     (uint64_t glyph)      { return    glyph_transpose(   glyph_mirror(   glyph)); }
static inline  /*w*/pair tallpair_rotate90  (pair     tallpair)   {
   tallpair.first  = glyph_rotate90(tallpair.first);
   tallpair.second = glyph_rotate90(tallpair.second);
   return tallpair;
}
static inline  /*t*/pair widepair_rotate90  (pair     widepair) {
   uint64_t tmp    = glyph_rotate90(widepair.first);
   widepair.first  = glyph_rotate90(widepair.second);
   widepair.second = tmp;
   return widepair;
}
static inline  tetra     tetra_rotate90     (tetra    ttra)       { return    tetra_transpose(   tetra_mirror(    ttra)); }

static inline  uint16_t  quadrant_rotate180 (uint16_t quadrant)   { return quadrant_mirror(quadrant_flip(quadrant)); }
static inline  uint32_t  tallhalf_rotate180 (uint32_t tallhalf)   { return tallhalf_mirror(tallhalf_flip(tallhalf)); }
static inline  uint32_t  widehalf_rotate180 (uint32_t widehalf)   { return widehalf_mirror(widehalf_flip(widehalf)); }
static inline  uint64_t  glyph_rotate180    (uint64_t glyph)      { return    glyph_mirror(   glyph_flip(   glyph)); }
static inline  pair      tallpair_rotate180 (pair     tallpair)   { return tallpair_mirror(tallpair_flip(tallpair)); }
static inline  pair      widepair_rotate180 (pair     widepair)   { return widepair_mirror(widepair_flip(widepair)); }
static inline  tetra     tetra_rotate180    (tetra    ttra)       { return    tetra_mirror(   tetra_flip(    ttra)); }

static inline  uint16_t  quadrant_rotate270 (uint16_t quadrant)   { return quadrant_transpose(quadrant_flip(quadrant)); }
//static inline  uint32_t  tallhalf_rotate270 (uint32_t tallhalf)   { return tallhalf_transpose(tallhalf_flip(tallhalf)); }
//static inline  uint32_t  widehalf_rotate270 (uint32_t widehalf)   { return widehalf_transpose(widehalf_flip(widehalf)); }
static inline  uint64_t  glyph_rotate270    (uint64_t glyph)      { return    glyph_transpose(   glyph_flip(   glyph)); }
static inline  /*w*/pair tallpair_rotate270 (pair     tallpair)   { return tallpair_transpose(tallpair_flip(tallpair)); }
static inline  /*t*/pair widepair_rotate270 (pair     widepair)   { return widepair_transpose(widepair_flip(widepair)); }
static inline  tetra     tetra_rotate270    (tetra    ttra)       { return    tetra_transpose(   tetra_flip(    ttra)); }




/* shifts */

static inline uint64_t
glyph_shiftLeft(uint64_t glyph, unsigned n)
{ return (glyph << n) & (~GLYPH(0) - n * GLYPH(0101010101010101)); }
// works only for 1


// glyph_shiftLeft(uint64_t glyph, unsigned n)
// { return (glyph >>n) & n * GLYPH(0101010101010101); }
//
static inline uint64_t
glyph_shiftRight(uint64_t glyph, unsigned n)
{ return (glyph >> n) & (~GLYPH(0) - n * GLYPH(1010101010101010)); }

static inline uint64_t
glyph_shiftTop(uint64_t glyph, unsigned n)
{ return (glyph << GLYPH_WIDTH * n); }

static inline uint64_t
glyph_shiftBottom(uint64_t glyph, unsigned n)
{ return (glyph >> GLYPH_WIDTH * n); }



/* swaps */

static inline uint64_t  glyph_swapWidehalves(uint64_t glyph) // swap top/bottom halves
{ return (glyph << 32) | (glyph >> 32); }

static inline uint64_t  glyph_swapTallhalves(uint64_t glyph) // swap left/right halves
{ return glyph & GLYPH(f0f0f0f0f0f0f0f0); }
//{ return (glyph & GLYPH(f0f0f0f0f0f0f0f0)) | (glyph & GLYPH(0f0f0f0f0f0f0f0f)); }

// (use n=4 to swap top and bottom halves)
static inline uint64_t  glyph_cycleTop(uint64_t glyph, unsigned n)
{  assert(n < 8);
   n*=8; return (glyph << n) | (glyph >> (64 - n)); }

// (use n=4 to swap top and bottom halves)
static inline uint64_t  glyph_cycleBottom(uint64_t glyph, unsigned n)
{  assert(n < 8);
   n*=8; return (glyph >> n) | (glyph << (64 - n)); }


// merge glyph a and b, according to a mask
// mask bits contain 1 where bits from b should be selected; 0 where from a.
static inline uint64_t
glyph_merge(uint64_t a, uint64_t b, uint64_t mask) {
   // return (a & ~mask) | (b & mask);
   //
   // same but, this saves one operation:
   // (see: https://graphics.stanford.edu/~seander/bithacks.html#MaskedMerge)
   return a ^ ((a ^ b) & mask);
}


#endif //KONPU_GLYPH_H
