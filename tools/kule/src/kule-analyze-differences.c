#include "kule.h"

#define STEP_MAX  16

C_ATTRIBUTE_NORETURN
void Usage(const char* argv0, int fail)
{
   fprintf(stderr, "Usage: %s [Options]... Palette\n", ProgramName(argv0));
   fprintf(stderr, "Compute distances between sRGB colors and their closest approximation in\n");
   fprintf(stderr, "the given palette. Print the average and max distance.\n\n");

   fprintf(stderr, "Options\n");
   fprintf(stderr, "  -h, --help     : Print this help\n");
   fprintf(stderr, "  -p, --palette  : Ignored\n");
   fprintf(stderr, "  -q, --quiet    : Only print the average and max distance\n");
   fprintf(stderr, "                   Otherwise, add more details, including visualization of\n");
   fprintf(stderr, "                   the colors which have the max difference.\n");
   fprintf(stderr, "  -v, --verbose  : Print more info on colors.\n");
   fprintf(stderr, "  -f, --fast [N] : Set the step at which R,G,B components vary as N (1-%d).\n", STEP_MAX);
   fprintf(stderr, "                   Without this, step is 1, thus every sRGB color is sampled.\n");
   fprintf(stderr, "                   Increasing N divides the compute time by around N^3.\n");

   exit((fail)? EXIT_FAILURE : EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
   // Read command line arguments
   int verbosity = 1;
   bool quiet = false;
   int step = 1;
   int flag;
   Options opt = OptionsInit();
   const char *optstr = "hpqvf:";
   OptionsLong longopts[] = { {"help",    OPTIONS_NO_ARGUMENT,       'h'},
                              {"palette", OPTIONS_NO_ARGUMENT,       'p'},
                              {"quiet",   OPTIONS_NO_ARGUMENT,       'q'},
                              {"verbose", OPTIONS_NO_ARGUMENT,       'v'},
                              {"fast",    OPTIONS_REQUIRED_ARGUMENT, 'f'},
                              {0} };
   while ((flag = OptionsGet(&opt, argc, argv, optstr, longopts)) != -1) {
      switch(flag) {
         default : Usage(argv[0], 1);
         case 'h': Usage(argv[0], argc != 2);
         case 'v': verbosity++;  break;
         case 'q': quiet = true; break;
         case 'p': break;
         case 'f': step = atoi(opt.arg);
                   if (step < 1 || step > STEP_MAX)  Usage(argv[0], 1);
      }
   }
   if (opt.ind != argc - 1)  Usage(argv[0], 1);
   Palette pal = PaletteInitFromFile(argv[opt.ind]);


   // Compute the distance between sRGB colors and their closest approximation
   // in the palette. Keep the parameters when the difference is maximum.
   Lab color = {0.f, 0.f, 0.f};
   int index = 0;
   float dMax = 0.f;
   double dSum = 0.;
   int32_t nColors = 0;
   for (int r = 0; r < 256; r += step) {
      for (int g = 0; g < 256; g += step) {
         for (int b = 0; b < 256; b += step) {
            float d;
            Lab c = LabFromSRGB(r<<16 | g<<8 | b);
            int i = PaletteClosest(pal, c, &d);
            // To compute the average:
            dSum += d;
            nColors++;
            // Keeping track of the max
            if (d > dMax) {
               dMax  = d;
               color = c;
               index = i;
            }

         }
      }
   }


   // Print the color and palette entry that differ the most
   if (!quiet)  printf("Average distance: ");
   printf("%f\n", dSum / nColors);
   if (!quiet)  printf("Max. distance   : ");
   printf("%f\n", dMax);
   if (!quiet) {
      printf("     true color -> ");
      LabPrint(color, verbosity);
      printf("     index %3d  -> ", index);
      LabPrint(pal.color[index], verbosity);
      putchar('\n');
   }


   PaletteDrop(pal);
   return EXIT_SUCCESS;
}
