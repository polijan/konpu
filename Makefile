CC = gcc
CDEFS = -DKONPU_PLATFORM_SDL2 -DKONPU_PLATFORM_POSIX $(CDEFS_EXTRA)
CWARN = -Wall -Wextra -pedantic -Wformat=2 -Wno-unused-function # -fanalyzer # -Werror $(CWARN_EXTRA)
ARCHFLAGS = $(ARCHFLAGS_EXTRA)
CFLAGS  = -g -std=c11 -O2 $(shell sdl2-config --cflags) -fdiagnostics-color $(CDEFS) $(CWARN) $(ARCHFLAGS) $(CFLAGS_EXTRA) # -flto
LDFLAGS = -lm $(shell sdl2-config --libs) $(LDFLAGS_EXTRA)

#===============================================================================

# disable implicit rules and builtin variables, I prefer to be explicit.
MAKEFLAGS += -rR

.PHONY: all konpu test tests clean cleanall print-%

all: konpu tests

clean:
	rm -f obj/* tests/bin/*

cleanall: clean
	rm -f bin/konpu include/konpu.h lib/libkonpu.*

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

konpu: include/konpu.h lib/libkonpu.a lib/libkonpu.so bin/konpu

-include $(KONPU_DEPS)

include/konpu.h: src/mk_header.sh $(KONPU_HEADERS)
	@printf "\033[32mcreating konpu.h header:\033[m "
	@mkdir -p $(@D)
	$<  >  $@

lib/libkonpu.a: $(KONPU_OBJS)
	@printf "\033[32mcreating static lib:\033[m "
	@mkdir -p $(@D)
	ar rcs $@ $^

lib/libkonpu.so: $(KONPU_OBJS)
	@printf "\033[32mcreating shared lib:\033[m "
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $^ $(LDFLAGS)

bin/konpu: src/main/start.c $(KONPU_OBJS) src/platform.h src/config.h
	@printf "\033[32mcreating konpu executable:\033[m "
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $< $(KONPU_OBJS) $(LDFLAGS)

obj/%.o: src/%.c
	@printf "\033[34mcompiling %s:\033[m " $<
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -MMD -c $< -o $@

#-------------------------------------------------------------------------------
# tests
#
# target test  => compile and run the tests
# target tests => compile the tests
#-------------------------------------------------------------------------------
TESTS_SRCS = $(wildcard tests/*.c)
TESTS_BINS = $(patsubst tests/%.c, tests/bin/%, $(TESTS_SRCS))

test: tests
	@run-tests

tests: $(TESTS_BINS)

tests/bin/%: tests/%.c lib/libkonpu.a include/konpu.h tests/test.h
	@printf "\033[34mcompiling %s:\033[m " $<
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -Iinclude -o $@ $< lib/libkonpu.a $(LDFLAGS)
