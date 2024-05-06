// This program simply calls the `KonpuMain` function.

extern int KonpuMain(int argc, char* argv[]);

#include "../platform.h"
#if KONPU_PLATFORM_LIBC
   int main(int argc, char *argv[]) {
      return KonpuMain(argc, argv);
   }
#else
   // When compiled without a libc (gcc -nostdlib), we don't have a `main`.
   // The entry point of programs is conventionally named `_start`.
   void _start() {
      /* int returnValue = */  KonpuMain(0, (void*)0);

      // Under an OS, we would call `exit(returnValue)`, and without a libc, it
      // would be done with a syscall using some assembler statement.
      // see: https://stackoverflow.com/questions/2548486/compiling-without-libc
   }
#endif
