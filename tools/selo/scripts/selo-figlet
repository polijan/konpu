#!/bin/sh

# Usage: $0 [FIGLET_ARGS]...
# A wrapper around figlet for Konpu.
#
# - Use `figlet` or if not installed, try `toilet`
# - By default set the output width to the terminal width (use -w to override)
# - Set the default font to use (use -f to select another font)

. "$KONPU_HOME"/tools/dev/lib/common.sh
FONT_DEFAULT=$KONPU_HOME/tools/selo/fonts.out/selo.flf

if exists figlet; then
   FIGLET=figlet
   WIDTH=$(stty size | cut -d' ' -f 2)
   if [ -t 1 ]; then set -- -w "$WIDTH" "$@"; fi
   set -- -C utf8 "$@"
elif exists toilet; then
   FIGLET=toilet
   if [ -t 1 ]; then set -- -t "$@"; fi
else
   die 'Cannot find a suitable figlet program'
fi

[ -f "$FONT_DEFAULT" ] && set -- -f "$FONT_DEFAULT" "$@"

$FIGLET "$@"
