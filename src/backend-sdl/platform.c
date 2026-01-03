#include "backend-common/backend.h"

#include "platform_.h"

int BackendInitPlatform(void)
{ return SDL_Init(0); }

int BackendDropPlatform(void)
{
   SDL_Quit();
   // SDL_Quit should always be invoked when leaving SDL, even when if every
   // subsystem has already been shutdown via SDL_QuitSubSystem().
   return 0;
}
