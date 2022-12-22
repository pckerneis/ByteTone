/*
  ==============================================================================

    vm.h
    Created: 17 Dec 2022 10:58:12pm
    Author:  pcker

  ==============================================================================
*/

#ifndef btlang_vm_h
#define btlang_vm_h

#include "object.h"
#include "table.h"
#include "value.h"

#define FRAMES_MAX 64
#define STACK_MAX (FRAMES_MAX * UINT8_COUNT)

typedef struct
{
  ObjFunction* function;
  uint8_t* ip;
  BtlValue* slots;
} CallFrame;

typedef struct
{
  CallFrame frames[FRAMES_MAX];
  int frameCount;
  BtlValue stack[STACK_MAX];
  BtlValue* stackTop;
  Table globals;
  Table strings;
  Obj* objects;
} VM;

typedef enum
{
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
} InterpretResult;

extern VM vm;

void initVM();
void freeVM();
static void resetStack();
InterpretResult interpret(const char* source);
double interpretDouble(const char* source);
void push(BtlValue value);
BtlValue pop();

static BtlValue runExpression();
static InterpretResult run(bool evaluateExpr);

#endif

