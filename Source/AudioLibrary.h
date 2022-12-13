/*
  ==============================================================================

    AudioLibrary.h
    Created: 13 Dec 2022 10:34:39pm
    Author:  pcker

  ==============================================================================
*/

#pragma once

#include "BaseLibrary.h"
#include "Environment.h"

class AudioLibrary : public BaseLibrary
{
    using Args = Var::Args;

public:
    AudioLibrary()
    {
        registerFunction("sine", sine);
        registerFunction("dB", dB);
    }

    // TODO : make sample-rate (tick rate) independant
    static Var sine(Args a) { return std::sin(getDouble(a, 0)); }
    static Var dB(Args a) { return juce::Decibels::decibelsToGain(getDouble(a, 0)); }
};