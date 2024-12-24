CC = gcc
CWARN  = -Wall -Wextra -pedantic
CFLAGS = -std=c99 -Os $(CWARN)

#===============================================================================

# disable implicit rules and builtin variables, I prefer to be explicit.
MAKEFLAGS += -rR

print-%: # Use make print-<VARIABLE> to print out a variable from this Makefile
	@echo '$(subst ','\'',$*=$($*))'

#===============================================================================

.PHONY: all clean cleanall print-%

all: bin/endian

clean: #nothing to clean

cleanall:
	rm -f bin/endian

bin/endian: endian.c
	@printf "\033[33mcreating tool %s:\033[m " $@
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $<
