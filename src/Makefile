CC = gcc
CDEFS = -DKONPU_PLATFORM_SDL2 -DKONPU_PLATFORM_POSIX $(EXTRA_CDEFS)
CWARN = -Wall -Wextra -pedantic -fanalyzer $(EXTRA_CWARN)
CFLAGS = -std=c99 $(shell sdl2-config --cflags) -fdiagnostics-color -O2 $(CDEFS) $(CWARN) $(EXTRA_CFLAGS)
ARCHFLAGS = $(EXTRA_ARCHFLAGS)

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
DEP = $(SRC:.c=.d)

.PHONY: all clean cleanall

all: $(OBJ)

-include $(DEP)

.c.o:
	$(CC) $(CFLAGS) $(ARCHFLAGS) -MMD -c $<


clean:
	rm -f $(OBJ) $(DEP)

cleanall:
	rm -f *.o *.d *.gch $(EXE)
