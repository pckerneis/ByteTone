/*
  ==============================================================================

    Scanner.cpp
    Created: 11 Nov 2020 11:07:45am
    Author:  Pierre

  ==============================================================================
*/

#include "Scanner.h"

Scanner::Scanner(juce::String src) : source(src) {}

juce::Array<Token> Scanner::scanTokens()
{
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }

    tokens.add(Token(TokenType::EOF_, 0, 0, ""));

    return tokens;
}

void Scanner::scanToken()
{
    char c = advance();

    switch (c)
    {
    case '(': addToken(TokenType::LEFT_PAREN); break;
    case ')': addToken(TokenType::RIGHT_PAREN); break;
    case '.': addToken(TokenType::DOT); break;
    case '-': addToken(TokenType::MINUS); break;
    case '+': addToken(TokenType::PLUS); break;
    case '*': addToken(TokenType::STAR); break;
    case '/': addToken(TokenType::SLASH); break;
    case '&': addToken(TokenType::BITWISE_AND); break;
    case '|': addToken(TokenType::BITWISE_OR); break;
    case '^': addToken(TokenType::BITWISE_XOR); break;
    case '~': addToken(TokenType::BITWISE_COMPLEMENT); break;
    case '%': addToken(TokenType::MODULUS); break;

    case '!':
        addToken(match('=') ? TokenType::NOT_EQUAL : TokenType::BANG);
        break;
    case '=':
        addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
        break;
    case '<':
        if (match('<'))         addToken(TokenType::BIT_SHIFT_LEFT);
        else if (match('='))    addToken(TokenType::LESS_EQUAL);
        else                    addToken(TokenType::LESS);
        break;
    case '>':
        if (match('>'))         addToken(TokenType::BIT_SHIFT_RIGHT);
        else if (match('='))    addToken(TokenType::GREATER_EQUAL);
        else                    addToken(TokenType::GREATER);
        break;
    case ' ':
    case '\r':
    case '\t':
        // Ignore whitespace.
        break;
    case '\n':
        line++;
        break;
    case 't': addToken(TokenType::T); break;
    default:
        if (isDigit(c))
        {
            number();
        }
        else
        {
            // TODO Show some kind of error
        }
        break;
    }
}

char Scanner::advance()
{
    current++;
    return source[current - 1];
}

bool Scanner::match(char expected)
{
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;
    current++;
    return true;
}

char Scanner::peek()
{
    if (isAtEnd()) return '\0';
    return source[current];
}

char Scanner::peekNext()
{
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

bool Scanner::isDigit(char c)
{
    return c >= '0' && c <= '9';
}

void Scanner::number()
{
    while (isDigit(peek())) advance();

    if (peek() == '.' && isDigit(peekNext()))
    {
        advance();

        while (isDigit(peek())) advance();
    }

    juce::String text = source.substring(start, current);
    int intValue = std::stoi(text.toStdString());
    tokens.add(Token(TokenType::NUMBER, intValue, start, text));
}

void Scanner::addToken(TokenType type)
{
    juce::String text = source.substring(start, current);
    tokens.add(Token(type, 0, start, text));
}

bool Scanner::isAtEnd()
{
    return current >= source.length();
}
