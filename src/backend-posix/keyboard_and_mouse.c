// TODO: move this to keyboard.h, use <termios.h> or something to read keyboard

#include "backend-common/backend.h"
int BackendInitMouse(void)  { return 0; }
int BackendDropMouse(void)  { return 0; }

#include "termbox2/termbox2.h"
#include <locale.h>



#include "core84/arch.h"
void MouseUpdate(void)  { }

#include "core84/keyboard.h"
void KeyboardUpdate(void)  { }
