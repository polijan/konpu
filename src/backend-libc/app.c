#include "core84/app.h"
#include <stdio.h>

#define ERROR_EN   "App Loading is not available using only a pure C backend."
#define ERROR_TOK  "ilo Konpu pi toki C taso li ken ala open e ilo ante."

void *AppLoad(const char* app_name)
{
   (void)app_name;
   fputs(ERROR_EN "\n", stderr);
   return NULL;
}

int AppUnload(void *app_handle)
{ (void)app_handle; return 0; }
