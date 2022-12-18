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
  BtlValue stack[STACK_MAX];
  BtlValue* stackTop;
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
void push(BtlValue value);
BtlValue pop();

static InterpretResult run();

#endif

