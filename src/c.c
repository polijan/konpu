#include "c.h"


#include "draw.h"
int c_putchar_(int c) {
   // TODO. For now, let's put a Temporary dummy implementation where we accept
   // printable ASCII character and display it with ASCII6 font in the top left
   // corner with color 1.
   if (c < 32 || c > 126) return 0;
   Glyph32 g = *((const uint32_t*)(ROM + ROM_FONT_ASCII6) + c - 32);
   DrawGlyph(g ,0,0, 1);
   return c;
}

int c_puts_(const char* s) {
   // TODO. For now, let's put a temporary dummy/naive implementation where we
   // loop to c_putchar for each character of the string
   assert(s);
   for (; *s != '\0'; s++)
      c_putchar_(*s);
   return 0;
}

int c_printf_(const char* restrict format, ...) {
   // TODO. For now, only puts(the format string).
   assert(format);
   return c_puts_(format);
}


/*

// For sure, we'd want those:
int putchar(int c);
int puts(const char* s);
int printf(const char* restrict format, ...);
   // ^-- with extra modifiers in format (for vairables, etc.)
   //     and maybe a few omissions too!


// Perhaps / Maybe Later the following function from C's <stdio.h>
// And more??? See: https://en.cppreference.com/w/c/header/stdio.html

int sprintf(char* restrict s, const char* restrict format, ...);
int snprintf(char* restrict s, size_t n, const char* restrict format, ...);
int scanf(const char* restrict format, ...);
int sscanf(const char* restrict s, const char* restrict format, ...);
int getchar(void);
int vprintf(const char* restrict format, va_list arg);
int vscanf(const char* restrict format, va_list arg);
int remove(const char* filename);
int rename(const char* old, const char* new);
void perror(const char* s); // ????
// an ungetc-like function: int ungetchar(int c);

*/