#ifndef KONPU_CHARACTER_H_
#define KONPU_CHARACTER_H_
#include "c.h"

typedef uint8_t Character;

// Return a Unicode pseudo-graphics "octant" (2x4) character
// correspondingg to a Glyph8 character
// TODO: add to glyph.h
// char32_t Glyph8ToChar32(Glyph8 octant);

// Return a Unicode character correspoding to the given Konpu character.
// The UCUSR Sitelen Pona block is used to return Sitelen Pona characters as
// those are not (yet?) part of Unicode.
// char32_t  CharacterToChar32(Character c);

// if the Unicode is not representable in Konpu, returns -1
// int CharacterFromChar32(char32_t);



//------------------------------------------------------------------------------
// <ctype.h> - character type features
//
// We don't have have an explicit "locale" in Konpu C, but we can make our own
// implementation of the function in <ctype.h>, based on Konpu charset.
//------------------------------------------------------------------------------

#undef  isalnum
// Return non-zero iff the given character is alphanumeric
#define isalnum(c)    c_isalnum_(c)
   static inline int  c_isalnum_(int c)
   { return (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') ; }

#undef  isalpha
// Return non-zero iff the given character is alphabetic
#define isalpha(c)    c_isalpha_(c)
   static inline int  c_isalpha_(int c)
   { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }

#undef  islower
// Return non-zero iff the given character is a lowercase character
#define islower(c)    c_islower_(c)
   static inline int  c_islower_(int c)
   { return (c >= 'a' && c <= 'z'); }

#undef  isupper
// Return non-zero iff the given character is an uppercase character
#define isupper(c)    c_isupper_(c)
   static inline int  c_isupper_(int c)
   { return (c >= 'A' && c <= 'Z'); }

#undef  isdigit
// Return non-zero iff the given character is a digit (0-9)
#define isdigit(c)    c_isdigit_(c)
   static inline int  c_isdigit_(int c)
   { return (c >= '0' && c <= '9'); }

#undef  isxdigit
// Return non-zero iff the given character is an hexadecimal character
#define isxdigit(c)   c_isxdigit_(c)
   static inline int  c_isxdigit_(int c)
   { return (c >= '0' && c <= '9') ||
            (c >= 'a' && c <= 'f') ||
            (c >= 'A' && c <= 'F') ; }

#undef  iscntrl
// Return non-zero iff the given character is a control character
#define iscntrl(c)    c_iscntrl_(c)
   static inline int  c_iscntrl_(int c)
   { return ((unsigned)c < 32) || (c == 127); /* TODO */ }

#undef  isgraph
// Return non-zero iff the given character has a graphical representation
#define isgraph(c)    c_isgraph_(c)
   static inline int  c_isgraph_(int c)
   { return (c > 32) && (c != 127); /* TODO */ }

#undef  isspace
// Return non-zero iff the given character is a space character
#define isspace(c)    c_isspace_(c)
   static inline int  c_isspace_(int c)
   { return (c == ' ') || (c == '\n'); /* TODO: \f \r \t \v ... */ }

#undef  isblank
// Return non-zero iff the given character is a blank character
// (note: this does NOT look at a glyph)
#define isblank(c)    c_isblank_(c)
   static inline int  c_isblank_(int c)
   { return (c == ' '); /* TODO: \t */ }

#undef  isprint
// Return non-zero iff the given character is a printing character
#define isprint(c)    c_isprint_(c)
   static inline int  c_isprint_(int c)
   { return !c_iscntrl_(c); /* TODO: currently same as: (c >=32) && (c!=127) */}

#undef  ispunct
// Return non-zero iff the given character is a punctuation character
// TODO: for now same as ASCII punctuation: !"#$%&'()*+,-./:;<=>?@[\]^_`{|}~
#define ispunct(c)    c_ispunct_(c)
   static inline int  c_ispunct_(int c)
   { return (c >= '!' && c <= '/') ||
            (c >= ':' && c <= '@') ||
            (c >= '[' && c <= '`') ||
            (c >= '}' && c < 127); }

#undef  tolower
// Converts the given character to lowercase
#define tolower(c)    TODO_FIXME_c_tolower_(c)
   static inline int  c_tolower_(int c)
   { return (c >= 'A' && c <= 'Z')? c + 32 : c; }

#undef  toupper
// Converts the given character to Uppercase
#define toupper(c)    TODO_FIXME_c_toupper_(c)
   static inline int  c_toupper_(int c)
   { return (c >= 'a' && c <= 'z')? c - 32 : c; }


//------------------------------------------------------------------------------

// Character Names.
// Characters #32-#126 are the printable ASCII characters (' ' - '~')
// They do NOT receive a CHARACTER_* macro. Use C literals instead.
// So CHARACTER_A refers to the character for the toki pona/sitelen pona word
// 'a', not to the ascii letter 'a' or 'A'.

// Characters #128-#247 ("classic 120" of lipu pu, 1st edition)

#define CHARACTER_A         128
#define CHARACTER_AKESI     129
#define CHARACTER_ALA       130
#define CHARACTER_ALASA     131
#define CHARACTER_ALE       132
#define CHARACTER_ANPA      133
#define CHARACTER_ANTE      134
#define CHARACTER_ANU       135
#define CHARACTER_AWEN      136
#define CHARACTER_E         137
#define CHARACTER_EN        138
#define CHARACTER_ESUN      139
#define CHARACTER_IJO       140
#define CHARACTER_IKE       141
#define CHARACTER_ILO       142
#define CHARACTER_INSA      143
#define CHARACTER_JAKI      144
#define CHARACTER_JAN       145
#define CHARACTER_JELO      146
#define CHARACTER_JO        147
#define CHARACTER_KALA      148
#define CHARACTER_KALAMA    149
#define CHARACTER_KAMA      150
#define CHARACTER_KASI      151
#define CHARACTER_KEN       152
#define CHARACTER_KEPEKEN   153
#define CHARACTER_KILI      154
#define CHARACTER_KIWEN     155
#define CHARACTER_KO        156
#define CHARACTER_KON       157
#define CHARACTER_KULE      158
#define CHARACTER_KULUPU    159
#define CHARACTER_KUTE      160
#define CHARACTER_LA        161
#define CHARACTER_LAPE      162
#define CHARACTER_LASO      163
#define CHARACTER_LAWA      164
#define CHARACTER_LEN       165
#define CHARACTER_LETE      166
#define CHARACTER_LI        167
#define CHARACTER_LILI      168
#define CHARACTER_LINJA     169
#define CHARACTER_LIPU      170
#define CHARACTER_LOJE      171
#define CHARACTER_LON       172
#define CHARACTER_LUKA      173
#define CHARACTER_LUKIN     174
#define CHARACTER_LUPA      175
#define CHARACTER_MA        176
#define CHARACTER_MAMA      177
#define CHARACTER_MANI      178
#define CHARACTER_MELI      179
#define CHARACTER_MI        180
#define CHARACTER_MIJE      181
#define CHARACTER_MOKU      182
#define CHARACTER_MOLI      183
#define CHARACTER_MONSI     184
#define CHARACTER_MU        185
#define CHARACTER_MUN       186
#define CHARACTER_MUSI      187
#define CHARACTER_MUTE      188
#define CHARACTER_NANPA     189
#define CHARACTER_NASA      190
#define CHARACTER_NASIN     191
#define CHARACTER_NENA      192
#define CHARACTER_NI        193
#define CHARACTER_NIMI      194
#define CHARACTER_NOKA      195
#define CHARACTER_O         196
#define CHARACTER_OLIN      197
#define CHARACTER_ONA       198
#define CHARACTER_OPEN      199
#define CHARACTER_PAKALA    200
#define CHARACTER_PALI      201
#define CHARACTER_PALISA    202
#define CHARACTER_PAN       203
#define CHARACTER_PANA      204
#define CHARACTER_PI        205
#define CHARACTER_PILIN     206
#define CHARACTER_PIMEJA    207
#define CHARACTER_PINI      208
#define CHARACTER_PIPI      209
#define CHARACTER_POKA      210
#define CHARACTER_POKI      211
#define CHARACTER_PONA      212
#define CHARACTER_PU        213
#define CHARACTER_SAMA      214
#define CHARACTER_SELI      215
#define CHARACTER_SELO      216
#define CHARACTER_SEME      217
#define CHARACTER_SEWI      218
#define CHARACTER_SIJELO    219
#define CHARACTER_SIKE      220
#define CHARACTER_SIN       221
#define CHARACTER_SINA      222
#define CHARACTER_SINPIN    223
#define CHARACTER_SITELEN   224
#define CHARACTER_SONA      225
#define CHARACTER_SOWELI    226
#define CHARACTER_SULI      227
#define CHARACTER_SUNO      228
#define CHARACTER_SUPA      229
#define CHARACTER_SUWI      230
#define CHARACTER_TAN       231
#define CHARACTER_TASO      232
#define CHARACTER_TAWA      233
#define CHARACTER_TELO      234
#define CHARACTER_TENPO     235
#define CHARACTER_TOKI      236
#define CHARACTER_TOMO      237
#define CHARACTER_TU        238
#define CHARACTER_UNPA      239
#define CHARACTER_UTA       240
#define CHARACTER_UTALA     241
#define CHARACTER_WALO      242
#define CHARACTER_WAN       243
#define CHARACTER_WASO      244
#define CHARACTER_WAWA      245
#define CHARACTER_WEKA      246
#define CHARACTER_WILE      247

// Character #132 has two names
#define CHARACTER_ALI       CHARACTER_ALE

// Characters #248-#250 (3 "synonyms"(but not so IRL) from lipu pu, 1st ed.)

#define CHARACTER_KIN       248
#define CHARACTER_NAMAKO    249
#define CHARACTER_OKO       250

// Characters #251-#252: nimi almost everyone uses (~ "honorary" pu)

#define CHARACTER_MONSUTA   251
#define CHARACTER_TONSI     252

// Characters #253-#255
// TODO?
// Extra ...
//
// - KIPISI, LEKO, N, KIJETESANTAKALU
// - MAJUNA, KU, SU, MISIKEKE, SOKO
// - LANPAN, LINLUWI, EPIKU, KOKOSILA, MESO, JASIMA
// - punctuation: TE, TO, OPEN_NIMI ([), PINI_NIMI (]), SIKE (.), SIKE_TU (:)

#endif //include guard
