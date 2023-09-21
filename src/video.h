#ifndef  KONPU_VIDEO_H
#define  KONPU_VIDEO_H
#include "canvas.h"

enum video_mode {
   //-----------.----------------------------------------.-----------.-------.
   //    Mode   |  Description                           | Resolution| Grid  |
   //-----------+----------------------------------------+-----------+-------|
   VIDEO_MODE0, // glyphs, no attributes                 | 400 x 288 | 50x36 |
   VIDEO_MODE1, // glyphs + 1 attribute                  | 400 x 256 | 50x32 |
   VIDEO_MODE2, // glyphs + 2 attributes (left and right)| 360 x 256 | 45x32 |
   VIDEO_MODE3, // glyphs + 2 attributes (top and bottom)| 360 x 256 | 45x32 |
   VIDEO_MODE4, // glyphs + 4 attributes [STARTING MODE] | 320 x 240 | 40x30 |
   VIDEO_MODE5, // glyphs + 1-bit coloring               | 288 x 200 | 36x25 |
   VIDEO_MODE6, // 2-bits pixels (4 colors)              | 288 x 200 | 36x25 |
   VIDEO_MODE7, // 4-bits pixels (16 colors)             | 200 x 144 | 25x18 |
   VIDEO_MODE8, // 8-bits pixels (256 colors)            | 144 x 100 | 18x12 |
}; //----------------------------------------------------'-----------'-------'

/// set the video mode
void video_setMode(enum video_mode mode);

/// return a canvas representing the whole grid on the screen
static inline  canvas  video_getScreenCanvas(void);


// return a (uint8_t*) pointer to the start of the attributes
#define canvas_attrs(canvas)    (video.attr + (video.buffer - (canvas).glyphs))

// return a pointer to the attribute for (x,y)
static inline uint8_t* canvas_attr(canvas cvas, int x, int y);




//--- video mode contants ------------------------------------------------------

// video buffer size (in bytes)
#define VIDEO_BUFSZ              14400

#define VIDEO_MODE0_WIDTH_PX       400
#define VIDEO_MODE0_HEIGHT_PX      288
#define VIDEO_MODE0_WIDTH_CELL      50
#define VIDEO_MODE0_HEIGHT_CELL     36

#define VIDEO_MODE1_WIDTH_PX       400
#define VIDEO_MODE1_HEIGHT_PX      256
#define VIDEO_MODE1_WIDTH_CELL      50
#define VIDEO_MODE1_HEIGHT_CELL     32

#define VIDEO_MODE2_WIDTH_PX       360
#define VIDEO_MODE2_HEIGHT_PX      256
#define VIDEO_MODE2_WIDTH_CELL      45
#define VIDEO_MODE2_HEIGHT_CELL     32

#define VIDEO_MODE3_WIDTH_PX       VIDEO_MODE2_WIDTH_PX
#define VIDEO_MODE3_HEIGHT_PX      VIDEO_MODE2_HEIGHT_PX
#define VIDEO_MODE3_WIDTH_CELL     VIDEO_MODE2_WIDTH_CELL
#define VIDEO_MODE3_HEIGHT_CELL    VIDEO_MODE2_HEIGHT_CELL

#define VIDEO_MODE4_WIDTH_PX       320
#define VIDEO_MODE4_HEIGHT_PX      240
#define VIDEO_MODE4_WIDTH_CELL      40
#define VIDEO_MODE4_HEIGHT_CELL     30

#define VIDEO_MODE5_WIDTH_PX       288
#define VIDEO_MODE5_HEIGHT_PX      200
#define VIDEO_MODE5_WIDTH_CELL      36
#define VIDEO_MODE5_HEIGHT_CELL     25

#define VIDEO_MODE6_WIDTH_PX       288
#define VIDEO_MODE6_HEIGHT_PX      200
#define VIDEO_MODE6_WIDTH_CELL      36
#define VIDEO_MODE6_HEIGHT_CELL     25

#define VIDEO_MODE7_WIDTH_PX       200
#define VIDEO_MODE7_HEIGHT_PX      144
#define VIDEO_MODE7_WIDTH_CELL      25
#define VIDEO_MODE7_HEIGHT_CELL     18

#define VIDEO_MODE8_WIDTH_PX       144
#define VIDEO_MODE8_HEIGHT_PX      100
#define VIDEO_MODE8_WIDTH_CELL      18
#define VIDEO_MODE8_HEIGHT_CELL     12


//--- inline implementation ----------------------------------------------------


// global video object: you can read the fields, but don't modify them directly.
struct video {
   uint64_t        *buffer;        // framebuffer    (contains pixels or glyphs)
   uint8_t         *attr;          // attributes      (or NULL if no attributes)
   int              width_px;      // width in pixels
   int              height_px;     // height in pixels
   int              width_cell;    // width of the grid in cells  (= width_px/8)
   int              height_cell;   // height of the grid in cells (=height_px/8)
   enum video_mode  mode;          // video mode
};
extern struct video video;

// TODO: modify the name to video_screenGrid(void) or something
// TODO: modify canvas_ to grid_ ???
static inline canvas
video_getScreenCanvas(void)
{
   return (canvas) { .glyphs = video.buffer,
                     .width  = video.width_cell,
                     .height = video.height_cell,
                     .stride = video.width_cell
                   };
}

static inline uint8_t* canvas_attr(canvas cvas, int x, int y)
{ return canvas_attrs(cvas) + canvas_glyphIndex(cvas, x,y); }

#endif //KONPU_VIDEO_H
