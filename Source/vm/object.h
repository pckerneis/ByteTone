/*
  ==============================================================================

    object.h
    Created: 18 Dec 2022 1:38:27pm
    Author:  pcker

  ==============================================================================
*/

#ifndef btlang_object_h
#define btlang_object_h

#include "common.h"
#include "value.h"

#define OBJ_TYPE(value)     (AS_OBJ(value)->type)

#define IS_STRING(value)    isObjType(value, OBJ_STRING)

#define AS_STRING(value)       ((ObjString*)AS_OBJ(value))
#define AS_CSTRING(value)      (((ObjString*)AS_OBJ(value))->chars)

typedef enum
{
  OBJ_STRING,
} ObjType;

struct Obj
{
  ObjType type;
  struct Obj* next;
};

struct ObjString
{
  Obj obj;
  int length;
  char* chars;
  uint32_t hash;
};

ObjString* takeString(char* chars, int length);
ObjString* copyString(const char* chars, int length);
void printObject(BtlValue value);

static inline bool isObjType(BtlValue value, ObjType type)
{
  return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

#endif