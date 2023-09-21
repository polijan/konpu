#include "renderer_pseudographics.h"
#if RENDERER_PSEUDOGRAPHICS
#include "renderer.h"
#include "video.h"
#include <stdio.h>


/// @brief RENDERER_PSEUDOGRAPHICS_STREAM settings:
/// By default, the PseudoGraphics renderer will output to stdout, however
/// you may define another stream by defining this macro constant.
#ifndef    RENDERER_PSEUDOGRAPHICS_STREAM
#   define RENDERER_PSEUDOGRAPHICS_STREAM   stdout
#endif

// flush the stream and check/return errors
// if error are found, clean stream, increment renderer's error count
static int
rendererPseudoGraphics_fflush()
{
   fflush(RENDERER_PSEUDOGRAPHICS_STREAM);
   int err = ferror(RENDERER_PSEUDOGRAPHICS_STREAM);
   if (unlikely(err)) {
      clearerr(RENDERER_PSEUDOGRAPHICS_STREAM);
      rendererSingleton.error++;
   }
   return err;
}


/// @brief RENDERER_PSEUDOGRAPHICS_UNLOCKED settings:
/// If this macro constant is defined and Konpu uses the POSIX platform, the
/// PseudoGraphics renderer will use the "_unlocked" version of `putc` to output
/// characters. This may be faster, but is thread-UNSAFE.
#if defined(RENDERER_PSEUDOGRAPHICS_UNLOCKED) && KONPU_PLATFORM_POSIX
#   define RENDERER_PSEUDOGRAPHICS_PUTCHAR(character) \
           putc_unlocked((character), RENDERER_PSEUDOGRAPHICS_STREAM)
#else
#   define RENDERER_PSEUDOGRAPHICS_PUTCHAR(character) \
           putc((character), RENDERER_PSEUDOGRAPHICS_STREAM)
#endif

// print the two first UTF-8 bytes for a U+25xx "block" character
#define  RENDERER_PSEUDOGRAPHICS_PUTBLOCK_START() do { \
           RENDERER_PSEUDOGRAPHICS_PUTCHAR(0xe2);      \
           RENDERER_PSEUDOGRAPHICS_PUTCHAR(0x96);      \
         } while(0)

// print a U+25xx "block" character (all three bytes)
#define  RENDERER_PSEUDOGRAPHICS_PUTBLOCK(byte) do {   \
           RENDERER_PSEUDOGRAPHICS_PUTBLOCK_START();   \
           RENDERER_PSEUDOGRAPHICS_PUTCHAR((byte));    \
         } while(0)

// TODO: user could decide to clean a terminal or not
//       put this somewhere ...
// escape sequence for clearing a terminal, ie: "\x1B[H\x1B[J"
#define  RENDERER_PSEUDOGRAPHICS_TTY_CLEAR() do { \
           RENDERER_PSEUDOGRAPHICS_PUTCHAR(0x1b); \
           RENDERER_PSEUDOGRAPHICS_PUTCHAR(0x5b); \
           RENDERER_PSEUDOGRAPHICS_PUTCHAR(0x48); \
           RENDERER_PSEUDOGRAPHICS_PUTCHAR(0x1b); \
           RENDERER_PSEUDOGRAPHICS_PUTCHAR(0x5b); \
           RENDERER_PSEUDOGRAPHICS_PUTCHAR(0x4a); \
         } while(0)


////////////////////////////////////////////////////////////////////////////////

// render screen using "1x1" blocks
static int rendererPseudoGraphics_renderFullBlocks(void)
{  canvas screen = video_getScreenCanvas();
   CANVAS_ASSERT(screen);

   for (int y = 0; y < GLYPH_WIDTH * screen.height; y++) {
       for (int x = 0; x < GLYPH_HEIGHT * screen.width; x++) {

           // get a position for pixel (x,y):
           uint64_t glyph = canvas_glyph(screen, x / GLYPH_WIDTH, y / GLYPH_HEIGHT);
           int n = glyph_pixelIndex(x % GLYPH_WIDTH, y % GLYPH_HEIGHT);

           // read pixel value and print space or block accordingly:
           if (uint_bitValue(glyph, n)) {
              RENDERER_PSEUDOGRAPHICS_PUTBLOCK(0x88); //U+2588 (full block)
           } else {
              RENDERER_PSEUDOGRAPHICS_PUTCHAR(0x20); // space
           }

       }
       RENDERER_PSEUDOGRAPHICS_PUTCHAR(0x0a); // '\n'
   }
   return rendererPseudoGraphics_fflush();
}


// render screen using "1x2" half blocks
static int rendererPseudoGraphics_renderHorizontalHalfBlocks(void)
{  canvas screen = video_getScreenCanvas();
   CANVAS_ASSERT(screen);
   for (int y = 0; y < GLYPH_WIDTH * screen.height; y += 2) {
       for (int x = 0; x < GLYPH_HEIGHT * screen.width; x++) {

           // get a position for pixel (x,y):
           uint64_t glyph = canvas_glyph(screen, x / GLYPH_WIDTH, y / GLYPH_HEIGHT);
           int n = glyph_pixelIndex(x % GLYPH_WIDTH, y % GLYPH_HEIGHT);

           // print half block
           unsigned char c = uint_bitValue(glyph, n)               << 1 |
                             uint_bitValue(glyph, n - GLYPH_WIDTH)      ;
           switch(c) {
              case 0 : RENDERER_PSEUDOGRAPHICS_PUTCHAR( 0x20); break; // space
              case 1 : RENDERER_PSEUDOGRAPHICS_PUTBLOCK(0x84); break; // U+2584 (lower half)
              case 2 : RENDERER_PSEUDOGRAPHICS_PUTBLOCK(0x80); break; // U+2580 (upper half)
              case 3 : RENDERER_PSEUDOGRAPHICS_PUTBLOCK(0x88); break; // U+2588 (full block)
              default: unreachable();
           }

       }
       RENDERER_PSEUDOGRAPHICS_PUTCHAR(0x0a); // '\n'
   }
   return rendererPseudoGraphics_fflush();
}


// render screen using "2x1" half blocks
static int rendererPseudoGraphics_renderVerticalHalfBlocks(void)
{  canvas screen = video_getScreenCanvas();
   CANVAS_ASSERT(screen);
   for (int y = 0; y < GLYPH_WIDTH * screen.height; y++) {
       for (int x = 0; x < GLYPH_HEIGHT * screen.width; x += 2) {

           // get a position for pixel (x,y):
           uint64_t glyph = canvas_glyph(screen, x / GLYPH_WIDTH, y / GLYPH_HEIGHT);
           int n = glyph_pixelIndex(x % GLYPH_WIDTH, y % GLYPH_HEIGHT);

           // print vertical half block
           unsigned char c = uint_bitValue(glyph, n)     << 1 |
                             uint_bitValue(glyph, n - 1)      ;
           switch(c) {
              case 0 : RENDERER_PSEUDOGRAPHICS_PUTCHAR( 0x20); break; // space
              case 1 : RENDERER_PSEUDOGRAPHICS_PUTBLOCK(0x90); break; // U+2590 (right half)
              case 2 : RENDERER_PSEUDOGRAPHICS_PUTBLOCK(0x8c); break; // U+258C (left half)
              case 3 : RENDERER_PSEUDOGRAPHICS_PUTBLOCK(0x88); break; // U+2588 (full block)
              default: unreachable();
           }

       }
       RENDERER_PSEUDOGRAPHICS_PUTCHAR(0x0a); // '\n'
   }
   return rendererPseudoGraphics_fflush();
}


// render screen using "2x2" quadrant blocks
static int rendererPseudoGraphics_renderQuadBlocks(void)
{  canvas screen = video_getScreenCanvas();
   CANVAS_ASSERT(screen);
   for (int y = 0; y < GLYPH_WIDTH * screen.height; y += 2) {
       for (int x = 0; x < GLYPH_HEIGHT * screen.width; x += 2) {

           // get a position for pixel (x,y):
           uint64_t glyph = canvas_glyph(screen, x / GLYPH_WIDTH, y / GLYPH_HEIGHT);
           int n = glyph_pixelIndex(x % GLYPH_WIDTH, y % GLYPH_HEIGHT);

           // read a half block character and print it out:
           unsigned char c = uint_bitValue(glyph, n)                   << 3 |
                             uint_bitValue(glyph, n - 1)               << 2 |
                             uint_bitValue(glyph, n - GLYPH_WIDTH)     << 1 |
                             uint_bitValue(glyph, n - GLYPH_WIDTH - 1)      ;

           // print it out as UTF-8
           static unsigned char quad_lastbyte[] = {
              0x20, 0x97, 0x96, 0x84, 0x9d, 0x90, 0x9e, 0x9f,
              0x98, 0x9a, 0x8c, 0x99, 0x80, 0x9c, 0x9b, 0x88  };
           if (c)  RENDERER_PSEUDOGRAPHICS_PUTBLOCK_START();
           RENDERER_PSEUDOGRAPHICS_PUTCHAR(quad_lastbyte[c]);

       }
       RENDERER_PSEUDOGRAPHICS_PUTCHAR(0x0a); // '\n'
   }
   return rendererPseudoGraphics_fflush();
}


// render screen using "2x4" braille dots
static int rendererPseudoGraphics_renderBrailleDots(void)
{  canvas screen = video_getScreenCanvas();
   CANVAS_ASSERT(screen);
   for (int y = 0; y < GLYPH_WIDTH * screen.height; y += 4) {
       for (int x = 0; x < GLYPH_HEIGHT * screen.width; x += 2) {

           // get a position for pixel (x,y):
           uint64_t glyph = canvas_glyph(screen, x / GLYPH_WIDTH, y / GLYPH_HEIGHT);
           int n = glyph_pixelIndex(x % GLYPH_WIDTH, y % GLYPH_HEIGHT);

           // Read the 2x4 bit pixels of the braille cell.        0x01 0x08
           // The 'Braille Patterns' Unicode block assigns  ___\  0x02 0x10
           // the codepoint 0x28XX, with XX being computed     /  0x04 0x20
           // with those weights for the raised bits,             0x40 0x80
           unsigned char braille =
              uint_bitValue(glyph, n                      ) * 0x01 |
              uint_bitValue(glyph, n -                   1) * 0x08 |
              uint_bitValue(glyph, n -     GLYPH_WIDTH    ) * 0x02 |
              uint_bitValue(glyph, n -     GLYPH_WIDTH - 1) * 0x10 |
              uint_bitValue(glyph, n - 2 * GLYPH_WIDTH    ) * 0x04 |
              uint_bitValue(glyph, n - 2 * GLYPH_WIDTH - 1) * 0x20 |
              uint_bitValue(glyph, n - 3 * GLYPH_WIDTH    ) * 0x40 |
              uint_bitValue(glyph, n - 3 * GLYPH_WIDTH - 1) * 0x80 ;

           // conversion to UTF-8 representation (3 bytes) reduces to:
           RENDERER_PSEUDOGRAPHICS_PUTCHAR(0xe2);
           RENDERER_PSEUDOGRAPHICS_PUTCHAR(0xa0 | (braille >> 6));
           RENDERER_PSEUDOGRAPHICS_PUTCHAR( 128 | (braille & 63));

       }
       RENDERER_PSEUDOGRAPHICS_PUTCHAR(0x0a); // '\n'
   }
   return rendererPseudoGraphics_fflush();
}


// render screen using "2x3" sextants blocks
// TODO/FIXME: this rendered is broken!
//             especially, when the sextant must read "pixel" from two glyphs,
//             this is not working properly.
static int rendererPseudoGraphics_renderSextantBlocks(void)
{  canvas screen = video_getScreenCanvas();
   CANVAS_ASSERT(screen);
   // The main issue with sextants is their height (3) isn't a divisor of the
   // glyph's height. The info to put in a sextant might come from TWO glyphs.

   for (int y = 0; y < GLYPH_WIDTH * screen.height; y += 3) {
       for (int x = 0; x < GLYPH_HEIGHT * screen.width; x += 2) {

           // glyph from which the sextant info comes from ...
           uint64_t glyph = canvas_glyph(screen, x / GLYPH_WIDTH, y / GLYPH_HEIGHT);
           int ypos = y % (GLYPH_WIDTH);
           int n = glyph_pixelIndex(x % GLYPH_WIDTH, ypos);

           // read the sextant pixels
           unsigned sext = uint_bitValue(glyph, n                      ) << 5 |
                           uint_bitValue(glyph, n -                   1) << 4 ;
                           // we started by reading the first line
           switch (ypos) {
              default:     // we can read the two following line from same glyph
                  sext |=  uint_bitValue(glyph, n -     GLYPH_WIDTH    ) << 3 |
                           uint_bitValue(glyph, n -     GLYPH_WIDTH - 1) << 2 |
                           uint_bitValue(glyph, n - 2 * GLYPH_WIDTH    ) << 1 |
                           uint_bitValue(glyph, n - 2 * GLYPH_WIDTH - 1)      ;
                  break;

              case  6:     // we can read one more line from same glyph
                  sext |=  uint_bitValue(glyph, n -     GLYPH_WIDTH    ) << 3 |
                           uint_bitValue(glyph, n -     GLYPH_WIDTH - 1) << 2 ;
                  if (y > GLYPH_WIDTH * screen.height)
                     break;
                  glyph = *(&glyph + screen.stride); // then we go to the glyph
                  n = glyph_pixelIndex(x % GLYPH_WIDTH, 0);  //    underneath
                  sext |=  uint_bitValue(glyph, n                      ) << 2 |
                           uint_bitValue(glyph, n                   - 1) << 1 ;
                  break;

              case  7:     // we need to read all the rest from underneath glyph
                  if (y > GLYPH_WIDTH * screen.height)
                     break;
                  glyph = *(&glyph + screen.stride);
                  n = glyph_pixelIndex(x % GLYPH_WIDTH, 0);
                  sext |=  uint_bitValue(glyph, n                      ) << 3 |
                           uint_bitValue(glyph, n                   - 1) << 2 |
                           uint_bitValue(glyph, n -     GLYPH_WIDTH    ) << 2 |
                           uint_bitValue(glyph, n -     GLYPH_WIDTH - 1) << 1 ;
                  break;
           }


//TODO/FIXME ASAP NOW: quick ugly temporary hack 'cause we       ^
//                     built the sextant wrongly!!! FIX ABOVE ---' and remove:
sext = byte_reverse(sext) >> 2;


           // print out the sextant characters in UTF-8

           // codepoints for sextants in the 'Symbols for Legacy Computing'
           // Unicode blocks are logically arranged as per the bit pixels, but
           // as sextant can be vertical half blocks, they sometimes have other
           // codepoints. I first thought to just have a "switch" or lookup for
           // all 64 cases, but I figured out this logic which isn't so
           // expensive to compute... do an integer division by 21:
           // sext = 21 * q + r,
           //  if r != 0  => codepoint is: (0x1FB00 - 1) + 20*q + r
           //             => else sextant is a half block.
           // in terms of UTF-8, codepoint 0x1FBxx is easy to compute.

           unsigned q = sext / 21;
           if (sext % 21) {
              // the UTF-8 conversion of codepoint 0x1FB00 - 1 + sext - q
              // is four bytes and reduces to this:
              RENDERER_PSEUDOGRAPHICS_PUTCHAR(0xf0);
              RENDERER_PSEUDOGRAPHICS_PUTCHAR(0x9f);
              RENDERER_PSEUDOGRAPHICS_PUTCHAR(0xac);
              RENDERER_PSEUDOGRAPHICS_PUTCHAR(128 | (sext - q - 1));
           } else {
              switch(q) { // sext is vertical halves/empty/full
                 case 0 : RENDERER_PSEUDOGRAPHICS_PUTCHAR( 0x20); break; // space
                 case 1 : RENDERER_PSEUDOGRAPHICS_PUTBLOCK(0x8c); break; // U+258C (left half)
                 case 2 : RENDERER_PSEUDOGRAPHICS_PUTBLOCK(0x90); break; // U+2590 (right half)
                 case 3 : RENDERER_PSEUDOGRAPHICS_PUTBLOCK(0x88); break; // U+2588 (full block)
                 default: unreachable();
              }
           }

       }
       RENDERER_PSEUDOGRAPHICS_PUTCHAR(0x0a); // '\n'
   }
   return rendererPseudoGraphics_fflush();
}


////////////////////////////////////////////////////////////////////////////////

int rendererPseudoGraphics_init(enum rendererPseudoGraphicsMode mode)
{
   // drop the active renderer
   renderer_drop();
   rendererSingleton.error = 0;

   // set the active render and render() once
   rendererSingleton.id = RENDERER_PSEUDOGRAPHICS;
   switch(mode) {
      case RENDERER_PSEUDOGRAPHICS_MODE_1x1:
         rendererSingleton.render = &rendererPseudoGraphics_renderFullBlocks;
         break;
      case RENDERER_PSEUDOGRAPHICS_MODE_1x2:
         rendererSingleton.render = &rendererPseudoGraphics_renderHorizontalHalfBlocks;
         break;
      case RENDERER_PSEUDOGRAPHICS_MODE_2x1:
         rendererSingleton.render = &rendererPseudoGraphics_renderVerticalHalfBlocks;
         break;
      case RENDERER_PSEUDOGRAPHICS_MODE_2x2:
         rendererSingleton.render = &rendererPseudoGraphics_renderQuadBlocks;
         break;
      case RENDERER_PSEUDOGRAPHICS_MODE_2x4:
         rendererSingleton.render = &rendererPseudoGraphics_renderBrailleDots;
         break;
      default: // falthrough
      case RENDERER_PSEUDOGRAPHICS_MODE_2x3:
         rendererSingleton.render = &rendererPseudoGraphics_renderSextantBlocks;
         break;
   }

   // TODO: to complete the initialization, we may wish to do something
   //       if we are writing to a terminal,
   //       either clear it (RENDERER_PSEUDOGRAPHICS_TTY_CLEAR)
   //       or move to the alternate screen (if the tty is capable)

   return 0;
}

#endif //RENDERER_PSEUDOGRAPHICS
