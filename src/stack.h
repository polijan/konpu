#ifndef  KONPU_STACK_H_
#define  KONPU_STACK_H_
#include "ram.h"
#include "error.h"
#include "var.h"

// Array containing the values in the Stack.
#define STACK_VAR_ARRAY   ((var*)(RAM + RAM_STACK_VAR))

// Array containing the types of the values in the Stack.
#define STACK_TYPE_ARRAY  ((Type*)(RAM + RAM_STACK_TYPE))

// Current size of the Stack (as an int16_t lvalue)
#define STACK_SIZE        (*(int16_t*)(RAM + RAM_STACK_SIZE))


// Returns the offset corresponding to the Waso stack level n.
// In Waso when we push new values to level 1, that corresponds to offset
// STACK_CAPACITY - 1.
#define StackIndex(n)      (STACK_CAPACITY - (n))

#define StackVar(level)     STACK_VAR_ARRAY[STACK_CAPACITY - (level)]
#define StackType(level)    STACK_TYPE_ARRAY[STACK_CAPACITY - (level)]

/*
// access with index like shown by the Waso interpreter (1(top) ... size)
#define StackIndex(n)  (STACK_SIZE - (n))
#define StackVar(n)    STACK_VAR_ARRAY[STACK_SIZE - (n)]
#define StackType(n)   STACK_TYPE_ARRAY[STACK_SIZE - (n)]
*/

// Push function (and macro shortcut for builtin types):
int     StackPush(var v, Type type);
// Macro like "functions" to reduce the verbosity of pushing builtin types:
#define StackPushInteger(i)   StackPush((var){.integer = (i)}, TYPE_INTEGER)
#define StackPushReal(d)      StackPush((var){.real    = (d)}, TYPE_REAL)
#define StackPushBits(u)      StackPush((var){.uint64  = (u)}, TYPE_BITS)
#define StackPushChar(c)      StackPush((var){.chr     = (c)}, TYPE_CHAR)
#define StackPushBoolean(b)   StackPush((var){.boolean = (b)}, TYPE_BOOLEAN)
#define StackPushAddress(a)   StackPush((var){.int32   = (a)}, TYPE_ADDRESS)
#define StackPushCString(s)   StackPush((var){.cstring = (s)}, TYPE_CSTRING)
#define StackPushPointer(p)   StackPush((var){.pointer = (p)}, TYPE_POINTER)
#define StackPushError(e)     StackPush((var){.error   = (e)}, TYPE_ERROR)
#define StackPushType(t)      StackPush((var){.type    = (t)}, TYPE_TYPE)
#define StackPushGlyph8(g)    StackPush((var){.glyph8  = (g)}, TYPE_GLYPH8)
#define StackPushGlyph16(g)   StackPush((var){.glyph16 = (g)}, TYPE_GLYPH16)
#define StackPushGlyph32(g)   StackPush((var){.glyph32 = (g)}, TYPE_GLYPH32)
#define StackPushGlyph64(g)   StackPush((var){.glyph64 = (g)}, TYPE_GLYPH64)


// Print the Stack
void StackPrint_(void);


int     StackPop(void);
void    StackClear(void);

int     StackSwap(void);
#define StackDup()     StackPush(StackVar(1), StackType(1))




// Inline Implementation


// UNSAFE Inline version of some operations (this does not check any error
// conditions on the stack) that simply return void. You probably DO NOT
// want to use those

static inline void StackPop_(void)
{  assert(STACK_SIZE >= 0);
   STACK_SIZE--;
   VarDrop(STACK_VAR_ARRAY[STACK_SIZE], STACK_TYPE_ARRAY[STACK_SIZE]);
}

#define StackClear_()   (void)StackClear()

static inline void StackSwap_(void)
{  assert(STACK_SIZE >= 2);

   var  tmp_var = STACK_VAR_ARRAY[STACK_SIZE - 2];
   STACK_VAR_ARRAY[STACK_SIZE - 2] = STACK_VAR_ARRAY[STACK_SIZE - 1];
   STACK_VAR_ARRAY[STACK_SIZE - 1] = tmp_var;

   Type tmp_type = STACK_TYPE_ARRAY[STACK_SIZE - 2];
   STACK_TYPE_ARRAY[STACK_SIZE - 2] = STACK_TYPE_ARRAY[STACK_SIZE - 1];
   STACK_TYPE_ARRAY[STACK_SIZE - 1] = tmp_type;
}

#endif //include guard
