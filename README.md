(*o lukin e lipu ni lon toki pona: [OLUKIN.md](OLUKIN.md)*)

**ilo Konpu** is a [retro](https://en.wikipedia.org/wiki/Home_computer "retro")-style computing environment with native support for English and [Toki Pona](https://en.wikipedia.org/wiki/Toki_Pona "Toki Pona").
## In Short

- Tiny and Retro-styled (we're talking about kilobytes, not megabytes)
- Native support for printable ASCII and [Sitelen Pona](https://en.wikipedia.org/wiki/Sitelen_Pona) (with efficient encoding, input method, and bitmap fonts rendering engine)
- Easy multimedia access:
   - a small (default: ~11Kb) video canvas that works for text-oriented applications (with capabilities similar to a [terminal emulator](https://en.wikipedia.org/wiki/Terminal_emulator "terminal emulator")) but also offer the ability to do color graphics (via [attributes](https://en.wikipedia.org/wiki/Attribute_clash), [planar graphics](https://en.wikipedia.org/wiki/Planar_(computer_graphics)), or [standard](https://en.wikipedia.org/wiki/Packed_pixel) full bitmapped modes). Extremelly configurable.
   - an audio system with the capability to generate waveform signals and play 8Khz 8-bit PCM samples.
- An Interactive [shell](https://en.wikipedia.org/wiki/Read%E2%80%93eval%E2%80%93print_loop)
- I/O and (virtual) floppy disk interfaces.
- A small address space (fits in 20-bits) which you can freely [peek and poke](https://en.wikipedia.org/wiki/PEEK_and_POKE#Generic_usage_of_POKE) including a 512Kb heap to satisfy the dynamic memory allocation needs of applications.
- Simple but full-featured "[modern](https://www.youtube.com/watch?v=QpAhX-gsHMs)" **C** API: Just `#include <konpu.h>` and that's it, no other headers or dependencies are needed to develop apps (or games) that can run inside ilo Konpu.
- Or use **Waso**, a small and cute custom Sitelen Pona-based programming language and shell interpreter (taking inspiration from Toki Pona and [RPL](https://en.wikipedia.org/wiki/RPL_%28programming_language%29)), to interact with your environment or write scripts and apps.

## Status

Early work in progress.  
Nothing to see - Do Not click on the image below.

[![ilo Konpu first preview of an "App"](https://img.youtube.com/vi/Hr_CLlFB3Yg/0.jpg)](https://www.youtube.com/watch?v=Hr_CLlFB3Yg)
