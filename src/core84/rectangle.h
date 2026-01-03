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

#if 0
// Coerce argument into a Rectangle to work around the utter broken-ness of C's
// _Generic statement.
// WHY?: In C, _Generic selects an expression at compile-time based on the type
// of its first argument. The problem is that every expression, even the ones
// that will not be selected, must be semantically valid. This is needlessly
// problematic and frankly annoying.
// To understand more, read this article:
// https://www.chiark.greenend.org.uk/~sgtatham/quasiblog/c11-generic/

/* TODO: REMOVE THIS PART
// Transform a surface into a Rectangle
#define PRIVATE_RECTANGLE_(surface)                               \
   _Generic(surface                                             , \
      Rectangle: (surface)                                      , \
      Window * : PRIVATE_ENSURE_WINDOW_PTR_(surface)->rectangle , \
      VideoSurface_: Video.active                                 \
   )
#  define PRIVATE_ENSURE_WINDOW_PTR_(surface)      \
      _Generic(surface,                            \
         Window * : (surface)                      \
         default  : PrivateWindowPtr_((surface)),  \
      )
*/



// Transform a surface into a Rectangle
#define PRIVATE_RECTANGLE_(surface)                                            \
   _Generic((surface)                                                        , \
      Rectangle    : (surface)                                               , \
      VideoSurface_: Video.active                                            , \
      Window*      :                                                           \
         /* The following is necessary because every expression in _Generic */ \
         /* (even the ones that won't be selected) must be semantically     */ \
         /* valid. Thus we must ensure validity for non-Window* too.        */ \
         _Generic((surface)                                                  , \
            Window*: (surface)                                               , \
            default: PrivateCoerceToWindowPtr_()                               \
         )->rectangle                                                          \
   )
   Window* PrivateCoerceToWindowPtr_(void); // (no implementation needed)

/* ^---  TODO MAYBE/LATER: Perhaps, it can be valuable to have a RECTANGLE macro
         in rectangle.h which construct Rectangle from anything. In that case,
         we could use that instead.
*/
#endif

/*
// Obtain a Rectangle from a Surface, or (an origin and) width/height
//
// Rectangle RECTANGLE(Video)                 // Video.active rectangle
// Rectangle RECTANGLE(int window_number);    // from a window number ???
// Rectangle RECTANGLE(const Window *window); // from a window
// Rectangle RECTANGLE(Rectangle rectangle);  // copy from another rectangle
// Rectangle RECTANGLE([int x0,y0 = 0,0], int width, height); // from parameters

//#define RECTANGLE(...) UTIL_OVERLOAD(RECTANGLE, __VA_ARGS__)
//#  define RECTANGLE_4_(x0,y0, w,h)     ((Rectangle){(x0),(y0), (w),(h)})
//#  define RECTANGLE_2_(w, h)           ((Rectangle){0,0, (w),(h)})

//#  define RECTANGLE_1_(surface)                                        \
*/



#  define RECTANGLE(surface)                                        \
      _Generic((surface)                                                     , \
         Rectangle    : (surface)                                            , \
         VideoSurface_: Video.active                                         , \
         Window*      :                                                        \
            /* The following is necessary because every expression in  */      \
            /* _Generic (even the ones that won't be selected) must be */      \
            /* semantically valid. Thus we must ensure validity for    */      \
            /* non-Window* types too.                                  */      \
            _Generic((surface)                                               , \
               Window*: (surface)                                            , \
               default: PrivateCoerceToWindowPtr_()                            \
            )->rectangle                                                       \
      )
   Window* PrivateCoerceToWindowPtr_(void); // (no implementation needed)


   #endif //include guard
