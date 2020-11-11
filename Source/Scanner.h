/*
  ==============================================================================

    Scanner.h
    Created: 11 Nov 2020 11:07:45am
    Author:  Pierre

  ==============================================================================
*/

#pragma once

#include "Token.h"

class Scanner {
public:
    Scanner(juce::String src);

    juce::Array<Token> scanTokens();

private:
    void scanToken();
    void number();
    void addToken(TokenType type);

    char advance();
    bool match(char expected);
    char peek();
    char peekNext();

    bool isDigit(char c);
    bool isAtEnd();

    juce::Array<Token> tokens;
    juce::String source;

    int start = 0;
    int current = 0;
    int line = 0;
};