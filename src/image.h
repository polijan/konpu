#ifndef KONPU_IMAGE_H_
#define KONPU_IMAGE_H_
#include "color.h"
#include "pixel.h"


// Render the video framebuffer into a memory buffer. The graphics are rendered
// in ARGB form as 8bits-gamma-encoded sRGB color + the given alpha component.
//
static inline void VideoRenderToARGB(uint32_t *frame_out, int alpha)
{
   const uint32_t A = (uint32_t)alpha << 24;
   const uint8_t *palette = ColorPalette();

   for (int y = 0; y < VIDEO_HEIGHT; y++) {
      for (int x = 0; x < VIDEO_WIDTH; x++) {
         int color = PixelGet_(x, y);
         const uint8_t *rgb = ColorToRGB((palette)? palette[color] : color);
         *frame_out++ = A | (rgb[0] << 16) | (rgb[1] << 8) | rgb[2];
      }
   }
}
/* TODO: uint32_t??? This invites endianness issues?
static inline void VideoRenderToARGB_(void *buffer, int alpha)
{
   uint8_t *frame = buffer;
   const int A = (uint32_t)alpha << 24;
   const uint8_t *palette = ColorPalette();

   for (int y = 0; y < VIDEO_HEIGHT; y++) {
      for (int x = 0; x < VIDEO_WIDTH; x++) {
         const int color = PixelGet_(x,y);
         const uint8_t *rgb = ColorToRGB((palette) ? palette[color] : color);
         *frame = A;
         memcpy(frame + 1, rgb, 3);
         frame += (1+3);
      }
   }
}*/

static inline void VideoRenderToRGB(void *buffer)
{
   uint8_t *frame = buffer;
   const uint8_t *palette = ColorPalette();

   for (int y = 0; y < VIDEO_HEIGHT; y++) {
      for (int x = 0; x < VIDEO_WIDTH; x++) {
         const int color = PixelGet_(x, y);
         const uint8_t *rgb = ColorToRGB((palette)? palette[color] : color);
         memcpy(frame, rgb, 3);
         frame += 3;
      }
   }
}

#endif //include guard
