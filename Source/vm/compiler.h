/*
  ==============================================================================

    compiler.h
    Created: 18 Dec 2022 12:02:01am
    Author:  pcker

  ==============================================================================
*/

#ifndef btlang_compiler_h
#define btlang_compiler_h

#include "object.h"
#include "vm.h"

bool compile(const char* source, Chunk* chunk);

#endif
