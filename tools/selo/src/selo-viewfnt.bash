#!/usr/bin/env bash

# Usage: $0 [file.fnt]...
# Interactive interface to preview fnt fonts
# (Requires: fzf)

. "$KONPU_HOME"/tools/dev/lib/common.sh
requires fzf
[ $# -ne 0 ] || usage

cat "$@"                       |
while read -r codepoint glyph; do
   # note: bash required for printf to display the character
   printf '%s  %s  %b\n' "$codepoint" "$glyph" "\U$codepoint"
done                           |
fzf --prompt '.fnt viewer > '  \
    --reverse --inline-info    \
    --preview-window 'down:5'  \
    --preview 'glyph-print "$(printf "%s\n" {2})"'
