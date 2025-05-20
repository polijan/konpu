#!/bin/sh

# Usage: $0
# print a single header file for ilo Konpu


die() { printf '%s\n' "$*... " >&2; exit 5; }
hr()  { printf '\n//%78s\n' '' | tr ' ' '@'; }

include() {
    for header in "$@"; do
        printf '\n'; hr
        [ -f "$header" ] || die "$header not found"
        printf '// %s' "$header"; hr
        # print header, but remove non-system #includes
        sed 's|^#include "|// #include "|g' < "$header"
    done
}

cd src || exit
printf '// Single header file for ilo Konpu.\n\n'
printf '#ifndef  KONPU_H\n'
printf '#define  KONPU_H\n'

# API Includes
include  config.h platform.h c.h \
         bits.h util.h time.h random.h options.h \
         var.h memory.h key.h \
         video_mode.h video_mode_auto.h glyph.h tile.h color.h attribute.h \
         video.h \
         error.h heap.h stack.h \
         \
         printer.h

# Extra Resources
include  glyph_constants/glyph_patterns_blocks.h \
         glyph_constants/glyph_patterns_8x8.me.h \
         glyph_constants/glyph_tokipona.h        \
         glyph_constants/glyph_tukitiki.h

hr
printf '\n#endif //KONPU_H\n'
