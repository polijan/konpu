// Placeholder (You can copy this file when creating a new test file)

#include "test.h"

// Test: ...
static const char *testPlaceHolder1(void)
{
   TestAssert("true", true);
   return 0;
}

// Test: ...
static const char *testPlaceHolder2(void)
{
   TestAssert("true", true);
   return 0;
}

// Run the tests
TEST_BEGIN
   TestRun(testPlaceHolder1);
   TestRun(testPlaceHolder2);
TEST_END
