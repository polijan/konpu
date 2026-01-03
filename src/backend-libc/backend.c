#include "backend-common/backend.h"

int BackendInitPlatform(void)  { return 0; }
int BackendDropPlatform(void)  { return 0; }

int BackendInitGraphics(C_ATTRIBUTE_MAYBE_UNUSED int z)  { (void)z; return 0; }
int BackendDropGraphics(void)  { return 0; }

int BackendInitMouse(void)  { return 0; }
int BackendDropMouse(void)  { return 0; }


#include "core84/arch.h"
void MouseUpdate(void)  { }
#include "core84/keyboard.h"
void KeyboardUpdate(void)  { }
#include "core84/video_mode.h"
int VideoRender(void)  { return 0; }


#include "backend-common/options.c"

#include <stdlib.h>
int main(int argc, char *argv[])
{ return BackendMain(argc,argv) ? EXIT_FAILURE : EXIT_SUCCESS; }
