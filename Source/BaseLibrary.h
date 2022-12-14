/*
  ==============================================================================

    BaseLibrary.h
    Created: 13 Dec 2022 10:36:34pm
    Author:  pcker

  ==============================================================================
*/

#pragma once

#include "Var.h"

class BaseLibrary
{
public:
    using Args = Var::Args;

    bool hasFunction(juce::String name) const
    {
        return registeredFunctions.contains(name);
    }

    Var getFunctionVar(juce::String name) const
    {
        return Var(registeredFunctions[name]);
    }

protected:
    void registerFunction(juce::String name, Var::NativeFunction function)
    {
        registeredFunctions.set(name, function);
    }

    static double getDouble(const Args* a, int pos) { return a->arguments[pos].coercedToDouble(); }

    juce::HashMap<juce::String, Var::NativeFunction> registeredFunctions;
};