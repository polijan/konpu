#include "kule.h"
#include "kule-kdtree.h"
#include "kule-konpu.h"
#include <time.h>


// Print number of difference between results of PaletteClosest and TreeClosest
// (hopefully 0)
static void check_misses(struct Node *kd_tree, Palette pal)
{
   printf("Checking Miss:\n");
   int32_t misses = 0;
   for (int r = 0; r < 256; r++)
   for (int g = 0; g < 256; g++)
   for (int b = 0; b < 256; b++) {
      int from_pal  = PaletteClosest(      pal, LabFromSRGB(r,g,b), NULL);
      int from_tree = TreeClosest(kd_tree, pal, LabFromSRGB(r,g,b), NULL);
      if (from_pal != from_tree) {
         misses++;
         printf("     ");            LabPrint(LabFromSRGB(r,g,b), 2);
         printf("%3d: ", from_pal);  LabPrint(pal.color[from_pal], 2);
         printf("%3d: ", from_tree); LabPrint(pal.color[from_tree], 2);
      }
   }
   printf("=> %"PRId32" miss\n\n", misses);
}


// Benchmark PaletteClosest vs TreeClosest on all 16.7M sRGB colors
// The conversion from sRGB->Lab is common in every case in both case, so is
// also measured.
void benchmark(struct Node *kd_tree, Palette pal)
{
   volatile int res;
   volatile Lab lab;
   time_t t;

   printf("Benchmark:\n");

   printf("sRGB -> Lab ...   "); fflush(stdout);
   t = time(NULL);
   for (int r = 0; r < 256; r++)
   for (int g = 0; g < 256; g++)
   for (int b = 0; b < 256; b++) {
      lab = LabFromSRGB(r,g,b);
   }
   printf("%d\n", (int)(time(NULL) - t));

   printf("PaletteClosest... "); fflush(stdout);
   t = time(NULL);
   for (int r = 0; r < 256; r++)
   for (int g = 0; g < 256; g++)
   for (int b = 0; b < 256; b++) {
      res = PaletteClosest(pal, LabFromSRGB(r,g,b), NULL);
   }
   printf("%d\n", (int)(time(NULL) - t));

   printf("TreeClosest...    "); fflush(stdout);
   t = time(NULL);
   for (int r = 0; r < 256; r++)
   for (int g = 0; g < 256; g++)
   for (int b = 0; b < 256; b++) {
      res = TreeClosest(kd_tree, pal, LabFromSRGB(r,g,b), NULL);
   }
   printf("%d\n", (int)(time(NULL) - t));

   (void)res;
   (void)lab;
}


int main(void)
{
   Palette pal = PaletteKonpu();
   Node* kd = TreeInit(pal);
   NodeAssert(kd, pal);

   check_misses(kd, pal);

   benchmark(kd, pal);

   // Clean-Up
   TreeDrop(kd);
   return EXIT_SUCCESS;
}
