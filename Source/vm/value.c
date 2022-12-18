/*
  ==============================================================================

    value.c
    Created: 17 Dec 2022 11:07:09pm
    Author:  pcker

  ==============================================================================
*/

#include <stdio.h>

#include "memory.h"
#include "value.h"

bool valuesEqual(BtlValue a, BtlValue b)
{
  if (a.type != b.type) return false;
  switch (a.type)
  {
    case VAL_BOOL:   return AS_BOOL(a) == AS_BOOL(b);
    case VAL_NULL:    return true;
    case VAL_NUMBER: return AS_NUMBER(a) == AS_NUMBER(b);
    default:         return false; // Unreachable.
  }
}

void initValueArray(ValueArray* array)
{
  array->values = NULL;
  array->capacity = 0;
  array->count = 0;
}

void writeValueArray(ValueArray* array, BtlValue value)
{
  if (array->capacity < array->count + 1) {
    int oldCapacity = array->capacity;
    array->capacity = GROW_CAPACITY(oldCapacity);
    array->values = GROW_ARRAY(BtlValue, array->values,
                               oldCapacity, array->capacity);
  }

  array->values[array->count] = value;
  array->count++;
}

void freeValueArray(ValueArray* array)
{
  FREE_ARRAY(BtlValue, array->values, array->capacity);
  initValueArray(array);
}

void printValue(BtlValue value)
{
  switch (value.type) {
    case VAL_BOOL:
      printf(AS_BOOL(value) ? "true" : "false");
      break;
    case VAL_NULL: printf("null"); break;
    case VAL_NUMBER: printf("%g", AS_NUMBER(value)); break;
  }
}