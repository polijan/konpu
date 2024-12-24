#include "init.h"
#include "options.h"
#include "printer.h"
#include "memory.h"
#include "app.h"

#include "platform_include.h"
#include <stdio.h>
#include <stdlib.h>


#define DEFAULT_ZOOM  2
#define DEFAULT_APP  "apps/lib/test.so"
   // "apps/lib/lsmodes.so"


// Print the program's usage.
// if the bool is true, write in English, otherwise in Toki Pona.
//
// Note: None of this help text is in ROM because that's not a part of the
//       Konpu computer per se.
static void Help(bool english)
{
   if (english) {
      puts("Usage: konpu [OPTION]...");
      puts("ilo Konpu - recreational computing environment.");
   } else {
      puts("o kepeken sama ni: konpu [NAMAKO]...");
      puts("ilo Konpu li ilo lili tawa pali nanpa anu tawa musi.");
   }

   printf("\n%s:\n", english? "Help options" : "namako sona");
   printf("  --seme             %s.\n", english?
      "show help in Toki Pona and exit":
      "o pana e sona kepeken toki pona. pini");
   printf("  -h, --help         %s.\n", english?
      "show help in English and exit":
      "o pana e sona kepeken toki Inli. pini");
   printf("  -v, --version      %s.\n", english?
      "show version and exit":
      "o toki e ni: ilo ni li nanpa seme. pini");


   // TODO: use Options ...
   // Help:
   // -h, --help            --> show help and exit
   // --seme                --> show basic help in Toki Pona and exit
   // -v, --version         --> show version and exit
   // Printer Interface Options:
   //   -[-printer]-stdout  --> connect printer interface to stdout
   //   -[-printer]-stderr  --> connect printer interface to stderr
   //   --printer=FILENAME  --> connect printer interface to the given file
   // Video Backend Initialization
   //   -f, --fullscreen    --> fullscreen
   //   -z, --zoom          --> desired zoom (max)
   //   --novideo           --> do not output video (video system still present)
   // Waso/Startup Options
   //   --app=FILENAME      --> load an app
   // Memory Options
   //   --mem=...           --> hmmm, maybe later, but not now!
}


int main(int argc, char *argv[])
{
   int return_code = 0;

   // Initialize subsystems (BEFORE reading CLI arguments)
   PrinterInit();

   // Read command line arguments
   const char *optstr = "hvfz:";
   OptionsLong longopts[] = {
      // Help Options:
      { "help",           OPTIONS_NO_ARGUMENT,       'h' },
      { "seme",           OPTIONS_NO_ARGUMENT,       'H' },
      { "version",        OPTIONS_NO_ARGUMENT,       'v' },
      // Video Options:
      { "novideo",        OPTIONS_NO_ARGUMENT,       '0' },
      { "fullscreen",     OPTIONS_NO_ARGUMENT,       'f' },
      { "zoom",           OPTIONS_REQUIRED_ARGUMENT, 'z' },
      // Start-Up Options:
      { "app",            OPTIONS_REQUIRED_ARGUMENT, 'A' },
      // Printer Interface Options:
      { "stdout",         OPTIONS_NO_ARGUMENT,       '1' },
      { "printer-stdout", OPTIONS_NO_ARGUMENT,       '1' },
      { "stderr",         OPTIONS_NO_ARGUMENT,       '2' },
      { "printer-stderr", OPTIONS_NO_ARGUMENT,       '2' },
      { "printer",        OPTIONS_REQUIRED_ARGUMENT, 'P' },
      // ...
      {0}
   };

   const char *app = DEFAULT_APP;
   int max_desired_zoom = DEFAULT_ZOOM;
   bool video_output = true;
   int flag;
   Options opt = OptionsInit();
   while ((flag = OptionsGet(&opt, argc, argv, optstr, longopts)) != -1) {
      switch(flag) {

      // Usage errors
         case '?':   fprintf(stderr, "konpu: Unknown option: %s", opt.optstr);
                     goto cli_error;
         case ':':   fprintf(stderr, "konpu: Missing argument for %s (%c)", opt.optstr, opt.opt);
                     goto cli_error;
         default :   // Note: we shouldn't be here (but anyway...):
                     fprintf(stderr, "konpu: Arguments error [%c]", flag);
cli_error:
                     fputs("\no ni tawa sona kepeken: konpu --seme" \
                     "\nTry 'konpu --help' for more information.\n", stderr);
                     return_code = 64; // (typically EX_USAGE in <sysexits.h>)
                     goto clean_cli;

      // Help Options:
         case 'H':   Help(0); goto clean_cli;
         case 'h':   Help(1); goto clean_cli;
         case 'v':   printf("%d.%d.%d\n", KONPU_VERSION_MAJOR, KONPU_VERSION_MINOR, KONPU_VERSION_PATCH);
                     goto clean_cli;

      // Graphical Options:
         case '0':   video_output = false; break;
         case 'f':   max_desired_zoom = 0; break;
         case 'z':   max_desired_zoom = atoi(opt.arg);
                     if (max_desired_zoom <= 0) max_desired_zoom = DEFAULT_ZOOM;
                     break;

      // Start-Up Options:
         case 'A':   app = opt.arg; break;

      // IO Interfaces Options:
         case '1':   KonpuConnectPrinterToStdout(); break;
         case '2':   KonpuConnectPrinterToStderr(); break;
         case 'P':   if (! KonpuConnectPrinterToFile(opt.arg))
                        perror("Cannot connect Printer Interface");
                     break;
      }
   }

   // Initialize subsystems (AFTER reading CLI arguments)
   PlatformInit();
   ColorInit();
   HeapInit();
   StackInit();
   VideoInit();
   if (video_output) {
      return_code = VideoBackendInit(max_desired_zoom);
      if (return_code != 0) goto clean;
   }

   // Loading "App"
   DEBUG("Loading App: %s\n", app);
   void *handle = AppLoad(app);
   if (handle == NULL) {
      // TODO: some `perror` instead of fprintf?
      fprintf(stderr, "Cannot load app '%s'\n", app);
      return_code = 1;
      goto clean;
   }

   // De-Init the app
   DEBUG("UnLoading App: %s\n", app);
   return_code = AppUnload(handle);
   // TODO: write something if unload fails?


clean: // De-Initialize subsystems
   DEBUG("Shuting Down Up\n");
   if (video_output) VideoBackendDrop();
   VideoDrop();
   StackDrop();
   HeapDrop();
   ColorDrop();
   PlatformDrop();
clean_cli: // De-initialized subsystems initialized before reading CLI arguments
   PrinterDrop();
   return (return_code)? EXIT_FAILURE : EXIT_SUCCESS;
}
