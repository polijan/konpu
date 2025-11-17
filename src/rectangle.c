#include "rectangle.h"
#include "video.h"


// redefine as inline functions instead of macros,
// to avoid avoid evaluating params several times.
static inline int min(int x, int y) { return UTIL_MIN(x, y); }
static inline int max(int x, int y) { return UTIL_MAX(x, y); }

/* TODO: DELETE
Rectangle RectangleFromScreen(void)
{
   int w,h;
   if (VideoModeDimension() == PIXELS_Nx1) {
      // video has Pixels
      w = Video.width;
      h = Video.height;
   } else if (VideoModeHasAttributes() || VideoModeLowNibble() < 9) {
      // video has Glyphs
      w = VIDEO_WIDTH_GLYPH;
      h = VIDEO_HEIGHT_GLYPH;
   } else { // or tiles
      // TODO/FIXME: video has Tiles
      // w = VIDEO_WIDTH_TILE;
      // h = VIDEO_HEIGHT_TILE;
   }
   return (Rectangle){ 0,0, w,h };
}

#define RECTANGLE_SCREEN                                        \
   ((VideoModeDimension() == PIXELS_Nx1) ?                          \
      RECTANGLE_PIXEL_SCREEN         :                          \
      (Rectangle){ 0,0, VIDEO_WIDTH_GLYPH, VIDEO_HEIGHT_GLYPH } )


      RECTANGLE_SCREEN

Rectangle RectangleFromScreen(void)
{
   if (VideoModeDimension() == PIXELS_Nx1) { // video has Pixels
      return RECTANGLE_PIXEL_SCREEN;
   } else { // video has Glyphs or Tiles
      return (Rectangle){
         // VIDEO_WIDTH_GLYPH and VIDEO_WIDTH_TILE macro are in fact the same,
         // and same for HEIGHso in any case:
         .width  = VIDEO_WIDTH_GLYPH,
         .height = VIDEO_HEIGHT_GLYPH,
      }
   }
}

   int w,h;
   if (VideoModeDimension() == PIXELS_Nx1) { // video has Pixels
      w = Video.width;
      h = Video.height;
   } else {
      // video has Glyphs or Tiles
      // VIDEO_WIDTH_GLYPH and VIDEO_WIDTH_TILE are computed the same,
      // so in any case:
      w = VIDEO_WIDTH_GLYPH;
      h = VIDEO_HEIGHT_GLYPH;
   }
   return (Rectangle){ 0,0, w,h };
}
*/

Rectangle RectangleUnion(Rectangle r1, Rectangle r2)
{
   C_HINT_ASSUME(r1.width >= 0 && r1.height >= 0);
   C_HINT_ASSUME(r2.width >= 0 && r2.height >= 0);

   int x0 = UTIL_MIN(r1.x0, r2.x0);
   int y0 = UTIL_MIN(r1.y0, r2.y0);
   return (Rectangle){
      .x0 = x0, .y0 = y0,
      .width  = UTIL_MAX(r1.x0 + r1.width,  r2.x0 + r2.width)  - x0,
      .height = UTIL_MAX(r1.y0 + r1.height, r2.y0 + r2.height) - y0,
   };
}

Rectangle RectangleIntersection(Rectangle r1, Rectangle r2)
{
   C_HINT_ASSUME(r1.width >= 0 && r1.height >= 0);
   C_HINT_ASSUME(r2.width >= 0 && r2.height >= 0);

   int x1 = UTIL_MAX(r1.x0, r2.x0);
   int y1 = UTIL_MAX(r1.y0, r2.y0);
   int x2 = UTIL_MIN(r1.x0 + r1.width,  r2.x0 + r2.width);
   int y2 = UTIL_MIN(r1.y0 + r1.height, r2.y0 + r2.height);
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