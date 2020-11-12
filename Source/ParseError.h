/*
  ==============================================================================

    ParseError.h
    Created: 12 Nov 2020 6:24:44pm
    Author:  Pierre

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Token.h"

class ParseError : std::exception
{
public:
    ParseError(juce::String msg, Token tok) : message(msg), token(tok)
    {
    }

    juce::String getMessage()
    {
        juce::String pos(token.type == TokenType::EOF_ ? "EOF" : juce::String(token.position));
        return message + " at [" + pos + "]";
    }

private:
    juce::String message;
    Token token;
};
