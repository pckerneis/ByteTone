/*
  ==============================================================================

    Var.h
    Created: 13 Dec 2022 1:52:40pm
    Author:  pcker

  ==============================================================================
*/

#pragma once

class Var
{
public:
    enum class Type {
        BOOL, INT, DOUBLE, UNDEFINED
    };

    bool operator ==(const Var& other) const
    {
        if (type != other.type)
        {
            return false;
        }

        return coercedToBool() == other.coercedToBool()
            && coercedToDouble() == other.coercedToDouble()
            && coercedToInt() == other.coercedToInt();
    }

    Var() : type(Type::UNDEFINED), boolValue(false), intValue(0), doubleValue(0)
    {

    }

    Var(const Var& other) : type(other.type), boolValue(other.boolValue), intValue(other.intValue), doubleValue(other.doubleValue)
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

    bool isUndefined() const { return type == Type::UNDEFINED; }
    bool isBool() const { return type == Type::BOOL; }
    bool isInt() const { return type == Type::INT; }
    bool isDouble() const { return type == Type::DOUBLE; }

    bool getBoolValue() const { return boolValue; }
    int getIntValue() const { return intValue; }
    double getDoubleValue() const { return doubleValue; }


    bool coercedToBool() const {
        if (isUndefined()) return false;
        if (isBool()) return getBoolValue();
        if (isInt()) return bool(getIntValue());
        return bool(getDoubleValue());
    }

    int coercedToInt() const {
        if (isUndefined()) return 0;
        return isInt() ? getIntValue() : (isDouble() ? (int)getDoubleValue() : (int)getBoolValue());
    }

    double coercedToDouble() const {
        if (isUndefined()) return 0.0;
        return isDouble() ? getDoubleValue() : (isInt() ? (double)getIntValue() : (double)getBoolValue());
    }

private:
    Type type;
    bool boolValue;
    int intValue;
    double doubleValue;

    JUCE_LEAK_DETECTOR(Var)
};