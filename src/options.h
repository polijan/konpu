// An API offering a `OptionsGet()` function for command line argument parsing,
// which is similar to `getopt_long` (GNU's extension of POSIX's getopt()).
// The heap is not used.


#ifndef  KONPU_OPTIONS_H
#define  KONPU_OPTIONS_H
#include <stddef.h>

// This is extracted from the excellent STC library, by Tyge Løvset
// See: https://github.com/stclib/STC/
// Specificially, about this API, see:
// https://github.com/stclib/STC/blob/eb21f59e5d17cb547eefa4545b2789e383a99174/include/stc/coption.h
// https://github.com/stclib/STC/blob/eb21f59e5d17cb547eefa4545b2789e383a99174/docs/coption_api.md
// License: MIT / Copyright (see end of file)


/** @file
 * OptionsGet() is similar to GNU's getopt_long(). Each call parses one option and
 * returns the option name. opt->arg points to  the option argument if present.
 * The function returns -1 when all command-line arguments are parsed. In this case,
 * opt->ind is the index of the first non-option argument.
 *
 * #include <stdio.h>
 * #include "options.h"
 *
 * int main(int argc, char *argv[])
 * {
 *    OptionsLong longopts[] = {
 *        {"foo", OPTIONS_NO_ARGUMENT,       'f'},
 *        {"bar", OPTIONS_REQUIRED_ARGUMENT, 'b'},
 *        {"opt", OPTIONS_OPTIONAL_ARGUMENT, 'o'},
 *        {0}
 *    };
 *    const char* optstr = "xy:z::123";
 *    printf("program -x -y ARG -z [ARG] -1 -2 -3 --foo --bar ARG --opt [ARG] [ARGUMENTS]\n");
 *    int c;
 *    Options opt = OptionsInit();
 *    while ((c = OptionsGet(&opt, argc, argv, optstr, longopts)) != -1) {
 *        switch (c) {
 *            case '?': printf("error: unknown option: %s\n", opt.optstr); break;
 *            case ':': printf("error: missing argument for %s (%c)\n", opt.optstr, opt.opt); break;
 *            default:  printf("option: %c [%s]\n", opt.opt, opt.arg ? opt.arg : ""); break;
 *        }
 *    }
 *    printf("\nNon-option arguments:");
 *    for (int i = opt.ind; i < argc; ++i)
 *        printf(" %s", argv[i]);
 *    putchar('\n');
 *    return 0;
 * }
 */

typedef enum OptionsType {
    OPTIONS_NO_ARGUMENT,
    OPTIONS_REQUIRED_ARGUMENT,
    OPTIONS_OPTIONAL_ARGUMENT,
} OptionsType;

typedef struct OptionsLong {
    const char *name;
    OptionsType type;
    int val;
} OptionsLong;

typedef struct Options Options;

// Initialize
static inline Options OptionsInit(void);

/* @param opt   output; must be initialized to OptionsInit() on first call
 * @return      ASCII val for a short option; longopt.val for a long option;
 *              -1 if argv[] is fully processed; '?' for an unknown option or
 *              an ambiguous long option; ':' if an option argument is missing
 */
int OptionsGet(Options *opt, int argc, char *argv[],
                const char *shortopts, const OptionsLong *longopts);


//--- implementation -----------------------------------------------------------

struct Options {
    int ind;            /* equivalent to optind */
    int opt;            /* equivalent to optopt */
    const char *optstr; /* points to the option string */
    const char *arg;    /* equivalent to optarg */
    int _i, _pos, _nargs;
    char _optstr[4];
};

static inline Options OptionsInit(void) {
     Options opt = {1, 0, NULL, NULL, 1, 0, 0, {'-', '?', '\0'}};
     return opt;
}

/* MIT License
 *
 * Copyright (c) 2023 Tyge Løvset
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
/*
Inspired by https://attractivechaos.wordpress.com/2018/08/31/a-survey-of-argument-parsing-libraries-in-c-c
Fixed major bugs with optional arguments (both long and short).
Added arg->optstr output field, more consistent API.
*/

#endif //KONPU_OPTIONS_H
