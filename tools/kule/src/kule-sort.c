// Sort a palette

#include "kule.h"

C_ATTRIBUTE_NORETURN
void Usage(const char* argv0, int fail)
{
   fprintf(stderr, "Usage: %s [Options]... Palette\n", ProgramName(argv0));
   fprintf(stderr, "Print the Palette with sorted entries.\n\n");

   fprintf(stderr, "General Options\n");
   fprintf(stderr, "  -h, --help      : Print this help\n");
   fprintf(stderr, "  -p, --palette   : Ignored\n");
   fprintf(stderr, "  -v, --verbose   : Increase verbosity (print OkLab details)\n");
   fprintf(stderr, "  -q, --quiet     : Print in palette format\n\n");

   fprintf(stderr, "Sorting Options (Use only one! Case matters!)\n");
   fprintf(stderr, "  -L, --lightness : Sort by ascending Oklab's perceptual lightness (default)\n");
   fprintf(stderr, "  -a              : Sort by ascending OkLab's a (green-to-redness)\n");
   fprintf(stderr, "  -b              : Sort by ascending OkLab's b (blue-to-yellowness)\n");
   fprintf(stderr, "  -C, --chroma    : Sort by ascending OkLab's Chroma\n");
   fprintf(stderr, "  -S, --saturation: Sort by ascending Saturation (Chroma/Lightness)\n");
   fprintf(stderr, "  -H, --hue       : Sort by ascending OkLab's Hue\n");
   fprintf(stderr, "  -R, --red       : Sort by ascending sRGB red   component\n");
   fprintf(stderr, "  -G, --green     : Sort by ascending sRGB green component\n");
   fprintf(stderr, "  -B, --blue      : Sort by ascending sRGB blue  component\n");
   fprintf(stderr, "  -r, --reverse   : Reverse the palette order\n");
   fprintf(stderr, "  -0, --nothing   : Do not sort\n");

   exit((fail)? EXIT_FAILURE : EXIT_SUCCESS);
}


// A Struct to contain the color and the old index
typedef struct {
   Lab  color;
   int  oldIndex;
} ColorAndIndex;


// Global ColorCompare function pointer that compare two OkLab colors
int (*ColorCompare)(Lab color1, Lab color2);

// Adapter for calling the global ColorCompare in `qsort`
static int QSortCompare(const void *elem1, const void *elem2)
{
   Lab l1 = ((const ColorAndIndex *)elem1)->color;
   Lab l2 = ((const ColorAndIndex *)elem2)->color;
   return ColorCompare(l1, l2);
}

//-- Various comparison function which can be assigned to ColorCompare ---------

int SortByL(Lab color1, Lab color2)
{ return (color1.L < color2.L)? -1 : 1; }

int SortByA(Lab color1, Lab color2)
{ return (color1.a < color2.a)? -1 : 1; }

int SortByB(Lab color1, Lab color2)
{ return (color1.b < color2.b)? -1 : 1; }

int SortByHue(Lab color1, Lab color2)
{ return (LabHue(color1) < LabHue(color2))? -1 : 1; }

int SortByChroma(Lab color1, Lab color2)
{ return (LabChroma(color1) < LabChroma(color2))? -1 : 1; }

int SortBySaturation(Lab color1, Lab color2)
{ return (LabSaturation(color1) < LabSaturation(color2))? -1 : 1; }

int SortByRed(Lab color1, Lab color2) {
  RGB rgb1 = oklab_to_linear_srgb(color1);
  RGB rgb2 = oklab_to_linear_srgb(color2);
  return (rgb1.r < rgb2.r)? -1 : 1;
}

int SortByGreen(Lab color1, Lab color2) {
  RGB rgb1 = oklab_to_linear_srgb(color1);
  RGB rgb2 = oklab_to_linear_srgb(color2);
  return (rgb1.g < rgb2.g)? -1 : 1;
}

int SortByBlue(Lab color1, Lab color2) {
  RGB rgb1 = oklab_to_linear_srgb(color1);
  RGB rgb2 = oklab_to_linear_srgb(color2);
  return (rgb1.b < rgb2.b)? -1 : 1;
}

int Reverse(Lab color1, Lab color2)
{ (void)color1; (void)color2; return 1; }

int Nothing(Lab color1, Lab color2)
{ (void)color1; (void)color2; return -1; }



int main(int argc, char *argv[])
{  (void)argc; (void)argv;
   ColorCompare = SortByL;

   // Read command line flags
   int verbosity = 1;
   int flag;
   Options opt = OptionsInit();
   const char *optstr = "hqpvLabCHSRGBr0";
   OptionsLong longopts[] = { {"help",       OPTIONS_NO_ARGUMENT, 'h'},
                              {"palette",    OPTIONS_NO_ARGUMENT, 'p'},
                              {"verbose",    OPTIONS_NO_ARGUMENT, 'v'},
                              {"quiet",      OPTIONS_NO_ARGUMENT, 'q'},
                              {"lightness",  OPTIONS_NO_ARGUMENT, 'L'},
                              {"chroma",     OPTIONS_NO_ARGUMENT, 'C'},
                              {"hue",        OPTIONS_NO_ARGUMENT, 'H'},
                              {"saturation", OPTIONS_NO_ARGUMENT, 'S'},
                              {"red",        OPTIONS_NO_ARGUMENT, 'R'},
                              {"green",      OPTIONS_NO_ARGUMENT, 'G'},
                              {"blue",       OPTIONS_NO_ARGUMENT, 'B'},
                              {"reverse",    OPTIONS_NO_ARGUMENT, 'r'},
                              {"nothing",    OPTIONS_NO_ARGUMENT, '0'},
                              {0} };
   while ((flag = OptionsGet(&opt, argc, argv, optstr, longopts)) != -1) {
      switch(flag) {
         default : Usage(argv[0], 1);
         case 'h': Usage(argv[0], argc != 2);
         case 'v': verbosity++; break;
         case 'q': verbosity--; break;
         case 'p': break;
         case 'L': ColorCompare = SortByL; break;
         case 'a': ColorCompare = SortByA; break;
         case 'b': ColorCompare = SortByB; break;
         case 'C': ColorCompare = SortByChroma; break;
         case 'H': ColorCompare = SortByHue; break;
         case 'S': ColorCompare = SortBySaturation; break;
         case 'R': ColorCompare = SortByRed; break;
         case 'G': ColorCompare = SortByGreen; break;
         case 'B': ColorCompare = SortByBlue; break;
         case 'r': ColorCompare = Reverse; break;
         case '0': ColorCompare = Nothing; break;
      }
   }

   // Read arguments
   if (argc - opt.ind != 1)  Usage(argv[0], 1);
   Palette pal = PaletteInitFromFile(argv[argc - 1]);

   // Create a ColorAndIndex array and populate it from the palette.
   ColorAndIndex* sorted = MallocOrExit(pal.size * sizeof(ColorAndIndex));
   for (int i = 0; i < pal.size; i++) {
      sorted[i] = (ColorAndIndex){ .oldIndex = i, .color = pal.color[i] };
   }

   // Sort And Print the Result
   qsort(sorted, pal.size, sizeof(ColorAndIndex), QSortCompare);

   for (int i = 0; i < pal.size; i++) {
      if (verbosity > 0) printf("%3d (prev. %3d) ", i, sorted[i].oldIndex);
      LabPrint(sorted[i].color, verbosity);
   }

   // Exit
   free(sorted);
   PaletteDrop(pal);
   return EXIT_SUCCESS;
}
