/*
  ==============================================================================

    compiler.c
    Created: 18 Dec 2022 12:02:28am
    Author:  pcker

  ==============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "compiler.h"
#include "btl_scanner.h"

#ifdef DEBUG_PRINT_CODE
#include "debug.h"
#endif

typedef struct
{
  BtlToken current;
  BtlToken previous;
  bool hadError;
  bool panicMode;
} Parser;

typedef enum
{
  PREC_NONE,
  PREC_ASSIGNMENT,  // =
  PREC_OR,          // ||
  PREC_AND,         // &&
  PREC_BITWISE_OR,  // |
  PREC_BITWISE_XOR, // ^
  PREC_BITWISE_AND, // &
  PREC_EQUALITY,    // == !=
  PREC_COMPARISON,  // < > <= >=
  PREC_TERM,        // + -
  PREC_FACTOR,      // * /
  PREC_UNARY,       // ! - ~
  PREC_CALL,        // () []
  PREC_PRIMARY
} Precedence;

typedef void (*ParseFn)(bool canAssign);

typedef struct
{
  ParseFn prefix;
  ParseFn infix;
  Precedence precedence;
} ParseRule;

typedef struct
{
  BtlToken name;
  int depth;
} Local;

typedef enum
{
  TYPE_FUNCTION,
  TYPE_SCRIPT
} FunctionType;

typedef struct Compiler
{
  struct Compiler* enclosing;
  ObjFunction* function;
  FunctionType type;

  Local locals[UINT8_COUNT];
  int localCount;
  int scopeDepth;
} Compiler;

Parser parser;
Compiler* current = NULL;

static Chunk* currentChunk()
{
  return &current->function->chunk;
}

static void errorAt(BtlToken* token, const char* message)
{
  if (parser.panicMode) return;

  parser.panicMode = true;

  fprintf(stderr, "[line %d] Error", token->line);

  if (token->type == TOKEN_EOF)
  {
    fprintf(stderr, " at end");
  }
  else if (token->type == TOKEN_ERROR)
  {
    // Nothing.
  }
  else
  {
    fprintf(stderr, " at '%.*s'", token->length, token->start);
  }

  fprintf(stderr, ": %s\n", message);
  parser.hadError = true;
}

static void error(const char* message)
{
  errorAt(&parser.previous, message);
}

static void errorAtCurrent(const char* message)
{
  errorAt(&parser.current, message);
}

static void advance()
{
  parser.previous = parser.current;

  for (;;)
  {
    parser.current = scanToken();
    if (parser.current.type != TOKEN_ERROR) break;

    errorAtCurrent(parser.current.start);
  }
}

static void consume(TokenType type, const char* message)
{
  if (parser.current.type == type) {
    advance();
    return;
  }

  errorAtCurrent(message);
}

static bool check(TokenType type)
{
  return parser.current.type == type;
}

static bool match(TokenType type)
{
  if (!check(type)) return false;
  advance();
  return true;
}

static void emitByte(uint8_t byte)
{
  writeChunk(currentChunk(), byte, parser.previous.line);
}

static void emitBytes(uint8_t byte1, uint8_t byte2)
{
  emitByte(byte1);
  emitByte(byte2);
}

static int emitJump(uint8_t instruction)
{
  emitByte(instruction);
  emitByte(0xff);
  emitByte(0xff);
  return currentChunk()->count - 2;
}

static void emitReturn()
{
  emitByte(OP_NULL);
  emitByte(OP_RETURN);
}

static uint8_t makeConstant(BtlValue value)
{
  int constant = addConstant(currentChunk(), value);

  if (constant > UINT8_MAX)
  {
    error("Toom any constants in one chunk.");
    return 0;
  }

  return (uint8_t)constant;
}

static void emitConstant(BtlValue value)
{
  emitBytes(OP_CONSTANT, makeConstant(value));
}

static void patchJump(int offset)
{
  // -2 to adjust for the bytecode for the jump offset itself.
  int jump = currentChunk()->count - offset - 2;

  if (jump > UINT16_MAX) {
    error("Too much code to jump over.");
  }

  currentChunk()->code[offset] = (jump >> 8) & 0xff;
  currentChunk()->code[offset + 1] = jump & 0xff;
}

static void initCompiler(Compiler* compiler, FunctionType type)
{
  compiler->enclosing = current;
  compiler->function = NULL;
  compiler->type = type;
  compiler->localCount = 0;
  compiler->scopeDepth = 0;
  compiler->function = newFunction();
  current = compiler;

  if (type != TYPE_SCRIPT)
  {
    current->function->name = copyString(parser.previous.start,
                                         parser.previous.length);
  }

  Local* local = &current->locals[current->localCount++];
  local->depth = 0;
  local->name.start = "";
  local->name.length = 0;
}

static ObjFunction* endCompiler()
{
  emitReturn();
  ObjFunction* function = current->function;

#ifdef DEBUG_PRINT_CODE
  if (!parser.hadError)
  {
    disassembleChunk(currentChunk(), function->name != NULL
      ? function->name->chars : "<script>");
  }
#endif

  current = current->enclosing;
  return function;
}

static void beginScope()
{
  current->scopeDepth++;
}

static void endScope()
{
  current->scopeDepth--;

  while (current->localCount > 0 &&
         current->locals[current->localCount - 1].depth >
            current->scopeDepth)
  {
    emitByte(OP_POP); // TODO : add OP_POPN to pop a batch at once 
    current->localCount--;
  }
}

static void expression();
static void statement();
static void declaration();
static ParseRule* getRule(TokenType type);
static void parsePrecedence(Precedence precedence);

static void binary(bool canAssign)
{
  TokenType operatorType = parser.previous.type;
  ParseRule* rule = getRule(operatorType);
  parsePrecedence((Precedence)(rule->precedence + 1));

  switch (operatorType)
  {
    case TOKEN_BANG_EQUAL:    emitBytes(OP_EQUAL, OP_NOT); break;
    case TOKEN_EQUAL_EQUAL:   emitByte(OP_EQUAL); break;
    case TOKEN_GREATER:       emitByte(OP_GREATER); break;
    case TOKEN_GREATER_EQUAL: emitBytes(OP_LESS, OP_NOT); break;
    case TOKEN_LESS:          emitByte(OP_LESS); break;
    case TOKEN_LESS_EQUAL:    emitBytes(OP_GREATER, OP_NOT); break;
    case TOKEN_PLUS:          emitByte(OP_ADD); break;
    case TOKEN_MINUS:         emitByte(OP_SUBTRACT); break;
    case TOKEN_STAR:          emitByte(OP_MULTIPLY); break;
    case TOKEN_SLASH:         emitByte(OP_DIVIDE); break;
    case TOKEN_BITWISE_AND:   emitByte(OP_BITWISE_AND); break;
    case TOKEN_BITWISE_OR:    emitByte(OP_BITWISE_OR); break;
    case TOKEN_BITWISE_XOR:   emitByte(OP_BITWISE_XOR); break;
    default: return; // Unreachable.
  }
}

static uint8_t argumentList()
{
  uint8_t argCount = 0;

  if (! check(TOKEN_RIGHT_PAREN))
  {
    do
    {
      expression();
      if (argCount == 255)
      {
        error("Can't have more than 255 arguments.");
      }
      argCount++;
    } while (match(TOKEN_COMMA));
  }

  consume(TOKEN_RIGHT_PAREN, "Expect ')' after arguments.");
  return argCount;
}

static void call(bool canAssign)
{
  uint8_t argCount = argumentList();
  emitBytes(OP_CALL, argCount);
}

static void literal(bool canAssign)
{
  switch (parser.previous.type)
  {
    case TOKEN_FALSE: emitByte(OP_FALSE); break;
    case TOKEN_NULL: emitByte(OP_NULL); break;
    case TOKEN_TRUE: emitByte(OP_TRUE); break;
    default: return; // Unreachable.
  }
}

static void grouping(bool canAssign)
{
  expression();
  consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

static void number(bool canAssign)
{
  double value = strtod(parser.previous.start, NULL);
  emitConstant(NUMBER_VAL(value));
}

static void and_(bool canAssign)
{
  int endJump = emitJump(OP_JUMP_IF_FALSE);

  emitByte(OP_POP);
  parsePrecedence(PREC_AND);

  patchJump(endJump);
}

static void or_(bool canAssign)
{
  int elseJump = emitJump(OP_JUMP_IF_FALSE);
  int endJump = emitJump(OP_JUMP);

  patchJump(elseJump);
  emitByte(OP_POP);

  parsePrecedence(PREC_OR);
  patchJump(endJump);
}

static void string(bool canAssign)
{
  emitConstant(OBJ_VAL(copyString(parser.previous.start + 1,
                                  parser.previous.length - 2)));
}

static uint8_t identifierConstant(BtlToken* name)
{
  return makeConstant(OBJ_VAL(copyString(name->start,
                                         name->length)));
}

static bool identifiersEqual(BtlToken* a, BtlToken* b)
{
  if (a->length != b->length) return false;
  return memcmp(a->start, b->start, a->length) == 0;
}

static int resolveLocal(Compiler* compiler, BtlToken* name)
{
  for (int i = compiler->localCount - 1; i >= 0; i--)
  {
    Local* local = &compiler->locals[i];

    if (identifiersEqual(name, &local->name))
    {
      if (local->depth == -1)
      {
        error("Can't read local variable in its own initializer.");
      }

      return i;
    }
  }

  return -1;
}

static void addLocal(BtlToken name)
{
  if (current->localCount == UINT8_COUNT)
  {
    error("Too many local variables in function.");
    return;
  }

  Local* local = &current->locals[current->localCount++];
  local->name = name;
  local->depth = -1; // mark as uninitialized
  local->depth = current->scopeDepth;
}


static void declareVariable()
{
  if (current->scopeDepth == 0) return;

  BtlToken* name = &parser.previous;

  for (int i = current->localCount - 1; i >= 0; i--)
  {
    Local* local = &current->locals[i];
    if (local->depth != -1 && local->depth < current->scopeDepth)
    {
      break; 
    }

    if (identifiersEqual(name, &local->name))
    {
      error("Already a variable with this name in this scope.");
    }
  }

  addLocal(*name);
}

static void namedVariable(BtlToken name, bool canAssign)
{
  uint8_t getOp, setOp;
  int arg = resolveLocal(current, &name);

  if (arg != -1)
  {
    getOp = OP_GET_LOCAL;
    setOp = OP_SET_LOCAL;
  }
  else
  {
    arg = identifierConstant(&name);
    getOp = OP_GET_GLOBAL;
    setOp = OP_SET_GLOBAL;
  }

  if (canAssign && match(TOKEN_EQUAL))
  {
    expression();
    emitBytes(setOp, (uint8_t)arg);
  }
  else
  {
    emitBytes(getOp, (uint8_t)arg);
  }
}

static void variable(bool canAssign)
{
  namedVariable(parser.previous, canAssign);
}

static void unary(bool canAssign)
{
  TokenType operatorType = parser.previous.type;

  parsePrecedence(PREC_UNARY);

  switch(operatorType)
  {
    case TOKEN_BANG: emitByte(OP_NOT); break;
    case TOKEN_MINUS: emitByte(OP_NEGATE); break;
    case TOKEN_BITWISE_NOT: emitByte(OP_BITWISE_NOT); break;
    default: return; // Unreachable
  }
}

ParseRule rules[] = {
  [TOKEN_LEFT_PAREN]    = {grouping, call,   PREC_CALL},
  [TOKEN_RIGHT_PAREN]   = {NULL,     NULL,   PREC_NONE},
  [TOKEN_LEFT_BRACE]    = {NULL,     NULL,   PREC_NONE}, 
  [TOKEN_RIGHT_BRACE]   = {NULL,     NULL,   PREC_NONE},
  [TOKEN_COMMA]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_DOT]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_MINUS]         = {unary,    binary, PREC_TERM},
  [TOKEN_PLUS]          = {NULL,     binary, PREC_TERM},
  [TOKEN_SEMICOLON]     = {NULL,     NULL,   PREC_NONE},
  [TOKEN_SLASH]         = {NULL,     binary, PREC_FACTOR},
  [TOKEN_STAR]          = {NULL,     binary, PREC_FACTOR},
  [TOKEN_BANG]          = {unary,    NULL,   PREC_NONE},
  [TOKEN_BANG_EQUAL]    = {NULL,     binary, PREC_EQUALITY},
  [TOKEN_EQUAL]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_EQUAL_EQUAL]   = {NULL,     binary, PREC_EQUALITY},
  [TOKEN_GREATER]       = {NULL,     binary, PREC_COMPARISON},
  [TOKEN_GREATER_EQUAL] = {NULL,     binary, PREC_COMPARISON},
  [TOKEN_LESS]          = {NULL,     binary, PREC_COMPARISON},
  [TOKEN_LESS_EQUAL]    = {NULL,     binary, PREC_COMPARISON},
  [TOKEN_IDENTIFIER]    = {variable, NULL,   PREC_NONE},
  [TOKEN_STRING]        = {string,   NULL,   PREC_NONE},
  [TOKEN_NUMBER]        = {number,   NULL,   PREC_NONE},
  [TOKEN_LOGICAL_AND]   = {NULL,     and_,   PREC_AND},
  [TOKEN_LOGICAL_OR]    = {NULL,     or_,    PREC_OR},
  [TOKEN_BITWISE_AND]   = {NULL,     binary, PREC_BITWISE_AND},
  [TOKEN_BITWISE_OR]    = {NULL,     binary, PREC_BITWISE_OR},
  [TOKEN_BITWISE_XOR]   = {NULL,     binary, PREC_BITWISE_XOR},
  [TOKEN_BITWISE_NOT]   = {unary,    NULL,   PREC_UNARY},
  [TOKEN_FALSE]         = {literal,  NULL,   PREC_NONE},
  [TOKEN_NULL]          = {literal,  NULL,   PREC_NONE},
  [TOKEN_TRUE]          = {literal,  NULL,   PREC_NONE},
  [TOKEN_ERROR]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_EOF]           = {NULL,     NULL,   PREC_NONE},
};

static void parsePrecedence(Precedence precedence)
{
  advance();
  ParseFn prefixRule = getRule(parser.previous.type)->prefix;
  if (prefixRule == NULL)
  {
    error("Expect expression.");
    return;
  }

  bool canAssign = precedence <= PREC_ASSIGNMENT;
  prefixRule(canAssign);
  
  while (precedence <= getRule(parser.current.type)->precedence)
  {
    advance();
    ParseFn infixRule = getRule(parser.previous.type)->infix;
    infixRule(canAssign);
  }

  if (canAssign && match(TOKEN_EQUAL))
  {
    error("Invalid assignment target.");
  }
}

static uint8_t parseVariable(const char* errorMessage)
{
  consume(TOKEN_IDENTIFIER, errorMessage);

  declareVariable();
  if (current->scopeDepth > 0) return 0;

  return identifierConstant(&parser.previous);
}

static void markInitialized()
{
  if (current->scopeDepth == 0) return;
  current->locals[current->localCount - 1].depth =
      current->scopeDepth;
}


static void defineVariable(uint8_t global)
{
  if (current->scopeDepth > 0)
  {
    markInitialized();
    return;
  }

  emitBytes(OP_DEFINE_GLOBAL, global);
}

static ParseRule* getRule(TokenType type)
{
  return &rules[type];
}

static void expression()
{
  parsePrecedence(PREC_ASSIGNMENT);
}

static void block()
{
  while (!check(TOKEN_RIGHT_BRACE) && !check(TOKEN_EOF))
  {
    declaration();
  }

  consume(TOKEN_RIGHT_BRACE, "Expect '}' after block.");
}

static void function(FunctionType type)
{
  Compiler compiler;
  initCompiler(&compiler, type);
  beginScope(); 

  consume(TOKEN_LEFT_PAREN, "Expect '(' after function name.");

  if (!check(TOKEN_RIGHT_PAREN))
  {
    do
    {
      current->function->arity++;
      if (current->function->arity > 255)
      {
        errorAtCurrent("Can't have more than 255 parameters.");
      }

      uint8_t constant = parseVariable("Expect parameter name.");
      defineVariable(constant);
    } while (match(TOKEN_COMMA));
  }

  consume(TOKEN_RIGHT_PAREN, "Expect ')' after parameters.");
  consume(TOKEN_LEFT_BRACE, "Expect '{' before function body.");
  block();

  ObjFunction* function = endCompiler();
  emitBytes(OP_CONSTANT, makeConstant(OBJ_VAL(function)));
}

static void funDeclaration()
{
  uint8_t global = parseVariable("Expect function name.");
  markInitialized();
  function(TYPE_FUNCTION);
  defineVariable(global);
}

static void varDeclaration()
{
  uint8_t global = parseVariable("Expect variable name.");

  if (match(TOKEN_EQUAL))
  {
    expression();
  }
  else
  {
    emitByte(OP_NULL);
  }
  consume(TOKEN_SEMICOLON,
          "Expect ';' after variable declaration.");

  defineVariable(global);
}

static void expressionStatement()
{
  expression();
  consume(TOKEN_SEMICOLON, "Expect ';' after expression.");
  emitByte(OP_POP);
}

static void printStatement()
{
  expression();
  consume(TOKEN_SEMICOLON, "Expect ';' after value.");
  emitByte(OP_PRINT);
}

static void returnStatement()
{
  if (current->type == TYPE_SCRIPT)
  {
    error("Can't return from top-level code.");
  }

  if (match(TOKEN_SEMICOLON))
  {
    emitReturn();
  }
  else
  {
    expression();
    consume(TOKEN_SEMICOLON, "Expect ';' after return value.");
    emitByte(OP_RETURN);
  }
}

static void synchronize()
{
  parser.panicMode = false;

  while (parser.current.type != TOKEN_EOF)
  {
    if (parser.previous.type == TOKEN_SEMICOLON) return;

    switch (parser.current.type)
    {
      case TOKEN_PRINT:
        return;

      default:
        ; // Do nothing.
    }

    advance();
  }
}

static void declaration()
{
  if (match(TOKEN_FUN))
  {
    funDeclaration();
  }
  else if (match(TOKEN_VAR))
  {
    varDeclaration();
  }
  else
  {
    statement();
  }

  if (parser.panicMode) synchronize();
}

static void statement()
{
  if (match(TOKEN_RETURN))
  {
    returnStatement();
  }
  else if (match(TOKEN_PRINT))
  {
    expression();
    consume(TOKEN_SEMICOLON, "Expect ';' after value.");
    emitByte(OP_PRINT);
  }
  else if (match(TOKEN_LEFT_BRACE)) {
    beginScope();
    block();
    endScope();
  }
  else
  {
    expressionStatement();
  }
}

ObjFunction* compile(const char* source)
{
  initScanner(source);
  Compiler compiler;
  initCompiler(&compiler, TYPE_SCRIPT);

  parser.hadError = false;
  parser.panicMode = false;

  advance();

  while (!match(TOKEN_EOF))
  {
    declaration();
  }

  ObjFunction* function = endCompiler();
  return parser.hadError ? NULL : function;
}

ObjFunction* compileExpression(const char* source)
{
  initScanner(source);
  Compiler compiler;
  initCompiler(&compiler, TYPE_SCRIPT);

  parser.hadError = false;
  parser.panicMode = false;

  advance();

  expression();
  emitByte(OP_RETURN);

  ObjFunction* function = endCompiler();
  return parser.hadError ? NULL : function;
}