#include "posix.h"

// TODO: needed for termbox???
// #include "locale.h"

// Include termbox implementation
#define  TB_IMPL
#pragma  GCC diagnostic push
#pragma  GCC diagnostic ignored "-Wsuggest-attribute=format"
#pragma  GCC diagnostic ignored "-Wswitch-default"
#define  TB_OPT_ATTR_W   32 // for TrueColor support
#include "termbox2/termbox2.h"
#pragma  GCC diagnostic pop
