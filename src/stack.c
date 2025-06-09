#include "stack.h"

// Static checks of memory addresses
#include "memory_.h"
MEMORY_STATIC_ASSERT(STACK_VAR_ADDRESS, var);
MEMORY_STATIC_ASSERT(STACK_TYPE_ADDRESS, Type);
MEMORY_STATIC_ASSERT(STACK_SIZE_ADDRESS, int16_t);

// void StackInit(void)
// { memset(STACK_MEMORY, 0, STACK_MEMORY_OVERFLOW - STACK_MEMORY); }


int StackPush(var v, Type type)
{
   if (STACK_SIZE >= (STACK_CAPACITY - 1)) return KONPU_ERROR_STACK_OVERFLOW;
   STACK_VAR_ARRAY[STACK_SIZE]  = v;
   STACK_TYPE_ARRAY[STACK_SIZE] = type;
   STACK_SIZE++;
   return 0;
}
/*
int StackPush(var v, Type type)
{
   if (STACK_SIZE >= (STACK_CAPACITY - 1)) return KONPU_ERROR_STACK_OVERFLOW;
   STACK_SIZE++;
   StackVar(1) = v;
   StackType(1) = type;
   return 0;
}
*/

int StackPop(void)
{
   if (STACK_SIZE <= 0) return KONPU_ERROR_STACK_NOT_ENOUGH_ARGUMENTS;
   STACK_SIZE--;
   VarDrop(STACK_VAR_ARRAY[STACK_SIZE], STACK_TYPE_ARRAY[STACK_SIZE]);
   return 0;
}

void StackClear(void)
{
   for (int i = 0; i < STACK_SIZE; i++) {
      VarDrop(STACK_VAR_ARRAY[i], STACK_TYPE_ARRAY[i]);
   }
   STACK_SIZE = 0;
}


int StackSwap(void)
{
   if (STACK_SIZE <= 2) return KONPU_ERROR_STACK_NOT_ENOUGH_ARGUMENTS;

   var tmp_var   = StackVar(2);
   StackVar(2)   = StackVar(1);
   StackVar(1)   = tmp_var;

   Type tmp_type = StackType(2);
   StackType(2)  = StackType(1);
   StackType(1)  = tmp_type;

   return 0;
}

#include <stdio.h>
void StackPrint_(void)
{
   /*
   for (int i = 0; i < STACK_SIZE; i++) {
       printf("\x1b[32m%4d\x1b[36m:\x1b[0m ", StackIndex(i));
       VarPrintTTY(STACK_VAR_ARRAY[i], STACK_TYPE_ARRAY[i]);
       putchar('\n');
   }
   */

   for (int i = 0; i < STACK_SIZE; i++) {
      printf("\x1b[32m%4d\x1b[36m:\x1b[0m ", STACK_SIZE - i);
      VarPrintTTY(STACK_VAR_ARRAY[i], STACK_TYPE_ARRAY[i]);
      putchar('\n');
   }
}
