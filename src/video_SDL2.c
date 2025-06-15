#include "platform.h"

#if KONPU_PLATFORM_SDL2
#   include "platform_include.h"
#   include "video.h"
#   include "init.h"
#   include "util.h"
#   include "image.h"

#include "printer.h" // TODO: for fast ugly DEBUG(), remove.

// Video Register for Errors:
static uint32_t  video_register_errors = 0;
// Expose address of static video registers variable for read-only access.
const uint32_t *VIDEO_REGISTER_ERRORS_ = &video_register_errors;


// An icon for ilo Konpu (TODO)
#define ICON_WIDTH           64
#define ICON_HEIGHT          64
#define ICON_BYTES_PER_PIXEL  4
static uint8_t ICON[ICON_WIDTH * ICON_HEIGHT * ICON_BYTES_PER_PIXEL];
// ^- This should be const and contain an image.
// static const uint8_t ICON[ICON_WIDTH * ICON_HEIGHT * ICON_BYTES_PER_PIXEL] = {
//   ...
// };

// Global state for a SDL graphical context and renderer
static struct {
   SDL_Window    *window;   // SDL window
   SDL_Renderer  *renderer; // SDL renderer
   SDL_Texture   *texture;  // Konpu graphics are rendered to this one texture

   // When rendering a frame, we check if those fields have the same value as
   // their Konpu's counterpart values. If they don't have, we have to take
   // actions during the rendering.
   int16_t  width;  // mimics the value of VIDEO_WIDTH
   int16_t  height; // mimics the value of VIDEO_HEIGHT
   int16_t  border; // mimics the value of COLOR_BORDER

} sdl = {
   .window   = NULL,
   .renderer = NULL,
   .texture  = NULL,
   .width    = -1,
   .height   = -1,
   .border   = -1,
};

//------------------------------------------------------------------------------
// Stuff to read/write on the sdl.texture (when it's been locked)
//------------------------------------------------------------------------------

// Given a 2D array (same width as Konpu's framebuffer) of pixel values,
// Return a pointer to the pixel (x,y)
#define PIXEL(pixels, x,y) \
   ((pixels) + (x) +(y) * VIDEO_WIDTH)

//------------------------------------------------------------------------------

// Release resources and quit SDL video subsystem
void VideoBackendDrop(void)
{
   if (sdl.texture)  { SDL_DestroyTexture(sdl.texture);   sdl.texture  = NULL; }
   if (sdl.renderer) { SDL_DestroyRenderer(sdl.renderer); sdl.renderer = NULL; }
   if (sdl.window)   { SDL_DestroyWindow(sdl.window);     sdl.window   = NULL; }
   SDL_QuitSubSystem(SDL_INIT_VIDEO);
}


// Initialize the SDL video system, sdl.window, and sdl.enderer.
// sdl.texture is left null, because it is (re)initialized in the rendering
// function (whenever Konpu's resolution has changed).
//
// User can specify a desired zoom level by which to magnify Konpu's framebuffer
// on the real screen. Magnification level will be as close as possible to the
// given parameter but will not exceed it. If zoom level is 0 or negative, then
// we try to initialize full screen graphics instead of windowed.
int VideoBackendInit(int zoom_desired_max)
{
   // If we have already initialized the video system, drop and re-init.
   if (sdl.window != NULL) {
      assert(sdl.renderer != NULL);
      assert(sdl.texture  != NULL);
      VideoBackendDrop();
   }
   assert(sdl.renderer == NULL);
   assert(sdl.texture  == NULL);

   video_register_errors = 0;
   // Make sure that at first render, the texture will be allocated and the
   // border redrawn.
   sdl.width  = -1;
   sdl.height = -1;
   sdl.border = -1;

   // Initialize video system
   int error = SDL_InitSubSystem(SDL_INIT_VIDEO);
   if (error) goto error;
   error = -1;

   // By default in SDL, the hint `SDL_HINT_RENDER_SCALE_QUALITY` is set to
   // "nearest" which is crisp and blocky and probably what's wished for Konpu.
   // Other options are "best" and "linear" ("best" might be same as "linear")
   //if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best"))
   //   SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");


   // Zoom level by which to multiply the max. size of a Konpu framebuffer so
   // that it fits in the display of size w x h pixels. If zoom is zero, we'll
   // try to go full screen
   int zoom = 0;
   SDL_DisplayMode display;
   if (SDL_GetDesktopDisplayMode(0, &display) == 0) {
      zoom = UTIL_MIN(display.w/VIDEO_MAX_WIDTH, display.h/VIDEO_MAX_HEIGHT);
      if (zoom < 2) {
         zoom = 0; // try to go full screen
      } else if (zoom > zoom_desired_max) {
         zoom = zoom_desired_max;
      }
   }
DEBUG("%dx%d -> zoom=%d\n", display.w, display.h, zoom); //TODO:REMOVE

   /* Create a window */
   Uint32 win_flags = 0; // Maybe we should take an extra parameter to the init
                         // function, but for now, we don't set the SDL_WINDOW_RESIZABLE
                         // flag. Note that it is possible to modify this later
                         // using the `SDL_SetWindowResizable` function.
   if (zoom <= 0)  win_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
   sdl.window = SDL_CreateWindow("ilo Konpu",
                  SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                  zoom * VIDEO_MAX_WIDTH, zoom * VIDEO_MAX_HEIGHT,
                  win_flags);
   if (sdl.window == NULL) goto error;

#if (ICON_BYTES_PER_PIXEL == 3) || (ICON_BYTES_PER_PIXEL == 4)
   // Give the window an icon
   // For now, just fill the icon with random stuff (TODO:remove)
   // Actually in the future, maybe we want it from a Glyph or a Tile.
   for (int i = 0; i < UTIL_ARRAY_SIZE(ICON); i++) { ICON[i] = i % 256; }
   SDL_Surface *icon = SDL_CreateRGBSurfaceWithFormatFrom(ICON,
      ICON_WIDTH, ICON_HEIGHT,
      ICON_BYTES_PER_PIXEL * CHAR_BIT,   // bpp
      ICON_BYTES_PER_PIXEL * ICON_WIDTH, // pitch = size in bytes of a row of pixels
#  if ICON_BYTES_PER_PIXEL == 3
      SDL_PIXELFORMAT_RGB888
#  else
      SDL_PIXELFORMAT_ARGB8888
#  endif
   );
   if (icon != NULL) {
      SDL_SetWindowIcon(sdl.window, icon);
      DEBUG("Icon was loaded\n"); // TODO:REMOVE
      SDL_FreeSurface(icon);
   }
   else DEBUG("Cannot load Icon: %s\n", SDL_GetError()); // TODO:REMOVE
#endif

   /* Create the renderer */
   // - by default SDL2 gives priority to SDL_RENDERER_ACCELERATED renderers,
   //   so it's no necessary to add flag
   Uint32 renderer_flags = 0;
   // - VSYNC? to add or not? TODO!!!
      renderer_flags |= SDL_RENDERER_PRESENTVSYNC;
   sdl.renderer = SDL_CreateRenderer(sdl.window, -1, renderer_flags);
   if (sdl.renderer == NULL) goto error;

   // We'll have to make this configurable somehow...
   SDL_RenderSetIntegerScale(sdl.renderer, true);
   // not sure the following is needed:
//   SDL_RenderSetViewport(sdl.renderer, &(SDL_Rect){ .x = 0, .y = 0,
//                                       .w = width, .h = height});

   // we have an open window with nothing in it, so let's render once ???
   error = VideoRender();
   if (error) goto error;

   return 0;
error:
   VideoBackendDrop();
   SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ilo Konpu"                ,
      "pakala! mi ken ala open e nasin sitelen " KONPU_PLATFORM_SDL_NAME "." \
      "- Error while initializing " KONPU_PLATFORM_SDL_NAME " graphics."     ,
      NULL);
   return error;
}


// Render Konpu's frambuffer into sdl.texture and present it on screen
int VideoRender(void)
{
   // If we don't have a window, assume we don't want to display anything.
   if (sdl.window == NULL) return 0;
   assert(sdl.renderer != NULL);

   // (Re)create the texture if necessary, ie. when the dimensions have changed.
   if (sdl.width != VIDEO_WIDTH || sdl.height != VIDEO_HEIGHT) {
      sdl.width  = VIDEO_WIDTH;
      sdl.height = VIDEO_HEIGHT;

      // Delete and recreate as there's no resizing for textures
      if (sdl.texture != NULL)  SDL_DestroyTexture(sdl.texture);
      sdl.texture = SDL_CreateTexture(sdl.renderer,
         SDL_PIXELFORMAT_ARGB8888,    // could we use other formats (RGB888)...?
         SDL_TEXTUREACCESS_STREAMING, // A "Streaming texture" is intended to be
                       // updated often and assumes a full update of the texture
         VIDEO_WIDTH, VIDEO_HEIGHT);
      if (sdl.texture == NULL)  return -1;

      // TODO: remove later, but useful for now to give us some info:
      const char fmt[] = "ilo Konpu - nasin sitelen # %3d ";
      char title[UTIL_STRING_LITERAL_LENGTH(fmt) + 1];
      snprintf(title, UTIL_STRING_LITERAL_LENGTH(fmt), fmt, VIDEO_MODE);
                      // ^--- videomode as a string is no longer than "%d" in the title
      SDL_SetWindowTitle(sdl.window, title);

      // `SDL_RenderSetLogicalSize` applies "letter boxing" to the viewport
      // thus creating Konpu's border.
      SDL_RenderSetLogicalSize(sdl.renderer, VIDEO_WIDTH, VIDEO_HEIGHT);

      // As dimensions changed, ensure that we'll repaint the border area.
      sdl.border = -1;
   }
   assert(sdl.texture != NULL);

   // Lock the texture to gain *WRITE-Only* access to its pixels.
   // SDL says that all pixels *must* be written before unlocking the texture,
   // otherwise they may have uninitialized value.
   int       pitch;
   void     *pixel_data;
   SDL_LockTexture(sdl.texture, NULL, &pixel_data, &pitch);
   uint32_t *pixels = pixel_data;
   assert(pixels != NULL);
   assert(pitch == (C_SIZEOF(*pixels) * VIDEO_WIDTH));

   // Render Konpu's frambuffer in to the locked texture, and as soon as we've
   // finnished drawing, release (unlock) the texture.
   VideoRenderToARGB(pixels, SDL_ALPHA_OPAQUE);
      // Here in the Future: after rendering video to a framebuffer, we can add
      // sprites or whatever overlay on top (Debug text, FPS counter, etc.)
   SDL_UnlockTexture(sdl.texture);

   // Set/draw border when the border color has changed.
   if (sdl.border != COLOR_BORDER) {
      sdl.border = COLOR_BORDER;
      const uint8_t *rgb = ColorToRGB(sdl.border);
      SDL_SetRenderDrawColor(sdl.renderer, rgb[0], rgb[1], rgb[2], SDL_ALPHA_OPAQUE);
   }
   // TODO/FIXME? Normally I'd guess this should be in the above, only needed
   // if the border changes, but for some reason, it seems needed all the time,
   // othwerwise the border just change color. WHY IS THAT? AND DOES IT IMPACT
   // MY FPS or WHAT?
      SDL_RenderClear(sdl.renderer);


   // Render
   int err = SDL_RenderCopy(sdl.renderer, sdl.texture, NULL, NULL);
   if (err) {
      video_register_errors++;
      return err;
   }
   SDL_RenderPresent(sdl.renderer);
   return 0;
}

#endif //KONPU_PLATFORM_SDL2
