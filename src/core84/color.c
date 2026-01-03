#include "color.h"

// Recursively find the closest Konpu color by performing a Nearest Neighbor
// Seach using the color k-d tree information included in the ROM.
// This is the function called by ColorFromLABf
// The implementation is similar to tools/kule/kule-kdtree.h
static void closest_color(int color,
   float Lx510, float ax510, float bx510,
   int *best_color, float *best_distance2, int depth)
{
   ColorInfo c = Rom.color[color];
   float dL = Lx510 - ((int)c.L_half << 1);
   float da = ax510 - (c.a_minus_12 + 12);
   float db = bx510 - (c.b_plus_30  - 30);

   float d2 = dL*dL + da*da + db*db;
   if (d2 < *best_distance2) {
      *best_distance2 = d2;
      *best_color = color;
   }

   float axis_distance;
   switch (depth % 3) {
      case  0: axis_distance = dL; break;
      case  1: axis_distance = da; break;
      case  2: axis_distance = db; break;
      default: unreachable();
   }

   int near;
   if (axis_distance < 0) {
      near = c.kd_left;
      if (near != 127) {
         closest_color(near, Lx510,ax510,bx510, best_color, best_distance2, depth + 1);
         // in Konpu's palette, if left != 127, right is also != 127
         int far = c.kd_right;
         if ((axis_distance * axis_distance) < *best_distance2)
            closest_color(far, Lx510,ax510,bx510, best_color, best_distance2, depth + 1);
      } else {
         // in Konpu palette, color 216 is the only case where left is 127,
         // but right isn't (in that case right is 211)
         if (color == 216 && ((axis_distance * axis_distance) < *best_distance2))
            closest_color(211 /* <-- right of 216 */,
               Lx510,ax510,bx510, best_color, best_distance2, depth + 1);
      }
   } else {
      near = c.kd_right;
      // in Konpu palette, if right is 127, left is always 127, and we can stop
      if (near == 127) return;
      closest_color(near, Lx510,ax510,bx510, best_color, best_distance2, depth + 1);
      int far = c.kd_left;
      if (((axis_distance * axis_distance) < *best_distance2) && far != 127)
         closest_color(far, Lx510,ax510,bx510, best_color, best_distance2, depth + 1);
   }
}

int ColorFromLABf(struct ColorLABf lab)
{
   int result;
   float d2 = FLT_MAX;
   closest_color( 127, lab.L * 510.f, lab.a * 510.f, lab.b * 510.f,
                  &result, &d2, 0 );
   return result;
}

// Same as closest_color (ie Nearest Neighbor Serach via the k-d tree info
// included in the ROM). This is the function called by ColorFromLABi.
static void closest_color_i(int color,
   int_fast32_t L, int_fast32_t a, int_fast32_t b,
   int *best_color, int_fast32_t *best_distance2, int depth)
{
   ColorInfo c = Rom.color[color];
   int_fast32_t dL = L - ((int_fast32_t)c.L_half << 1);
   int_fast32_t da = a - ((int_fast32_t)c.a_minus_12 + 12);
   int_fast32_t db = b - ((int_fast32_t)c.b_plus_30  - 30);
   int_fast32_t d2 = dL*dL + da*da + db*db;
   if (d2 < *best_distance2) {
      *best_distance2 = d2;
      *best_color = color;
   }

   int_fast32_t axis_distance;
   switch (depth % 3) {
      case  0: axis_distance = dL; break;
      case  1: axis_distance = da; break;
      case  2: axis_distance = db; break;
      default: unreachable();
   }

   int near;
   if (axis_distance < 0) {
      near = c.kd_left;
      if (near != 127) {
         closest_color_i(near, L,a,b, best_color, best_distance2, depth + 1);
         // in Konpu's palette, if left != 127, right is also != 127
         int far = c.kd_right;
         if ((axis_distance * axis_distance) < *best_distance2)
            closest_color_i(far, L,a,b, best_color, best_distance2, depth + 1);
      } else {
         // in Konpu palette, color 216 is the only case where left is 127,
         // but right isn't (in that case right is 211)
         if (color == 216 && ((axis_distance * axis_distance) < *best_distance2))
            closest_color_i(211 /* <-- right of 216 */,
               L,a,b, best_color, best_distance2, depth + 1);
      }
   } else {
      near = c.kd_right;
      // in Konpu palette, if right is 127, left is always 127, and we can stop
      if (near == 127) return;
      closest_color_i(near, L,a,b, best_color, best_distance2, depth + 1);
      int far = c.kd_left;
      if (((axis_distance * axis_distance) < *best_distance2) && far != 127)
         closest_color_i(far, L,a,b, best_color, best_distance2, depth + 1);
   }
}

int ColorFromLABi(struct ColorLABi lab)
{
   int result;
   int_fast32_t d2 = INT_FAST32_MAX;
   closest_color_i(127, lab.L, lab.a, lab.b, &result, &d2, 0);
   return result;
}
