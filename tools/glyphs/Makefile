-include $(KONPU_HOME)/tools/dev/makefiles/common.mk

#===============================================================================

.PHONY: all tools clean cleanall
all: tools

clean: #nothing to clean
	@echo "Try `make cleanall` to remove binaries."

cleanall:
	rm -f bin/*

#-------------------------------------------------------------------------------
# Tools in src/ are single .c file and re-use the same few headers
#-------------------------------------------------------------------------------
CFLAGS_GLYPHS  = -std=c11 $(CFLAGS) -I$(KONPU_HOME)/src -I$(KONPU_HOME)/src/external
LDFLAGS_GLYPHS = $(LDFLAGS)

GLYPHS_SRCS  = $(wildcard src/*.c)
GLYPHS_DEPS  = $(wildcard src/*.h)
GLYPHS_DEPS += $(KONPU_HOME)/src/backend-common/options.h
GLYPHS_DEPS += $(KONPU_HOME)/src/backend-common/options.c
GLYPHS_DEPS += $(KONPU_HOME)/src/external/termbox2/termbox2.h
GLYPHS_BINS  = $(patsubst src/%.c, bin/%, $(GLYPHS_SRCS))

bin/%: src/%.c $(GLYPHS_DEPS)
	@printf "\033[33mcreating tool %s:\033[m " $@
	@mkdir -p $(@D)
	$(CC) $(CFLAGS_GLYPHS) -o $@ $< $(LDFLAGS_GLYPHS)

#-------------------------------------------------------------------------------
# Stand-alone scripts
#-------------------------------------------------------------------------------
STANDALONE_SCRIPTS = $(wildcard src-standalone/*.sh)

bin/%: src-standalone/%.sh
	@printf "\033[33mcopying script %s:\033[m " $<
	@mkdir -p $(@D)
	cp $< $@


tools: $(GLYPHS_BINS) $(STANDALONE_SCRIPTS)
