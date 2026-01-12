// For now, let's implemented stuff that doesn't have a FILE*,
// ie, it operates on active window.
//
// unformatted i/o:
// int  c_getchar(void);
// int  c_putchar(int c);
// int  c_puts(const char *s);
// // formatted i/o:
// int  c_printf(const char *restrict format, ...) C_HINT_PRINTF(1,2);
// int  c_vprintf(const char *restrict format, va_list arg);
// int  c_scanf(const char *restrict format, ...)  C_HINT_SCANF(1,2);



#include "io.h"
#include "draw.h"

int c_putchar(int c)
{
   // TODO. For now, a temporary dummy implementation where we accept printable
   //       ASCII character and display it with pixels with ASCII6 font in
   //       the top left corner with color 1.
   if (c < 32 || c > 126) return 0;
   Glyph32 g = *((const uint32_t*)Rom.font.ascii6 + c - 32);
                 //     ^-- this assume litte endian.
   DrawGlyph(g ,0,0, 1);
   return c;
}

int c_puts(const char* s) {
   // TODO. For now, let's put a temporary dummy/naive implementation where we
   // loop to c_putchar for each character of the string
   assert(s);
   for (; *s != '\0'; s++)
      c_putchar(*s);
   return 0;
}

/*
int c_printf(const char* restrict format, ...) {
   // TODO. For now, only puts(the format string).
   assert(format);
   return c_puts(format);
}
*/
