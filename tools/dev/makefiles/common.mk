#-------------------------------------------------------------------------------
# Makefile flags
#-------------------------------------------------------------------------------
# Disable IMPLICIT Rules and Builtin Variables
# (I prefer to be explicit, even if that's more verbose)
MAKEFLAGS += -rR

# Try to compile faster by running in parallel
# Note: You can be override this by manually invoking make with a -j flag
#       (for example, use: make -j 1 for building in a non-parallel way)
MAKEFLAGS = -j $(shell dev-nproc) $(MAKEFILE)

#-------------------------------------------------------------------------------
# OS / Platform: `platform`
# (values can be `linux` (default), soon also `web`, and maybe/later others)
#-------------------------------------------------------------------------------

ifndef platform
	platform = linux
endif

#-------------------------------------------------------------------------------
# C Compiler: `compiler` will set variable CC for compilation
# - native platform: `gcc` (default) or `clang`
# - web platform: ignored, CC will be set to `emcc` (EmScriptem)
#-------------------------------------------------------------------------------

ifeq ($(platform), web)
	CC = emcc # Use EmScripten for compiling
else ifndef compiler
	CC = gcc
else
	CC = $(compiler)
endif

CFLAGS += -fdiagnostics-color

#-------------------------------------------------------------------------------
# Build type: variable `build` may have the following values:
# - `debug`        : set up debugger info and minimal optimization
# - `dev` (default): like `debug` and also: - enable some suggestions
#                                           - will NOT build with warnings
# - `release`      : set up a lot of optimizations
#-------------------------------------------------------------------------------

ifndef build
	build = dev
endif

ifeq ($(build), dev)
	CFLAGS_WARNINGS += -Werror #<-- refuse to build with warnings
	build = debug
	# TODO: we may add -Wsuggest* "warnings" when build is `dev`
	#       search for -Wsuggest in: https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
endif
ifeq ($(build), debug)
	# see: https://gcc.gnu.org/onlinedocs/gcc/Debugging-Options.html#Debugging-Options
	# -g     produces debug info in the OS' native format.
	# -ggdb  produces debug info specifically intended for gdb.
	# -ggdb3 produces extra info, for example including macro definitions.
	CFLAGS_DEBUG    += -ggdb3
	CFLAGS_OPTIMIZE += -Og
else ifeq ($(build), release)
	CFLAGS_DEBUG    += -g0 -DNDEBUG
	CFLAGS_OPTIMIZE += -Ofast -fexpensive-optimizations -flto
	ifneq ($(platform), web)
		CFLAGS_ARCH += -march=native -mtune=native
	endif
endif


	# Optimization:
	# + -O2 or `-O3 -march=native -mtune=native -fexpensive-optimizations` or ...
	# + -Wsuggest* warnings, search for -Wsuggest in: https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
	#COPTIMIZE = -DNDEBUG -O3 -march=native -mtune=native -fexpensive-optimizations
	#COPTIMIZE = -O2 \
	#        \
	#        -Wsuggest-attribute=pure -Wsuggest-attribute=const       \
	#        -Wsuggest-attribute=noreturn -Wsuggest-attribute=malloc  \
	#        -Wsuggest-attribute=format -Wsuggest-attribute=cold \
	##        -Wsuggest-attribute=returns_nonnull




#-------------------------------------------------------------------------------
# Base Warnings (to apply everywhere / add some some if you want/can)
# the classics,     ie: -Wall -Wextra -(W)pedantic
# + -fanalyzer,     see: https://gcc.gnu.org/onlinedocs/gcc/Static-Analyzer-Options.html
# + extra warnings, see: https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
#-------------------------------------------------------------------------------
CFLAGS_WARNINGS += -Wall -Wextra -pedantic  \
        \
        -fanalyzer \
        \
        -Wundef -Wstrict-prototypes  -Wcast-align                        \
        -Wmissing-format-attribute -Wnested-externs -Wpointer-arith      \
        -Wreturn-type -Wswitch -Wshadow -Wcast-qual -Wwrite-strings      \
        -Wformat=2 -Wpointer-sign -Wmissing-noreturn                     \
		-Wvla-larger-than=0                                              \
        \
        -Wno-cast-align -Wno-unused-function  \
        \
        -Wfloat-equal \
        \
        -Wunreachable-code \

#        \
#        -Wstrict-prototypes \
#        -Wswitch-default -Wswitch-enum \

# Even Stricter Warnings
CFLAGS_WARNINGS_STRICT += -Wmissing-prototypes -Wmissing-declarations -Wswitch-default -Wswitch-enum


#       too strict?
#       -Wstrict-overflow=5 \
#       -Wconversion \


#
#        $(CFLAGS_WARNINGS_EXTRA) \
#        -fanalyzer -fsanitize=address,undefined \
        # -fanalyzer -fsanitize=address,undefined # -Werror

#CFLAGS_WARNINGS += -Werror

#   ifdef option-fail-on-warnings
#   CFLAGS_WARNINGS += -Werror
#   endif


# TODO: REMOVE
# For now, neutralize -Wpedantic with -Wno-pedantic as Konpu start using
# C23 constructions which are understood by older GCC/CLANG in earlier C.
# Konpu will switch to C23 and then this can be removed.
#CFLAGS_WARNINGS += -Wno-pedantic




# TODO: -fsanitize, also needed in LDFLAGS?


#-------------------------------------------------------------------------------

CDEFS = 
#CDEFS = -DKONPU_PLATFORM_SDL2 -DKONPU_PLATFORM_POSIX $(CDEFS_EXTRA)




CFLAGS += $(CFLAGS_WARNINGS) $(CFLAGS_DEBUG) $(CFLAGS_OPTIMIZE) $(CFLAGS_ARCH)
#CFLAGS  = -std=c11 $(COPTIMIZE) $(CDEBUG) $(shell sdl2-config --cflags) -fdiagnostics-color $(CDEFS) $(CFLAGS_WARNINGS) $(CFLAGS_ARCH) $(CFLAGS_EXTRA)
# CFLAGS  = -Os -s -DNDEBUG -std=c11 $(shell sdl2-config --cflags) -fdiagnostics-color $(CDEFS) $(CFLAGS_WARNINGS) $(CFLAGS_ARCH) $(CFLAGS_EXTRA) -flto


LDFLAGS += -rdynamic -ldl -lm
#LDFLAGS = -rdynamic -ldl -lm $(shell sdl2-config --libs) $(LDFLAGS_EXTRA)
# TODO: should -flto been added also to LDFLAGS ?
# TODO: add -s to LDFLAGS when in release mode?

#-------------------------------------------------------------------------------
# Common Special Targets
#-------------------------------------------------------------------------------

# Use make print-<VARIABLE> to print out a variable from this Makefile
.PHONY: print-%
print-%:
	@echo '$(subst ','\'',$*=$($*))'
