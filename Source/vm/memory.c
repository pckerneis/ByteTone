/*
  ==============================================================================

    memory.c
    Created: 17 Dec 2022 4:29:04pm
    Author:  pcker

  ==============================================================================
*/

#include <stdlib.h>
#include "memory.h"

void* reallocate(void* pointer, size_t oldSize, size_t newSize)
{
  if (newSize == 0)
  {
    free(pointer);
    return NULL;
  }

  void* result = realloc(pointer, newSize);
  
  if (result == NULL) exit(1); // OOM

  return result;
}