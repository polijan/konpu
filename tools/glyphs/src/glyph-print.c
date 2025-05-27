#include "glyph_tools.h"

C_ATTRIBUTE_NORETURN
void Usage(const char* argv0, bool fail)
{
   const char *prog = UtilBasename(argv0);
   if (fprintf(stderr,
      "Usage: %s [FORMAT]... [GLYPH]\n"
      "\n"
      "Print a Glyph\n"
      "\n"
      "OPTIONS\n"
      "   -h, --help    print usage and exits\n"
      "   -r            before printing the glyph, rotate it 180 degrees."
      "FORMAT OPTIONS"
      "   -o, --octant  print glyph in pseudographics using Unicode octants\n"
      "   --hex         print glyph as hexadecimal as in Konpu C\n"
      "   -c            print glyph Konpu C literal, ie GLYPHn(hex).\n"
      "   -k, --kbitx   print glyph's d=\"...\" description in bits'n'picas .kbitx format\n"
      "   --cpc         print comma-separated glyph lines in decimal (MSB order)\n"
      "   --bfm2        print comma-separated glyph lines in decimal (LSB order)\n"
      "   Default format (if none is given) is: -oc"
      "\n\n", prog) <= 0) fail = true;

   exit(fail ? EXIT_FAILURE : EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
   bool outOctants  = false;
   bool outC        = false;
   bool outHex      = false;
   bool outCPC      = false;
   bool outBFM2     = false;
   bool outKbitx    = false;

   bool glyphRotate = false;

   int flag;
   Options opt = OptionsInit();
   const char *optstr = "hockr";
   OptionsLong longopts[] = { {"help",   OPTIONS_NO_ARGUMENT, 'h'},
                              {"hex",    OPTIONS_NO_ARGUMENT, 'H'},
                              {"octant", OPTIONS_NO_ARGUMENT, 'o'},
                              {"kbitx",  OPTIONS_NO_ARGUMENT, 'k'},
                              {"cpc",    OPTIONS_NO_ARGUMENT, 'A'},
                              {"bfm2",   OPTIONS_NO_ARGUMENT, 'B'},
                              {0} };
   while ((flag = OptionsGet(&opt, argc, argv, optstr, longopts)) != -1) {
      switch(flag) {
         default : Usage(argv[0], true);
         case 'h': Usage(argv[0], argc != 2);
         case 'r': glyphRotate = true; break;
         case 'o': outOctants  = true;  break;
         case 'c': outC        = true;  break;
         case 'H': outHex      = true;  break;
         case 'k': outKbitx    = true;  break;
         case 'A': outCPC      = true;  break;
         case 'B': outBFM2     = true;  break;
      }
   }

   if (argc - opt.ind != 1) Usage(argv[0], true);
   Symbol s = SymbolInit(argv[opt.ind]);
   if (s.size <= 0) {
      fprintf(stderr, "Invalid Glyph format.\n");
      return EXIT_FAILURE;
   }

   // Transform glyph
   if (glyphRotate)  s = SymbolRotate180(s);

   // Print glyph
   if (!(outOctants || outHex || outC || outCPC || outBFM2 || outKbitx))
      outOctants = outC = true;
   if (outOctants) { SymbolPrintOctants(s); putchar('\n'); }
   if (outHex)     { SymbolPrintBits(s);    putchar('\n'); }
   if (outC)  { printf("GLYPH%d(", 8 * s.size); SymbolPrintBits(s); puts(")"); }
   if (outCPC)   { SymbolPrintDecimal(s, true);  putchar('\n'); }
   if (outBFM2)  { SymbolPrintDecimal(s, false); putchar('\n'); }
   if (outKbitx) { SymbolPrintKbitxBitmap(s);    putchar('\n'); }
   return EXIT_SUCCESS;
}
