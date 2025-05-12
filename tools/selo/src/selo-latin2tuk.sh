#!/bin/sh

# Usage: $0 [SED_ARGUMENT]...
#
# sed(1)-based filter that transforms tuki tiki input written using
# ASCII latin to titi pula using the ConScript Unicode Registry (UCSUR)
# private codepoints (titi pula as updated on 2024-04-11)
#
# In addition to latin input, the following special characters can
# be used as input and will be interpreted as follow and will result in
# characters from UCSUR SITELEN PONA.
#   [  is replaced with  U+F1990  (START OF CARTOUCHE)
#   ]  is replaced with  U+F1991  (END OF CARTOUCHE)


LC_ALL=C                 \
sed -e 's/\<a\>/󱱀/g'    \
    -e 's/\<ala\>/󱱁/g'  \
    -e 's/\<i\>/󱱂/g'    \
    -e 's/\<iku\>/󱱃/g'  \
    -e 's/\<ilu\>/󱱄/g'  \
    -e 's/\<ka\>/󱱅/g'   \
    -e 's/\<kati\>/󱱆/g' \
    -e 's/\<ki\>/󱱇/g'   \
    -e 's/\<kiku\>/󱱈/g' \
    -e 's/\<ku\>/󱱉/g'   \
    -e 's/\<la\>/󱱊/g'   \
    -e 's/\<lapi\>/󱱋/g' \
    -e 's/\<li\>/󱱌/g'   \
    -e 's/\<lika\>/󱱍/g' \
    -e 's/\<lili\>/󱱎/g' \
    -e 's/\<lu\>/󱱏/g'   \
    -e 's/\<lupa\>/󱱐/g' \
    -e 's/\<mi\>/󱱑/g'   \
    -e 's/\<muku\>/󱱒/g' \
    -e 's/\<muti\>/󱱓/g' \
    -e 's/\<paka\>/󱱔/g' \
    -e 's/\<pali\>/󱱕/g' \
    -e 's/\<puka\>/󱱖/g' \
    -e 's/\<pula\>/󱱗/g' \
    -e 's/\<taka\>/󱱘/g' \
    -e 's/\<tama\>/󱱙/g' \
    -e 's/\<tiki\>/󱱚/g' \
    -e 's/\<tiku\>/󱱛/g' \
    -e 's/\<tila\>/󱱜/g' \
    -e 's/\<tilu\>/󱱝/g' \
    -e 's/\<timi\>/󱱞/g' \
    -e 's/\<tipi\>/󱱟/g' \
    -e 's/\<titi\>/󱱠/g' \
    -e 's/\<tu\>/󱱡/g'   \
    -e 's/\<tuki\>/󱱢/g' \
    -e 's/\<tula\>/󱱣/g' \
    -e 's/\<tulu\>/󱱤/g' \
    -e 's/\<uli\>/󱱥/g'  \
    -e 's/\<upi\>/󱱦/g'  \
                         \
    -e 's/\[/󱦐/g'       \
    -e 's/\]/󱦑/g'       \
                         \
    "$@"
