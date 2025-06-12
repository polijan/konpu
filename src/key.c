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

   // Calculate new state
   static_assert(KEY_COUNT == 256);
   static_assert(KEY_COUNT <= SDL_NUM_SCANCODES);
#if KONPU_PLATFORM_SDL1
   const uint8_t *state = SDL_GetKeyState(NULL);
#else
   const uint8_t *state = SDL_GetKeyboardState(NULL);
#endif
   uint64_t c0 = 0, c1 = 0, c2 = 0, c3 = 0;
   for (int k = 0; k < 63; k++) {
      uint64_t bit = UINT64_C(1) << k;
      if (state[k])        c0 |= bit;
      if (state[k + 64])   c1 |= bit;
      if (state[k + 128])  c2 |= bit;
      if (state[k + 192])  c3 |= bit;
   }
   // Update previous state and new state in memory
   memcpy(KEY_PREVIOUS, KEY_STATE, RAM_KEY_STATE_SZ_);
   KEY_STATE[0] = c0;
   KEY_STATE[1] = c1;
   KEY_STATE[2] = c2;
   KEY_STATE[3] = c3;
}
#else
#  error "KeyUpdate() is only implemented with a SDL backend for now."
#endif // KONPU_PLATFORM_SDL_ANY
