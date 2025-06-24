// The first part of this file is the PCG random number generator
// (https://www.pcg-random.org/), Minimal C Implementation, 0.9.
// Mostly as-is, just with all functions marked as static
//
// The second part of the file adapts the random number generator
// to Konpu's random interface.
//------------------------------------------------------------------------------

#include <stdint.h>
 /*
 * PCG Random Number Generation for C.
 * Copyright 2014 Melissa O'Neill <oneill@pcg-random.org>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For additional information about the PCG random number generation scheme,
 * including its license and other licensing options, visit
 *
 *     http://www.pcg-random.org
 */

struct pcg_state_setseq_64 {  // Internals are *Private*.
   uint64_t state;            // RNG state.  All values are possible.
   uint64_t inc;              // Controls which RNG sequence (stream) is
                              // selected. Must *always* be odd.
};
typedef struct pcg_state_setseq_64 pcg32_random_t;

// If you *must* statically initialize it, here's one.
#define PCG32_INITIALIZER   { 0x853c49e6748fea9bULL, 0xda3e39cb94b95bdbULL }

// State for global RNGs
// static pcg32_random_t pcg32_global = PCG32_INITIALIZER;

// Generate a uniformly distributed 32-bit random number
static uint32_t pcg32_random_r(pcg32_random_t* rng)
{
   uint64_t oldstate = rng->state;
   rng->state = oldstate * 6364136223846793005ULL + rng->inc;
   uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
   uint32_t rot = oldstate >> 59u;
   return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

// Seed the rng.  Specified in two parts, state initializer and a
// sequence selection constant (a.k.a. stream id)
static
void pcg32_srandom_r(pcg32_random_t* rng, uint64_t initstate, uint64_t initseq)
{
   rng->state = 0U;
   rng->inc = (initseq << 1u) | 1u;
   pcg32_random_r(rng);
   rng->state += initstate;
   pcg32_random_r(rng);
}

// Generate a uniformly distributed number, r, where 0 <= r < bound
static uint32_t pcg32_boundedrand_r(pcg32_random_t* rng, uint32_t bound)
{
   // To avoid bias, we need to make the range of the RNG a multiple of
   // bound, which we do by dropping output less than a threshold.
   // A naive scheme to calculate the threshold would be to do
   //
   //     uint32_t threshold = 0x100000000ull % bound;
   //
   // but 64-bit div/mod is slower than 32-bit div/mod (especially on
   // 32-bit platforms).  In essence, we do
   //
   //     uint32_t threshold = (0x100000000ull-bound) % bound;
   //
   // because this version will calculate the same modulus, but the LHS
   // value is less than 2^32.

   uint32_t threshold = -bound % bound;

   // Uniformity guarantees that this loop will terminate.  In practice, it
   // should usually terminate quickly; on average (assuming all bounds are
   // equally likely), 82.25% of the time, we can expect it to require just
   // one iteration.  In the worst case, someone passes a bound of 2^31 + 1
   // (i.e., 2147483649), which invalidates almost 50% of the range.  In
   // practice, bounds are typically small and only a tiny amount of the range
   // is eliminated.
   for (;;) {
      uint32_t r = pcg32_random_r(rng);
      if (r >= threshold)
         return r % bound;
   }
}

//------------------------------------------------------------------------------
// Adapt PCG32 to Konpu
//------------------------------------------------------------------------------
#include "random.h"
#include "ram.h"

// We keep a global state a RAM
#define RAM_PCG32  ((pcg32_random_t *)(RAM + RAM_RNG_STATE))

uint32_t UtilRandom32_0_(void)
{ return pcg32_random_r(RAM_PCG32); }

uint32_t UtilRandom32_1_(uint32_t bound)
{ return pcg32_boundedrand_r(RAM_PCG32, bound); }

void UtilInitRandom_2_(uint64_t seed, uint64_t seq)
{ pcg32_srandom_r(RAM_PCG32, seed, seq); }

#include "var.h"
void UtilInitRandom_1_(uint64_t seed)
{
   // As we don't have a sequence, let's create one with bits shenanigans...
   uint64_t seq = VarHash((var){.uint64 = seed});
   pcg32_srandom_r(RAM_PCG32, seed, seq);
}

#if KONPU_PLATFORM_LIBC
#   include <time.h>  // C's <time.h>
#endif
#include "time.h" // Konpu's "time.h"
#include "rom.h"
void UtilInitRandom_0_(void)
{
   // Random Compile-time constant
   uint64_t seed =
      // Some pointer values that may vary in Konpu
         (uint64_t)(uintptr_t)ROM + (uint64_t)(uintptr_t)RAM
      // Some constants that comes from our config/build
      +  (uint64_t)__STDC_VERSION__
      +  KONPU_VERSION_PATCH +
      +  (KONPU_PLATFORM_SDL1 + KONPU_PLATFORM_SDL2 + KONPU_PLATFORM_SDL3) * 4
      +  KONPU_PLATFORM_POSIX * 9 + KONPU_PLATFORM_WINDOWS * 11;
#ifdef NDEBUG
   seed += 7;
#endif
#ifdef __GNUC__
   seed += 32;
#endif
#ifdef __clang__
   seed += 64;
#endif
#ifdef _MSC_VER
   seed += 128;
#endif
#ifdef SDL_PATCHLEVEL
   seed += SDL_PATCHLEVEL;
#endif
#ifdef __GNUC_MINOR__
   seed += __GNUC_MINOR__;
#endif

   // Most importantly, get some runtime time-based value(s)
#if KONPU_PLATFORM_LIBC
   seed += (uint64_t)time(NULL);
#endif
   seed += TimeTicks();

   UtilInitRandom_1_(seed);
}
