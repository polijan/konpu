#include "video.h"
#include "init.h"

// Video Registers
static uint8_t  video_register_mode;
static int16_t  video_register_width;
static int16_t  video_register_height;
// Expose address of static video registers variable for read-only access.
const uint8_t  *VIDEO_REGISTER_MODE_   = &video_register_mode;
const int16_t  *VIDEO_REGISTER_WIDTH_  = &video_register_width;
const int16_t  *VIDEO_REGISTER_HEIGHT_ = &video_register_height;


//------------------------------------------------------------------------------

void VideoInit(void)
{ VideoMode(VIDEO_MODE_DEFAULT);

  // We need to reset the attributes...
  int attr_offset = VideoAttributeOffset();
  int attr = COLOR_PALETTE16[1] << 4 | COLOR_PALETTE16[0];
  memset(VIDEO_BUFFER + attr_offset, attr, VIDEO_SIZE - attr_offset);
}

//#include <string.h>
void VideoReset(void)
{
   VideoInit();

   // Clear the glyphs
   int attr_offset = VideoAttributeOffset();
   memset(VIDEO_BUFFER, 0, attr_offset);
}

int VideoMode_(int mode)
{
   if (mode < 0 || mode > 255) return 0;
   unsigned element_descriptor = (mode >> 4) & 0x7;
   if (element_descriptor == 6) return 0; // descriptor should not be 6
   unsigned attribute_bit = mode >> 7;
   unsigned low_nibble = mode & 0xf;

   // Glyph and Attribute mode
   if (attribute_bit) {
      int attr_nbytes_log2  = (low_nibble & 3) == ATTRIBUTE_COLORS_256;
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

   // Chuncky Modes:
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

int VideoMode(int mode)
{
   int index;
   int res = VideoMode_(mode);
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

   video_register_width  = 8 * ROM[ROM_RESOLUTION + index];
   video_register_height = 8 * ROM[ROM_RESOLUTION + index + 1];
   video_register_mode   = mode;
   return res;
}
