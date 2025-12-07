#ifndef  KONPU_ARCH_H_
#define  KONPU_ARCH_H_
#include "c.h"

// Konpu Version - We aren't there yet, but we eventually want to follow a
// "semantic version" scheme, where:
// - MAJOR version: incompatible API changes
// - MINOR version: added functionality in a backward compatible manner
// - PATCH version: backward compatible fixes

#ifndef KONPU_MODEL
// TODO: This is an idea for the future, maybe remove this entirely.
#define KONPU_MODEL 86
#endif

#define KONPU_VERSION_MAJOR  0
#define KONPU_VERSION_MINOR  0
#define KONPU_VERSION_PATCH  1

//------------------------------------------------------------------------------
// Essential Types: Video Elements
//------------------------------------------------------------------------------

struct GlyphOrTile128 { uint64_t top, bottom; };
struct GlyphOrTile256 { uint64_t top_left,    top_right,
                                 bottom_left, bottom_right; };

// Glyph types (2D bitmaps, see glyph.h)
typedef void                  Glyph;    // <-- TODO: remove?
typedef uint8_t               Glyph8;   // Glyph  2x4  (bit) pixels
typedef uint16_t              Glyph16;  // Glyph  4x4  (bit) pixels
typedef uint32_t              Glyph32;  // Glyph  4x8  (bit) pixels
typedef uint64_t              Glyph64;  // Glyph  8x8  (bit) pixels
typedef struct GlyphOrTile128 Glyph128; // Glyph  8x16 (bit) pixels
typedef struct GlyphOrTile256 Glyph256; // Glyph 16x16 (bit) pixels

// Tile types ("chunky" Glyphs, see tile.h)
typedef uint16_t              Tile16;  // Tile 2x4 4c
typedef uint32_t              Tile32;  // Tile 2x4 16c  or 4x4 4c
typedef uint64_t              Tile64;  // Tile 2x4 256c or 4x4 16c or 4x8 4c
typedef struct GlyphOrTile128 Tile128; // Tile 4x4 256c or 4x8 16c or 8x8 4c
typedef struct GlyphOrTile256 Tile256; // Tile 4x8 256c or 8x16 4c or 8x8 16c


// Strip type (packed pixels byte, see pixel.h)
typedef uint8_t  Strip; // Strip = 1,2,4,or 8 pixels packed in a byte

// Attribute types (hold pen and/or paper color(s), see attribute.h)
typedef uint8_t  Attribute8;
typedef struct { uint8_t pen, paper; } Attribute16;


// Hold information (64-bits) about the Konpu 256 possible colors
typedef struct ColorInfo {
   // Components in OkLab color space (transformed from [0,1] to 8-bits):
   uint8_t L_half;      //<  L = 2 * L_half        / 510
   int8_t  a_minus_12;  //<  a = (a_minus_12 + 12) / 510
   int8_t  b_plus_30;   //<  b = (b_plus_30 + 30)  / 510
   // Color distance to other colors encoded as a Kd-tree:
   uint8_t kd_left, kd_right;
   // Components in sRGB color space (gamma-encoded, 8-bits):
   uint8_t r, g, b;
} ColorInfo;

// Geometry types

// Rectangle in absolute coordinates (0,0) is screen top left
typedef struct Rectangle {
   int16_t x0, y0;        // top-left point of the rectangle
   int16_t width, height; // width and height (always >=0)
} Rectangle;


typedef struct Window {
   Rectangle geometry;    // the windows' geometry

   // Window's current "cursor"
   uint16_t  x, y;        // cursor coordinates
   uint8_t   bit_pixel;   // glyph/tiles only: active bit can indicate a pixel

   // pen and paper color
   uint8_t   pen, paper;

   // (text) properties
   // TODO: font, and other props (bold, etc), etc.

   // note:
   // geometry's rectangle occupies the first 64 bits,
   // cursor and pen/paper occupies the second 64 bits,
   // properties occupy... the rest of the struct Window.
} Window;

//------------------------------------------------------------------------------
// Video card
//------------------------------------------------------------------------------

#define VIDEO_LCM_SZ_    2880 // 2^6 * 3^2 * 5
#ifndef VIDEO_FACTOR_      // Factor value may be can be changed in config.h
#define VIDEO_FACTOR_    4 // <-- default (implemented values: 2,3,4,5,6,7)
#endif
#define VIDEO_SIZE       (VIDEO_FACTOR_ * VIDEO_LCM_SZ_)


struct _VideoMemory {

   // Framebuffer
   union {
      // Raw framebuffer
      uint8_t     frame[VIDEO_SIZE];

      // Names for convenient Glyph access:
      // The Glyph<N> at (x,y) is located at: Video.glyph<N>[x + y * VIDEO_WIDTH_GLYPH<N>]
      // (But, it is simpler tho use VIDEO_GLYPH(N)[y][x])
      Glyph8      glyph8  [VIDEO_SIZE / 1];
      Glyph16     glyph16 [VIDEO_SIZE / 2];
      Glyph32     glyph32 [VIDEO_SIZE / 4];
      Glyph64     glyph64 [VIDEO_SIZE / 8];
      Glyph128    glyph128[VIDEO_SIZE / 16];
      Glyph256    glyph256[VIDEO_SIZE / 32];

      // Names for convenient Tile access
      // The Tile<N> at (x,y) is located at: Video.tile<N>[x + y * VIDEO_WIDTH_TILE<N>]
      // (But, it is simpler tho use VIDEO_TILE(N)[y][x])
      Tile16      tile16 [VIDEO_SIZE / 2];
      Tile32      tile32 [VIDEO_SIZE / 4];
      Tile64      tile64 [VIDEO_SIZE / 8];
      Tile128     tile128[VIDEO_SIZE / 16];
      Tile256     tile256[VIDEO_SIZE / 32];

      // Names for convenient Strip access
      // (But, it is simpler tho use VIDEO_STRIP()[y][x])
      Strip       strip[VIDEO_SIZE / 1];

      // Name specifically for the full color pixel mode (mode 123)
      // In that mode you can access the pixel (x,y) with: Video.pixel[x + y * Video.width]
      uint8_t     pixel[VIDEO_SIZE];

      // Just to make sure it can contain this type
      Attribute8  DO_NOT_USE_THIS_FIELD_attr8_[VIDEO_SIZE / 1];
      Attribute16 DO_NOT_USE_THIS_FIELD_attr16_[VIDEO_SIZE / 2];

      // Names for convenient access when the framebuffer is divided into planes
      // (video elements are then either Glyphs or Strip8 (aka bit pixels))
#     define VIDEO_DECLARE_PLANES_(N)              \
      union {                                      \
         Strip     strip8   [VIDEO_SIZE / 1  / N]; \
         Glyph8    glyph8   [VIDEO_SIZE / 1  / N]; \
         Glyph16   glyph16  [VIDEO_SIZE / 2  / N]; \
         Glyph32   glyph32  [VIDEO_SIZE / 4  / N]; \
         Glyph64   glyph64  [VIDEO_SIZE / 8  / N]; \
         Glyph128  glyph128 [VIDEO_SIZE / 16 / N]; \
         Glyph256  glyph256 [VIDEO_SIZE / 32 / N]; \
      } UTIL_CAT(planes, N) [N];
      VIDEO_DECLARE_PLANES_(1)
      VIDEO_DECLARE_PLANES_(2)
      VIDEO_DECLARE_PLANES_(3)
      VIDEO_DECLARE_PLANES_(4)
      VIDEO_DECLARE_PLANES_(5)
      VIDEO_DECLARE_PLANES_(6)
#if (VIDEO_FACTOR_ % 7 == 0)
      VIDEO_DECLARE_PLANES_(7)
#endif
      VIDEO_DECLARE_PLANES_(8)
#     undef  VIDEO_DECLARE_PLANES_

   };

      // Current palettes.
      // Note: palette of n=2,4,6,8,16,32,64 colors starts at: Video.palettes + n
      union {
         uint8_t     palettes  [128]; // the palettes whole-area, which
         uint8_t     palette128[128]; // is also the 128-color palette.
         struct {
            uint8_t  palette128_0_;   // = Ram.video.palette128[0]
            uint8_t  palette128_1_;   // = Ram.video.palette128[1]
            uint8_t  palette2  [ 2];  // the  2-color palette
            uint8_t  palette4  [ 4];  // the  4-color palette
            uint8_t  palette8  [ 8];  // the  8-color palette
            uint8_t  palette16 [16];  // the 16-color palette
            uint8_t  palette32 [32];  // the 32-color palette
            uint8_t  palette64 [64];  // the 64-color palette
         };
      };

      // TODO: finalize what we keep here:
      // Out of bound parameters: border color and "stray" elements
      uint8_t      border;            // border color (true color)
      union {                         // stray Attributes parameters
         uint8_t   stray_attr[2];
         struct {
            // is also used as default pen/paper
            // when attributes' color type is FG256 or G256
            uint8_t attr_default_pen;          // true color
            uint8_t attr_default_paper;        // true color
         };
      };
      union {  // stray Glyph, Tile, or Strip
         Strip     stray_strip;
         Glyph8    stray_glyph8;
         Glyph16   stray_glyph16;
         Glyph32   stray_glyph32;
         Glyph64   stray_glyph64;
         Glyph128  stray_glyp128;
         Glyph256  stray_glyp256;
      };

   // Active Window
   Window   active_window;

   // Parameter related to the video layout.
   // They are declared as "const" to highlight one should not casually
   // modify them (without a proper understanding of the video framebuffer).
   // To change the video mode, use the `VideoMode()` function.
   const uint8_t   mode;        // video mode
   const int16_t   width;       // framebuffer's width in pixels
   const int16_t   height;      // framebuffer's height in pixels

   // TODO: not used for now, just an idea to explore or Remove!
#if KONPU_MODEL < 86
   Rectangle UTIL_CAT(_reserved_for_future_models_, __LINE__);
#else
   // User may change it, but it will also be reset by `VideoMode()`
   Rectangle viewport;          // rendering viewport
#endif

   const uint32_t  errors;      // number of errors //TODO:remove?
};


//------------------------------------------------------------------------------
// Sub-systems / Peripherals
//------------------------------------------------------------------------------

// Videocard parameters (see: tools/resfinder)
#define Video                Ram.video

// Soundcard parameters
#define Audio                Ram.audio

// Keyboard interface parameters
#define Keyboard             Ram.keyboard
#define KEY_COUNT            256 // number of keys we track

// Waso Stack parameters
#define STACK_CAPACITY       2047 // max number of stack elements

// Var Pool parameters
// ...

//------------------------------------------------------------------------------
// The Rom
//------------------------------------------------------------------------------

struct ROM_ {
   uint8_t version[1/*major*/ +1/*minor*/ +2/*patch*/];

   // Possible Framebuffer Resolutions (width and height in 8x8 cells)
   uint8_t resolution_8x8[2 * 11/*# possible resolutions*/];

   // Information about the 256 existing Konpu colors
   ColorInfo color[256];

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

   // Default font data (stored in bytes with little endian)
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

// Global ROM
extern struct ROM_ Rom;

// Return the address in ROM based on a named <member>
static_assert(sizeof(struct ROM_) <= INT16_MAX);
#define ROM_ADDRESS_OF(MEMBER)     ((int16_t)offsetof(struct ROM_, MEMBER))

//------------------------------------------------------------------------------
// The RAM
//------------------------------------------------------------------------------

struct RAM_
{
   // Memory-mapped I/O systems

   struct _VideoMemory video;

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

// Return the address in RAM or ROM based on a named <member>
static_assert(sizeof(struct RAM_) <= INT32_MAX);
#define RAM_ADDRESS_OF(MEMBER)     ((int32_t)offsetof(struct RAM_, MEMBER))


#endif //include guard
