/*******************************************************************************
 * @file
 * miscellaneous macros or utilities used throughout the project,
 *                                   or that we wish to provide to users.
 ******************************************************************************/
#ifndef  KONPU_UTIL_H
#define  KONPU_UTIL_H
#include "platform.h"
#include "c.h"


//===< time >===================================================================

// sleep (in milliseconds)
// *tries* to wait a specified number of milliseconds before returning.
//
// depending on the platform, this functionality might not be available
// and thus may return "immediately". On the other hand, if it is implemented,
// it may dlay a bit longer due to OS scheduling. If milliseconds is <= 0, it
// returns immediately.
void sleep_ms(int milliseconds);

//===</ time >==================================================================



//===< PSEUDO RANDOM 64-bits NUMBER GENERATOR (PRNG) >==========================
// 64 bits PRNG (STC64):
// (we want to have a 64 bits,so can use it to generate random glyph)
//
// Usage:
//
// 1) Use the global wrapper:
//    uint64_t seed = 123456789;
//    random_init(seed);                   // init with the seed
//    uint64_t i = random();               // get a random number
//    double   f = randomf();              // get a random double [0,1)
//
// 2) Or alternatively, pass the state object around:
//    uint64_t  seed = 123456789;
//    stc64_rng rng  = stc64_init(seed);    // init with the seed
//    uint64_t  i    = stc64_random(&rng);  // get a random number
//    double    f    = stc64_randomf(&rng); // get a random double
//

// I extracted this PRNG from the STC library (https://github.com/tylov/STC),
// so that part of code is under MIT license. I changed a bit the naming, etc.
//
/* MIT License
 *
 * Copyright (c) 2022 Tyge Løvset, NORCE, www.norceresearch.no
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

// (hidden) global object:
static inline  void       random_init(uint64_t seed);
static inline  uint64_t   random(void);
static inline  double     randomf(void);

// or explicitely, use by passing around the state:
typedef struct stc64_rng  { uint64_t state[5]; } stc64_rng; // PRNG state
// functions:
static inline  stc64_rng  stc64_init(uint64_t seed);
static inline  uint64_t   stc64_random (stc64_rng* rng);
static inline  double     stc64_randomf(stc64_rng* rng);


//--- inline implementation ----------------------------------------------------

static inline void random_init(uint64_t seed) {
   extern stc64_rng stc64_global; // global stc64 state
   stc64_global = stc64_init(seed);
}

static inline uint64_t random(void) {
   extern stc64_rng stc64_global; // global stc64 state
   return stc64_random(&stc64_global);
}

static inline double randomf(void) {
   extern stc64_rng stc64_global; // global stc64 state
   return stc64_randomf(&stc64_global);
}

/* Init stc64 prng with and without sequence number */
stc64_rng stc64_with_seq(uint64_t seed, uint64_t seq);
static inline stc64_rng stc64_init(uint64_t seed)
   { return stc64_with_seq(seed, seed + 0x3504f333d3aa0b37); }

// Main stc64 prng
static inline uint64_t
stc64_random(stc64_rng* rng) {
    uint64_t *s = rng->state; enum {LR=24, RS=11, LS=3};
    const uint64_t result = (s[0] ^ (s[3] += s[4])) + s[1];
    s[0] = s[1] ^ (s[1] >> RS);
    s[1] = s[2] + (s[2] << LS);
    s[2] = ((s[2] << LR) | (s[2] >> (64 - LR))) + result;
    return result;
}

// Float64 random number in range [0.0, 1.0).
static inline double stc64_randomf(stc64_rng* rng) {
    union {uint64_t i; double f;} u = {0x3FF0000000000000ull | (stc64_random(rng) >> 12)};
    return u.f - 1.0;
}

//===</ PSEUDO RANDOM NUMBER GENERATOR (PRNG) >=================================



//===< MISC. >==================================================================

// compile-time constant of type size_t representing the number of elements of
// the given array. ONLY USE ON ARRAYS with size known at compile time (no VLAs)
// DON'T USE ON POINTERS (hint: arrays received as parameters decay to pointers)
#define ARRAY_SIZE(array)       (sizeof((array)) / sizeof((array)[0]))

// `strlen` as a compile time constant for C string literals (BE CAREFUL)
#define CSTR_LENGTH(cstr_lit)   (ARRAY_SIZE(cstr_lit) - 1)

// min and max macros
#define UTIL_MIN(x, y)          (((x) < (y)) ? (x) : (y))
#define UTIL_MAX(x, y)          (((x) > (y)) ? (x) : (y))

// clamp an *integer* variable of a *signed* type between 0 and a max value
#define UTIL_CLAMP(x, max) do                  \
        { if      ((x) < 0)      (x) = 0;      \
          else if ((x) > (max))  (x) = (max);  \
        } while (0)

// The typical SWAP macro that swaps two values.
// Classically in C, it also passes the type as parameter, like this:
//    #define UTIL_SWAP(T, x, y) do { T tmp = (x); (x) = (y); (y) = tmp; } while (0)
// but we can remove this redundant parameter.
//
#if __STDC_VERSION__ > 201710L
    // ^^-- strictly superior to C17, ie the (as-of-today) upcoming C23 and
    //      later versions which would have "native" typeof (and static_assert)
#   define UTIL_SWAP(x, y) do                            \
           { static_assert(sizeof((x)) == sizeof((y)));  \
             typeof((x)) tmp = (x);                      \
             (x) = (y);                                  \
             (y) = tmp;                                  \
           } while (0)
#elif (defined(__GNUC__) || defined(__clang__))
#   define UTIL_SWAP(x, y) do                            \
           { static_assert(__builtin_types_compatible_p(__typeof__((x)), __typeof__((y)))); \
             __typeof__((x)) tmp = (x);                  \
             (x) = (y);                                  \
             (y) = tmp;                                  \
           } while (0)
#else
    // inspired by: https://stackoverflow.com/questions/3982348/implement-generic-swap-macro-in-c
    // memcopy would hopefully be optimized by the compiler as just the swap.
#   define UTIL_SWAP(x, y) do                            \
           { unsigned char tmp[sizeof((x)) == sizeof((y)) ? (signed)sizeof((x)) : -1]; \
             memcpy(tmp,  &(y), sizeof((x)));            \
             memcpy(&(y), &(x), sizeof((x)));            \
             memcpy(&(x), tmp,  sizeof((x)));            \
           } while(0)
#endif

//===</ MISC. >=================================================================

#endif //KONPU_UTIL_H
