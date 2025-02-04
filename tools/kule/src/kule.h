// Utilities, i/o, palettes, and color functions supplementing oklab.h
//
// As a rule of thumb, when creating kule tools, input or output of colors
// should be using gamma-encoded sRGB colors, but internally, the color
// manipulation is meant to be done using OkLab (or OkLCh, OkHSV, OkHSL).

#ifndef TOOLS_KULE_H
#define TOOLS_KULE_H


// stb_image (https://github.com/nothings/stb)
// Defining KULE_OPTION_USE_STB_IMAGE will include stb_image which allows the
// function `ImageInitFromFile(filename)` to open different image file formats
// instead of just PPM. Custom PPM-only image loading remain available via
// `ImageInitFromPPMFile(filename)`
#ifdef KULE_OPTION_USE_STB_IMAGE
   // stb_libraries are full of warnings (and possibly UBs too). I get plenty of
   // discarding const warnings and some casting pointers with different
   // alignements. Beurk!
#  pragma  GCC diagnostic push
#  pragma  GCC diagnostic ignored "-Wcast-align"
#  pragma  GCC diagnostic ignored "-Wcast-qual"
#  define STB_IMAGE_IMPLEMENTATION // actually include implementation too.
#  include "../external/stb_image.h"
#  pragma  GCC diagnostic pop
#endif

// OkLab colors
#include "oklab.h"

// C standard library
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

// Include 'Options' from Konpu's core (actually derived from STC library)
// (includes the *implementation*)
#include "../../../src/options.c"

// k-Means Clustering
// Defining KULE_OPTION_USE_KMEANS will include k-means clustering functions.
// This also is need to include finding the Palette for an Image.
#ifdef KULE_OPTION_USE_KMEANS
   // 1. Include the kmeans *implementation* in order to compile everything in a
   //    single program.
   // 2. Also temporarily disable warnings about comparing integer expressions
   //    of different signs. Such comparisons in 'kmeans' appear safe.
#  pragma  GCC diagnostic push
#  pragma  GCC diagnostic ignored "-Wsign-compare"
#  include "../external/kmeans/kmeans.c" // include the implementation
#  pragma  GCC diagnostic pop
#endif


//------------------------------------------------------------------------------
// General utility functions
//------------------------------------------------------------------------------

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
#if __STDC_VERSION__ < 202311L
#   include <stdbool.h>
#endif
#if __STDC_VERSION__ >= 202311L
#   define C_ATTRIBUTE_NORETURN       [[noreturn]]
#   define C_ATTRIBUTE_MAYBE_UNUSED   [[maybe_unused]]
#elif defined(__GNUC__)
#   define C_ATTRIBUTE_NORETURN       __attribute__((noreturn))
#   define C_ATTRIBUTE_MAYBE_UNUSED   __attribute__((unused))
#elif __STDC_VERSION__ >= 201112L
#   define C_ATTRIBUTE_NORETURN       _Noreturn
#   define C_ATTRIBUTE_MAYBE_UNUSED
#else
#   define C_ATTRIBUTE_NORETURN
#   define C_ATTRIBUTE_MAYBE_UNUSED
#endif

#include <stddef.h>
#ifndef unreachable
#   if defined(__GNUC__)
#      define unreachable()   (__builtin_unreachable())
#   elif defined(_MSC_VER)
#      define unreachable()   (__assume(false))
#   else
#      define unreachable()   ((void)0)
#   endif
#endif

// Given main's argv[0], return the program's basename
// Assume / as file spearator
const char *ProgramName(const char *argv0)
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
   return last;
}

// Return the size of an array (note: as a `int32_t`, not a `size_t`)
#define UTIL_ARRAY_LENGTH(x) ((int32_t)(sizeof(x) / sizeof((x)[0])))

// Clamp the value of `n` into [0..dimension[
static inline void UtilClampCoordinate(int *n, int dimension) {
   if (*n < 0) {
      *n = 0;
   } else if (*n >= dimension) {
      *n = dimension - 1;
   }
}

//------------------------------------------------------------------------------
// Functions related to Lab Polar form (LCh)
//
// OkLab is often used in polar form LCh, where C is the Chroma and h is the hue
// angle (in radians). C,h and a,b are related as follow:
//         _________
//        |  2    2                  b
//   C = \| a  + b        h = atan( --- ) = atan2(b, a)
//                                   a
// or reversely:
//
//   a = C cos(h)         b = C sin(h)
//------------------------------------------------------------------------------

typedef struct LCh { float L, C, h; } LCh;

// Return a color of the given hue that can be displayed in sRGB with the almost
// max chroma (= 0.1275) that allows to display every hue as a valid sRGB color.
// The corresponsing lightness of the color is exactly 75%.
#define LChTone(hue)    ((LCh){ 0.75f, 0.1275f, hue })
                        // ^ Note: I determined the 0.1275 chroma and 75% light
                        // "empirically" from a OkLHc color chooser and that's
                        // accurate up to the 4th decimal place.

// Saturation is Chroma / Lightness
float LChSaturation(LCh color)
{ return (color.L == 0)? INFINITY : (color.C / color.L); }

float LabHue(Lab color)
{ return atan2f(color.b, color.a); }

float LabChroma(Lab color)
{ return sqrtf(color.a * color.a + color.b * color.b); }

float LabSaturation(Lab color)
{ return (color.L == 0)? INFINITY : (LabChroma(color) / color.L); }

LCh LabToLCh(Lab color)
{ return (LCh){ color.L, LabChroma(color), LabHue(color) }; }

Lab LabFromLCh(LCh lch)
{ return (Lab){ lch.L, lch.C * cosf(lch.h), lch.C * sinf(lch.h), }; }

// Return a color of the given hue that can be displayed in sRGB with the almost
// max chroma (= 0.1275) that allows to display every hue as a valid sRGB color.
// The corresponsing lightness of the color is exactly 75%.
Lab LabTone(float hue)
{ return LabFromLCh(LChTone(hue)); }

// Return a color of the given hue that can be displayed in sRGB with
// maximum lightness and chroma
Lab LabBrightestTone(float hue)
{
   // normalize a and b so that a^2+b^2=1, which is expected by find_cusp
   float a = cosf(hue);
   float b = sinf(hue);
   LC rgb_max = find_cusp(a, b);

   // Lab color corresponding to LC and the given hue:
   return (Lab) { rgb_max.L, rgb_max.C * a , rgb_max.C * b };
}


//------------------------------------------------------------------------------
// Functions on Lab colors
//------------------------------------------------------------------------------

// Return a uint32_t sRGB colors from a string.
// Exit program if format is wrong
// Format for the string is similar to CSS, i.e. [#]xxxxxx or [#]xxx (with x is
// a 0-9 or {a,A}-{f,F})
uint32_t SRGBFromString(const char* str)
{
   if (str == NULL) {
      fprintf(stderr, "Empty color string\n");
      exit(EXIT_FAILURE);
   }

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
   return sRGB;

error:
   if (errno != 0) {
      perror("Error while `scanf`ing a color string");
   } else {
      fprintf(stderr, "Invalid sRGB color: %s\n", str);
   }
   exit(EXIT_FAILURE);
}

// Create an okLab color from a (gamma-encoded) sRGB color.
// the sRGB color can be given as one argument (encoded as 0xRRGGBB)
// or as three r,g,b integers.
#define LabFromSRGB(...) \
   OKLAB_UTIL_OVERLOAD(LabFromSRGB, __VA_ARGS__)
   Lab LabFromSRGB_3_(int r, int g, int b) {
      return linear_srgb_to_oklab((RGB){
               srgb_transfer_function_inv(r / 255.f),
               srgb_transfer_function_inv(g / 255.f),
               srgb_transfer_function_inv(b / 255.f) });
   }
   static inline Lab LabFromSRGB_1_(uint32_t rgb)
   { return LabFromSRGB_3_((rgb >> 16) & 0xFF, (rgb >> 8) & 0xFF, rgb & 0xFF); }

// Create an OkLab color from a string specifying a sRGB color.
// Exit program if format is wrong
Lab LabFromString(const char* str)
{ return LabFromSRGB(SRGBFromString(str)); }

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
         // display half as clamped color
         printf(";38;2;%sm*\x1B[m", (lab.L > 0.5f)? "0;0;0":"255;255;255");
         // display second half with gamut clipping
         // (see: https://bottosson.github.io/posts/gamutclipping/)
         RGB clip = gamut_clip_adaptive_L0_0_5(linear);
         r = roundf(255.f * srgb_transfer_function(clip.r));
         g = roundf(255.f * srgb_transfer_function(clip.g));
         b = roundf(255.f * srgb_transfer_function(clip.b));
         if      (r < 0)   r = 0;
         else if (r > 255) r = 255;
         if      (g < 0)   g = 0;
         else if (g > 255) g = 255;
         if      (b < 0)   r = 0;
         else if (b > 255) r = 255;
         printf ("\x1B[48;2;%d;%d;%dm \x1B[m", r,g,b);
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
   if (verbosity == 2) {
      printf(" > L:%.3f a:%+.3f b:%+.3f Chroma:%.3f Hue:%+.3f",
                 lab.L, lab.a, lab.b, LabChroma(lab), LabHue(lab));
   } else if (verbosity > 2) {
      // same with more precision and some more fields...
      printf(" > L:%f a:%+f b:%+f Chroma:%f Hue:%+f Lr:%f Sat.:%f",
                 lab.L, lab.a, lab.b, LabChroma(lab), LabHue(lab),
                 toe(lab.L), LabSaturation(lab));
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
   if (pal.size == 0) return (Palette){0};
   assert(pal.color != NULL);

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

// Print a palette
void PalettePrint(Palette pal, int verbosity)
{
   assert(pal.color != NULL);
   if (pal.size == 0 && verbosity)  printf("<empty palette>\n");

   for (int i = 0; i < pal.size; i++) {
      if (verbosity) printf("%3d: ", i);
      LabPrint(pal.color[i], verbosity);
   }
}


//------------------------------------------------------------------------------
// Functions on Images
//
// In the filesystem, palette are stored as a list of gamma sRGB colors
//------------------------------------------------------------------------------

// Type representing an Image
typedef struct Image {
   Lab  *data;
   int   width;
   int   height;
   int   pitch;
         // The pitch is the stride between the image's lines,
         // see: https://en.wikipedia.org/wiki/Stride_of_an_array
         // (pitch != width) means image has been cropped from another image.
} Image;

// Create an Empty Image
Image ImageInitEmpty(int width, int height)
{
   return (Image) {
      .data   = CallocOrExit(width * height, sizeof(Lab)),
      .width  = width  ,
      .height = height ,
      .pitch  = width  };
}

// Release memory owned by an image.
void ImageDrop(Image img)
{ free(img.data); }

// Initialize a new Image as a clone from another palette.
// Exit the program if the allocation fails
Image ImageClone(Image img)
{
   if (img.width == 0 || img.height == 0) return (Image){0};
   assert(img.data != NULL);

   Image clone =  ImageInitEmpty(img.width, img.height);
   if (img.pitch == img.width) {
      // fast copy of pixels
      memcpy(clone.data, img.data, sizeof(Lab) * img.width * img.height);
   } else {
      // copy line by line
      for (int y = 0; y < img.height; y++) {
         size_t line = y * img.pitch;
         memcpy(clone.data + line, img.data + line, sizeof(Lab) * img.width);
      }
   }
   return clone;
}

// Return the pixel at coordinate (x,y) in the image. This is a lvalue.
// if coordinate (x,y) if out of the image, it will be clamped to the edge.
#define ImagePixel(img, x,y)    \
   (*ImagePixel_((img), (x),(y)))
   Lab* ImagePixel_(Image img, int x, int y) {
      UtilClampCoordinate(&x, img.width);
      UtilClampCoordinate(&y, img.height);
      return img.data + x + y * img.pitch;
   }

// Image ImageCrop(Image img, [int x, int y], int width, int height);
//
// Return a part of the image starting at (x,y) of the given width and height.
// If x and y are not provided, the cropping will be centered.
// There's no allocation, thus the returned image will NOT OWN ITS PIXELS
// and will share them with the original.
#define ImageCrop(...) \
   OKLAB_UTIL_OVERLOAD(ImageCrop, __VA_ARGS__)
   Image ImageCrop_4_(Image img, int x, int y, int width, int height) {
      assert(img.data != NULL);
      UtilClampCoordinate(&x, img.width);
      UtilClampCoordinate(&y, img.height);
      UtilClampCoordinate(&width,  img.width  - x);
      UtilClampCoordinate(&height, img.height - y);
      img.data += x + y * img.pitch;
      img.width  = width;
      img.height = height;
      return img;
   }
   Image ImageCrop_2_(Image img, int width, int height) {
      assert(img.data != NULL);
      UtilClampCoordinate(&width,  img.width);
      UtilClampCoordinate(&height, img.height);
      int x = (img.width  - width ) / 2;
      int y = (img.height - height) / 2;
      img.data += x + y * img.pitch;
      img.width  = width;
      img.height = height;
      return img;
   }

// Initialize an image from a stream with a PPM file (PPM P6, max 255 colors).
// Exit the program if failure occurs.
// Format: see https://en.wikipedia.org/wiki/Netpbm#File_formats
Image ImageInitFromPPMStream(FILE *stream)
{  assert(stream != NULL);

   // P6
   // # some possible
   // # comment lines
   // width height
   // 255
   // bytes.....

   const char *error_str = "Not the expected format";
   errno = 0;

   // Read and validate the magic number (P6)
   char magic[3];
   if (fscanf(stream, "%2s\n", magic) != 1 || strcmp(magic, "P6") != 0) {
      error_str = "Not a valid PPM P6";
      goto error;
   }

   // Skip any comment lines
   char buf[256]; // max line length
   do {
      char *str = fgets(buf, UTIL_ARRAY_LENGTH(buf), stream);
      if (str == NULL) goto error;
   } while (buf[0] == '#');

   // Read width, height
   int w, h;
   if (sscanf(buf, "%d %d\n", &w, &h) != 2) {
      error_str = "Failed to read image dimensions";
      goto error;
   }

   // Read the maximum color value (we'll only support 255)
   int max;
   if (fscanf(stream, "%d\n", &max) != 1 || max != 255) {
      error_str = "Invalid max color value (Only 255 is supported)";
      goto error;
   }

   // Read the pixel data (usually in PPM P6 we'd fread the whole thing, but
   // here read one at a time, so we can convert each one to a Lab color)
   Image img = ImageInitEmpty(w, h);
   error_str = "Not the expected amount of pixel data";
   for (int i = 0; i < w * h; i++) {
      int r = fgetc(stream);  if (r == EOF) goto error;
      int g = fgetc(stream);  if (g == EOF) goto error;
      int b = fgetc(stream);  if (b == EOF) goto error;
      img.data[i] = LabFromSRGB(r, g, b);
   }
   return img;

error:
   if (errno) {
      perror("Error while reading the image stream");
   } else {
      fprintf(stderr, "%s.\n", error_str);
   }
   ImageDrop(img);
   exit(EXIT_FAILURE);
}

// Construct an Image by loading it from a PPM (P6) file.
// If filename is "-", then load it from standard input instead.
// In case of failure, exit the program.
Image ImageInitFromPPMFile(const char *filename)
{
   if (strcmp(filename, "-") == 0) {
      return ImageInitFromPPMStream(stdin);
   }

   errno = 0;
   FILE *stream = fopen(filename, "r");
   if (stream == NULL)  goto error;
   Image img = ImageInitFromPPMStream(stream);

   errno = 0;
   if (fclose(stream) != 0)  goto error;
   return img;

error:
   perror(filename);
   exit(EXIT_FAILURE);
}

#ifndef KULE_OPTION_USE_STB_IMAGE
#  define ImageInitFromFile(filename)  ImageInitFromPPMFile((filename))
#else
Image ImageInitFromFile(const char *filename) {
   int width, height, n;
   unsigned char *data = stbi_load(filename, &width, &height, &n, 3);

   Image img = ImageInitEmpty(width, height);
   for (int i = 0; i < img.width * img.height; i++) {
      img.data[i] = LabFromSRGB(data[i], data[i+1], data[i+2]);
   }

   stbi_image_free(data);
   return img;
}
#endif

// Save the Image as a PPM P6 (Gamut our image is clipped to sRGB)
// Returns 0 for success or -1 in case of I/O failure.
int ImageSaveAsPPM(Image img, FILE *stream)
{  assert(stream != NULL);
   assert(img.data != NULL);
   errno = 0;

   // Print the ASCII start of the PPM P6
   fprintf(stream, "P6\n%d %d\n255\n", img.width, img.height);
   if (errno)  goto error;

   // Print the pixel data (clip the LabColor)
   for (int i = 0; i < img.width * img.height; i++) {
      RGB color = oklab_to_linear_srgb(img.data[i]);
      color = gamut_clip_adaptive_L0_0_5(color);
      float r = roundf(255.f * srgb_transfer_function(color.r));
      float g = roundf(255.f * srgb_transfer_function(color.g));
      float b = roundf(255.f * srgb_transfer_function(color.b));
      putc(r, stream); if (errno)  goto error;
      putc(g, stream); if (errno)  goto error;
      putc(b, stream); if (errno)  goto error;
   }
   return 0;

error:
   perror("Error while writing the image stream");
   return -1;
}

// Will try to print a (small) image on a (truecolor UTF-8 based) TTY, by using
// colored upper half block characters (U+2580).
// Only accepts up to 120 pixel wide.
int ImagePrintOnTTY(Image img)
{
#  define LINE_MAX_WIDTH 120

   if (img.width > LINE_MAX_WIDTH) {
      // TODO (could be resized)
      printf("Image is %d x %d (too wide)\n", img.width, img.width);
      return -1;
   }

   for (int y = 0; y < img.height - (img.height % 2); y += 2) {
      for (int x = 0; x < img.width; x++) {
         RGB color;

         color = oklab_to_linear_srgb(ImagePixel(img, x, y));
         color = gamut_clip_adaptive_L0_0_5(color);
         int top_r = roundf(255.f * srgb_transfer_function(color.r));
         int top_g = roundf(255.f * srgb_transfer_function(color.g));
         int top_b = roundf(255.f * srgb_transfer_function(color.b));

         color = oklab_to_linear_srgb(ImagePixel(img, x, y + 1));
         color = gamut_clip_adaptive_L0_0_5(color);
         int bottom_r = roundf(255.f * srgb_transfer_function(color.r));
         int bottom_g = roundf(255.f * srgb_transfer_function(color.g));
         int bottom_b = roundf(255.f * srgb_transfer_function(color.b));

         printf ("\x1B[38;2;%d;%d;%d;48;2;%d;%d;%dm\xE2\x96\x80",
            top_r, top_g, top_b, bottom_r, bottom_g, bottom_b);
      }
      puts("\x1B[0m");
   }

   // Write the last line separately if the number of lines is odd
   if (img.height % 2) {
      for (int x = 0; x < img.width; x++) {
         RGB color = oklab_to_linear_srgb(ImagePixel(img, x, img.height - 1));
         color = gamut_clip_adaptive_L0_0_5(color);
         int top_r = roundf(255.f * srgb_transfer_function(color.r));
         int top_g = roundf(255.f * srgb_transfer_function(color.g));
         int top_b = roundf(255.f * srgb_transfer_function(color.b));
         printf ("\x1B[38;2;%d;%d;%dm\xE2\x96\x80", top_r, top_g, top_b);
      }
      puts("\x1B[0m");
   }
   return 0;
}


//------------------------------------------------------------------------------
// "Classic" Error-Diffusion Dithering Matrices
//------------------------------------------------------------------------------

#define IMAGE_DITHER_NONE                               ((float[3][5]){0})

#define IMAGE_DITHER_FLOYD_STEINBERG                                           \
   ((float[3][5]){                                                             \
      { 0, 0 /16.f, 0 /16.f, 7 /16.f, 0 },              /* |    x 7  |      */ \
      { 0, 3 /16.f, 5 /16.f, 1 /16.f, 0 },              /* |  1 5 3  | / 16 */ \
      { 0,    0   ,    0   ,    0   , 0 },              /* |         |      */ \
   })

#define IMAGE_DITHER_JARVIS_JUDICE_NINKE                                       \
   ((float[3][5]){                                                             \
      { 0      , 0      , 0      , 7 /46.f, 5 /46.f },  /* |    x 7 5|      */ \
      { 3 /46.f, 5 /46.f, 7 /46.f, 5 /46.f, 3 /46.f },  /* |3 5 7 5 3| / 46 */ \
      { 0      , 3 /46.f, 5 /46.f, 3 /46.f, 0       },  /* |  3 5 3  |      */ \
   })

#define IMAGE_DITHER_STUCKI                                                    \
   ((float[3][5]){                                                             \
      { 0      , 0      , 0      , 8 /42.f, 4 /42.f },  /* |    x 8 4|      */ \
      { 2 /42.f, 4 /42.f, 8 /42.f, 4 /42.f, 2 /42.f },  /* |2 4 8 4 2| / 42 */ \
      { 1 /42.f, 2 /42.f, 4 /42.f, 2 /42.f, 1 /42.f },  /* |1 2 4 2 1|      */ \
   })

#define IMAGE_DITHER_BURKES                                                    \
   ((float[3][5]){                                                             \
      { 0      , 0      , 0      , 8 /32.f, 4 /32.f },  /* |    x 8 4|      */ \
      { 2 /32.f, 4 /32.f, 8 /32.f, 4 /32.f, 2 /32.f },  /* |2 4 8 4 2| / 32 */ \
      { 0      , 0      , 0      , 0      , 0       },  /* |         |      */ \
   })

#define IMAGE_DITHER_ATKINSON                                                  \
   ((float[3][5]){          /* note: only propagate a fraction of the error */ \
      { 0,   0   ,   0   , 1 /8.f, 1 /8.f },            /* |    x 1 1|      */ \
      { 0, 1 /8.f, 1 /8.f, 1 /8.f, 0      },            /* |  1 1 1  | / 8  */ \
      { 0,   0   , 1 /8.f,      0, 0      },            /* |    1    |      */ \
   })

#define IMAGE_DITHER_SIERRA_THREE_ROWS                                         \
   ((float[3][5]){                                                             \
      { 0      , 0      , 0      , 5 /32.f, 3 /32.f },  /* |    x 5 3|      */ \
      { 2 /32.f, 4 /32.f, 5 /32.f, 4 /32.f, 2 /32.f },  /* |2 4 5 4 2| / 32 */ \
      { 0     ,  2 /32.f, 3 /32.f, 2 /32.f, 0       },  /* |  2 3 2  |      */ \
   })

#define IMAGE_DITHER_SIERRA_TWO_ROWS                                           \
   ((float[3][5]){                                                             \
      { 0      , 0      , 0      , 4 /16.f, 3 /16.f },  /* |    x 4 3|      */ \
      { 1 /16.f, 2 /16.f, 3 /16.f, 2 /16.f, 1 /16.f },  /* |1 2 3 2 1| / 16 */ \
      { 0      , 0      , 0      , 0      , 0       },  /* |         |      */ \
   })

#define IMAGE_DITHER_SIERRA_LITE                                               \
   ((float[3][5]){                                                             \
      { 0,   0   ,   0   , 2 /4.f, 0 },                 /* |    x 2  |      */ \
      { 0, 1 /4.f, 1 /4.f, 0     , 0 },                 /* |  1 1    | / 4  */ \
      { 0,   0   ,   0   , 0     , 0 },                 /* |         |      */ \
   })


#define IMAGE_DITHER_DIFFUSE_ERROR_(X)                    \
   ImagePixel(img, x  , y  ).X = new_pixel.X;             \
   ImagePixel(img, x+1, y  ).X += error.X * matrix[0][3]; \
   ImagePixel(img, x+2, y  ).X += error.X * matrix[0][4]; \
   ImagePixel(img, x-2, y+1).X += error.X * matrix[1][0]; \
   ImagePixel(img, x-1, y+1).X += error.X * matrix[1][1]; \
   ImagePixel(img, x  , y+1).X += error.X * matrix[1][2]; \
   ImagePixel(img, x+1, y+1).X += error.X * matrix[1][3]; \
   ImagePixel(img, x+2, y+1).X += error.X * matrix[1][4]; \
   ImagePixel(img, x-2, y+2).X += error.X * matrix[2][0]; \
   ImagePixel(img, x-1, y+2).X += error.X * matrix[2][2]; \
   ImagePixel(img, x  , y+2).X += error.X * matrix[2][2]; \
   ImagePixel(img, x+1, y+2).X += error.X * matrix[2][3]; \
   ImagePixel(img, x+2, y+2).X += error.X * matrix[2][4]

// Dither the image in-place. Colors used are given by the palette.
// The dither method is error-diffusion (which is betetr for statici mages).
// The `error_diffusion` array specifies how the error is diffused (if NULL, a
// default one is chosen). `strength` is a coefficient between 0 and 1 by which
// the matrix is multiplied, thus providing the "strength" of the dithering.
void ImageDither( Image img, Palette pal,
                  float strength, float error_diffusion[3][5])
{
   // Check Parameters / Defaults
   assert(img.data  != NULL);
   assert(pal.color != NULL);
   if (error_diffusion == NULL) {
      error_diffusion = IMAGE_DITHER_JARVIS_JUDICE_NINKE;
   }
   if (strength < 0.f || strength > 0.999f) {
      strength = 0.999f;
   }

   // Construct error diffusion matrix
   float matrix[3][5];
   for (int y = 0; y < 3; y++)
   for (int x = 0; x < 5; x++) {
      matrix[y][x] = strength * error_diffusion[y][x];
   }

   // Ordered Dithering
   for (int y = 0; y < img.height; y++) {
      for (int x = 0; x < img.width; x++) {

         // Compute the new pixel and the error
         Lab old_pixel = ImagePixel(img, x, y);
         Lab new_pixel = pal.color[ PaletteClosest(pal, old_pixel, NULL) ];
         Lab error = {  old_pixel.L - new_pixel.L,
                        old_pixel.a - new_pixel.a,
                        old_pixel.b - new_pixel.b  };

         // Distribute the error to neighboring pixels
         IMAGE_DITHER_DIFFUSE_ERROR_(L);
         IMAGE_DITHER_DIFFUSE_ERROR_(a);
         IMAGE_DITHER_DIFFUSE_ERROR_(b);
      }
   }
}
#undef IMAGE_DITHER_DIFFUSE_ERROR_

//------------------------------------------------------------------------------
// Functions using the k-Means clustering
//------------------------------------------------------------------------------
#ifdef KULE_OPTION_USE_KMEANS

// K-Means distance method = OkLab color distance
double KuleKM_LabDistance_(const Pointer color1, const Pointer color2)
{ return LabDistance(*(Lab*)color1, *(Lab*)color2); }

// K-Means method for assigning clusters' centroids =
// The cluster center becomes the average of all OkLab colors in the cluster.
//
// Reminder about the parameters passed by k-Means:
// objs       the list of all objects in the calculation
// clusters   the list of cluster numbers for each object
// num_objs   the number of objects/cluster numbers in the previous arrays
// cluster    the cluster number we are actually generating a centroid for here
// centroid   the object to write the centroid result into (already allocated)
void KuleKM_AverageLabCentroids_(const Pointer *objs, const int* clusters,
                                 size_t num_objs, int cluster, Pointer centroid)
{
   if (num_objs <= 0) return;

   Lab * const *color = (Lab * const *)objs;
   Lab *center = (Lab*)centroid;

   int n = 0;
   double sum_L = 0.;
   double sum_a = 0.;
   double sum_b = 0.;

   for (size_t i = 0; i < num_objs; i++) {
      // Only process objects of interest
      if (clusters[i] != cluster)
         continue;

      sum_L += color[i]->L;
      sum_a += color[i]->a;
      sum_b += color[i]->b;
      n++;
   }
   if (n != 0) {
      center->L = (float)(sum_L / n);
      center->a = (float)(sum_a / n);
      center->b = (float)(sum_b / n);
   }
}

// Make a palette taylored for an Image.
// The palette must already be initalized and will be assigned new colors.
//
// Colors are chosen by using k-Means to partition all pixels of a given
// image into clusters based on the OkLab distance.
//
// Return value is as per the k-Means library, ie:
// - KMEANS_OK (= 0)                 -> Success
// - KMEANS_EXCEEDED_MAX_ITERATIONS  -> Too many iterations, hasn't converged
// - KMEANS_ERROR                    -> Another error in k-Means
int PaletteFromImage(Palette *pal, Image img)
{
   assert(pal != NULL);
   assert(pal->color != NULL);

   // The k-Means km object
   kmeans_config km = {
      .distance_method = KuleKM_LabDistance_,
      .centroid_method = KuleKM_AverageLabCentroids_,
      .num_objs        = img.width * img.height,
      .k               = pal->size,
   };

   // k-Means samples = pixels of the image
   size_t i = 0;
   km.objs = CallocOrExit(km.num_objs, sizeof(Pointer));
   for (int y = 0; y < img.height; y++)
   for (int x = 0; x < img.width ; x++) {
      km.objs[i++] = &ImagePixel(img, x,y);
   }
   assert(i == km.num_objs);

   // k-Means cluster centroids = OkLab colors in our palette
   // Initially, we must populate them (equidistantly)
   km.centers  = CallocOrExit(km.k, sizeof(Pointer));
   km.clusters = CallocOrExit(km.num_objs, sizeof(int));
   for (size_t i = 0; i < km.k; i++) {
      pal->color[i] = *(Lab*)(km.objs[i * (km.num_objs / km.k)]);
      km.centers[i] = &(pal->color[i]);
   }

   // Run k-Means partitioning
   kmeans_result returnValue = kmeans(&km);

   // Cleanup and exit
   free(km.objs);
   free(km.clusters);
   free(km.centers);
   return returnValue;
}
#endif //KULE_OPTION_USE_KMEANS


#endif //TOOLS_KULE_H
