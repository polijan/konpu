
# disable implicit rules and builtin variables, I prefer to be explicit.
MAKEFLAGS += -rR

.PHONY: all fonts clean cleanall print-%

all: fonts

clean: # nothing to clean
	@echo "Try 'make cleanall' to remove fonts."

cleanall:
	rm -f fonts.out/*

print-%: # Use make print-<VARIABLE> to print out a variable from this Makefile
	@echo '$(subst ','\'',$*=$($*))'


#-------------------------------------------------------------------------------
# FONTS
#-------------------------------------------------------------------------------

FONTS_IN    = $(wildcard fonts.in/*.in)
FONTS_SELO  = $(patsubst fonts.in/%.in, fonts.out/%.selo,  $(FONTS_IN))
FONTS_FLF   = $(patsubst fonts.in/%.in, fonts.out/%.flf,   $(FONTS_IN))
FONTS_KBITX = $(patsubst fonts.in/%.in, fonts.out/%.kbitx, $(FONTS_IN))
FONTS_OUT   = $(FONTS_SELO) $(FONTS_KBITX) $(FONTS_FLF)

fonts.out/%.kbitx: fonts.out/%.selo bin/selo-to-kbitx
	@printf "\033[33mgenerating kbitx font '%s':\033[m " $@
	@mkdir -p $(@D)
	selo-to-kbitx $< > $@

fonts.out/%.flf: fonts.out/%.selo bin/selo-to-flf
	@printf "\033[33mgenerating flf font '%s':\033[m " $@
	@mkdir -p $(@D)
	selo-to-flf $< > $@

fonts.out/%.selo: fonts.in/%.in
	@printf "\033[33mgenerating selo font '%s':\033[m " $@
	@mkdir -p $(@D)
	dev-cpp $< > $@

#-------------------------------------------------------------------------------

fonts: $(FONTS_OUT)
