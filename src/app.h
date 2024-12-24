#ifndef  KONPU_APP_H
#define  KONPU_APP_H
#include "c.h"

//------------------------------------------------------------------------------
// App Loading/Unloading
//------------------------------------------------------------------------------

// Load the given app and execute its `int AppInit(void)` function
// Return a handle the the app or NULL if app cannot be loaded.
//
// Note: If you load a symbol from an app, it is not defined whether or not it
// goes into the global symbol namespace for the application. If it does and it
// conflicts with symbols from Konpu or other apps, then anything can happen!
void *AppLoad(const char* app_name);

// Load the given Module/App. Check if the load function of the app/module has
// put a command on top of the stack, if so then eval/execute it.
// Then what? should I Unload the module???

// Unload an app given by its handle.
// return 0 on success / non-zero on error.
int AppUnload(void *app_handle);



/* We'll need to provide a list of loaded applications.
   (whether we want to store app handle in Konpu RAM or not, I'm not sure yet)

typedef struct App {
   // var  id;
   char id[16];

   int (*unload)(void*);
} App;

extern App  AppList[16];
// Return the list of loaded apps
AppList()
*/


#endif //KONPU_APP_H