#!/bin/sh


if [ $# -eq 1 ] && [ -f "$1" ]; then :; else
   printf 'Usage: %s PALETTE\n' "$(basename "$0")"
   printf 'Generate C macros for approximation of web colors name\n'
   exit 1
fi >&2


css_colors() {
cat << EOF
// Pink Colors:
MEDIUM_VIOLET_RED       C71585
DEEP_PINK               FF1493
PALE_VIOLET_RED         DB7093
HOT_PINK                FF69B4
LIGHT_PINK              FFB6C1
PINK                    FFC0CB
// Red Colors:
DARK_RED                8B0000
RED                     FF0000
FIREBRICK               B22222
CRIMSON                 DC143C
INDIAN_RED              CD5C5C
LIGHT_CORAL             F08080
SALMON                  FA8072
DARK_SALMON             E9967A
LIGHT_SALMON            FFA07A
// Orange Colors:
ORANGE_RED              FF4500
TOMATO                  FF6347
DARK_ORANGE             FF8C00
CORAL                   FF7F50
ORANGE                  FFA500
// Yellow Colors:
DARK_KHAKI              BDB76B
GOLD                    FFD700
KHAKI                   F0E68C
PEACH_PUFF              FFDAB9
YELLOW                  FFFF00
PALE_GOLDENROD          EEE8AA
MOCCASIN                FFE4B5
PAPAYA_WHIP             FFEFD5
LIGHT_GOLDENROD_YELLOW  FAFAD2
LEMON_CHIFFON           FFFACD
LIGHT_YELLOW            FFFFE0
// Brown Colors:
MAROON                  800000
BROWN                   A52A2A
SADDLE_BROWN            8B4513
SIENNA                  A0522D
CHOCOLATE               D2691E
DARK_GOLDENROD          B8860B
PERU                    CD853F
ROSY_BROWN              BC8F8F
GOLDENROD               DAA520
SANDY_BROWN             F4A460
TAN                     D2B48C
BURLYWOOD               DEB887
WHEAT                   F5DEB3
NAVAJO_WHITE            FFDEAD
BISQUE                  FFE4C4
BLANCHED_ALMOND         FFEBCD
CORNSILK                FFF8DC
// Purple, Violet, and Magenta Colors:
INDIGO                  4B0082
PURPLE                  800080
DARK_MAGENTA            8B008B
DARK_VIOLET             9400D3
DARK_SLATE_BLUE         483D8B
BLUE_VIOLET             8A2BE2
DARK_ORCHID             9932CC
FUCHSIA                 FF00FF
MAGENTA                 FF00FF
SLATE_BLUE              6A5ACD
MEDIUM_SLATE_BLUE       7B68EE
MEDIUM_ORCHID           BA55D3
MEDIUM_PURPLE           9370DB
ORCHID                  DA70D6
REBECCA_PURPLE          663399
VIOLET                  EE82EE
PLUM                    DDA0DD
THISTLE                 D8BFD8
LAVENDER                E6E6FA
// Blue Colors:
MIDNIGHT_BLUE           191970
NAVY                    000080
DARK_BLUE               00008B
MEDIUM_BLUE             0000CD
BLUE                    0000FF
ROYAL_BLUE              4169E1
STEEL_BLUE              4682B4
DODGER_BLUE             1E90FF
DEEP_SKY_BLUE           00BFFF
CORNFLOWER_BLUE         6495ED
SKY_BLUE                87CEEB
LIGHT_SKY_BLUE          87CEFA
LIGHT_STEEL_BLUE        B0C4DE
LIGHT_BLUE              ADD8E6
POWDER_BLUE             B0E0E6
// Cyan Colors:
TEAL                    008080
DARK_CYAN               008B8B
LIGHT_SEA_GREEN         20B2AA
CADET_BLUE              5F9EA0
DARK_TURQUOISE          00CED1
MEDIUM_TURQUOISE        48D1CC
TURQUOISE               40E0D0
AQUA                    00FFFF
CYAN                    00FFFF
AQUAMARINE              7FFFD4
PALE_TURQUOISE          AFEEEE
LIGHT_CYAN              E0FFFF
// Green Colors:
DARK_GREEN              006400
GREEN                   008000
DARK_OLIVE_GREEN        556B2F
FOREST_GREEN            228B22
SEA_GREEN               2E8B57
OLIVE                   808000
OLIVE_DRAB              6B8E23
MEDIUM_SEA_GREEN        3CB371
LIME_GREEN              32CD32
LIME                    00FF00
SPRING_GREEN            00FF7F
MEDIUM_SPRING_GREEN     00FA9A
DARK_SEA_GREEN          8FBC8F
MEDIUM_AQUAMARINE       66CDAA
YELLOW_GREEN            9ACD32
LAWN_GREEN              7CFC00
CHARTREUSE              7FFF00
LIGHT_GREEN             90EE90
GREEN_YELLOW            ADFF2F
PALE_GREEN              98FB98
// White Colors:
MISTY_ROSE              FFE4E1
ANTIQUE_WHITE           FAEBD7
LINEN                   FAF0E6
BEIGE                   F5F5DC
WHITE_SMOKE             F5F5F5
LAVENDER_BLUSH          FFF0F5
OLD_LACE                FDF5E6
ALICE_BLUE              F0F8FF
SEASHELL                FFF5EE
GHOST_WHITE             F8F8FF
HONEYDEW                F0FFF0
FLORAL_WHITE            FFFAF0
AZURE                   F0FFFF
MINT_CREAM              F5FFFA
SNOW                    FFFAFA
IVORY                   FFFFF0
WHITE                   FFFFFF
// Gray and Black Colors:
BLACK                   000000
DARK_SLATE_GRAY         2F4F4F
DIM_GRAY                696969
SLATE_GRAY              708090
GRAY                    808080
LIGHT_SLATE_GRAY        778899
DARK_GRAY               A9A9A9
SILVER                  C0C0C0
LIGHT_GRAY              D3D3D3
GAINSBORO               DCDCDC
EOF
}

printf '// Web Color Names (generated by %s)\n\n' "$(basename "$0")"
printf '//-- WebColorName                     Color  TrueColor   Distance\n'
# very inefficient but gets the job done
css_colors |
while IFS= read -r line; do
   case $line in
      ''|//*) printf '%s\n' "$line";;

           *) printf '#define COLOR_'
              color=$(printf %s "$line" | awk '{print $2}') || exit
              index=$(kule-closest -q -p "$1" "$color" | head -n 1)
              error=$(kule-closest -q -p "$1" "$color" | tail -n 1)
              str=$(printf ' %3d   // %s   %s' "$index" "$color" "$error")
              printf '%s\n' "$line" | sed "s| [^ ].*| $str|"
              ;;
   esac
done
