#include "c.h"

int konpu_putchar_(int c) {
   // TODO
   (void)c;
   return c;
}

int konpu_puts_(const char* s) {
   // TODO
   (void)s;
   return 0;
}

int konpu_printf_(const char* restrict format, ...) {
   // TODO
   (void)format;
   return 0;
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