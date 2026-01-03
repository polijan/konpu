-include $(KONPU_HOME)/tools/dev/makefiles/common.mk

#===============================================================================

.PHONY: all core84 tests backends clean cleanall run test cppcheck

all: core84 tests backends apps


clean:
	@printf '\033[1;33mRemoving core objects     :\033[m '
	rm -fR obj/core*
	@printf '\033[1;33mRemoving backend objects  :\033[m '
	rm -fR obj/backend*
	@printf '\033[1;33mRemoving core tests       :\033[m '
	rm -f  bin/tests/*

cleanall: clean
	@printf '\033[1;33mRemoving konpu dev headers:\033[m '
	rm -f  include/konpu*.h
	@printf '\033[1;33mRemoving konpu executables:\033[m '
	rm -f  bin/konpu*
	@printf '\033[1;33mRemoving apps "libraries" :\033[m '
	rm -f  apps/lib/*


#-------------------------------------------------------------------------------
# Cores:
# - Generate <konpuXX.h> in include/
# - Generate core's *.o objects in obj/coreXX
#-------------------------------------------------------------------------------
CFLAGS_CORE    = -std=c11 -I$(KONPU_HOME)/src $(CFLAGS) $(CFLAGS_WARNINGS_STRICT)

CORE84_SRCS    = $(wildcard src/core84/*.c)
CORE84_HEADERS = $(wildcard src/core84/*.h) $(wildcard src/core84/glyph_constants/*.h)
CORE84_OBJS    = $(patsubst src/core84/%.c, obj/core84/%.o, $(CORE84_SRCS))
CORE84_DEPS    = $(patsubst src/core84/%.c, obj/core84/%.d, $(CORE84_SRCS))

core84: include/konpu.h $(CORE84_OBJS)

-include $(CORE84_DEPS)

# Generating <konpu.h>
include/konpu.h: src/core84/mk_header.sh $(CORE84_HEADERS)
	@printf '\033[1;32mGenerating <konpu.h> header:\033[m '
	@mkdir -p $(@D)
	$<  >  $@

# Compiling core (use -MMD to auto create .d dependencies files)
obj/core84/%.o: src/core84/%.c
	@printf '\033[34mCompiling %s:\033[m ' $<
	@mkdir -p $(@D)
	$(CC) $(CFLAGS_CORE) -MMD -c $< -o $@

#-------------------------------------------------------------------------------
# Tests executables
#-------------------------------------------------------------------------------

CFLAGS_TESTS = -std=c11 -Iinclude $(CFLAGS) $(CFLAGS_WARNINGS_STRICT)

TESTS_SRCS   = $(wildcard src/tests/*.c)
TESTS_BINS   = $(patsubst src/tests/%.c, bin/tests/%, $(TESTS_SRCS))

tests: $(TESTS_BINS)

# Every test is a .c file that compiles to an executable of its own (linked with
# all the core *.o files, and header-wise only dependant on konpu.h and test.h)
bin/tests/%: src/tests/%.c $(CORE84_OBJS) include/konpu.h src/tests/test.h
	@printf "\033[32mCreating Test %s:\033[m " $<
	@mkdir -p $(@D)
	$(CC) $(CFLAGS_TESTS) -o $@ $< $(CORE84_OBJS) -lm


################################################################################
# Backends
################################################################################
CFLAGS_BACKEND  = -I$(KONPU_HOME)/src -I$(KONPU_HOME)/src/external $(CFLAGS) $(CFLAGS_WARNINGS_STRICT)
CFLAGS_BACKEND += -DKONPU_PLATFORM_POSIX  # <-- temporary, remove it or check platform
LDFLAGS_BACKEND = -rdynamic

backends: bin/konpu-sdl2 bin/konpu-sokol bin/konpu-libc


#-------------------------------------------------------------------------------
# Backend: konpu-sdl2
#-------------------------------------------------------------------------------
CFLAGS_BACKEND_SDL2  = $(CFLAGS_BACKEND)  $(shell sdl2-config --cflags) -DKONPU_PLATFORM_SDL2
LDFLAGS_BACKEND_SDL2 = $(LDFLAGS_BACKEND) $(shell sdl2-config --libs) #-fvisibility=hidden

BACKEND_SDL2_SRCS = $(wildcard src/backend-sdl/*.c)
BACKEND_SDL2_OBJS = $(patsubst src/backend-sdl/%.c, obj/backend-sdl2/%.o, $(BACKEND_SDL2_SRCS))
BACKEND_SDL2_DEPS = $(patsubst src/backend-sdl/%.c, obj/backend-sdl2/%.d, $(BACKEND_SDL2_SRCS))

-include $(BACKEND_SDL2_DEPS)

bin/konpu-sdl2: $(BACKEND_SDL2_OBJS) $(CORE84_OBJS)
	@printf "\033[1;32mcreating konpu-sdl2 executable:\033[m "
	@mkdir -p $(@D)
	$(CC) $(CFLAGS_BACKEND_SDL2) -o $@ $^ $(LDFLAGS_BACKEND_SDL2)

obj/backend-sdl2/%.o: src/backend-sdl/%.c
	@printf "\033[34mcompiling %s:\033[m " $<
	@mkdir -p $(@D)
	$(CC) $(CFLAGS_BACKEND_SDL2) -MMD -c $< -o $@

#-------------------------------------------------------------------------------
# `sokol_backend`: Graphical backend for sokol_apps.h
#-------------------------------------------------------------------------------

# temporary
sokol_backend = GLES3

CFLAGS_BACKEND_SOKOL  = $(CFLAGS_BACKEND) -DKONPU_PLATFORM_SOKOL -DKONPU_PLATFORM_POSIX
LDFLAGS_BACKEND_SOKOL = $(LDFLAGS_BACKEND)

ifeq ($(platform), linux)
    # sokol_apps.h gives those instructions for Linux:
    #   - all backends: X11, Xi, Xcursor, dl, pthread, m
    #   - with SOKOL_GLCORE: GL
    #   - with SOKOL_GLES3: GLESv2
    #   - with SOKOL_WGPU: a WebGPU implementation library (tested with webgpu_dawn)
    #   - with EGL: EGL
    # Also instruct to use the -pthread compiler and linker option
    #
	CFLAGS_BACKEND_SOKOL  += -pthread
	LDFLAGS_BACKEND_SOKOL += -lX11 -lXi -lXcursor -dl -lm -lpthread
	ifeq ($(sokol_backend), GLCORE)
		CFLAGS_BACKEND_SOKOL  += -DSOKOL_GLCORE
		LDFLAGS_BACKEND_SOKOL += -lGL
	else ifeq ($(sokol_backend), GLES3)
		CFLAGS_BACKEND_SOKOL  += -DSOKOL_GLES3
		LDFLAGS_BACKEND_SOKOL += -lGLESv2 -lEGL
	else ifeq ($(sokol_backend), EGL)
		CFLAGS_BACKEND_SOKOL  += -DSOKOL_GLES3
		LDFLAGS_BACKEND_SOKOL += -lGLESv2 -lEGL
	endif
endif
# ^TODO: support more OS

BACKEND_SOKOL_SRCS = $(wildcard src/backend-sokol/*.c)
BACKEND_SOKOL_OBJS = $(patsubst src/backend-sokol/%.c, obj/backend-sokol/%.o, $(BACKEND_SOKOL_SRCS))
BACKEND_SOKOL_DEPS = $(patsubst src/backend-sokol/%.c, obj/backend-sokol/%.d, $(BACKEND_SOKOL_SRCS))

-include $(BACKEND_SOKOL_DEPS)

bin/konpu-sokol: $(BACKEND_SOKOL_OBJS) $(CORE84_OBJS)
	@printf "\033[1;32mcreating konpu-sokol executable:\033[m "
	@mkdir -p $(@D)
	$(CC) $(CFLAGS_BACKEND_SOKOL) -o $@ $(BACKEND_SOKOL_OBJS) $(CORE84_OBJS) $(LDFLAGS_BACKEND_SOKOL)

obj/backend-sokol/%.o: src/backend-sokol/%.c
	@printf "\033[34mcompiling %s:\033[m " $<
	@mkdir -p $(@D)
	$(CC) $(CFLAGS_BACKEND_SOKOL) -MMD -c $< -o $@

#-------------------------------------------------------------------------------
# Backend: konpu-libc
#-------------------------------------------------------------------------------
CFLAGS_BACKEND_LIBC  = -std=c11 $(CFLAGS_BACKEND) -DKONPU_PLATFORM_LIBC
LDFLAGS_BACKEND_LIBC = $(LDFLAGS_BACKEND) -lm -lpthread
#^-- -lm needed for <math.h>, -lpthread needed for <thread.h>

BACKEND_LIBC_SRCS = $(wildcard src/backend-libc/*.c)
BACKEND_LIBC_OBJS = $(patsubst src/backend-libc/%.c, obj/backend-libc/%.o, $(BACKEND_LIBC_SRCS))
BACKEND_LIBC_DEPS = $(patsubst src/backend-libc/%.c, obj/backend-libc/%.d, $(BACKEND_LIBC_SRCS))

-include $(BACKEND_LIBC_DEPS)

bin/konpu-libc: $(BACKEND_LIBC_OBJS) $(CORE84_OBJS)
	@printf "\033[1;32mcreating konpu-libc executable:\033[m "
	@mkdir -p $(@D)
	$(CC) $(CFLAGS_BACKEND_LIBC) -o $@ $^ $(LDFLAGS_BACKEND_LIBC)

obj/backend-libc/%.o: src/backend-libc/%.c
	@printf "\033[34mcompiling %s:\033[m " $<
	@mkdir -p $(@D)
	$(CC) $(CFLAGS_BACKEND_LIBC) -MMD -c $< -o $@

#-------------------------------------------------------------------------------
# Backend: konpu-posix
#-------------------------------------------------------------------------------
CFLAGS_BACKEND_POSIX  = $(CFLAGS_BACKEND) -DKONPU_PLATFORM_POSIX
LDFLAGS_BACKEND_POSIX = $(LDFLAGS_BACKEND) -ldl -lm -lpthread

BACKEND_POSIX_SRCS = $(wildcard src/backend-posix/*.c)
BACKEND_POSIX_OBJS = $(patsubst src/backend-posix/%.c, obj/backend-posix/%.o, $(BACKEND_POSIX_SRCS))
BACKEND_POSIX_DEPS = $(patsubst src/backend-posix/%.c, obj/backend-posix/%.d, $(BACKEND_POSIX_SRCS))

-include $(BACKEND_POSIX_DEPS)

bin/konpu-posix: $(BACKEND_POSIX_OBJS) $(CORE84_OBJS)
	@printf "\033[1;32mcreating konpu-posix executable:\033[m "
	@mkdir -p $(@D)
	$(CC) $(CFLAGS_BACKEND_POSIX) -o $@ $^ $(LDFLAGS_BACKEND_POSIX)

obj/backend-posix/%.o: src/backend-posix/%.c
	@printf "\033[34mcompiling %s:\033[m " $<
	@mkdir -p $(@D)
	$(CC) $(CFLAGS_BACKEND_POSIX) -MMD -c $< -o $@


#-------------------------------------------------------------------------------
# Apps: single .c file konpu apps compiled into their own shared lib
#-------------------------------------------------------------------------------
CFLAGS_APPS  = -std=c11 -Iinclude $(CFLAGS)
LDFLAGS_APPS = -nostdlib -fPIC -shared
# -ffreestanding ?
#-nostdlib? -> to make it static

APPS_SRCS = $(wildcard apps/src/*.c)
APPS_LIBS = $(patsubst apps/src/%.c, apps/lib/%.so, $(APPS_SRCS))

apps: $(APPS_LIBS)

apps/lib/%.so: apps/src/%.c include/konpu.h
	@printf "\033[32mcreating application (shared lib %s):\033[m " $@
	@mkdir -p $(@D)
	$(CC) $(CFLAGS_APPS) -o $@ $< $(LDFLAGS_APPS)


#-------------------------------------------------------------------------------
# Special targets
#-------------------------------------------------------------------------------

# Run Konpu
run: bin/konpu-sdl2
	@konpu-sdl2 --stdout

# Run the tests
test: tests
	@printf "\033[1;33mRunning Core's Tests:\033[m\n"
	@dev-run-tests

# Static C source code check
cppcheck: include/konpu.h
	@printf "\033[33mcppcheck src\033[m\n"
	cppcheck -i tmp  --force src
	@printf "\033[33mcppcheck tests\033[m\n"
	cppcheck -i tmp  -I include  tests
	@printf "\033[33mcppcheck apps\033[m\n"
	cppcheck -i tmp  -I include  apps/src
	@printf "\033[33mcppcheck tools\033[m\n"
	cppcheck -i tmp  tools




#all: konpu tests apps
#
#clean: clean_konpu clean_tests clean_apps
#
#cleanall: cleanall_konpu cleanall_tests cleanall_apps


#clean: clean_konpu clean_apps
#	rm -f obj/* tests/bin/*
#
#cleanall: clean
#	rm -f bin/konpu include/konpu.h lib/libkonpu.* apps/lib/*


#-------------------------------------------------------------------------------
# tools: simply invoke tools' Makefile via dev-make-tools
#-------------------------------------------------------------------------------

tools:
	@dev-make-tools

clean_tools:
	@dev-make-tools clean

cleanall_tools:
	@dev-make-tools cleanall
