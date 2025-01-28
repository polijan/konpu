// Generate a color palette by using k-Means to partition all pixels of a given
// image into clusters based on the OkLab distance.

#define  KULE_OPTION_USE_KMEANS
#include "../src/kule.h"

// Print usage and exit
C_ATTRIBUTE_NORETURN
void Usage(const char* argv0, int fail)
{
   fprintf(stderr, "Usage: %s [Options]... Image N\n\n", ProgramName(argv0));
   fprintf(stderr, "Generate and print a palette of N colors based on the given image.\n");
   fprintf(stderr, "The tool uses the k-Means algorithm to partition all pixels of the\n");
   fprintf(stderr, "image into N clusters based on the OkLab distance.\n\n");

   fprintf(stderr, "Arguments:\n");
   fprintf(stderr, "  Image is an image file\n");
   fprintf(stderr, "  N is the number of colors in the palette (1..%d)\n\n", PALETTE_SIZE_MAX);

   fprintf(stderr, "Options TODO:\n");
   fprintf(stderr, "  -h, --help     : Print this help\n");
   fprintf(stderr, "  -q, --quiet    : Decrease verbosity (incompatible with -v).\n");
   fprintf(stderr, "                   Only print colors' sRGB codes.\n");
   fprintf(stderr, "  -v, --verbose  : Increase verbosity (print OkLab details).\n\n");

   fprintf(stderr, "Return Value:\n");
   fprintf(stderr, "%3d  Success\n", KMEANS_OK);
   fprintf(stderr, "%3d  Exceeded max. k-Means iterations\n", KMEANS_EXCEEDED_MAX_ITERATIONS);
   fprintf(stderr, "%3d  Another error in k-Means\n", KMEANS_ERROR);
   fprintf(stderr, "%3d  Error unrelated to k-Means (wrong usage, failed allocation, etc.)\n", EXIT_FAILURE);

   exit((fail)? EXIT_FAILURE : KMEANS_OK);
}


int main(int argc, char *argv[])
{
   // Read command line flags
   int verbosity = 0; // by default, just print the palette
   int flag;
   Options opt = OptionsInit();
   const char *optstr = "hqvp:";
   OptionsLong longopts[] = { {"help",    OPTIONS_NO_ARGUMENT, 'h'},
                              {"verbose", OPTIONS_NO_ARGUMENT, 'v'},
                              {"quiet",   OPTIONS_NO_ARGUMENT, 'q'},
                              {"palette", OPTIONS_REQUIRED_ARGUMENT, 'p'},
                              {0} };
   while ((flag = OptionsGet(&opt, argc, argv, optstr, longopts)) != -1) {
      switch(flag) {
         default : Usage(argv[0], 1);
         case 'h': Usage(argv[0], argc != 2);
         case 'v': verbosity++; break;
         case 'q': verbosity--; break;
         // -p: TODO (restrict the choice of colors to the given color palette)
      }
   }

   // Create Image from given filename
   if (opt.ind != argc - 2)  Usage(argv[0], 1);
   Image img = ImageInitFromFile(argv[opt.ind++]);

   // Create empty Palette from the given size
   int sz = atoi(argv[opt.ind]);
   if (sz < 1 || sz > PALETTE_SIZE_MAX)  Usage(argv[0], 1);
   Palette pal = PaletteInitEmpty(sz);

   // Run PaletteFromImage()) and print the resulting palette.
   int return_val = PaletteFromImage(&pal, img);
   if (return_val != KMEANS_OK) return return_val;
   PalettePrint(pal, verbosity);

   // Clean
   PaletteDrop(pal);
   ImageDrop(img);
   return EXIT_SUCCESS;
}
