#include "font.h"

/*
#define Q_TODO  0x6ff6 // some kind of filled "circle" (for now!)
uint16_t chr_quadrant_font[] = {
   //////////////////////////////////
   // range 0-31                   //
   //////////////////////////////////
   // TODO: find which 32 symbols should be in the range 0-31
   [  0] = Q_TODO, [  1] = Q_TODO, [  2] = Q_TODO, [  3] = Q_TODO,
   [  4] = Q_TODO, [  5] = Q_TODO, [  6] = Q_TODO, [  7] = Q_TODO,
   [  8] = Q_TODO, [  9] = Q_TODO, [ 10] = Q_TODO, [ 11] = Q_TODO,
   [ 12] = Q_TODO, [ 13] = Q_TODO, [ 14] = Q_TODO, [ 15] = Q_TODO,
   [ 16] = Q_TODO, [ 17] = Q_TODO, [ 18] = Q_TODO, [ 19] = Q_TODO,
   [ 20] = Q_TODO, [ 21] = Q_TODO, [ 22] = Q_TODO, [ 23] = Q_TODO,
   [ 24] = Q_TODO, [ 25] = Q_TODO, [ 26] = Q_TODO, [ 27] = Q_TODO,
   [ 28] = Q_TODO, [ 29] = Q_TODO, [ 30] = Q_TODO, [ 31] = Q_TODO,

   //////////////////////////////////
   // range 32-96 is same as ASCII //
   //////////////////////////////////
   // letters (TODO: check those alts too)
   //                B:0xcec0 / C:0xe8e0 / J:0x2ae0(!!!)
   //                Q:0xee20 0xeac0 0xeac2 / R:0xe880
   // numbers (note: 0 = O, 2 = Z, 5 = S, 8 = full 3x3, 9 = Q or alt-Q (TODO))
   // punctuation: * = X,  // @ = Q or alt-Q (TODO)
   //              & alts (but they're worse): 0x5ad0 0x4ae0 0x5ae1
   // toki pona "look-alike":
   // +=en, I=pini, K:ken, L:pi, T:pake, U:luka/poki, V:suli, X:ala, Y:anu

   [' '] = 0x0000, ['!'] = 0x44a0, ['"'] = 0xa000,

   // note: all those have width 4
   ['#'] = 0x6f60, ['$'] = 0x76e0, ['%'] = 0xb6d0, ['&'] = 0x5ad0,

   ['\'']= 0x4400, ['('] = 0xc8c0, [')'] = 0x6260, ['*'] = 0xa4a0,
   ['+'] = 0x4e40, [','] = 0x0240, ['-'] = 0x0e00, ['.'] = 0x0040,
   ['/'] = 0x2480,

   ['0'] = 0xeae0, ['1'] = 0xc4e0, ['2'] = 0xc460, ['3'] = 0xc6c0,
   ['4'] = 0xae20, ['5'] = 0x64c0, ['6'] = 0x8ee0, ['7'] = 0xe220,
   ['8'] = 0xeee0, ['9'] = 0xee20,

   [':'] = 0x4040, [';'] = 0x2060, ['<'] = 0x2420, ['='] = 0xe0e0,
   ['>'] = 0x8480, ['?'] = 0xe640, ['@'] = 0xeac0,

   ['A'] = 0x4ea0, ['B'] = 0xcee0, ['C'] = 0x6860, ['D'] = 0xcac0,
   ['E'] = 0xece0, ['F'] = 0xec80, ['G'] = 0xcae0, ['H'] = 0xaea0,
   ['I'] = 0xe4e0, ['J'] = 0x22c0, ['K'] = 0xaca0, ['L'] = 0x88e0,
   ['M'] = 0xeea0, ['N'] = 0xcaa0, ['O'] = 0xeae0, ['P'] = 0xee80,
   ['Q'] = 0xeac4, ['R'] = 0xcea0, ['S'] = 0x64c0, ['T'] = 0xe440,
   ['U'] = 0xaae0, ['V'] = 0xaac0, ['W'] = 0xaee0, ['X'] = 0xa4a0,
   ['Y'] = 0xa440, ['Z'] = 0xc460,

   ['['] = 0x6460, ['\\']= 0x8420, [']'] = 0xc4c0, ['^'] = 0x4a00,
   ['_'] = 0x00e0, ['`'] = 0x8400,

   ////////////////////////////////////
   // 97-122 is DIFFERENT than ASCII //
   ////////////////////////////////////
   // in this range are the ASCII lowercase letters, but we can't differentiate
   // them in such a tiny font, so we use that range for other symbols.
   // (one should ascii-uppercase a string before printing with this minifont)
   // TODO: find which 26 tiny symbols would be good for the range 97-122
   [ 97] = Q_TODO, [ 98] = Q_TODO, [ 99] = Q_TODO, [100] = Q_TODO,
   [101] = Q_TODO, [102] = Q_TODO, [103] = Q_TODO, [104] = Q_TODO,
   [105] = Q_TODO, [106] = Q_TODO, [107] = Q_TODO, [108] = Q_TODO,
   [109] = Q_TODO, [110] = Q_TODO, [111] = Q_TODO, [112] = Q_TODO,
   [113] = Q_TODO, [114] = Q_TODO, [115] = Q_TODO, [116] = Q_TODO,
   [117] = Q_TODO, [118] = Q_TODO, [119] = Q_TODO, [120] = Q_TODO,
   [121] = Q_TODO, [122] = Q_TODO,

   ////////////////////////////////////
   // range 123-126 is same as ASCII //
   ////////////////////////////////////
   ['{'] = 0x6c60, ['|'] = 0x4440, ['}'] = 0xc6c0, ['~'] = 0x2e80,

   // TODO: find defaults symbols for the range 127-255
   //       or alternatively, we can make the array stops here.
};
*/



#define QUADRANT_TODO_  0x6ff6 // some kind of filled "circle" (for now!)

uint16_t chr_quadrant_font[] = {
   // TODO: find 32 symbols to put in range 0-31
   QUADRANT_TODO_, //   0
   QUADRANT_TODO_, //   1
   QUADRANT_TODO_, //   2
   QUADRANT_TODO_, //   3
   QUADRANT_TODO_, //   4
   QUADRANT_TODO_, //   5
   QUADRANT_TODO_, //   6
   QUADRANT_TODO_, //   7
   QUADRANT_TODO_, //   8
   QUADRANT_TODO_, //   9
   QUADRANT_TODO_, //  10
   QUADRANT_TODO_, //  11
   QUADRANT_TODO_, //  12
   QUADRANT_TODO_, //  13
   QUADRANT_TODO_, //  14
   QUADRANT_TODO_, //  15
   QUADRANT_TODO_, //  16
   QUADRANT_TODO_, //  17
   QUADRANT_TODO_, //  18
   QUADRANT_TODO_, //  19
   QUADRANT_TODO_, //  20
   QUADRANT_TODO_, //  21
   QUADRANT_TODO_, //  22
   QUADRANT_TODO_, //  23
   QUADRANT_TODO_, //  24
   QUADRANT_TODO_, //  25
   QUADRANT_TODO_, //  26
   QUADRANT_TODO_, //  27
   QUADRANT_TODO_, //  28
   QUADRANT_TODO_, //  29
   QUADRANT_TODO_, //  30
   QUADRANT_TODO_, //  31
   // range 32-96 contains printable ASCII
   QUADRANT(0000), //  32: U+0020 ( )
   QUADRANT(0522), //  33: U+0021 (!)
   QUADRANT(0005), //  34: U+0022 (")
   QUADRANT(06F6), //  35: U+0023 (#) - note: width 4
   QUADRANT(076E), //  36: U+0024 ($) - note: width 4
   QUADRANT(0B6D), //  37: U+0025 (%) - note: width 4
   QUADRANT(0B5A), //  38: U+0026 (&) - note: width 4
   QUADRANT(0022), //  39: U+0027 (')
   QUADRANT(0313), //  40: U+0028 (()
   QUADRANT(0646), //  41: U+0029 ())
   QUADRANT(0525), //  42: U+002A (*)
   QUADRANT(0272), //  43: U+002B (+)
   QUADRANT(0240), //  44: U+002C (,)
   QUADRANT(0070), //  45: U+002D (-)
   QUADRANT(0200), //  46: U+002E (.)
   QUADRANT(0124), //  47: U+002F (/)
   QUADRANT(0757), //  48: U+0030 (0)
   QUADRANT(0723), //  49: U+0031 (1)
   QUADRANT(0623), //  50: U+0032 (2)
   QUADRANT(0363), //  51: U+0033 (3)
   QUADRANT(0475), //  52: U+0034 (4)
   QUADRANT(0326), //  53: U+0035 (5)
   QUADRANT(0771), //  54: U+0036 (6)
   QUADRANT(0447), //  55: U+0037 (7)
   QUADRANT(0777), //  56: U+0038 (8)
   QUADRANT(0477), //  57: U+0039 (9)
   QUADRANT(0202), //  58: U+003A (:)
   QUADRANT(0604), //  59: U+003B (;)
   QUADRANT(0424), //  60: U+003C (<)
   QUADRANT(0707), //  61: U+003D (=)
   QUADRANT(0121), //  62: U+003E (>)
   QUADRANT(0267), //  63: U+003F (?)
   QUADRANT(0357), //  64: U+0040 (@)
   QUADRANT(0572), //  65: U+0041 (A) / U+0061 (a)
   QUADRANT(0773), //  66: U+0042 (B) / U+0062 (b)
   QUADRANT(0616), //  67: U+0043 (C) / U+0063 (c)
   QUADRANT(0353), //  68: U+0044 (D) / U+0064 (d)
   QUADRANT(0737), //  69: U+0045 (E) / U+0065 (e)
   QUADRANT(0137), //  70: U+0046 (F) / U+0066 (f)
   QUADRANT(0753), //  71: U+0047 (G) / U+0067 (g)
   QUADRANT(0575), //  72: U+0048 (H) / U+0068 (h)
   QUADRANT(0727), //  73: U+0049 (I) / U+0069 (i)
   QUADRANT(0344), //  74: U+004A (J) / U+006A (j)
   QUADRANT(0535), //  75: U+004B (K) / U+006B (k)
   QUADRANT(0711), //  76: U+004C (L) / U+006C (l)
   QUADRANT(0577), //  77: U+004D (M) / U+006D (m)
   QUADRANT(0553), //  78: U+004E (N) / U+006E (n)
   QUADRANT(0757), //  79: U+004F (O) / U+006F (o)
   QUADRANT(0177), //  80: U+0050 (P) / U+0070 (p)
   QUADRANT(2357), //  81: U+0051 (Q) / U+0071 (q)
   QUADRANT(0573), //  82: U+0052 (R) / U+0072 (r)
   QUADRANT(0326), //  83: U+0053 (S) / U+0073 (s)
   QUADRANT(0227), //  84: U+0054 (T) / U+0074 (t)
   QUADRANT(0755), //  85: U+0055 (U) / U+0075 (u)
   QUADRANT(0355), //  86: U+0056 (V) / U+0076 (v)
   QUADRANT(0775), //  87: U+0057 (W) / U+0077 (w)
   QUADRANT(0525), //  88: U+0058 (X) / U+0078 (x)
   QUADRANT(0225), //  89: U+0059 (Y) / U+0079 (y)
   QUADRANT(0623), //  90: U+005A (Z) / U+007A (z)
   QUADRANT(0626), //  91: U+005B ([)
   QUADRANT(0421), //  92: U+005C (\)
   QUADRANT(0323), //  93: U+005D (])
   QUADRANT(0052), //  94: U+005E (^)
   QUADRANT(0700), //  95: U+005F (_)
   QUADRANT(0021), //  96: U+0060 (`)
   // TODO: find 26 symbols to put in range 97-122
   QUADRANT_TODO_, //  97
   QUADRANT_TODO_, //  98
   QUADRANT_TODO_, //  99
   QUADRANT_TODO_, // 100
   QUADRANT_TODO_, // 101
   QUADRANT_TODO_, // 102
   QUADRANT_TODO_, // 103
   QUADRANT_TODO_, // 104
   QUADRANT_TODO_, // 105
   QUADRANT_TODO_, // 106
   QUADRANT_TODO_, // 107
   QUADRANT_TODO_, // 108
   QUADRANT_TODO_, // 109
   QUADRANT_TODO_, // 110
   QUADRANT_TODO_, // 111
   QUADRANT_TODO_, // 112
   QUADRANT_TODO_, // 113
   QUADRANT_TODO_, // 114
   QUADRANT_TODO_, // 115
   QUADRANT_TODO_, // 116
   QUADRANT_TODO_, // 117
   QUADRANT_TODO_, // 118
   QUADRANT_TODO_, // 119
   QUADRANT_TODO_, // 120
   QUADRANT_TODO_, // 121
   QUADRANT_TODO_, // 122
   // range 123 - 126 contains printable ASCII
   QUADRANT(0636), // 123: U+007B ({)
   QUADRANT(0222), // 124: U+007C (|)
   QUADRANT(0363), // 125: U+007D (})
   QUADRANT(0174), // 126: U+007E (~)
};


// 3x4 quadrant font:
uint16_t chr_quadrant_font_3x4[] = {
   // The letters are from the PICO-8 fantasy console (lowercase)
   // (whose FAQ say they're free to use for any projects)
   // https://www.lexaloffle.com/gfx/pico-8_font_022.png
   QUADRANT(5756), //  65: U+0041 (A) / U+0061 (a)
   QUADRANT(7533), //  66: U+0042 (B) / U+0062 (b)
   QUADRANT(6116), //  67: U+0043 (C) / U+0063 (c)
   QUADRANT(3553), //  68: U+0044 (D) / U+0064 (d)
   QUADRANT(6137), //  69: U+0045 (E) / U+0065 (e)
   QUADRANT(1137), //  70: U+0046 (F) / U+0066 (f)
   QUADRANT(7516), //  71: U+0047 (G) / U+0067 (g)
   QUADRANT(5755), //  72: U+0048 (H) / U+0068 (h)
   QUADRANT(7227), //  73: U+0049 (I) / U+0069 (i)
   QUADRANT(3227), //  74: U+004A (J) / U+006A (j)
   QUADRANT(5535), //  75: U+004B (K) / U+006B (k)
   QUADRANT(6111), //  76: U+004C (L) / U+006C (l)
   QUADRANT(5577), //  77: U+004D (M) / U+006D (m)
   QUADRANT(5553), //  78: U+004E (N) / U+006E (n)
   QUADRANT(3556), //  79: U+004F (O) / U+006F (o)
   QUADRANT(1756), //  80: U+0050 (P) / U+0070 (p)
   QUADRANT(6352), //  81: U+0051 (Q) / U+0071 (q)
   QUADRANT(5353), //  82: U+0052 (R) / U+0072 (r)
   QUADRANT(3416), //  83: U+0053 (S) / U+0073 (s)
   QUADRANT(2227), //  84: U+0054 (T) / U+0074 (t)
   QUADRANT(6555), //  85: U+0055 (U) / U+0075 (u)
   QUADRANT(2755), //  86: U+0056 (V) / U+0076 (v)
   QUADRANT(7755), //  87: U+0057 (W) / U+0077 (w)
   QUADRANT(5225), //  88: U+0058 (X) / U+0078 (x)
   QUADRANT(3475), //  89: U+0059 (Y) / U+0079 (y)
   QUADRANT(7147), //  90: U+005A (Z) / U+007A (z)
};

#define TALLHALF_TODO_  TALLHALF(02666620)
uint32_t chr_tallhalf_font[] = {
   // TODO: find 32 symbols to put in range 0-31
   TALLHALF_TODO_,     //   0
   TALLHALF_TODO_,     //   1
   TALLHALF_TODO_,     //   2
   TALLHALF_TODO_,     //   3
   TALLHALF_TODO_,     //   4
   TALLHALF_TODO_,     //   5
   TALLHALF_TODO_,     //   6
   TALLHALF_TODO_,     //   7
   TALLHALF_TODO_,     //   8
   TALLHALF_TODO_,     //   9
   TALLHALF_TODO_,     //  10
   TALLHALF_TODO_,     //  11
   TALLHALF_TODO_,     //  12
   TALLHALF_TODO_,     //  13
   TALLHALF_TODO_,     //  14
   TALLHALF_TODO_,     //  15
   TALLHALF_TODO_,     //  16
   TALLHALF_TODO_,     //  17
   TALLHALF_TODO_,     //  18
   TALLHALF_TODO_,     //  19
   TALLHALF_TODO_,     //  20
   TALLHALF_TODO_,     //  21
   TALLHALF_TODO_,     //  22
   TALLHALF_TODO_,     //  23
   TALLHALF_TODO_,     //  24
   TALLHALF_TODO_,     //  25
   TALLHALF_TODO_,     //  26
   TALLHALF_TODO_,     //  27
   TALLHALF_TODO_,     //  28
   TALLHALF_TODO_,     //  29
   TALLHALF_TODO_,     //  30
   TALLHALF_TODO_,     //  31
   // range 32-126 contains printable ASCII
   TALLHALF(00000000), //  32: U+0020 ( )
   TALLHALF(02022222), //  33: U+0021 (!)
   TALLHALF(00000055), //  34: U+0022 (")
   TALLHALF(05575755), //  35: U+0023 (#)
   TALLHALF(02747172), //  36: U+0024 ($)
   TALLHALF(06612433), //  37: U+0025 (%)
   TALLHALF(06555252), //  38: U+0026 (&)
   TALLHALF(00000022), //  39: U+0027 (')
   TALLHALF(02111112), //  40: U+0028 (()
   TALLHALF(02444442), //  41: U+0029 ())
   TALLHALF(00052500), //  42: U+002a (*)
   TALLHALF(00027200), //  43: U+002b (+)
   TALLHALF(02200000), //  44: U+002c (,)
   TALLHALF(00007000), //  45: U+002d (-)
   TALLHALF(02000000), //  46: U+002e (.)
   TALLHALF(01122244), //  47: U+002f (/)
   TALLHALF(03555556), //  48: U+0030 (0)
   TALLHALF(07222232), //  49: U+0031 (1)
   TALLHALF(07112443), //  50: U+0032 (2)
   TALLHALF(03442443), //  51: U+0033 (3)
   TALLHALF(04447551), //  52: U+0034 (4)
   TALLHALF(03443117), //  53: U+0035 (5)
   TALLHALF(03557116), //  54: U+0036 (6)
   TALLHALF(01112447), //  55: U+0037 (7)
   TALLHALF(03552556), //  56: U+0038 (8)
   TALLHALF(03447556), //  57: U+0039 (9)
   TALLHALF(00200200), //  58: U+003a (:)
   TALLHALF(02200200), //  59: U+003b (;)
   TALLHALF(00421240), //  60: U+003c (<)
   TALLHALF(00070700), //  61: U+003d (=)
   TALLHALF(00124210), //  62: U+003e (>)
   TALLHALF(02022443), //  63: U+003f (?)
   TALLHALF(06135552), //  64: U+0040 (@)
   TALLHALF(05575556), //  65: U+0041 (A)
   TALLHALF(03553556), //  66: U+0042 (B)
   TALLHALF(03511156), //  67: U+0043 (C)
   TALLHALF(03555553), //  68: U+0044 (D)
   TALLHALF(03513156), //  69: U+0045 (E)
   TALLHALF(01113156), //  70: U+0046 (F)
   TALLHALF(02555516), //  71: U+0047 (G)
   TALLHALF(05557555), //  72: U+0048 (H)
   TALLHALF(07222227), //  73: U+0049 (I)
   TALLHALF(03444447), //  74: U+004a (J)
   TALLHALF(05553555), //  75: U+004b (K)
   TALLHALF(06111111), //  76: U+004c (L)
   TALLHALF(05555575), //  77: U+004d (M)
   TALLHALF(05555553), //  78: U+004e (N)
   TALLHALF(06555553), //  79: U+004f (O)
   TALLHALF(01135556), //  80: U+0050 (P)
   TALLHALF(46555553), //  81: U+0051 (Q)
   TALLHALF(05535556), //  82: U+0052 (R)
   TALLHALF(03542156), //  83: U+0053 (S)
   TALLHALF(02222227), //  84: U+0054 (T)
   TALLHALF(03555555), //  85: U+0055 (U)
   TALLHALF(02555555), //  86: U+0056 (V)
   TALLHALF(05755555), //  87: U+0057 (W)
   TALLHALF(05552555), //  88: U+0058 (X)
   TALLHALF(02222555), //  89: U+0059 (Y)
   TALLHALF(07112447), //  90: U+005a (Z)
   TALLHALF(03111113), //  91: U+005b ([)
   TALLHALF(04422211), //  92: U+005c (\)
   TALLHALF(06444446), //  93: U+005d (])
   TALLHALF(00000052), //  94: U+005e (^)
   TALLHALF(70000000), //  95: U+005f (_)
   TALLHALF(00000021), //  96: U+0060 (`)
   TALLHALF(06555600), //  97: U+0061 (a)
   TALLHALF(06555311), //  98: U+0062 (b)
   TALLHALF(02515600), //  99: U+0063 (c)
   TALLHALF(03555644), // 100: U+0064 (d)
   TALLHALF(06175200), // 101: U+0065 (e)
   TALLHALF(01131600), // 102: U+0066 (f)
   TALLHALF(34655600), // 103: U+0067 (g)
   TALLHALF(05553111), // 104: U+0068 (h)
   TALLHALF(07223020), // 105: U+0069 (i)
   TALLHALF(02544040), // 106: U+006a (j)
   TALLHALF(05535511), // 107: U+006b (k)
   TALLHALF(07222223), // 108: U+006c (l)
   TALLHALF(05557500), // 109: U+006d (m)
   TALLHALF(05555300), // 110: U+006e (n)
   TALLHALF(06555300), // 111: U+006f (o)
   TALLHALF(13555600), // 112: U+0070 (p)
   TALLHALF(46555300), // 113: U+0071 (q)
   TALLHALF(01113500), // 114: U+0072 (r)
   TALLHALF(03421600), // 115: U+0073 (s)
   TALLHALF(04222722), // 116: U+0074 (t)
   TALLHALF(06555500), // 117: U+0075 (u)
   TALLHALF(02555500), // 118: U+0076 (v)
   TALLHALF(05755500), // 119: U+0077 (w)
   TALLHALF(05525500), // 120: U+0078 (x)
   TALLHALF(03465500), // 121: U+0079 (y)
   TALLHALF(07124700), // 122: U+007a (z)
   TALLHALF(04221224), // 123: U+007b ({)
   TALLHALF(02222222), // 124: U+007c (|)
   TALLHALF(01224221), // 125: U+007d (})
   TALLHALF(00000036), // 126: U+007e (~)
};



uint64_t sitelen_pona_font[] = {
   // toki pona glyph       // UCSUR Private Codepoint and word
   GLYPH(0038141800080808), // U+F1900 a|!a
   GLYPH(001c227f227f1c22), // U+F1901 akesi
   GLYPH(0041221408142241), // U+F1902 ala
   GLYPH(00060a2a7f2a0a06), // U+F1903 alasa
   GLYPH(0000364949493600), // U+F1904 ale
   GLYPH(000008007f414100), // U+F1905 anpa
   GLYPH(0063360800083663), // U+F1906 ante (too fat!!!)
   GLYPH(0008080808142241), // U+F1907 anu
   GLYPH(0063222214140808), // U+F1908 awen
   GLYPH(0009122448241209), // U+F1909 e
   GLYPH(000808087f080808), // U+F190A en
   GLYPH(0006493e083e4930), // U+F190B esun
   GLYPH(001c22414141221c), // U+F190C ijo
   GLYPH(00004141221c0000), // U+F190D ike
   GLYPH(000808087f49497f), // U+F190E ilo
   GLYPH(00007f4149414100), // U+F190F insa
   GLYPH(b2496a955e6a354c), // U+F1910 jaki (8x8 but it's ok)
   GLYPH(0041221c2222221c), // U+F1911 jan
   GLYPH(007f221408081408), // U+F1912 jelo
   GLYPH(001c223a020c1408), // U+F1913 jo
   GLYPH(0000314a444a3100), // U+F1914 kala
   GLYPH(001c22417f002a49), // U+F1915 kalama
   GLYPH(0077444428281010), // U+F1916 kama
   GLYPH(000808083e494936), // U+F1917 kasi
   GLYPH(002222120e122222), // U+F1918 ken
   GLYPH(00464538107c547c), // U+F1919 kepeken
   GLYPH(001c224141493608), // U+F191A kili
   GLYPH(000814224141221c), // U+F191B kiwen
   GLYPH(003649412244522c), // U+F191C ko
   GLYPH(0022444422111122), // U+F191D kon
   GLYPH(007f4122227f1408), // U+F191E kule
   GLYPH(0022552200081408), // U+F191F kulupu
   GLYPH(000c10102428221c), // U+F1920 kute
   GLYPH(0004081010100804), // U+F1921 la
   GLYPH(000030484f483000), // U+F1922 lape
   GLYPH(007f221c083e4936), // U+F1923 laso
   GLYPH(000c1221217f120c), // U+F1924 lawa
   GLYPH(0049497f4141417f), // U+F1925 len
   GLYPH(004122147f142241), // U+F1926 lete
   GLYPH(00060c1830180c06), // U+F1927 li # ??? slim li : 32,16,8,4,8,16,32,0
   GLYPH(0000000814220000), // U+F1928 lili
   GLYPH(0000000649300000), // U+F1929 linja
   GLYPH(003e22222222223e), // U+F192A lipu
   GLYPH(007f221c001c223e), // U+F192B loje
   GLYPH(0000007f00080000), // U+F192C lon
   GLYPH(0044424544444438), // U+F192D luka
   GLYPH(00001c2249221c00), // U+F192E lukin (also see: oko)
   GLYPH(001c224141414141), // U+F192F lupa
   GLYPH(001c2a497f492a1c), // U+F1930 ma
   GLYPH(001c221c2241221c), // U+F1931 mama
   GLYPH(001c2222227f4122), // U+F1932 mani
   GLYPH(004149555549221c), // U+F1933 meli
   GLYPH(000402021a26221c), // U+F1934 mi
   GLYPH(00417f142222221c), // U+F1935 mije
   GLYPH(002625180018243c), // U+F1936 moku # TODO:alignement might be strange? or mabye it's on purpose
   GLYPH(001c3e556b553e1c), // U+F1937 moli
   GLYPH(003808080a080838), // U+F1938 monsi
   GLYPH(001c224941225d22), // U+F1939 mu
   GLYPH(001c264c4c4c261c), // U+F193A mun
   GLYPH(001c224141635522), // U+F193B musi
   GLYPH(002a2a2a2a2a2a2a), // U+F193C mute
   GLYPH(00227f2222227f22), // U+F193D nanpa
   GLYPH(001c22492519423c), // U+F193E nasa
   GLYPH(0008082a1c080808), // U+F193F nasin TODO ??? flip for up/down ???
   GLYPH(004141414141221c), // U+F1940 nena
   GLYPH(00081c2a08080808), // U+F1941 ni
   GLYPH(00003e4141413e00), // U+F1942 nimi
   GLYPH(003f414139090909), // U+F1943 noka
   GLYPH(0008140800080808), // U+F1944 o|!o
   GLYPH(001c224936224936), // U+F1945 olin
   GLYPH(00003e5148483000), // U+F1946 ona
   GLYPH(007f417f41414141), // U+F1947 open
   GLYPH(007f49515d45497f), // U+F1948 pakala
   GLYPH(0026252418242418), // U+F1949 pali !!!should be 1px wider???
   GLYPH(0008141414141408), // U+F194A palisa
   GLYPH(0008142a142a1422), // U+F194B pan
   GLYPH(0026252418002a49), // U+F194C pana
   GLYPH(007f010101010101), // U+F194D pi
   GLYPH(0008142241414936), // U+F194E pilin
   GLYPH(005d6b362a1c1c08), // U+F194F pimeja
   GLYPH(003e08080808083e), // U+F1950 pini
   GLYPH(00083e083e083e14), // U+F1951 pipi
   GLYPH(00001f1151111100), // U+F1952 poka
   GLYPH(007f414141414141), // U+F1953 poki
   GLYPH(00001c2241410000), // U+F1954 pona
   GLYPH(007f776b777f557f), // U+F1955 pu
   GLYPH(0000003e003e0000), // U+F1956 sama
   GLYPH(000800222a494908), // U+F1957 seli
   GLYPH(0041555555557f00), // U+F1958 selo
   GLYPH(000800083041413e), // U+F1959 seme
   GLYPH(004a555554504040), // U+F195A sewi
   GLYPH(004949494949497f), // U+F195B sijelo
   GLYPH(001c22495549221c), // U+F195C sike
   GLYPH(0000000063000808), // U+F195D sin
   GLYPH(001c22261a020204), // U+F195E sina
   GLYPH(000e08082808080e), // U+F195F sinpin
   GLYPH(007f41416b41417f), // U+F1960 sitelen
   GLYPH(003e22223e002a49), // U+F1961 sona
   GLYPH(001515204054403e), // U+F1962 soweli
   GLYPH(0008081414222241), // U+F1963 suli
   GLYPH(0008081463140808), // U+F1964 suno
   GLYPH(00002222227f0000), // U+F1965 supa
   GLYPH(0000080000552200), // U+F1966 suwi
   GLYPH(00000f4345390000), // U+F1967 tan
   GLYPH(002020203e202020), // U+F1968 taso
   GLYPH(007711110a0a0404), // U+F1969 tawa
   GLYPH(0030490600304906), // U+F196A telo
   GLYPH(001c224159492a1c), // U+F196B tenpo
   GLYPH(001c22221c002a49), // U+F196C toki
   GLYPH(007f414141221408), // U+F196D tomo
   GLYPH(0014141414141414), // U+F196E tu
   GLYPH(0008141c36554936), // U+F196F unpa
   GLYPH(0008001c22417f00), // U+F1970 uta
   GLYPH(0022775d081c3663), // U+F1971 utala
   GLYPH(007f412222551408), // U+F1972 walo
   GLYPH(0010101010141810), // U+F1973 8 wan
   GLYPH(0002023e20150804), // U+F1974 waso
   GLYPH(001c362263554941), // U+F1975 wawa
   GLYPH(0041220000002241), // U+F1976 weka
   GLYPH(0000364949412200), // U+F1977 wile
   GLYPH(0008080063000808), // U+F1978 namako
   GLYPH(0014081400080808), // U+F1979 kin
   GLYPH(0018246251622418), // U+F197A oko # oko-alt???: 56,100,178,179,178,100,56
   GLYPH(0021722408122742), // U+F197B kipisi
   GLYPH(007f415d555d417f), // U+F197C leko
   GLYPH(000014552a2a0800), // U+F197D monsuta # previously: 16,84,84,170,170,170,40,0
   GLYPH(0008081c221c2241), // U+F197E tonsi
   GLYPH(0008394e08394e08), // U+F197F jasima
   GLYPH(002a2922422a0418), // U+F1980 kijetesantakalu | |
   GLYPH(001c14147f41221c), // U+F1981 soko
   GLYPH(0041414149414141), // U+F1982 meso
   GLYPH(001c224141002a49), // U+F1983 epiku
   GLYPH(001a2c1a2c002a49), // U+F1984 kokosila
   GLYPH(0052453901122c10), // U+F1985 lanpan
   GLYPH(0014140c00080808), // U+F1986 n|!n
   GLYPH(001824243c242418), // U+F1987 misikeke
   GLYPH(007d355d65714d3d), // U+F1988 ku
   GLYPH(e0101010101010e0), // U+F1990 start of cartouche
   GLYPH(0708080808080807), // U+F1991 end of cartouche
   GLYPH(ff000000000000ff), // U+F1992 cartouche extension (combiner)
   GLYPH(f808080808080808), // U+F1993 start of long pi
   GLYPH(ff00000000000000), // U+F1994 long pi extension (combiner) -> same as CPC's _
   GLYPH(0000000014080000), // U+F1995 stacking joiner (combiner!)
   GLYPH(000000081c080000), // U+F1996 scaling joiner (combiner!)
   GLYPH(ff00000000000000), // U+F1997 start of long glyph (combiner!) => after: pi, ala, prepositions
   GLYPH(0f08000000000000), // U+F1998 end of long glyph (zero-width!)
   GLYPH(ff00000000000000), // U+F1999 long glyph extension (combiner!) -> same as CPC's _
   GLYPH(f010000000000000), // U+F199A start of reverse long glyph (combiner)
   GLYPH(ff00000000000000), // U+F199B end of reverse long glyph (combiner)
   GLYPH(0000000008000000), // U+F199C middle dot
   GLYPH(0000000800080000), // U+F199D colon
   GLYPH(000808080808087f), // U+F19A0 pake
   GLYPH(0018184224186666), // U+F19A1 apeja
   GLYPH(0008080063000000), // U+F19A2 majuna 8x7
   GLYPH(00007f0014081400), // U+F19A3 powe

   // tuki tiki glyph       // UCSUR Private Codepoint and word
   GLYPH(0008000808080808), // a
   GLYPH(0041221408142241), // ala
   GLYPH(000818287F281808), // i
   GLYPH(001C22414141221C), // iku
   GLYPH(000808080808087F), // ilu
   GLYPH(001C22415541221C), // ka
   GLYPH(000808081C2A4908), // kati
   GLYPH(00081C2A0810221C), // ki
   GLYPH(0008142241221408), // kiku
   GLYPH(0033444422111166), // ku
   GLYPH(000C10101010100C), // la
   GLYPH(000000304F300000), // lapi
   GLYPH(00080C0A7F0A0C08), // li
   GLYPH(0000003149460000), // lika
   GLYPH(0000384445443800), // lili
   // GLYPH(0000000814000000), // lili-alt
   GLYPH(007F000000000000), // lu
   GLYPH(007F414141414141), // lupa
   GLYPH(00081C2A7F080808), // mi
   GLYPH(00001C22417F0000), // muku
   GLYPH(001C222222775577), // muti
   GLYPH(0022221C00140814), // paka
   GLYPH(0022221C001C221C), // pali
   GLYPH(000042427E424200), // puka
   GLYPH(00001C2241410000), // pula
   GLYPH(0074121112141810), // taka
   GLYPH(00007F0000007F00), // tama
   GLYPH(001C22414047231D), // tiki
   GLYPH(004141414141417F), // tiku
   GLYPH(000808087F2A1C08), // tila
   GLYPH(0031494600314946), // tilu
   GLYPH(000800081020221C), // timi
   GLYPH(007F40404040407F), // tipi
   GLYPH(007F00002A00007F), // titi
   GLYPH(0004141414141410), // tu
   GLYPH(001C22221C002A49), // tuki
   GLYPH(003E4149413E4141), // tula
   GLYPH(0008081C771C0808), // tulu
   GLYPH(000E060A103E0408), // uli
   GLYPH(001C22221C082A1C), // upi
};
