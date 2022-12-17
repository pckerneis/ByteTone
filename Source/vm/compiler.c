/*
  ==============================================================================

    compiler.c
    Created: 18 Dec 2022 12:02:28am
    Author:  pcker

  ==============================================================================
*/

#include <stdio.h>

#include "common.h"
#include "compiler.h"
#include "scanner.h"

void compile(const char* source)
{
  initScanner(source);
  int line = -1;
  for (;;) {
    Token token = scanToken();
    if (token.line != line)
    {
      printf("%4d ", token.line);
      line = token.line;
    }
    else
    {
      printf("   | ");
    }
    printf("%2d '%.*s'\n", token.type, token.length, token.start); 

    if (token.type == TOKEN_EOF) break;
  }
}
