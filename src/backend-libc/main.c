//------------------------------------------------------------------------------
// Initialization and Deinitialization of Backend's systems
//------------------------------------------------------------------------------

#include "core84/c.h"
static
uint64_t BackendEntropy(void)
{
   int unused_;
   return (uintptr_t) &unused_;
}

// void    VideoReset(void); //TODO <-- should be public? (so move to video.h/c ?)
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
// Parameters
//
// Optional Macro injecting extra code:
//
// BACKEND_EXTRA_HELP - code to add in the
// BACKEND_EXTRA_SHORTOPTS - literal string for extra short flags
// BACKEND_EXTRA_LONGOPTS  - code for extra long options definitions
// BACKEND_EXTRA_CASE - code to add in the switch reading options arguments
//
// BACKEND_EXTRA_INIT - initialisation code to add
// BACKEND_EXTRA_DROP - cleanup code to add
//------------------------------------------------------------------------------

#ifndef BACKEND_DEFAULT_ZOOM
#define BACKEND_DEFAULT_ZOOM  3
#endif

#ifndef BACKEND_DEFAULT_APP
#define BACKEND_DEFAULT_APP  "apps/lib/test.so"   // "apps/lib/lsmodes.so"
#endif





// Core Includes
#include "core84/arch.h"
#include "core84/app.h"
#include "core84/random.h"

// Backend Includes
#include "backend-common/backend.h"
#include "backend-common/printer.h"
#include "backend-common/options.h"

// C Includes
#include <stdio.h>
// ^-- TODO: if we'd get rid of the <stdio.h> include,
//           we could move this file to backend-common


// Print the program's usage (to stdout)
// if the bool parameter is true, write in English, otherwise in Toki Pona.
static void BackendHelp(bool english)
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


int BackendMain(int argc, char *argv[])
{
   int ret = 0;

   // Initialize platform and subsystems (BEFORE reading CLI arguments)
   ret = BackendInitPlatform();
   if (ret) {
      fprintf(stderr, "Cannot initialize SDL\n");
      return ret;
   }

   ret = BackendInitPrinter();
   if (ret) {
      fprintf(stderr, "Cannot initialize Printer\n");
      goto clean_cli;
   }

   // Read command line arguments
#  ifndef BACKEND_EXTRA_SHORTOPTS
#  define BACKEND_EXTRA_SHORTOPTS ""
#  endif
   const char *optstr = "hvfz:" BACKEND_EXTRA_SHORTOPTS ;
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
#     ifdef BACKEND_EXTRA_LONGOPTS
            BACKEND_EXTRA_LONGOPTS
#     endif
      {0}
   };

   const char *app = BACKEND_DEFAULT_APP;
   int max_desired_zoom = BACKEND_DEFAULT_ZOOM;
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
                     ret = 64; // (typically EX_USAGE in <sysexits.h>)
                     goto clean_cli;

      // Help Options:
         case 'H':   BackendHelp(0); goto clean_cli;
         case 'h':   BackendHelp(1); goto clean_cli;
         case 'v':   printf("Core   : %d.%d.%d\nBackend: ...\n",
                        KONPU_VERSION_MAJOR, KONPU_VERSION_MINOR, KONPU_VERSION_PATCH);
                     goto clean_cli;

      // Graphical Options:
         case '0':   video_output = false; break;
         case 'f':   max_desired_zoom = 0; break;
         case 'z':   max_desired_zoom = atoi(opt.arg);
                     if (max_desired_zoom <= 0) max_desired_zoom = BACKEND_DEFAULT_ZOOM;
                     break;

      // Start-Up Options:
         case 'A':   app = opt.arg; break;

      // IO Interfaces Options:
         case '1':   KonpuConnectPrinterToStdout(); break;
         case '2':   KonpuConnectPrinterToStderr(); break;
         case 'P':   if (! KonpuConnectPrinterToFile(opt.arg))
                        perror("Cannot connect Printer Interface");
                     break;

#        ifdef BACKEND_EXTRA_CASE
               BACKEND_EXTRA_CASE
#        endif
      }
   }

   // Initialize backend systems (AFTER reading CLI arguments)

   ret = BackendInitTime();
   if (ret) {
      fprintf(stderr, "Cannot Initialize Time\n");
      goto clean;
   }

   // Initialize Konpu core
   KonpuReboot();

   if (video_output) {
      ret = BackendInitGraphics(max_desired_zoom);
      if (ret) {
         fprintf(stderr, "Cannot Initialize Graphics\n");
         goto clean;
      }
   }

   ret = BackendInitMouse();
   if (ret) goto clean;

   uint64_t entropy = BackendEntropy();
   RandomInit(entropy);

#  ifdef BACKEND_EXTRA_INIT
         BACKEND_EXTRA_INIT
#  endif

   // Loading "App"
   DEBUG("Loading App: %s\n", app);
   void *handle = AppLoad(app);
   if (handle == NULL) {
      // TODO: some `perror` instead of fprintf?
      fprintf(stderr, "Cannot load app '%s'\n", app);
      ret = 1;
      goto clean;
   }

   // De-Init the app
   DEBUG("UnLoading App: %s\n", app);
   ret = AppUnload(handle); // TODO: write something if unload fails?


clean: // De-Initialize subsystems
   DEBUG("Shuting Down\n");
   BackendDropMouse();
   if (video_output) BackendDropGraphics();
   BackendDropTime();
#  ifdef BACKEND_EXTRA_DROP
         BACKEND_EXTRA_DROP
#  endif


clean_cli: // De-initialized subsystems initialized before reading CLI arguments
   BackendDropPrinter();
   BackendDropPlatform();
   return ret;
}
