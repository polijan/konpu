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

include  config.h platform.h c.h \
         util.h options.h \
         var.h memory.h \
         video_mode.h video_mode_auto.h glyph.h tile.h color.h attribute.h \
         video.h \
         error.h heap.h stack.h \
         \
         printer.h

hr
printf '\n#endif //KONPU_H\n'
