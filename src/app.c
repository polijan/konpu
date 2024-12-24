#include "app.h"

#include "platform_include.h"
#if KONPU_PLATFORM_SDL2
   // SDL3 uses SDL_SharedObject*, but SDL2 uses void*
   typedef void SDL_SharedObject;
#endif

void *AppLoad(const char* app_name)
{
#if KONPU_PLATFORM_SDL3 || KONPU_PLATFORM_SDL2
   // Load the given shared object
   SDL_SharedObject *handle = SDL_LoadObject(app_name);
   if (handle == NULL) {
      fprintf(stderr, "Cannot load App: %s\n", SDL_GetError());
      return NULL;
   }

   // Initialize the App
   int (*init)(void);
   //init = (int (*)(void))SDL_LoadFunction(handle, "AppInit");
   *(SDL_SharedObject **)(&init) = SDL_LoadFunction(handle, "AppInit");

   if (init != NULL) {
      init();
   }

   // Initialize the App
   return handle;


#elif KONPU_PLATFORM_POSIX

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


// #elif KONPU_PLATFORM_WINDOWS
// // in Windows, there are some equivalent functions to the POSIX ones:
// // LoadLibrary / LoadLibraryEX = dlopen
// // GetProcAddress = dlsym
// // FreeLibrary = dlclose
#else
   // TODO: remove the fprintf
   fprintf(stderr, "Dynamic Loading is not compiled in this version.\n");
   return NULL;
#endif
}


int AppUnload(void *app_handle)
{
   if (app_handle == NULL) return 0;

#if KONPU_PLATFORM_SDL3 || KONPU_PLATFORM_SDL2
   SDL_UnloadObject((SDL_SharedObject *)app_handle);
   return 0;

#elif KONPU_PLATFORM_POSIX
   return dlclose(app_handle);

// #elif KONPU_PLATFORM_WINDOWS
// // in Windows, there are some equivalent functions to the POSIX ones:
// // LoadLibrary / LoadLibraryEX = dlopen
// // GetProcAddress = dlsym
// // FreeLibrary = dlclose
#else
   return 0;
#endif
}