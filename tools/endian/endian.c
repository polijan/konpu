// A tool used to define a macro with the endianness from Konu's Makefile.

// Prints| Endianness
// ------+------------------------------------
//  4321 | Little endian
//  1234 | Big endian
//  3412 | Middle endian (Honeywell 316 style)
//  2143 | Middle endian (PDP-11 style)
//  *    | Something else
//

#include <stdio.h>
#include <inttypes.h>

int main(void)
{
   printf("-DKONPU_ENDIAN_ ");

   // A union so that we can cast the byte sequence
   // 1 2 3 4 from memory into a uint32_t
   union {
      uint8_t   memory[4];
      uint32_t  word;
   } endian = { .memory = { 01, 02, 03, 04 } };

   // Print `endian` as a uint32, skip the '0' characters.
   char str[10];
   sprintf(str, "%08" PRIx32 "\n", endian.word);
   for (char *c = str; *c != '\0'; c++) {
      if (*c != '0') putchar(*c);
   }
}
