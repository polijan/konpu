// Sitelen Pona Ideograms in Glyph32 format (ie width=3, ie Half-width)
//
// This format is just too small (and thus a bad idea) for most glyphs,
// which won't be recognizable. But for a slim glyphs, like several Toki Pona
// particle, or numbers, or pronouns, it might have some interest.
//
// It might also allow a "| joiner" to easily combine two half-widths into a
// single glyphs. But even then, in practice such combined glyph which might
// be more elaboarate than simply putting two half halfwidth:
// kon|wan: GLYPH64(004A54544A45654A) / kon is width 5 and fits well with
// a slim wan to make a width 7 combo glyph because of "kerning".

#ifndef KONPU_GLYPH_TOKIPONA_HALFWIDTH_H_
#define KONPU_GLYPH_TOKIPONA_HALFWIDTH_H_
#include "../glyph.h"

// nimi pu 120
#define GLYPH32_TOKIPONA7_A                       GLYPH32(06560222)
#define GLYPH32_TOKIPONA7_AKESI                   GLYPH32(02575205)
#define GLYPH32_TOKIPONA7_ALA                     GLYPH32(00052500)  // <- 3x3 | or use: 00552550  3x5
#define GLYPH32_TOKIPONA7_ALASA                   GLYPH32(03557553)
#define GLYPH32_TOKIPONA7_ALE                     GLYPH32(00252520)  // turned 90 degrees.
#define GLYPH32_TOKIPONA7_ANPA                    GLYPH32(02075500)  // 3x5
#define GLYPH32_TOKIPONA7_ANTE                    GLYPH32(00520250)  // 3x5
#define GLYPH32_TOKIPONA7_ANU                     GLYPH32(02222555)
#define GLYPH32_TOKIPONA7_AWEN                    GLYPH32(05555222)
#define GLYPH32_TOKIPONA7_AWEN_                   GLYPH32(C5555522)  // awen_
#define GLYPH32_TOKIPONA7_E                       GLYPH32(01210242)
#define GLYPH32_TOKIPONA7_EN                      GLYPH32(00027200)  // 3x3
#define GLYPH32_TOKIPONA7_ESUN                    GLYPH32(05322265)
#define GLYPH32_TOKIPONA7_IJO                     GLYPH32(00255520)  // 3x5
#define GLYPH32_TOKIPONA7_IKE                     GLYPH32(00055600)
#define GLYPH32_TOKIPONA7_ILO                     GLYPH32(02275757)
#define GLYPH32_TOKIPONA7_INSA                    GLYPH32(00000000)
#define GLYPH32_TOKIPONA7_JAKI                    GLYPH32(06B657E5) // width=4
#define GLYPH32_TOKIPONA7_JAN                     GLYPH32(00525520)
#define GLYPH32_TOKIPONA7_JELO                    GLYPH32(00000000)
#define GLYPH32_TOKIPONA7_JO                      GLYPH32(02551252)
#define GLYPH32_TOKIPONA7_KALA                    GLYPH32(00016100)
#define GLYPH32_TOKIPONA7_KALAMA                  GLYPH32(02570142)
#define GLYPH32_TOKIPONA7_KAMA                    GLYPH32(03215531)  // (rough! but can't find better)
#define GLYPH32_TOKIPONA7_KASI                    GLYPH32(02222340)
#define GLYPH32_TOKIPONA7_KEN                     GLYPH32(05553555)
#define GLYPH32_TOKIPONA7_KEPEKEN                 GLYPH32(05562757)
#define GLYPH32_TOKIPONA7_KEPEKEN_                GLYPH32(C5562757)  // kepeken_
#define GLYPH32_TOKIPONA7_KILI                    GLYPH32(02775240)
#define GLYPH32_TOKIPONA7_KIWEN                   GLYPH32(00255770)
#define GLYPH32_TOKIPONA7_KO                      GLYPH32(03763376)
#define GLYPH32_TOKIPONA7_KON                     GLYPH32(03260326)
#define GLYPH32_TOKIPONA7_KON1                    GLYPH32(02449224) // 1width=4
        // other options for kon: single "s" --> 02442112 (or more pronounced: 03442116)
#define GLYPH32_TOKIPONA7_KULE                    GLYPH32(00000000)
#define GLYPH32_TOKIPONA7_KULUPU                  GLYPH32(00050200)  // (3x3, I don't see how to di differently)
#define GLYPH32_TOKIPONA7_KUTE                    GLYPH32(01225543)
#define GLYPH32_TOKIPONA7_LA                      GLYPH32(01244421)
#define GLYPH32_TOKIPONA7_LAPE                    GLYPH32(0004B400)  // (note: width is 4)
#define GLYPH32_TOKIPONA7_LASO                    GLYPH32(00000000)
#define GLYPH32_TOKIPONA7_LAWA                    GLYPH32(02555F52)  // (note: width is 4)
#define GLYPH32_TOKIPONA7_LEN                     GLYPH32(05755557)
#define GLYPH32_TOKIPONA7_LETE                    GLYPH32(00527250)  // 3x5
#define GLYPH32_TOKIPONA7_LI                      GLYPH32(00124210)  // 3x5
#define GLYPH32_TOKIPONA7_LILI                    GLYPH32(00025000)  // 3x2
#define GLYPH32_TOKIPONA7_LINJA                   GLYPH32(00017400)
#define GLYPH32_TOKIPONA7_LIPU                    GLYPH32(07555557)
#define GLYPH32_TOKIPONA7_LOJE                    GLYPH32(00000000)
#define GLYPH32_TOKIPONA7_LON                     GLYPH32(00070200)  // 3x2
#define GLYPH32_TOKIPONA7_LUKA                    GLYPH32(06545552)  // (maybe better?: 04654552)
#define GLYPH32_TOKIPONA7_LUKIN                   GLYPH32(00257520)  // 3x5
#define GLYPH32_TOKIPONA7_LUPA                    GLYPH32(02555555)
#define GLYPH32_TOKIPONA7_MA                      GLYPH32(07027207)  // meh
#define GLYPH32_TOKIPONA7_MAMA                    GLYPH32(02525552)
#define GLYPH32_TOKIPONA7_MANI                    GLYPH32(00000000)
#define GLYPH32_TOKIPONA7_MELI                    GLYPH32(02725552)  // meli venus
#define GLYPH32_TOKIPONA7_MI                      GLYPH32(02113552)
#define GLYPH32_TOKIPONA7_MIJE                    GLYPH32(02553246)  // mije mars
#define GLYPH32_TOKIPONA7_MOKU                    GLYPH32(05560257)
#define GLYPH32_TOKIPONA7_MOLI                    GLYPH32(05250525)
#define GLYPH32_TOKIPONA7_MONSI                   GLYPH32(06223226)
#define GLYPH32_TOKIPONA7_MU                      GLYPH32(02103012)  // <-- alt emitter on the side
#define GLYPH32_TOKIPONA7_MUN                     GLYPH32(00366630)  // 3x5
#define GLYPH32_TOKIPONA7_MUSI                    GLYPH32(00250550)
#define GLYPH32_TOKIPONA7_MUTE                    GLYPH32(00102040)  // rotated: GLYPH32(00707070) // rotated
#define GLYPH32_TOKIPONA7_NANPA                   GLYPH32(05755575)
#define GLYPH32_TOKIPONA7_NANPA_                  GLYPH32(C5755575)  // nanpa_
#define GLYPH32_TOKIPONA7_NASA                    GLYPH32(00000000)
#define GLYPH32_TOKIPONA7_NASIN                   GLYPH32(00000000)
#define GLYPH32_TOKIPONA7_NENA                    GLYPH32(05555552)
#define GLYPH32_TOKIPONA7_NI                      GLYPH32(02722222)
#define GLYPH32_TOKIPONA7_NIMI                    GLYPH32(00799E00)
#define GLYPH32_TOKIPONA7_NOKA                    GLYPH32(00000000)
#define GLYPH32_TOKIPONA7_O                       GLYPH32(02520222)
#define GLYPH32_TOKIPONA7_OLIN                    GLYPH32(02750275)
#define GLYPH32_TOKIPONA7_ONA                     GLYPH32(00000000)
#define GLYPH32_TOKIPONA7_OPEN                    GLYPH32(07575555)
#define GLYPH32_TOKIPONA7_PAKALA                  GLYPH32(07655537)
#define GLYPH32_TOKIPONA7_PALI                    GLYPH32(05556252)
#define GLYPH32_TOKIPONA7_PALISA                  GLYPH32(02555552)
#define GLYPH32_TOKIPONA7_PAN                     GLYPH32(02252525)
#define GLYPH32_TOKIPONA7_PANA                    GLYPH32(05560142)
#define GLYPH32_TOKIPONA7_PI                      GLYPH32(07111111)
#define GLYPH32_TOKIPONA7_PI_                     GLYPH32(E2222222)  // pi-
#define GLYPH32_TOKIPONA7_PILIN                   GLYPH32(00277500)
#define GLYPH32_TOKIPONA7_PIMEJA                  GLYPH32(00000000)
#define GLYPH32_TOKIPONA7_PINI                    GLYPH32(07222227)
#define GLYPH32_TOKIPONA7_PIPI                    GLYPH32(02727205)
#define GLYPH32_TOKIPONA7_POKA                    GLYPH32(00337330)
#define GLYPH32_TOKIPONA7_POKI                    GLYPH32(07555555)
#define GLYPH32_TOKIPONA7_PONA                    GLYPH32(00035500)
#define GLYPH32_TOKIPONA7_PU                      GLYPH32(07757777)
#define GLYPH32_TOKIPONA7_SAMA                    GLYPH32(00070700)  // (3x3)
#define GLYPH32_TOKIPONA7_SAMA_                   GLYPH32(08870F00)  // sama_
#define GLYPH32_TOKIPONA7_SELI                    GLYPH32(02022775)
#define GLYPH32_TOKIPONA7_SELO                    GLYPH32(05577777)
#define GLYPH32_TOKIPONA7_SEME                    GLYPH32(02024552)
#define GLYPH32_TOKIPONA7_SEWI                    GLYPH32(00557020)  // (3x5, logical)
#define GLYPH32_TOKIPONA7_SIJELO                  GLYPH32(05557777)
#define GLYPH32_TOKIPONA7_SIKE                    GLYPH32(02755572) //07025207
#define GLYPH32_TOKIPONA7_SIN                     GLYPH32(00072200)
#define GLYPH32_TOKIPONA7_SINA                    GLYPH32(02553112)
#define GLYPH32_TOKIPONA7_SINPIN                  GLYPH32(03226223)
#define GLYPH32_TOKIPONA7_SITELEN                 GLYPH32(07575757)
#define GLYPH32_TOKIPONA7_SONA                    GLYPH32(07570142)
#define GLYPH32_TOKIPONA7_SOWELI                  GLYPH32(01134543)  // (simplified)
#define GLYPH32_TOKIPONA7_SULI                    GLYPH32(00000000)
#define GLYPH32_TOKIPONA7_SUNO                    GLYPH32(00000000)
#define GLYPH32_TOKIPONA7_SUPA                    GLYPH32(00555700)  // (3x4)
#define GLYPH32_TOKIPONA7_SUWI                    GLYPH32(00200550)  // (3x5)
#define GLYPH32_TOKIPONA7_TAN                     GLYPH32(00000000)
#define GLYPH32_TOKIPONA7_TASO                    GLYPH32(04447444)
#define GLYPH32_TOKIPONA7_TAWA                    GLYPH32(06245564)
    //  ^ TODO
    //   \_ rough! --> is it better or not?  --> 02125564
    //                                        or 06245766
    //                                        or 06245764
    // #define GLYPH32_TOKIPONA7_TAWA7_           GLYPH32(0C555664)
    // #define GLYPH32_TOKIPONA7_TAWA8_           GLYPH32(C4555664)
    // Do other long glyphs too!

#define GLYPH32_TOKIPONA7_TELO                    GLYPH32(01741740)
#define GLYPH32_TOKIPONA7_TENPO                   GLYPH32(00000000)
#define GLYPH32_TOKIPONA7_TOKI                    GLYPH32(02520142)
#define GLYPH32_TOKIPONA7_TOMO                    GLYPH32(00755520)  // (3x5)
#define GLYPH32_TOKIPONA7_TU                      GLYPH32(05555555)
#define GLYPH32_TOKIPONA7_UNPA                    GLYPH32(00000000)
#define GLYPH32_TOKIPONA7_UTA                     GLYPH32(02025700)
#define GLYPH32_TOKIPONA7_UTALA                   GLYPH32(05772555) // 05525000
#define GLYPH32_TOKIPONA7_WALO                    GLYPH32(00000000)
#define GLYPH32_TOKIPONA7_WAN                     GLYPH32(04444564)
#define GLYPH32_TOKIPONA7_WASO                    GLYPH32(01174521)
#define GLYPH32_TOKIPONA7_WAWA                    GLYPH32(00000000)
#define GLYPH32_TOKIPONA7_WEKA                    GLYPH32(01540154) // or symmetrically: 04510451
#define GLYPH32_TOKIPONA7_WILE                    GLYPH32(07775550) // hmm, bof!

// synonyms
#define GLYPH32_TOKIPONA7_ALI                     GLYPH32_TOKIPONA7_ALE
#define GLYPH32_TOKIPONA7_KIN                     GLYPH32(05250222)
#define GLYPH32_TOKIPONA7_NAMAKO                  GLYPH32_TOKIPONA7_NAMAKO_PEPPER
#define GLYPH32_TOKIPONA7_OKO                     GLYPH32(00675760)

// other common names
#define GLYPH32_TOKIPONA7_TONSI                   GLYPH32(02255625) // better: 02235625 ?
#define GLYPH32_TOKIPONA7_KIJETESANTAKALU         GLYPH32(05577F52) // width=4 | weird: GLYPH32(0D673D52)
#define GLYPH32_TOKIPONA7_MONSUTA                 GLYPH32(05557775)
#define GLYPH32_TOKIPONA7_N                       GLYPH32(05530222)
#define GLYPH32_TOKIPONA7_KIPISI                  GLYPH32(05122245)
#define GLYPH32_TOKIPONA7_LEKO                    GLYPH32(07755577)
#define GLYPH32_TOKIPONA7_SOKO                    GLYPH32(02227552)

// other semi-common names
#define GLYPH32_TOKIPONA7_MISIKEKE                GLYPH32(02557552)
#define GLYPH32_TOKIPONA7_LANPAN                  GLYPH32(05421352)
#define GLYPH32_TOKIPONA7_EPIKU                   GLYPH32(03550142)
#define GLYPH32_TOKIPONA7_MESO                    GLYPH32(05202025)
#define GLYPH32_TOKIPONA7_JASIMA                  GLYPH32(02362362)
#define GLYPH32_TOKIPONA7_KOKOSILA                GLYPH32(03760142)
#define GLYPH32_TOKIPONA7_MAJUNA                  GLYPH32(00227000)
#define GLYPH32_TOKIPONA7_LINLUWI                 GLYPH32(00000000) //05757557 ?

// nimi lipu
#define GLYPH32_TOKIPONA7_KU                      GLYPH32(07573567)
#define GLYPH32_TOKIPONA7_SU                      GLYPH64_TOKIPONA7_SU1
                                                 // ^---- or: 05555567  su  or  07774517
#define GLYPH32_TOKIPONA7_SU1                     GLYPH32(07733567)
#define GLYPH32_TOKIPONA7_SU2                     GLYPH32(07777537)

// other common names
#define GLYPH32_TOKIPONA7_KIJETESANTAKALU_        GLYPH32(0E5E0000)  kijetesantakalu_  (start of tail)
#define GLYPH32_TOKIPONA7__KIJETESANTAKALU_       GLYPH32(0F5F0000)  _kijetesantakalu_ (tail continuation)
#define GLYPH32_TOKIPONA7__KIJETESANTAKALU        GLYPH32(06337536)  _kijetesantakalu  (body to be added after kijetesantakalu_ or _kijetesantakalu_)

// Uncommon names... but in UCSUR
#define GLYPH32_TOKIPONA7_APEJA                   GLYPH32(05052020)
#define GLYPH32_TOKIPONA7_PAKE                    GLYPH32(02222227)
#define GLYPH32_TOKIPONA7_POWE                    GLYPH32(00705250)


//------------------------------------------------------------------------------
// Variations for Common Glyphs
//------------------------------------------------------------------------------
#define GLYPH32_TOKIPONA7_NAMAKO_SIN_EXTENDED     GLYPH32(02205022)
#define GLYPH32_TOKIPONA7_NAMAKO_PEPPER           GLYPH32(03660142)


/*
     02103012  mu2 / side-emitters
*/

/*
#######################
# pre-composed number #
#######################
#              0=ala
#              1=wan
#              2=tu
     05550223  3=tu-wan
     05550555  4=tu-tu
#              5=luka -->
     05560446  6=luka-wan
     05560555  7=luka-tu
     05560502  8=luka-tu-wan (also luka-kulupu)
     05560505  9=luka-tu-tu
     05560556  10=luka-luka
     05605602  11=luka-luka-wan
     05605605  12=luka-luka-tu


#
254  00020200  two-dots
255  00002000  middot
000  CEEEEEEC  cartouche_dark_open
000  37777773  cartouche_dark_close
000  C222222C  cartouche_light_open
000  34444443  cartouche_light_close
000  00001117  te
000  07444000  to
# long pi, prepositions, preverbs (official + potential)
000  00000000  pi_
000  00000000  kepeken_
000  00000000  lon_
000  00000000  sama_
000  00000000  tan_
000  00000000  tawa_
000  00000000  alasa_
000  00000000  ken_
000  00000000  lukin_
000  00000000  sona_
000  00000000  oko_
000  00000000  open_
000  00000000  pini_

000  00000000  _la
000  00000000  ala_/_ala_
000  00000000  _anu_
000  00000000  _kin
000  00000000  _taso
000  00000000  _a
000  00000000  a_
000  00000000  _a_
000  00000000  _n
000  00000000  n_
000  00000000  _n_



000  00000000               nimi-sin
000  00000000               emitter
000  00000000               sewi logical
000  00000000               soko



# UCSUR

. . @
. @ @           . @ @
@ . @           @ . @
@ . @           @ . @
. . @           . . @
. . @           . @ .
. @ .           @ . .
. . @           . @ @




luka       luka
. X .      . X .
. X X      X . X
. X X      X . X
. X X      . . X
X X X      X . X
. X X      . X X
. . X      . . X
. . .      . . .

*/

#endif
