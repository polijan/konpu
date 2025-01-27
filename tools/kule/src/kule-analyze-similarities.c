#include "kule.h"

#define DEFAULT_TRESHOLD  0.03f

C_ATTRIBUTE_NORETURN
void Usage(const char* argv0, int fail)
{
   fprintf(stderr, "Usage: %s [Options]... Palette [Distance]\n\n", ProgramName(argv0));
   fprintf(stderr, "Print the color pairs in the given palette for which the perceptual\n");
   fprintf(stderr, "similarity is less than a given distance (by default %f).\n\n", DEFAULT_TRESHOLD);
   fprintf(stderr, "Options:\n");
   fprintf(stderr, "  -q, --quiet    : Only print the number of color pairs\n");
   fprintf(stderr, "  -v, --verbose  : Print more info on color pairs (can be applied several times)\n");
   fprintf(stderr, "  -h, --help     : Print this help\n");
   fprintf(stderr, "  -p, --palette  : Ignored\n");
   exit((fail)? EXIT_FAILURE : EXIT_SUCCESS);
}

// A pair of colors and their distance
typedef struct {
   uint8_t color1;
   uint8_t color2;
   float   distance;
} ColorPair;

// Comparison function for sorting an array of ColorPair with `qsort`
static int ColorPairCompare(const void* pair1, const void* pair2)
{
   float d1 = ((const ColorPair*)pair1)->distance;
   float d2 = ((const ColorPair*)pair2)->distance;
   return (d1 < d2)? -1 : 1;
}


int main(int argc, char *argv[])
{
   // Read command line arguments
   int flag;
   bool quiet = false;
   int verbosity = 0;
   Options opt = OptionsInit();
   const char *optstr = "hpqv";
   OptionsLong longopts[] = { {"help",    OPTIONS_NO_ARGUMENT, 'h'},
                              {"palette", OPTIONS_NO_ARGUMENT, 'p'},
                              {"quiet",   OPTIONS_NO_ARGUMENT, 'q'},
                              {"verbose", OPTIONS_NO_ARGUMENT, 'v'},
                              {0} };
   while ((flag = OptionsGet(&opt, argc, argv, optstr, longopts)) != -1) {
      switch(flag) {
         default : Usage(argv[0], 1);
         case 'h': Usage(argv[0], argc != 2);
         case 'v': verbosity++;  break;
         case 'q': quiet = true; break;
         case 'p': break;
      }
   }

   if (argc - opt.ind < 1)  Usage(argv[0], 1);
   // Palette
   Palette pal = PaletteInitFromFile(argv[opt.ind]);
   // Treshold
   float treshold;
   switch (argc - opt.ind) {
      default: Usage(argv[0], 1);
      case 1:  treshold = DEFAULT_TRESHOLD;
               break;
      case 2:; char *fail;
               treshold = strtof(argv[opt.ind + 1], &fail);
               if (*fail != '\0' || treshold < 0.f || treshold >= 1.0f)
                  Usage(argv[0], 1);
   }


   // Create and populate a `distance` array containing all distance between
   // colors in the palette (as distance is commutative, we restrict to i>j)
   int32_t size = (pal.size * (pal.size - 1)) / 2;
   ColorPair *distance = malloc(size * sizeof(ColorPair));
   int32_t idx = 0;
   for (int i = 0; i < pal.size; i++) {
       for (int j = 0; j < i; j++) {
          distance[idx] = (ColorPair){ i, j, LabDistance(pal.color[i], pal.color[j])};
          idx++;
       }
   }
   assert(idx == size);
   // Get the distance array sorted by increasing distance
   qsort(distance, size, sizeof(ColorPair), ColorPairCompare);

   // Print colors under the treshold
   int i;
   for(i = 0; i < size; i++) {
      if (distance[i].distance >= treshold)  break;
      if (!quiet) {
         ColorPair pair = distance[i];
         printf("%3d ", pair.color1);
         LabPrint(pal.color[pair.color1], -1);
         LabPrint(pal.color[pair.color2], -1);
         printf(" %3d => %f\n", pair.color2, pair.distance);
         if (verbosity != 0) {
             printf("                "); LabPrint(pal.color[pair.color1], verbosity);
             printf("                "); LabPrint(pal.color[pair.color2], verbosity);
         }
      }
   }
   if (quiet) {
      printf("%d\n", i);
   } else {
      printf("==> number of pairs with distance <= %f: %d\n", treshold, i);
   }

   // Print median difference of every color in the palette
   if (!quiet) {
      float median;
      if (size % 2) {
         median = distance[size/2].distance;
      } else {
         median = (distance[size/2 - 1].distance + distance[size/2 + 1].distance) / 2;
      }
      printf ("==> median distance of every palette colors: %f\n", median);
   }

   // Exit
   free(distance);
   PaletteDrop(pal);
   return EXIT_SUCCESS;
}
