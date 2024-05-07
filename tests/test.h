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

#ifndef  KONPU_TEST_H
#define  KONPU_TEST_H
#include <stdio.h>
#include <stdlib.h>

#define TestAssert(message, test)              \
        do {                                   \
           if (!(test))                        \
           return message;                     \
        } while (0)

#define TestRun(testFunctionName)              \
        do {                                   \
           char *msg = testFunctionName();     \
           TestRun_++;                         \
           if (msg) return msg;                \
        } while (0)


#define TEST_BEGIN                             \
   int TestRun_ = 0;                           \
   static char* TestRunAll_(void) {

#define TEST_END                               \
      return NULL;                             \
   }                                           \
                                               \
   int main(int argc, char* argv[])            \
   {  (void)argc; (void)argv;                  \
                                               \
      char* result = TestRunAll_();            \
      if (result != NULL)                      \
         printf("\033[31m%s\033[m\n", result); \
      else                                     \
         printf("\033[32mOK\033[m ");          \
      printf("(tests run: %d)\n", TestRun_);   \
      if (result == NULL)                      \
         return EXIT_SUCCESS;                  \
      return EXIT_FAILURE;                     \
   }

//----------------------------------------------

#include <konpu.h>

#endif //KONPU_TEST_H
