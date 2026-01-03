#include "var.h"

// we shouldn't use <stdio.h>...
#undef printf
#include <stdio.h>
#include <inttypes.h>

#include "arch.h"
#include "error.h"

void VarPrintTTY(var v, Type t)
{
   switch(t) {
     // Builtin Types
   case TYPE_NULL:
      printf("\x1b[38;5;8mnil"); break;
   case TYPE_INTEGER:
      printf("\x1b[38;5;12m%"PRId64, v.integer); break;
   case TYPE_REAL:
      printf("\x1b[38;5;12m%f", v.real); break;
   case TYPE_BITS:
      for (int i = 0; i < 8; i++) {
         printf("%02x ", v.bytes[i]);
      }
      break;
   case TYPE_CHAR:
      printf("%c", v.chr); break;
   case TYPE_BOOLEAN:
      printf("%s", v.boolean? "lon":"ala"); break;
   case TYPE_ADDRESS:
      printf("&%05"PRIx32, (uint32_t)(v.address));
      if (v.address < 0)  {
         printf(" (underflow)");
      } else if (v.address > C_SIZEOF(Ram)) {
         printf(" (overflow)");
      }
      break;
   case TYPE_HEAP_ADDRESS:
      printf("@%04"PRIx16" (&%05"PRIx32")", v.uint16,
         RAM_ADDRESS_OF(heap.memory) + 8 * v.uint16);
      break;
   case TYPE_POINTER:
      printf("%p", v.pointer); break;
   case TYPE_CSTRING:
      printf("\x1b[38;5;11mc\"%s\"", v.cstring); break;
   case TYPE_ERROR:
      printf("\x1b[48;5;9;38;5;15mpakala #%d\x1b[m %s\n", v.error, ErrorMessage(v.error));
      break;
   case TYPE_TYPE:
      printf("type: %d\n", v.type);
      break;
   case TYPE_GLYPH8:
      for (int i = 0; i < 2; i++) {
         for (int j = 0; j < 4; j++) {
            putchar(v.glyph8 & (1u << (i+j))? '#':'.' );
            // putchar(BitsIsSet(v.glyph8, i + j)? '#':'.'
            putchar(' ');
         }
         putchar('\n');
      }
      break;
   case TYPE_GLYPH16:
      for (int i = 0; i < 4; i++) {
         for (int j = 0; j < 4; j++) {
            putchar(v.glyph16 & (1u << (i+j))? '#':'.' );
            putchar(' ');
         }
         putchar('\n');
      }
      break;
   case TYPE_GLYPH32:
      for (int i = 0; i < 4; i++) {
         for (int j = 0; j < 8; j++) {
            putchar(v.glyph32 & (UINT32_C(1) << (i+j))? '#':'.' );
            putchar(' ');
         }
         putchar('\n');
      }
      break;
   case TYPE_GLYPH64:
      printf("\033[4A\0337\n\n\n\n\n\n\n\n\n\0338");
      for (int i = 0; i < 8; i++) {
         for (int j = 0; j < 8; j++) {
            putchar(v.glyph64 & (UINT64_C(1) << (i+j))? '#':'.' );
            putchar(' ');
         }
         printf("\x1b[16D\x1b[1B");
      }
      break;
   case TYPE_GLYPH128: printf("TODO"); break;
   case TYPE_GLYPH256: printf("TODO"); break;
   default:
      printf("\x1b[35mpoki #%d la\x1b[36m:\x1b[m ", t);
      for (int i = 0; i < 8; i++) {
         printf("%02x ", v.bytes[i]);
      }
      break;
   }
   printf("\x1b[m");
}
