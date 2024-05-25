#include "kule.h"

C_ATTRIBUTE_NORETURN
void Usage(const char* argv0, int fail)
{
   fprintf(stderr, "Usage: %s [Options]... [Color]...\n", ProgramName(argv0));
   fprintf(stderr, "Print/show the given colors (or a palette).\n");
   fprintf(stderr, "Note: Colors can only be omitted if -p flag is used.\n\n");

   fprintf(stderr, "Options\n");
   fprintf(stderr, "  -p,               : Colors may reference indices in the palette.\n");
   fprintf(stderr, "  --palette=Palette   Otherwise they will be replaced by the closest matching\n");
   fprintf(stderr, "                      color existing in the palette.\n");
   fprintf(stderr, "                      If no Colors are given, print the whole palette.\n");
   fprintf(stderr, "  -h, --help        : Print this help\n");
   fprintf(stderr, "  -q, --quiet       : Decrease verbosity (incompatible with -v).\n");
   fprintf(stderr, "                      Only print colors' sRGB codes.\n");
   fprintf(stderr, "  -v, --verbose     : Increase verbosity (print OkLab details).\n");
   fprintf(stderr, "  -w, --width[=N]   : Only show colors visually, no other info.\n");
   fprintf(stderr, "                      N colors per line is printed (default N=16)\n");

   exit((fail)? EXIT_FAILURE : EXIT_SUCCESS);
}


int main(int argc, char *argv[])
{
   // Read command line flags
   const char* palFile = NULL;
   int verbosity = 1;
   int flag;
   int w = 0;
   Options opt = OptionsInit();
   const char *optstr = "hqvp:w::";
   OptionsLong longopts[] = { {"help",    OPTIONS_NO_ARGUMENT, 'h'},
                              {"verbose", OPTIONS_NO_ARGUMENT, 'v'},
                              {"quiet",   OPTIONS_NO_ARGUMENT, 'q'},
                              {"palette", OPTIONS_REQUIRED_ARGUMENT, 'p'},
                              {"width",   OPTIONS_OPTIONAL_ARGUMENT, 'w'},
                              {0} };
   while ((flag = OptionsGet(&opt, argc, argv, optstr, longopts)) != -1) {
      switch(flag) {
         default : Usage(argv[0], 1);
         case 'h': Usage(argv[0], argc != 2);
         case 'v': verbosity++; break;
         case 'q': verbosity--; break;
         case 'w': verbosity = -1;
                   if(opt.arg != NULL)  w = atoi(opt.arg);
                   break;
         case 'p': palFile = opt.arg; break;;
      }
   }
   if (w == 0)  w = 16;

   // Read remaining command line arguments
   if (palFile == NULL) {
      if (opt.ind == argc)  Usage(argv[0], 1);
      // Print true colors from arguments
      for (int i = opt.ind; i < argc; i++) {
         Lab color = LabFromString(argv[i]);
         LabPrint(color, verbosity);
         if (verbosity < 0  &&  (i+1) % w == 0) putchar('\n');
      }
   } else {
      Palette pal = PaletteInitFromFile(palFile);

      if (opt.ind == argc) {
         // Print all colors of a palette
         for (int i = 0; i < pal.size; i++) {
            if (verbosity >= 0)  printf("%3d ", i);
            LabPrint(pal.color[i], verbosity);
            if (verbosity < 0  &&  (i+1) % w == 0) putchar('\n');
         }
      } else {
         // Print palette colors from arguments
         for (int i = opt.ind; i < argc; i++) {
            int idx = PaletteIndexFromString(pal, argv[i]);
            printf("%3d ", idx);
            LabPrint(pal.color[idx], verbosity);
            if (verbosity < 0  &&  (i+1) % w == 0) putchar('\n');
         }
      }

      PaletteDrop(pal);
   }
   return EXIT_SUCCESS;
}
