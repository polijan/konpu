#ifndef  KONPU_ARCH_H_
#define  KONPU_ARCH_H_
#include "c.h"
#include "printer.h" //TODO: Remove this, but for now, it's useful for debugging

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

// Forms types ("Forms" are elements that may be Glyphs, Tiles, or Strips)
typedef uint8_t  Form8;
typedef uint16_t Form16;
typedef uint32_t Form32;
typedef uint64_t Form64;
typedef struct { uint64_t  top, bottom; } Form128;
typedef struct { uint64_t  top_left,    top_right,
                           bottom_left, bottom_right; } Form256;

// Attribute types (hold pen and/or paper color(s))
typedef uint8_t  Attribute8;
typedef struct { uint8_t pen, paper; } Attribute16;

/* TODO: MOVE/REMOVE
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

// Strip type (a byte containing packed pixels, see pixel.h)
typedef uint8_t  Strip; // Strip = 1,2,4,or 8 pixels packed in a byte
*/


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
   /* first 64 bits */
   Rectangle rectangle;  // the Window's geometry

   /* second 64 bits */
   uint16_t  x, y;       // current "cursor" coordinates
   uint8_t   pen, paper; // a pen and paper color
   uint8_t   px;         // location of a single pixel within the current Form
   uint8_t   planes;     // active planes

   /* thris 64 bits: properties (font, modes (bold/underline), wrappe (etc))*/
   uint8_t   font;       // font
   uint8_t   flags;
} Window;


//------------------------------------------------------------------------------
// Video card (see video.h)
//------------------------------------------------------------------------------

#define VIDEO_LCM_SZ_    2880 // 2^6 * 3^2 * 5
#ifndef VIDEO_FACTOR_      // Factor value may be can be changed in config.h
#define VIDEO_FACTOR_    4 // <-- default (implemented values: 2,3,4,5,6,7)
#endif
#define VIDEO_SIZE       (VIDEO_FACTOR_ * VIDEO_LCM_SZ_)

typedef struct {

   // Framebuffer
   union {
      // The raw framebuffer
      uint8_t    frame[VIDEO_SIZE];

      // Form elements
      // Note: To access an Element located at (x,y), it is possible to use
      // Video.<element>[x +y * VIDEO_WIDTH_<ELEMENT>], but it is easier and
      // more convenient to use the VIDEO_<ELEMENT>(...)[y][x] 2D arrays.
      Form8   form8[VIDEO_SIZE/1],    glyph8[VIDEO_SIZE/1] ,   strip[VIDEO_SIZE/1];
      Form16  form16[VIDEO_SIZE/2],   glyph16[VIDEO_SIZE/2],   tile16[VIDEO_SIZE/2];
      Form32  form32[VIDEO_SIZE/4],   glyph32[VIDEO_SIZE/4],   tile32[VIDEO_SIZE/4];
      Form64  form64[VIDEO_SIZE/8],   glyph64[VIDEO_SIZE/8],   tile64[VIDEO_SIZE/8];
      Form128 form128[VIDEO_SIZE/16], glyph128[VIDEO_SIZE/16], tile128[VIDEO_SIZE/16];
      Form256 form256[VIDEO_SIZE/32], glyph256[VIDEO_SIZE/32];

      // The fields below are simply to make sure the framebuffer contains the
      // C Attribute types. But for (raw) access to Attribute at (x,y), use the
      // VIDEO_ATTRIBUTE(N)[y][x] 2D array.
      Attribute8  DONT_USE_THIS_FIELD_Attribute8_[VIDEO_SIZE / 1];
      Attribute16 DONT_USE_THIS_FIELD_Attribute16_[VIDEO_SIZE / 2];

      // Names for convenient access when the framebuffer is divided into planes
      // (video elements are then either Glyphs or Strip8 (aka bit-Strips))
#     define VIDEO_DECLARE_PLANES_(N)                                    \
         union {                                                         \
            Form8   form8[VIDEO_SIZE/N], glyph8[VIDEO_SIZE/N],           \
                    strip[VIDEO_SIZE/N], strip8[VIDEO_SIZE/N];           \
            Form16  form16[VIDEO_SIZE/2/N],   glyph16[VIDEO_SIZE/2/N];   \
            Form32  form32[VIDEO_SIZE/4/N],   glyph32[VIDEO_SIZE/4/N];   \
            Form64  form64[VIDEO_SIZE/8/N],   glyph64[VIDEO_SIZE/8/N];   \
            Form128 form128[VIDEO_SIZE/16/N], glyph128[VIDEO_SIZE/16/N]; \
            Form256 form256[VIDEO_SIZE/32/N], glyph256[VIDEO_SIZE/32/N]; \
         } UTIL_CAT(planes, N) [N];
      VIDEO_DECLARE_PLANES_(1)
      VIDEO_DECLARE_PLANES_(2)
      VIDEO_DECLARE_PLANES_(3)
      VIDEO_DECLARE_PLANES_(4)
      VIDEO_DECLARE_PLANES_(5)
      VIDEO_DECLARE_PLANES_(6)
#     if (VIDEO_FACTOR_ % 7 == 0)
      VIDEO_DECLARE_PLANES_(7)
#     endif
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

   uint8_t      border;            // border color (always a full-color)

   // TODO: "stray" elements - is this needed???
   union { // stray Form
      Form8   stray_form8,   stay_glyph8,   stray_strip;
      Form16  stray_form16,  stay_glyph16,  stray_tile16;
      Form32  stray_form32,  stay_glyph32,  stray_tile32;
      Form64  stray_form64,  stay_glyph64,  stray_tile64;
      Form128 stray_form128, stay_glyph128, stray_tile128;
      Form256 stray_form256, stay_glyph256, stray_tile256;
   };
   union {
      uint8_t   stray_attr[2]; // stray Attribute
      struct { // also used as default pen/paper for Attributes with a single color
         uint8_t attr_default_pen;          // true color
         uint8_t attr_default_paper;        // true color
      };
   };

   // "Active" Rectangle / Window
   Rectangle active; // active area (Form)
   union {
      Rectangle active_form, active_glyph, active_tile, active_strip;
   };
   union {
      Rectangle active_attribute;  // only used in attribute mode
      int8_t    active_plane;      // only used in bit plane mode
   };
   Window   active_window;

   // Parameter related to the video layout.
   // They are declared as "const" to highlight one should not casually
   // modify them (without a proper understanding of the video framebuffer).
   // To change the video mode, use the `VideoMode()` function.
   const uint8_t   mode;        // video mode
   const int16_t   width;       // framebuffer's width  in PIXELS
   const int16_t   height;      // framebuffer's height in PIXELS

   // TODO: not used for now, just an idea to explore or Remove!
#if KONPU_MODEL < 86
   Rectangle UTIL_CAT(_reserved_for_future_models_, __LINE__);
#else
   // User may change it, but it will also be reset by `VideoMode()`
   Rectangle viewport;          // rendering viewport
#endif

   const uint32_t  errors;      // number of errors //TODO:remove?
} VideoSurface_;

//------------------------------------------------------------------------------
// Other (i.e. non-Video) sub-systems or peripherals
//------------------------------------------------------------------------------

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

   VideoSurface_ video;

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

   struct {
      int16_t x,y; // mouse coordinates (pixels!)
   } mouse;

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


//------------------------------------------------------------------------------
// Boot the system
//------------------------------------------------------------------------------

void Update(void);
void MouseUpdate(void);


// Reboot Konpu
void KonpuReboot(void);

// Switch on
#define KonpuTurnOn_(...)    KonpuReboot()

#define KonpuTurnOff_()      // nothing for now


#endif //include guard
