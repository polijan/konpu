#include "renderer_ppm.h"
#if RENDERER_PPM
#include "renderer.h"
#include "video.h"
#include <stdio.h>

// global variables for a PPM renderer:
static int rendererPPM_zoomx;
static int rendererPPM_zoomy;

/// @brief RENDERER_PPM_STREAM settings:
/// By default, the PPM renderer will output to `stdout`, however you
/// may define another stream by defining this macro constant.
#ifndef    RENDERER_PPM_STREAM
#   define RENDERER_PPM_STREAM   stdout
#endif

/// @brief RENDERER_PPM_UNLOCKED settings:
/// If this macro constant is defined and Konpu uses the POSIX platform, the PPM
/// renderer will use the "_unlocked" version of `putc` to output characters.
/// This may be faster, but is thread-UNSAFE.
#if defined(RENDERER_PPM_UNLOCKED) && KONPU_PLATFORM_POSIX
#   define RENDERER_PPM_PUTC(character, stream) \
           putc_unlocked((character), (stream))
#else
#   define RENDERER_PPM_PUTC(character, stream)  \
           putc((character), (stream))
#endif


static int
canvas_renderToPPM(const_canvas cvas, FILE* stream, int zoomx, int zoomy)
{  CANVAS_ASSERT(cvas);
   fprintf(stream, "P6\n%d %d\n255\n", zoomx * GLYPH_WIDTH  * cvas.width,
                                       zoomy * GLYPH_HEIGHT * cvas.height);

   for (int y = 0; y < GLYPH_HEIGHT * cvas.height; y++) {
       for (int ny = 0; ny < zoomy; ny++) {
           for (int x = 0; x < cvas.width; x++) {
               uint64_t glyph = canvas_glyph(cvas, x, y/GLYPH_HEIGHT);
               unsigned char line = glyph_line(glyph, y%GLYPH_HEIGHT);

               for (int i = 0; i < GLYPH_WIDTH; i++) {
                   if (byte_isSetBit(line, i)) {
                      for (int nx = 0; nx < zoomx; nx++) {
                          RENDERER_PPM_PUTC(RENDERER_FG_R, stream);
                          RENDERER_PPM_PUTC(RENDERER_FG_G, stream);
                          RENDERER_PPM_PUTC(RENDERER_FG_B, stream);
                      }
                   } else {
                      for (int nx = 0; nx < zoomx; nx++) {
                          RENDERER_PPM_PUTC(RENDERER_BG_R, stream);
                          RENDERER_PPM_PUTC(RENDERER_BG_G, stream);
                          RENDERER_PPM_PUTC(RENDERER_BG_B, stream);
                      }
                   }
               }
           }
       }
   }

   // flush the stream and check for errors
   fflush(stream);
   int err = ferror(stream);
   if (unlikely(err)) {
      clearerr(stream);
      rendererSingleton.error++;
   }
   return err;
}


// `render` function for the PPM renderer
static int
rendererPPM_render(void)
{
   canvas screen = video_getScreenCanvas();
   return canvas_renderToPPM(screen, RENDERER_PPM_STREAM,
                             rendererPPM_zoomx, rendererPPM_zoomy);
}

int rendererPPM_init(int zoomx, int zoomy)
{
   // Assuming a dimension with 200 glyphs in one direction, 20*8*200 is max
   // value which wouldn't overflow 15 bits (the max value a int may have if
   // it's 16bits). So let's restrict the zoom values to <= 20 (we put a
   // default value (3) if range is incorrect)
   if ((zoomx <= 0) || (zoomx > 20))   zoomx = 3;
   if ((zoomy <= 0) || (zoomy > 20))   zoomy = 3;
   rendererPPM_zoomx = zoomx;
   rendererPPM_zoomy = zoomy;

   // drop the active renderer (we can't do error checking)
   renderer_drop();
   rendererSingleton.error  = 0;

   // set the active render
   rendererSingleton.id     = RENDERER_PPM;
   rendererSingleton.render = &rendererPPM_render;
   render();
   return 0;
}

#endif //RENDERER_PPM
