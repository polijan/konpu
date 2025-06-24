#include "ram.h"

// Definition of Konpu's RAM memory
alignas(max_align_t)
#if defined(__GNUC__)
   __attribute__((__may_alias__))
#endif
uint8_t RAM[RAM_SIZE];

//------------------------------------------------------------------------------
// Static checks on the RAM addresses to verify their aligments
//------------------------------------------------------------------------------
#define RAM_STATIC_ASSERT(address, TYPE)                               \
   static_assert((address) >= 0, "address must be >= 0");              \
   static_assert((address) < RAM_SIZE, "address must be < RAM_SIZE");  \
   static_assert((address) % alignof(TYPE) == 0, "address alignement")


RAM_STATIC_ASSERT(RAM_VIDEO_BUFFER, max_align_t);
RAM_STATIC_ASSERT(RAM_COLOR_BORDER, uint8_t);
RAM_STATIC_ASSERT(RAM_COLOR_DEFAULT_FG, uint8_t);
RAM_STATIC_ASSERT(RAM_COLOR_DEFAULT_BG, uint8_t);
RAM_STATIC_ASSERT(RAM_COLOR_PALETTE, uint8_t);
RAM_STATIC_ASSERT(RAM_KEY_MOD, uint8_t);
RAM_STATIC_ASSERT(RAM_KEY_CURRENT_STATE,  uint64_t);
RAM_STATIC_ASSERT(RAM_KEY_PREVIOUS_STATE, uint64_t);
RAM_STATIC_ASSERT(RAM_RNG_STATE, uint64_t /*pcg32_random_t*/);
RAM_STATIC_ASSERT(RAM_STACK_VAR, uint64_t /*var*/);
RAM_STATIC_ASSERT(RAM_STACK_SIZE, int16_t );
RAM_STATIC_ASSERT(RAM_STACK_TYPE, uint16_t /*Type*/);
RAM_STATIC_ASSERT(RAM_HEAP, max_align_t);
RAM_STATIC_ASSERT(RAM_HEAP_CTRL, uint8_t);
RAM_STATIC_ASSERT(RAM_GENERIC, max_align_t);
