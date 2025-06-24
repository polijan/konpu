#ifndef  KONPU_RAM_H_
#define  KONPU_RAM_H_
#include "c.h"
#include "video_mode.h"
#define STACK_CAPACITY              2047 // max number of stack elements


// RAM memory size in bytes
// Valid RAM Addresses are in the range [0, RAM_SIZE[; they fits in 20 bits.
#define RAM_SIZE                    1048576  // (2^20 bits, i.e. one megabyte)

// The RAM (as a byte array)
extern alignas(max_align_t)
#if defined(__GNUC__)
//  C allows accessing of all objects as bytes, but the other way around is UB
//  (strict aliasing). However, it is common practice to use byte arrays to
//  store other objects and most compilers simply allow this. There is a
//  proposal for C to formally allow the access of byte arrays as other types,
//  see: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3254.pdf
//
//  Here, for GCC/clang, specifically mark that strict aliasing should not apply
    __attribute__((__may_alias__))
#endif
uint8_t RAM[RAM_SIZE];


#define RAM_AFTER_(ADDRESS)         ((int32_t)(ADDRESS + ADDRESS##_SZ_))

// List of starting addresses of each section in the RAM
#define RAM_VIDEO_BUFFER            0
#define RAM_COLOR_BORDER            RAM_AFTER_(RAM_VIDEO_BUFFER)
#define RAM_COLOR_DEFAULT_FG        RAM_AFTER_(RAM_COLOR_BORDER)
#define RAM_COLOR_DEFAULT_BG        RAM_AFTER_(RAM_COLOR_DEFAULT_FG)
#define RAM_COLOR_PALETTE           RAM_AFTER_(RAM_COLOR_DEFAULT_BG)
#define RAM_KEY_MOD                 RAM_AFTER_(RAM_COLOR_PALETTE)
#define RAM_KEY_CURRENT_STATE       RAM_AFTER_(RAM_KEY_MOD)
#define RAM_KEY_PREVIOUS_STATE      RAM_AFTER_(RAM_KEY_CURRENT_STATE)
#define RAM_RNG_STATE               RAM_AFTER_(RAM_KEY_PREVIOUS_STATE)
#define RAM_STACK_VAR               69632  // TODO:address at 68k for now...
#define RAM_STACK_SIZE              RAM_AFTER_(RAM_STACK_VAR)
#define RAM_STACK_TYPE              RAM_AFTER_(RAM_STACK_SIZE)
#define RAM_HEAP                    RAM_AFTER_(RAM_STACK_TYPE)
#define RAM_HEAP_CTRL               RAM_AFTER_(RAM_HEAP)
#define RAM_GENERIC                 RAM_AFTER_(RAM_HEAP_CTRL)

// Size in bytes of each section of the ROM
#define RAM_VIDEO_BUFFER_SZ_        VIDEO_SIZE
#define RAM_COLOR_BORDER_SZ_        1
#define RAM_COLOR_DEFAULT_FG_SZ_    1
#define RAM_COLOR_DEFAULT_BG_SZ_    1
#define RAM_COLOR_PALETTE_SZ_       128
#define RAM_KEY_MOD_SZ_             5 // <--- TODO/UGLY/FIX.... I pad so that RAM_KEY_STATE is aligned.
#define RAM_KEY_CURRENT_STATE_SZ_   (256/*key states*/ / 8 /*stored as bits*/)
#define RAM_KEY_PREVIOUS_STATE_SZ_  RAM_KEY_CURRENT_STATE_SZ_
#define RAM_RNG_STATE_SZ_           (2*sizeof(uint64_t)) //sizeof pcg32_random_t
#define RAM_STACK_VAR_SZ_           (STACK_CAPACITY * sizeof(uint64_t /*var*/))
#define RAM_STACK_SIZE_SZ_          sizeof(int16_t)
#define RAM_STACK_TYPE_SZ_          (STACK_CAPACITY * sizeof(uint16_t /*Type*/))
#define RAM_HEAP_SZ_                524288  // 512 Kilobytes
#define RAM_HEAP_CTRL_SZ_           65536   //  64 Kilobytes
#define RAM_GENERIC_SZ_             (RAM_SIZE - RAM_GENERIC)

#endif //include guard
