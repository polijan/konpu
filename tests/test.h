// Minimal header-only test "framework"
// (inspired from this article: https://jera.com/techinfo/jtns/jtn002)
//
// To create an automatic test file:
//
// 1. Define your test cases as functions like this: char *testCase(void);
//    Each test function would use the TestAssert("message", condition) macro,
//    and return 0 at the end if all went well.
//
//    #include "test.h"
//
//    static char *testFoo(void) {
//       ...
//       TestAssert("...", ...);
//       ...
//       TestAssert("...", ...);
//       return 0;
//    }
//
//    static char *testBar(void) {
//       ...
//       TestAssert("...", ...);
//       return 0;
//    }
//
// 2. At the end of the file, inside a TEST_BEGIN ... TEST_END block,
//    invoke the test functions you have defined:
//
//    TEST_BEGIN
//       TestRun(testFoo);
//       TestRun(testBar);
//    TEST_END
//
//
// Tests normally wouldn't print anything, but you can: #define TEST_TRACE
// *before* including "test.h" to make the macro TestTrace(...) behave like
// `fprintf(stderr, ...)` (difference: it automatically outputs a new line).
// (if TEST_TRACE isn't defined, the macro expands to nothing)


#ifndef  KONPU_TEST_H
#define  KONPU_TEST_H

#define TestAssert(message, test)                             \
        do {                                                  \
            if (!(test))                                      \
               return "TestAssert failed at line "            \
                      UTIL_STRINGIFY(__LINE__) ": " message;  \
        } while (0)

#define TestRun(testFunctionName)            \
        do {                                 \
            char *msg = testFunctionName();  \
            TestRun_++;                      \
            if (msg) return msg;             \
        } while (0)

#define TEST_BEGIN   static char* TestRunAll_(void) {

#define TEST_END     return NULL; }

#ifdef TEST_TRACE
#   define TestTrace(...)                                        \
           do {                                                  \
               fprintf(stderr, "\033[34m%d:\033[m ", __LINE__);  \
               fprintf(stderr, __VA_ARGS__);                     \
               putc('\n', stderr);                               \
           } while (0)
#else
#   define TestTrace(...)
#endif


// Numbers of test run
int TestRun_ = 0;

static char* TestRunAll_(void);

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{  (void)argc; (void)argv;
#ifdef TEST_TRACE
   fprintf(stderr, "\033[34m(TestTrace enabled)\033[m\n");
#endif

   char* result = TestRunAll_();
   if (result != NULL)
      printf("\033[31m%s\033[m\n", result);
   else
      printf("\033[32mOK\033[m ");
   printf("(tests run: %d)\n", TestRun_);

   return (result == NULL)? EXIT_SUCCESS : EXIT_FAILURE;
}


// Include <konpu.h> + Extra headers we want all tests to have access to
#include <konpu.h>
#include <errno.h>     // macros reporting error conditions
#include <string.h>    // string handling + memory functions
#include <inttypes.h>  // format conversion of integer types

#endif //KONPU_TEST_H
