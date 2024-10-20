// Tool to find possible framebuffer resolutions for ilo Konpu


//==============================================================================
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#if !defined(__STDC_VERSION__) || (__STDC_VERSION__ < 199901L)
#   error "Requires C99 or above"
#elif __STDC_VERSION__ <= 201710L
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

#ifdef _WIN32
#  define FILE_SEPARATOR  '\\'
#else
#  define FILE_SEPARATOR  '/'
#endif
//==============================================================================


// Konpu's framebuffer size in byte is a multiple of this:
#define FRAMEBUFFER_LCM  2880

// the Multiple of FRAMEBUFFER_LCM shall not exceed this:
#define FRAMEBUFFER_FACTOR_MAX  355
        // Notes:
        // - Values over  22 yield a framebuffer over  64Kb
        // - Values over 182 yield a framebuffer over 512Kb
        // - Values over 355 yield a framebuffer over   1Mb


// Print the program usage and exits
C_ATTRIBUTE_NORETURN
static void Usage(const char *argv0)
{
   // Find argv0's basename (ie. strip directory name)
   const char *basename = argv0;
   while (*argv0 != '\0') {
      if (*argv0 == FILE_SEPARATOR)  basename = argv0;
      argv0++;
   }

   // Print Usage
   fprintf(stderr, "Usage: %s N\n", basename);
   fprintf(stderr, "       Find and print possible resolutions for ilo Konpu's framebuffer.\n");
   fprintf(stderr, "       Resolutions are searched for a framebuffer of size N x %d bytes.\n", FRAMEBUFFER_LCM);
   fprintf(stderr, "       Aspect ratio for each resolution is kept between 1 and 2.\n");
   fprintf(stderr, "       N should be in 1 - %d.\n", FRAMEBUFFER_FACTOR_MAX);

   exit(EXIT_FAILURE);
}

// Find and print resolutions (with an aspect ratio between 1 and 2)
// for the given number of 8x8 areas in the framebuffer.
static void ResFinder(int size)
{
   for(int i = 1; i < size; i++) {
      // Find two integers x and y, so that size = x*y
      // (This might not be the most efficient way, but never mind)
      int x, y;
      if (size % i) continue; else x = size / i;
      if (size % x) continue; else y = size / x;

      // Ensure Aspect ratio is between 1 and 2
      double ratio = (double)x / y;
      if ((ratio < 1.0) || (ratio > 2.0)) continue;

      printf("resolution: %3dx%3d (-> glyph64: %2dx%2d) / ratio: %g\n",
             8*x,8*y, x,y, ratio);
   }
}


// Pretty print a message + Call ResFinder() with proper arguments
static void Show(int factor, const char *message, int size_8x8)
{
   int size_fb = factor * FRAMEBUFFER_LCM;
   if (size_fb % size_8x8 != 0)
      printf("\x1b[41m!\x1b[0m ");
   printf("\x1b[33m%2d (%s)\x1b[0m\n", size_8x8, message);
   ResFinder(size_fb / size_8x8);
}


int main(int argc, char* argv[])
{
   if (argc != 2)  Usage(argv[0]);

   int n = atoi(argv[1]);
   if (n < 1 || n > FRAMEBUFFER_FACTOR_MAX)  Usage(argv[0]);

   printf("Framebuffer Memory = %d bytes (= %g Kb)\n",
          n * FRAMEBUFFER_LCM, (n * FRAMEBUFFER_LCM) / 1024.0);

   Show(n, "Glyph mono | Bitmap mono | Planar x1", 8);
   Show(n, "Glyph + 1 byte", 9);
   Show(n, "Glyph + 2 bytes", 10);
   Show(n, "Glyph + 4 bytes", 12);
   Show(n, "Glyph + 8 bytes | Bitmap 4 colors | Planar x2", 16);
   Show(n, "Glyph + 16 bytes | Planar x3", 24);
   Show(n, "Bitmap 16 colors | Planar x4", 32);
   Show(n, "glyph + 32 bytes | Planar x5", 40);
   Show(n, "Planar x6", 48);
   Show(n, "Planar x7", 56);
   Show(n, "Bitmap 256 colors | Planar x8", 64);

   return EXIT_SUCCESS;
}
