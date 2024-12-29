#ifndef  KONPU_BITS_H
#define  KONPU_BITS_H
#include "c.h"

// Terminology:
// A "Chunk" is a certain number of bits contained within an unsigned integer.
// An 8-bits chunk [0-255] may be referred to as a Byte.
// A  4-bits chunk [0-15]  may be referred to as a Nibble.
// A  2-bits chunk [0-3]   may be referred to as Quarter.
// A  1-bit  chunk {0,1}   may be referred to asa Bit.


// Unsigned integers as "chunk" arrays, ie arrays of bytes, nibbles
// (= 4-bits value), quarter (= a 2-bits chunk), or bits:
#define BitsByteAt(bits, index)             (((bits) >> ((index) << 3)) & 0xFFu)
#define BitsNibbleAt(bits, index)           (((bits) >> ((index) << 2)) & 0xFu)
#define BitsQuarterAt(bits, index)          (((bits) >> ((index) << 1)) & 3u)
#define BitsBitAt(bits, index)              (((bits) >> ((index)     )) & 1u)


#endif //KONPU_BITS_H
