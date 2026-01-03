#include "backend-common/backend.h"
#include "backend-common/printer.h"
#include "core84/printer.h"
#include <stdio.h>

// the FILE the printer is directed to
static FILE *printer = NULL;

int BackendInitPrinter()
{ return 0; }

int BackendDropPrinter(void)
{ return KonpuDisconnectPrinter(); }


int KonpuConnectPrinterToFile(const char *filename)
{
   KonpuDisconnectPrinter();

   if (filename == NULL) return -1;
   printer = fopen(filename, "ab");
   // Reminder that "b" means binary mode. This has no effect on POSIX systems,
   // but on Windows it disables special handling of '\n' and '\x1A'.

   return (printer == NULL);
}

void KonpuConnectPrinterToStdout(void)
{ KonpuDisconnectPrinter(); printer = stdout; }

void KonpuConnectPrinterToStderr(void)
{ KonpuDisconnectPrinter(); printer = stderr; }

int KonpuDisconnectPrinter(void)
{
   int err = (printer != NULL && printer != stdout && printer != stderr) ?
      fclose(printer) : 0;

   printer = NULL;
   return err;
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
