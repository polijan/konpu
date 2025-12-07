#ifndef RECTANGLE_H_
#define RECTANGLE_H_
#include "arch.h"
#include "util.h"
//------------------------------------------------------------------------------
// Operations on Rectangles
// Note: The rectangle type is defined in arch.h
//------------------------------------------------------------------------------

// Creates a rectangle from a point and vector that translate that point.
// (dx, dy) is similar to width and height but can be negative.
static inline Rectangle
RectangleFromPointAndVector(int x, int y, int dx, int dy)
{
   if (dx < 0)  { x += dx;  dx = -dx; }
   if (dy < 0)  { y += dy;  dy = -dy; }
   return (Rectangle){ .x0 = x, .y0 = y, .width = dx, .height = dy };
}

// Creates a rectangle from any two corners (order doesn’t matter).
static inline Rectangle
RectangleFromTwoPoints(int x1, int x2, int y1, int y2)
{
   int x0 = UTIL_MIN(x1, x2);
   int y0 = UTIL_MIN(y1, y2);
   return (Rectangle){
      .x0 = x0, .y0 = y0,
      .width  = UTIL_MAX(x1, x2) - x0,
      .height = UTIL_MAX(y1, y2) - y0,
   };
}

// Return the Union of two rectangles
// (i.e. the smallest rectangle that contains both)
Rectangle RectangleUnion(Rectangle r1, Rectangle r2);

// Return the Intersection two rectangles
// If there's no intersection: return the {0} rectangle.
Rectangle RectangleIntersection(Rectangle r1, Rectangle r2);

// Return true iff the point (x,y) is in the rectangle
static inline bool
RectangleContainsPoint(Rectangle r, int x, int y)
{
   C_HINT_ASSUME(r.width >= 0 && r.height >= 0);
   return ( x >= r.x0 && x < r.x0 + r.width
         && y >= r.y0 && y < r.y0 + r.height);
}

// Return true iff one rectangle is completely inside another
static inline bool
RectangleContainsRectangle(Rectangle outer, Rectangle inner)
{
   C_HINT_ASSUME(outer.width >= 0 && outer.height >= 0);
   C_HINT_ASSUME(inner.width >= 0 && inner.height >= 0);
   return ( inner.x0 >= outer.x0 &&
            inner.y0 >= outer.y0 &&
            inner.x0 + inner.width  <= outer.x0 + outer.width &&
            inner.y0 + inner.height <= outer.y0 + outer.height);
}

// Return true iff two rectangle have an intersection.
// Note: If you're interested by the actual intersection, compute it using
//       `RectangleIntersection()`, then you can check if an intersection does
//       exist by looking at the result's width (or height) is > 0.
static inline bool
RectangleHasIntersection(Rectangle r1, Rectangle r2)
{
   C_HINT_ASSUME(r1.width >= 0 && r1.height >= 0);
   C_HINT_ASSUME(r2.width >= 0 && r2.height >= 0);
   return !(r2.x0 >= r1.x0 + r1.width  || r2.x0 + r2.width  <= r1.x0 ||
            r2.y0 >= r1.y0 + r1.height || r2.y0 + r2.height <= r1.y0 );
}

// Return the rectangle area, i.e. width*height
// This can also be interpretend as the count of "elements" in the rectangle
static inline int32_t RectangleArea(Rectangle r)
{
   C_HINT_ASSUME(r.width >= 0 && r.height >= 0);
   return (int32_t)r.width * (int32_t)r.height;
}

// Return the given rectangle, translated
static inline C_ATTRIBUTE_NODISCARD Rectangle
RectangleMove(Rectangle r, int dx, int dy)
{ r.x0 += dx; r.y0 += dy; return r; }

// Return the given rectangle, rotated 90 degrees over its center
static inline C_ATTRIBUTE_NODISCARD Rectangle
RectangleRotate(Rectangle r)
{
   C_HINT_ASSUME(r.width >= 0 && r.height >= 0);
   return (Rectangle){
      .x0 = r.x0 + r.width  / 2 - r.height / 2,
      .y0 = r.y0 + r.height / 2 - r.width  / 2,
      .width  = r.height,
      .height = r.width,
   };
}

// Return the given rectangle, scaled from the center.
static inline C_ATTRIBUTE_NODISCARD Rectangle
RectangleInflate(Rectangle r, int dw, int dh)
{
   C_HINT_ASSUME(r.width >= 0 && r.height >= 0);
   r.x0 -= dw;
   r.y0 -= dh;
   r.width  += 2 * dw;
   r.height += 2 * dh;
   return r;
}

#endif //include guard
