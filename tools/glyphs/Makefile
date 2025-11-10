CC = gcc

CWARN = -Wall -Wextra -pedantic -Wformat=2                           \
        -Wundef -Wcast-align                                         \
        -Wmissing-format-attribute -Wnested-externs -Wpointer-arith  \
        -Wreturn-type -Wswitch -Wcast-qual -Wwrite-strings           \
        -Wformat=2 -Wpointer-sign -Wmissing-noreturn                 \
        -Wvla -Wshadow

CFLAGS  = -std=c11 -fdiagnostics-color $(CWARN)
LDFLAGS =

#===============================================================================

# disable implicit rules and builtin variables, I prefer to be explicit.
MAKEFLAGS += -rR

.PHONY: all tools clean cleanall print-%

all: tools

clean: #nothing to clean
	@echo "Try `make cleanall` to remove binaries."

cleanall:
	rm -f bin/*

print-%: # Use make print-<VARIABLE> to print out a variable from this Makefile
	@echo '$(subst ','\'',$*=$($*))'


#-------------------------------------------------------------------------------
# Tools in src/ are single .c file and re-use the same few headers
#-------------------------------------------------------------------------------
GLYPHS_SRCS = $(wildcard src/*.c)
GLYPHS_DEPS = $(wildcard src/*.h) $(KONPU_HOME)/src/options.h $(KONPU_HOME)/src/options.c external/termbox2/termbox2.h
GLYPHS_BINS = $(patsubst src/%.c, bin/%, $(GLYPHS_SRCS))
GLYPHS_CFLAGS = $(CFLAGS) -O2
GLYPHS_LDFLAGS = $(LDFLAGS)

bin/%: src/%.c $(GLYPHS_DEPS)
	@printf "\033[33mcreating tool %s:\033[m " $@
	@mkdir -p $(@D)
	$(CC) $(GLYPHS_CFLAGS) -o $@ $< $(GLYPHS_LDFLAGS)

#-------------------------------------------------------------------------------
# Stand-alone scripts
#-------------------------------------------------------------------------------
STANDALONE_SCRIPTS = $(wildcard src-standalone/*.sh)

bin/%: src-standalone/%.sh
	@printf "\033[33mcopying script %s:\033[m " $<
	@mkdir -p $(@D)
	cp $< $@


tools: $(GLYPHS_BINS) $(STANDALONE_SCRIPTS)
