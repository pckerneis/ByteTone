/*
  ==============================================================================

    Token.h
    Created: 11 Nov 2020 2:10:20pm
    Author:  Pierre

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

enum class TokenType {
    EOF_,

    T,

    NUMBER,

    BITWISE_COMPLEMENT,
    LEFT_PAREN,
    RIGHT_PAREN,
    DOT,
    MINUS,
    PLUS,
    SLASH,
    STAR,
    MODULUS,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,
    EQUAL_EQUAL,
    NOT_EQUAL,
    BIT_SHIFT_LEFT,
    BIT_SHIFT_RIGHT,
    BITWISE_XOR,
    BITWISE_OR,
    BITWISE_AND,
    CONDITIONAL,
    COLON,

    // Not part of restricted C set from source paper
    EQUAL,
    BANG,

    // Strings
    // Brackets (for char access)

    // Types reserved words
    // Primitive types for type casting

    // Keywords
};

struct Token {
    Token() : type(TokenType::EOF_), literal(0), position(0), lexeme("")
    {
    }

    Token(TokenType tokenType, int lit, int pos, juce::String lex)
        : type(tokenType), literal(lit), position(pos), lexeme(lex)
    {
    }

    TokenType type;
    int literal;
    int position;
    juce::String lexeme;
};
