#!/bin/sh

# Usage: $0
# Print the single header file for ilo Konpu


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

# Start of the file
cat << EOF
// Single header file for ilo Konpu.
// See: $KONPU_URL
//

#ifndef  KONPU_H_
#define  KONPU_H_

// "Entry point" function that must be implemented to create a Konpu app
int AppInit(void);

// Konpu's External Dependencies
#if (__STDC_VERSION__ < 202311L) && !defined(__GNUC__)
EOF
hr
cat  "$KONPU_HOME"/external/jtckdint-1.0/jtckdint.h
hr
echo "#endif // End of Konpu's External Dependencies'"


# API Includes
include  config.h platform.h c.h                                               \
         bits.h util.h character.h time.h random.h options.h                   \
         var.h arch.h                                                          \
         printer.h \
         rectangle.h form.h video_mode.h keyboard.h video_mode_auto.h          \
         glyph.h tile.h strip.h attribute.h color.h                            \
         video.h pixel.h tile.h                                                \
         draw.h                                                                \
         error.h heap.h stack.h

# Extra Resources
include  glyph_constants/glyph_patterns_blocks.h    \
         glyph_constants/glyph_patterns_8x8.me.h    \
         glyph_constants/glyph_ascii.h              \
         glyph_constants/glyph_tokipona.h           \
         glyph_constants/glyph_tokipona_halfwidth.h \
         glyph_constants/glyph_tukitiki.h

# End of the file
hr
cat << EOF

#endif //KONPU_H_

EOF
