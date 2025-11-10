#ifndef  KONPU_ARCH_H_
#define  KONPU_ARCH_H_
#include "c.h"

// Konpu Version - We aren't there yet, but we eventually want to follow a
// "semantic version" scheme, where:
// - MAJOR version: incompatible API changes
// - MINOR version: added functionality in a backward compatible manner
// - PATCH version: backward compatible fixes
#define KONPU_VERSION_MAJOR  0
#define KONPU_VERSION_MINOR  0
#define KONPU_VERSION_PATCH  1

//------------------------------------------------------------------------------
// Essential Types
//------------------------------------------------------------------------------

// Glyphs (see glyph.h for operations)

typedef void      Glyph;
typedef uint8_t   Glyph8;
typedef uint16_t  Glyph16;
typedef uint32_t  Glyph32;
typedef uint64_t  Glyph64;
typedef struct {
   Glyph64  top,
            bottom;
} Glyph128;
typedef struct {
   Glyph64  top_left,    top_right,
            bottom_left, bottom_right;
} Glyph256;







/* TODO: Does it make sense to use C23's fixed-bits integers _BintInt() ?

typedef union {
   struct {
      Glyph64  top,
               bottom;
   };
#if BITINT_MAXWIDTH >= 128 // may or may not exist
   unsigned _BitInt(128)  bits_;
#ifdef __SIZEOF_INT128__   // may or may not exist
   unsigned __int128      bits_;
#endif
} Glyph128;

typedef union {
   struct {
      Glyph64  top_left,    top_right,
               bottom_left, bottom_right;
   };
#if BITINT_MAXWIDTH >= 256 // may or may not exist
   unsigned _BitInt(256)  bits_;
#endif
} Glyph256;
*/

//------------------------------------------------------------------------------
// Fundamental Types
//------------------------------------------------------------------------------

// Videocard parameters (see: tools/resfinder)
#define VIDEO_FACTOR_        4 // implemented: 2,3,4,5,6,7
#define VIDEO_LCM_SZ_        2880 // 2^6 * 3^2 * 5
#define VIDEO_SIZE           (VIDEO_FACTOR_ * VIDEO_LCM_SZ_)
#define Video                Ram.video

// Soundcard parameters
#define Audio                Ram.audio

// Keyboard interface parameters
#define Keyboard             Ram.keyboard
#define KEY_COUNT            256 // number of keys we track

// Waso Stack parameters
#define STACK_CAPACITY       2047 // max number of stack elements

// Var Pool parameters


//------------------------------------------------------------------------------
// The Rom
//------------------------------------------------------------------------------

#define ROM_ADDRESS_OF(MEMBER)     ((int32_t)offsetof(struct ROM_, MEMBER))

struct ROM_ {
   uint8_t version[1/*major*/ +1/*minor*/ +2/*patch*/];

   // Possible framebuffer Resolutions (size in 8x8 cells)
   uint8_t resolution_8x8[2 * 11/*# possible resolutions*/];

   // Definition of Konpu colors
   uint8_t color_info[256/*colors*/ * 8/*Lab + kd + RGB*/];
   /* TODO: replace the color_info byte array with that more explicit structure:
   struct RomColorInfo_ {
      uint8_t L_half;
      int8_t  a_minus_12;
      int8_t  b_plus_30;
      uint8_t kd_left, kd_right; // kd-tree for fast color search
      uint8_t r, g, b;           // color's sRGB components
   } color_info[256];
   */

   // Default palettes (mirror the similar structure in Ram)
   union {
      uint8_t     default_palettes  [128];
      uint8_t     default_palette128[128];
      struct {
         uint8_t  default_palette128_0_;
         uint8_t  default_palette128_1_;
         uint8_t  default_palette2  [ 2];
         uint8_t  default_palette4  [ 4];
         uint8_t  default_palette8  [ 8];
         uint8_t  default_palette16 [16];
         uint8_t  default_palette32 [32];
         uint8_t  default_palette64 [64];
      };
   };

   struct {
      uint8_t ascii4[95/*printable ascii chars*/ * 2/*sizeof Glyph16*/];
      uint8_t ascii5[95/*printable ascii chars*/ * 4/*sizeof Glyph32*/];
      uint8_t ascii6[95/*printable ascii chars*/ * 4/*sizeof Glyph32*/];
      uint8_t ascii7[95/*printable ascii chars*/ * 4/*sizeof Glyph32*/];
      uint8_t tokipona5[123/*pu words*/ * 8/*sizeof Glyph64*/];
      uint8_t tokipona6[123/*pu words*/ * 8/*sizeof Glyph64*/];
      uint8_t tokipona7[123/*pu words*/ * 8/*sizeof Glyph64*/];
   } font;

   uint8_t wav_header[44];
   uint8_t url[64];
};

extern struct ROM_ Rom;

//------------------------------------------------------------------------------
// The RAM
//------------------------------------------------------------------------------

// Return the address in RAM or ROM based on a named <member>
#define RAM_ADDRESS_OF(MEMBER)     ((int32_t)offsetof(struct RAM_, MEMBER))

struct RAM_
{
   // Memory-mapped I/O systems

   struct {
      union {
         uint8_t   frame[VIDEO_SIZE];  // 00000  video framebuffer

         // Names for convenient glyph access in glyph modes
         // The glyph<n> at (x,y) is located at:
         // Video.glyph<n>[x + y * VIDEO_WIDTH_GLYPH<n>]
         //                         '--> = Video.width >> GLYPH<n>_WIDTH_LOG2
         Glyph8    glyph8  [VIDEO_SIZE];
         Glyph16   glyph16 [VIDEO_SIZE / 2];
         Glyph32   glyph32 [VIDEO_SIZE / 4];
         Glyph64   glyph64 [VIDEO_SIZE / 8];
         Glyph128  glyph128[VIDEO_SIZE / 16];
         Glyph256  glyph256[VIDEO_SIZE / 32];

         // Names for convenient pixel bytes access in pixel modes:
         union {
            uint8_t  bits;      // 8x1   on/off pixels
            uint8_t  quarters;  // 4x1  4-color pixels
            uint8_t  nibbles;   // 2x1 16-color pixels
            uint8_t  byte;      // single 256-color pixel
         } pixel_byte[VIDEO_SIZE];
         // full 256-color pixel mode (aka mode 123)
         // (just a more convenient name than .pixel_byte.byte)
         uint8_t pixel[VIDEO_SIZE];

         // Names for convenient access when video frames is divided into
         // planes (planes may contain glyphs or bit pixels)
#        define RAM_VIDEO_PLANES_(N)                  \
         union {                                      \
            uint8_t   pixels8x1[VIDEO_SIZE      / N]; \
            Glyph8    glyph8   [VIDEO_SIZE      / N]; \
            Glyph16   glyph16  [VIDEO_SIZE / 2  / N]; \
            Glyph32   glyph32  [VIDEO_SIZE / 4  / N]; \
            Glyph64   glyph64  [VIDEO_SIZE / 8  / N]; \
            Glyph128  glyph128 [VIDEO_SIZE / 16 / N]; \
            Glyph256  glyph256 [VIDEO_SIZE / 32 / N]; \
         } UTIL_CAT(planes, N) [N];

         //
         RAM_VIDEO_PLANES_(1)
         RAM_VIDEO_PLANES_(2)
         RAM_VIDEO_PLANES_(3)
         RAM_VIDEO_PLANES_(4)
         RAM_VIDEO_PLANES_(5)
         RAM_VIDEO_PLANES_(6)
#if (VIDEO_FACTOR_ % 7 == 0)
         RAM_VIDEO_PLANES_(7)
#endif
         RAM_VIDEO_PLANES_(8)
#        undef  RAM_VIDEO_PLANES_
      };

      // Current palettes.
      // Note: palette of n=2,4,6,8,16,32,64 colors starts at: Video.palettes + n
      union {
         uint8_t     palettes  [128]; // .....  the palettes whole-area, which
         uint8_t     palette128[128]; //        is also the 128-color palette.
         struct {
            uint8_t  palette128_0_;   // .....  = Ram.video.palette128[0]
            uint8_t  palette128_1_;   // .....  = Ram.video.palette128[1]
            uint8_t  palette2  [ 2];  // .....  the  2-color palette
            uint8_t  palette4  [ 4];  // .....  the  4-color palette
            uint8_t  palette8  [ 8];  // .....  the  8-color palette
            uint8_t  palette16 [16];  // .....  the 16-color palette
            uint8_t  palette32 [32];  // .....  the 32-color palette
            uint8_t  palette64 [64];  // .....  the 64-color palette
         };
      };

      // Color parameters
      uint8_t  border;             // .....  border color (true color)
      uint8_t  default_pen;        // .....  default pen
      uint8_t  default_paper;      // .....  default paper

      // Parameter related to the video layout.
      // They are declared as "const" to highlight one should not casually
      // modify them (without a proper understanding of the video framebuffer).
      // To change the video mode, use the `VideoMode()` function.
      const uint8_t   mode;        // ..... video mode
      const int16_t   width;       // ..... video width
      const int16_t   height;      // ..... video height
      const uint32_t  errors;      // ..... number of errors //TODO:remove?
   } video;

   struct {
      uint8_t  samples[128];       // .....  PCM samples to play
   } audio;

   struct {
      // The keyboard current state: 256 bits, each representing the state of
      // keys (1=pressed, 0=not pressed)
      uint8_t  mod;                // .....  8-bits for the modifier keys
      union {                      // .....  256-bits for current key presses
         uint8_t  current[KEY_COUNT/8]; //        (as bytes or as uint64s)
         uint64_t current_u64[KEY_COUNT/64];
      };
      // Copy of the keyboard state prior to the last tick
      union {                      // .....  256-bits for previous keys presses
         uint8_t  previous[KEY_COUNT/8]; //        (as bytes or as uint64s)
         uint64_t previous_u64[KEY_COUNT/64];
      };
   } keyboard;

   // System RAM

   struct RAM_RNG_ {
      uint64_t state;   // RNG state.  All values are possible.
      uint64_t inc;     // Controls which RNG sequence (stream) is selected.
                        // Must *always* be odd.
   } rng;

   struct {
      uint64_t lvl[STACK_CAPACITY]; // .....  stack slots
      int16_t  type[STACK_CAPACITY];
      int16_t  size;                // .....  stack size
   } stack;

   struct {
      uint64_t slots[16384];       // .....  64K pool (16K var slots)
      uint64_t free_list;          // .....  start of the pool's free list
   } pool;
   struct {
      uint8_t  memory[524288];     // .....  512K Heap memory
      uint8_t  ctrl[65536];        // .....  Heap control blocks (64K)
   } heap;

   // System Memory

   union {
      uint8_t   generic[1 << 16];
      uintmax_t generic_ensure_alignment_;
   };
}
#if defined(__GNUC__)
//  C allows accessing of all objects as bytes, but the other way around is UB
//  (strict aliasing). However, it is common practice to use byte arrays to
//  store other objects and most compilers simply allow this. There is a
//  proposal for C to formally allow the access of byte arrays as other types,
//  see: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3254.pdf
//
//  And when using GCC/clang, we can specifically mark that strict aliasing
//  should not apply to the RAM struct.
   __attribute__((__may_alias__))
#endif
;

// Global Ram object
extern alignas(max_align_t) struct RAM_ Ram;


#endif //include guard
