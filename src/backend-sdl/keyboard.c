#include "platform_.h"
#include "core84/keyboard.h"

void KeyboardUpdate(void)
{
   // Instruct SDL to gather input information from devices, it does not need to
   // be done explicitely if we'd used SDL_PollEvent() or SDL_WaitEvent().
   // So, TODO maybe: instead of SDL_PumpEvent, we could/should check special
   // events here, such as SQL_QUIT, or possibly react to other special keys to
   // open a config menu, etc. (maybe the menu key?)
   SDL_PumpEvents();

   // Copy current key state to previous key state
   memcpy(Keyboard.current, Keyboard.previous, KEY_COUNT / CHAR_BIT);

   // Calculate new state from SDL keyboard state
   static_assert(KEY_COUNT <= SDL_NUM_SCANCODES);
#if KONPU_PLATFORM_SDL1
   const uint8_t *state = SDL_GetKeyState(NULL);
#else
   const uint8_t *state = SDL_GetKeyboardState(NULL);
#endif
   for (unsigned i = 0; i < KEY_COUNT / CHAR_BIT; i++) {
      Keyboard.current[i] =
         ((state[0] != 0) << 0) | ((state[1] != 0) << 1) |
         ((state[2] != 0) << 2) | ((state[3] != 0) << 3) |
         ((state[4] != 0) << 4) | ((state[5] != 0) << 5) |
         ((state[6] != 0) << 6) | ((state[7] != 0) << 7) ;
      state += 8;
   }
}
