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
#include <inttypes.h>
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

// Create an OkLab color from a string specifying a sRGB color. Format is
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
               sRGB = ((uint32_t)r << 20) | ((uint32_t)r << 16)
                      | (g << 12) | (g << 8) | (b <<  4) | b;
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

// Print an OkLab color on stdout.
// What is printed depends on verbosity:
//   < 0: just visually print the color
//     0: print sRGB code
//     1: similar to 0, but also visually print the color
//  >= 2: same as 1 and also print OkLab details
void LabPrint(Lab lab, int verbosity)
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

   // Print actual color
   if (verbosity != 0) {
      printf ("\x1B[48;2;%d;%d;%d", r,g,b);
      if (error_r || error_g || error_b) {
         printf(";38;2;%sm *\x1B[m", (lab.L > 0.5f)? "0;0;0":"255;255;255");
      } else {
         printf ("m  \x1B[m");
      }
   }
   if (verbosity < 0) return;

   // Print sRGB code #RRGGBB
   if (verbosity == 0) {
      printf("#%02x%02x%02x", (unsigned)r, (unsigned)g, (unsigned)b);
      if (error_r || error_g || error_b)
         printf("*");
   } else {
      printf(" #");
      if (error_r) printf("\x1B[31m");
      printf("%02x", (unsigned)r);
      if (error_r) printf("\x1B[m");
      if (error_g) printf("\x1B[31m");
      printf("%02x", (unsigned)g);
      if (error_g) printf("\x1B[m");
      if (error_b) printf("\x1B[31m");
      printf("%02x", (unsigned)b);
      if (error_b) printf("\x1B[m");
   }

   // Print OkLab details (L,a,b, Lr, Chroma, Hue[in radians])
   // Lr: alt. lightness estimate assuming a reference white Y=1
   if (verbosity > 1) {
      printf(" > L:%.3f a:%+.3f b:%+.3f Lr:%.3f Chroma:%.3f Hue:%+.3f",
                 lab.L, lab.a, lab.b, toe(lab.L), LabChroma(lab), LabHue(lab));
   }

   putchar('\n');
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

#define PALETTE_NULL  ((Palette){.color=NULL, .size=0})


// "Destructor" for Palettes
void PaletteDrop(Palette pal)
{ free(pal.color); }

// Construct an empty Palette (all colors set to Black).
// Exit the program if fails.
Palette PaletteInitEmpty(int size)
{ return (Palette){ .color = CallocOrExit(size, sizeof(Lab)), .size = size }; }

// Copy constructor (construct a Palette as a clone from another palette)
// Exit the program if the allocation fails
Palette PaletteClone(Palette pal)
{
   Palette clone = PaletteInitEmpty(pal.size);
   memcpy(clone.color, pal.color, pal.size * sizeof(Lab));
   return clone;
}

// Construct a Palette by reading it from a stream.
// Exit the program if fails.
Palette PaletteInitFromStream(FILE *stream)
{  assert(stream != NULL);
   Palette tmp = PaletteInitEmpty(PALETTE_SIZE_MAX);
   tmp.size = 0; // <-- keep the real count of colors read

   while (1) {
      errno = 0;
      int c = fgetc(stream);
      if (errno != 0)  goto stream_error;
      switch (c) {
      case ';': // Comments: if needed, read the stream until \n (or EOF)
         while (1) {
            errno = 0;
            int i = fgetc(stream);
            if (errno != 0) goto stream_error;
            if (i == EOF)   goto end;
            if (i == '\n')  break;
         }
         continue;

      case '\n': // Empty line
      case '#': // Optional # (in front of a color)
         continue;

      case EOF: // Reached EOF
         goto end;

      default:  // Read color
         errno = 0;
         //if (fseek(stream, -1, SEEK_CUR) != 0)  goto stream_error;
         if (ungetc(c, stream) == EOF)  goto stream_error;
         uint32_t color;
         errno = 0;
         int scanned = fscanf(stream, "%06" SCNx32 "\n", &color);
         if (errno != 0)  goto stream_error;
         if (scanned == 1) {
            if (tmp.size == PALETTE_SIZE_MAX) {
               fprintf(stderr, "Too Many Entries\n");
               goto error;
            }
            tmp.color[tmp.size] = LabFromSRGB(color);
            tmp.size++;
         } else {
            fprintf(stderr, "Palette format incorrect\n");
            goto error;
         }
      }
   }

end:
   ;Palette pal = PaletteClone(tmp);
   PaletteDrop(tmp);
   return pal;

stream_error:
   perror("Error while reading palette stream");
error:
   PaletteDrop(tmp);
   exit(EXIT_FAILURE);
}


// Construct a Palette by loading it from a file.
// If filename is "-", then load it from standard input instead.
// In case of failure, exit the program.
Palette PaletteInitFromFile(const char *filename)
{
   if (strcmp(filename, "-") == 0) {
      return PaletteInitFromStream(stdin);
   }

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

// Given a color, return the index of the perceptually closest palette entry
// If distanceOut is not NULL, assign the distance difference in it.
int PaletteClosest(Palette palette, Lab color, float *distanceOut)
{  assert(palette.color != NULL);
   assert(palette.size > 0);

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

   if (distanceOut != NULL)
      *distanceOut = sqrtf(d2Min);
   return closest;
}

// Return a palette color index from a string.
// If the string indicates an index that exists in the palette, return it.
// Otherwise if the string represents a color, return the index of the palette
// color that is perceptually closest.
// In any other case, exit the program
int PaletteIndexFromString(Palette pal, const char* str)
{
   char *fail;
   long l = strtol(str, &fail, 10);
   if (*fail != '\0' || l >= pal.size) {
      return PaletteClosest(pal, LabFromString(str), NULL);
   }
   if (l < 0) {
      fprintf(stderr, "Invalid color: %s\n", str);
      exit(EXIT_FAILURE);
   }
   return l;
}


#endif //TOOLS_KULE_H
