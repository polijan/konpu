# Disable implicit rules and builtin variables, I prefer to be explicit.
MAKEFLAGS += -rR
# Try to compile faster by running in parallel
MAKEFLAGS += -j $(shell dev-nproc)

.PHONY: all fonts clean cleanall print-%

#-------------------------------------------------------------------------------

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

FONTS_IN     = $(wildcard fonts.in/selo*)
FONTS_GLYPHS = $(wildcard $(KONPU_HOME)/src/glyph_constants/glyph_*.h)

FONTS        = $(wildcard scripts/selo-generate-*)
FONTS_SELO   = $(patsubst scripts/selo-generate-%, fonts.out/%.selo,  $(FONTS))
FONTS_FLF    = $(patsubst scripts/selo-generate-%, fonts.out/%.flf,   $(FONTS))
FONTS_KBITX  = $(patsubst scripts/selo-generate-%, fonts.out/%.kbitx, $(FONTS))
FONTS_OUT    = $(FONTS_SELO) $(FONTS_KBITX) $(FONTS_FLF)

fonts.out/%.selo: scripts/selo-generate-% $(FONTS_IN) $(FONTS_GLYPHS)
	@printf "\033[33mgenerating selo font '%s':\033[m " $@
	@mkdir -p $(@D)
	$< > $@

fonts.out/%.kbitx: fonts.out/%.selo scripts/selo-to-kbitx
	@printf "\033[33mgenerating kbitx font '%s':\033[m " $@
	@mkdir -p $(@D)
	selo-to-kbitx $< > $@

fonts.out/%.flf: fonts.out/%.selo scripts/selo-to-flf
	@printf "\033[33mgenerating flf font '%s':\033[m " $@
	@mkdir -p $(@D)
	selo-to-flf $< > $@

#-------------------------------------------------------------------------------

fonts: $(FONTS_OUT)
