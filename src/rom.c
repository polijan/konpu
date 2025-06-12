#include "rom.h"

//------------------------------------------------------------------------------
// Include things we need to stuff into the ROM
//------------------------------------------------------------------------------
#include "video_mode.h"

// Constants glyph definition (but prevent inclusion of glyph.h)
#define KONPU_GLYPH_H_
#define GLYPH8(hex)   ((uint8_t)0x##hex##U)
#define GLYPH16(hex)  ((uint16_t)0x##hex##U)
#define GLYPH32(hex)  ((uint32_t)0x##hex##U)
#define GLYPH64(hex)  ((uint64_t)0x##hex##U)
#include "glyph_constants/glyph_ascii.h"
#include "glyph_constants/glyph_tokipona.h"

//------------------------------------------------------------------------------
// Static checks on the ROM addresses to verify their aligments if they
// may hold something else than just raw bytes.
//------------------------------------------------------------------------------

// static_assert(ROM_FONT % alignof(uint64_t) == 0);


//------------------------------------------------------------------------------
// Definition of Konpu's ROM and its content
//------------------------------------------------------------------------------
alignas(max_align_t)
#if defined(__GNUC__)
    __attribute__((__may_alias__))
#endif
const uint8_t ROM[ROM_SIZE] = {

   //---------------------------------------------------------------------------
   // ROM Section: Version
   //---------------------------------------------------------------------------
   KONPU_VERSION_MAJOR, // Major
   KONPU_VERSION_MINOR, // Minor
   // Patch (encoded as 16-bit LITTLE endian):
   (KONPU_VERSION_PATCH & 0xFF), (KONPU_VERSION_PATCH >> 8),

   //---------------------------------------------------------------------------
   // ROM Section: Resolution
   //
   // This gives the possible Framebuffer Resolutions (size in 8x8 cells)
   // (framebuffer "factor" --> see: tools/resfinder)
   //---------------------------------------------------------------------------
#if VIDEO_FACTOR_ == 7
   // 20160 bytes (~19.7Kb) -- also allow 7 bitplanes

   //------..---------.--------.-----.-----------------------------------------
   // WxH  // Pixels  | Aspect |Bytes| Type of modes (bitplanes, number of)
   //      // Resltn. | Ratio  |8x8 / attribute bytes per 8x8px, packed pixels)
   //------//---------|--------|---/-------------------------------------------
   60, 42, // 480x336 |1.42857 | 8| 1 bitplane, i.e. "monochrome" modes
   56, 40, // 448x320 |1.4     | 9| attributes: 1 byte  per 8x8
   56, 36, // 448x288 |1.55555 |10| attributes: 2 bytes per 8x8
   48, 35, // 384x280 |1.37143 |12| attributes: 4 bytes per 8x8                *
   42, 30, // 336x240 |1.4     |16| 4-colors chunky pixels / 2 bitplanes /
           //         |        |  |             attributes:  8 bytes per 8x8
   35, 24, // 280x192 |1.45833 |24| 3 bitplanes, ie. 8 colors / attr: 16 bytes *
   30, 21, // 240x168 |1.42857 |32| 16-color chunky pixels / 4 bitplanes
   28, 18, // 224x144 |1.55555 |40| 5 bitplanes, i.e.  32 colors
   28, 15, // 224x120 |1.86666 |48| 6 bitplanes, i.e.  64 colors
   24, 15, // 192x120 |1.6     |56| 7 bitplanes, i.e. 128 colors
   21, 15, // 168x120 |1.4     |64| 256-color byte pixels / 8 bitplanes
#elif VIDEO_FACTOR_ == 6
   // 17280 bytes (~16.9Kb)
   // this factor lends itself well to favor wide ratio (1.6 - 1.875)
   // exception the default for 256 color mode, but even then it could be
   // changed to 160x108 (AR: 1.48148) for pixel modes.
   60, 36, // 480x288 |1.66666 | 8|
   60, 32, // 480x256 |1.875   | 9|
   54, 32, // 432x256 |1.6875  |10|
   48, 30, // 384x240 |1.6     |12|
   45, 24, // 360x192 |1.875   |16|  *
   36, 20, // 288x160 |1.8     |24|
   30, 18, // 240x144 |1.66666 |32|
   27, 16, // 216x128 |1.6875  |40|
   24, 15, // 192x120 |1.6     |48|
    0,  0, // > NOT AVAILABLE <|56|
   18, 15, // 144x120 |1.2     |64|
#elif VIDEO_FACTOR_ == 5
   // 14400 bytes (~14Kb)            odd (in mode <= 24)
   50, 36, // 400x288 |1.38888 | 8|
   50, 32, // 400x256 |1.5265  | 9|
   45, 32, // 360x256 |1.40625 |10|  *
// 48, 30, // 384x240 |1.6     |10|
   40, 30, // 320x240 |1.33333 |12|
   36, 25, // 288x200 |1.44    |16|  *
// 30, 30, // 240x240 | 1      |16|
   30, 20, // 240x160 |1.5     |24|
   25, 18, // 200x144 |1.38888 |32|
   24, 15, // 192x120 |1.6     |40|
   20, 15, // 160x120 |1.33333 |48|
    0,  0, // > NOT AVAILABLE <|56|
   15, 15, // 120x120 |1       |64|  144x100px (AR=1.44) common for 256c pixels
#elif VIDEO_FACTOR_ == 4
   // 11520 bytes (~11.25Kb)
   // this factor lends itself well to favor almost square ratios (1.06 - 1-25)
   40, 36, // 320x288 |1.11111 | 8|
   40, 32, // 320x256 |1.25    | 9|
   36, 32, // 288x256 |1.125   |10|
   32, 30, // 256x240 |1.06666 |12|
   30, 24, // 240x192 |1.25    |16|
   24, 20, // 192x160 |1.2     |24|
   20, 18, // 160x144 |1.11111 |32|
   18, 16, // 144x128 |1.125   |40|
   16, 15, // 128x120 |1.06666 |48|
    0,  0, // > NOT AVAILABLE <|56|
   15, 12, // 120x96  |1.25    |64|
#elif VIDEO_FACTOR_ == 3
   // 8640 bytes (~8.4Kb)
   40, 27, // 320x216 |1.48148 | 8|  *
   40, 24, // 320x192 |1.66666 | 9|
   36, 24, // 288x192 |1.5     |10|
   36, 20, // 288x160 |1.8     |12|
   30, 18, // 240x144 |1.66666 |16|
   24, 15, // 192x120 |1.6     |24|  *
   18, 15, // 144x120 |1.2     |32|
   18, 12, // 144x96  |1.5     |40|
   18, 10, // 144x80  |1.8     |48|
    0,  0, // > NOT AVAILABLE <|56|
   15,  9, // 120x172 |1.66666 |64|
#elif VIDEO_FACTOR_ == 2
   // 5760 bytes (~5.6Kb)
   36, 20, // 288x160 |1.8     | 8|
   32, 20, // 256x160 |1.6     | 9|
   32, 18, // 256x144 |1.77777 |10|
   30, 16, // 240x128 |1.875   |12|
   24, 15, // 192x120 |1.6     |16|  *
   20, 12, // 160x96  |1.66666 |24|
   18, 10, // 144x80  |1.8     |32|
   16,  9, // 128x72  |1.77777 |40|
   15,  8, // 120x64  |1.875   |48|
    0,  0, // > NOT AVAILABLE <|56|
   10,  9, //  80x72  |1.11111 |64|
#else
#   error "This VIDEO_FACTOR_ is not implemented"
   ??, ??, // ???x??? |?       | 8|
   ??, ??, // ???x??? |?       | 9|
   ??, ??, // ???x??? |?       |10|
   ??, ??, // ???x??? |?       |12|
   ??, ??, // ???x??? |?       |16|
   ??, ??, // ???x??? |?       |24|
   ??, ??, // ???x??? |?       |32|
   ??, ??, // ???x??? |?       |40|
   ??, ??, // ???x??? |?       |48|
    0,  0, // > NOT AVAILABLE <|56|
   ??, ??, // ???x??? |?       |64|
#endif

   //---------------------------------------------------------------------------
   // ROM Section: Color
   //
   // OkLab components multiplied by 510 and rounded to 8-bits
   // + space-partitioning k-d tree info on the palette in L,a,b space [note:
   // (median) color 127 means no child node] + 8-bits gamma sRGB components.
   // Generated by `kule-generate-konpu-rom` (see: tools/kule).
   //
   //---------------------------------------------------------------------------
   // Color #|OkLab: L/2, a-12,  b+30 |kd: left, right |RGB: R,   G,   B
   //--------|------------------------|----------------|-----------------
   /*    0  */         0,  -12,   30  ,     127, 127   ,     0,   0,   0,
   /*    1  */        15,  -14,   10  ,      55,  49   ,     0,   0,   7,
   /*    2  */        22,    3,   39  ,      12,  22   ,     8,   0,   0,
   /*    3  */        30,  -16,  -11  ,     127, 127   ,     0,   0,  36,
   /*    4  */        32,  -29,   43  ,      29,  16   ,     0,  10,   0,
   /*    5  */        34,   10,   12  ,     127, 127   ,    17,   1,  21,
   /*    6  */        40,    5,   44  ,     127, 127   ,    27,   5,   1,
   /*    7  */        43,  -22,   15  ,       4,   2   ,     0,  17,  28,
   /*    8  */        45,    4,  -17  ,      21,  20   ,    19,   1,  55,
   /*    9  */        49,   25,   22  ,       5,  39   ,    41,   2,  26,
   /*   10  */        49,  -37,   46  ,     127, 127   ,     0,  27,   4,
   /*   11  */        53,  -18,  -41  ,     127, 127   ,     1,   1,  86,
   /*   12  */        56,   -6,   34  ,       0,  41   ,    33,  24,  22,
   /*   13  */        57,  -20,   -7  ,      35,   8   ,     2,  26,  61,
   /*   14  */        58,   27,  -11  ,     127, 127   ,    46,   1,  63,
   /*   15  */        58,   25,   52  ,     127, 127   ,    59,   5,   1,
   /*   16  */        65,  -46,   56  ,      10,  37   ,     1,  44,   1,
   /*   17  */        65,    8,  -42  ,     127, 127   ,    36,   2, 100,
   /*   18  */        65,   39,   20  ,     127, 127   ,    67,   3,  43,
   /*   19  */        66,  -32,   22  ,     127, 127   ,     1,  42,  45,
   /*   20  */        68,    2,    9  ,       3,  31   ,    43,  31,  57,
   /*   21  */        69,  -20,  -62  ,      11,  42   ,     3,   3, 124,
   /*   22  */        69,  -11,   56  ,       6,  34   ,    49,  37,   3,
   /*   23  */        73,  -22,  -27  ,     127, 127   ,     1,  37,  97,
   /*   24  */        74,   37,   59  ,     127, 127   ,    86,   7,   2,
   /*   25  */        74,   33,  -23  ,      14,  53   ,    64,   8,  92,
   /*   26  */        80,    9,   36  ,       9,  38   ,    68,  40,  41,
   /*   27  */        80,   51,    5  ,     127, 127   ,    88,   4,  72,
   /*   28  */        81,    6,  -64  ,      17,  45   ,    45,   2, 140,
   /*   29  */        81,  -36,   42  ,      19,  47   ,    26,  58,  37,
   /*   30  */        82,  -33,    4  ,      23,  43   ,     0,  56,  78,
   /*   31  */        84,    1,  -11  ,     127, 127   ,    51,  44,  93,
   /*   32  */        84,  -22,  -84  ,     127, 127   ,     5,   1, 165,
   /*   33  */        87,   55,   40  ,      18,  44   ,   107,   3,  42,
   /*   34  */        87,  -12,   65  ,     127, 127   ,    70,  54,   2,
   /*   35  */        87,  -22,  -47  ,      48,  30   ,     5,  46, 134,
   /*   36  */        88,   35,  -47  ,     127, 127   ,    76,   5, 131,
   /*   37  */        89,  -59,   65  ,     127, 127   ,     0,  72,   6,
   /*   38  */        91,   23,   61  ,      15,  60   ,    98,  40,  16,
   /*   39  */        92,   20,   14  ,     127, 127   ,    83,  46,  77,
   /*   40  */        92,   57,  -20  ,      27,  62   ,   100,   1, 109,
   /*   41  */        93,  -16,   26  ,     127, 127   ,    56,  64,  67,
   /*   42  */        96,    4,  -86  ,     127, 127   ,    53,   1, 182,
   /*   43  */        96,  -28,  -17  ,     127, 127   ,    14,  67, 115,
   /*   44  */        97,   68,   12  ,     127, 127   ,   120,   0,  82,
   /*   45  */        97,    7,  -37  ,     127, 127   ,    63,  49, 134,
   /*   46  */        98,   55,   69  ,      24,  67   ,   130,   8,   2,
   /*   47  */        98,  -49,   36  ,     127, 127   ,     6,  80,  62,
   /*   48  */       100,  -24, -107  ,      32,  51   ,     8,   1, 210,
   /*   49  */       101,  -32,   69  ,       7, 117   ,    67,  76,  10,
   /*   50  */       102,   35,  -70  ,      36,  56   ,    87,   4, 172,
   /*   51  */       103,  -24,  -70  ,     127, 127   ,     3,  55, 177,
   /*   52  */       103,   35,   38  ,      26,  58   ,   114,  48,  61,
   /*   53  */       103,   29,  -13  ,     127, 127   ,    95,  50, 116,
   /*   54  */       104,   -2,   51  ,      87,  97   ,    91,  69,  48,
   /*   55  */       105,   -4,    2  ,      13,  61   ,    72,  70, 105,
   /*   56  */       106,   57,  -43  ,     127, 127   ,   112,  12, 149,
   /*   57  */       106,  -67,   71  ,     127, 127   ,     4,  93,  13,
   /*   58  */       107,   70,   42  ,      33,  46   ,   143,  10,  59,
   /*   59  */       108,  -43,    8  ,     127, 127   ,     3,  88, 104,
   /*   60  */       108,   24,   73  ,     127, 127   ,   124,  56,   4,
   /*   61  */       110,  -24,  -37  ,      82,  96   ,    30,  77, 152,
   /*   62  */       110,   74,  -13  ,     127, 127   ,   135,   9, 122,
   /*   63  */       111,    4, -107  ,     127, 127   ,    64,   0, 224,
   /*   64  */       111,    6,  -60  ,       1,  65   ,    71,  57, 175,
   /*   65  */       113,   47,    9  ,      77,  71   ,   126,  51, 104,
   /*   66  */       113,  -32,   42  ,     127, 127   ,    65,  91,  70,
   /*   67  */       114,   68,   75  ,     127, 127   ,   161,   9,   4,
   /*   68  */       114,  -28, -127  ,     127, 127   ,     0,   2, 252,
   /*   69  */       116,  -12,   77  ,     127, 127   ,   106,  83,   1,
   /*   70  */       116,    9,  -25  ,      28,  25   ,    88,  73, 146,
   /*   71  */       117,   15,   22  ,      52, 107   ,   109,  75,  95,
   /*   72  */       117,   33,  -88  ,      70,  76   ,    98,  19, 211,
   /*   73  */       118,  -26,  -93  ,      68, 104   ,     1,  64, 221,
   /*   74  */       119,   82,   18  ,     127, 127   ,   159,  12,  99,
   /*   75  */       119,   41,   50  ,      98,  92   ,   142,  60,  62,
   /*   76  */       119,   43,  -26  ,      50,  40   ,   121,  56, 148,
   /*   77  */       119,   61,  -68  ,      72, 126   ,   126,   1, 191,
   /*   78  */       120,  -47,   75  ,      80, 106   ,    73, 102,  21,
   /*   79  */       121,  -21,   -4  ,     127, 127   ,    65,  93, 130,
   /*   80  */       123,  -53,   22  ,      57,  99   ,     7, 109, 106,
   /*   81  */       124,   78,  -37  ,     127, 127   ,   149,  15, 164,
   /*   82  */       124,  -30,  -57  ,      73,  94   ,     9,  90, 192,
   /*   83  */       125,   16,   73  ,     127, 127   ,   139,  80,  31,
   /*   84  */       125,   79,   50  ,     103, 123   ,   176,  23,  67,
   /*   85  */       125,   27,  -55  ,      95, 101   ,   107,  65, 188,
   /*   86  */       127,  -79,   81  ,     127, 127   ,     3, 120,   8,
   /*   87  */       127,  -12,   30  ,      66, 118   ,    99,  99,  99,
   /*   88  */       128,   16, -115  ,     127, 127   ,    90,  22, 255,
   /*   89  */       128,    2,  -82  ,      63, 113   ,    77,  69, 221,
   /*   90  */       129,   23,   -3  ,     127, 127   ,   122,  83, 136,
   /*   91  */       130,   53,   22  ,     127, 127   ,   155,  65, 108,
   /*   92  */       130,   52,   81  ,      83, 115   ,   173,  57,   7,
   /*   93  */       131,   90,   -6  ,     127, 127   ,   174,  19, 139,
   /*   94  */       131,  -41,  -25  ,      59, 105   ,     4, 109, 166,
   /*   95  */       132,   51,  -99  ,      88, 112   ,   127,   1, 241,
   /*   96  */       132,   -6,  -35  ,      89, 110   ,    84,  95, 176,
   /*   97  */       132,  -22,   63  ,      69, 108   ,   108, 107,  60,
   /*   98  */       133,   21,   38  ,      91, 120   ,   139,  89,  93,
   /*   99  */       133,  -67,   48  ,     127, 127   ,     4, 125,  84,
   /*  100  */       135,   80,  -66  ,      81, 121   ,   158,   4, 208,
   /*  101  */       135,   49,  -38  ,      90, 114   ,   142,  68, 181,
   /*  102  */       135,   86,   79  ,     127, 127   ,   203,   5,  25,
   /*  103  */       136,   98,   28  ,      74, 125   ,   196,   0, 106,
   /*  104  */       136,  -28, -101  ,     127, 127   ,     5,  85, 254,
   /*  105  */       136,  -34,    7  ,     127, 127   ,    67, 117, 137,
   /*  106  */       137,  -51,   81  ,      86, 116   ,    90, 123,  29,
   /*  107  */       140,   55,   52  ,      75,  84   ,   180,  73,  81,
   /*  108  */       141,   -4,   87  ,     127, 127   ,   147, 106,   2,
   /*  109  */       141,   94,  -36  ,     100, 111   ,   183,  20, 183,
   /*  110  */       141,    1,    9  ,      79, 119   ,   119, 108, 138,
   /*  111  */       141,   59,   -5  ,      93, 122   ,   166,  73, 151,
   /*  112  */       141,   25,  -76  ,     127, 127   ,   117,  77, 231,
   /*  113  */       141,  -18,  -68  ,     127, 127   ,    62, 102, 225,
   /*  114  */       143,   19,  -23  ,     127, 127   ,   128,  98, 176,
   /*  115  */       144,   29,   75  ,     127, 127   ,   173,  93,  49,
   /*  116  */       144,  -88,   85  ,     127, 127   ,     1, 143,  26,
   /*  117  */       144,  -41,   40  ,      78,  54   ,    86, 129, 106,
   /*  118  */       145,   -4,   47  ,     127, 127   ,   134, 115,  97,
   /*  119  */       146,  -22,  -22  ,     127, 127   ,    82, 120, 180,
   /*  120  */       146,   35,   20  ,     127, 127   ,   161,  96, 130,
   /*  121  */       146,   72,  -92  ,     127, 127   ,   161,  19, 253,
   /*  122  */       146,  106,    0  ,     127, 127   ,   208,   9, 150,
   /*  123  */       147,  102,   56  ,     102, 124   ,   223,   1,  81,
   /*  124  */       148,   62,   87  ,     127, 127   ,   207,  69,  17,
   /*  125  */       149,   70,   26  ,     127, 127   ,   194,  74, 125,
   /*  126  */       149,   53,  -62  ,      85, 109   ,   156,  74, 225,
   /*  127  */       150,  -75,   52  ,      64, 151   ,     2, 148,  98,
   /*  128  */       150,  -59,   12  ,     127, 127   ,     8, 142, 149,
   /*  129  */       151,  -40,  -50  ,     127, 127   ,    10, 128, 219,
   /*  130  */       151,   94,  -66  ,     127, 127   ,   189,  18, 230,
   /*  131  */       151,  -28,   71  ,     127, 127   ,   127, 131,  68,
   /*  132  */       152,    0,  -48  ,     127, 127   ,   109, 113, 216,
   /*  133  */       153,   47,  -27  ,     127, 127   ,   165,  93, 192,
   /*  134  */       154,  -24,   12  ,     127, 127   ,   105, 134, 152,
   /*  135  */       154,  -59,   93  ,     169, 184   ,   105, 145,   2,
   /*  136  */       156,   78,  -11  ,     127, 127   ,   198,  75, 176,
   /*  137  */       156,  110,  -29  ,     149, 157   ,   217,  18, 195,
   /*  138  */       156,   24,   48  ,     127, 127   ,   175, 113, 106,
   /*  139  */       156,  112,   28  ,     127, 127   ,   235,  15, 128,
   /*  140  */       157,    6,   -5  ,     133, 148   ,   138, 123, 174,
   /*  141  */       157,    8,   93  ,     127, 127   ,   180, 118,   6,
   /*  142  */       157,   62,   56  ,     167, 154   ,   209,  88,  94,
   /*  143  */       158,  -51,  -19  ,     127, 127   ,    18, 146, 194,
   /*  144  */       159,   19,  -74  ,     132, 165   ,   131, 105, 254,
   /*  145  */       160,  103,   94  ,     127, 127   ,   255,   0,   0,
   /*  146  */       160,  -98,   95  ,     127, 127   ,     4, 166,  10,
   /*  147  */       161,  -51,   38  ,     128, 178   ,    87, 153, 129,
   /*  148  */       161,   40,    4  ,     127, 127   ,   179, 110, 167,
   /*  149  */       161,   73,  -45  ,     130, 159   ,   192,  79, 222,
   /*  150  */       161,  -20,  -70  ,     129, 193   ,    79, 128, 255,
   /*  151  */       162,  -16,   44  ,     190, 236   ,   141, 140, 120,
   /*  152  */       163,   41,   83  ,     141, 175   ,   211, 106,  55,
   /*  153  */       164,  -28,   95  ,     127, 127   ,   155, 144,  17,
   /*  154  */       164,   94,   55  ,     156, 162   ,   242,  64, 103,
   /*  155  */       164,  -18,  -29  ,     150, 172   ,   105, 140, 212,
   /*  156  */       165,   82,   19  ,     139, 186   ,   223,  84, 151,
   /*  157  */       165,  123,    0  ,     136, 170   ,   247,   9, 173,
   /*  158  */       165,   21,  -35  ,     144, 140   ,   152, 122, 219,
   /*  159  */       166,  110,  -60  ,     127, 127   ,   223,  25, 243,
   /*  160  */       166,  -86,   63  ,     146, 214   ,     2, 171, 101,
   /*  161  */       167,   12,   24  ,     138, 166   ,   168, 133, 151,
   /*  162  */       168,   72,   97  ,     145, 174   ,   246,  83,   5,
   /*  163  */       168,  -63,    7  ,     143, 181   ,    21, 165, 177,
   /*  164  */       168,  -47,  -50  ,     194, 155   ,     6, 152, 243,
   /*  165  */       168,   47,  -64  ,     127, 127   ,   173, 103, 254,
   /*  166  */       169,   47,   34  ,     127, 127   ,   206, 115, 139,
   /*  167  */       170,    8,   67  ,     161, 152   ,   185, 136,  94,
   /*  168  */       170,  -51,   68  ,     131, 197   ,   119, 163,  95,
   /*  169  */       171,  -69,   99  ,     160, 202   ,   114, 169,  13,
   /*  170  */       171,   92,  -24  ,     127, 127   ,   226,  79, 209,
   /*  171  */       171,   54,  -25  ,     158, 137   ,   194, 111, 213,
   /*  172  */       172,  -29,    2  ,     134, 180   ,   113, 156, 186,
   /*  173  */       174,  127,  -31  ,     171, 183   ,   254,  18, 220,
   /*  174  */       175,   62,   65  ,     127, 127   ,   236, 108, 101,
   /*  175  */       175,   18,   99  ,     127, 127   ,   214, 132,  16,
   /*  176  */       175,    4,  -11  ,     127, 127   ,   155, 145, 204,
   /*  177  */       175,   -3,  -54  ,     127, 127   ,   128, 142, 255,
   /*  178  */       176,  -69,   38  ,     127, 127   ,    63, 178, 148,
   /*  179  */       177, -106,   99  ,     127, 127   ,     1, 190,  32,
   /*  180  */       178,  -33,   36  ,     127, 127   ,   134, 166, 150,
   /*  181  */       178,  -57,  -21  ,     127, 127   ,    25, 173, 224,
   /*  182  */       179,   74,  -51  ,     127, 127   ,   213, 100, 252,
   /*  183  */       179,   35,    5  ,     222, 200   ,   197, 134, 187,
   /*  184  */       180,  -22,   79  ,     168, 191   ,   172, 162,  86,
   /*  185  */       180,   71,    2  ,     127, 127   ,   229, 111, 191,
   /*  186  */       181,   82,   39  ,     127, 127   ,   251, 102, 145,
   /*  187  */       181,   21,   44  ,     142, 245   ,   202, 145, 141,
   /*  188  */       182,   29,  -47  ,     177, 203   ,   176, 135, 254,
   /*  189  */       183,   52,  101  ,     127, 127   ,   253, 119,  23,
   /*  190  */       184,  -92,   63  ,     204, 218   ,     5, 195, 122,
   /*  191  */       184,  -47,  105  ,     153, 212   ,   164, 176,   4,
   /*  192  */       185,   -6,   18  ,     127, 127   ,   167, 163, 181,
   /*  193  */       185,  -24,  -31  ,     127, 127   ,   118, 167, 242,
   /*  194  */       186,  -55,   11  ,     163, 147   ,    87, 184, 193,
   /*  195  */       187,  104,  -45  ,     182, 205   ,   251,  83, 254,
   /*  196  */       187,   27,   77  ,     189, 216   ,   229, 146,  98,
   /*  197  */       188,  -57,   63  ,     127, 127   ,   126, 188, 125,
   /*  198  */       188,   -4,  106  ,     127, 127   ,   214, 160,  13,
   /*  199  */       189,   51,   30  ,     127, 127   ,   234, 136, 168,
   /*  200  */       189,   47,  -21  ,     195, 215   ,   212, 137, 231,
   /*  201  */       190,  -10,   52  ,     198, 234   ,   185, 170, 141,
   /*  202  */       190,  -82,  108  ,     179, 206   ,   121, 197,  10,
   /*  203  */       192,    9,  -27  ,     127, 127   ,   173, 161, 245,
   /*  204  */       193,  -81,   30  ,     164, 210   ,    11, 203, 179,
   /*  205  */       193,   82,  -21  ,     127, 127   ,   248, 117, 235,
   /*  206  */       194, -116,  110  ,     127, 127   ,     5, 214,  10,
   /*  207  */       195,  -28,   -3  ,     127, 127   ,   139, 184, 221,
   /*  208  */       195,   21,    6  ,     176, 223   ,   204, 162, 208,
   /*  209  */       196,  -65,  -19  ,     127, 127   ,    11, 198, 244,
   /*  210  */       197,  -43,   36  ,     238, 251   ,   142, 194, 174,
   /*  211  */       197,   27,  108  ,     127, 127   ,   255, 153,  26,
   /*  212  */       197,  -35,   83  ,     127, 127   ,   181, 189, 100,
   /*  213  */       199,   20,   50  ,     225, 196   ,   227, 166, 153,
   /*  214  */       199, -102,   77  ,     127, 127   ,    26, 218, 117,
   /*  215  */       200,   59,    4  ,     185, 240   ,   247, 145, 215,
   /*  216  */       201,    1,   79  ,     127, 211   ,   223, 177, 111,
   /*  217  */       202,  -10,   24  ,     192, 255   ,   186, 186, 195,
   /*  218  */       202,  -54,  112  ,     135, 219   ,   179, 201,  18,
   /*  219  */       203,  -67,   77  ,     243, 246   ,   138, 210, 120,
   /*  220  */       204,  -19,  112  ,     127, 127   ,   223, 187,  22,
   /*  221  */       205,  -63,   12  ,     209, 244   ,    93, 212, 218,
   /*  222  */       205,   33,  -21  ,     188, 208   ,   219, 166, 254,
   /*  223  */       207,   -6,  -17  ,     127, 127   ,   178, 188, 254,
   /*  224  */       207,  -92,  107  ,     127, 127   ,   123, 223,  55,
   /*  225  */       208,   35,   24  ,     199, 232   ,   243, 170, 200,
   /*  226  */       208,  -24,   56  ,     127, 127   ,   193, 200, 158,
   /*  227  */       208,  -94,   46  ,     127, 127   ,     8, 228, 176,
   /*  228  */       212,  -59,   44  ,     227, 247   ,   139, 220, 182,
   /*  229  */       212, -126,  118  ,     127, 127   ,     5, 242,   3,
   /*  230  */       213,  -38,   -9  ,     207, 235   ,   141, 211, 253,
   /*  231  */       214,  -43,   89  ,     127, 127   ,   196, 214, 110,
   /*  232  */       215,    7,   42  ,     127, 127   ,   231, 194, 186,
   /*  233  */       216, -112,   83  ,     224, 239   ,     8, 244, 128,
   /*  234  */       217,   -6,   91  ,     127, 127   ,   243, 200, 108,
   /*  235  */       218,  -30,   26  ,     127, 127   ,   180, 215, 213,
   /*  236  */       218,    7,    8  ,     173, 187   ,   219, 198, 236,
   /*  237  */       218,  -67,  120  ,     229, 250   ,   187, 227,   1,
   /*  238  */       219,  -86,   18  ,     221, 228   ,    23, 238, 228,
   /*  239  */       219,  -80,   71  ,     127, 127   ,   130, 237, 151,
   /*  240  */       220,   41,   -5  ,     127, 127   ,   254, 181, 253,
   /*  241  */       221,  -33,  120  ,     220, 252   ,   234, 215,  15,
   /*  242  */       224,  -39,   61  ,     226, 254   ,   197, 227, 172,
   /*  243  */       225, -100,  114  ,     233, 237   ,   134, 249,  64,
   /*  244  */       227,  -57,    5  ,     127, 127   ,   133, 238, 255,
   /*  245  */       228,    1,   65  ,     248, 213   ,   252, 213, 169,
   /*  246  */       229,  -61,   93  ,     249, 241   ,   192, 241, 123,
   /*  247  */       229,  -73,   42  ,     127, 127   ,   131, 248, 208,
   /*  248  */       231,  -10,   30  ,     217, 201   ,   227, 223, 224,
   /*  249  */       233,  -26,   91  ,     231, 253   ,   241, 230, 129,
   /*  250  */       236,  -76,  126  ,     127, 127   ,   201, 253,  19,
   /*  251  */       237,  -43,   36  ,     230, 242   ,   193, 246, 225,
   /*  252  */       239,  -41,  122  ,     127, 127   ,   248, 242,  54,
   /*  253  */       242,  -49,   71  ,     127, 127   ,   211, 254, 178,
   /*  254  */       245,  -16,   58  ,     127, 127   ,   251, 243, 200,
   /*  255  */       255,  -12,   30  ,     127, 127   ,   255, 255, 255,

   //---------------------------------------------------------------------------
   // ROM Section: Palette
   //---------------------------------------------------------------------------
   // Default 2-color palette:
   11, 254,
   //darblue/lgith yellow: more extreme: 3, 254,
   //2, 250,
   // Default 4-color palette:
   32, 252, 145, 181,  // marine blue, light yellow, red, cyan    //8, 240, 187, 183,
   // Default 8-color palette:
   20, 102, 179, 241, 175, 114, 164, 255,
   // Default 16-color palette:
      2, 199, 198, 226,  20, 107, 121, 227,
     90, 162, 249, 251, 105, 143, 144, 245,
   // Default 32-color palette:
    30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,
    46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,
   // Default 64-color palette:
    62,  63,  64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,
    78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,
    94,  95,  96,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
   110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125,
   // Two last colors in 128-color palette
   126, 127,


   //---------------------------------------------------------------------------
   // ROM Section: Font
   //---------------------------------------------------------------------------
   // Macro helpers to add Glyphs in LITTLE ENDIAN.
#  define ADD_GLYPH8(X)   X
#  define ADD_GLYPH16(X)  (X & 0xFFu), ((X>>8) & 0xFFu)
#  define ADD_GLYPH32(X)  ADD_GLYPH16(X), ((X>>16) & 0xFFu), ((X>>24) & 0xFFu)
#  define ADD_GLYPH64(X)  ADD_GLYPH32(X), ((X>>32) & 0xFFu), ((X>>40) & 0xFFu),\
                                          ((X>>48) & 0xFFu), ((X>>56) & 0xFFu)

   // ASCII4's printable ascii (characters 32-126)
#  define ASCII4(g)  ADD_GLYPH16(UTIL_CAT(GLYPH16_ASCII4_, g))
   ASCII4(SPACE), ASCII4(EXCLAMATION_MARK), ASCII4(DOUBLE_QUOTE),
   ASCII4(HASHTAG), ASCII4(DOLLAR_SIGN), ASCII4(PERCENT_SIGN),
   ASCII4(AMPERSAND), ASCII4(SINGLE_QUOTE), ASCII4(LEFT_PARENTHESIS),
   ASCII4(RIGHT_PARENTHESIS), ASCII4(ASTERISK), ASCII4(PLUS_SIGN),
   ASCII4(COMMA), ASCII4(MINUS_SIGN), ASCII4(DOT), ASCII4(SLASH),
   ASCII4(0), ASCII4(1), ASCII4(2), ASCII4(3), ASCII4(4), ASCII4(5), ASCII4(6),
   ASCII4(7), ASCII4(8), ASCII4(9),
   ASCII4(COLON), ASCII4(SEMICOLON), ASCII4(LESS_THAN_SIGN), ASCII4(EQUAL_SIGN),
   ASCII4(GREATER_THAN_SIGN), ASCII4(QUESTION_MARK), ASCII4(AT_SIGN),
   ASCII4(A), ASCII4(B), ASCII4(C), ASCII4(D), ASCII4(E), ASCII4(F), ASCII4(G),
   ASCII4(H), ASCII4(I), ASCII4(J), ASCII4(K), ASCII4(L), ASCII4(M), ASCII4(N),
   ASCII4(O), ASCII4(P), ASCII4(Q), ASCII4(R), ASCII4(S), ASCII4(T), ASCII4(U),
   ASCII4(V), ASCII4(W), ASCII4(X), ASCII4(Y), ASCII4(Z),
   ASCII4(LEFT_SQUARE_BACKET), ASCII4(BACKSLASH), ASCII4(RIGHT_SQUARE_BRACKET),
   ASCII4(CARET), ASCII4(UNDERSCORE), ASCII4(BACKQUOTE),
   ASCII4(a), ASCII4(b), ASCII4(c), ASCII4(d), ASCII4(e), ASCII4(f), ASCII4(g),
   ASCII4(h), ASCII4(i), ASCII4(j), ASCII4(k), ASCII4(l), ASCII4(m), ASCII4(n),
   ASCII4(o), ASCII4(p), ASCII4(q), ASCII4(r), ASCII4(s), ASCII4(t), ASCII4(u),
   ASCII4(v), ASCII4(w), ASCII4(x), ASCII4(y), ASCII4(z),
   ASCII4(LEFT_CURLY_BACKET), ASCII4(VERTICAL_BAR), ASCII4(RIGHT_CURLY_BRACKET),
   ASCII4(TILDE),

   // ASCII5's printable ascii (characters 32-126)
#  define ASCII5(g)  ADD_GLYPH32(UTIL_CAT(GLYPH32_ASCII5_, g))
   ASCII5(SPACE), ASCII5(EXCLAMATION_MARK), ASCII5(DOUBLE_QUOTE),
   ASCII5(HASHTAG), ASCII5(DOLLAR_SIGN), ASCII5(PERCENT_SIGN),
   ASCII5(AMPERSAND), ASCII5(SINGLE_QUOTE), ASCII5(LEFT_PARENTHESIS),
   ASCII5(RIGHT_PARENTHESIS), ASCII5(ASTERISK), ASCII5(PLUS_SIGN),
   ASCII5(COMMA), ASCII5(MINUS_SIGN), ASCII5(DOT), ASCII5(SLASH),
   ASCII5(0), ASCII5(1), ASCII5(2), ASCII5(3), ASCII5(4), ASCII5(5), ASCII5(6),
   ASCII5(7), ASCII5(8), ASCII5(9),
   ASCII5(COLON), ASCII5(SEMICOLON), ASCII5(LESS_THAN_SIGN), ASCII5(EQUAL_SIGN),
   ASCII5(GREATER_THAN_SIGN), ASCII5(QUESTION_MARK), ASCII5(AT_SIGN),
   ASCII5(A), ASCII5(B), ASCII5(C), ASCII5(D), ASCII5(E), ASCII5(F), ASCII5(G),
   ASCII5(H), ASCII5(I), ASCII5(J), ASCII5(K), ASCII5(L), ASCII5(M), ASCII5(N),
   ASCII5(O), ASCII5(P), ASCII5(Q), ASCII5(R), ASCII5(S), ASCII5(T), ASCII5(U),
   ASCII5(V), ASCII5(W), ASCII5(X), ASCII5(Y), ASCII5(Z),
   ASCII5(LEFT_SQUARE_BACKET), ASCII5(BACKSLASH), ASCII5(RIGHT_SQUARE_BRACKET),
   ASCII5(CARET), ASCII5(UNDERSCORE), ASCII5(BACKQUOTE),
   ASCII5(a), ASCII5(b), ASCII5(c), ASCII5(d), ASCII5(e), ASCII5(f), ASCII5(g),
   ASCII5(h), ASCII5(i), ASCII5(j), ASCII5(k), ASCII5(l), ASCII5(m), ASCII5(n),
   ASCII5(o), ASCII5(p), ASCII5(q), ASCII5(r), ASCII5(s), ASCII5(t), ASCII5(u),
   ASCII5(v), ASCII5(w), ASCII5(x), ASCII5(y), ASCII5(z),
   ASCII5(LEFT_CURLY_BACKET), ASCII5(VERTICAL_BAR), ASCII5(RIGHT_CURLY_BRACKET),
   ASCII5(TILDE),

   // ASCII6's printable ascii (characters 32-126)
   #  define ASCII6(g)  ADD_GLYPH32(UTIL_CAT(GLYPH32_ASCII6_, g))
   ASCII6(SPACE), ASCII6(EXCLAMATION_MARK), ASCII6(DOUBLE_QUOTE),
   ASCII6(HASHTAG), ASCII6(DOLLAR_SIGN), ASCII6(PERCENT_SIGN),
   ASCII6(AMPERSAND), ASCII6(SINGLE_QUOTE), ASCII6(LEFT_PARENTHESIS),
   ASCII6(RIGHT_PARENTHESIS), ASCII6(ASTERISK), ASCII6(PLUS_SIGN),
   ASCII6(COMMA), ASCII6(MINUS_SIGN), ASCII6(DOT), ASCII6(SLASH),
   ASCII6(0), ASCII6(1), ASCII6(2), ASCII6(3), ASCII6(4), ASCII6(5), ASCII6(6),
   ASCII6(7), ASCII6(8), ASCII6(9),
   ASCII6(COLON), ASCII6(SEMICOLON), ASCII6(LESS_THAN_SIGN), ASCII6(EQUAL_SIGN),
   ASCII6(GREATER_THAN_SIGN), ASCII6(QUESTION_MARK), ASCII6(AT_SIGN),
   ASCII6(A), ASCII6(B), ASCII6(C), ASCII6(D), ASCII6(E), ASCII6(F), ASCII6(G),
   ASCII6(H), ASCII6(I), ASCII6(J), ASCII6(K), ASCII6(L), ASCII6(M), ASCII6(N),
   ASCII6(O), ASCII6(P), ASCII6(Q), ASCII6(R), ASCII6(S), ASCII6(T), ASCII6(U),
   ASCII6(V), ASCII6(W), ASCII6(X), ASCII6(Y), ASCII6(Z),
   ASCII6(LEFT_SQUARE_BACKET), ASCII6(BACKSLASH), ASCII6(RIGHT_SQUARE_BRACKET),
   ASCII6(CARET), ASCII6(UNDERSCORE), ASCII6(BACKQUOTE),
   ASCII6(a), ASCII6(b), ASCII6(c), ASCII6(d), ASCII6(e), ASCII6(f), ASCII6(g),
   ASCII6(h), ASCII6(i), ASCII6(j), ASCII6(k), ASCII6(l), ASCII6(m), ASCII6(n),
   ASCII6(o), ASCII6(p), ASCII6(q), ASCII6(r), ASCII6(s), ASCII6(t), ASCII6(u),
   ASCII6(v), ASCII6(w), ASCII6(x), ASCII6(y), ASCII6(z),
   ASCII6(LEFT_CURLY_BACKET), ASCII6(VERTICAL_BAR), ASCII6(RIGHT_CURLY_BRACKET),
   ASCII6(TILDE),

      // ASCII7's printable ascii (characters 32-126)
#  define ASCII7(g)  ADD_GLYPH32(UTIL_CAT(GLYPH32_ASCII7_, g))
   ASCII7(SPACE), ASCII7(EXCLAMATION_MARK), ASCII7(DOUBLE_QUOTE),
   ASCII7(HASHTAG), ASCII7(DOLLAR_SIGN), ASCII7(PERCENT_SIGN),
   ASCII7(AMPERSAND), ASCII7(SINGLE_QUOTE), ASCII7(LEFT_PARENTHESIS),
   ASCII7(RIGHT_PARENTHESIS), ASCII7(ASTERISK), ASCII7(PLUS_SIGN),
   ASCII7(COMMA), ASCII7(MINUS_SIGN), ASCII7(DOT), ASCII7(SLASH),
   ASCII7(0), ASCII7(1), ASCII7(2), ASCII7(3), ASCII7(4), ASCII7(5), ASCII7(6),
   ASCII7(7), ASCII7(8), ASCII7(9),
   ASCII7(COLON), ASCII7(SEMICOLON), ASCII7(LESS_THAN_SIGN), ASCII7(EQUAL_SIGN),
   ASCII7(GREATER_THAN_SIGN), ASCII7(QUESTION_MARK), ASCII7(AT_SIGN),
   ASCII7(A), ASCII7(B), ASCII7(C), ASCII7(D), ASCII7(E), ASCII7(F), ASCII7(G),
   ASCII7(H), ASCII7(I), ASCII7(J), ASCII7(K), ASCII7(L), ASCII7(M), ASCII7(N),
   ASCII7(O), ASCII7(P), ASCII7(Q), ASCII7(R), ASCII7(S), ASCII7(T), ASCII7(U),
   ASCII7(V), ASCII7(W), ASCII7(X), ASCII7(Y), ASCII7(Z),
   ASCII7(LEFT_SQUARE_BACKET), ASCII7(BACKSLASH), ASCII7(RIGHT_SQUARE_BRACKET),
   ASCII7(CARET), ASCII7(UNDERSCORE), ASCII7(BACKQUOTE),
   ASCII7(a), ASCII7(b), ASCII7(c), ASCII7(d), ASCII7(e), ASCII7(f), ASCII7(g),
   ASCII7(h), ASCII7(i), ASCII7(j), ASCII7(k), ASCII7(l), ASCII7(m), ASCII7(n),
   ASCII7(o), ASCII7(p), ASCII7(q), ASCII7(r), ASCII7(s), ASCII7(t), ASCII7(u),
   ASCII7(v), ASCII7(w), ASCII7(x), ASCII7(y), ASCII7(z),
   ASCII7(LEFT_CURLY_BACKET), ASCII7(VERTICAL_BAR), ASCII7(RIGHT_CURLY_BRACKET),
   ASCII7(TILDE),

   // Sitelen Pona characters, height 5 (pu120:ale-wile, pu+3:kin/namako/oko)
#  define SP5(g)   ADD_GLYPH64(UTIL_CAT(GLYPH64_TOKIPONA6_, g))
   SP5(A), SP5(AKESI), SP5(ALA), SP5(ALASA), SP5(ALE), SP5(ANPA), SP5(ANTE),
   SP5(ANU), SP5(AWEN), SP5(E), SP5(EN), SP5(ESUN), SP5(IJO), SP5(IKE),
   SP5(ILO), SP5(INSA), SP5(JAKI), SP5(JAN), SP5(JELO), SP5(JO), SP5(KALA),
   SP5(KALAMA), SP5(KAMA), SP5(KASI), SP5(KEN), SP5(KEPEKEN), SP5(KILI),
   SP5(KIWEN), SP5(KO), SP5(KON), SP5(KULE), SP5(KULUPU), SP5(KUTE), SP5(LA),
   SP5(LAPE), SP5(LASO), SP5(LAWA), SP5(LEN), SP5(LETE), SP5(LI), SP5(LILI),
   SP5(LINJA), SP5(LIPU), SP5(LOJE), SP5(LON), SP5(LUKA), SP5(LUKIN), SP5(LUPA),
   SP5(MA), SP5(MAMA), SP5(MANI), SP5(MELI), SP5(MI), SP5(MIJE), SP5(MOKU),
   SP5(MOLI), SP5(MONSI), SP5(MU), SP5(MUN), SP5(MUSI), SP5(MUTE), SP5(NANPA),
   SP5(NASA), SP5(NASIN), SP5(NENA), SP5(NI), SP5(NIMI), SP5(NOKA), SP5(O),
   SP5(OLIN), SP5(ONA), SP5(OPEN), SP5(PAKALA), SP5(PALI), SP5(PALISA),
   SP5(PAN), SP5(PANA), SP5(PI), SP5(PILIN), SP5(PIMEJA), SP5(PINI), SP5(PIPI),
   SP5(POKA), SP5(POKI), SP5(PONA), SP5(PU), SP5(SAMA), SP5(SELI), SP5(SELO),
   SP5(SEME), SP5(SEWI), SP5(SIJELO), SP5(SIKE), SP5(SIN), SP5(SINA),
   SP5(SINPIN), SP5(SITELEN), SP5(SONA), SP5(SOWELI), SP5(SULI), SP5(SUNO),
   SP5(SUPA), SP5(SUWI), SP5(TAN), SP5(TASO), SP5(TAWA), SP5(TELO), SP5(TENPO),
   SP5(TOKI), SP5(TOMO), SP5(TU), SP5(UNPA), SP5(UTA), SP5(UTALA), SP5(WALO),
   SP5(WAN), SP5(WASO), SP5(WAWA), SP5(WEKA), SP5(WILE), SP5(KIN), SP5(NAMAKO),
   SP5(OKO),

   // Sitelen Pona characters, height 6 (pu120:ale-wile, pu+3:kin/namako/oko)
#  define SP6(g)   ADD_GLYPH64(UTIL_CAT(GLYPH64_TOKIPONA6_, g))
   SP6(A), SP6(AKESI), SP6(ALA), SP6(ALASA), SP6(ALE), SP6(ANPA), SP6(ANTE),
   SP6(ANU), SP6(AWEN), SP6(E), SP6(EN), SP6(ESUN), SP6(IJO), SP6(IKE),
   SP6(ILO), SP6(INSA), SP6(JAKI), SP6(JAN), SP6(JELO), SP6(JO), SP6(KALA),
   SP6(KALAMA), SP6(KAMA), SP6(KASI), SP6(KEN), SP6(KEPEKEN), SP6(KILI),
   SP6(KIWEN), SP6(KO), SP6(KON), SP6(KULE), SP6(KULUPU), SP6(KUTE), SP6(LA),
   SP6(LAPE), SP6(LASO), SP6(LAWA), SP6(LEN), SP6(LETE), SP6(LI), SP6(LILI),
   SP6(LINJA), SP6(LIPU), SP6(LOJE), SP6(LON), SP6(LUKA), SP6(LUKIN), SP6(LUPA),
   SP6(MA), SP6(MAMA), SP6(MANI), SP6(MELI), SP6(MI), SP6(MIJE), SP6(MOKU),
   SP6(MOLI), SP6(MONSI), SP6(MU), SP6(MUN), SP6(MUSI), SP6(MUTE), SP6(NANPA),
   SP6(NASA), SP6(NASIN), SP6(NENA), SP6(NI), SP6(NIMI), SP6(NOKA), SP6(O),
   SP6(OLIN), SP6(ONA), SP6(OPEN), SP6(PAKALA), SP6(PALI), SP6(PALISA),
   SP6(PAN), SP6(PANA), SP6(PI), SP6(PILIN), SP6(PIMEJA), SP6(PINI), SP6(PIPI),
   SP6(POKA), SP6(POKI), SP6(PONA), SP6(PU), SP6(SAMA), SP6(SELI), SP6(SELO),
   SP6(SEME), SP6(SEWI), SP6(SIJELO), SP6(SIKE), SP6(SIN), SP6(SINA),
   SP6(SINPIN), SP6(SITELEN), SP6(SONA), SP6(SOWELI), SP6(SULI), SP6(SUNO),
   SP6(SUPA), SP6(SUWI), SP6(TAN), SP6(TASO), SP6(TAWA), SP6(TELO), SP6(TENPO),
   SP6(TOKI), SP6(TOMO), SP6(TU), SP6(UNPA), SP6(UTA), SP6(UTALA), SP6(WALO),
   SP6(WAN), SP6(WASO), SP6(WAWA), SP6(WEKA), SP6(WILE), SP6(KIN), SP6(NAMAKO),
   SP6(OKO),

   // Sitelen Pona characters, height 7 (pu120:ale-wile, pu+3:kin/namako/oko)
#  define SP7(g)   ADD_GLYPH64(UTIL_CAT(GLYPH64_TOKIPONA7_, g))
   SP7(A), SP7(AKESI), SP7(ALA), SP7(ALASA), SP7(ALE), SP7(ANPA), SP7(ANTE),
   SP7(ANU), SP7(AWEN), SP7(E), SP7(EN), SP7(ESUN), SP7(IJO), SP7(IKE),
   SP7(ILO), SP7(INSA), SP7(JAKI), SP7(JAN), SP7(JELO), SP7(JO), SP7(KALA),
   SP7(KALAMA), SP7(KAMA), SP7(KASI), SP7(KEN), SP7(KEPEKEN), SP7(KILI),
   SP7(KIWEN), SP7(KO), SP7(KON), SP7(KULE), SP7(KULUPU), SP7(KUTE), SP7(LA),
   SP7(LAPE), SP7(LASO), SP7(LAWA), SP7(LEN), SP7(LETE), SP7(LI), SP7(LILI),
   SP7(LINJA), SP7(LIPU), SP7(LOJE), SP7(LON), SP7(LUKA), SP7(LUKIN), SP7(LUPA),
   SP7(MA), SP7(MAMA), SP7(MANI), SP7(MELI), SP7(MI), SP7(MIJE), SP7(MOKU),
   SP7(MOLI), SP7(MONSI), SP7(MU), SP7(MUN), SP7(MUSI), SP7(MUTE), SP7(NANPA),
   SP7(NASA), SP7(NASIN), SP7(NENA), SP7(NI), SP7(NIMI), SP7(NOKA), SP7(O),
   SP7(OLIN), SP7(ONA), SP7(OPEN), SP7(PAKALA), SP7(PALI), SP7(PALISA),
   SP7(PAN), SP7(PANA), SP7(PI), SP7(PILIN), SP7(PIMEJA), SP7(PINI), SP7(PIPI),
   SP7(POKA), SP7(POKI), SP7(PONA), SP7(PU), SP7(SAMA), SP7(SELI), SP7(SELO),
   SP7(SEME), SP7(SEWI), SP7(SIJELO), SP7(SIKE), SP7(SIN), SP7(SINA),
   SP7(SINPIN), SP7(SITELEN), SP7(SONA), SP7(SOWELI), SP7(SULI), SP7(SUNO),
   SP7(SUPA), SP7(SUWI), SP7(TAN), SP7(TASO), SP7(TAWA), SP7(TELO), SP7(TENPO),
   SP7(TOKI), SP7(TOMO), SP7(TU), SP7(UNPA), SP7(UTA), SP7(UTALA), SP7(WALO),
   SP7(WAN), SP7(WASO), SP7(WAWA), SP7(WEKA), SP7(WILE), SP7(KIN), SP7(NAMAKO),
   SP7(OKO),

   //---------------------------------------------------------------------------
   // ROM Section: Wav Header
   //
   // Wav format header, see: http://soundfile.sapp.org/doc/WaveFormat/
   // numbers are in little endian in the wav file (it doesn't matter)
   //---------------------------------------------------------------------------
   // "RIFF" chunk description:
    'R', 'I', 'F', 'F',  // ChunkID       -> "RIFF"
   0xff,0xff,0xff,0xff,  // ChunkSize     -> (*)
    'W', 'A', 'V', 'E',  // Format        -> "WAVE"
   // "fmt" sub chunk:
    'f', 'm', 't', ' ',  // SubChunk1     -> "fmt "
   0x10,0x00,0x00,0x00,  // SubChunk1Size -> number of bytes to follow -> 16
   0x01,0x00,            // AudioFormat   -> 1 for PCM
   0x01,0x00,            // NumChannels   -> 1 (mono)
   0x40,0x1f,0x00,0x00,  // SampleRate    -> 8000 Hz
   0x40,0x1f,0x00,0x00,  // ByteRate      -> SampleRate * NumChannels *
                         //                  BitsPerSample/8 -> 8000*1*8/8
   0x01,0x00,            // BlockAlign    -> NumChannels * BitsPerSample/8 -> 1
   0x08,0x00,            // BitsPerSample -> 8
   // "data" sub chunk:
    'd', 'a', 't', 'a',  // SubChunk2     -> "data"
   0xff,0xff,0xff,0xff,  // SubChunk2Size -> (*) normally number of bytes in the
                         //                      data
   // (*) Simply put the max value instead of real length
   //     Software dealing with WAV know this means the rest of the file.


   //---------------------------------------------------------------------------
   // ROM Section: ...
   //---------------------------------------------------------------------------
   // Static Strings...
   // Error Messages strings
/*
   ALE,LI,PONA, 0
   PAKALA,A,'.',MI,SONA,ALA,E,TAN, ' ','(','U','n','k','n','o','w','n',' ','E','r','r','o','r',')','\0',


 ' ','(','N','o',' ','E','r','r','o','r',')','\0',
   PAKALA,A, MI,SONA,ALA,E,TAN, ' ','(','U','n','k','n','o','w','n',' ','E','r','r','o','r',')','\0',
*/

   //---------------------------------------------------------------------------
   // ROM Section: URL
   //---------------------------------------------------------------------------
   'h','t','t','p','s',':','/','/','g','i','t','h','u','b','.','c','o','m','/',
   'p','o','l','i','j','a','n','/','k','o','n','p','u','\0',

};
