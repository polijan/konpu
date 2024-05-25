#include "kule.h"

C_ATTRIBUTE_NORETURN
void Usage(const char* argv0, int fail)
{
   fprintf(stderr, "Usage: %s [Options]... ColorFrom ColorTo N\n", ProgramName(argv0));
   fprintf(stderr, "Print a gradient between two colors in N intermediate colors\n\n");

   fprintf(stderr, "Options\n");
   fprintf(stderr, "  -h, --help        : Print this help\n");
   fprintf(stderr, "  -q, --quiet       : Only print the color code or index, no visual clues.\n");
   fprintf(stderr, "  -v, --verbose     : Increase verbosity (print OkLab details).\n");

   exit((fail)? EXIT_FAILURE : EXIT_SUCCESS);
}

static long readN(int argc, char* argv[])
{
   char *fail;
   long l = strtol(argv[argc - 1], &fail, 10);
   if (*fail != '\0')   Usage(argv[0], 1);
   return l + 2;
}

int main(int argc, char* argv[])
{
   // Read command line flags
   const char *palFile = NULL;
   int verbosity = 1;
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
         case 'p': palFile = opt.arg; break;
      }
   }
   if (opt.ind != argc - 3)  Usage(argv[0], 1);

   float ratio = 0.f;
   if (palFile == NULL) {
      // No palette: Gradient between true colors
      Lab from = LabFromString(argv[argc - 3]);
      Lab to   = LabFromString(argv[argc - 2]);
      long n = readN(argc, argv);
      for (long i = 0; i < n; i++) {
         LabPrint(LabMix(from, to, ratio), verbosity);
         ratio += 1.f / (n - 1);
      }
   } else {
      // Palette: Gradient and everytime lookup for the closest entry
      Palette pal = PaletteInitFromFile(palFile);
      Lab from  = pal.color[ PaletteIndexFromString(pal, argv[argc - 3]) ];
      Lab to    = pal.color[ PaletteIndexFromString(pal, argv[argc - 2]) ];
      long n = readN(argc, argv);
      for (long i = 0; i < n; i++) {
         Lab color = LabMix(from, to, ratio);
         int idx = PaletteClosest(pal, color, NULL);
         if (verbosity == 0) {
            printf("%d\n", idx);
         } else {
            if (verbosity > 0)  printf("%03d: ", idx);
            LabPrint(pal.color[idx], verbosity);
         }
         ratio += 1.f / (n - 1);
      }
      PaletteDrop(pal);
   }

   if (verbosity < 0)  putchar('\n');
   return EXIT_SUCCESS;
}
