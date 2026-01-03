#include "backend-common/backend.h"
#include "core84/arch.h"
#include "platform_.h"

#define Video Ram.video

int BackendInitMouse(void) {
   Ram.mouse.x = Video.width  / 2;
   Ram.mouse.y = Video.height / 2;
   return 0;
}

int BackendDropMouse(void) { return 0; }

void MouseUpdate(void)
{
#define ZOOMX  3
#define ZOOMY  3

   int x = Ram.mouse.x;
   int y = Ram.mouse.y;
   uint32_t buttons = SDL_GetMouseState(&x, &y);
   (void)buttons; // don't use buttons for now, TODO

/*
   x /= ZOOMX;
   y /= ZOOMY;
   if (x < 0) x = 0;
   if (y < 0) y = 0;
   if (x >= Video.width)  x = Video.width  - 1;
   if (y >= Video.height) y = Video.height - 1;
*/
   Ram.mouse.x = x;
   Ram.mouse.y = y;




   /*
   int dx = Ram.mouse.x;
   int dy = Ram.mouse.y;
   uint32_t buttons = SDL_GetRelativeMouseState(&dx, &dy);

   Ram.mouse.x += dx;
   Ram.mouse.y += dy;

   if (x < 0) x = 0;
   if (y < 0) y = 0;
   if (x >= Ram.video.width)  x = Ram.video.width  - 1;
   if (y >= Ram.video.height) y = Ram.video.height - 1;
   Ram.mouse.x = x;
   Ram.mouse.y = y;
   */
}
