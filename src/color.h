#ifndef  KONPU_COLOR_H_
#define  KONPU_COLOR_H_
#include "arch.h"
#include "video_mode.h"


//------------------------------------------------------------------------------
// Palettes
//
// The overall palette can contains 128 entries (which are index to the 256
// master colors). It allows distinct for 2,4,8,8,16,32,64 colors subpalettes
// in distinct areas.
//------------------------------------------------------------------------------

// TODO: Maybe those should be called `VideoResetPalette<N>()`
#define ColorResetPalette2()  memcpy(Video.palette2,  Rom.default_palette2,  2)
#define ColorResetPalette4()  memcpy(Video.palette4,  Rom.default_palette4,  4)
#define ColorResetPalette8()  memcpy(Video.palette8,  Rom.default_palette8,  8)
#define ColorResetPalette16() memcpy(Video.palette16, Rom.default_palette16, 16)
#define ColorResetPalette32() memcpy(Video.palette32, Rom.default_palette32, 32)
#define ColorResetPalette64() memcpy(Video.palette64, Rom.default_palette64, 64)
#define ColorResetPalette128() memcpy(Video.palette128, Rom.default_palette128, 128)
#define ColorResetPalettes()  ColorResetPalette128()


// Return the palette in-use or NULL (in 256 color modes)
//
// When using this function to lookup for a color index, make sure to account
// for a possible NULL return value.
// ```
//    uint8_t color;
//    // Lookup up for the corresponding Konpu 256-color
//    uint8_t *palette = ColorPalette();
//    uint8_t color256 = (palette)? palette[color] : color;
// ```
static inline uint8_t *ColorPalette(void)
{
   int color_depth = ColorDepth();
   switch(color_depth) {
      case 8:  return NULL;        // 256 colors
#if (VIDEO_FACTOR_ % 7 != 0)
      case 7:  return Video.palette128; // 128 color palette
#endif
      default: return Video.palettes + (1 << color_depth);
               // ^-- For n=2,4,8,16,32,64 colors, palette starts at:
               //     Video.palette + n
   }
}

// Given a value, return it normalized between 0 and the current color count.
#define ColorNormalize(color) \
   ((color) & ((1 << ColorDepth()) - 1))
   // We want to return: color % current number of color.
   // As the number of color is a power of 2 (i.e. it's 1 << ColorDepth()),
   // we can optimize the modulo as: color & (current number of color - 1)
   //
   // In fact, the & technique makes the color normalization correct for
   // negative numbers too (on two's complement systems, which Konpu [and C23]
   // assumes)

//------------------------------------------------------------------------------
// Color Constants for INDEXED color indices
// MAYBE DELETE as this file deals with "true" colors, not index
//------------------------------------------------------------------------------

#define COLOR16_TTY_BLACK          0
#define COLOR16_TTY_RED            1
#define COLOR16_TTY_GREEN          2
#define COLOR16_TTY_YELLOW         3
#define COLOR16_TTY_BLUE           4
#define COLOR16_TTY_MAGENTA        5
#define COLOR16_TTY_CYAN           6
#define COLOR16_TTY_WHITE          7
#define COLOR16_TTY_HIGH_BLACK     8
#define COLOR16_TTY_HIGH_RED       9
#define COLOR16_TTY_HIGH_GREEN    10
#define COLOR16_TTY_HIGH_YELLOW   11
#define COLOR16_TTY_HIGH_BLUE     12
#define COLOR16_TTY_HIGH_MAGENTA  13
#define COLOR16_TTY_HIGH_CYAN     14
#define COLOR16_TTY_HIGH_WHITE    15




//------------------------------------------------------------------------------
// True Colors
//------------------------------------------------------------------------------

//#include <math.h>
// trigonometric functions
extern float cosf(float x);
extern float sinf(float x);
extern float atan2f(float y, float x);
// power functions
extern float sqrtf(float x);
extern float cbrtf(float x);
extern float powf(float x, float y);
// misc math.
extern float roundf(float x);

// TODO: replace and remove remove!
#define COLOR_ROM(i)  (Rom.color_info[i])


//------------------------------------------------------------------------------

// In Konpu, a color is a number in 0..255.
// The numbers are arranged so that a higher number mean more perceptual
// lightness (thus 0 is black and 255 is white).
// The ROM contains info about the color values in OkLab and sRGB as series of
// 8 bytes per color starting at address KONPU_ROM_COLOR. The information is
// stored as follow:
// - The first three bytes describes the OkLab L,a,b attributes of the color
//   (with some transformation).
// - The next two bytes are data partionning the Konpu colors in OkLab space
//   in the form a k-d tree. Its main purpose is to speed up reverse palette
//   lookup function which return a Konpu color from a "true" color.
// - The last three bytes are gamma-encoded sRGB values in 0..255 as would be
//   displayed on screen.
//
// Some colors modes can display all 255 colors, but many cannot, in which case
// a palette is used.


//------------------------------------------------------------------------------

// An OkLab color
struct ColorLABf  { float L, a, b; };

// An OkLab color, but components are integers and scaled by a factor of 510
struct ColorLABi  { int   L, a, b; };

// An OkLCh color, ie. a polar OkLab color (Lab, Chroma, Hue)
struct ColorLCHf  { float L, C, h; };


// Conversion LABf -> LABi (ie multiply by 510)
static inline struct ColorLABi
ColorLABfToLABi(struct ColorLABf lab) {
   return (struct ColorLABi)
   { roundf(lab.L * 510.f), roundf(lab.a * 510.f), roundf(lab.b * 510.f) };
}

// Conversion LABi -> LABf (ie divide by 510)
static inline struct ColorLABf
ColorLABiToLABf(struct ColorLABi lab)
{ return (struct ColorLABf) { lab.L / 510.f, lab.a / 510.f, lab.b / 510.f }; }


// Return a OkLab color (with integer components multiplied by 510) for the
// given color
static inline struct ColorLABi ColorToLABi(int color)
{
   int i = color << 3;
   return (struct ColorLABi) {
      .L = COLOR_ROM(i) << 1,
      .a = (int8_t)(COLOR_ROM(1 + i)) + 12,
      .b = (int8_t)(COLOR_ROM(2 + i)) - 30,
   };
}

// Return a OkLab color (float) for the given color
static inline struct ColorLABf ColorToLABf(int color)
{
   int i = color << 3;
   return (struct ColorLABf) {
      .L = COLOR_ROM(i) / 255.f,
      .a = ((int8_t)(COLOR_ROM(1 + i)) + 12) / 510.f,
      .b = ((int8_t)(COLOR_ROM(2 + i)) - 30) / 510.f,
   };
}

// Return a OkLCh color (float) for the given color
static inline struct ColorLCHf ColorToLCHf(int color)
{
   int i = color << 3;
   int32_t a_x510 = (int8_t)(COLOR_ROM(1 + i)) + 12;
   int32_t b_x510 = (int8_t)(COLOR_ROM(2 + i)) - 30;
   return (struct ColorLCHf) {
      .L = COLOR_ROM(i) / 255.f,
      .C = sqrtf(a_x510 * a_x510 + b_x510 * b_x510) / 510.f,
      .h = atan2f(b_x510, a_x510),
   };
}

// Return a color close to the given OkLab color
int ColorFromLABf(struct ColorLABf lab);

// Return a color close to the given OkLab color
int ColorFromLABi(struct ColorLABi lab);
// Othwerwise, we would need a specific inline function or macro:
// #define ColorFromLABi(lab)   ColorFromLABf(ColorLABiToLABf((lab)))


// Conversion LABf -> LCHf
static inline struct ColorLCHf
ColorLABfToLCHf(struct ColorLABf lab) {
   return (struct ColorLCHf)
      { lab.L, sqrtf(lab.a*lab.a + lab.b*lab.b), atan2f(lab.b, lab.a) };
}

// Conversion LCHf -> LABf
static inline struct ColorLABf
ColorLCHfToLABf(struct ColorLCHf lch)
{ return (struct ColorLABf){lch.L, lch.C * cosf(lch.h), lch.C * sinf(lch.h)}; }



// Return the 'L' component (= perceptual lightness) of a color (integer scale)
static inline int ColorLightness(int color)
{ return COLOR_ROM(color << 3) << 1; }

// Return the 'a' component ("green" to "red") of a color (integer scale)
static inline int ColorA(int color)
{ return (int8_t)(COLOR_ROM(1 + (color << 3))) + 12; }

// Return the 'b' component ("blue" to "yellow") of a color (integer scale)
static inline int ColorB(int color)
{ return (int8_t)(COLOR_ROM(2 + (color << 3))) - 30; }

// Return the distance squared between two colors
static inline int32_t ColorDistanceSquared(int color1, int color2)
{
   int i1 = color1 << 3;
   int i2 = color2 << 3;
   int_fast32_t dL = (COLOR_ROM(i1) - COLOR_ROM(i2)) << 1;
   int_fast32_t da = COLOR_ROM(1 + i1) - COLOR_ROM(1 + i2);
   int_fast32_t db = COLOR_ROM(2 + i1) - COLOR_ROM(2 + i2);
   return dL*dL + da*da + db*db;
}

// Return a color's chroma (ie "colorfullness") squared (integer scale)
static inline int32_t ColorChromaSquared(int color)
{
   int i = color << 3;
   int_fast32_t a = (int8_t)(COLOR_ROM(1 + i)) + 12;
   int_fast32_t b = (int8_t)(COLOR_ROM(2 + i)) - 30;
   return a*a + b*b;
}

// Return a color's saturation squared
// (ie saturation = chroma in proportion of its luminance)
static inline int32_t ColorSaturationSquared(int color)
{
   if (color == 0) return INT32_MAX;
   int i = color << 3;
   int_fast32_t L_half = COLOR_ROM(i);
   int_fast32_t a = (int8_t)(COLOR_ROM(1 + i)) + 12;
   int_fast32_t b = (int8_t)(COLOR_ROM(2 + i)) - 30;
   return (4*510) * (a*a + b*b) / (L_half*L_half);
}


#define ColorSaturation(color)         sqrtf(ColorSaturationSquared((color)))
#define ColorChroma(color)             sqrtf(ColorChromaSquared((color)))
#define ColorDistance(color1, color2)  \
   sqrtf(ColorDistanceSquared((color1), (color2)))


// Return the hue of a color as a angle in [-pi, pi]
// TODO: could we maybe have some fast "atan2i" maybe giving the angle in
//       degrees as an integer or in 0..255?
static inline float ColorHue(int color)
{
   int i = color << 3;
   int a = (int8_t)(COLOR_ROM(1 + i)) + 12;
   int b = (int8_t)(COLOR_ROM(2 + i)) - 30;
   return atan2f(b, a);
}


//------------------------------------------------------------------------------
// sRGB Colors (officially known as IEC 61966-2-1:1999)
// https://en.wikipedia.org/wiki/SRGB
//
// Konpu prefers to use OkLab which is a perceptual color space, thus it is not
// recommended to perform color work directly in sRGB. But Konpu C API provides
// sRGB utilities. And colors are assumed to be shown on a sRGB display device.
//------------------------------------------------------------------------------

// Return an array with the 3 r,g,b components (gamma-encoded sRGB) of a color
static inline const uint8_t* ColorToRGB(int color)
{ return Rom.color_info + 5 + (color << 3); }
// TODO ^-- replace with a proper structure for color_info
// and DELETE: // { return ROM + ROM_COLOR + 5 + (color << 3); }

// sRGB color, linear float components in [0,1]
struct ColorRGBf { float   r, g, b; };

// sRGB color, gamma-encoded components in 0-255
struct ColorRGBi { uint8_t r, g, b; };

// Create a RGBi color from a 24-bit value encoded in "RGB888 format".
// Note: rgb888 is not supposed to have an alpha channel, so if you have a color
//       c in ARGB888 format, you wanna use `ColorRGBi(c & 0xFFFFFF)`.
static inline struct ColorRGBi
ColorRGBi(uint32_t rgb888) {
   return (struct ColorRGBi)
      { rgb888 >> 16, (rgb888 >> 8) & 0xFF, rgb888 & 0xFF };
}


// Transform an 8-bit integer gamma-encoded sRGB component into a linearized
// component (as a float in [0-1]).
// This is the inverse sRGB gamma function (altered to accept a 0-255 int input)
// See: https://en.wikipedia.org/wiki/SRGB#Transfer_function_(%22gamma%22)
static inline float ColorRGBLinearize(int i)
{ return (i > 10) ? powf((i + 14.025f) / 269.025f, 2.4f) : i / 3294.6f; }

// Transform a [0-1] linear sRGB component into a gamma-encoded 8-bit component.
// This is the sRGB gamma function (multiplied by 255).
// See: https://en.wikipedia.org/wiki/SRGB#Transfer_function_(%22gamma%22)
static inline int ColorRGBGammaify(float x) {
   return roundf( (x <= .0031308f)? x * 3294.6f :
                                    269.025f * powf(x, (1.f/2.4f)) - 14.025f );
}


// A squared distance that can be applied directly on gamma-encoded sRGB colors.
// While *not* a perceptual distance, it has the advantage of being fast to
// compute while being "more perceptual" than the Euclidean distance.
// See: https://en.wikipedia.org/wiki/Color_difference
// This is never used in ilo Konpu and is provided only for fun/convenience.
static inline int32_t
ColorRGBRedmeanDistanceSquared(struct ColorRGBi c1, struct ColorRGBi c2)
{
   int_fast32_t dr = c1.r - c2.r;
   int_fast32_t dg = c1.g - c2.g;
   int_fast32_t db = c1.b - c2.b;
   int_fast32_t rm = (c1.r + c2.r) >> 1;
   return  (((512 + rm) * dr*dr) >> 8)
         + ((dg*dg) << 2)
         + (((767 - rm) * db*db) >> 8);
   // TODO: this looks equivalent but "faster", right?
   //return ((512 + rm) * dr*dr + ((dg*dg) << 10) + (767 - rm) * db*db) >> 8;
}

// Conversion RGBf -> RGBi
static inline struct ColorRGBi
ColorRGBfToRGBi(struct ColorRGBf rgb) {
   return (struct ColorRGBi) {
      ColorRGBGammaify(rgb.r), ColorRGBGammaify(rgb.g), ColorRGBGammaify(rgb.b),
   };
}

// Conversion RGBi -> RGBf
static inline struct ColorRGBf
ColorRGBiToRGBf(struct ColorRGBi rgb) {
   return (struct ColorRGBf) {
   ColorRGBLinearize(rgb.r), ColorRGBLinearize(rgb.g), ColorRGBLinearize(rgb.b),
   };
}

//------------------------------------------------------------------------------
// The Code for the OkLab <> RGB conversion functions is adapted from the OkLab
// reference implementation. Copyright(c) 2021 Björn Ottosson
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
// See: https://en.wikipedia.org/wiki/Oklab#Conversion_from_sRGB
// and: https://bottosson.github.io/posts/oklab/

// Convert a (linear) sRGB color to OkLab
static inline struct ColorLABf
ColorRGBfToLABf(struct ColorRGBf c)
{
   float l = 0.4122214708f * c.r + 0.5363325363f * c.g + 0.0514459929f * c.b;
   float m = 0.2119034982f * c.r + 0.6806995451f * c.g + 0.1073969566f * c.b;
   float s = 0.0883024619f * c.r + 0.2817188376f * c.g + 0.6299787005f * c.b;

   float l_ = cbrtf(l);
   float m_ = cbrtf(m);
   float s_ = cbrtf(s);

   return (struct ColorLABf) {
      0.2104542553f * l_ + 0.7936177850f * m_ - 0.0040720468f * s_,
      1.9779984951f * l_ - 2.4285922050f * m_ + 0.4505937099f * s_,
      0.0259040371f * l_ + 0.7827717662f * m_ - 0.8086757660f * s_,
   };
}

// Convert an OkLab color into a (linear) sRGB color
static inline struct ColorRGBf
ColorLABfToRGBf(struct ColorLABf c)
{
   float l_ = c.L + 0.3963377774f * c.a + 0.2158037573f * c.b;
   float m_ = c.L - 0.1055613458f * c.a - 0.0638541728f * c.b;
   float s_ = c.L - 0.0894841775f * c.a - 1.2914855480f * c.b;

   float l = l_ * l_ * l_;
   float m = m_ * m_ * m_;
   float s = s_ * s_ * s_;

   return (struct ColorRGBf) {
      +4.0767416621f * l - 3.3077115913f * m + 0.2309699292f * s,
      -1.2684380046f * l + 2.6097574011f * m - 0.3413193965f * s,
      -0.0041960863f * l - 0.7034186147f * m + 1.7076147010f * s,
   };
}
//------------------------------------------------------------------------------

// Remaining Conversions:

#define ColorFromLCHf(lch)        ColorFromLABf(ColorLCHfToLABf(lch))
#define ColorFromRGBf(rgb)        ColorFromLABf(ColorRGBfToLABf(rgb))
#define ColorLABfToRGBi(lab)      ColorRGBfToRGBi(ColorLABfToRGBf((lab)))
#define ColorLABiToRGBf(lab)      ColorLABfToRGBf(ColorLABiToLABf((lab)))
#define ColorRGBfToLABi(rgb)      ColorLABfToLABi(ColorRGBfToLABf((rgb)))
#define ColorRGBiToLABf(rgb)      ColorRGBfToLABf(ColorRGBiToRGBf((rgb)))
#define ColorRGBiToLABi(rgb)      \
   ColorLABfToLABi( ColorRGBfToLABf(ColorRGBiToRGBf((rgb))) )
#define ColorLABiToRGBi(lab)      \
   ColorRGBfToRGBi( ColorLABfToRGBf(ColorLABiToLABf((lab))) )
#define ColorFromRGBi(rgb)        \
   ColorFromLABf( ColorRGBfToLABf(ColorRGBiToRGBf((rgb))) )


//------------------------------------------------------------------------------
// Color Constants, CSS Names (generated by kule-generate-webcolors)
//
// Obviously not all colors can be very similar (check the Oklab distance data)
//------------------------------------------------------------------------------

//                                        Color  TrueColor    Distance
// Pink Colors:
#define COLOR_CSS_MEDIUM_VIOLET_RED        122   // #C71585   0.031101
#define COLOR_CSS_DEEP_PINK                157   // #FF1493   0.046187
#define COLOR_CSS_PALE_VIOLET_RED          166   // #DB7093   0.026896
#define COLOR_CSS_HOT_PINK                 185   // #FF69B4   0.048309
#define COLOR_CSS_LIGHT_PINK               225   // #FFB6C1   0.040771
#define COLOR_CSS_PINK                     232   // #FFC0CB   0.045439
// Red Colors:
#define COLOR_CSS_DARK_RED                  46   // #8B0000   0.019024
#define COLOR_CSS_RED                      145   // #FF0000   0.000000
#define COLOR_CSS_FIREBRICK                 92   // #B22222   0.041830
#define COLOR_CSS_CRIMSON                  123   // #DC143C   0.029486
#define COLOR_CSS_INDIAN_RED               142   // #CD5C5C   0.011194
#define COLOR_CSS_LIGHT_CORAL              174   // #F08080   0.047266
#define COLOR_CSS_SALMON                   174   // #FA8072   0.051820
#define COLOR_CSS_DARK_SALMON              196   // #E9967A   0.028607
#define COLOR_CSS_LIGHT_SALMON             213   // #FFA07A   0.054525
// Orange Colors:
#define COLOR_CSS_ORANGE_RED               162   // #FF4500   0.023223
#define COLOR_CSS_TOMATO                   174   // #FF6347   0.043271
#define COLOR_CSS_DARK_ORANGE              211   // #FF8C00   0.028076
#define COLOR_CSS_CORAL                    189   // #FF7F50   0.036897
#define COLOR_CSS_ORANGE                   211   // #FFA500   0.029970
// Yellow Colors:
#define COLOR_CSS_DARK_KHAKI               212   // #BDB76B   0.023105
#define COLOR_CSS_GOLD                     241   // #FFD700   0.030804
#define COLOR_CSS_KHAKI                    249   // #F0E68C   0.011517
#define COLOR_CSS_PEACH_PUFF               245   // #FFDAB9   0.021092
#define COLOR_CSS_YELLOW                   252   // #FFFF00   0.039003
#define COLOR_CSS_PALE_GOLDENROD           249   // #EEE8AA   0.044387
#define COLOR_CSS_MOCCASIN                 245   // #FFE4B5   0.036979
#define COLOR_CSS_PAPAYA_WHIP              254   // #FFEFD5   0.023498
#define COLOR_CSS_LIGHT_GOLDENROD_YELLOW   254   // #FAFAD2   0.017943
#define COLOR_CSS_LEMON_CHIFFON            254   // #FFFACD   0.018562
#define COLOR_CSS_LIGHT_YELLOW             254   // #FFFFE0   0.036768
// Brown Colors:
#define COLOR_CSS_MAROON                    46   // #800000   0.008508
#define COLOR_CSS_BROWN                     67   // #A52A2A   0.039384
#define COLOR_CSS_SADDLE_BROWN              83   // #8B4513   0.026047
#define COLOR_CSS_SIENNA                   115   // #A0522D   0.038315
#define COLOR_CSS_CHOCOLATE                152   // #D2691E   0.017397
#define COLOR_CSS_DARK_GOLDENROD           141   // #B8860B   0.041849
#define COLOR_CSS_PERU                     175   // #CD853F   0.028278
#define COLOR_CSS_ROSY_BROWN               187   // #BC8F8F   0.024185
#define COLOR_CSS_GOLDENROD                198   // #DAA520   0.014860
#define COLOR_CSS_SANDY_BROWN              216   // #F4A460   0.041483
#define COLOR_CSS_TAN                      216   // #D2B48C   0.035627
#define COLOR_CSS_BURLYWOOD                216   // #DEB887   0.027480
#define COLOR_CSS_WHEAT                    245   // #F5DEB3   0.022647
#define COLOR_CSS_NAVAJO_WHITE             245   // #FFDEAD   0.022816
#define COLOR_CSS_BISQUE                   254   // #FFE4C4   0.036927
#define COLOR_CSS_BLANCHED_ALMOND          254   // #FFEBCD   0.024028
#define COLOR_CSS_CORNSILK                 254   // #FFF8DC   0.026056
// Purple, Violet, and Magenta Colors:
#define COLOR_CSS_INDIGO                    36   // #4B0082   0.005555
#define COLOR_CSS_PURPLE                    62   // #800080   0.021735
#define COLOR_CSS_DARK_MAGENTA              62   // #8B008B   0.029036
#define COLOR_CSS_DARK_VIOLET              100   // #9400D3   0.022104
#define COLOR_CSS_DARK_SLATE_BLUE           45   // #483D8B   0.035769
#define COLOR_CSS_BLUE_VIOLET               95   // #8A2BE2   0.043132
#define COLOR_CSS_DARK_ORCHID              100   // #9932CC   0.036575
#define COLOR_CSS_FUCHSIA                  173   // #FF00FF   0.052400
#define COLOR_CSS_MAGENTA                  173   // #FF00FF   0.052400
#define COLOR_CSS_SLATE_BLUE               112   // #6A5ACD   0.050731
#define COLOR_CSS_MEDIUM_SLATE_BLUE        144   // #7B68EE   0.026926
#define COLOR_CSS_MEDIUM_ORCHID            149   // #BA55D3   0.022310
#define COLOR_CSS_MEDIUM_PURPLE            158   // #9370DB   0.026597
#define COLOR_CSS_ORCHID                   185   // #DA70D6   0.039764
#define COLOR_CSS_REBECCA_PURPLE            76   // #663399   0.041728
#define COLOR_CSS_VIOLET                   205   // #EE82EE   0.028590
#define COLOR_CSS_PLUM                     208   // #DDA0DD   0.034019
#define COLOR_CSS_THISTLE                  236   // #D8BFD8   0.029350
#define COLOR_CSS_LAVENDER                 248   // #E6E6FA   0.035323
// Blue Colors:
#define COLOR_CSS_MIDNIGHT_BLUE             23   // #191970   0.041119
#define COLOR_CSS_NAVY                      21   // #000080   0.008072
#define COLOR_CSS_DARK_BLUE                 21   // #00008B   0.026688
#define COLOR_CSS_MEDIUM_BLUE               48   // #0000CD   0.010263
#define COLOR_CSS_BLUE                      68   // #0000FF   0.004686
#define COLOR_CSS_ROYAL_BLUE               113   // #4169E1   0.008459
#define COLOR_CSS_STEEL_BLUE               119   // #4682B4   0.029305
#define COLOR_CSS_DODGER_BLUE              164   // #1E90FF   0.029979
#define COLOR_CSS_DEEP_SKY_BLUE            209   // #00BFFF   0.028549
#define COLOR_CSS_CORNFLOWER_BLUE          155   // #6495ED   0.039766
#define COLOR_CSS_SKY_BLUE                 230   // #87CEEB   0.027537
#define COLOR_CSS_LIGHT_SKY_BLUE           230   // #87CEFA   0.015358
#define COLOR_CSS_LIGHT_STEEL_BLUE         217   // #B0C4DE   0.039377
#define COLOR_CSS_LIGHT_BLUE               235   // #ADD8E6   0.023615
#define COLOR_CSS_POWDER_BLUE              235   // #B0E0E6   0.027408
// Cyan Colors:
#define COLOR_CSS_TEAL                     128   // #008080   0.047305
#define COLOR_CSS_DARK_CYAN                128   // #008B8B   0.016379
#define COLOR_CSS_LIGHT_SEA_GREEN          178   // #20B2AA   0.032839
#define COLOR_CSS_CADET_BLUE               163   // #5F9EA0   0.045248
#define COLOR_CSS_DARK_TURQUOISE           221   // #00CED1   0.041782
#define COLOR_CSS_MEDIUM_TURQUOISE         221   // #48D1CC   0.025977
#define COLOR_CSS_TURQUOISE                238   // #40E0D0   0.039339
#define COLOR_CSS_AQUA                     238   // #00FFFF   0.051105
#define COLOR_CSS_CYAN                     238   // #00FFFF   0.051105
#define COLOR_CSS_AQUAMARINE               247   // #7FFFD4   0.017898
#define COLOR_CSS_PALE_TURQUOISE           251   // #AFEEEE   0.037925
#define COLOR_CSS_LIGHT_CYAN               255   // #E0FFFF   0.038938
// Green Colors:
#define COLOR_CSS_DARK_GREEN                57   // #006400   0.024743
#define COLOR_CSS_GREEN                     86   // #008000   0.025449
#define COLOR_CSS_DARK_OLIVE_GREEN          78   // #556B2F   0.032799
#define COLOR_CSS_FOREST_GREEN             116   // #228B22   0.016293
#define COLOR_CSS_SEA_GREEN                127   // #2E8B57   0.026792
#define COLOR_CSS_OLIVE                    131   // #808000   0.042445
#define COLOR_CSS_OLIVE_DRAB               135   // #6B8E23   0.017922
#define COLOR_CSS_MEDIUM_SEA_GREEN         160   // #3CB371   0.035175
#define COLOR_CSS_LIME_GREEN               206   // #32CD32   0.033066
#define COLOR_CSS_LIME                     229   // #00FF00   0.035538
#define COLOR_CSS_SPRING_GREEN             233   // #00FF7F   0.031025
#define COLOR_CSS_MEDIUM_SPRING_GREEN      233   // #00FA9A   0.030131
#define COLOR_CSS_DARK_SEA_GREEN           197   // #8FBC8F   0.033246
#define COLOR_CSS_MEDIUM_AQUAMARINE        204   // #66CDAA   0.041414
#define COLOR_CSS_YELLOW_GREEN             218   // #9ACD32   0.030111
#define COLOR_CSS_LAWN_GREEN               243   // #7CFC00   0.025192
#define COLOR_CSS_CHARTREUSE               243   // #7FFF00   0.028636
#define COLOR_CSS_LIGHT_GREEN              239   // #90EE90   0.014600
#define COLOR_CSS_GREEN_YELLOW             250   // #ADFF2F   0.029540
#define COLOR_CSS_PALE_GREEN               239   // #98FB98   0.045475
// White Colors:
#define COLOR_CSS_MISTY_ROSE               248   // #FFE4E1   0.041993
#define COLOR_CSS_ANTIQUE_WHITE            254   // #FAEBD7   0.033047
#define COLOR_CSS_LINEN                    254   // #FAF0E6   0.042523
#define COLOR_CSS_BEIGE                    254   // #F5F5DC   0.024648
#define COLOR_CSS_WHITE_SMOKE              255   // #F5F5F5   0.029849
#define COLOR_CSS_LAVENDER_BLUSH           255   // #FFF0F5   0.036134
#define COLOR_CSS_OLD_LACE                 255   // #FDF5E6   0.035068
#define COLOR_CSS_ALICE_BLUE               255   // #F0F8FF   0.027896
#define COLOR_CSS_SEASHELL                 255   // #FFF5EE   0.027898
#define COLOR_CSS_GHOST_WHITE              255   // #F8F8FF   0.021033
#define COLOR_CSS_HONEYDEW                 255   // #F0FFF0   0.029416
#define COLOR_CSS_FLORAL_WHITE             255   // #FFFAF0   0.019792
#define COLOR_CSS_AZURE                    255   // #F0FFFF   0.019215
#define COLOR_CSS_MINT_CREAM               255   // #F5FFFA   0.015198
#define COLOR_CSS_SNOW                     255   // #FFFAFA   0.012283
#define COLOR_CSS_IVORY                    255   // #FFFFF0   0.020024
#define COLOR_CSS_WHITE                    255   // #FFFFFF   0.000000
// Gray and Black Colors:
#define COLOR_CSS_BLACK                      0   // #000000   0.000000
#define COLOR_CSS_DARK_SLATE_GRAY           47   // #2F4F4F   0.045950
#define COLOR_CSS_DIM_GRAY                  87   // #696969   0.021111
#define COLOR_CSS_SLATE_GRAY               134   // #708090   0.018629
#define COLOR_CSS_GRAY                     134   // #808080   0.043172
#define COLOR_CSS_LIGHT_SLATE_GRAY         134   // #778899   0.019895
#define COLOR_CSS_DARK_GRAY                192   // #A9A9A9   0.028056
#define COLOR_CSS_SILVER                   217   // #C0C0C0   0.020475
#define COLOR_CSS_LIGHT_GRAY               235   // #D3D3D3   0.039485
#define COLOR_CSS_GAINSBORO                248   // #DCDCDC   0.013372

#endif //include_guard
