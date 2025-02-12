#ifndef  TOOLS_KULE_KONPU_H
#define  TOOLS_KULE_KONPU_H
#include <stdint.h>

// Konpu Palette hard-coded as a `uint32_t` array of (gamma) sRGB(888) colors.
// To use with "kule.h" as an OkLab palette, #include "kule.h" before including
// "kule-konpu.h"; then use `PaletteKonpu()` to get the palette.
static uint32_t PaletteKonpuSRGB[256] = {

// Values of `pal/konpu.pal` dumped as uint32_t here:

0x000000,
0x000007,
0x080000,
0x000024,
0x000a00,
0x110115,
0x1b0501,
0x00111c,
0x130137,
0x29021a,
0x001b04,
0x010156,
0x211816,
0x021a3d,
0x2e013f,
0x3b0501,
0x012c01,
0x240264,
0x43032b,
0x012a2d,
0x2b1f39,
0x03037c,
0x312503,
0x012561,
0x560702,
0x40085c,
0x442829,
0x580448,
0x2d028c,
0x1a3a25,
0x00384e,
0x332c5d,
0x0501a5,
0x6b032a,
0x463602,
0x052e86,
0x4c0583,
0x004806,
0x622810,
0x532e4d,
0x64016d,
0x384043,
0x3501b6,
0x0e4373,
0x780052,
0x3f3186,
0x820802,
0x06503e,
0x0801d2,
0x434c0a,
0x5704ac,
0x0337b1,
0x72303d,
0x5f3274,
0x5b4530,
0x484669,
0x700c95,
0x045d0d,
0x8f0a3b,
0x035868,
0x7c3804,
0x1e4d98,
0x87097a,
0x4000e0,
0x4739af,
0x7e3368,
0x415b46,
0xa10904,
0x0002fc,
0x6a5301,
0x584992,
0x6d4b5f,
0x6213d3,
0x0140dd,
0x9f0c63,
0x8e3c3e,
0x793894,
0x7e01bf,
0x496615,
0x415d82,
0x076d6a,
0x950fa4,
0x095ac0,
0x8b501f,
0xb01743,
0x6b41bc,
0x037808,
0x636363,
0x5a16ff,
0x4d45dd,
0x7a5388,
0x9b416c,
0xad3907,
0xae138b,
0x046da6,
0x7f01f1,
0x545fb0,
0x6c6b3c,
0x8b595d,
0x047d54,
0x9e04d0,
0x8e44b5,
0xcb0519,
0xc4006a,
0x0555fe,
0x437589,
0x5a7b1d,
0xb44951,
0x936a02,
0xb714b7,
0x776c8a,
0xa64997,
0x754de7,
0x3e66e1,
0x8062b0,
0xad5d31,
0x018f1a,
0x56816a,
0x867361,
0x5278b4,
0xa16082,
0xa113fd,
0xd00996,
0xdf0151,
0xcf4511,
0xc24a7d,
0x9c4ae1,
0x029462,
0x088e95,
0x0a80db,
0xbd12e6,
0x7f8344,
0x6d71d8,
0xa55dc0,
0x698698,
0x699102,
0xc64bb0,
0xd912c3,
0xaf716a,
0xeb0f80,
0x8a7bae,
0xb47606,
0xd1585e,
0x1292c2,
0x8369fe,
0xff0000,
0x04a60a,
0x579981,
0xb36ea7,
0xc04fde,
0x4f80ff,
0x8d8c78,
0xd36a37,
0x9b9011,
0xf24067,
0x698cd4,
0xdf5497,
0xf709ad,
0x987adb,
0xdf19f3,
0x02ab65,
0xa88597,
0xf65305,
0x15a5b1,
0x0698f3,
0xad67fe,
0xce738b,
0xb9885e,
0x77a35f,
0x72a90d,
0xe24fd1,
0xc26fd5,
0x719cba,
0xfe12dc,
0xec6c65,
0xd68410,
0x9b91cc,
0x808eff,
0x3fb294,
0x01be20,
0x86a696,
0x19ade0,
0xd564fc,
0xc586bb,
0xaca256,
0xe56fbf,
0xfb6691,
0xca918d,
0xb087fe,
0xfd7717,
0x05c37a,
0xa4b004,
0xa7a3b5,
0x76a7f2,
0x57b8c1,
0xfb53fe,
0xe59262,
0x7ebc7d,
0xd6a00d,
0xea88a8,
0xd489e7,
0xb9aa8d,
0x79c50a,
0xada1f5,
0x0bcbb3,
0xf875eb,
0x05d60a,
0x8bb8dd,
0xcca2d0,
0x0bc6f4,
0x8ec2ae,
0xff991a,
0xb5bd64,
0xe3a699,
0x1ada75,
0xf791d7,
0xdfb16f,
0xbabac3,
0xb3c912,
0x8ad278,
0xdfbb16,
0x5dd4da,
0xdba6fe,
0xb2bcfe,
0x7bdf37,
0xf3aac8,
0xc1c89e,
0x08e4b0,
0x8bdcb6,
0x05f203,
0x8dd3fd,
0xc4d66e,
0xe7c2ba,
0x08f480,
0xf3c86c,
0xb4d7d5,
0xdbc6ec,
0xbbe301,
0x17eee4,
0x82ed97,
0xfeb5fd,
0xead70f,
0xc5e3ac,
0x86f940,
0x85eeff,
0xfcd5a9,
0xc0f17b,
0x83f8d0,
0xe3dfe0,
0xf1e681,
0xc9fd13,
0xc1f6e1,
0xf8f236,
0xd3feb2,
0xfbf3c8,
0xffffff,


// That's it!
};
#include "kule.h"
#ifdef TOOLS_KULE_H
Palette PaletteKonpu(void) {
   static Lab array[UTIL_ARRAY_SIZE(PaletteKonpuSRGB)] = {0};
   static Palette konpu_pal = { .color = array };
   if (konpu_pal.size == 0) {
      konpu_pal.size = UTIL_ARRAY_SIZE(PaletteKonpuSRGB);
      for (int i = 0; i < konpu_pal.size; i++)
         konpu_pal.color[i] = LabFromSRGB(PaletteKonpuSRGB[i]);
   }
   return konpu_pal;
}
#endif // TOOLS_KULE_H
#endif // TOOLS_KULE_KONPU_H
