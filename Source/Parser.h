/*
  ==============================================================================

    Parser.h
    Created: 11 Nov 2020 11:07:33am
    Author:  Pierre

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Expr.h"
#include "Scanner.h"
#include "ParseError.h"

class Parser
{
public:
    Parser(juce::Array<Token> toks) : tokens(toks) {}

    Expr* parse();

private:
    Expr* expression();
    Expr* assignment();
    Expr* ternaryConditional();
    Expr* bitwiseOr();
    Expr* bitwiseXor();
    Expr* bitwiseAnd();
    Expr* equality();
    Expr* comparison();
    Expr* bitshift();
    Expr* term();
    Expr* factor();
    Expr* unary();
    Expr* call();
    Expr* finishCall(Expr* callee);
    Expr* primary();

    bool match(juce::Array<TokenType> types);
    bool check(TokenType type);
    Token consume(TokenType type, juce::String message);
    ParseError error(Token token, juce::String message);
    Token advance();
    bool isAtEnd();
    Token peek();
    Token previous();

    juce::Array<Token> tokens;
    int current = 0;
};
