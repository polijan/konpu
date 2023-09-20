#ifndef  KONPU_CANVAS_H
#define  KONPU_CANVAS_H
#include "platform.h"
#include "glyph.h"
#include "rect.h"

//===< CANVAS >=================================================================

typedef struct canvas {
  uint64_t   *glyphs;
  int         width;
  int         height;
  int         stride;  // https://en.wikipedia.org/wiki/Stride_of_an_array
} canvas;

typedef canvas const_canvas;
// we use this synonym when indicating that we don't INTEND to modify the glyphs
// on the canvas. Of course, it means nothing, the compiler will not check.




// macros and functions for *direct* access to glyphs (or their numerical index)
// those do NOT do any bound checking, use responsibly.
// (we also do not use assert() ~ although it's rare, one might want to write
//                                beyond the edge of a window on purpose)
// canvas_glyph and canvas_glyphPointer can serve either as a rvalue or lvalue
#define                  canvas_glyph(canvas, x,y)                        (*canvas_glyphPointer((canvas), (x),(y)))
#define                  canvas_glyphIndex(canvas, x,y)                   ((x) + (y) * (canvas).stride)
static inline uint64_t*  canvas_glyphPointer(canvas cvas, int x, int y)   { return cvas.glyphs + canvas_glyphIndex(cvas, x ,y); }


// SET or BLEND a GLYPH at position (x,y)
// --------------------------------------
//
// * Set whole glyph as-is:
//   copy   : canvas_glyph(canvas, x,y)  =  glyph;
//
// * Blend using the set pixels of the glyph:
//   with the following effects on the canvas pixels:
//   - set    : canvas_glyph(canvas, x,y) |=  glyph;
//   - unset  : canvas_glyph(canvas, x,y) &= ~glyph;
//   - toggle : canvas_glyph(canvas, x,y) ^= ~glyph;
//
// * Blend using a mask:
//   uint64_t mask; // contains 1 where bits from the glyph are selected
//   uint64_t *bg = CANVAS_PTR(canvas, x,y);
//   *bg = UINT_MERGE(*bg, glyph, mask);
//
//
// MANIPULATE PIXELS at (x,y)
// --------------------------
//
//   - Query the value:
//     bool lit = uint_bitValue(canvas_glyph(canvas, x/8, y/8),
//                         glyph_pixel_idx(x%8, y%8));
//
//   - Set/Unset/Toggle pixel(x,y)
//     - set    : canvas_glyph(canvas, x/8, y/8) |=  glyph_pixel(x%8, y%8);
//     - unset  : canvas_glyph(canvas, x/8, y/8) &= ~glyph_pixel(x%8, y%8);
//     - toggle : canvas_glyph(canvas, x/8, y/8) ^= ~glyph_pixel(x%8, y%8);


#define canvas_glyphFromPixel(canvas, x, y)           canvas_glyph(       (canvas), (x) / GLYPH_WIDTH, (y) / GLYPH_HEIGHT)
#define canvas_glyphPointerFromPixel(canvas, x, y)    canvas_glyphPointer((canvas), (x) / GLYPH_WIDTH, (y) / GLYPH_HEIGHT)
#define canvas_glyphIndexFromPixel(canvas, x, y)      canvas_glyphIndex(  (canvas), (x) / GLYPH_WIDTH, (y) / GLYPH_HEIGHT)

// ^ #define canvas_getGlyphIndexFromPixel(canvas, x, y)   canvas_glyphIndex(  (canvas), (x) / GLYPH_WIDTH, (y) / GLYPH_HEIGHT)
//   todo canvas_glyphIndex => canvas_getGlyphIndex
// #define canvas_getDotFromPixel(x, y)               glyph_fromPixel((x) % GLYPH_WIDTH, (y) % GLYPH_HEIGHT); }

// #define canvas_setPixelValue
// #define canvas_getPixelValue
// #define canvas_hasPixel
// #define canvas_setPixel
// #define canvas_unsetPixel




static inline void  canvas_setPixel(canvas cvas, int x, int y)       { canvas_glyphFromPixel(cvas, x, y) |=  glyph_fromPixel(x % GLYPH_WIDTH, y % GLYPH_HEIGHT); }
static inline void  canvas_unsetPixel(canvas cvas, int x, int y)     { canvas_glyphFromPixel(cvas, x, y) &= ~glyph_fromPixel(x % GLYPH_WIDTH, y % GLYPH_HEIGHT); }
static inline void  canvas_tooglePixel(canvas cvas, int x, int y)    { canvas_glyphFromPixel(cvas, x, y) ^= ~glyph_fromPixel(x % GLYPH_WIDTH, y % GLYPH_HEIGHT); }





/////////////////////
// canvas creation //
/////////////////////

// a literal of one canvas which is a null state
#define CANVAS_NULL             ((canvas){0})

// for a given canvas, returns a canvas defined by the crop area.
// the returned canvas will *share* the same underlying memory.
// if the crop area is not visible, the null canvas will be returned.
static inline  canvas  canvas_crop(canvas cvas, rect crop_area);


/////////////////
// safe access //
/////////////////

// return true iff canvas is in a *null state*
// null state means glyph pointer is null or width or height is 0
static inline  bool  canvas_isnull(canvas c);

// assert canvas isn't in an *invalid state*
// a canvas is not in an invalid state iff all those conditions are true:
// - its dimensions and stride are positive
// - (TODO: stride >= width ???)
// - its glyphs pointer may be NULL, but then its dimensions must be 0
#define CANVAS_ASSERT(canvas) do            \
        { assert((canvas).height >= 0);     \
          assert((canvas).width  >= 0);     \
          assert((canvas).stride >= 0);     \
          assert((canvas).glyphs || ((canvas).height == 0 && (canvas).width == 0)); \
        } while(0)

// clamp (signed integer) coordinate variables to a canvas dimensions
#define CANVAS_CLAMP(canvas, x,y) do        \
        { CANVAS_ASSERT((canvas));          \
          UTIL_CLAMP((x), (canvas).width);  \
          UTIL_CLAMP((y), (canvas).height); \
        } while (0)

// returns the glyph at position (x,y)
//         or 0 if position is invalid or if canvas is null
// (note: 0 may also be returned as a glyph, not just
//        when canvas is null or position is invalid)
static inline uint64_t canvas_at(canvas c, int x, int y);

// set a glyph at position (x,y)
// will return true iff glyph is set (valid position, canvas not null)
static inline bool canvas_putglyph(canvas c, uint64_t glyph, int x, int y);


//--- inline implementation ----------------------------------------------------

static inline bool canvas_isnull(canvas cvas)
{  CANVAS_ASSERT(cvas);
   // return (cvas.glyphs == NULL || cvas.height == 0 || cvas.width == 0);
   //        ^-- let's use <= 0 (shouldn't be more "expensive" than == 0),
   //            in case we get an invalid canvas and `assert` is disabled.
   return (cvas.glyphs == NULL || cvas.height <= 0 || cvas.width <= 0);
}

static inline uint64_t
canvas_at(canvas cvas, int x, int y)
{ return (canvas_isnull(cvas) || x >= cvas.width || y >= cvas.height) ?
         canvas_glyph(cvas, x,y) : 0; }

static inline bool
canvas_putglyph(canvas cvas, uint64_t glyph, int x, int y)
{  if (canvas_isnull(cvas) || x >= cvas.width || y >= cvas.height)
      return false;
   canvas_glyph(cvas, x,y) = glyph;
   return true;
}

static inline canvas
canvas_crop(canvas cvas, rect crop_area)
{  CANVAS_ASSERT(cvas);

   if (rect_clip(&crop_area, cvas.width, cvas.height)) {
      cvas.glyphs += canvas_glyphIndex(cvas, crop_area.x, crop_area.y),
      cvas.width   = crop_area.w;
      cvas.height  = crop_area.h;
      return cvas;
   } else {
      return CANVAS_NULL;
   }
}


////////////////////////////////////////////////////////////////////////////////


#include <string.h> // memcpy
#include "renderer.h" //tmp: TODO - remove this include

static void
canvas_scrollUp(canvas cvas)
{  CANVAS_ASSERT(cvas);
   uint64_t *end = cvas.glyphs + cvas.stride * (cvas.height - 1);
   uint64_t *pos;
   for (pos = cvas.glyphs; pos < end; pos += cvas.stride) {
      memcpy(pos, pos + cvas.stride, cvas.stride * sizeof(uint64_t));
      sleep_ms(100); render(); //DEBUG
   }
   memset(pos, 0, cvas.stride);
}

static void
canvas_scrollUpPixels(canvas cvas)
{  CANVAS_ASSERT(cvas);
   uint64_t *g = cvas.glyphs;
   for (int y = 0; y < cvas.height - 1; y++) {
      for (int x = 0; x < cvas.width; x++, g++) {
         //     scroll up each glyph  +  copy highest line from the below glyph to the last line
         *g = (*g >> GLYPH_WIDTH)   |  ((*(g + cvas.stride) & 0xFFu) << 56);
      }
   }
   for (int x = 0; x < cvas.width; x++, g++) {
      *g = *g >> GLYPH_WIDTH;
   }
}

// fill the canvas' grid with one glyph
static inline void
canvas_set(canvas cvas, uint64_t glyph)
{  CANVAS_ASSERT(cvas);
   for (int y = 0; y < cvas.height; y++)
       for (int x = 0; x < cvas.width; x++)
           canvas_glyph(cvas, x,y) = glyph;
}


// draw a line on the given canvas between the points (x0,y0) and (x1,y1)
void canvas_line(canvas cvas, int x0, int y0, int x1, int y1);


#endif //KONPU_CANVAS_H
