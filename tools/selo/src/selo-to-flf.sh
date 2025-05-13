#!/bin/sh

# Usage: $0 file.selo
# Print on stdout an octant-based .flf FIGlet font corresponfing to file.selo

. "$KONPU_HOME"/tools/dev/lib/common.sh
requires glyph-print

# Print header: a figlet font starts like this:
#
#          flf2a$ 6 5 20 15 3 0 143 229
#            |  | | | |  |  | |  |   |
#           /  /  | | |  |  | |  |   \
#  Signature  /  /  | |  |  | |   \   Codetag_Count
#    Hardblank  /  /  |  |  |  \   Full_Layout*
#         Height  /   |  |   \  Print_Direction
#         Baseline   /    \   Comment_Lines
#          Max_Length      Old_Layout*
#
# Hardblank      : We don't use, is usually a $ in figlet fonts
# Height         : Equal for every chars, find it from the first char defined.
HEIGHT=$(head -n 1 "$1" | {
         read -r codepoint glyph
         case ${#glyph} in
              2|4) echo 1 ;; # Glyph8/16
             8|16) echo 2 ;; # Glyph32/64
            33|67) echo 4 ;; # Glyph128/256
                *) die "Wrong glyph length for codepoint $codepoint";;
         esac
       })
# Baseline       : TODO
# Max_Length     : We set this to 8 (max # octant in a Glyph256 line)
#                               * 4 (max #bytes encoding an octant in UTF-8)
#                               + 2 (to accommodate @@ endmarks) ==> 34
# Old_Layout     : -1 = Full-width layout by default
# Comment_Lines  : We decide to add 4 lines of comments (see below)
# Print_Direction: 0  = Left-to-right
# Full_Layout    : We don't use any rules for those simple fonts, thus 0.
# Codetag Count  : The number of code-tagged (non-required) characters, ie
#                  number of characters minus the 102 required ones
#                  (95 printable ascii + 7 germans).
CODETAG_COUNT=$(( $(wc -l < "$1") - 102 ))
#
printf 'flf2a$ %d 2 34 -1 4 0 0 %d\n' "$HEIGHT" "$CODETAG_COUNT"


# The 4 lines of comments we decided to add:
printf '#\n'
printf '# Font %s (converted by selo-to-flf tool)\n' "$(basename "$1")"
printf "# selo Konpu's (sub)project, see: %s\n" "$KONPU_URL"
printf '#\n'


# The Glyphs
c=0
while read -r codepoint glyph
do
   [ $c -lt 102 ] || printf "0x%s\n" "$codepoint"
   c=$((c + 1))
   printf %s "$(glyph-print --octant "$glyph")" | sed 's/$/@/'
   echo '@'
done < "$1"
