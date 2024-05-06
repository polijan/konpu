#!/bin/sh

# Usage: $0
# print a single header file for ilo Konpu


die() { printf '%s\n' "$*... " >&2; exit 5; }
hr()  { printf '\n//%78s\n' '' | tr ' ' '@'; }

headers() {
    cd src || exit
    printf '// Single header file for ilo Konpu.\n\n'
    printf '#ifndef  KONPU_H\n'
    printf '#define  KONPU_H\n'
    for header in "$@"; do
        printf '\n'; hr
        [ -f "$header" ] || die "$header not found"
        printf '// %s' "$header"; hr
        # print header, but remove non-system #includes
        sed 's|^#include "|// #include "|g' < "$header"
    done
    hr; printf '\n#endif //KONPU_H\n'
}

headers  config.h platform.h c.h \
         heap.h
