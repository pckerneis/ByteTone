/*
  ==============================================================================

    MathLibrary.h
    Created: 13 Dec 2022 10:34:48pm
    Author:  pcker

  ==============================================================================
*/

#pragma once

#include "BaseLibrary.h"

class MathLibrary : public BaseLibrary
{
    using Args = Var::Args;

public:
    MathLibrary()
    {
        registerFunction("sin", sin);
        registerFunction("asin", asin);
        registerFunction("sinh", sinh);
        registerFunction("asinh", asinh);
        registerFunction("cos", cos);
        registerFunction("acos", acos);
        registerFunction("tan", tan);
        registerFunction("atan", atan);
        registerFunction("tanh", tanh);
        registerFunction("atanh", atanh);
        registerFunction("ceil", ceil);
        registerFunction("floor", floor);
        registerFunction("round", round);
        registerFunction("max", max);
        registerFunction("min", min);
        registerFunction("sign", sign);
        registerFunction("log", log);
        registerFunction("log10", log10);
        registerFunction("exp", exp);
        registerFunction("pow", pow);
        registerFunction("sqrt", sqrt);
        registerFunction("rand", rand);
    }

    static Var sin(Args a) { return std::sin(getDouble(a, 0)); }
    static Var asin(Args a) { return std::asin(getDouble(a, 0)); }
    static Var sinh(Args a) { return std::sinh(getDouble(a, 0)); }
    static Var asinh(Args a) { return std::asinh(getDouble(a, 0)); }
    static Var cos(Args a) { return std::cos(getDouble(a, 0)); }
    static Var acos(Args a) { return std::acos(getDouble(a, 0)); }
    static Var cosh(Args a) { return std::cosh(getDouble(a, 0)); }
    static Var acosh(Args a) { return std::acosh(getDouble(a, 0)); }
    static Var tan(Args a) { return std::tan(getDouble(a, 0)); }
    static Var atan(Args a) { return std::atan(getDouble(a, 0)); }
    static Var tanh(Args a) { return std::tanh(getDouble(a, 0)); }
    static Var atanh(Args a) { return std::atanh(getDouble(a, 0)); }
    static Var ceil(Args a) { return std::ceil(getDouble(a, 0)); }
    static Var floor(Args a) { return std::floor(getDouble(a, 0)); }
    static Var round(Args a) { return std::round(getDouble(a, 0)); }
    static Var max(Args a) { return std::max(getDouble(a, 0), getDouble(a, 1)); }
    static Var min(Args a) { return std::min(getDouble(a, 0), getDouble(a, 1)); }
    static Var sign(Args a) { return signum(getDouble(a, 0)); }
    static Var log(Args a) { return std::log(getDouble(a, 0)); }
    static Var log10(Args a) { return std::log10(getDouble(a, 0)); }
    static Var exp(Args a) { return std::exp(getDouble(a, 0)); }
    static Var pow(Args a) { return std::pow(getDouble(a, 0), getDouble(a, 1)); }
    static Var sqrt(Args a) { return std::sqrt(getDouble(a, 0)); }
    static Var rand(Args a) { return std::rand(); }
    static Var fmod(Args a) { return std::fmod(getDouble(a, 0), getDouble(a, 1)); }

    template <typename T>
    static int signum(T val) {
        return (T(0) < val) - (val < T(0));
    }
};