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


#endif //include guard
