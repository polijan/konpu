#ifndef KONPU_DRAW_H_
#define KONPU_DRAW_H_
#include "video.h"
#include "pixel.h"


// Helper function implementing the flood fill algorithm.
// Use DrawFill() instead.
static void // no inline, because recursive
DrawFill_(int x, int y, int old_color, int new_color) {
   // This is the simple 4-ways recursive flood fill algorithm.
   // See: https://en.wikipedia.org/wiki/Flood_fill
   if (x < 0 || x >= VIDEO_WIDTH || y < 0 || y >= VIDEO_HEIGHT) return;
   if (PixelGet_(x,y) == old_color)  {
      PixelSet_(x,y, new_color);
      DrawFill_(x - 1, y, old_color, new_color);
      DrawFill_(x + 1, y, old_color, new_color);
      DrawFill_(x, y - 1, old_color, new_color);
      DrawFill_(x, y + 1, old_color, new_color);
   }
}

// Paint the "connected area" at pixel (x,y) with the given color.
// Connected area = all the pixels with same color as the pixel (x,y) and which
//                  you can access from (x,y) by going up,left,right,down.
// Note: implementtaion of the flood fill here is good for code size, but
// not optimized for performance, and uses a lot of C stack space.
static inline void DrawFill(int x, int y, int color)
{ DrawFill_(x, y, PixelGet(x,y), color); }


// Draw the line between (x0,y0) and (x1,y1) with pixels of the given color
static inline void DrawLine(int x0, int y0, int x1, int y1, int color)
{
   // Draw the line using the classic Brensenham's algorithm.
   // See: https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm#All_cases
   int dx  = c_abs_(x1 - x0);
   int sx  = (x0 < x1) ? 1 : -1;
   int dy  = c_abs_(y1 - y0);
   int sy  = (y0 < y1) ? 1 : -1;
   int err = (dx>dy ? dx : -dy)/2;
   int e2;

   while (true) {
      PixelSet(x0, y0, color);
      if (x0 == x1 && y0 == y1) return;
      e2 = err;
      if (e2 > -dx) { err -= dy; x0 += sx; }
      if (e2 <  dy) { err += dx; y0 += sy; }
   }
}

// Expands to a C for loop traversing every pixel in the line between the points
// (x0,y0) and (x1,y1). So you can set your own action into the loop body.
#define DRAW_LINE_FOR(VARNAME_X,VARNAME_Y, x0, y0, x1, y1)   \
   for(int VARNAME_X = (x0), VARNAME_Y = (y0),               \
         x1_ = (x1), y1_ = (y1),                             \
         dx_ = c_abs_(x1_ - VARNAME_X),                      \
         dy_ = c_abs_(y1_ - VARNAME_Y),                      \
         sx_ = (VARNAME_X < x1_) ? 1 : -1,                   \
         sy_ = (VARNAME_Y < y1_) ? 1 : -1,                   \
         err_ = (dx_ > dy_ ? dx_ : -dy_) / 2,                \
         e2_, continue_ = 1                                  \
      ;                                                      \
         continue_                                           \
      ;                                                      \
         continue_ = (VARNAME_X != x1_ || VARNAME_Y != y1_), \
         e2_ = err_,                                         \
         (e2_ > -dx_)? (err_ -= dy_, VARNAME_X += sx_) : 0,  \
         (e2_ <  dy_)? (err_ += dx_, VARNAME_Y += sy_) : 0   )



// Draw the circle of the given radius centered as (x0,y0) and (x1,y1)
// with pixels of the given color
static inline void DrawCircle(int x0, int y0, int radius, int color)
{
   // Draw the line using the classic Brensenham's / Midpoint circle algorithm.
   // See: https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
   //      https://rosettacode.org/wiki/Bitmap/Midpoint_circle_algorithm#C

   int f = 1 - radius;
   int ddF_x = 0;
   int ddF_y = -2 * radius;
   int x = 0;
   int y = radius;

   PixelSet(x0, y0 + radius, color);
   PixelSet(x0, y0 - radius, color);
   PixelSet(x0 + radius, y0, color);
   PixelSet(x0 - radius, y0, color);

   while (x < y) {
      if (f >= 0) {
         y--;
         ddF_y += 2;
         f += ddF_y;
      }
      x++;
      ddF_x += 2;
      f += ddF_x + 1;
      PixelSet(x0 + x, y0 + y, color);
      PixelSet(x0 - x, y0 + y, color);
      PixelSet(x0 + x, y0 - y, color);
      PixelSet(x0 - x, y0 - y, color);
      PixelSet(x0 + y, y0 + x, color);
      PixelSet(x0 - y, y0 + x, color);
      PixelSet(x0 + y, y0 - x, color);
      PixelSet(x0 - y, y0 - x, color);
   }
}


// void DrawGlyph(Glyph<N> g, int x, int y, int color);
// Draw a Glyph starting at pixel (x,y) with the given color.
// Note: This works in Glyph-based video modes too, but obviously if you want to
//       draw glyphs aligned to the Glyph grid, there are much faster ways.
#define DrawGlyph(glyph, x,y, color) \
   _Generic((glyph)                 ,\
      uint8_t:  DrawGlyph_8_        ,\
      uint16_t: DrawGlyph_16_       ,\
      uint32_t: DrawGlyph_32_       ,\
      uint64_t: DrawGlyph_64_       ,\
      Glyph128: DrawGlyph_128_      ,\
      Glyph256: DrawGlyph_256_       \
   )((glyph), (x), (y), (color))
   static inline void  DrawGlyph_8_(uint8_t g, int x, int y, int c)
   {  for (int j = 0; j < GLYPH8_HEIGHT; j++)
         for (int i = 0; i < GLYPH8_WIDTH; i++)
            if (GlyphGetPixel(g, i, j)) PixelSet(x+i, y+j, c);
   }
   static inline void  DrawGlyph_16_(uint16_t g, int x, int y, int c)
   {  for (int j = 0; j < GLYPH16_HEIGHT; j++)
         for (int i = 0; i < GLYPH16_WIDTH; i++)
            if (GlyphGetPixel(g, i, j))  PixelSet(x+i, y+j, c);
   }
   static inline void  DrawGlyph_32_(uint32_t g, int x, int y, int c)
   {  for (int j = 0; j < GLYPH32_HEIGHT; j++)
         for (int i = 0; i < GLYPH32_WIDTH; i++)
            if (GlyphGetPixel(g, i, j))  PixelSet(x+i, y+j, c);
   }
   static inline void  DrawGlyph_64_(uint64_t g, int x, int y, int c)
   {  for (int j = 0; j < GLYPH64_HEIGHT; j++)
         for (int i = 0; i < GLYPH64_WIDTH; i++)
            if (GlyphGetPixel(g, i, j))  PixelSet(x+i, y+j, c);
   }
   static inline void  DrawGlyph_128_(Glyph128 g, int x, int y, int c)
   {  for (int j = 0; j < GLYPH128_HEIGHT; j++)
      for (int i = 0; i < GLYPH128_WIDTH; i++)
         if (GlyphGetPixel(g, i, j))  PixelSet(x+i, y+j, c);
   }
   static inline void  DrawGlyph_256_(Glyph256 g, int x, int y, int c)
   {  for (int j = 0; j < GLYPH256_HEIGHT; j++)
      for (int i = 0; i < GLYPH256_WIDTH; i++)
         if (GlyphGetPixel(g, i, j))  PixelSet(x+i, y+j, c);
   }


#endif //include guard
