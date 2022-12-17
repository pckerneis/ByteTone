/*
  ==============================================================================

    value.h
    Created: 17 Dec 2022 11:07:21pm
    Author:  pcker

  ==============================================================================
*/

#ifndef btlang_value_h
#define btlang_value_h

#include "common.h"

typedef double Value;

typedef struct
{
  int capacity;
  int count;
  Value* values;
} ValueArray;

void initValueArray(ValueArray* array);
void writeValueArray(ValueArray* array, Value value);
void freeValueArray(ValueArray* array);
void printValue(Value value);

#endif