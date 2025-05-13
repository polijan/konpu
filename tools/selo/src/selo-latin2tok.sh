#!/bin/sh

# Usage: $0 [SED_ARGUMENT]...
#
# sed(1)-based filter that transforms toki pona input written using
# ASCII latin to sitelen pona using the ConScript Unicode Registry (UCSUR)
# private codepoints (sitelen pona proposal as updated on 2022-01-31)
#
# In addition to latin input, the following special characters can
# be used as input and will be interpreted as follow:
#
# * long pi:
#   pi-   replaced  with U+F1993  (START OF LONG PI)
#   __    replaced* with U+F1994  (COMBINING LONG PI EXTENSION)
#
# * cartouches:
#   [ replaced  with  U+F1990  (START OF CARTOUCHE)
#   ] replaced  with  U+F1991  (END OF CARTOUCHE)
#   _ replaced* with  U+F1992  (COMBINING CARTOUCHE EXTENSION)
#                     '-> adds lines above and below the previous glyph
# * combining glyphs:
#   - replaced* with  U+200D  (ZERO WIDTH JOINER)
#   ^ replaced* with  U+F1995 (STACKING JOINER)
#   + replaced* with  U+F1996 (SCALING JOINER)

. "$KONPU_HOME"/tools/dev/lib/common.sh

LC_ALL=C                            \
sed -e 's/\<a\>/󱤀/g'               \
    -e 's/\<akesi\>/󱤁/g'           \
    -e 's/\<ala\>/󱤂/g'             \
    -e 's/\<alasa\>/󱤃/g'           \
    -e 's/\<ale\>/󱤄/g'             \
    -e 's/\<ali\>/󱤄/g'             \
    -e 's/\<anpa\>/󱤅/g'            \
    -e 's/\<ante\>/󱤆/g'            \
    -e 's/\<anu\>/󱤇/g'             \
    -e 's/\<awen\>/󱤈/g'            \
    -e 's/\<e\>/󱤉/g'               \
    -e 's/\<en\>/󱤊/g'              \
    -e 's/\<esun\>/󱤋/g'            \
    -e 's/\<ijo\>/󱤌/g'             \
    -e 's/\<ike\>/󱤍/g'             \
    -e 's/\<ilo\>/󱤎/g'             \
    -e 's/\<insa\>/󱤏/g'            \
    -e 's/\<jaki\>/󱤐/g'            \
    -e 's/\<jan\>/󱤑/g'             \
    -e 's/\<jelo\>/󱤒/g'            \
    -e 's/\<jo\>/󱤓/g'              \
    -e 's/\<kala\>/󱤔/g'            \
    -e 's/\<kalama\>/󱤕/g'          \
    -e 's/\<kama\>/󱤖/g'            \
    -e 's/\<kasi\>/󱤗/g'            \
    -e 's/\<ken\>/󱤘/g'             \
    -e 's/\<kepeken\>/󱤙/g'         \
    -e 's/\<kili\>/󱤚/g'            \
    -e 's/\<kiwen\>/󱤛/g'           \
    -e 's/\<ko\>/󱤜/g'              \
    -e 's/\<kon\>/󱤝/g'             \
    -e 's/\<kule\>/󱤞/g'            \
    -e 's/\<kulupu\>/󱤟/g'          \
    -e 's/\<kute\>/󱤠/g'            \
    -e 's/\<la\>/󱤡/g'              \
    -e 's/\<lape\>/󱤢/g'            \
    -e 's/\<laso\>/󱤣/g'            \
    -e 's/\<lawa\>/󱤤/g'            \
    -e 's/\<len\>/󱤥/g'             \
    -e 's/\<lete\>/󱤦/g'            \
    -e 's/\<li\>/󱤧/g'              \
    -e 's/\<lili\>/󱤨/g'            \
    -e 's/\<linja\>/󱤩/g'           \
    -e 's/\<lipu\>/󱤪/g'            \
    -e 's/\<loje\>/󱤫/g'            \
    -e 's/\<lon\>/󱤬/g'             \
    -e 's/\<luka\>/󱤭/g'            \
    -e 's/\<lukin\>/󱤮/g'           \
    -e 's/\<lupa\>/󱤯/g'            \
    -e 's/\<ma\>/󱤰/g'              \
    -e 's/\<mama\>/󱤱/g'            \
    -e 's/\<mani\>/󱤲/g'            \
    -e 's/\<meli\>/󱤳/g'            \
    -e 's/\<mi\>/󱤴/g'              \
    -e 's/\<mije\>/󱤵/g'            \
    -e 's/\<moku\>/󱤶/g'            \
    -e 's/\<moli\>/󱤷/g'            \
    -e 's/\<monsi\>/󱤸/g'           \
    -e 's/\<mu\>/󱤹/g'              \
    -e 's/\<mun\>/󱤺/g'             \
    -e 's/\<musi\>/󱤻/g'            \
    -e 's/\<mute\>/󱤼/g'            \
    -e 's/\<nanpa\>/󱤽/g'           \
    -e 's/\<nasa\>/󱤾/g'            \
    -e 's/\<nasin\>/󱤿/g'           \
    -e 's/\<nena\>/󱥀/g'            \
    -e 's/\<ni\>/󱥁/g'              \
    -e 's/\<nimi\>/󱥂/g'            \
    -e 's/\<noka\>/󱥃/g'            \
    -e 's/\<o\>/󱥄/g'               \
    -e 's/\<olin\>/󱥅/g'            \
    -e 's/\<ona\>/󱥆/g'             \
    -e 's/\<open\>/󱥇/g'            \
    -e 's/\<pakala\>/󱥈/g'          \
    -e 's/\<pali\>/󱥉/g'            \
    -e 's/\<palisa\>/󱥊/g'          \
    -e 's/\<pan\>/󱥋/g'             \
    -e 's/\<pana\>/󱥌/g'            \
    -e 's/\<pilin\>/󱥎/g'           \
    -e 's/\<pimeja\>/󱥏/g'          \
    -e 's/\<pini\>/󱥐/g'            \
    -e 's/\<pipi\>/󱥑/g'            \
    -e 's/\<poka\>/󱥒/g'            \
    -e 's/\<poki\>/󱥓/g'            \
    -e 's/\<pona\>/󱥔/g'            \
    -e 's/\<pu\>/󱥕/g'              \
    -e 's/\<sama\>/󱥖/g'            \
    -e 's/\<seli\>/󱥗/g'            \
    -e 's/\<selo\>/󱥘/g'            \
    -e 's/\<seme\>/󱥙/g'            \
    -e 's/\<sewi\>/󱥚/g'            \
    -e 's/\<sijelo\>/󱥛/g'          \
    -e 's/\<sike\>/󱥜/g'            \
    -e 's/\<sin\>/󱥝/g'             \
    -e 's/\<sina\>/󱥞/g'            \
    -e 's/\<sinpin\>/󱥟/g'          \
    -e 's/\<sitelen\>/󱥠/g'         \
    -e 's/\<sona\>/󱥡/g'            \
    -e 's/\<soweli\>/󱥢/g'          \
    -e 's/\<suli\>/󱥣/g'            \
    -e 's/\<suno\>/󱥤/g'            \
    -e 's/\<supa\>/󱥥/g'            \
    -e 's/\<suwi\>/󱥦/g'            \
    -e 's/\<tan\>/󱥧/g'             \
    -e 's/\<taso\>/󱥨/g'            \
    -e 's/\<tawa\>/󱥩/g'            \
    -e 's/\<telo\>/󱥪/g'            \
    -e 's/\<tenpo\>/󱥫/g'           \
    -e 's/\<toki\>/󱥬/g'            \
    -e 's/\<tomo\>/󱥭/g'            \
    -e 's/\<tu\>/󱥮/g'              \
    -e 's/\<unpa\>/󱥯/g'            \
    -e 's/\<uta\>/󱥰/g'             \
    -e 's/\<utala\>/󱥱/g'           \
    -e 's/\<walo\>/󱥲/g'            \
    -e 's/\<wan\>/󱥳/g'             \
    -e 's/\<waso\>/󱥴/g'            \
    -e 's/\<wawa\>/󱥵/g'            \
    -e 's/\<weka\>/󱥶/g'            \
    -e 's/\<wile\>/󱥷/g'            \
    -e 's/\<namako\>/󱥸/g'          \
    -e 's/\<kin\>/󱥹/g'             \
    -e 's/\<oko\>/󱥺/g'             \
    -e 's/\<kipisi\>/󱥻/g'          \
    -e 's/\<leko\>/󱥼/g'            \
    -e 's/\<monsuta\>/󱥽/g'         \
    -e 's/\<tonsi\>/󱥾/g'           \
    -e 's/\<jasima\>/󱥿/g'          \
    -e 's/\<kijetesantakalu\>/󱦀/g' \
    -e 's/\<soko\>/󱦁/g'            \
    -e 's/\<meso\>/󱦂/g'            \
    -e 's/\<epiku\>/󱦃/g'           \
    -e 's/\<kokosila\>/󱦄/g'        \
    -e 's/\<lanpan\>/󱦅/g'          \
    -e 's/\<n\>/󱦆/g'               \
    -e 's/\<misikeke\>/󱦇/g'        \
    -e 's/\<ku\>/󱦈/g'              \
                                    \
    -e 's/\<pake\>/󱦠/g'            \
    -e 's/\<apeja\>/󱦡/g'           \
    -e 's/\<majuna\>/󱦢/g'          \
    -e 's/\<powe\>/󱦣/g'            \
                                    \
    -e 's/\[/󱦐/g'                  \
    -e 's/\]/󱦑/g'                  \
    -e 's/\<pi-/󱦓/g'               \
    -e 's/\<pi\>/󱥍/g'              \
                                    \
    -e "s/${SP}__/󱦔/g"             \
    -e "s/${SP}_/󱦒/g"              \
    -e "s/${SP}-/‍/g"               \
    -e "s/${SP}\^/󱦕/g"             \
    -e "s/${SP}\+/󱦖/g"             \
                                    \
    "$@"



