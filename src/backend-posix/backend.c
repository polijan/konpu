

#include "backend-common/backend.h"

#define  TB_OPT_ATTR_W   32 // for TrueColor support
#include "termbox2/termbox2.h"
#include <locale.h>

int BackendInitPlatform(void)  {
   setlocale(LC_ALL, ""); // We actually expect UTF-8
   int ret = tb_init();
   if (ret) {
      fprintf(stderr, "Cannot init Termbox2");
      return ret;
   }

   tb_hide_cursor();
   tb_set_input_mode(TB_INPUT_ALT | TB_INPUT_MOUSE);
   tb_set_output_mode(TB_OUTPUT_TRUECOLOR);
}

int BackendDropPlatform(void)
{ return tb_shutdown(); }


int BackendInitGraphics(C_ATTRIBUTE_MAYBE_UNUSED int z)  { (void)z; return 0; }
int BackendDropGraphics(void)  { return 0; }

int BackendInitMouse(void)  { return 0; }
int BackendDropMouse(void)  { return 0; }

#include "core84/arch.h"
void MouseUpdate(void)  { }

#include "core84/video_mode.h"
int VideoRender(void)  { return 0; }

#include "backend-common/options.c"

// TODO later: avoid libc???
#include "backend-libc/printer.c"

#include "backend-libc/main.c"
int main(int argc, char *argv[])
{ return BackendMain(argc, argv); }
