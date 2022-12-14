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

    static Var sine(const Args& a) { return Var(std::sin(getDouble(a, 0) * juce::MathConstants<double>::twoPi / (double)a.env.tickRate)); }
    static Var dB(const Args& a) { return juce::Decibels::decibelsToGain(getDouble(a, 0)); }
};

class AudioLibraryTest : juce::UnitTest
{
public:
    AudioLibraryTest() : UnitTest("AudioLibraryTest") {}

    void runTest() override
    {

        beginTest("sine");

        Environment env1 = Environment::withTickRate(1);

        juce::Array<Var> arguments0;
        arguments0.add(Var(0.0));
        const Var::Args args0(arguments0.begin(), arguments0.size(), env1);
        expectEquals(AudioLibrary::sine(args0).getDoubleValue(), 0.0);

        double epsilon = 0.000001;

        juce::Array<Var> arguments1;
        arguments1.add(Var(1.0));
        const Var::Args args1(arguments1.begin(), arguments1.size(), env1);
        expectLessThan(std::abs(AudioLibrary::sine(args1).getDoubleValue()), epsilon);

        juce::Array<Var> arguments05;
        arguments05.add(Var(0.5));
        const Var::Args args05(arguments05.begin(), arguments05.size(), env1);
        expectLessThan(std::abs(AudioLibrary::sine(args05).getDoubleValue()), epsilon);

        juce::Array<Var> arguments025;
        arguments025.add(Var(0.25));
        const Var::Args args025(arguments025.begin(), arguments025.size(), env1);
        expectLessThan(1.0 - std::abs(AudioLibrary::sine(args025).getDoubleValue()), epsilon);

        juce::Array<Var> arguments075;
        arguments075.add(Var(0.75));
        const Var::Args args075(arguments075.begin(), arguments075.size(), env1);
        expectLessThan(1.0 - std::abs(AudioLibrary::sine(args075).getDoubleValue()), epsilon);
    }

    Var::Args argDouble(double value)
    {
        juce::Array<Var> arguments;
        arguments.add(Var(value));
        return Var::Args(arguments.begin(), arguments.size(), Environment::withTickRate(8000));
    }
};

static AudioLibraryTest test;