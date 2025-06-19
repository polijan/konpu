#include "key.h"

#include "platform_include.h"
#if KONPU_PLATFORM_SDL_ANY
void KeyUpdate(void)
{
   // Instruct SDL to gather input information from devices, it does not need to
   // be done explicitely if we'd used SDL_PollEvent() or SDL_WaitEvent().
   // So, TODO maybe: instead of SDL_PumpEvent, we could/should check special
   // events here, such as SQL_QUIT, or possibly react to other special keys to
   // open a config menu, etc. (maybe the menu key?)
   SDL_PumpEvents();

   // Copy current key state to previsou key state
   memcpy(KEY_PREVIOUS_STATE, KEY_CURRENT_STATE, RAM_KEY_CURRENT_STATE_SZ_);

   // Calculate new state from SDL keyboard state
   static_assert(KEY_COUNT <= SDL_NUM_SCANCODES);
#if KONPU_PLATFORM_SDL1
   const uint8_t *state = SDL_GetKeyState(NULL);
#else
   const uint8_t *state = SDL_GetKeyboardState(NULL);
#endif
   for (unsigned i = 0; i < RAM_KEY_CURRENT_STATE_SZ_; i++) {
      KEY_CURRENT_STATE[i] =
         ((state[0] != 0) << 0) | ((state[1] != 0) << 1) |
         ((state[2] != 0) << 2) | ((state[3] != 0) << 3) |
         ((state[4] != 0) << 4) | ((state[5] != 0) << 5) |
         ((state[6] != 0) << 6) | ((state[7] != 0) << 7) ;
      state += 8;
   }
}
#else
#  error "KeyUpdate() is only implemented with a SDL backend for now."
#endif // KONPU_PLATFORM_SDL_ANY
