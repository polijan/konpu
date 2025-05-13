# Compilers:
# GCC is the most tested compiler for ilo Konpu,
# CLANG should work too.
# Other compilers, not tested.
CC = gcc

# Warnings:
# the classics,     ie: -Wall -Wextra -pedantic
# + -fanalyzer,     see: https://gcc.gnu.org/onlinedocs/gcc/Static-Analyzer-Options.html
# + extra warnings, see: https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
CWARN = -Wall -Wextra -pedantic  \
        \
        -fanalyzer \
        \
        -Wundef -Wmissing-prototypes -Wmissing-declarations -Wcast-align \
        -Wmissing-format-attribute -Wnested-externs -Wpointer-arith      \
        -Wreturn-type -Wswitch -Wshadow -Wcast-qual -Wwrite-strings      \
        -Wformat=2 -Wpointer-sign -Wmissing-noreturn                     \
		-Wvla-larger-than=0                                              \
        \
        -Wno-cast-align -Wno-unused-function  \
        \
#        $(CWARN_EXTRA) \
#        -fanalyzer -fsanitize=address,undefined \
        # -fanalyzer -fsanitize=address,undefined # -Werror

#CWARN += -Werror

#   ifdef option-fail-on-warnings
#   CWARN += -Werror
#   endif



# TODO: -fsanitize, also needed in LDFLAGS?



# Debug info to include:
# see: https://gcc.gnu.org/onlinedocs/gcc/Debugging-Options.html#Debugging-Options
# -g     produces debug info in the OS' native format.
# -ggdb  produces debug info specifically intended for gdb.
# -ggdb3 produces extra info, for example including macro definitions.
#
# For a release build do: CFLAGS_DEBUG = -DNDEBUG
CDEBUG = #-ggdb3 # -Werror #<-- don't build with warnings

# Optimization:
# + -O2 or `-O3 -fexpensive-optimizations` or ...
# + -Wsuggest* warnings, search for -Wsuggest in: https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
COPTIMIZE = -O2
#COPTIMIZE = -O2 \
#        \
#        -Wsuggest-attribute=pure -Wsuggest-attribute=const       \
#        -Wsuggest-attribute=noreturn -Wsuggest-attribute=malloc  \
#        -Wsuggest-attribute=format -Wsuggest-attribute=cold      \
##        -Wsuggest-attribute=returns_nonnull                      \

CDEFS = -DKONPU_PLATFORM_SDL2 -DKONPU_PLATFORM_POSIX $(CDEFS_EXTRA)

ARCHFLAGS = $(ARCHFLAGS_EXTRA)



CFLAGS  = -std=c11 $(COPTIMIZE) $(CDEBUG) $(shell sdl2-config --cflags) -fdiagnostics-color $(CDEFS) $(CWARN) $(ARCHFLAGS) $(CFLAGS_EXTRA) -flto
# CFLAGS  = -Os -s -DNDEBUG -std=c11 $(shell sdl2-config --cflags) -fdiagnostics-color $(CDEFS) $(CWARN) $(ARCHFLAGS) $(CFLAGS_EXTRA) -flto
LDFLAGS = -rdynamic -ldl -lm $(shell sdl2-config --libs) $(LDFLAGS_EXTRA)

#===============================================================================

# disable implicit rules and builtin variables, I prefer to be explicit.
MAKEFLAGS += -rR

.PHONY: all konpu test tests apps cppcheck clean cleanall print-%

all: konpu tests apps

clean:
	rm -f obj/* tests/bin/*

cleanall: clean
	rm -f bin/konpu include/konpu.h lib/libkonpu.* apps/lib/*

print-%: # Use make print-<VARIABLE> to print out a variable from this Makefile
	@echo '$(subst ','\'',$*=$($*))'


#-------------------------------------------------------------------------------
# konpu:
# - generate konpu.h in include/
# - build static library in lib/ (and objects in obj)
# - generate a main calling KonpuMain and build konpu binary in bin/
#-------------------------------------------------------------------------------
KONPU_SRCS    = $(wildcard src/*.c)
KONPU_HEADERS = $(wildcard src/*.h)
KONPU_OBJS    = $(patsubst src/%.c, obj/%.o, $(KONPU_SRCS))
KONPU_DEPS    = $(patsubst src/%.c, obj/%.d, $(KONPU_SRCS))

konpu: include/konpu.h lib/libkonpu.a bin/konpu #lib/libkonpu.so

-include $(KONPU_DEPS)

include/konpu.h: src/mk_header.sh $(KONPU_HEADERS)
	@printf "\033[32mcreating konpu.h header:\033[m "
	@mkdir -p $(@D)
	$<  >  $@

# static library is used for tests
lib/libkonpu.a: $(KONPU_OBJS)
	@printf "\033[32mcreating static lib:\033[m "
	@mkdir -p $(@D)
	ar rcs $@ $^

#lib/libkonpu.so: $(KONPU_OBJS)
#	@printf "\033[32mcreating shared lib:\033[m "
#	@mkdir -p $(@D)
#	$(CC) $(CFLAGS) -fPIC -shared -o $@ $^ $(LDFLAGS)

#bin/konpu: src/main/start.c $(KONPU_OBJS) src/platform.h src/config.h
#	@printf "\033[32mcreating konpu executable:\033[m "
#	@mkdir -p $(@D)
#	$(CC) $(CFLAGS) -o $@ $< $(KONPU_OBJS) $(LDFLAGS)
bin/konpu: $(KONPU_OBJS) src/platform.h src/config.h
	@printf "\033[32mcreating konpu executable:\033[m "
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $(KONPU_OBJS) $(LDFLAGS)

obj/%.o: src/%.c
	@printf "\033[34mcompiling %s:\033[m " $<
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -MMD -c $< -o $@

#-------------------------------------------------------------------------------
# tests:
#
# target test  => compile and run the tests
# target tests => compile the tests
#-------------------------------------------------------------------------------
TESTS_SRCS = $(wildcard tests/*.c)
TESTS_BINS = $(patsubst tests/%.c, tests/bin/%, $(TESTS_SRCS))

test: tests
	@dev-run-tests

tests: $(TESTS_BINS)

tests/bin/%: tests/%.c lib/libkonpu.a include/konpu.h tests/test.h
	@printf "\033[34mcompiling %s:\033[m " $<
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -Iinclude -o $@ $< lib/libkonpu.a $(LDFLAGS)

#-------------------------------------------------------------------------------
# apps: single .c file konpu apps compiled into their own shared lib
#-------------------------------------------------------------------------------
APPS_SRCS = $(wildcard apps/src/*.c)
APPS_LIBS = $(patsubst apps/src/%.c, apps/lib/%.so, $(APPS_SRCS))

apps: $(APPS_LIBS)

apps/lib/%.so: apps/src/%.c include/konpu.h
	@printf "\033[32mcreating application (shared lib %s):\033[m " $@
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -nostdlib -Iinclude -fPIC -shared -o $@ $<
#-nostdlib? -> to make it static


#-------------------------------------------------------------------------------
# konpu:
# - generate konpu.h in include/
# - build static library in lib/ (and objects in obj)
# - generate a main calling KonpuMain and build konpu binary in bin/
#-------------------------------------------------------------------------------

cppcheck: include/konpu.h
	@printf "\033[33mcppcheck src\033[m\n"
	cppcheck -i tmp  --force src
	@printf "\033[33mcppcheck tests\033[m\n"
	cppcheck -i tmp  -I include  tests
	@printf "\033[33mcppcheck apps\033[m\n"
	cppcheck -i tmp  -I include  apps/src
	@printf "\033[33mcppcheck tools\033[m\n"
	cppcheck -i tmp  tools

