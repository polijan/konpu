CC = gcc
CWARN = -Wall -Wextra -pedantic -Wformat=2 # -Wno-unused-function -fanalyzer $(CWARN_EXTRA)
CFLAGS  = -std=c99 -fdiagnostics-color $(CWARN) $(CFLAGS_EXTRA)
LDFLAGS = $(LDFLAGS_EXTRA)

#===============================================================================

# disable implicit rules and builtin variables, I prefer to be explicit.
MAKEFLAGS += -rR

.PHONY: all tools clean cleanall print-%

all: tools

clean: #nothing to clean

cleanall:
	rm -f bin/*

print-%: # Use make print-<VARIABLE> to print out a variable from this Makefile
	@echo '$(subst ','\'',$*=$($*))'


#-------------------------------------------------------------------------------
# Tools in src/ are single .c file and re-use the same few headers
#-------------------------------------------------------------------------------
COLORS_SRCS = $(wildcard src/*.c)
COLORS_DEPS = $(wildcard src/*.h) $(KONPU_HOME)/src/options.h $(KONPU_HOME)/src/options.c
COLORS_BINS = $(patsubst src/%.c, bin/%, $(COLORS_SRCS))
COLORS_CFLAGS = $(CFLAGS) -O2
COLORS_LDFLAGS = $(LDFLAGS) -lm

bin/%: src/%.c $(COLORS_DEPS)
	@printf "\033[33mcreating tool %s:\033[m " $@
	@mkdir -p $(@D)
	$(CC) $(COLORS_CFLAGS) -o $@ $< $(COLORS_LDFLAGS)


#-------------------------------------------------------------------------------
# Stand-alone programs (single file .c with no dependencies and scripts)
#-------------------------------------------------------------------------------
STANDALONE_SRCS = $(wildcard src-standalone/*.c)
STANDALONE_SCRIPTS = $(wildcard src-standalone/*.sh)
STANDALONE_BINS = $(patsubst src-standalone/%.c, bin/%, $(STANDALONE_SRCS)) $(patsubst src-standalone/%.sh, bin/%, $(STANDALONE_SCRIPTS))
STANDALONE_CFLAGS = $(CFLAGS) -O2
STANDALONE_LDFLAGS = $(LDFLAGS)

bin/%: src-standalone/%.c
	@printf "\033[33mcreating tool %s:\033[m " $@
	@mkdir -p $(@D)
	$(CC) $(STANDALONE_CFLAGS) -o $@ $< $(STANDALONE_LDFLAGS)

bin/%: src-standalone/%.sh
	@printf "\033[33mcopying script %s:\033[m " $<
	@mkdir -p $(@D)
	cp $< $@


#-------------------------------------------------------------------------------
# Programs using the k-means clustering method
#-------------------------------------------------------------------------------
KMEANS_SRCS = $(wildcard src-kmeans/*.c)
KMEANS_DEPS = $(COLORS_DEPS) external/kmeans/kmeans.c external/kmeans/kmeans.h
KMEANS_BINS = $(patsubst src-kmeans/%.c, bin/%, $(KMEANS_SRCS))

# Multithreading control (requires 'pthreads' [POSIX], remove for pure C)
# KMEANS_THR_MAX could be set up to the number of CPU cores for max perf.
KMEANS_MULTITHREAD_CFLAGS = -DKMEANS_THREADED -DKMEANS_THR_MAX=4 -DKMEANS_THR_THRESHOLD=250000
KMEANS_MULTITHREAD_LDFLAGS = -lpthread
# Try even more optimizations as those program are CPU-intensive
KMEANS_OPTIMIZATION = -DNDEBUG -O3 -fno-math-errno
KMEANS_CFLAGS = $(CFLAGS) $(KMEANS_OPTIMIZATION) $(KMEANS_MULTITHREAD_CFLAGS)
KMEANS_LDFLAGS = $(COLORS_LDFLAGS) $(KMEANS_MULTITHREAD_LDFLAGS)

bin/%: src-kmeans/%.c $(KMEANS_DEPS)
	@printf "\033[33mcreating tool %s:\033[m " $<
	@mkdir -p $(@D)
	$(CC) $(KMEANS_CFLAGS) -o $@ $< $(KMEANS_LDFLAGS)


#-------------------------------------------------------------------------------

tools: $(COLORS_BINS) $(STANDALONE_BINS) $(KMEANS_BINS)
