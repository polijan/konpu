#include "printer.h"
#include <stdio.h>
static FILE *printer = NULL;


bool KonpuConnectPrinterToFile(const char *filename)
{
   KonpuDisconnectPrinter();

   if (filename == NULL) return false;
   printer = fopen(filename, "ab");
   // Reminder that "b" means binary mode. This has no effect on POSIX systems,
   // but on Windows it disables special handling of '\n' and '\x1A'.

   return (printer != NULL);
}

void KonpuConnectPrinterToStdout(void)
{ KonpuDisconnectPrinter(); printer = stdout; }

void KonpuConnectPrinterToStderr(void)
{ KonpuDisconnectPrinter(); printer = stderr; }

void KonpuDisconnectPrinter(void)
{
   if (printer != NULL && printer != stdout && printer != stderr)
      fclose(printer);
   printer = NULL;
}


#if defined(__GNUC__)
    __attribute__ ((format (printf, 1, 2)))
#endif
int Printer(const char *restrict format, ...)
{
   if (printer == NULL) return 0;

   va_list args;
   va_start(args, format);
   int ret = vfprintf(printer, format, args);
   va_end(args);
   return ret;
}
