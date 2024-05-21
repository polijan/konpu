// Utilities, i/o, palettes, and color functions supplementing oklab.h
//
// As a rule of thumb, when creating kule tools, input or output of colors
// should be using gamma-encoded sRGB colors, but internally, the color
// manipulation is meant to be done using OkLab (or OkLCh, OkHSV, OkHSL).

#ifndef TOOLS_KULE_H
#define TOOLS_KULE_H
#include "oklab.h"
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


//------------------------------------------------------------------------------
// General utility functions
//------------------------------------------------------------------------------

// Include 'options' from Konpu's core (include the *implementation*)
#include "../../../src/options.c"

// Linear interpolation utility function
// The returned value varies linearly from a (when f=0) to b (when f=1)
float lerp(float a, float b, float f)
{ return a + f * (b - a); }

// Malloc but exit on allocation failure
void* MallocOrExit(size_t size) {
   void *ptr = malloc(size);
   if (ptr == NULL) {
      fprintf(stderr, "malloc: Allocating %zu bytes failed\n", size);
      exit(EXIT_FAILURE);
   }
   return ptr;
}

// Calloc but exit on allocation failure
void* CallocOrExit(size_t nElems, size_t size) {
   void *ptr = calloc(nElems, size);
   if (ptr == NULL) {
      fprintf(stderr, "calloc: Allocating %zu elements of size %zu bytes failed\n",
                      nElems, size);
      exit(EXIT_FAILURE);
   }
   return ptr;
}

// Realloc but exit on allocation failure
void* ReallocOrExit(void *pointer, size_t size) {
   void *ptr = realloc(pointer, size);
   if (ptr == NULL) {
      fprintf(stderr, "realloc: Reallocating buffer %p to size %zu bytes failed\n",
                      pointer, size);
      exit(EXIT_FAILURE);
   }
   return ptr;
}

// A few more things with the goal to harmonize C versions
#if __STDC_VERSION__ <= 201710L
#   include <stdbool.h>
#endif
#if __STDC_VERSION__ > 201710L
#   define C_ATTRIBUTE_NORETURN  [[noreturn]]
#elif __STDC_VERSION__ >= 201112L
#   define C_ATTRIBUTE_NORETURN  _Noreturn
#elif defined(__GNUC__)
#   define C_ATTRIBUTE_NORETURN  __attribute__((noreturn))
#else
#   define C_ATTRIBUTE_NORETURN
#endif

// Given main's argv[0], return the program's basename
// Assume / as file spearator
char *ProgramName(const char *argv0)
{
#ifdef _WIN32
#  define FILE_SEPARATOR  '\\'
#else
#  define FILE_SEPARATOR  '/'
#endif
   const char *last = argv0;
   while (*argv0 != '\0') {
      if (*argv0 == FILE_SEPARATOR)  last = argv0;
      argv0++;
   }
   return (char*)last;
}


//------------------------------------------------------------------------------
// Functions related to gamma-encoded sRGB (represented as uint32_t)
//
// As a rule of thumb, when creating kule tools, input or output of colors
// should be using gamma-encoded sRGB colors, but internally, the color
// manipulation is meant to be done using OkLab (or OkLCh, OkHSV, OkHSL, etc.)
//------------------------------------------------------------------------------

// Print a sRGB color
void sRGBPrint(uint32_t color)
{
   unsigned r = color >> 16;
   unsigned g = (color >> 8) & 0xFF;
   unsigned b = color & 0xFF;
   printf ("\x1B[48;2;%d;%d;%dm  \x1B[m", r,g,b);
}


//--------------------------------------------------------------------------
// Functions related to Lab Polar form (LCh)
//
// OkLab is often used in polar form LCh, where C is the Chroma and h is the
// hue angle (in radians). C,h and a,b are related as follow:
//         _________
//        |  2    2                  b
//   C = \| a  + b        h = atan( --- ) = atan2(b, a)
//                                   a
// or reversely:
//
//   a = C cos(h)         b = C sin(h)
//--------------------------------------------------------------------------

typedef struct LCh { float L, C, h; } LCh;

float LabHue(Lab color)
{ return atan2f(color.b, color.a); }

float LabChroma(Lab color)
{ return sqrtf(color.a * color.a + color.b * color.b); }

LCh LabToLCh(Lab color)
{ return (LCh){ color.L, LabChroma(color), LabHue(color) }; }

Lab LabFromLCh(LCh lch)
{ return (Lab){ lch.L, lch.C * cosf(lch.h), lch.C * sinf(lch.h), }; }


//------------------------------------------------------------------------------
// Functions on Lab colors
//------------------------------------------------------------------------------

// Create an okLab color from a (gamma-encoded) sRGB color (encoded as 0xRRGGBB)
Lab LabFromSRGB(uint32_t rgb) {
   return linear_srgb_to_oklab((RGB){
             srgb_transfer_function_inv( ((rgb >> 16) & 0xFF) / 255.f ),
             srgb_transfer_function_inv( ((rgb >>  8) & 0xFF) / 255.f ),
             srgb_transfer_function_inv( ( rgb        & 0xFF) / 255.f ) });
}

// Create an okLab color from a string specifying a sRGB color. Format is
// similar to CSS, i.e. [#]xxxxxx or [#]xxx (with x is a 0-9 or {a,A}-{f,F})
// Exit program if format is wrong
Lab LabFromString(const char* str)
{
   // Input string may start with a '#', skip it.
   if (str[0] == '#')  str++;

   // Read a string of type xxx or xxxxxx
   uint32_t sRGB;
   errno = 0;
   switch (strlen(str)) {
      case 3:; unsigned r,g,b;
               if (sscanf(str, "%1x%1x%1x", &r, &g, &b) != 3)  goto error;
               sRGB = ((uint32_t)r << 20) + ((uint32_t)r << 16)
                      + (g << 12) + (g << 8) + (b <<  4) + b;
               break;

      case 6:; unsigned long longRGB;
               if (sscanf(str, "%06lx", &longRGB) != 1)  goto error;
               sRGB = (uint32_t)longRGB;
               break;

      default: goto error;
   }

   return LabFromSRGB(sRGB);

error:
   if (errno != 0) {
      perror("Error while `scanf`ing a color string");
   } else {
      fprintf(stderr, "Invalid sRGB color: %s\n", str);
   }
   exit(EXIT_FAILURE);
}

// Linear interpolation between two OkLab colors
// (ratio=0 gives the first color, ratio=1 gives the second color)
Lab LabMix(Lab color1, Lab color2, float ratio)
{
   return (Lab){ .L = lerp(color1.L, color2.L, ratio),
                 .a = lerp(color1.a, color2.a, ratio),
                 .b = lerp(color1.b, color2.b, ratio) };
}

// Mix of two OkLab color in same proportion
// functionally same as LabMix(color1, color2, 0.5f)
Lab LabMixEqual(Lab color1, Lab color2)
{
   color1.L += color2.L;  color1.L /= 2;
   color1.a += color2.a;  color1.a /= 2;
   color1.b += color2.b;  color1.b /= 2;
   return color1;
}

// Return the *square* of the Euclidean distance between two colors
float LabDistance2(Lab color1, Lab color2)
{
   Lab d = { color1.L - color2.L, color1.a - color2.a, color1.b - color2.b };
   return d.L*d.L + d.a*d. a + d.b*d.b;
}

// Return the Euclidean distance between two colors
float LabDistance(Lab color1, Lab color2)
{ return sqrtf(LabDistance2(color1, color2)); }

// Return the "opposite" color of the given color
// (so that when the two mixes, it gives a medium gray)
Lab LabOpposite(Lab color)
{ return (Lab){ (1.f - color.L), - color.a, - color.b }; }


// Options Flag for the LabPrint function
#define PRINT_COLOR     (1U << 0)
#define PRINT_SRGB      (1U << 1)
#define PRINT_OKLAB     (1U << 2)
#define PRINT_OKLCH     (1U << 3)
#define PRINT_DEFAULT   PRINT_COLOR|PRINT_SRGB|PRINT_OKLAB
// #define PRINT_DEFAULT   PRINT_COLOR|PRINT_SRGB

// void LabPrint(Lab lab, unsigned PrintOptionsFlag = PRINT_DEFAULT)
//
// Print an okLab color on stdout (assuming a full-color terminal)
#define LabPrint(...) \
        OKLAB_UTIL_OVERLOAD(LabPrint, __VA_ARGS__)
#       define LabPrint_1_(lab) \
               LabPrint_2_(lab, PRINT_DEFAULT)
void LabPrint_2_(Lab lab, unsigned printOptions)
{
   // Convert the Lab color to a gamma-encoded sRGB color.
   // Clamp the value and indicate over/underflows.
   RGB linear = oklab_to_linear_srgb(lab);
   int r = roundf(255.f * srgb_transfer_function(linear.r));
   int  error_r = 1;
   if      (r < 0)   r = 0;
   else if (r > 255) r = 255;
   else error_r = 0;
   int g = roundf(255.f * srgb_transfer_function(linear.g));
   int  error_g = 1;
   if      (g < 0)   g = 0;
   else if (g > 255) g = 255;
   else error_g = 0;
   int b = roundf(255.f * srgb_transfer_function(linear.b));
   int  error_b = 1;
   if      (b < 0)   b = 0;
   else if (b > 255) b = 255;
   else error_b = 0;

   // Print
   if (printOptions == 0)            // we don't want to print nothing
      printOptions = PRINT_DEFAULT;
   if (printOptions & PRINT_COLOR) { // print as a color
      printf("\x1B[48;2;%d;%d;%dm   \x1B[m", r,g,b);
      if (error_r || error_g || error_b) {
         printf("\x1B[31m*\x1B[m ");
      } else {
         printf("  ");
      }
   }
   if (printOptions & PRINT_SRGB) {  // print as #xxxxxxx
      printf("#");
      if (error_r) printf("\x1B[31m");
      printf("%02x", (unsigned)r);
      if (error_r) printf("\x1B[m");
      if (error_g) printf("\x1B[31m");
      printf("%02x", (unsigned)g);
      if (error_g) printf("\x1B[m");
      if (error_b) printf("\x1B[31m");
      printf("%02x", (unsigned)b);
      if (error_b) printf("\x1B[m");
      printf("  ");
   }
   if (printOptions & PRINT_OKLAB)  // print as Lab:(L,a,b)
      printf("Lab(%.3f, %.3f, %.3f)  ", lab.L, lab.a, lab.b);
   if (printOptions & PRINT_OKLCH)  // print as polar Lab:(L,C,h)
      printf("LCh(%.3f, %.3f, %.3f)  ", lab.L, LabChroma(lab), LabHue(lab));
}


//------------------------------------------------------------------------------
// Functions on Palettes
//
// In the filesystem, palette are stored as a list of gamma sRGB colors
//------------------------------------------------------------------------------

#define PALETTE_SIZE_MAX  4096

// Palette
typedef struct Palette {
   Lab *color; // colors of the palette
   int  size;  // the number of colors in the palette
} Palette;


// "Destructor" for Palettes
void PaletteDrop(Palette pal)
{ free(pal.color); }

// Construct an empty Palette (all colors set to Black).
// Exit the program if fails.
Palette PaletteInitEmpty(int size)
{ return (Palette){ .color = CallocOrExit(size, sizeof(Lab)), .size = size }; }

// Construct a Palette by reading it from a stream.
// Exit the program if fails.
Palette PaletteInitFromStream(FILE *stream)
{  assert(stream != NULL);

   // We don't know what the size should be, so just allocate a big temporary
   Palette tmp = PaletteInitEmpty(PALETTE_SIZE_MAX);

   // Read colors from the stream into the palette
   for (tmp.size = 0; tmp.size < PALETTE_SIZE_MAX; tmp.size++) {
      unsigned long c;
      errno = 0;
      int ret = fscanf(stream, "%lx\n", &c);
      if (errno != 0) {
         perror("`fscanf`ing color from palette stream");
         exit(EXIT_FAILURE);
      } else if (ret == EOF) {
         break;
      } else if (ret == 0) {
         break;
      } else {
         assert(ret == 1);
      }
      tmp.color[tmp.size] = LabFromSRGB(c);
   }

   // Copy the temporary into a properly sized palette
   Palette pal = PaletteInitEmpty(tmp.size);
   memcpy(pal.color, tmp.color, tmp.size * sizeof(Lab));
   PaletteDrop(tmp);
   return pal;
}

// Construct a Palette by loading it from a file.
// In case of failure, exit the program.
Palette PaletteInitFromFile(const char *filename)
{
   errno = 0;
   FILE *stream = fopen(filename, "r");
   if (stream == NULL)  goto error;
   Palette pal = PaletteInitFromStream(stream);

   errno = 0;
   if (fclose(stream) != 0)  goto error;
   return pal;

error:
   perror(filename);
   exit(EXIT_FAILURE);
}

// Print a palette
void PalettePrint(Palette palette)
{
   for (int i = 0; i < palette.size; i++) {
      printf("%3d  ", i);
      LabPrint(palette.color[i], PRINT_COLOR|PRINT_SRGB);
      putchar('\n');
   }
}

// Given a color, return the index of the perceptually closest palette entry
int PaletteClosest(Palette palette, Lab color)
{
   // Iterate through the palette to find an entry with the minimum (squared)
   // distance. This is brute force (which is fine in the general case), and
   // obviously one would do better on specific palettes.

   float d2Min = 5.f; // init with < 1 (ie higher that what max distance can be)
   int closest = 0;
   for (int i = 0; i < palette.size; i++) {
      float d2 = LabDistance2(palette.color[i], color);
      if (d2 < d2Min) {
         closest = i;
         d2Min = d2;
      }
   }
   return closest;
}

#endif //TOOLS_KULE_H
