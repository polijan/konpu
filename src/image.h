#ifndef KONPU_IMAGE_H_
#define KONPU_IMAGE_H_
#include "color.h"
#include "pixel.h"


// Render the video framebuffer into a memory buffer. The graphics are rendered
// in ARGB form as 8bits-gamma-encoded sRGB color + the given alpha component.
static inline void VideoRenderToARGB(uint32_t *frame_out, int alpha)
{
   const uint32_t A = (uint32_t)alpha << 24;
   const uint8_t *palette = ColorPalette();

   for (int y = 0; y < VIDEO_HEIGHT; y++) {
      for (int x = 0; x < VIDEO_WIDTH; x++) {
         int color = PixelGet_(x, y);
         uint8_t c = (palette)? palette[color] : color;
         *frame_out++ = A | ((uint32_t)Rom.color[c].r << 16) | ((uint32_t)Rom.color[c].g << 8) | Rom.color[c].b;
      }
   }
}

// Render the video framebuffer into a memory buffer. The graphics are rendered
// in RGB form as 8bits-gamma-encoded sRGB color.
static inline void VideoRenderToRGB(uint8_t *buffer)
{
   const uint8_t *palette = ColorPalette();

   for (int y = 0; y < VIDEO_HEIGHT; y++) {
      for (int x = 0; x < VIDEO_WIDTH; x++) {
         int color = PixelGet_(x, y);
         uint8_t c = (palette)? palette[color] : color;
         buffer[0] = Rom.color[c].r;
         buffer[1] = Rom.color[c].g;
         buffer[2] = Rom.color[c].b;
         buffer += 3;
      }
   }
}

#endif //include guard
