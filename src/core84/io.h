#ifndef  KONPU_IO_H_
#define  KONPU_IO_H_
#include "c.h"

//------------------------------------------------------------------------------
// Hints
//------------------------------------------------------------------------------
// C_HINT_PRINTF and C_HINT_SCANF wraps GCC __attributes__ which assign printf-
// like or scanf-like semantics to function declarations and makes the compiler
// check function invocation.
// M: M-th argument is the "format string" parameter
// N: N-th argument is the start of variadic parameters
#ifdef __GNUC__
#  define C_HINT_PRINTF(M, N)    __attribute__((format(printf, M, N)))
#  define C_HINT_SCANF(M, N)     __attribute__((format(scanf, M, N)))
#else
#  define C_HINT_PRINTF(M, N)
#  define C_HINT_SCANF(M, N)
#endif

//------------------------------------------------------------------------------
// Konpu non-stdio interface
//------------------------------------------------------------------------------

// Konpu Printer port
//#define Printer
#define FileA  // TODO
#define FileB  // TODO


//------------------------------------------------------------------------------
// Mostly-compatible <stdio.h> interface
//------------------------------------------------------------------------------

// File Access

#define  C_FILE_   void
#define  C_IOFBF_  0 // TODO
#define  C_IOLBF_  1 // TODO
#define  C_IONBF_  2 // TODO
#define  C_BUFSIZ_ 512 // TODO
C_FILE_ *c_fopen(const char *restrict filename, const char *restrict mode);
C_FILE_ *c_freopen(const char *restrict filename, const char *restrict mode, C_FILE_ *restrict stream);
int      c_fclose(C_FILE_ *stream);
int      c_fflush(C_FILE_ *stream);
int      c_setvbuf(C_FILE_ *restrict stream, char *restrict buf, int mode, size_t size);
#if defined(KONPU_H_) && !defined(KONPU_OPTION_NO_STDIO)
#  define  fopen(filename, mode)              c_fopen(filename, mode)
#  define  freopen(filename, mode, stream)    c_freopen(filename, mode, stream)
#  define  fclose(stream)                     c_fclose(stream)
#  define  fflush(stream)                     c_fflush(stream)
#  define  setvbuf(stream, buf, mode, size)   c_setvbuf(stream, buf, mode, size)
#  define  setbuf(stream, buf)                c_setbuf(stream, buf)
#  define  FOPEN_MAX     2
#  define  FILENAME_MAX  45
#  define  BUFSIZ        C_BUFSIZ_
#  define _IOFBF         C_IOFBF_
#  define _IOLBF         C_IOLBF_
#  define _IONBF         C_IONBF_
#endif
static inline void c_setbuf(C_FILE_ *restrict stream, char *restrict buf)
{ c_setvbuf(stream, buf, buf ? C_IOFBF_ : C_IONBF_, C_BUFSIZ_); }

// Operations on Files

int c_remove(const char *filename);
int c_rename(const char *old_name, const char *new_name);
#if defined(KONPU_H_) && !defined(KONPU_OPTION_NO_STDIO)
#  define  remove(filename)              c_remove(filename)
#  define  rename(old_name, new_name)    c_rename(old_name, new_name)
// `FILE* tmpfile(void)` isn't really implemented and always return NULL
#  define  tmpfile()   NULL
// `char *tmpnam(char *s)`  isn't really implemented and always return NULL
#  define  tmpnam(s)   NULL
#  define  TMP_MAX     0
#  define  L_tmpnam    0
#endif

// Direct Input/Output

size_t c_fread(void *restrict ptr, size_t size, size_t nmemb, C_FILE_ *restrict stream);
size_t c_fwrite(const void *restrict ptr, size_t size, size_t nmemb, C_FILE_ *restrict stream);
#if defined(KONPU_H_) && !defined(KONPU_OPTION_NO_STDIO)
#  define  fread(ptr, size, nmemb, stream)    c_fread(ptr, size, nmemb, stream)
#  define  fwrite(ptr, size, nmemb, stream)   c_fwrite(ptr, size, nmemb, stream)
#endif

// Unformatted Input/Output

int   c_fgetc(C_FILE_ *stream);
int   c_getc(C_FILE_ *stream);
int   c_getchar(void);
int   c_fputc(int c, C_FILE_ *stream);
int   c_putc(int c, C_FILE_ *stream);
int   c_putchar(int c);
int   c_ungetc(int c, C_FILE_ *stream);
char *c_fgets(char *restrict s, int n, C_FILE_ *restrict stream);
int   c_fputs(const char *restrict s, C_FILE_ *restrict stream);
int   c_puts(const char *s);
#if defined(KONPU_H_) && !defined(KONPU_OPTION_NO_STDIO)
#  define  fgetc(stream)          c_fgetc(stream)
#  define  fgets(s, n, stream)    c_fgets(s, n, stream)
#  define  fputc(c, stream)       c_fputc(c, stream)
#  define  fputs(s, stream)       c_fputs(s, stream)
#  define  getc(stream)           c_getc(stream)
#  define  getchar()              c_getchar()
#  define  putc(c, stream)        c_putc(c, stream)
#  define  putchar(c)             c_putchar(c)
#  define  puts(s)                c_puts(s)
#  define  ungetc(c, stream)      c_ungetc(c, stream)
//#define EOF   TODO
#endif

// Formatted Input/Output

int  c_printf(const char *restrict format, ...) C_HINT_PRINTF(1,2);
int  c_fprintf(C_FILE_ *restrict stream, const char *restrict format, ...) C_HINT_PRINTF(2,3);
int  c_sprintf(char *restrict s, const char *restrict format, ...) C_HINT_PRINTF(2,3);
int  c_snprintf(char *restrict s, size_t n, const char *restrict format, ...) C_HINT_PRINTF(3,4);
int  c_vprintf(const char *restrict format, va_list arg);
int  c_vfprintf(C_FILE_ *restrict stream, const char *restrict format, va_list arg);
int  c_vsprintf(char *restrict s, const char *restrict format, va_list arg);
int  c_vsnprintf(char *restrict s, size_t n, const char *restrict format, va_list arg);
int  c_scanf(const char *restrict format, ...)  C_HINT_SCANF(1,2);
int  c_fscanf(C_FILE_ *restrict stream, const char *restrict format, ...) C_HINT_SCANF(2,3);
int  c_sscanf(const char *restrict s, const char *restrict format, ...) C_HINT_SCANF(2,3);
int  c_vfscanf(C_FILE_ *restrict stream, const char *restrict format, va_list arg);
int  c_vscanf(const char *restrict format, va_list arg);
int  c_vsscanf(const char *restrict s, const char *restrict format, va_list arg);
#if defined(KONPU_H_) && !defined(KONPU_OPTION_NO_STDIO)
#  define  vprintf(format, varg)            c_vprintf(format, varg)
#  define  vfprintf(stream, format, varg)   c_vfprintf(stream, format, varg)
#  define  vsprintf(s, format, varg)        c_vsprintf(s, format, varg)
#  define  vsnprintf(s, n,  format, varg)   c_vsnprintf(s, n,  format, varg)
#  define  vscanf(format, varg)             c_vscanf(format, varg)
#  define  vfscanf(stream, format, varg)    c_vfscanf(stream, format, varg)
#  define  vsscanf(s, format, varg)         c_vsscanf(s, format, varg)
#  if __STDC_VERSION__ >= 202311L
#     define  printf(format, ...)           c_printf(format __VA_OPT__(,) __VA_ARGS__)
#     define  fprintf(stream, format, ...)  c_fprintf(stream, format __VA_OPT__(,) __VA_ARGS__)
#     define  sprintf(s, format, ...)       c_sprintf(s, format __VA_OPT__(,) __VA_ARGS__)
#     define  snprintf(s, n, format, ...)   c_snprintf(s, n, format __VA_OPT__(,) __VA_ARGS__)
#     define  scanf(format, ...)            c_scanf(format __VA_OPT__(,) __VA_ARGS__)
#     define  fscanf(stream, format, ...)   c_fscanf(stream, format __VA_OPT__(,) __VA_ARGS__)
#     define  sscanf(s, format, ...)        c_sscanf(s, format __VA_OPT__(,) __VA_ARGS__)
#  elif defined(__GNUC__)
      // GCC has this non-standard C language extension: inserting ## between ,
      // and __VA_ARGS__ has a same effect as C23's __VA_OPT__(,)
#     define  printf(format, ...)           c_printf(format, ##  __VA_ARGS__)
#     define  fprintf(stream, format, ...)  c_fprintf(stream, format, ##  __VA_ARGS__)
#     define  sprintf(s, format, ...)       c_sprintf(s, format, ##  __VA_ARGS__)
#     define  snprintf(s, n, format, ...)   c_snprintf(s, n, format, ##  __VA_ARGS__)
#     define  scanf(format, ...)            c_scanf(format, ##  __VA_ARGS__)
#     define  fscanf(stream, format, ...)   c_fscanf(stream, format, ##  __VA_ARGS__)
#     define  sscanf(s, format, ...)        c_sscanf(s, format, ##  __VA_ARGS__)
#  endif
#endif

// Stream Position

#define c_fpos_t_ void
int     c_fgetpos(C_FILE_ *restrict stream, c_fpos_t_ *restrict fpos);
int     c_fseek(C_FILE_ *stream, long offset, int whence);
int     c_fsetpos(C_FILE_ *stream, const c_fpos_t_ *fpos);
long    c_ftell(C_FILE_ *stream);
void    c_rewind(C_FILE_ *stream);
#if defined(KONPU_H_) && !defined(KONPU_OPTION_NO_STDIO)
#  define  fgetpos(stream, pos)             c_fgetpos(stream, pos)
#  define  fseek(stream, offset, whence)    c_fseek(stream, offset, whence)
#  define  fsetpos(stream, pos)             c_fsetpos(stream, pos)
#  define  ftell(stream)                    c_ftell(stream)
#  define  rewind(stream)                   c_rewind(stream)
//#define  SEEK_CUR   TODO
//#define  SEEK_END   TODO
//#define  SEEK_SET   TODO
#endif

// Error Handling

void  c_clearerr(C_FILE_ *stream);
int   c_feof(C_FILE_ *stream);
int   c_ferror(C_FILE_ *stream);
void  c_perror(const char *s);
#if defined(KONPU_H_) && !defined(KONPU_OPTION_NO_STDIO)
#  define  clearerr(stream)    c_clearerr(stream)
#  define  feof(stream)        c_feof(stream)
#  define  ferror(stream)      c_ferror(stream)
#  define  perror(s)           c_perror(s)
#endif

// Types, Objects, etc.

#if defined(KONPU_H_) && !defined(KONPU_OPTION_NO_STDIO)
#  define  fpos_t  c_fpos_t_
//#define  stdin   // TODO
//#define  stdout  // TODO
//#define  stderr  // TODO
#endif
// Not implemented by Konpu (from C23)
// #define __STDC_VERSION_STDIO_H__   ...
// #define _PRINTF_NAN_LEN_MAX        ...


//------------------------------------------------------------------------------
// TODO: Maybe/Later --- Posix-like interface
//------------------------------------------------------------------------------

/*
#if defined(KONPU_H_) && !defined(KONPU_OPTION_NO_STDIO)
   static inline int fileno(C_FILE_ *file)  { return FILEToStream(file); }
   static inline C_FILE_ *fdopen(int fd, const char *mode_ignored_)
   { (void)mode_ignored_; return StreamToFile(fd); }
#endif
*/


#endif //include guard
