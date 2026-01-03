#ifndef  KONPU_ERROR_H_
#define  KONPU_ERROR_H_
#include "c.h"

typedef int Error;

// Return a string describing the given type of error
const char *ErrorMessage(Error error);

// Builtin error types
typedef enum ErrorBuiltin {
   KONPU_ERROR_NONE    = 0,
   KONPU_ERROR_FAILURE = 1,

   // An error that comes from a potential backend Konpu may use
   KONPU_ERROR_PLATFORM_1,
   KONPU_ERROR_PLATFORM_2,
   KONPU_ERROR_PLATFORM_3,
   KONPU_ERROR_PLATFORM_4,
   KONPU_ERROR_PLATFORM_5,
   KONPU_ERROR_PLATFORM_6,
   KONPU_ERROR_PLATFORM_7,
   KONPU_ERROR_PLATFORM_8,
   KONPU_ERROR_PLATFORM_9,
   KONPU_ERROR_PLATFORM_A, // typically for POSIX error in the dlopen API
   KONPU_ERROR_PLATFORM_B,
   KONPU_ERROR_PLATFORM_C, // typically for system errors from C via errno
   KONPU_ERROR_PLATFORM_D,
   KONPU_ERROR_PLATFORM_E,
   KONPU_ERROR_PLATFORM_F,

   // typically reserved for C and POSIX errors

   // Stack Operation Errors
   KONPU_ERROR_STACK_NOT_ENOUGH_ARGUMENTS,
   KONPU_ERROR_STACK_OVERFLOW            ,

   // Heap Errors
   KONPU_ERROR_HEAP_ALLOCATION_FAILURE,

   // This is the counter (not an error)
   KONPU_ERROR_BUILTIN_COUNT_
} ErrorBuiltin;

#endif //include guard
