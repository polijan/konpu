#!/bin/sh

# Usage: $0
# Generate #defines that transforms the beautiful patterns from the 8x8.me
# project into Glyph64 constants for ilo Konpu.
#
# 1. Clone the 8x8.me git repo: git clone https://github.com/ace-dent/8x8.me.git
# 2. ./generate_pattern_defines.sh


# filter to transform stdin from CamelCase to SNAKE_CASE
camel_to_snake_case() {
   sed -E 's/([a-z])([A-Z])/\1_\2/g' | tr 'a-z' 'A-Z'
}

# In case we do not have tac(1), here's an implementation of it in awk
tac_() {
   awk '
      { lines[NR] = $0 }
      END { for(i = NR; i > 0; i--)  print lines[i] }
   '
}

# Read the 8x8 pbm file contained in "$1" and output a Glyph64
pbm_to_glyph() {
   awk '
      # Define a Pre-computed array for binary-to-hex conversion of a nibble
      BEGIN {
         bin_to_hex["0000"] = "0"
         bin_to_hex["0001"] = "1"
         bin_to_hex["0010"] = "2"
         bin_to_hex["0011"] = "3"
         bin_to_hex["0100"] = "4"
         bin_to_hex["0101"] = "5"
         bin_to_hex["0110"] = "6"
         bin_to_hex["0111"] = "7"
         bin_to_hex["1000"] = "8"
         bin_to_hex["1001"] = "9"
         bin_to_hex["1010"] = "A"
         bin_to_hex["1011"] = "B"
         bin_to_hex["1100"] = "C"
         bin_to_hex["1101"] = "D"
         bin_to_hex["1110"] = "E"
         bin_to_hex["1111"] = "F"
      }

      /^[01]/ {
         # Combine the 8 binary digits into a single string.
         # We also mirror the the line.
         bin = $8 "" $7 "" $6 "" $5 "" $4 "" $3 "" $2 "" $1

         # Split the binary string into two nibbles
         first_nibble  = substr(bin, 1, 4)
         second_nibble = substr(bin, 5, 4)

         # Convert each nibble to hexadecimal and print
         printf "%s%s\n", bin_to_hex[first_nibble], bin_to_hex[second_nibble]
      }
   ' "$1" |
   tac_ | tr -d '\n' # Flip The Lines
#   printf '\n'
}


work() {
   for family in [0-9]*; do
       [ -d "$family" ] || continue
       for image in "$family"/pbm/*.pbm; do
           [ -f "$image" ] || continue
           printf "#define GLYPH64_PATTERN_"
           printf %s "$image" |
              sed -e 's/^.*-//' -e 's|/pbm/||' -e 's/.pbm$//' |
              camel_to_snake_case
           printf '  GLYPH64('; pbm_to_glyph "$image"; printf ')\n'
       done
   done
}


cd "$KONPU_HOME/tmp/8x8.me" || {
   printf 'You first need to clone the 8x8.me git repo in %s\n' "$KONPU_HOME/tmp" >&2
   exit 1
}

GUARD='KONPU_GLYPH_PATTERNS_8x8_ME_H'
printf '// The awesome 8x8.me fill patterns as Glyph64.\n'
printf '// Patterns from ace-dent, see: from https://github.com/ace-dent/8x8.me\n'
printf '// Those patterns are in Public Domain / CC0.\n'
printf '// Automatically Converted: see tools/patterns\n\n'
printf '#ifndef %s\n' "$GUARD"
printf '#define %s\n' "$GUARD"
printf '#include "glyph.h"\n\n'

work | if command -v 'column' >/dev/null
          then column -t | sed 's/  / /'
          else cat
       fi

printf '\n#endif //%s\n' "$GUARD"
