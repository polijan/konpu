#include "sokol/sokol_app.h"
#include "core84/keyboard.h"

void KeyboardUpdate(void)
{
   // TODO

   // Copy current key state to previous key state
   memcpy(Keyboard.current, Keyboard.previous, KEY_COUNT / CHAR_BIT);


}
