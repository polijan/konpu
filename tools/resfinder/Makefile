CC = gcc

CWARN  = -Wall -Wextra -pedantic  \
         \
         -Wundef -Wmissing-prototypes -Wmissing-declarations -Wcast-align \
         -Wmissing-format-attribute -Wnested-externs -Wpointer-arith      \
         -Wreturn-type -Wswitch -Wshadow -Wcast-qual -Wwrite-strings      \
         -Wformat=2 -Wpointer-sign -Wmissing-noreturn

CFLAGS = -std=c99 -O2 $(CWARN)

#===============================================================================

# disable implicit rules and builtin variables, I prefer to be explicit.
MAKEFLAGS += -rR

print-%: # Use make print-<VARIABLE> to print out a variable from this Makefile
	@echo '$(subst ','\'',$*=$($*))'

#===============================================================================

.PHONY: all clean cleanall print-%

all: bin/resfinder

clean: #nothing to clean

cleanall:
	rm -f bin/resfinder

bin/resfinder: src/resfinder.c
	@printf "\033[33mcreating tool %s:\033[m " $@
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $<
