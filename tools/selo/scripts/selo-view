#!/usr/bin/env bash

# Usage: $0 [file.selo]... -> Preview the given selo font
#        ... | $0          -> Preview a selo font inputted from stdin
#
# Interactive fzf interface to preview selo fonts
#


. "$KONPU_HOME"/tools/dev/lib/common.sh
[ $# -eq 0 ] && [ -t 0 ] && usage
requires fzf

cat "$@"                           |
while read -r codepoint glyph; do
   # note: bash required for printf to display the character
   printf '%s  %s  %b\n' "$codepoint" "$glyph" "\U$codepoint"
done                               |
fzf --prompt 'selo font viewer > ' \
    --reverse --inline-info        \
    --preview-window 'down:5'      \
    --preview 'glyph-print "$(printf "%s\n" {2})"'
