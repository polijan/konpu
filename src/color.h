#ifndef  KONPU_COLOR_H
#define  KONPU_COLOR_H
#include "memory.h"
#include "video_mode.h"

// The overall palette can contains 128 entries (which are index to the 256
// master colors). It allows distinct for 2,4,8,8,16,32,64 colors subpalettes
// in distinct areas.
#define COLOR_PALETTE128  (KonpuMemory + COLOR_PALETTE_ADDRESS)
#define COLOR_PALETTE2     COLOR_PALETTE128
#define COLOR_PALETTE4    (COLOR_PALETTE2  + 2)
#define COLOR_PALETTE8    (COLOR_PALETTE4  + 4)
#define COLOR_PALETTE16   (COLOR_PALETTE8  + 8)
#define COLOR_PALETTE32   (COLOR_PALETTE16 + 16)
#define COLOR_PALETTE64   (COLOR_PALETTE32 + 32)

#define COLOR_BORDER       KonpuMemory[COLOR_BORDER_ADDRESS]
#define COLOR_DEFAULT_FG   KonpuMemory[COLOR_DEFAULT_FG_ADDRESS]
#define COLOR_DEFAULT_BG   KonpuMemory[COLOR_DEFAULT_BG_ADDRESS]


// Return the palette in-use or NULL (in 256 color modes)
//
// When using this function to lookup for a color index, make sure to account
// for a possible NULL return value.
// ```
//    uint8_t color;
//    // Lookup up for the corresponding Konpu 256-color
//    uint8_t *palette = ColorPalette();
//    uint8_t color256 = (palette)? palette[color] : color;
// ```
static inline uint8_t *ColorPalette()
{
   int offset = VideoColorDepth();
   switch(offset) {
      case 8:  return NULL;        // 256 colors
#if (VIDEO_SIZE_FACTOR_ % 7 != 0)
      case 7:  offset  = 0; break; // 128 color palette
#endif
      default: offset -= 2; break; // 2,4,8,16,32,64 color palettes
   }
   return KonpuMemory + COLOR_PALETTE_ADDRESS + offset;
}

// return to
static inline const uint8_t *COLOR_RGB(int color256)
{  assert(color256 >= 0 && color256 < 256);
   return KonpuROM + KONPU_ROM_COLOR + 3 * color256;
}

#endif //KONPU_COLOR_H
