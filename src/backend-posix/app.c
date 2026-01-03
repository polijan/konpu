#include "core84/app.h"
#include <stdio.h>
#include <dlfcn.h> // dlopen/dlsym to access shared objs (requires: -ldl)

void *AppLoad(const char* app_name)
{
   // Load the given shared object
   void *handle = dlopen(app_name, RTLD_LAZY | RTLD_GLOBAL);
   if (handle == NULL) {
      fprintf(stderr, "Cannot load App: %s\n", dlerror());
      return NULL;
   }

   // Initialize the App
   int (*init)(void);
   *(void **)(&init) =  dlsym(handle, "AppInit");
     /* ^-- This cast is because init = dlsym(... does produce warnings in
      * `gcc -pedantic` as in ISO C, casting between function pointers and
      * void* is undefined behaviour although most architectures would support
      * such casting and (the 2013 Technical Corrigendum 1 to) POSIX.1-2008
      * states that a conforming implementations should support such. */
   if (init != NULL) {
      init();
   }
   return handle;
}

int AppUnload(void *app_handle)
{
   if (app_handle == NULL) return 0;
   return dlclose(app_handle);
}
