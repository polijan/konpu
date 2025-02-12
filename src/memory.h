#ifndef  KONPU_MEMORY_H
#define  KONPU_MEMORY_H
#include "c.h"


#define ADDRESS_AFTER(ADDRESS_NAME) \
   ((int32_t)(ADDRESS_NAME + ADDRESS_NAME##_CAP_))


// Expand to a few `static_assert` statements to ensure that the given address
// is a valid address in Konpu's RAM and is aligned with the given C type.
#define MEMORY_STATIC_ASSERT(ram_address, C_TYPENAME)                  \
        static_assert((ram_address) >= 0, "RAM address must be >= 0"); \
        static_assert((ram_address) < KONPU_MEMORY_SIZE,               \
                      "RAM address must be < KONPU_MEMORY_SIZE");      \
        static_assert((ram_address) % alignof(C_TYPENAME) == 0,        \
                      "RAM address alignement")


//------------------------------------------------------------------------------
// Konpu's ROM
//------------------------------------------------------------------------------
// A valid Konpu's ROM Address should be in the range [0, KONPU_ROM_SIZE[
// It fits in 14 bits.


// Notable Addresses in Konpu's ROM
#define KONPU_ROM_VERSION              0
#define KONPU_ROM_VERSION_CAP_         (1/*major*/ +1/*minor*/ +2/*patch*/)
#define KONPU_ROM_RESOLUTION           ADDRESS_AFTER(KONPU_ROM_VERSION)
#define KONPU_ROM_RESOLUTION_CAP_      (2 * 11/*possible resolutions*/)
#define KONPU_ROM_COLOR                ADDRESS_AFTER(KONPU_ROM_RESOLUTION)
#define KONPU_ROM_COLOR_CAP_           (256 /*colors*/ * 8 /*Lab + kd + RGB*/)
#define KONPU_ROM_PALETTE              ADDRESS_AFTER(KONPU_ROM_COLOR)
#define KONPU_ROM_PALETTE_CAP_         128
#define KONPU_ROM_WAV_HEADER           ADDRESS_AFTER(KONPU_ROM_PALETTE)
#define KONPU_ROM_WAV_HEADER_CAP_      44
#define KONPU_ROM_URL                  ADDRESS_AFTER(KONPU_ROM_WAV_HEADER)
#define KONPU_ROM_URL_CAP_             64

// Konpu's ROM
#define KONPU_ROM_SIZE  ADDRESS_AFTER(KONPU_ROM_URL)
extern const uint8_t KonpuROM[KONPU_ROM_SIZE];


//------------------------------------------------------------------------------
// Konpu's RAM
//
// A valid Konpu's (RAM) Memory Address should be in the range
// [0, KONPU_MEMORY_SIZE[. It fits in 20 bits.
//------------------------------------------------------------------------------

// Size in bytes of Konpu's RAM memory.
#define KONPU_MEMORY_SIZE  1048576   // (for now 2^20 bit, ie a full megabyte)

// Global byte array that serves as Konpu's RAM memory
extern  alignas(max_align_t)
#if defined(__GNUC__)
//      C allows accessing of all objects as bytes, but the other way around is
//      UB (strict aliasing). However, it is common practice to use byte arrays
//      to store other objects and most compilers simply allow this. There is a
//      proposal for C to formally allow the access of byte arrays as other
//      types, see: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3254.pdf
//
//      Here, for GCC/clang, specifically mark that strict aliasing should not
//      apply:
        __attribute__((__may_alias__))
#endif
uint8_t KonpuMemory[KONPU_MEMORY_SIZE];




static inline int32_t AddressOf(void *pointer)
{
   assert(pointer != NULL);
   int32_t addr = (uint8_t*)pointer - KonpuMemory;
   // assert(addr >= 0 && addr < KONPU_MEMORY_SIZE);
   return addr;
}


//------------------------------------------------------------------------------
// Video Framebuffer
//------------------------------------------------------------------------------
// The Video Framebuffer encodes the screen. It can be set in different video
// modes, which determine the resolution of the display and how memory stores
// the graphical information. Here's how the video system fits in Konpu's RAM
//
// VIDEO_SIZE = amount of memory in bytes in the video framebuffer
// VIDEO_FRAMEBUFFER = the video framebuffer memory
// VIDEO_RENDER_ERRORS = number of errors when rendering the framebuffer
// VIDEO_WIDTH = width in pixels of the framebuffer  (depends on the video mode)
// VIDEO_HEIGHT = height in pixels of the framebuffer(depends on the video mode)
// VIDEO_MODE = control the layout of the framebuffer (see video.h)
//------------------------------------------------------------------------------
#define VIDEO_SIZE_LCM_                  2880 // (see: tools/resfinder)
#ifndef VIDEO_SIZE_FACTOR_
#   define VIDEO_SIZE_FACTOR_            4    // implemented: 7,6,5,4,3,2
#endif
#define VIDEO_SIZE                       (VIDEO_SIZE_FACTOR_ * VIDEO_SIZE_LCM_)
//
#define VIDEO_FRAMEBUFFER_ADDRESS        0
#define VIDEO_FRAMEBUFFER_ADDRESS_CAP_   VIDEO_SIZE
#define VIDEO_RENDER_ERRORS_ADDRESS      ADDRESS_AFTER(VIDEO_FRAMEBUFFER_ADDRESS)
#define VIDEO_RENDER_ERRORS_ADDRESS_CAP_ sizeof(uint32_t)
#define VIDEO_WIDTH_ADDRESS              ADDRESS_AFTER(VIDEO_RENDER_ERRORS_ADDRESS)
#define VIDEO_WIDTH_ADDRESS_CAP_         sizeof(int16_t)
#define VIDEO_HEIGHT_ADDRESS             ADDRESS_AFTER(VIDEO_WIDTH_ADDRESS)
#define VIDEO_HEIGHT_ADDRESS_CAP_        sizeof(int16_t)
#define VIDEO_MODE_ADDRESS               ADDRESS_AFTER(VIDEO_HEIGHT_ADDRESS)
#define VIDEO_MODE_ADDRESS_CAP_          1

//------------------------------------------------------------------------------
// Colors
//------------------------------------------------------------------------------
// ilo Konpu knows 256 colors (whose sRGB colors are defined in ROM).
// While certain video modes allow to use all those colors, many other modes
// only display less colors (often 2, 4, 16, or (using bitplane modes) 8, 32,
// 64, or even 128 colors) which are index to the full 256 colors via a palette.
//
// COLOR_BORDER = color of the border possibly displayed around the framebuffer
// COLOR_DEFAULT_FG = default "pen" color
// COLOR_DEFAULT_BG = default "paper" color
// COLOR_PALETTE = palette area
//------------------------------------------------------------------------------
#define COLOR_BORDER_ADDRESS             ADDRESS_AFTER(VIDEO_MODE_ADDRESS)
#define COLOR_BORDER_ADDRESS_CAP_        1
#define COLOR_DEFAULT_FG_ADDRESS         ADDRESS_AFTER(COLOR_BORDER_ADDRESS)
#define COLOR_DEFAULT_FG_ADDRESS_CAP_    1
#define COLOR_DEFAULT_BG_ADDRESS         ADDRESS_AFTER(COLOR_DEFAULT_FG_ADDRESS)
#define COLOR_DEFAULT_BG_ADDRESS_CAP_    1
#define COLOR_PALETTE_ADDRESS            ADDRESS_AFTER(COLOR_DEFAULT_BG_ADDRESS)
#define COLOR_PALETTE_ADDRESS_CAP_       128

//------------------------------------------------------------------------------
// Keyboard
// KEY_STATE: the state of each key on the keyboard (see keyboard.h)
// KEY_PREVIOUS: a copy of the keyboard state prior to the previous update.
//------------------------------------------------------------------------------
#define KEY_MOD_ADDRESS             ADDRESS_AFTER(COLOR_PALETTE_ADDRESS)
#define KEY_MOD_ADDRESS_CAP_        4 // make sure to pad so that
                                    // KEY_STATE_ADDRESS is aligned for uint64s
#define KEY_STATE_ADDRESS           ADDRESS_AFTER(KEY_MOD_ADDRESS)
#define KEY_STATE_ADDRESS_CAP_      32 // 256 key states stored as bits
#define KEY_PREVIOUS_ADDRESS        ADDRESS_AFTER(KEY_STATE_ADDRESS)
#define KEY_PREVIOUS_ADDRESS_CAP_   KEY_STATE_ADDRESS_CAP_


//------------------------------------------------------------------------------
// The Stack
//------------------------------------------------------------------------------
// The "Stack" is a piece of Konpu's memory used to implement a stack
// datastructure which is the primary way the Waso language manipulates
// data. Here's its memory layout:
//
//                            &STACK_SIZE
//      .-- STACK_VAR (var*)        |          .-- STACK_TYPE
//      v                           v          v   (Type*)
//
//  ... | STACK_CAPACITY x 64bits   | 16-bits  | STACK_CAPACITY x 16bits | ...
//      |---------------------------|----------|-------------------------|
//      | Waso vars                 | current  | types of the vars       |
//      |                           |stack size| in STACK_VAR            |
//------------------------------------------------------------------------------
#define STACK_CAPACITY                   2047 // max number of stack elements
//
#define STACK_VAR_ADDRESS                69632  // TODO:address at 68k for now...
#define STACK_VAR_ADDRESS_CAP_           (STACK_CAPACITY * sizeof(var))
#define STACK_SIZE_ADDRESS               ADDRESS_AFTER(STACK_VAR_ADDRESS)
#define STACK_SIZE_ADDRESS_CAP_          sizeof(int16_t)
#define STACK_TYPE_ADDRESS               ADDRESS_AFTER(STACK_SIZE_ADDRESS)
#define STACK_TYPE_ADDRESS_CAP_          (STACK_CAPACITY * sizeof(Type))

//------------------------------------------------------------------------------
// The Heap
//------------------------------------------------------------------------------
// The "Heap" is a piece of 512Kb memory for programs to allocate dynamically.
// Waso vars whose value doesn't fit in 64 bits use heap storage. In addition to
// the main memory block, extra space is also needed for tracking which heap
// blocks are checked out and the size of each block.
//------------------------------------------------------------------------------
#define HEAP_ADDRESS                     ADDRESS_AFTER(STACK_TYPE_ADDRESS)
#define HEAP_ADDRESS_CAP_                524288  // 512 Kilobytes
#define HEAP_CTRL_ADDRESS                ADDRESS_AFTER(HEAP_ADDRESS)
#define HEAP_CTRL_ADDRESS_CAP_           65536   //  64 Kilobytes


// Here be dragons...
#define HEAP_CTRL_OVERFLOW_ADDRESS       ADDRESS_AFTER(HEAP_CTRL_ADDRESS)


#endif //KONPU_MEMORY_H
