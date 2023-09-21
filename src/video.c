#include "video.h"

// static framebuffer
static uint64_t framebuffer[VIDEO_BUFSZ / 8];

static const uint16_t videomode_width_px[] = {
       VIDEO_MODE0_WIDTH_PX, VIDEO_MODE1_WIDTH_PX, VIDEO_MODE2_WIDTH_PX,
       VIDEO_MODE3_WIDTH_PX, VIDEO_MODE4_WIDTH_PX, VIDEO_MODE5_WIDTH_PX,
       VIDEO_MODE6_WIDTH_PX, VIDEO_MODE7_WIDTH_PX, VIDEO_MODE8_WIDTH_PX };

static const uint16_t videomode_height_px[] = {
       VIDEO_MODE0_HEIGHT_PX, VIDEO_MODE1_HEIGHT_PX, VIDEO_MODE2_HEIGHT_PX,
       VIDEO_MODE3_HEIGHT_PX, VIDEO_MODE4_HEIGHT_PX, VIDEO_MODE5_HEIGHT_PX,
       VIDEO_MODE6_HEIGHT_PX, VIDEO_MODE7_HEIGHT_PX, VIDEO_MODE8_HEIGHT_PX };



// global video object (starts in MODE4)
struct video video = {
   .buffer      = framebuffer             ,
   .width_px    = VIDEO_MODE4_WIDTH_PX    ,
   .height_px   = VIDEO_MODE4_HEIGHT_PX   ,
   .width_cell  = VIDEO_MODE4_WIDTH_CELL  ,
   .height_cell = VIDEO_MODE4_HEIGHT_CELL ,
   .attr = (uint8_t*)(framebuffer + VIDEO_MODE4_WIDTH_CELL * VIDEO_MODE4_HEIGHT_CELL),
};


void video_setMode(enum video_mode mode)
{
   // sanitize parameter
   if (mode < 0 || mode > ARRAY_SIZE(videomode_width_px))
      mode = VIDEO_MODE4;

   // if video mode doesn't change, no need to bother
   if (video.mode == mode)
      return;

   // update video object
   video.mode        = mode;
   video.width_px    = videomode_width_px[mode];
   video.height_px   = videomode_height_px[mode];
   video.width_cell  = video.width_px  / GLYPH_WIDTH;
   video.height_cell = video.height_px / GLYPH_HEIGHT;
   if (mode == 0 || mode > 4) {
      video.attr = NULL;
   } else {
      video.attr = (uint8_t*)(video.buffer + video.width_cell * video.height_cell);
   }

   // TODO: we need a mechanism to notify the renderer
   // ...
}
