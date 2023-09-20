#include "print.h"
#include "font.h"


static void
canvas_prepareCursor(canvas cvas, int x, int y)
{
   if (x >= cvas.width) {
      x = 0;
      y++;
      while (y >= cvas.height) { // scroll up as needed
         for(int i=0; i < GLYPH_HEIGHT; i++) { sleep_ms(25); canvas_scrollUpPixels(cvas); render(); } //DEBUG
         y--;
      }
   }
}

void print(canvas cvas, int x, int y, const char* str)
{  assert(str);
   CANVAS_CLAMP(cvas, x,y);
/* TODO: remove!
   CANVAS_ASSERT(cvas);
   if (x < 0)           x = 0;
   if (y < 0)           y = 0;
   if (x > cvas.width)  x = cvas.width;
   if (y > cvas.height) y = cvas.height; */

   for(; *str != '\0'; str++) {
      // before writing the glyph, check if cursor is at a correct
      // position, then scroll up, or move if needed.
      if (x >= cvas.width) {
         x = 0;
         y++;
         if (y >= cvas.height) {
            canvas_scrollUp(cvas);
            y = cvas.height;
         }
      }

sleep_ms(50); render(); //DEBUG
      canvas_glyph(cvas, x,y) = chr(*str);
      x++;
   }
}

void print_widehalf(canvas cvas, int x, int y, const char* str)
{ /*TODO:not implemented*/ print_quadrant(cvas, 2*x,y, str); }

void print_tallhalf(canvas cvas, int x, int y, const char* str)
{  assert(str);
   CANVAS_CLAMP(cvas, x,y);
/* TODO: remove!
   CANVAS_ASSERT(cvas);
   if (x < 0)           x = 0;
   if (y < 0)           y = 0;
   if (x > cvas.width)  x = cvas.width;
   if (y > cvas.height) y = cvas.height; */

   bool      left   = true;
   uint64_t *glyph  = canvas_glyphPointer(cvas, x,y);

   for(; *str != '\0'; str++) {

      switch (*str) {
         case '\n':
            x = 0;
            y++; // TODO: we should scroll
            left = true;
            break;

         default :
            ; // <- because in C, a declaration can't immediately follow a label
            uint32_t t = chr_tallhalf(*str);
            if (left) {
               // before writing the tallhalf, check if cursor is at a correct
               // position, then scroll up, or move if needed.
               if (x >= cvas.width) {
                  x = 0;
                  y++;
                  while (y >= cvas.height) { // scroll up as needed
                     for(int i=0; i < GLYPH_HEIGHT; i++) { sleep_ms(25); canvas_scrollUpPixels(cvas); render(); } //DEBUG
                     y--;
                  }
      /*            y++;
                  while (y >= cvas.height) {
                     // we scroll up everything one line: TODO
                     // for now, let's loop back to the origin
                     //canvas_scrollUp(cvas);
                     for(int i=0; i < GLYPH_HEIGHT; i++) { sleep_ms(25); canvas_scrollUpPixels(cvas); render(); } //DEBUG
                     y = cvas.height - 1;
                  }
      */
                  glyph  = canvas_glyphPointer(cvas, x,y);
               }
       
               *glyph = (uint64_t)tallhalf_line0(t) << 0*GLYPH_WIDTH |
                        (uint64_t)tallhalf_line1(t) << 1*GLYPH_WIDTH |
                        (uint64_t)tallhalf_line2(t) << 2*GLYPH_WIDTH |
                        (uint64_t)tallhalf_line3(t) << 3*GLYPH_WIDTH |
                        (uint64_t)tallhalf_line4(t) << 4*GLYPH_WIDTH |
                        (uint64_t)tallhalf_line5(t) << 5*GLYPH_WIDTH |
                        (uint64_t)tallhalf_line6(t) << 6*GLYPH_WIDTH |
                        (uint64_t)tallhalf_line7(t) << 7*GLYPH_WIDTH ;
               left   = false;
         

            } else {
               *glyph |= (uint64_t)tallhalf_line0_H(t) << 0*GLYPH_WIDTH |
                         (uint64_t)tallhalf_line1_H(t) << 1*GLYPH_WIDTH |
                         (uint64_t)tallhalf_line2_H(t) << 2*GLYPH_WIDTH |
                         (uint64_t)tallhalf_line3_H(t) << 3*GLYPH_WIDTH |
                         (uint64_t)tallhalf_line4_H(t) << 4*GLYPH_WIDTH |
                         (uint64_t)tallhalf_line5_H(t) << 5*GLYPH_WIDTH |
                         (uint64_t)tallhalf_line6_H(t) << 6*GLYPH_WIDTH |
                         (uint64_t)tallhalf_line7_H(t) << 7*GLYPH_WIDTH ;
               left    = true;
               glyph++;
               x++;
            }
            render(); sleep_ms(100); //DEBUG
      }
   }
}

void print_quadrant(canvas cvas, int x, int y, const char* str)
{  assert(str);
   CANVAS_CLAMP(cvas, x,y);

   // half-grid coordinates to grid coordinates + quandrant location indicator
   bool left = x % 2; // print quadrant of the left side of a glyph?
   bool top  = y % 2; // print quadrant of the top  side of a glyph?
   x /= 2;
   y /= 2;

   uint16_t  q_left = 0;
   uint64_t *glyph = canvas_glyphPointer(cvas, x,y);

   for (; *str != '\0'; str++) {
       unsigned char c = *str;
       uint16_t q = chr_quadrant(c);

       if (left) {
          q_left = q;
          glyph  = canvas_glyphPointer(cvas, x,y);
          left   = false;
       } else {
          // before writing the quadrant, check if cursor is at a correct
          // position, then scroll up, or move if needed.
          if (x >= cvas.width) {
             x = 0;
             if (top) {
                top = false;
             } else {
                top = true;
                y++;
             }
          }
          if (y >= cvas.height) {
             // we scroll up everything one line: TODO
             // for now, let's loop back to the origin
             y = 0;
          }

          // write the quadrant in the glyph
          uint64_t w = widehalf_fromQuadrants(q_left, q);
          if (top) {
            *glyph = (*glyph & GLYPH_BOTTOM) | w;
          } else {
            *glyph = (*glyph & GLYPH_TOP)    | w >> 32;
          }

/*
          if (top) {
//             *glyph = (uint64_t)widehalf_fromQuadrants(q_left, q) << 32 |
//                      (*glyph & GLYPH_BOTTOM) >> 32        ;
               *glyph = (*glyph & GLYPH_BOTTOM) | (uint64_t)widehalf_fromQuadrants(q_left, q) << 32;
          } else {
             // *glyph = (*glyph & GLYPH_TOP) | widehalf_fromQuadrants(q_left, q) ;
             *glyph = (*glyph & GLYPH_TOP) | widehalf_fromQuadrants(q_left, q);
          }
*/
          left=true;
          x++;
       }
   }


   // if we finished one the bottom-right corner, we need to make sure to clamp the
   // cursor there
   if (x >= cvas.width)  x = cvas.width;
}
