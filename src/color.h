#ifndef  KONPU_COLOR_H
#define  KONPU_COLOR_H
#include "memory.h"
#include "video_mode.h"

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
// Palettes
//
// The overall palette can contains 128 entries (which are index to the 256
// master colors). It allows distinct for 2,4,8,8,16,32,64 colors subpalettes
// in distinct areas.
//------------------------------------------------------------------------------
#define COLOR_PALETTE128  (KonpuMemory + COLOR_PALETTE_ADDRESS)
#define COLOR_PALETTE2     COLOR_PALETTE128
#define COLOR_PALETTE4    (COLOR_PALETTE2  + 2)
#define COLOR_PALETTE8    (COLOR_PALETTE4  + 4)
#define COLOR_PALETTE16   (COLOR_PALETTE8  + 8)
#define COLOR_PALETTE32   (COLOR_PALETTE16 + 16)
#define COLOR_PALETTE64   (COLOR_PALETTE32 + 32)

#define COLOR_BORDER       KonpuMemory[COLOR_BORDER_ADDRESS]
#define COLOR_DEFAULT_FG   KonpuMemory[COLOR_DEFAULT_FG_ADDRESS]
#define COLOR_DEFAULT_BG   KonpuMemory[COLOR_DEFAULT_BG_ADDRESS]


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
static inline uint8_t *ColorPalette()
{
   int offset = VideoColorDepth();
   switch(offset) {
      case 8:  return NULL;        // 256 colors
#if (VIDEO_SIZE_FACTOR_ % 7 != 0)
      case 7:  offset  = 0; break; // 128 color palette
#endif
      default: offset -= 2; break; // 2,4,8,16,32,64 color palettes
   }
   return KonpuMemory + COLOR_PALETTE_ADDRESS + offset;
}


//------------------------------------------------------------------------------

// A struct to represent an OkLab color
struct ColorLABf  { float   L, a, b; };

// A struct to represent an OkLab color where components are integers and
// scaled by a factor of 510 compared to ColorLABf.
struct ColorLABi  { int     L, a, b; };

// A struct to represent a (Lab, Chroma, Hue) Polar OkLab color
struct ColorLCHf  { float L, C, h; };

#define COLOR_ROM(i)  (KonpuROM[KONPU_ROM_COLOR + (i)])


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
static inline int ColorChromaSquared(int color)
{
   int i = color << 3;
   int a = (int8_t)(COLOR_ROM(1 + i)) + 12;
   int b = (int8_t)(COLOR_ROM(2 + i)) - 30;
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


#define ColorDistance(color)     sqrtf(ColorDistanceSquared((color1), (color2)))
#define ColorSaturation(color)   sqrtf(ColorSaturationSquared((color)))
#define ColorChroma(color)       sqrtf(ColorChromaSquared_((color)))

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

// Return a pointer from which gamma-encoded sRGB 8-bits components
// corresponding to the given color can be read
static inline const uint8_t* ColorToRGB(int color)
{ return KonpuROM + KONPU_ROM_COLOR + 5 + (color << 3); }


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

#endif //KONPU_COLOR_H
