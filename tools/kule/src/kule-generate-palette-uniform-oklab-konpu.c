#include "kule.h"

// Generate a uniform palette (to be adopted as the master 256-color palette for
// ilo Konpu) by sampling the OkLab space and making sure colors we peek have a
// minimum distance to each other (note: in fact if we want to do this well,
// this is analogous to filling the OkLab "box" with spheres and the known best
// ways to arrange the colors/spehere using an FCC or HCP lattice, see:
// https://en.wikipedia.org/wiki/Close-packing_of_equal_spheres, so perhaps we
// do this in the future). While picking the color in OkLab, any color which
// isn't a valid one in sRGB will be discarded.
//
// It is a better and far simpler approach than partitioning all sRGB colors
// using k-means fitted with an OkLab distance (as is done by the program
// `kule-generate-palette-uniform-via-kmeans`). The only main drawback is that
// we don't control the number of resulting colors, so we have to fiddle with
// what should be the minimum distance between color and other parameters.


//------------------------------------------------------------------------------
// kule-analyze-range gives those ranges    a: min:-0.233887, max:+0.276216
// for OkLab colors that are valid sRGB     b: min:-0.311528, max:+0.198570
#  define KULE_MIN_A -0.233886f
#  define KULE_MAX_A +0.276217f
#  define KULE_MIN_B -0.311527f
#  define KULE_MAX_B +0.198571f

// Colors we will populate
int count = 0;
Lab colors[PALETTE_SIZE_MAX] = {0};

// Keep count of rejected colors
int64_t rejected_not_sRGB = 0; // rejected because color doesn't exist in sRGB
int64_t rejected_similar  = 0; // rejected because too close to another color

// D    = Desired minimum distance between any two colors in the palette
// STEP = Step at which we iterate in OkLab dimension
// uint32_t desired_colors[] = Fixed (sRGB) colors we want in the palette
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Getting all fiddly with parameters to get 256 colors
//------------------------------------------------------------------------------
#define D 0.069f
#define STEP 0.004f
int verbosity = 0;
uint32_t desired_colors[] = {
   0x000000, // full black (pimeja)
   0xFFFFFF, // full white (walo)
   0x636363, // neutral gray
   0xFF0000, // full red   (vivid loje)
};
#  undef  KULE_MIN_A
#  undef  KULE_MAX_A
#  undef  KULE_MIN_B
#  undef  KULE_MAX_B
#  define KULE_MIN_A -0.5f
#  define KULE_MAX_A +0.5f
#  define KULE_MIN_B -0.5f
#  define KULE_MAX_B +0.5f
//------------------------------------------------------------------------------


int main(void)
{
   // Pre-populate the result with some colors we desire to have in the palette
   assert(UTIL_ARRAY_LENGTH(desired_colors) < PALETTE_SIZE_MAX);
   for (; count < UTIL_ARRAY_LENGTH(desired_colors); count++) {
      colors[count] = LabFromSRGB(desired_colors[count]);
   }

   // Iterate through OkLab space
   for (float L =        0.f; L <       1.0f; L += STEP)
   for (float a = KULE_MIN_A; a < KULE_MAX_A; a += STEP)
   for (float b = KULE_MIN_B; b < KULE_MAX_B; b += STEP) {
      Lab c = { L, a, b };

      // If color is not valid in sRGB space, reject it.
      RGB rgb = oklab_to_linear_srgb(c);
      if (  rgb.g > 1.f || rgb.g < 0.f ||
            rgb.b > 1.f || rgb.b < 0.f ||
            rgb.r > 1.f || rgb.r < 0.f ) {
         rejected_not_sRGB++;
         continue;
      }

      // If color is too close from a color already in the palette, reject it.
      for (int i = 0; i < count; i++) {
         if (LabDistance2(c, colors[i]) <= (D*D)) {
            rejected_similar++;
            goto continue_oklab_loop;
         }
      }

      // Check that we don't create too many colors (happens if D is too small)
      if (count > PALETTE_SIZE_MAX) {
         fprintf(stderr, "The distance (%f) leads to more than %d colors.\n",
            D, PALETTE_SIZE_MAX);
         exit(EXIT_FAILURE);
      }

      // Add the color
      colors[count++] = c;

continue_oklab_loop:  ;
   }


   // Print the results
   Palette pal = { .size = count, .color = colors };
   for (int i = 0; i < pal.size; i++) {
      if (verbosity) printf("%3d: ", i);
      LabPrint(pal.color[i], verbosity);
   }
   if (verbosity) {
      printf("count            : %d\n", count);
      printf("rejected non-sRGB: %" PRId64 "\n", rejected_not_sRGB);
      printf("rejected same    : %" PRId64 "\n", rejected_similar);
   }
   return EXIT_SUCCESS;
}
