#include "video.h"
#include "rectangle.h"
#include "printer.h"
#include "pixel.h"


// Initialize video
// - assuming the glyph area is zero'ed out
// - the video mode is set to default
static void video_init_(void)
{
// memset(&Video, 0, sizeof(Video));

   // Reset palette
   ColorResetPalette();

   // Reset border / stray
   Video.border = COLOR_CSS_DARK_GRAY;
   Video.active = (Rectangle){ 0,0, VIDEO_WIDTH_GLYPH32, VIDEO_HEIGHT_GLYPH32 };

   // needs to be set in the video mode function!!!
   Video.active_window = (Window){
      .rectangle = { 0,0, VIDEO_WIDTH_GLYPH32, VIDEO_HEIGHT_GLYPH32 },
         // VIDEO_RECTANGLE_FORM,
      .pen = COLOR_TTY_WHITE, // <-- TODO
   };

   // Set attribute area with the pen 1 and paper 0
   int attr_offset = VideoAttributeOffset();
   int attr =  Video.active_window.pen << 4 | Video.active_window.paper;
   //int attr = 0x10;
   memset(Video.frame + attr_offset, attr, VIDEO_SIZE - attr_offset);

   // Render the video
   VideoRender();
}

static void VideoInit(void)
{
   // We assume the whole video area is zero'ed out.
   VideoMode_DoNotClear_(VIDEO_MODE_DEFAULT);
   video_init_();
}

void VideoReset(void)
{
   // Zero out the framebuffer area occupied by the Glyphs in the default mode
   VideoMode_DoNotClear_(VIDEO_MODE_DEFAULT);
   memset(Video.frame, 0, VIDEO_GLYPHS_SIZE);

   video_init_();
}


//------------------------------------------------------------------------------
// VideoClear*() functions
//------------------------------------------------------------------------------

void VideoClearAttributes(void)
{
   int attr8;
   switch (ATTRIBUTE_COLOR_TYPE) {
      default: unreachable();
      case ATTRIBUTE16_COLOR:
         AttributeSetAll(ATTRIBUTE16(Video.attr_default_pen, Video.attr_default_paper));
         return;

      case ATTRIBUTE8_COLOR:
         attr8 = (Video.attr_default_pen << 4) | Video.attr_default_paper;
         break;
      case ATTRIBUTE8_COLOR_PEN:   attr8 = Video.attr_default_pen;   break;
      case ATTRIBUTE8_COLOR_PAPER: attr8 = Video.attr_default_paper; break;
   }
   AttributeSetAll(attr8);
}

void VideoClear(void)
{
   if (VideoModeHasAttributes()) {
      memset(Video.frame, 0, VIDEO_FORMS_SIZE);
      VideoClearAttributes();
   } else {
      memset(Video.frame, 0, VIDEO_SIZE);
   }
}


//------------------------------------------------------------------------------
// VideoMode*() functions
//------------------------------------------------------------------------------

// For Debugging purpose, display the video mode and related info on the Printer
static void VideoModePrint(void)
{
   int sz8x8 = VideoMode_DryRun_(VIDEO_MODE);
   if (sz8x8 == 0) {
      Printer("Invalid Video Mode: %3d\n", VIDEO_MODE);
      return;
   }
   Printer("%3d   %2d   %3dx%-3d   %3d   ",
      VIDEO_MODE, sz8x8, VIDEO_WIDTH, VIDEO_HEIGHT, 1 << ColorDepth());

   enum VideoElementDimension elem_dimension = VideoModeDimension();
   int low_nibble = VideoModeLowNibble();

   // Attribute Modes ----------------------------------------------------------
   if (VideoModeHasAttributes()) {
      if (elem_dimension == PIXELS_Nx1) {
         Printer("Strip8 (%dx%d", VIDEO_WIDTH_STRIP8, VIDEO_HEIGHT_STRIP);
      } else {
         Printer("Glyph%d (%dx%d", 8 << elem_dimension, VIDEO_WIDTH_GLYPH, VIDEO_HEIGHT_GLYPH);
      }

      const char *dimension_str[] = {"2x4","4x4","4x8","8x8","8x16","16x16"};
      const char *attr_color_type_str[] = {"8", "8_PEN", "8_PAPER", "16"};
      Printer(") + Attribute_%s_%s (%dx%d @ %d)\n",
         dimension_str[ATTRIBUTE_DIMENSION],
         attr_color_type_str[ATTRIBUTE_COLOR_TYPE],
         VIDEO_WIDTH_ATTRIBUTE, VIDEO_HEIGHT_ATTRIBUTE,
         VideoAttributeOffset());
      return;
   }

   // Chunky Modes -------------------------------------------------------------
   if (low_nibble > 8) {
      int bpp = 1 << (low_nibble - 8);
      if (elem_dimension == PIXELS_Nx1) {
         Printer("Strip%d (%dx%d", bpp, VIDEO_WIDTH_STRIP, VIDEO_WIDTH_STRIP);
      } else {
         Printer("TileT-O-D-O (%dx%d",
/*          chunk, 8 * bpp * (1 << elem_dimension),
            dimension_str[elem_dimension], */
            // TODO: we probably want VIDEO_WIDTH_TILE, VIDEO_HEIGHT_TILE
            //       name. In any case, tile's width and height depends
            //       on the same dimension indicator in the VIDEO_MODE,
            //       so it's in fact same implementation as glyphs:
            VIDEO_WIDTH_GLYPH, VIDEO_HEIGHT_GLYPH);
      }
      Printer(")\n");
      return;
   }

   // Planar Modes -------------------------------------------------------------
   if (elem_dimension == PIXELS_Nx1) {
      Printer("Strip8 (%dx%d", VIDEO_WIDTH_STRIP8, VIDEO_HEIGHT_STRIP);
   } else {
      Printer("Glyph%d (%dx%d", 8 << elem_dimension, VIDEO_WIDTH_GLYPH, VIDEO_HEIGHT_GLYPH);
   }
   if (low_nibble > 1) {
      Printer(") x %d Planes (@ n * %d", low_nibble, VIDEO_SIZE / low_nibble);
   }
   Printer(")\n");
}

int VideoMode_DryRun_(int mode)
{
   if (mode < 0 || mode > 255) return 0;
   unsigned element_descriptor = (mode >> 4) & 0x7;
   if (element_descriptor == 6) return 0; // descriptor should not be 6
   unsigned attribute_bit = mode >> 7;
   unsigned low_nibble = mode & 0xf;

   // Glyph and Attribute mode
   if (attribute_bit) {
      int attr_nbytes_log2  = (low_nibble & 3) == ATTRIBUTE16_COLOR;
      int attr_npixels_log2 = low_nibble >> 2;
      return 8 + ((8 >> attr_npixels_log2) << attr_nbytes_log2);
   }

   // Planar modes (and error case where low_nibble would be 0)
   if (low_nibble <= 8) {
#if (VIDEO_FACTOR_ % 7 != 0)
      if (low_nibble == 7) return 0;
#endif
      return 8 * low_nibble;
   }

   if (low_nibble > 11) return 0;

   // Chunky Modes:
   // Either we're in pixel modes (element_descriptor is 7)
   // Or otherwise, we're in tile mode, and then have constraints based on what
   // type of chunk tile the low_nibble tells us we have:
   // low_   | => Tile type -> and possible sizes  | => what value are possible
   // nibble |                                     |    for element_descriptor
   // -------|-------------------------------------|---------------------------
   //   9    | TileQuarter -> 2x4,4x4,4x8,8x8,8x16 | 0 - 4
   //   10   | TileNible   -> 2x4, 4x4, 4x8, 8x8   | 0 - 3
   //   11   | TileByte    -> 2x4, 4x4, 4x8        | 0 - 2
   // Thus from the table, we can see that for tiles, we must have:
   // low_nibble + element_descriptor must be <= 13
   if ((element_descriptor == 7) || (low_nibble + element_descriptor) <= 13) {
      // low_nibble ->
      // case 9:  return 16;
      // case 10: return 32;
      // case 11: return 64;
      return 1 << (low_nibble - 5);
   }

   return 0;
}


int VideoMode_DoNotClear_(int mode)
{
   int index;
   int res = VideoMode_DryRun_(mode);
   switch (res) {
      case  0: return res;
      case  8: index =  0 * 2; break;
      case  9: index =  1 * 2; break;
      case 10: index =  2 * 2; break;
      case 12: index =  3 * 2; break;
      case 16: index =  4 * 2; break;
      case 24: index =  5 * 2; break;
      case 32: index =  6 * 2; break;
      case 40: index =  7 * 2; break;
      case 48: index =  8 * 2; break;
#if (VIDEO_FACTOR_ % 7 == 0)
      case 56: index =  9 * 2; break;
#endif
      case 64: index = 10 * 2; break;
      default: unreachable();
   }

   // Update the Video's mode, width, height in RAM.
   // Note: those fields are marked const in the RAM as they aren't meant to be
   //       modifed by the casual user, so we must "un-const" them with a cast.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
   *(uint8_t*)&Video.mode   = mode;
   *(int16_t*)&Video.width  = 8 * Rom.resolution_8x8[index];
   *(int16_t*)&Video.height = 8 * Rom.resolution_8x8[index + 1];
#pragma GCC diagnostic pop

   Printer("Video Mode "); VideoModePrint();
   return res;
}
