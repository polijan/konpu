#!/bin/sh

# Usage: $0
# Fetch the Sokol developement headers

#-------------------------------------------------------------------------------
# Sokol URL
#-------------------------------------------------------------------------------
REPO_URL=https://github.com/floooh/sokol/
RAW_URL=https://raw.githubusercontent.com/floooh/sokol/refs/heads/master
#-------------------------------------------------------------------------------

. "$KONPU_HOME"/tools/dev/lib/common.sh

fetch() {
   if   exists wget; then wget    "${RAW_URL}/$*"
   elif exists curl; then curl -O "${RAW_URL}/$*"
   else die 'cannot find wget or curl'
   fi || exit
}

mkdir -p sokol || exit
cd sokol       || exit
fetch sokol_app.h
fetch sokol_args.h
fetch sokol_audio.h
fetch sokol_fetch.h
fetch sokol_gfx.h
fetch sokol_glue.h
fetch sokol_log.h
fetch sokol_time.h
printf 'Soko headers\nFetched on %s.\nFrom: %s\n' \
   "$(date +%Y-%m-%d)" "$REPO_URL" > README
