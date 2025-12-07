#include "rectangle.h"
#include "video.h"

// Redefine as inline functions instead of macros,
// to avoid avoid evaluating parameters several times.
static inline int min(int x, int y) { return UTIL_MIN(x, y); }
static inline int max(int x, int y) { return UTIL_MAX(x, y); }


Rectangle RectangleUnion(Rectangle r1, Rectangle r2)
{
   C_HINT_ASSUME(r1.width >= 0 && r1.height >= 0);
   C_HINT_ASSUME(r2.width >= 0 && r2.height >= 0);

   int x0 = min(r1.x0, r2.x0);
   int y0 = min(r1.y0, r2.y0);
   return (Rectangle){
      .x0 = x0, .y0 = y0,
      .width  = max(r1.x0 + r1.width,  r2.x0 + r2.width)  - x0,
      .height = max(r1.y0 + r1.height, r2.y0 + r2.height) - y0,
   };
}

Rectangle RectangleIntersection(Rectangle r1, Rectangle r2)
{
   C_HINT_ASSUME(r1.width >= 0 && r1.height >= 0);
   C_HINT_ASSUME(r2.width >= 0 && r2.height >= 0);

   int x1 = max(r1.x0, r2.x0);
   int y1 = max(r1.y0, r2.y0);
   int x2 = min(r1.x0 + r1.width,  r2.x0 + r2.width);
   int y2 = min(r1.y0 + r1.height, r2.y0 + r2.height);
   if (x2 > x1 && y2 > y1) {
      return (Rectangle){
         .x0 = x1,
         .y0 = y1,
         .width  = x2 - x1,
         .height = y2 - y1,
      };
   } else {
      return (Rectangle){0};
   }
}