#ifndef  KONPU_ROM_H_
#define  KONPU_ROM_H_
#include "c.h"

#define ROM_AFTER_(ADDRESS)      ((int32_t)(ADDRESS + ADDRESS##_SZ_))

// List of starting addresses of each section in the ROM
#define ROM_VERSION              0
#define ROM_RESOLUTION           ROM_AFTER_(ROM_VERSION)
#define ROM_COLOR                ROM_AFTER_(ROM_RESOLUTION)
#define ROM_PALETTE              ROM_AFTER_(ROM_COLOR)
#define ROM_FONT                 ROM_FONT_ASCII4
#define ROM_FONT_ASCII4          ROM_AFTER_(ROM_PALETTE)
#define ROM_FONT_ASCII5          ROM_AFTER_(ROM_FONT_ASCII4)
#define ROM_FONT_ASCII6          ROM_AFTER_(ROM_FONT_ASCII5)
#define ROM_FONT_ASCII7          ROM_AFTER_(ROM_FONT_ASCII6)
#define ROM_FONT_TOKIPONA5       ROM_AFTER_(ROM_FONT_ASCII7)
#define ROM_FONT_TOKIPONA6       ROM_AFTER_(ROM_FONT_TOKIPONA5)
#define ROM_FONT_TOKIPONA7       ROM_AFTER_(ROM_FONT_TOKIPONA6)
#define ROM_WAV_HEADER           ROM_AFTER_(ROM_FONT_TOKIPONA7)
#define ROM_URL                  ROM_AFTER_(ROM_WAV_HEADER)

// Size of the whole ROM (Valid ROM Addresses are in the range [0, ROM_SIZE[)
#define ROM_SIZE                 ROM_AFTER_(ROM_URL)

// Size in bytes of each section of the ROM
#define ROM_VERSION_SZ_          (1/*major*/ +1/*minor*/ +2/*patch*/)
#define ROM_RESOLUTION_SZ_       (2 * 11/*# possible resolutions*/)
#define ROM_COLOR_SZ_            (256/*colors*/ * 8/*Lab + kd + RGB*/)
#define ROM_PALETTE_SZ_          128
#define ROM_FONT_SZ_             ROM_FONT_ASCII4_SZ_ + ROM_FONT_ASCII5_SZ_ +   \
                                 ROM_FONT_ASCII6_SZ_ + ROM_FONT_ASCII7_SZ_ +   \
                                 ROM_FONT_TOKIPONA5_SZ_ +                      \
                                 ROM_FONT_TOKIPONA6_SZ_ +                      \
                                 ROM_FONT_TOKIPONA7_SZ_
#define ROM_FONT_ASCII4_SZ_      (95/*printable ascii chars*/*2)
#define ROM_FONT_ASCII5_SZ_      (95/*printable ascii chars*/*4)
#define ROM_FONT_ASCII6_SZ_      (95/*printable ascii chars*/*4)
#define ROM_FONT_ASCII7_SZ_      (95/*printable ascii chars*/*4)
#define ROM_FONT_TOKIPONA5_SZ_   (123/*pu words*/*8)
#define ROM_FONT_TOKIPONA6_SZ_   (123/*pu words*/*8)
#define ROM_FONT_TOKIPONA7_SZ_   (123/*pu words*/*8)
#define ROM_WAV_HEADER_SZ_       44
#define ROM_URL_SZ_              64


// The ROM (as a byte array)
extern  alignas(max_align_t)
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
const uint8_t ROM[ROM_SIZE];

#endif //include guard
