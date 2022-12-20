/*
  ==============================================================================

    scanner.h
    Created: 18 Dec 2022 12:03:39am
    Author:  pcker

  ==============================================================================
*/

#ifndef btlang_scanner_h
#define btlang_scanner_h

typedef enum
{
  // Single-character tokens.
  TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
  TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
  TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS, TOKEN_PLUS,
  TOKEN_SEMICOLON, TOKEN_SLASH, TOKEN_STAR,
  // One or two character tokens.
  TOKEN_BANG, TOKEN_BANG_EQUAL,
  TOKEN_EQUAL, TOKEN_EQUAL_EQUAL,
  TOKEN_GREATER, TOKEN_GREATER_EQUAL,
  TOKEN_LESS, TOKEN_LESS_EQUAL,
  TOKEN_LOGICAL_AND, TOKEN_LOGICAL_OR,
  TOKEN_BITWISE_AND, TOKEN_BITWISE_OR,
  // Literals.
  TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER,
  // Keywords.
  TOKEN_FALSE, TOKEN_NULL, TOKEN_TRUE,

  // clox keywords
  TOKEN_PRINT, TOKEN_VAR, TOKEN_FUN, TOKEN_RETURN,

  TOKEN_ERROR, TOKEN_EOF
} TokenType;

typedef struct
{
  TokenType type;
  const char* start;
  int length;
  int line;
} BtlToken;

void initScanner(const char* source);
BtlToken scanToken();

#endif