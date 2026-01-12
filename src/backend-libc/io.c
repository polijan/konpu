// Implement core/cstdio.h by wappring with <stdio.h>

#include "core84/io.h"
#undef FILE
#undef fpos_t

#include  <stdio.h>
#define FILE_    void
#define fpos_t_  void




// Somehow with -Wsuggest-attribute=format, GCC suggests we'd use format
// __attribute__ not just with variadic ... parameters but also with va_list.
// I don't get it, so disable for now.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsuggest-attribute=format"

int c_remove(const char* filename)
{ return remove(filename); }

int c_rename(const char* old_name, const char* new_name)
{ return rename(old_name, new_name); }

// FILE_ *c_tmpfile(void)
// { return tmpfile(); }

// char *c_tmpnam(char* s)
// { return tmpnam(s); }

int c_fclose(FILE_* stream)
{ return fclose((FILE *)stream); }

int c_fflush(FILE_* stream)
{ return fflush((FILE *)stream); }

// note for modes:
// POSIX systems makes no distinction between binary and text mode.
// Konpu operate similarly, so on non-POSIX systems, we will always add 'b' if
// it is not already added.
#include <string.h>

// Give a mode for fopen/freopen, return a string based on the mode but also
// making sure it as 'b' binary indicator in it.
// - *tiny_buffer is just used as a small backing buffer in which may store the
//   returned string.
static const char *ensure_binary_mode(const char *mode, uint64_t *tiny_buffer)
{
   // If the mode already has a 'b' in it, we just return it.
   if (strchr(mode, 'b')) return mode;

   // Otherwise, we copy the mode in the tiny_buffer and append a 'b'
   size_t len = strlen(mode);
   if C_HINT_UNLIKELY (len > sizeof(*tiny_buffer) - 1)  return NULL;
   *tiny_buffer = 0;
   char *m = (char *)tiny_buffer;
   strcpy(m, mode);
   m[len - 1] = 'b';
   return m;
}

FILE_ *c_fopen(const char* restrict filename, const char* restrict mode)
{
   uint64_t buf;
   const char *bin_mode = ensure_binary_mode(mode, &buf);
   return (bin_mode) ? fopen(filename, bin_mode) : NULL;
}

FILE_ *c_freopen(const char* restrict filename, const char* restrict mode, FILE_* restrict stream)
{
   uint64_t buf;
   const char *bin_mode = ensure_binary_mode(mode, &buf);
   return (bin_mode) ? freopen(filename, bin_mode, stream) : NULL;
}

// void c_setbuf(FILE_* restrict stream, char* restrict buf)
// { setbuf((FILE *)stream, buf); }

int c_setvbuf(FILE_* restrict stream, char* restrict buf, int mode, size_t size)
{ return setvbuf((FILE *)stream, buf, mode, size); }

int c_printf(const char* restrict format, ...)
{
   va_list args;
   va_start(args, format);
   int ret = vprintf(format, args);
   va_end(args);
   return ret;
}

int c_scanf(const char* restrict format, ...)
{
   va_list args;
   va_start(args, format);
   int ret = vscanf(format, args);
   va_end(args);
   return ret;
}

int c_snprintf(char* restrict s, size_t n, const char* restrict format, ...)
{
   va_list args;
   va_start(args, format);
   int ret = vsnprintf(s,n, format, args);
   va_end(args);
   return ret;
}

int c_sprintf(char* restrict s, const char* restrict format, ...)
{
   va_list args;
   va_start(args, format);
   int ret = vsprintf(s, format, args);
   va_end(args);
   return ret;
}

int c_sscanf(const char* restrict s, const char* restrict format, ...)
{
   va_list args;
   va_start(args, format);
   int ret = vsscanf(s, format, args);
   va_end(args);
   return ret;
}

int c_vfprintf(FILE_* restrict stream, const char* restrict format, va_list arg)
{ return vfprintf((FILE *)stream, format, arg); }

int c_vfscanf(FILE_* restrict stream, const char* restrict format, va_list arg)
{ return vfscanf((FILE *)stream, format, arg); }

int c_vprintf(const char* restrict format, va_list arg)
{ return vprintf(format, arg); }

int c_vscanf(const char* restrict format, va_list arg)
{ return vscanf(format, arg); }

int c_vsnprintf(char* restrict s, size_t n, const char* restrict format, va_list arg)
{ return vsnprintf(s, n, format, arg); }

int c_vsprintf(char* restrict s, const char* restrict format, va_list arg)
{ return vsprintf(s, format, arg); }

int c_vsscanf(const char* restrict s, const char* restrict format, va_list arg)
{ return vsscanf(s, format, arg); }

int c_fgetc(FILE_* stream)
{ return fgetc((FILE *)stream); }

char *c_fgets(char* restrict s, int n, FILE_* restrict stream)
{ return fgets(s, n, (FILE *)stream); }

int c_fputc(int c, FILE_* stream)
{ return fputc(c, (FILE *)stream); }

int c_fputs(const char* restrict s, FILE_* restrict stream)
{ return fputs(s, (FILE *)stream); }

int c_getc(FILE_* stream)
{ return getc((FILE *)stream); }

int c_getchar(void)
{ return getchar(); }

int c_putc(int c, FILE_* stream)
{ return putc(c, (FILE *)stream); }

// int c_putchar(int c)
// { return putchar(c); }

// int c_puts(const char* s)
// { return puts(s); }

int c_ungetc(int c, FILE_* stream)
{ return ungetc(c, (FILE *)stream); }

size_t c_fread(void* restrict ptr, size_t size, size_t nmemb, FILE_* restrict stream)
{ return fread(ptr, size, nmemb, (FILE *)stream); }

size_t c_fwrite(const void* restrict ptr, size_t size, size_t nmemb, FILE_* restrict stream)
{ return fwrite(ptr, size, nmemb, (FILE *)stream); }

int c_fgetpos(FILE_* restrict stream, fpos_t_* restrict pos)
{ return fgetpos((FILE *)stream, (fpos_t *)pos); }

int c_fseek(FILE_* stream, long offset, int whence)
{ return fseek((FILE *)stream, offset, whence); }

int c_fsetpos(FILE_* stream, const fpos_t_* pos)
{ return fsetpos((FILE *)stream, pos); }

long c_ftell(FILE_* stream)
{ return ftell((FILE *)stream); }

void c_rewind(FILE_* stream)
{ rewind((FILE *)stream); }

void c_clearerr(FILE_* stream)
{ clearerr((FILE *)stream); }

int c_feof(FILE_* stream)
{ return feof((FILE *)stream); }

int c_ferror(FILE_* stream)
{ return ferror((FILE *)stream); }

void c_perror(const char* s)
{ perror(s); }

int c_fprintf(FILE_* restrict stream, const char* restrict format, ...)
{
   va_list args;
   va_start(args, format);
   int ret = vfprintf((FILE *)stream, format, args);
   va_end(args);
   return ret;
}

/*TODO
int c_fscanf(FILE* restrict stream, const char* restrict format, ...)
{
   va_list args;
   va_start(args, format);
   int ret = vfscanf((FILE *)stream, format, args);
   va_end(args);
   return ret;
}
*/

#pragma GCC diagnostic pop // pop back every warning, we ignore
