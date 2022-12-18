/*
  ==============================================================================

    table.h
    Created: 18 Dec 2022 2:40:24pm
    Author:  pcker

  ==============================================================================
*/

#ifndef btlang_table_h
#define btlang_table_h

#include "common.h"
#include "value.h"

typedef struct
{
  ObjString* key;
  BtlValue value;
} Entry;

typedef struct
{
  int count;
  int capacity;
  Entry* entries;
} Table;

void initTable(Table* table);
void freeTable(Table* table);
bool tableGet(Table* table, ObjString* key, BtlValue* value);
bool tableSet(Table* table, ObjString* key, BtlValue value);
bool tableDelete(Table* table, ObjString* key);
void tableAddAll(Table* from, Table* to);
ObjString* tableFindString(Table* table, const char* chars,
                           int length, uint32_t hash);
#endif