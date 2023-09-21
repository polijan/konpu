#include "renderer_SDL2.h"
#if RENDERER_SDL2
#include "renderer.h"
#include "video.h"

// global state for a SDL2 renderer:
static SDL_Window   *rendererSDL2_win  = NULL;
static SDL_Renderer *rendererSDL2_rndr = NULL;
static SDL_Texture  *rendererSDL2_tex  = NULL;


// SDL2 renderer drop function
static int rendererSDL2_drop(void)
{
   if (rendererSDL2_tex) {
      SDL_DestroyTexture(rendererSDL2_tex);
      rendererSDL2_tex  = NULL;
   }
   if (rendererSDL2_rndr) {
      SDL_DestroyRenderer(rendererSDL2_rndr);
      rendererSDL2_rndr = NULL;
   }
   if (rendererSDL2_rndr) {
      SDL_DestroyWindow(rendererSDL2_win);
      rendererSDL2_win  = NULL;
   }
   SDL_QuitSubSystem(SDL_INIT_VIDEO);
   return 0;
}

// SDL2 render function
static int rendererSDL2_render(void)
{
   // lock our texture to gain **write-only** access to its pixels
   // all pixels *should* be written before unlocking the texture,
   // otherwise they may have uninitialized value.
   int       pitch;
   void     *pixel_data;
   SDL_LockTexture(rendererSDL2_tex, NULL, &pixel_data, &pitch);
   uint32_t *pixels = pixel_data;
   assert(pixels != NULL);
   assert(pitch == (sizeof(*pixels) * video.width_px));

   // paint the canvas onto the texture's pixels
   canvas screen = video_getScreenCanvas();
   for (int y = 0; y < screen.height * GLYPH_HEIGHT; y++) {
       for (int x = 0; x < screen.width; x++) {
           uint64_t glyph = canvas_glyph(screen, x, y / GLYPH_HEIGHT);
           unsigned char line = glyph_line(glyph, y % GLYPH_HEIGHT);

           for (int i = 0; i < GLYPH_WIDTH; i++) {
               // TODO/FIXME: maybe endianess could cause us issues
               //             also pushing uint32_t ???, naaah prob. fine
               //             or should it be bytes (unsigned char) instead
               //             or we should use SDL_MapRGB or something?
               // TODO:
               //       also in future, we'll have "attributes" here to
               //       determine the color
               if (byte_isSetBit(line, i)) {
                  *pixels = (uint32_t)(RENDERER_FG_R << 16 | RENDERER_FG_G << 8 | RENDERER_FG_B);
               } else {
                  *pixels = (uint32_t)(RENDERER_BG_R << 16 | RENDERER_BG_G << 8 | RENDERER_BG_B);
               }
               pixels++;
           }
       }
   }

   // now release (unlock) the texture and render it
   SDL_UnlockTexture(rendererSDL2_tex);
   int err = SDL_RenderCopy(rendererSDL2_rndr, rendererSDL2_tex, NULL, NULL);
   if (err)  return err;
   SDL_RenderPresent(rendererSDL2_rndr);
   return 0;
}

int rendererSDL2_init(const char* title, int win_width, int win_height)
{
   // drop the active renderer
   renderer_drop();
   rendererSingleton.error = 0;
   assert(rendererSDL2_win  == NULL);
   assert(rendererSDL2_rndr == NULL);
   assert(rendererSDL2_tex  == NULL);

   // init video
   int ret = SDL_InitSubSystem(SDL_INIT_VIDEO);
   if (ret)  goto error_init;

   // create window

   // TODO: SDL_WINDOW_RESIZABLE <-- nice, maybe, but should be configurable,
   //                         and we'd need to handle events too for resizing
   Uint32 win_flags = 0;
   // TODO: check win_width, win_height
   // they should be positive, ...
   // if they're both 0, maybe we could go full screen or something

   rendererSDL2_win = SDL_CreateWindow(
                          (title)? title : "ilo Konpu",
                          SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                          win_width, win_height, win_flags);
   if (rendererSDL2_win == NULL)  { ret = -1; goto error_window; }

   // create SDL renderer
   rendererSDL2_rndr = SDL_CreateRenderer(rendererSDL2_win, -1,
                            0); // TODO:   SDL_RENDERER_ACCELERATED???
                                //       | SDL_RENDERER_PRESENTVSYNC
   if (rendererSDL2_rndr == NULL)  { ret = -1; goto error_renderer; }

   // create a single SDL_Texture for painting the whole screen onto.
   rendererSDL2_tex = SDL_CreateTexture(rendererSDL2_rndr,
                           SDL_PIXELFORMAT_ARGB8888,
                           SDL_TEXTUREACCESS_STREAMING,
                           video.width_px, video.height_px);
   if (rendererSDL2_tex == NULL)   { ret = -1; goto error_texture; }

   // set the active render (and render() once, otherwise window is empty)
   rendererSingleton.id     = RENDERER_SDL2;
   rendererSingleton.render = &rendererSDL2_render;
   rendererSingleton.drop   = &rendererSDL2_drop;

   // TODO: to complete the initialization, we may wish to do
   //       something such as clearing the texture


   return 0;
// error handling:
error_texture:
   SDL_DestroyRenderer(rendererSDL2_rndr);
error_renderer:
   SDL_DestroyWindow(rendererSDL2_win);
error_window:
   SDL_QuitSubSystem(SDL_INIT_VIDEO);
error_init:
   return ret;
}

#endif //RENDERER_SDL2
