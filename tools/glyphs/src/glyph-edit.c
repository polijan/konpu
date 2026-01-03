// Maybe move this away, for example in the Makefile as -D... flags
#ifndef   _XOPEN_SOURCE
#  define _XOPEN_SOURCE
#endif
#ifndef   _DEFAULT_SOURCE
#  define _DEFAULT_SOURCE
#endif

#include  <locale.h>
#include  "termbox2/termbox2.h"

#include "glyph_tools.h"
#include <stdio.h>

C_ATTRIBUTE_NORETURN
static void Usage(const char* argv0, bool fail)
{
   const char *prog = UtilBasename(argv0);
   if (fprintf(stderr, "Usage: %s [GLYPH]\n", prog) <= 0) fail = true;
   exit(fail ? EXIT_FAILURE : EXIT_SUCCESS);
}

//------------------------------------------------------------------------------
// Glyph256
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
// Components of the TUI
//------------------------------------------------------------------------------
//  MMMMMMMM   CONTROLS
//  M GRID M   ......
//  MMMMMMMM
//  MMMMMMMM   PREVIEW
//
//  STRING...
//------------------------------------------------------------------------------

// Draw controls and other static elements
static void DrawControls(void)
{
   // Header
   tb_print(1,0, TB_BRIGHT|TB_CYAN, TB_DEFAULT, "𜷀𜴍𜵑𜺣𜵡𜺣𜵡𜺣▖▖𜴘▘𜵑𜴉  𜵉𜴶▐ ▖▖𜵡𜺣𜵦𜺣  𜵨𜺨𜵑▌𜶑 𜶦𜴉𜵑𜺣𜶻𜴉");
   tb_print(1,1, TB_BRIGHT|TB_CYAN, TB_DEFAULT, "▘▘𜴄𜺨▘▘𜵏𜺨𜴄▘  𜴈𜺨  𜴄▘𜺫𜴀𜶴𜴍𜵏𜺨▘▘  𜴇𜴀𜴄▘▝ 𜺫𜴀𜴄𜺨▘");

   // Controls
   struct { const char *key; const char *msg; } controls[] = {
      { "▲▼◀▶",   "Move Cursor"},
      { "Space",  "Toogle Pixel"},
      { "M-▲▼◀▶", "Move Glyph"},
      { "^◀ ^▶",  "Rotate Glyph"},
      { "^F",     "Flip Glyph"},
      { "^M",     "Mirror Glyph"},
      { "^I",     "Inverse Glyph"},
      { "^S",     "Save (selo-copy)"},
      { "^X",     "Reset Glyph"},
      { "^L",     "Refresh UI"},
      { "^Q",     "Quit"},
   };

   int col_key = 1 + 2 * GLYPH256_WIDTH + 4;
   int col_msg = col_key + 8;
   for (size_t row = 0; row < COUNTOF(controls); row++) {
      tb_print(col_key, row + 3, TB_BRIGHT | TB_YELLOW,   TB_DEFAULT, controls[row].key);
      tb_print(col_msg, row + 3, TB_DEFAULT, TB_DEFAULT, controls[row].msg);
   }
}


static void DrawGrid(Glyph256 g, int x, int y) {
   (void)x; (void)y; // TODO

/*
   int type = 256;
   if (g.top_left == 0 && g.top_right == 0) {
      type = 128;
      if (g.bottom_left == 0) type = 64;
   }
*/

   int j = 0;
   for (; j < GLYPH256_WIDTH; j++) {
      tb_print(0, 3+j, TB_BRIGHT|TB_WHITE, TB_DEFAULT, "▕");
      for (int i = 0; i < GLYPH256_HEIGHT; i++) {
         const char *str;
         uintattr_t bg;
         if (x == i && y == j) {
           str = "▔🭾";

           bg = GlyphGetPixel(g, i,j)? TB_YELLOW:(TB_BLACK|TB_BRIGHT);
         } else {
           bg = TB_BLACK;
           str = GlyphGetPixel(g, i,j)? "██":"▔🭾";
         }
         tb_print(1+2*i, 3+j, TB_WHITE|TB_BRIGHT, bg, str);
      }
   }
   for (int i = 0; i < 2 * GLYPH256_HEIGHT; i++) {
      tb_print(1+i, 3+j, TB_BRIGHT|TB_WHITE, TB_DEFAULT, "▔");
   }
}

static void DrawPreview(Glyph256 g)
{
   (void)g; // TODO

   int col = 1 + 2 * GLYPH256_WIDTH + 4;// + 8;
   tb_printf(col, 15, TB_YELLOW|TB_BRIGHT, TB_BLUE, "@@@@@@@@");
   tb_printf(col, 16, TB_YELLOW|TB_BRIGHT, TB_BLUE, "@@@@@@@@");
   tb_printf(col, 17, TB_YELLOW|TB_BRIGHT, TB_BLUE, "@@@@@@@@");
   tb_printf(col, 18, TB_YELLOW|TB_BRIGHT, TB_BLUE, "@@@@@@@@");
}

static void DrawFooter(Glyph256 g)
{
   int row_str = tb_height() - 1;
   for (int i = 0; i < tb_width(); i++)
      tb_print(i, row_str, TB_DEFAULT, TB_CYAN, " ");
   tb_print(1, row_str, TB_WHITE, TB_CYAN, "Glyph:");
   tb_printf(8, row_str, TB_BLACK, TB_CYAN,
      "%016"PRIX64":%016"PRIX64":%016"PRIX64":%016"PRIX64,
      g.top_left, g.top_right, g.bottom_left, g.bottom_right);
}



int main(int argc, char *argv[])
{
   // Read the glyph from arguments
   Glyph256 glyph = {0};

   switch (argc) {
      default: Usage(argv[0], true);
      case 1: break;
      case 2:
         if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
            Usage(argv[0], false);

         Symbol s = SymbolInit(argv[1]);
         switch (s.size) {
            default:
               fprintf(stderr, "Invalid Glyph format.\n");
               return EXIT_FAILURE;
            case  1: glyph.top_left = s.glyph8;  break;
            case  2: glyph.top_left = s.glyph16; break;
            case  4: glyph.top_left = s.glyph32; break;
            case  8: glyph.top_left = s.glyph64; break;
            case 16:
               glyph.top_left    = s.glyph128.top;
               glyph.bottom_left = s.glyph128.bottom;
               break;
            case 32:
               glyph = s.glyph256;
               break;
         }
         break;
   }

   // Init Termbox
   setlocale(LC_ALL, ""); // We actually expect UTF-8
   int ret = tb_init();
   if (ret) {
      fprintf(stderr, "Cannot init Termbox2");
      return EXIT_FAILURE;
   }
   // Init Variables
   int cursor_x = 0;
   int cursor_y = 0;
   bool paint = true;
   DrawControls();

   // Main loop
   struct tb_event ev;
   while (true) {
      if (paint) {
         DrawGrid(glyph, cursor_x, cursor_y);
         DrawPreview(glyph);
         DrawFooter(glyph);
         tb_present();
         paint = false;
      }

      ret = tb_poll_event(&ev);
      if (ret != TB_OK) {
         // if poll was interrupted, maybe by a SIGWINCH, try again
         if (ret == TB_ERR_POLL && tb_last_errno() == EINTR)  continue;
         // but if some other error occurred, bail
         break;
      }
      switch (ev.type) {

         case TB_EVENT_KEY:
            switch (ev.key) {
               default: break;
               case TB_KEY_CTRL_Q: goto quit;
               case TB_KEY_CTRL_L:
                  tb_clear();
                  DrawControls();
                  paint = true;
                  break;
               case TB_KEY_CTRL_X:
                  glyph = (Glyph256){ 0 };
                  paint = true;
                  break;
               case TB_KEY_CTRL_I:
                  glyph = GlyphNot(glyph);
                  paint = true;
                  break;
               case TB_KEY_CTRL_F:
                  glyph = GlyphFlip(glyph);
                  paint = true;
                  break;
               case TB_KEY_CTRL_M:
                  glyph = GlyphMirror(glyph);
                  paint = true;
                  break;
               case TB_KEY_SPACE:
                  // TODO: Those are not implemented in Konpu's "glyph.h"
                  //Toggle(glyph, cursor_x, cursor_y);
                  //GlyphSetPixel(glyph, cursor_x, cursor_y);
                  paint = true;
                  break;
               case TB_KEY_ARROW_UP:
                  if (ev.mod == TB_MOD_CTRL) {
                     glyph = GlyphCycleUp(glyph, 1);
                  } else if (cursor_y > 0) {
                     cursor_y--;
                  } else break;
                  paint = true;
                  break;
               case TB_KEY_ARROW_DOWN:
                  if (ev.mod == TB_MOD_CTRL) {
                     glyph = GlyphCycleDown(glyph, 1);
                  } else if (cursor_y < GLYPH256_HEIGHT - 1) {
                     cursor_y++;
                  } else break;
                  paint = true;
                  break;
               case TB_KEY_ARROW_LEFT:
                  if (ev.mod == TB_MOD_CTRL) {
                     glyph = GlyphCycleLeft(glyph, 1);
                  } else if (ev.mod == TB_MOD_SHIFT) {
                     glyph = GlyphRotate90(glyph);
                  } else if (cursor_x > 0) {
                     cursor_x--;
                  } else break;
                  paint = true;
                  break;
               case TB_KEY_ARROW_RIGHT:
                  if (ev.mod == TB_MOD_CTRL) {
                     glyph = GlyphCycleRight(glyph, 1);
                  } else if (ev.mod == TB_MOD_SHIFT) {
                     glyph = GlyphRotate270(glyph);
                  } else if (cursor_x < GLYPH256_WIDTH - 1) {
                     cursor_x++;
                  } else break;
                  paint = true;
                  break;
            }
            break;

         case TB_EVENT_RESIZE:
            tb_clear();
            DrawControls();
            paint = true;
            break;

         default:
            break;
      }
   }


quit:
   tb_shutdown();
   return EXIT_SUCCESS;
}



// Include termbox implementation
#define  TB_IMPL
#pragma  GCC diagnostic push
#pragma  GCC diagnostic ignored "-Wsuggest-attribute=format"
#pragma  GCC diagnostic ignored "-Wswitch-default"
#include "termbox2/termbox2.h"
#pragma  GCC diagnostic pop
