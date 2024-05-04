CC = gcc
CDEFS = -DKONPU_PLATFORM_SDL2 -DKONPU_PLATFORM_POSIX $(CDEFS_EXTRA)
CWARN = -Wall -Wextra -pedantic -Wformat=2 -Wno-unused-function # -fanalyzer # -Werror $(CWARN_EXTRA)
ARCHFLAGS = $(ARCHFLAGS_EXTRA)
CFLAGS  = -std=c11 -O2 $(shell sdl2-config --cflags) -fdiagnostics-color $(CDEFS) $(CWARN) $(ARCHFLAGS) $(CFLAGS_EXTRA) # -flto
LDFLAGS = -lm $(shell sdl2-config --libs) $(LDFLAGS_EXTRA)

#===============================================================================

# disable implicit rules and builtin variables, I prefer to be explicit.
MAKEFLAGS += -rR

.PHONY: all konpu test tests clean cleanall print-%

all: konpu tests

test: tests # just a synonym

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
	@mkdir -p $(@D)
	$<  >  $@

lib/libkonpu.a: $(KONPU_OBJS)
	@mkdir -p $(@D)
	ar rcs $@ $^

lib/libkonpu.so: $(KONPU_OBJS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(LDFLAGS) -fPIC -shared -o $@ $^

bin/konpu: $(KONPU_OBJS)
	@mkdir -p $(@D)
	printf 'extern int KonpuMain(int c, char *v[]);\nint main(int c, char *v[]){return KonpuMain(c,v);}\n' > obj/generated_main.c
	$(CC) -O2 -c obj/generated_main.c -o obj/generated_main.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ obj/generated_main.o

obj/%.o: src/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -MMD -c $< -o $@

#-------------------------------------------------------------------------------
# tests
#-------------------------------------------------------------------------------
TESTS_SRCS = $(wildcard tests/*.c)
TESTS_BINS = $(patsubst tests/%.c, tests/bin/%, $(TESTS_SRCS))

tests: $(TESTS_BINS)

tests/bin/%: tests/%.c lib/libkonpu.a include/konpu.h tests/test.h
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(LDFLAGS) -Iinclude -o $@ $< lib/libkonpu.a
