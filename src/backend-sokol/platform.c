#include "platform.h"
#include <stdio.h>
#include <stdlib.h>

int BackendInitPlatform(void) { return 0; }
int BackendDropPlatform(void) { return 0; }

int BackendInitGraphics(int desired_zoom_max);
int BackendDropGraphics(void);

int BackendInitAudio(void)  { return 0; }
int BackendDropAudio(void)  { return 0; }

int BackendInitKeyboard(void);
int BackendDropKeyboard(void);

int BackendInitMouse(void);
int BackendDropMouse(void);

//------------------------------------------------------------------------------

#include "core84/video_mode.h"
int VideoRender(void) {
   printf("VideoRender\n");
   return 0;
}

#include "core84/arch.h"
void MouseUpdate(void) {
   printf("MouseUpdate\n");
}

//------------------------------------------------------------------------------

// Callback that Sokol will call once after the application window, 3D rendering
// context and swap chain have been created.
static void init_callback(void)
{ printf("init_callback\n"); }



// Per-frame callback. Sokol would usually call this 60 times per second.
static void frame_callback(void)
{
   VideoRender();
   printf("frame_callback\n");
}

// Callback that Sokolo would call once right before the application quits
static void cleanup_callback(void)
{ printf("cleanup_callback\n"); }

// Callback used by Sokol for input handling (or other types of events)
static void event_callback(const sapp_event *event)
{ printf("event_callback: %p\n", (const void*)event); }



// RGBA8 data for a small icon for ilo Konpu (ilo + emitters)
static uint8_t icon8x8_RGBA8[] = {
#  define a  255
#  define w  255,255,255,a
#  define B  0,0,255,a
#  define R  255,0,0,a
//
   B,w,w,B,w,w,B,w,  // B . . B . . B .
   w,B,w,B,w,B,w,w,  // . B . B . B . .
   w,w,w,w,w,w,w,w,  // . . . . . . . .
   R,R,R,R,R,R,R,w,  // R R R R R R R .
   R,w,w,R,w,w,R,w,  // R . . R . . R .
   R,R,R,R,R,R,R,w,  // R R R R R R R .
   w,w,w,R,w,w,w,w,  // . . . R . . . .
   w,w,w,R,w,w,w,w,  // . . . R . . . .
//
#  undef a
#  undef w
#  undef B
#  undef R
};


// Sokol's entry point (using SOKOL_NO_ENTRY, it's possible to tell sokol_app.h
// not to "hijack" the main() function, but this works fine)
sapp_desc sokol_main(int argc, char* argv[])
{
   sapp_desc sokol_app = {
      .fullscreen = false,
      .window_title = "ilo Konpu",

      .icon.images[0] = {
         .width       = 8,
         .height      = 8,
         .pixels.size = 8*8,
         .pixels.ptr  = icon8x8_RGBA8,
      },

      // preferred size of the rendering canvas
      .width  = 640, // TODO
      .height = 480, // TODO

      // Callbacks
      .init_cb    = init_callback,
      .frame_cb   = frame_callback,
      .cleanup_cb = cleanup_callback,
      .event_cb   = event_callback,

      // Add logging function from <sokol_log.h> to get any logging output in
      // case of errors
      .logger.func = slog_func,
   };

   (void)argc; (void)argv; // TODO:
   // BackendMain(argc, argv);

   printf("Hello, Sokol\n");
   return sokol_app;
}