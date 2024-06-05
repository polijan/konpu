#include "kule.h"

// Basename of the program
char* argv0;

// Usage function
C_ATTRIBUTE_NORETURN
static void Usage(int fail)
{
   fprintf(stderr, "Usage: %s <Options>... \n", argv0);
   fprintf(stderr, "Print a color specified as a hue (defined as a toki pona hue (loje, jelo, laso)\n");
   fprintf(stderr, "or a ratio between two such hues), an added amount of white (walo), and an added\n");
   fprintf(stderr, "amount of black (pimeja).\n\n");

   fprintf(stderr, "Non-color Options\n");
   fprintf(stderr, "  -h, --help          : Print this help\n");
   fprintf(stderr, "  -q, --quiet         : Only print the color code or index, no visual clues.\n");
   fprintf(stderr, "  -v, --verbose       : Increase verbosity (print OkLab details).\n\n");

   fprintf(stderr, "Hue Options\n");
   fprintf(stderr, "  --loje              : Set Hue as 'pure loje'\n");
   fprintf(stderr, "  --loje-jelo[=Ratio] : Set Hue between pure loje and jelo (default: 0.5)\n");
   fprintf(stderr, "  --loje-laso[=Ratio] : Set Hue between pure loje and laso (default: 0.5)\n");
   fprintf(stderr, "  --jelo              : Set Hue as 'pure jelo'\n");
   fprintf(stderr, "  --jelo-loje[=Ratio] : Set Hue between pure jelo and loje (default: 0.5)\n");
   fprintf(stderr, "  --jelo-laso[=Ratio] : Set Hue between pure jelo and laso (default: 0.5)\n");
   fprintf(stderr, "  --laso        : Set Hue as 'pure laso'\n");
   fprintf(stderr, "  --laso-loje[=Ratio] : Set Hue between pure laso and loje (default: 0.5)\n");
   fprintf(stderr, "  --laso-jelo[=Ratio] : Set Hue between pure laso and jelo (default: 0.5)\n");
   fprintf(stderr, "  --radians=Angle     : Instead of using toki pona hues, specify hue as an angle\n");
   fprintf(stderr, "                        in radians. Not compatible with other hue options.\n");
   fprintf(stderr, "  --degrees=Angle     : Instead of using toki pona hues, specify hue as an angle\n");
   fprintf(stderr, "                        in degrees. Not compatible with other hue options.\n");
   fprintf(stderr, "  --hues              : Show visual of the chosen pimeja and walo with varying\n");
   fprintf(stderr, "                        hues. Not compatible with other hue options or the\n");
   fprintf(stderr, "                        --tones option.\n\n");

   fprintf(stderr, "Tone Options\n");
   fprintf(stderr, "  --walo[=Amount]     : Set an amount of white, (default 1, ie pure white)\n");
   fprintf(stderr, "  --pimeja[=Amount]   : Set an amount of black, (default 1, ie pure black)\n");
   fprintf(stderr, "  --tones             : Visually show the chosen hue with varying amount of walo\n");
   fprintf(stderr, "                        and pimeja. Not compatible with --walo, --pimeja, or\n");
   fprintf(stderr, "                        the --hues options\n\n");

   fprintf(stderr, "Others\n");
   fprintf(stderr, "  --from=Color        : Specify hue and walo and pimeja from the given color\n");

   exit((fail)? EXIT_FAILURE : EXIT_SUCCESS);
}


//------------------------------------------------------------------------------
// Misc Utility Functions
//------------------------------------------------------------------------------

// Return the ratio by which to lerp between a and b to get the given result
float rlerp(float result, float a, float b)
{ return (result - a) / (b - a); }

// Return a float [0,1] from a string
// If the string is NULL                       => return the default
// If the string represents a number in [0,1]  => return number
// In any other case, fail.
static float readAmount(const char* arg, float defaultAmount)
{
   if (arg == NULL)  return defaultAmount;

   char *fail;
   float f = strtof(arg, &fail);
   if (*fail != '\0')   Usage(1);
   if (f < 0.f || f > 1.f) {
      fprintf(stderr, "Amount should be in [0,1]\n");
      fprintf(stderr, "`%s -h` to see the usage.\n", argv0);
      exit(EXIT_FAILURE);
   }
   return f;
}


//------------------------------------------------------------------------------
// Hues utilities (ie. hue is an angle / angles are normalized in [-pi,pi])
//------------------------------------------------------------------------------

// Return a normalized [-pi, pi] angle for an OkLab hue
float HueNormalize(float angle)
{ return atan2f(sinf(angle), cos(angle)); }

// Return the circular average of two hues
float HueAverage(float angle1, float angle2)
{ return atan2f(sinf(angle1)+sinf(angle2), cosf(angle1)+cosf(angle2)); }

// Return a linear interpolation between two hues
float HueLerp(float angle1, float angle2, float ratio)
{ return atan2f(lerp(sinf(angle1), sinf(angle2), ratio),
                lerp(cosf(angle1), cosf(angle2), ratio)); }


//------------------------------------------------------------------------------
// Utility functions on RGB structures containg **gamma**-encoded RGB color
// (oklab.h's okhsv_to_srgb() and srgb_to_okhsv works with gamma RGB)
//------------------------------------------------------------------------------

// Get a gamma-encoded RGB from a string
RGB RGBFromString(const char* str)
{
   uint32_t c = SRGBFromString(str);
   return (RGB) { ((c >> 16) & 0xff) / 255.f,
                  ((c >>  8) & 0xff) / 255.f,
                  ( c        & 0xff) / 255.f };
}

// Print a gamma-encoded RGB color
void RGBPrint(RGB rgb, int verbosity)
{
   int r = (int)roundf(255.f * rgb.r);
   int g = (int)roundf(255.f * rgb.g);
   int b = (int)roundf(255.f * rgb.b);
   Lab lab = LabFromSRGB(((uint32_t)r) << 16 | g << 8 | b);
   LabPrint(lab, verbosity);
}

//------------------------------------------------------------------------------
// Utility Functions for converting between the equivalent HSL and HWB models
// see: https://en.wikipedia.org/wiki/HWB_color_model
//      https://bottosson.github.io/posts/colorpicker/
//------------------------------------------------------------------------------

// Return the amount of white in a HSV
float HSVWhite(HSV hsv)
{ return (1.f - hsv.s) * hsv.v; }

// Return the amount of black in a HSV
float HSVBlack(HSV hsv)
{ return 1.f - hsv.v; }

// Create a OkHSV Color from a OkHWB model
HSV HSVFromHWB(float hue, float white, float black)
{
   // If sum of white and black is > 1, scale it.
   float sum = white + black;
   if (sum > 1.0f) {
      white /= sum;
      black /= sum;
   }

   // Change HWB to HSV:
   float value = 1.f - black;
   float saturation = (value != 0.f)? (1.f - white / value) : 1.f;
   return (HSV){ hue, saturation, value };
}

//------------------------------------------------------------------------------
// Toki Pona hues
//------------------------------------------------------------------------------

// Toki Pona hues:
float hueLoje, hueJelo, hueLaso;
float hueGreen, hueBlue;
float hueNalanja; // = mid loje/jelo = "orange"
float hueUnu;     // = mid laso/loje = "violet"


// Initialize Primary Hues
void HuesInit(void)
{
   const uint32_t RGB_RED   = 0xff0000;
   const uint32_t RGB_GREEN = 0x00ff00;
   const uint32_t RGB_BLUE  = 0x0000ff;

   hueLoje = LabHue(LabFromSRGB(RGB_RED));
   hueJelo = LabHue(LabFromSRGB(RGB_GREEN|RGB_RED));

   hueGreen = LabHue(LabFromSRGB(RGB_GREEN));
   hueBlue  = LabHue(LabFromSRGB(RGB_BLUE));
   hueLaso = (hueGreen + hueBlue)/2 - pi;

   hueNalanja = (hueLoje + hueJelo) / 2;
   hueUnu     = HueAverage(hueLaso, hueLoje) - pi;
}

// Return a linearly interpolated hue between loje and jelo
float lerpLojeJelo(float ratio)
{ return lerp(hueLoje, hueJelo, ratio); }

// Return a linearly interpolated hue between jelo and laso
float lerpJeloLaso(float ratio)
{ return HueLerp(hueJelo, hueLaso, ratio); }
  // use hueLerp instead of just lerp because jelo and laso are separated by
  // the -pi/pi line.

// Return a linearly interpolated hue between laso and loje
float lerpLasoLoje(float ratio)
{
   // circular lerping is problematic because laso and loje are more than 180
   // degrees apart (one idea is to do +pi, but there is still an issue when the
   // lerp value is close to either end), so cut the domain in two.
   if (ratio <= 0.5f) {
      return lerp(hueLaso, hueUnu, 2 * ratio);
   } else {
      return lerp(hueUnu, hueLoje, 2 * (ratio - 0.5f));
   }
}


//------------------------------------------------------------------------------


int main(int argc, char* argv[])
{
   argv0 = ProgramName(argv[0]);

   HuesInit();
   float hue    = -1.f;
   float walo   = -1.f;
   float pimeja = -1.f;

   int verbosity = 1;
   RGB rgb;
   HSV hsv;
   bool input = false;
   bool tones = false;
   bool hues  = false;

   // Read command line flags
   int flag;
   Options opt = OptionsInit();
   const char *optstr = "hqv"; //p:";
   OptionsLong longopts[] = { {"help",      OPTIONS_NO_ARGUMENT,       'h'},
                              {"verbose",   OPTIONS_NO_ARGUMENT,       'v'},
                              {"quiet",     OPTIONS_NO_ARGUMENT,       'q'},
                              {"radians",   OPTIONS_REQUIRED_ARGUMENT, 'R'},
                              {"degrees",   OPTIONS_REQUIRED_ARGUMENT, 'D'},
                              {"loje",      OPTIONS_NO_ARGUMENT,       '0'},
                              {"loje-jelo", OPTIONS_OPTIONAL_ARGUMENT, '1'},
                              {"loje-laso", OPTIONS_OPTIONAL_ARGUMENT, '2'},
                              {"jelo",      OPTIONS_NO_ARGUMENT,       '3'},
                              {"jelo-loje", OPTIONS_OPTIONAL_ARGUMENT, '4'},
                              {"jelo-laso", OPTIONS_OPTIONAL_ARGUMENT, '5'},
                              {"laso",      OPTIONS_NO_ARGUMENT,       '6'},
                              {"laso-loje", OPTIONS_OPTIONAL_ARGUMENT, '7'},
                              {"laso-jelo", OPTIONS_OPTIONAL_ARGUMENT, '8'},
                              {"walo",      OPTIONS_OPTIONAL_ARGUMENT, 'w'},
                              {"pimeja",    OPTIONS_OPTIONAL_ARGUMENT, 'p'},
                              {"tones",     OPTIONS_NO_ARGUMENT,       'T'},
                              {"hues",      OPTIONS_NO_ARGUMENT,       'H'},

                              {"from",      OPTIONS_REQUIRED_ARGUMENT, 'F'},
                              {0} };
   while ((flag = OptionsGet(&opt, argc, argv, optstr, longopts)) != -1) {
      switch(flag) {
         default : Usage(1);

         // Non-Color Options:
         case 'h': Usage(argc != 2);
         case 'v': verbosity++; break;
         case 'q': verbosity--; break;

         // Hues Options:
         case 'R': ;char *fail;
                   hue = strtof(opt.arg, &fail);
                   // ^--- must it be normalized it in [-pi,pi] ?
                   if (*fail != '\0') Usage(1);
                   break;
         case 'D': hue = strtof(opt.arg, &fail);
                   if (*fail != '\0') Usage(1);
                   hue *= pi / 180.f;
                   break;
         case '0': hue = hueLoje;  break;
         case '1': hue = lerpLojeJelo(readAmount(opt.arg, 0.5f));  break;
         case '2': hue = lerpLasoLoje(1.f - readAmount(opt.arg, 0.5f));  break;
         case '3': hue = hueJelo;  break;
         case '4': hue = lerpLojeJelo(1.f - readAmount(opt.arg, 0.5f));  break;
         case '5': hue = lerpJeloLaso(readAmount(opt.arg, 0.5f));  break;
         case '6': hue = hueLaso;  break;
         case '7': hue = lerpLasoLoje(readAmount(opt.arg, 0.5f));  break;
         case '8': hue = lerpJeloLaso(1.f - readAmount(opt.arg, 0.5f));  break;
         case 'H': if (tones) Usage(1);
                   hues = true;
                   break;

         // Tones Options:
         case 'w': walo   = readAmount(opt.arg, 1.f);  break;
         case 'p': pimeja = readAmount(opt.arg, 1.f);  break;
         case 'T': if (hues)  Usage(1);
                   tones = true;
                   break;

         // From Options:
         case 'F': input  = true;
                   rgb    = RGBFromString(opt.arg);
                   hsv    = srgb_to_okhsv(rgb);
                   hue    = hsv.h * 2*pi;
                   walo   = HSVWhite(hsv);
                   pimeja = HSVBlack(hsv);
                   break;

      }
   }
   if (opt.ind != argc)  Usage(1);
   if ((hue == -1.f && walo == -1.f && pimeja == -1.f) && (!tones) && (!hues))
      Usage(1);

   if (walo   == -1.f)  walo   = 0.f;
   if (pimeja == -1.f)  pimeja = 0.f;

   if (input && verbosity > 0) {
      printf("Oklab hue  > %+f\n", hue);
      printf("walo       > %f\n",  walo);
      printf("pimeja     > %f\n",  pimeja);
   }

   if (tones && verbosity > 0)  printf("\x1B[33mKULE\x1B[m (walo ala, pimeja ala)\n");
   if (hue == -1.f) {
      hue = 0.f;
   } else if (verbosity > 2) {
      printf("OkLab hue  > %f\n", hue);
      printf("Brightest: "); LabPrint(LabBrightestTone(hue), verbosity);
   }
   hue /= 2*pi; // put hue now in [-0.5, +0.5] as expected for OkHSV, OkHWB

   if (hues) {
      const int N=19;
      for (int h = -N; h <= N; h++) {
          hsv = HSVFromHWB(h/2.0f/N, walo, pimeja);
          rgb = okhsv_to_srgb(hsv);
          RGBPrint(rgb, verbosity);
      }
      putchar('\n');
   } else {
      // Create a HWB, then OkHSV, LinearRGB, and OkLab, and print:
      hsv = HSVFromHWB(hue, walo, pimeja);
      rgb = okhsv_to_srgb(hsv);
      if (verbosity > 2) {
         printf("OkHWB      > hue: %+f, walo: %f, pmja: %f\n", hue, walo, pimeja);
         printf("OkHSV      > hue: %+f, sat.: %f, val.: %f\n", hsv.h, hsv.s, hsv.v);
         printf("lin. RGB   > r  :  %f, g   : %f, b   : %f\n", rgb.r, rgb.g, rgb.b);
      }
      RGBPrint(rgb, verbosity);
   }

   if (tones) {
      // Display square with varying amount of white and black
      printf("\n\x1B[33mWALO EN PIMEJA\x1B[m\n");
      printf("wa  pimeja:             1 1 1 1 1 1 1 1 1 1 2\n");
      printf("lo  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0\n");
      for (int w = 0; w <= 20; w++) {
          printf("%02d ", w);
          for (int p = 0; p <= 20; p++) {
              hsv = HSVFromHWB(hue, .05f * w, .05f * p);
              rgb = okhsv_to_srgb(hsv);
              RGBPrint(rgb, -1);
          }
          putchar('\n');
      }
   }

   return EXIT_SUCCESS;
}
