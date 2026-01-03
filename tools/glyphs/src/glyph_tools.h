//------------------------------------------------------------------------------
// Misc. Utilities
//------------------------------------------------------------------------------

#include "core84/glyph.h"            // Glyph   from Konpu's core
#include "backend-common/options.c"  // Options from Konpu's backend
// C includes:
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define COUNTOF(array)      (sizeof(array) / sizeof(0[array]))

// [[noreturn]]
#if __STDC_VERSION__ >= 202311L
#   define C_ATTRIBUTE_NORETURN       [[noreturn]]
#elif __STDC_VERSION__ >= 201112L
#   define C_ATTRIBUTE_NORETURN       _Noreturn
#elif defined(__GNUC__)
#   define C_ATTRIBUTE_NORETURN       __attribute__((noreturn))
#else
#   define C_ATTRIBUTE_NORETURN
#endif

// Given a path, return it's basename
const char *UtilBasename(const char *path)
{
#ifdef _WIN32
#  define FILE_SEPARATOR  '\\'
#else
#  define FILE_SEPARATOR  '/'
#endif
   const char *last = path;
   while (*path != '\0') {
      if (*path == FILE_SEPARATOR)  last = path;
      path++;
   }
   if (*last == FILE_SEPARATOR) last++;
   return last;
}


// Return true iff string starts with the prefix
static inline bool
CStringStartsWith(const char *restrict string, const char *restrict prefix)
{
   while(*prefix != '\0') {
      if (*prefix++ != *string++) return false;
   }
   return true;
}

//------------------------------------------------------------------------------
// Unicode Utilities
//------------------------------------------------------------------------------

// A table containing the codepoints of all octants     1   2
// (ie 2x4 pseudo-pixels graphics characters).          4   8
// The index is encoded using those values     -->     16  32
// for the lit pixels                                  64  128
static const uint32_t UnicodeOctants[256] = {
   0x00020, 0x1CEA8, 0x1CEAB, 0x1FB82, 0x1CD00, 0x02598, 0x1CD01, 0x1CD02,
   0x1CD03, 0x1CD04, 0x0259D, 0x1CD05, 0x1CD06, 0x1CD07, 0x1CD08, 0x02580,
   0x1CD09, 0x1CD0A, 0x1CD0B, 0x1CD0C, 0x1FBE6, 0x1CD0D, 0x1CD0E, 0x1CD0F,
   0x1CD10, 0x1CD11, 0x1CD12, 0x1CD13, 0x1CD14, 0x1CD15, 0x1CD16, 0x1CD17,
   0x1CD18, 0x1CD19, 0x1CD1A, 0x1CD1B, 0x1CD1C, 0x1CD1D, 0x1CD1E, 0x1CD1F,
   0x1FBE7, 0x1CD20, 0x1CD21, 0x1CD22, 0x1CD23, 0x1CD24, 0x1CD25, 0x1CD26,
   0x1CD27, 0x1CD28, 0x1CD29, 0x1CD2A, 0x1CD2B, 0x1CD2C, 0x1CD2D, 0x1CD2E,
   0x1CD2F, 0x1CD30, 0x1CD31, 0x1CD32, 0x1CD33, 0x1CD34, 0x1CD35, 0x1FB85,
   0x1CEA3, 0x1CD36, 0x1CD37, 0x1CD38, 0x1CD39, 0x1CD3A, 0x1CD3B, 0x1CD3C,
   0x1CD3D, 0x1CD3E, 0x1CD3F, 0x1CD40, 0x1CD41, 0x1CD42, 0x1CD43, 0x1CD44,
   0x02596, 0x1CD45, 0x1CD46, 0x1CD47, 0x1CD48, 0x0258C, 0x1CD49, 0x1CD4A,
   0x1CD4B, 0x1CD4C, 0x0259E, 0x1CD4D, 0x1CD4E, 0x1CD4F, 0x1CD50, 0x0259B,
   0x1CD51, 0x1CD52, 0x1CD53, 0x1CD54, 0x1CD55, 0x1CD56, 0x1CD57, 0x1CD58,
   0x1CD59, 0x1CD5A, 0x1CD5B, 0x1CD5C, 0x1CD5D, 0x1CD5E, 0x1CD5F, 0x1CD60,
   0x1CD61, 0x1CD62, 0x1CD63, 0x1CD64, 0x1CD65, 0x1CD66, 0x1CD67, 0x1CD68,
   0x1CD69, 0x1CD6A, 0x1CD6B, 0x1CD6C, 0x1CD6D, 0x1CD6E, 0x1CD6F, 0x1CD70,
   0x1CEA0, 0x1CD71, 0x1CD72, 0x1CD73, 0x1CD74, 0x1CD75, 0x1CD76, 0x1CD77,
   0x1CD78, 0x1CD79, 0x1CD7A, 0x1CD7B, 0x1CD7C, 0x1CD7D, 0x1CD7E, 0x1CD7F,
   0x1CD80, 0x1CD81, 0x1CD82, 0x1CD83, 0x1CD84, 0x1CD85, 0x1CD86, 0x1CD87,
   0x1CD88, 0x1CD89, 0x1CD8A, 0x1CD8B, 0x1CD8C, 0x1CD8D, 0x1CD8E, 0x1CD8F,
   0x02597, 0x1CD90, 0x1CD91, 0x1CD92, 0x1CD93, 0x0259A, 0x1CD94, 0x1CD95,
   0x1CD96, 0x1CD97, 0x02590, 0x1CD98, 0x1CD99, 0x1CD9A, 0x1CD9B, 0x0259C,
   0x1CD9C, 0x1CD9D, 0x1CD9E, 0x1CD9F, 0x1CDA0, 0x1CDA1, 0x1CDA2, 0x1CDA3,
   0x1CDA4, 0x1CDA5, 0x1CDA6, 0x1CDA7, 0x1CDA8, 0x1CDA9, 0x1CDAA, 0x1CDAB,
   0x02582, 0x1CDAC, 0x1CDAD, 0x1CDAE, 0x1CDAF, 0x1CDB0, 0x1CDB1, 0x1CDB2,
   0x1CDB3, 0x1CDB4, 0x1CDB5, 0x1CDB6, 0x1CDB7, 0x1CDB8, 0x1CDB9, 0x1CDBA,
   0x1CDBB, 0x1CDBC, 0x1CDBD, 0x1CDBE, 0x1CDBF, 0x1CDC0, 0x1CDC1, 0x1CDC2,
   0x1CDC3, 0x1CDC4, 0x1CDC5, 0x1CDC6, 0x1CDC7, 0x1CDC8, 0x1CDC9, 0x1CDCA,
   0x1CDCB, 0x1CDCC, 0x1CDCD, 0x1CDCE, 0x1CDCF, 0x1CDD0, 0x1CDD1, 0x1CDD2,
   0x1CDD3, 0x1CDD4, 0x1CDD5, 0x1CDD6, 0x1CDD7, 0x1CDD8, 0x1CDD9, 0x1CDDA,
   0x02584, 0x1CDDB, 0x1CDDC, 0x1CDDD, 0x1CDDE, 0x02599, 0x1CDDF, 0x1CDE0,
   0x1CDE1, 0x1CDE2, 0x0259F, 0x1CDE3, 0x02586, 0x1CDE4, 0x1CDE5, 0x02588,
};

// Return the codepoint of a given Braille character
// corresponding to the given octant.
uint32_t UnicodeBrailleOctant(unsigned octant)
{
   assert(octant < 256);

   // The 'Braille Patterns' Unicode block      Octant         Braille
   // assigns the codepoint 0x28XX, with XX      1  2           1  8
   // begins with this correspondance -->        4  8   /____\  2  16
   // between octants pixels and braille.       16  32  \    /  4  32
   //                                           64  128        64  128
   int braille =
      (BITS_GET_BIT(octant, 0))      | (BITS_GET_BIT(octant, 1) << 3) |
      (BITS_GET_BIT(octant, 2) << 1) | (BITS_GET_BIT(octant, 3) << 4) |
      (BITS_GET_BIT(octant, 4) << 2) | (BITS_GET_BIT(octant, 5) << 5) |
      (BITS_GET_BIT(octant, 6) << 6) | (BITS_GET_BIT(octant, 7) << 7) ;
   return 0x2800 + braille;
   // Note: UTF-8 representation would be the following three-bytes sequence:
   // 0xE2, 0xA0|(braille>>6), (128|(braille & 63))
}

// Print the given codepoint on stdout as UTF-8.
void UnicodePutChar(uint32_t c)
{
   if (c <= 0x7F) {
      putchar(c);
   } else if (c <= 0x7FF) {
      putchar(0xC0 | ((c >> 6) & 0x1F));
      putchar(0x80 | (c & 0x3F));
   } else if (c <= 0xFFFF) {
      putchar(0xE0 | ((c >> 12) & 0x0F));
      putchar(0x80 | ((c >>  6) & 0x3F));
      putchar(0x80 | (c & 0x3F));
   } else {
      putchar(0xF0 | ((c >> 18) & 0x07));
      putchar(0x80 | ((c >> 12) & 0x3F));
      putchar(0x80 | ((c >>  6) & 0x3F));
      putchar(0x80 | (c & 0x3F));
   }
}

//------------------------------------------------------------------------------
// Extending Function on Glyphs.
//------------------------------------------------------------------------------

#define GLYPH_PRINT_OCTANT(G, x, y)         UnicodePutChar(UnicodeOctants[ \
   GlyphGetPixel(G, x, y    ) << 0 | GlyphGetPixel(G, x + 1, y    ) << 1 | \
   GlyphGetPixel(G, x, y + 1) << 2 | GlyphGetPixel(G, x + 1, y + 1) << 3 | \
   GlyphGetPixel(G, x, y + 2) << 4 | GlyphGetPixel(G, x + 1, y + 2) << 5 | \
   GlyphGetPixel(G, x, y + 3) << 6 | GlyphGetPixel(G, x + 1, y + 3) << 7 ])

#define GLYPH_PRINT_OCTANT_2(G, y)   \
   do { GLYPH_PRINT_OCTANT(G, 0, y); \
        GLYPH_PRINT_OCTANT(G, 2, y); \
   } while(0)

#define GLYPH_PRINT_OCTANT_4(G, y)   \
   do { GLYPH_PRINT_OCTANT(G, 0, y); \
        GLYPH_PRINT_OCTANT(G, 2, y); \
        GLYPH_PRINT_OCTANT(G, 4, y); \
        GLYPH_PRINT_OCTANT(G, 6, y); \
   } while(0)

// Print a Glyph as Octant on stdout.
// '\n' is used to go to next line, but no final '\n' is printed.
#define GlyphPrintOctants(glyph)         \
   _Generic((glyph)                    , \
      uint8_t:  GlyphPrintOctants_8_   , \
      uint16_t: GlyphPrintOctants_16_  , \
      uint32_t: GlyphPrintOctants_32_  , \
      uint64_t: GlyphPrintOctants_64_  , \
      Glyph128: GlyphPrintOctants_128_ , \
      Glyph256: GlyphPrintOctants_256_   \
   )((glyph))
   void GlyphPrintOctants_8_(uint8_t g)
   { UnicodePutChar(UnicodeOctants[g]); }
   void GlyphPrintOctants_16_(uint16_t g)
   { GLYPH_PRINT_OCTANT_2(g, 0); }
   void GlyphPrintOctants_32_(uint32_t g) {
      GLYPH_PRINT_OCTANT_2(g, 0); putchar('\n');
      GLYPH_PRINT_OCTANT_2(g, 4);
   }
   void GlyphPrintOctants_64_(uint64_t g) {
      GLYPH_PRINT_OCTANT_4(g, 0); putchar('\n');
      GLYPH_PRINT_OCTANT_4(g, 4);
   }
   void GlyphPrintOctants_128_(Glyph128 g) {
      GLYPH_PRINT_OCTANT_4(g.top, 0);    putchar('\n');
      GLYPH_PRINT_OCTANT_4(g.top, 4);    putchar('\n');
      GLYPH_PRINT_OCTANT_4(g.bottom, 0); putchar('\n');
      GLYPH_PRINT_OCTANT_4(g.bottom, 4);
   }
   void GlyphPrintOctants_256_(Glyph256 g) {
      GLYPH_PRINT_OCTANT_4(g.top_left, 0);    GLYPH_PRINT_OCTANT_4(g.top_right, 0);    putchar('\n');
      GLYPH_PRINT_OCTANT_4(g.top_left, 4);    GLYPH_PRINT_OCTANT_4(g.top_right, 4);    putchar('\n');
      GLYPH_PRINT_OCTANT_4(g.bottom_left, 0); GLYPH_PRINT_OCTANT_4(g.bottom_right, 0); putchar('\n');
      GLYPH_PRINT_OCTANT_4(g.bottom_left, 4); GLYPH_PRINT_OCTANT_4(g.bottom_right, 4);
   }


//------------------------------------------------------------------------------
// Symbol is a "tagged union" that can contain any Konpu's Glyph.
//
// Operations on it are mostly the same as on Glyphs (boringly implemented as
// switch cases dispatching to the correct glyph function) and a bit more.
//------------------------------------------------------------------------------

typedef struct Symbol {
   union {
      uint8_t   glyph8;
      uint16_t  glyph16;
      uint32_t  glyph32;
      uint64_t  glyph64;
      Glyph128  glyph128;
      Glyph256  glyph256;
   };
   int8_t size; // Size in bytes of the Glyph, thus:
                // 1 = Glyph8  ,  2 = Glyph16  ,  4 = Glyph32
                // 8 = Glyph64 , 16 = Glyph128 , 32 = Glyph256
} Symbol;




Symbol SymbolFlip(Symbol s) {
   switch (s.size) {
      default: s.size = -1; break;
      case   1: s.glyph8   = GlyphFlip(s.glyph8);   break;
      case   2: s.glyph16  = GlyphFlip(s.glyph16);  break;
      case   4: s.glyph32  = GlyphFlip(s.glyph32);  break;
      case   8: s.glyph64  = GlyphFlip(s.glyph64);  break;
      case  16: s.glyph128 = GlyphFlip(s.glyph128); break;
      case  32: s.glyph256 = GlyphFlip(s.glyph256); break;
  }
  return s;
}
Symbol SymbolMirror(Symbol s) {
   switch (s.size) {
      default: s.size = -1; break;
      case   1: s.glyph8   = GlyphMirror(s.glyph8);   break;
      case   2: s.glyph16  = GlyphMirror(s.glyph16);  break;
      case   4: s.glyph32  = GlyphMirror(s.glyph32);  break;
      case   8: s.glyph64  = GlyphMirror(s.glyph64);  break;
      case  16: s.glyph128 = GlyphMirror(s.glyph128); break;
      case  32: s.glyph256 = GlyphMirror(s.glyph256); break;
  }
  return s;
}
Symbol SymbolTranspose(Symbol s) {
   switch (s.size) {
      default: s.size = -1; break;
//    case   1: s.glyph8   = GlyphTranspose(s.glyph8);   break;
      case   2: s.glyph16  = GlyphTranspose(s.glyph16);  break;
//    case   4: s.glyph32  = GlyphTranspose(s.glyph32);  break;
      case   8: s.glyph64  = GlyphTranspose(s.glyph64);  break;
//    case  16: s.glyph128 = GlyphTranspose(s.glyph128); break;
      case  32: s.glyph256 = GlyphTranspose(s.glyph256); break;
  }
  return s;
}
Symbol SymbolRotate90(Symbol s) {
   switch (s.size) {
      default: s.size = -1; break;
//    case   1: s.glyph8   = GlyphRotate90(s.glyph8);   break;
      case   2: s.glyph16  = GlyphRotate90(s.glyph16);  break;
//    case   4: s.glyph32  = GlyphRotate90(s.glyph32);  break;
      case   8: s.glyph64  = GlyphRotate90(s.glyph64);  break;
//    case  16: s.glyph128 = GlyphRotate90(s.glyph128); break;
      case  32: s.glyph256 = GlyphRotate90(s.glyph256); break;
  }
  return s;
}
Symbol SymbolRotate180(Symbol s) {
   switch (s.size) {
      default: s.size = -1; break;
      case   1: s.glyph8   = GlyphRotate180(s.glyph8);   break;
      case   2: s.glyph16  = GlyphRotate180(s.glyph16);  break;
      case   4: s.glyph32  = GlyphRotate180(s.glyph32);  break;
      case   8: s.glyph64  = GlyphRotate180(s.glyph64);  break;
      case  16: s.glyph128 = GlyphRotate180(s.glyph128); break;
      case  32: s.glyph256 = GlyphRotate180(s.glyph256); break;
  }
  return s;
}
Symbol SymbolRotate270(Symbol s) {
   switch (s.size) {
      default: s.size = -1; break;
//    case   1: s.glyph8   = GlyphRotate270(s.glyph8);   break;
      case   2: s.glyph16  = GlyphRotate270(s.glyph16);  break;
//    case   4: s.glyph32  = GlyphRotate270(s.glyph32);  break;
      case   8: s.glyph64  = GlyphRotate270(s.glyph64);  break;
//    case  16: s.glyph128 = GlyphRotate270(s.glyph128); break;
      case  32: s.glyph256 = GlyphRotate270(s.glyph256); break;
  }
  return s;
}








Symbol SymbolInit(const char *string)
{
   assert(string != NULL);
   Symbol s = { .size = -1 };

   size_t len;
   int checksz = 0;
   if (CStringStartsWith(string, "GLYPH")) {
      // If string is Glyph(...), advance the string, adjust len,
      // and make sure we'll check the ... part has right length.
      string += 5;
      if      (CStringStartsWith(string,   "8(")) { string += 2; checksz =  1; }
      else if (CStringStartsWith(string,  "16(")) { string += 3; checksz =  2; }
      else if (CStringStartsWith(string,  "32(")) { string += 3; checksz =  4; }
      else if (CStringStartsWith(string,  "64(")) { string += 3; checksz =  8; }
      else if (CStringStartsWith(string, "128(")) { string += 4; checksz = 16; }
      else if (CStringStartsWith(string, "256(")) { string += 4; checksz = 32; }
      else return s;
      len = strlen(string);
      if (string[len - 1] != ')') return s;
      len --;
   } else {
      len = strlen(string);
   }

   switch (len) {
      // Note: stuff like '45--' would still pass the scanf and be validated
      //       as a GLYPH16... (as 0x45) but it's tight enough for our tools.
      case  2:
         if (sscanf(string, "%02"SCNx8, &s.glyph8)  == 1)    s.size = 1;
         break;
      case  4:
         if (sscanf(string, "%04"SCNx16, &s.glyph16) == 1)   s.size = 2;
         break;
      case  8:
         if (sscanf(string, "%08"SCNx32, &s.glyph32) == 1)   s.size = 4;
         break;
      case 16:
         if (sscanf(string, "%016"SCNx64,&s.glyph64) == 1)   s.size = 8;
         break;
      case 33:
         if (sscanf(string, "%016"SCNx64":%016"SCNx64,
               &s.glyph128.top, &s.glyph128.bottom) == 2)    s.size = 16;
         break;
      case 67:
         if (sscanf(string, "%016"SCNx64":%016"SCNx64":%016"SCNx64":%016"SCNx64,
               &s.glyph256.top_left   , &s.glyph256.top_right,
               &s.glyph256.bottom_left, &s.glyph256.bottom_right) == 4)
            s.size = 32;
         break;
      default:
         fprintf(stderr, "Invalid Glyph format: %s\n", string);
         exit(EXIT_FAILURE);
   }

   // if we knew the size already (in the GLYPHn(...) case), then we checked
   // that we sscanned that
   if (checksz && checksz != s.size)  s.size = -1;

   return s;
}

void SymbolPrintBits(Symbol s)
{
   switch (s.size) {
      default: puts("Not a valid Symbol"); return;
      case  1: printf("%02"PRIX8,   s.glyph8);   break;
      case  2: printf("%04"PRIX16,  s.glyph16);  break;
      case  4: printf("%08"PRIX32,  s.glyph32);  break;
      case  8: printf("%016"PRIX64, s.glyph64);  break;
      case 16: printf("%016"PRIX64":%016"PRIX64, s.glyph128.top, s.glyph128.bottom);
               break;
      case 32: printf("%016"PRIX64":%016"PRIX64":%016"PRIX64":%016"PRIX64,
                  s.glyph256.top_left   , s.glyph256.top_right,
                  s.glyph256.bottom_left, s.glyph256.bottom_right);
               break;
   }
}

void SymbolPrintOctants(Symbol s)
{
   switch (s.size) {
      default: puts("Not a valid Symbol");   return;
      case  1: GlyphPrintOctants(s.glyph8);   break;
      case  2: GlyphPrintOctants(s.glyph16);  break;
      case  4: GlyphPrintOctants(s.glyph32);  break;
      case  8: GlyphPrintOctants(s.glyph64);  break;
      case 16: GlyphPrintOctants(s.glyph128); break;
      case 32: GlyphPrintOctants(s.glyph256); break;
   }
}

void SymbolPrintHalves(Symbol s)
{
   switch (s.size) {
      default: puts("Not a valid Symbol"); return;
      case  1: GlyphPrintOctants(GlyphUpscale(GlyphUpscale(s.glyph8)));   break;
      case  2: GlyphPrintOctants(GlyphUpscale(GlyphUpscale(s.glyph16)));  break;
      case  4: GlyphPrintOctants(GlyphUpscale(GlyphUpscale(s.glyph32)));  break;
      case  8: GlyphPrintOctants(GlyphUpscale(GlyphUpscale(s.glyph64)));  break;
      case 16: GlyphPrintOctants(GlyphUpscale(GlyphUpscale(s.glyph128.top)));
               putchar('\n');
               GlyphPrintOctants(GlyphUpscale(GlyphUpscale(s.glyph128.bottom)));
               break;
      // TODO: Not supported yet!
      case 32: GlyphPrintOctants(s.glyph256); break;
   }
}

void SymbolPrintDecimal(Symbol s, bool mirror)
{
   switch (s.size) {
      default: puts("Not a valid Symbol");   return;
      case  1:
         ;uint8_t g8 = mirror? s.glyph8 : GlyphMirror(s.glyph8);
         printf("%d,%d,%d,%d",
            GlyphGetLine(g8, 0), GlyphGetLine(g8, 1),
            GlyphGetLine(g8, 2), GlyphGetLine(g8, 3));
         break;
      case  2:
         ;uint16_t g16 = mirror ? s.glyph16 : GlyphMirror(s.glyph16);
         printf("%d,%d,%d,%d",
            GlyphGetLine(g16, 0), GlyphGetLine(g16, 1),
            GlyphGetLine(g16, 2), GlyphGetLine(g16, 3));
         break;
      case  4:
         ;uint32_t g32 = mirror ? s.glyph32 : GlyphMirror(s.glyph32);
         printf("%d,%d,%d,%d,%d,%d,%d,%d",
            GlyphGetLine(g32, 0), GlyphGetLine(g32, 1), GlyphGetLine(g32, 2),
            GlyphGetLine(g32, 3), GlyphGetLine(g32, 4), GlyphGetLine(g32, 5),
            GlyphGetLine(g32, 6), GlyphGetLine(g32, 7));
         break;
      case  8:
         ;uint64_t g64 = mirror ? s.glyph64 :GlyphMirror(s.glyph64);
         printf("%d,%d,%d,%d,%d,%d,%d,%d",
            GlyphGetLine(g64, 0), GlyphGetLine(g64, 1), GlyphGetLine(g64, 2),
            GlyphGetLine(g64, 3), GlyphGetLine(g64, 4), GlyphGetLine(g64, 5),
            GlyphGetLine(g64, 6), GlyphGetLine(g64, 7));
         break;
      case 16:
         ;Glyph128 g128 = mirror ? s.glyph128 :GlyphMirror(s.glyph128);
         printf("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
            GlyphGetLine(g128,  0), GlyphGetLine(g128,  1),
            GlyphGetLine(g128,  2), GlyphGetLine(g128,  3),
            GlyphGetLine(g128,  4), GlyphGetLine(g128,  5),
            GlyphGetLine(g128,  6), GlyphGetLine(g128,  7),
            GlyphGetLine(g128,  8), GlyphGetLine(g128,  9),
            GlyphGetLine(g128, 10), GlyphGetLine(g128, 11),
            GlyphGetLine(g128, 12), GlyphGetLine(g128, 13),
            GlyphGetLine(g128, 14), GlyphGetLine(g128, 15));
         break;
      case 32:
         ;Glyph256 g256 = mirror ? s.glyph256 :GlyphMirror(s.glyph256);
         printf("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
            GlyphGetLine(g256,  0), GlyphGetLine(g256,  1),
            GlyphGetLine(g256,  2), GlyphGetLine(g256,  3),
            GlyphGetLine(g256,  4), GlyphGetLine(g256,  5),
            GlyphGetLine(g256,  6), GlyphGetLine(g256,  7),
            GlyphGetLine(g256,  8), GlyphGetLine(g256,  9),
            GlyphGetLine(g256, 10), GlyphGetLine(g256, 11),
            GlyphGetLine(g256, 12), GlyphGetLine(g256, 13),
            GlyphGetLine(g256, 14), GlyphGetLine(g256, 15));
         break;
   }
}

// Print a Bitmap part (the d="..." arguments) that can correspond to the given
// Symbol in a Bits'n'Picas .kbitx files.
// (This prints a rather long string, not optimized from compression)
void SymbolPrintKbitxBitmap(Symbol s)
{
   uint64_t g = 0;   // The glyph is <= 64bits
   uint8_t w, h;     // Width and height of the symbol's glyph
   switch (s.size) {
      case  1: w =  2; h =  4; g = s.glyph8;  break;
      case  2: w =  4; h =  4; g = s.glyph16; break;
      case  4: w =  4; h =  8; g = s.glyph32; break;
      case  8: w =  8; h =  8; g = s.glyph64; break;
      case 16: w =  8; h = 16; break;
      case 32: w = 16; h = 16; break;
      default: printf("Not a valid glyph"); return;
   }

   // Byte buffer to write the symbols' bits into.
   uint8_t buffer[ 2 + 256 ];
   int buffer_sz = 2 + w * h; // <-- we'll write that many bytes into it.

   //---------------------------------------------------------------------------
   // Step 1: First, bitsnpicas expect the glyph's height and width
   //---------------------------------------------------------------------------
   // (bitsnpicas reads the size as ULEB128 integers, but since our size is less
   // than 7 bits, it's the same)
   uint8_t *dst = buffer;
   *dst++ = h;
   *dst++ = w;

   //---------------------------------------------------------------------------
   // Step 2: Write the bitmap in the buffer.
   //---------------------------------------------------------------------------
   // Bits'n'picas reads the glyph bitmap data using some special form of
   // Run-Length Encoding (RLE) which it calls "WIB" that allows encoding large
   // repeat counts compactly (especially of values 00 and FF).
   // https://github.com/kreativekorp/bitsnpicas/blob/master/main/java/BitsNPicas/src/com/kreative/bitsnpicas/WIBInputStream.java
   //
   // I figure out that each data unit starts with a control byte.
   //   Bits 0-5 (0x1F) sets a base repeat count
   //   Bits 6-7 (0xC0) sets one of the following "mode" of operation:
   //      - 00 => repeat the fixed value 0x00
   //      - 01 => repeat the fixed value 0xFF
   //      - 10 => repeat the next byte from stream
   //      - 11 => raw mode: copy next repeatCount bytes as-is
   //
   // We don't care about trying to compress our (fairly small) glyphs, so we'll
   // set repeat count to just 1 and and use the 6th anf 7th bit 00 and 01 to
   // one 0x00 or one 0xFF value for every bit.
   // this gives us: "0" bit pixel is 0x01
   // this gives     "1" bit pixel is 0x41
   //---------------------------------------------------------------------------

   if (s.size <= 8) {
      for (int i = 0; i < w * h; i++) {
         *dst++ = (g & 1)? 0x41 : 0x01;
         g >>= 1;
      }
   } else if (s.size == 16) {
      for (int i = 0; i < 64; i++) {
         *dst++ = (s.glyph128.top & 1)? 0x41 : 0x01;
         s.glyph128.top >>= 1;
      }
      for (int i = 0; i < 64; i++) {
         *dst++ = (s.glyph128.bottom & 1)? 0x41 : 0x01;
         s.glyph128.bottom >>= 1;
      }
   } else {
      for (int n = 0; n < 16; n++) {
         unsigned line = GlyphGetLine(s.glyph256, n);
         for (int i = 0; i < 16; i++) {
            *dst++ = (line & 1)? 0x41 : 0x01;
            line >>= 1;
         }
      }
   }

   //------------------------------------------------------------------
   // Step 3: Prints the buffer encoded as Base64 (no padding =)
   //------------------------------------------------------------------
   const char b64[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                        "abcdefghijklmnopqrstuvwxyz"
                        "0123456789+/";

   // As long as we can read three-bytes, write four-bytes groups
   int i;
   for (i = 0; i + 2 < buffer_sz; i += 3) {
      uint32_t triple = (buffer[i] << 16) | (buffer[i+1] << 8) | buffer[i+2];
      putchar(b64[(triple >> 18) & 0x3F]);
      putchar(b64[(triple >> 12) & 0x3F]);
      putchar(b64[(triple >> 6) & 0x3F]);
      putchar(b64[triple & 0x3F]);
   }

   // Handle the remaining one or two bytes
   if (i < buffer_sz) {
      uint32_t triple = buffer[i] << 16;
      if (i + 1 < buffer_sz)  triple |= buffer[i + 1] << 8;
      putchar(b64[(triple >> 18) & 0x3F]);
      putchar(b64[(triple >> 12) & 0x3F]);
      if (i + 1 < buffer_sz)  putchar(b64[(triple >> 6) & 0x3F]);
   }
}

// Print a line that can characterize the symbol in a .kbitx file
void SymbolPrintKbitxLine(Symbol s, uint32_t codepoint)
{
   int w;
   switch(s.size) {
      case  1: w =  2; break;
      case  2: w =  4; break;
      case  4: w =  4; break;
      case  8: w =  8; break;
      case 16: w =  8; break;
      case 32: w = 16; break;
      default: w =  0; break;
   }
   printf("<g u=\"%"PRIu32"\" x=\"0\" y=\"0\" w=\"%d\" d=\"", codepoint, w);
   SymbolPrintKbitxBitmap(s);
   puts("\"/>");
}
