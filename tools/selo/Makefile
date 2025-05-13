
# disable implicit rules and builtin variables, I prefer to be explicit.
MAKEFLAGS += -rR

.PHONY: all tools fonts clean cleanall print-%

all: tools fonts

clean: #nothing to clean
#	@echo "Try `make cleanall` to remove all."

cleanall:
	rm -f bin/*
	rm -f fonts.out/*

print-%: # Use make print-<VARIABLE> to print out a variable from this Makefile
	@echo '$(subst ','\'',$*=$($*))'



#-------------------------------------------------------------------------------
# *.SELO FONTS
#-------------------------------------------------------------------------------

FONTS_IN    = $(wildcard fonts.in/*.in)
FONTS_SELO  = $(patsubst fonts.in/%.in, fonts.out/%.selo,  $(FONTS_IN))
FONTS_FLF   = $(patsubst fonts.in/%.in, fonts.out/%.flf,   $(FONTS_IN))
FONTS_KBITX = $(patsubst fonts.in/%.in, fonts.out/%.kbitx, $(FONTS_IN))
FONTS_OUT   = $(FONTS_SELO) $(FONTS_KBITX) $(FONTS_FLF)

fonts.out/%.kbitx: fonts.out/%.selo bin/selo-to-kbitx
	@printf "\033[33mgenerating kbitx file %s:\033[m " $@
	@mkdir -p $(@D)
	selo-to-kbitx $< > $@

fonts.out/%.flf: fonts.out/%.selo bin/selo-to-flf
	@printf "\033[33mgenerating flf font %s:\033[m " $@
	@mkdir -p $(@D)
	selo-to-flf $< > $@

fonts.out/%.selo: fonts.in/%.in
	@printf "\033[33mgenerating selo font %s:\033[m " $@
	@mkdir -p $(@D)
	dev-cpp $< > $@

#-------------------------------------------------------------------------------
# Scripts
#-------------------------------------------------------------------------------
SCRIPTS_SH = $(wildcard src/*.sh)
SCRIPTS_BASH = $(wildcard src/*.bash)
SCRIPTS = $(patsubst src/%.sh, bin/%, $(SCRIPTS_SH)) $(patsubst src/%.bash, bin/%, $(SCRIPTS_BASH))

bin/%: src/%.sh
	@printf "\033[33mcopying script %s:\033[m " $<
	@mkdir -p $(@D)
	cp $< $@

bin/%: src/%.bash
	@printf "\033[33mcopying script %s:\033[m " $<
	@mkdir -p $(@D)
	cp $< $@

#-------------------------------------------------------------------------------

tools: $(SCRIPTS)

fonts: $(FONTS_OUT)
