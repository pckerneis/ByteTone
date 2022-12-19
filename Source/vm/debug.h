/*
  ==============================================================================

    debug.h
    Created: 17 Dec 2022 4:33:08pm
    Author:  pcker

  ==============================================================================
*/

#ifndef btlang_debug_h
#define btlang_debug_h

#include "chunk.h"

void disassembleChunk(Chunk* chunk, const char* name);
int disassembleInstruction(Chunk* chunk, int offset);

#endif