/*
  ==============================================================================

    vm.h
    Created: 17 Dec 2022 10:58:12pm
    Author:  pcker

  ==============================================================================
*/

#ifndef btlang_vm_h
#define btlang_vm_h

#include "chunk.h"
#include "value.h"

#define STACK_MAX 256

typedef struct
{
  Chunk* chunk;
  uint8_t* ip;
  Value stack[STACK_MAX];
  Value* stackTop;
} VM;

typedef enum
{
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
} InterpretResult;

void initVM();
void freeVM();
static void resetStack();
InterpretResult interpret(const char* source);
void push(Value value);
Value pop();

static InterpretResult run();

#endif

