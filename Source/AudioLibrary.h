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

    static Var sine(const Args* a)
    {
        return Var(std::sin(getDouble(a, 0) * juce::MathConstants<double>::twoPi / (double)a->env->tickRate));
    }

    static Var dB(const Args* a) { return juce::Decibels::decibelsToGain(getDouble(a, 0)); }
};

class AudioLibraryTest : juce::UnitTest
{
public:
    AudioLibraryTest() : UnitTest("AudioLibraryTest") {}

    void runTest() override
    {
        const double epsilon = 1e-6;

        beginTest("dB");

        std::vector<std::pair<double, double>> dbTestData {
            { 0.0,   1.0 },
            { -3.0,  0.707946 },
            { -6.0,  0.501187 },
            { -12.0, 0.251189 },
            { -15.0, 0.177828 },
            { -21.0, 0.0891251 }
        };

        for (auto data : dbTestData)
        {
            std::unique_ptr<Var::Args> args(argDouble(data.first, 1));
            double actual = AudioLibrary::dB(args.get()).getDoubleValue();
            double expected = data.second;
            expectLessThan(std::abs(expected - actual), epsilon, juce::String("Got ") << actual << " instead of " << expected << ".");
        }

        beginTest("sine");

        std::vector<std::pair<std::pair<int, double>, double>> sineTestData {
            {{ 1, 0.0  },  0.0 },
            {{ 1, 0.25 },  1.0 },
            {{ 1, 0.5  },  0.0 },
            {{ 1, 0.75 }, -1.0 },
            {{ 1, 1.0  },  0.0 },
            {{ 8000, 0.0 },     0.0 },
            {{ 8000, 2000.0 },  1.0 },
            {{ 8000, 4000.0 },  0.0 },
            {{ 8000, 6000.0 }, -1.0 },
            {{ 8000, 8000.0 },  0.0 }
        };

        for (auto data : sineTestData)
        {
            double tickRate = data.first.first;
            double t = data.first.second;
            std::unique_ptr<Var::Args> args(argDouble(t, tickRate));
            double expected = data.second;
            double actual = AudioLibrary::sine(args.get()).getDoubleValue();
            expectLessThan(std::abs(expected - actual), epsilon);
        }
    }

    Var::Args* argDouble(double value, int tickRate)
    {
        std::vector<Var> arguments;
        arguments.push_back(Var(value));
        return new Var::Args(arguments, arguments.size(), &Environment::withTickRate(tickRate));
    }
};

static AudioLibraryTest test;