#include "kule.h"

C_ATTRIBUTE_NORETURN
void Usage(const char* argv0, int fail)
{
   fprintf(stderr, "Usage: %s [Options]... [Palette]\n", ProgramName(argv0));
   fprintf(stderr, "Convert a palette (or all sRGB colors if no palette is given) to OkLab\n");
   fprintf(stderr, "colors, then print the min. and max. possible values for L,a,b.\n\n");

   fprintf(stderr, "Options:\n");
   fprintf(stderr, "  -h, --help     : Print this help\n");
   fprintf(stderr, "  -p, --palette  : Ignored\n");
   exit((fail)? EXIT_FAILURE : EXIT_SUCCESS);
}

// Initial min and max values
float LMin =  2.f;
float LMax = -2.f;
float aMin =  2.f;
float aMax = -2.f;
float bMin =  2.f;
float bMax = -2.f;

float CMin =  2.f;
float CMax = -2.f;
float HMin =  4.f;
float HMax = -4.f;


// Check a Lab color components and update min. and max. values if needed
static void UpdateMinMax(Lab color)
{
   if (color.L < LMin)  LMin = color.L;
   if (color.a < aMin)  aMin = color.a;
   if (color.b < bMin)  bMin = color.b;
   if (color.L > LMax)  LMax = color.L;
   if (color.a > aMax)  aMax = color.a;
   if (color.b > bMax)  bMax = color.b;

   float C = LabChroma(color);
   float H = LabHue(color);
   if (C < CMin)  CMin = C;
   if (H < HMin)  HMin = H;
   if (C > CMax)  CMax = C;
   if (H > HMax)  HMax = H;

}


int main(int argc, char *argv[])
{
   // Read command line flags
   int flag;
   Options opt = OptionsInit();
   const char *optstr = "hp";
   OptionsLong longopts[] = { {"help",    OPTIONS_NO_ARGUMENT, 'h'},
                              {"palette", OPTIONS_NO_ARGUMENT, 'p'},
                              {0} };
   while ((flag = OptionsGet(&opt, argc, argv, optstr, longopts)) != -1) {
      switch(flag) {
         default : Usage(argv[0], 1);
         case 'h': Usage(argv[0], argc != 2);
         case 'p': break;
      }
   }

   // Read palette arg (if any),
   // Iterate through colors and upate min and max values
   switch (argc - opt.ind) {
      default: Usage(argv[0], 1);

      case 0:  for (uint32_t r = 0; r < 256; r++)
                  for (uint32_t g = 0; g < 256; g++)
                     for (uint32_t b = 0; b < 256; b++)
                        UpdateMinMax(LabFromSRGB(r << 16 | g << 8 | b));
               break;

      case 1:  ;Palette pal = PaletteInitFromFile(argv[argc - 1]);
               for (int i = 0; i < pal.size; i++)
                  UpdateMinMax(pal.color[i]);
               PaletteDrop(pal);
               break;
   }

   // Print value and exit
   printf("L:  min: %+f, max: %+f\n", LMin, LMax);
   printf("a:  min: %+f, max: %+f\n", aMin, aMax);
   printf("b:  min: %+f, max: %+f\n", bMin, bMax);
   printf("C:  min: %+f, max: %+f\n", CMin, CMax);
   printf("H:  min: %+f, max: %+f\n", HMin, HMax);
   return EXIT_SUCCESS;
}
