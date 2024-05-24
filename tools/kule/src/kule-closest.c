#include "kule.h"

C_ATTRIBUTE_NORETURN
void Usage(const char* argv0, int fail)
{
   fprintf(stderr, "Usage: %s [Options]... Palette Color\n", ProgramName(argv0));
   fprintf(stderr, "Print the Palette's entry that is perceptually the closest to the given Color.\n\n");

   fprintf(stderr, "Options\n");
   fprintf(stderr, "  -h, --help     : Print this help\n");
   fprintf(stderr, "  -p, --palette  : Ignored\n");
   fprintf(stderr, "  -v, --verbose  : Increase verbosity (print OkLab details).\n");
   fprintf(stderr, "  -q, --quiet    : Decrease verbosity (incompatible with -v).\n");
   fprintf(stderr, "                   Applied once : only print palette index and distance.\n");
   fprintf(stderr, "                   Applied twice: only print a palette index.\n");

   exit((fail)? EXIT_FAILURE : EXIT_SUCCESS);
}


int main(int argc, char *argv[])
{
   // Read command line flags
   int verbosity = 1;
   int flag;
   Options opt = OptionsInit();
   const char *optstr = "hqpv";
   OptionsLong longopts[] = { {"help",    OPTIONS_NO_ARGUMENT, 'h'},
                              {"palette", OPTIONS_NO_ARGUMENT, 'p'},
                              {"verbose", OPTIONS_NO_ARGUMENT, 'v'},
                              {"quiet",   OPTIONS_NO_ARGUMENT, 'q'},
                              {0} };
   while ((flag = OptionsGet(&opt, argc, argv, optstr, longopts)) != -1) {
      switch(flag) {
         default : Usage(argv[0], 1);
         case 'h': Usage(argv[0], argc != 2);
         case 'v': verbosity++; break;
         case 'q': if(verbosity > 0) verbosity = 0;
                   verbosity--; break;
         case 'p': break;
      }
   }

   // Read remaining command line arguments
   if (argc - opt.ind != 2)  Usage(argv[0], 1);
   Palette pal = PaletteInitFromFile(argv[argc - 2]);
   Lab color = LabFromString(argv[argc - 1]);

   // Compute and Print result
   float d;
   int closest = PaletteClosest(pal, color, &d);
   if (verbosity < 0)   printf("%d\n", closest);
   if (verbosity < -1)  return EXIT_SUCCESS;
   if (verbosity < 0)   { printf("%f\n", d); return EXIT_SUCCESS; }
   printf("input    : ");           LabPrint(color, verbosity);
   printf("color %3d: ", closest);  LabPrint(pal.color[closest], verbosity);
   printf("distance : %f\n",        d);

   return EXIT_SUCCESS;
}
