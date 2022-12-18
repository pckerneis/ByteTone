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

typedef struct Obj Obj;
typedef struct ObjString ObjString;

typedef enum
{
  VAL_BOOL,
  VAL_NULL, 
  VAL_NUMBER,
  VAL_OBJ,
} ValueType;

typedef struct
{
  ValueType type;
  union
  {
    bool boolean;
    double number;
    Obj* obj;
  } as; 
} BtlValue;

#define IS_BOOL(value)    ((value).type == VAL_BOOL)
#define IS_NIL(value)     ((value).type == VAL_NULL)
#define IS_NUMBER(value)  ((value).type == VAL_NUMBER)
#define IS_OBJ(value)     ((value).type == VAL_OBJ)

#define AS_BOOL(value)    ((value).as.boolean)
#define AS_NUMBER(value)  ((value).as.number)
#define AS_OBJ(value)     ((value).as.obj)

#define BOOL_VAL(value)   ((BtlValue){VAL_BOOL, {.boolean = value}})
#define NULL_VAL          ((BtlValue){VAL_NULL, {.number = 0}})
#define NUMBER_VAL(value) ((BtlValue){VAL_NUMBER, {.number = value}})
#define OBJ_VAL(object)   ((BtlValue){VAL_OBJ, {.obj = (Obj*)object}})

typedef struct
{
  int capacity;
  int count;
  BtlValue* values;
} ValueArray;

bool valuesEqual(BtlValue a, BtlValue b);
void initValueArray(ValueArray* array);
void writeValueArray(ValueArray* array, BtlValue value);
void freeValueArray(ValueArray* array);
void printValue(BtlValue value);

#endif