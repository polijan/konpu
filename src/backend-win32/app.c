#include "core84/app.h"


#include <stdio.h> // TODO: remove

// #define  WIN32_LEAN_AND_MEAN
// #include <windows.h>

void *AppLoad(const char* app_name)
{
   // TODO: in Windows, there are some equivalent functions to the POSIX ones:
   // LoadLibrary / LoadLibraryEX = dlopen
   // GetProcAddress = dlsym
   // FreeLibrary = dlclose

   fprintf(stderr, "Dynamic Loading is not compiled in this version.\n");
   return NULL;
}

int AppUnload(void *app_handle)
{
   // TODO: in Windows, there are some equivalent functions to the POSIX ones:
   // LoadLibrary / LoadLibraryEX = dlopen
   // GetProcAddress = dlsym
   // FreeLibrary = dlclose

   return 0;
}
