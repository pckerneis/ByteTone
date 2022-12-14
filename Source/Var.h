/*
  ==============================================================================

    Var.h
    Created: 13 Dec 2022 1:52:40pm
    Author:  pcker

  ==============================================================================
*/

#pragma once

#include "Environment.h"

class Var
{
public:
    struct Args
    {
        Args(std::vector<Var>& args, int numArgs, Environment* environment) noexcept
            : arguments(args), numArguments(numArgs), env(environment)
        {
        }

        const std::vector<Var> arguments;
        const int numArguments;
        const Environment* env;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Args)
    };

    using NativeFunction = std::function<Var(const Args*)>;

    enum class Type {
        BOOL, INT, DOUBLE, UNDEFINED, FUNCTION
    };

    bool operator ==(const Var& other) const
    {
        if (type != other.type)
        {
            return false;
        }

        if (isFunction())
        {
            // TODO
            return false;
        }

        return coercedToBool() == other.coercedToBool()
            && coercedToDouble() == other.coercedToDouble()
            && coercedToInt() == other.coercedToInt();
    }

    Var() : type(Type::UNDEFINED), boolValue(false), intValue(0), doubleValue(0)
    {

    }

    Var(Var& other) : type(other.type), boolValue(other.boolValue), intValue(other.intValue), doubleValue(other.doubleValue), funcValue(other.funcValue)
    {

    }

    Var(const Var& other) : type(other.type), boolValue(other.boolValue), intValue(other.intValue), doubleValue(other.doubleValue), funcValue(other.funcValue)
    {

    }

    Var(bool value) : type(Type::BOOL), boolValue(value), intValue(0), doubleValue(0.0)
    {

    }

    Var(int value) : type(Type::INT), boolValue(false), intValue(value), doubleValue(0.0)
    {

    }

    Var(double value): type(Type::DOUBLE), boolValue(false), intValue(0), doubleValue(value)
    {
        
    }

    Var(NativeFunction function) : type(Type::FUNCTION), boolValue(false), intValue(0), doubleValue(0.0), funcValue(function)
    {
    }

    bool isUndefined() const { return type == Type::UNDEFINED; }
    bool isBool() const { return type == Type::BOOL; }
    bool isInt() const { return type == Type::INT; }
    bool isDouble() const { return type == Type::DOUBLE; }
    bool isFunction() const { return type == Type::FUNCTION; }

    bool getBoolValue() const { return boolValue; }
    int getIntValue() const { return intValue; }
    double getDoubleValue() const { return doubleValue; }

    Var call(const Args& args) const
    {
        if (!isFunction())
        {
            return Var();
        }

        return funcValue(&args);
    }

    bool coercedToBool() const
    {
        if (isUndefined()) return false;
        if (isFunction()) return false; // TODO
        if (isBool()) return getBoolValue();
        if (isInt()) return bool(getIntValue());
        return bool(getDoubleValue());
    }

    int coercedToInt() const
    {
        if (isUndefined() || isFunction()) return 0;
        return isInt() ? getIntValue() : (isDouble() ? (int)getDoubleValue() : (int)getBoolValue());
    }

    double coercedToDouble() const
    {
        if (isUndefined() || isFunction()) return 0.0;
        return isDouble() ? getDoubleValue() : (isInt() ? (double)getIntValue() : (double)getBoolValue());
    }

private:
    Type type;
    bool boolValue;
    int intValue;
    double doubleValue;
    NativeFunction funcValue;

    JUCE_LEAK_DETECTOR(Var)
};