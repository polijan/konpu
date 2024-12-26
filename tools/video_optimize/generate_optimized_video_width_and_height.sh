#!/bin/sh

# Usage: $0
# Code that generate the VIDEO_WIDTH and VIDEO_HEIGHT when the
# KONPU_OPTION_OPTIMIZE_VIDEO_MODE option is (well) defined.

start() {
    printf '#  if defined(KONPU_OPTION_OPTIMIZE_VIDEO_MODE) && '
    printf      "!defined(KONPU_OPTION_FORCE_VIDEO_MODE_%s)\n" "$1"
}
define() {
  awk -v "DIMENSION=$1" 'BEGIN { IF_ELIF = "if" }
  /^[[:space:]]*[0-9]/ {
    printf "#     %s KONPU_OPTION_OPTIMIZE_VIDEO_MODE == %d\n", IF_ELIF, $1
    printf "#        define VIDEO_%s   %3d\n", DIMENSION, $3;
    IF_ELIF = "elif"
  }'
}
end() {
    echo   '#     else'
    echo   '#        error "KONPU_OPTION_OPTIMIZE_VIDEO_MODE has invalid value"'
    echo   '#     endif // end check value of KONPU_OPTION_OPTIMIZE_VIDEO_MODE'
    echo   '#  endif'
}


[ -z "$BASH_VERSION" ] || set -o pipefail
gen_code() {
   [ -f "$1" ] || exit 3

   start 'WIDTH'
   sed -e 's/x.*/ /' "$1" | define 'WIDTH'
   end

   start 'HEIGHT'
   sed -e 's/x/ /' "$1" | define 'HEIGHT'
   end
}


echo '// THIS PREPROCESSOR CODE IS AUTO-GENERATED (see tools)'
echo '// DO NOT EDIT / DO NOT INCLUDE DIRECTLY'
echo
echo '#if   VIDEO_SIZE_FACTOR_ == 7'; gen_code modes_factor7.txt
echo '#elif VIDEO_SIZE_FACTOR_ == 6'; gen_code modes_factor6.txt
echo '#elif VIDEO_SIZE_FACTOR_ == 5'; gen_code modes_factor5.txt
echo '#elif VIDEO_SIZE_FACTOR_ == 3'; gen_code modes_factor4.txt
echo '#elif VIDEO_SIZE_FACTOR_ == 3'; gen_code modes_factor3.txt
echo '#elif VIDEO_SIZE_FACTOR_ == 2'; gen_code modes_factor2.txt
echo '#elif defined(KONPU_OPTION_OPTIMIZE_VIDEO_MODE)'
echo '#  error "KONPU_OPTION_OPTIMIZE_VIDEO_MODE cannot be used with this VIDEO_SIZE_FACTOR_"'
echo '#endif //VIDEO_SIZE_FACTOR_'
