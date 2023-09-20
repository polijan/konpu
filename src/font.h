#ifndef  KONPU_FONT_H
#define  KONPU_FONT_H
#include "platform.h"
#include "c.h"
#include "glyph.h"

// chr functions return glyphs from a codepoint.
static inline  uint64_t  chr         (unsigned char code);
#define chr_glyph(code)  chr(code) // synonym
static inline  uint32_t  chr_widehalf(unsigned char code);
static inline  uint32_t  chr_tallhalf(unsigned char code);
static inline  uint16_t  chr_quadrant(unsigned char code);

static inline  pair      chr_widepair(unsigned char code);
static inline  pair      chr_tallpair(unsigned char code);
static inline  tetra     chr_tetra   (unsigned char code);



// -- inline implementation ----------------------------------------------------

// TODO: needed???
/*
static inline uint16_t                          // scale q [0..15], which
chr_quadrant_quad(unsigned char q)              // represent a 2x2 bitmap,
{  assert(q < 16);                              // to the size of quadrant:
                                                //
   return                                       // pixel 1  _     _ pixel 2
      uint_bitValue(q, 3) * QUADRANT_TOP_LEFT     |  // (bit 3)   \. ./  (bit 2)
      uint_bitValue(q, 2) * QUADRANT_TOP_RIGHT    |  //            . .
      uint_bitValue(q, 1) * QUADRANT_BOTTOM_LEFT  |  // pixel 3  _/   \_ pixel 4
      uint_bitValue(q, 0) * QUADRANT_BOTTOM_RIGHT ;  // (bit 1)          (bit 0)
}
*/

static inline uint64_t
chr(unsigned char code)
{
   if (code >= 128) {
      extern uint64_t sitelen_pona_font[];
      return sitelen_pona_font[code - 128];
   } else {
      // TODO/FIXME: just for now and until we define a proper font,
      //             vertically stretch the corresponding widehalf
      uint64_t w = chr_widehalf(code);
      return uint_byteValue(w, 0) << 0*8 | uint_byteValue(w, 0) << 1*8 |
             uint_byteValue(w, 1) << 2*8 | uint_byteValue(w, 1) << 3*8 |
             uint_byteValue(w, 2) << 4*8 | uint_byteValue(w, 2) << 5*8 |
             uint_byteValue(w, 3) << 6*8 | uint_byteValue(w, 3) << 7*8 ;
   }
}

static inline uint32_t
chr_widehalf(unsigned char code) {
   // TODO/FIXME: just for now and until we define a proper font,
   //             just horizontally center the corresponding quadrant
   uint32_t q = chr_quadrant(code);
   return quadrant_line0(q) << (WIDEHALF_WIDTH*0 + (WIDEHALF_WIDTH - QUADRANT_WIDTH)/2) |
          quadrant_line1(q) << (WIDEHALF_WIDTH*1 + (WIDEHALF_WIDTH - QUADRANT_WIDTH)/2) |
          quadrant_line2(q) << (WIDEHALF_WIDTH*2 + (WIDEHALF_WIDTH - QUADRANT_WIDTH)/2) |
          quadrant_line3(q) << (WIDEHALF_WIDTH*3 + (WIDEHALF_WIDTH - QUADRANT_WIDTH)/2) ;
}

static inline uint32_t
chr_tallhalf(unsigned char code)
{  extern uint32_t chr_tallhalf_font[];

   if (code < 32 || code > 127)
      return TALLHALF_PLACEHOLDER;
   else
      return chr_tallhalf_font[code];

   /*
   // TODO/FIXME: just for now and until we define a proper font,
   //             just vertically stretch the corresponding quadrant
   uint32_t q = chr_quadrant(code);
   return (quadrant_line0_H(q) | quadrant_line0(q)) << 24 |
          (quadrant_line1_H(q) | quadrant_line1(q)) << 16 |
          (quadrant_line2_H(q) | quadrant_line2(q)) <<  8 |
          (quadrant_line3_H(q) | quadrant_line3(q))       ;
   */
}

static inline uint16_t
chr_quadrant(unsigned char code)
{  extern uint16_t chr_quadrant_font[];

   if (code <  ' ' || code >  '~')
      return QUADRANT_PLACEHOLDER;
   else if (code >= 'a' && code <= 'z') // lowercase ascii letter
      return chr_quadrant_font[code - 32]; //TODO: code & '_'
   else // other printable ascii character:
      return chr_quadrant_font[code];
}


static inline pair
chr_widepair(unsigned char code)
{  //TODO/FIXME: this is wrong, we just want to return something
   return chr_tallpair(code);
}

static inline pair
chr_tallpair(unsigned char code)
{  extern pair chr_taillpair_font[];

   if (code < ' ' || code >= 127)
      return TALLPAIR_PLACEHOLDER;
   else
      return chr_taillpair_font[code];
}

static inline tetra
chr_tetra(unsigned char code)
{  //TODO/FIXME: this is wrong, we just want to return something
   (void)code; return (tetra){0};
}

#endif //KONPU_FONT_H
