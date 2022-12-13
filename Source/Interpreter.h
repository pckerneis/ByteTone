/*
  ==============================================================================

    Interepreter.h
    Created: 11 Nov 2020 2:09:05pm
    Author:  Pierre

  ==============================================================================
*/

#pragma once

#include "AstVisitor.h"
#include "Scanner.h"
#include "Parser.h"

class MathLibrary
{
    using Args = Var::Args;
public:
    MathLibrary()
    {
        registerFunction("sin",    sin);
        registerFunction("asin",   asin);
        registerFunction("sinh",   sinh);
        registerFunction("asinh",  asinh);
        registerFunction("cos",    cos);
        registerFunction("acos",   acos);
        registerFunction("tan",    tan);
        registerFunction("atan",   atan);
        registerFunction("tanh",   tanh);
        registerFunction("atanh",  atanh);
        registerFunction("ceil",   ceil);
        registerFunction("floor",  floor);
        registerFunction("round",  round);
        registerFunction("max",    max);
        registerFunction("min",    min);
        registerFunction("sign",   sign);
        registerFunction("log",    log);
        registerFunction("log10",  log10);
        registerFunction("exp",    exp);
        registerFunction("pow",    pow);
        registerFunction("sqrt",   sqrt);
        registerFunction("rand",   rand);
    }

    void registerFunction(juce::String name, Var::NativeFunction function)
    {
        registeredFunctions.set(name, function);
    }

    bool hasFunction(juce::String name) const
    {
        return registeredFunctions.contains(name);
    }

    Var getFunctionVar(juce::String name) const
    {
        return Var(registeredFunctions[name]);
    }

    static double getDouble(Args a, int pos) { return a.arguments[pos].coercedToDouble(); }

    static Var sin(Args a)      { return std::sin(getDouble(a, 0)); }
    static Var asin(Args a)     { return std::asin(getDouble(a, 0)); }
    static Var sinh(Args a)     { return std::sinh(getDouble(a, 0)); }
    static Var asinh(Args a)    { return std::asinh(getDouble(a, 0)); }
    static Var cos(Args a)      { return std::cos(getDouble(a, 0)); }
    static Var acos(Args a)     { return std::acos(getDouble(a, 0)); }
    static Var cosh(Args a)     { return std::cosh(getDouble(a, 0)); }
    static Var acosh(Args a)    { return std::acosh(getDouble(a, 0)); }
    static Var tan(Args a)      { return std::tan(getDouble(a, 0)); }
    static Var atan(Args a)     { return std::atan(getDouble(a, 0)); }
    static Var tanh(Args a)     { return std::tanh(getDouble(a, 0)); }
    static Var atanh(Args a)    { return std::atanh(getDouble(a, 0)); }
    static Var ceil(Args a)     { return std::ceil(getDouble(a, 0)); }
    static Var floor(Args a)    { return std::floor(getDouble(a, 0)); }
    static Var round(Args a)    { return std::round(getDouble(a, 0)); }
    static Var max(Args a)      { return std::max(getDouble(a, 0), getDouble(a, 1)); }
    static Var min(Args a)      { return std::min(getDouble(a, 0), getDouble(a, 1)); }
    static Var sign(Args a)     { return signum(getDouble(a, 0)); }
    static Var log(Args a)      { return std::log(getDouble(a, 0)); }
    static Var log10(Args a)    { return std::log10(getDouble(a, 0)); }
    static Var exp(Args a)      { return std::exp(getDouble(a, 0)); }
    static Var pow(Args a)      { return std::pow(getDouble(a, 0), getDouble(a, 1)); }
    static Var sqrt(Args a)     { return std::sqrt(getDouble(a, 0)); }
    static Var rand(Args a)     { return std::rand(); }
    static Var fmod(Args a)     { return std::fmod(getDouble(a, 0), getDouble(a, 1)); }

    template <typename T>
    static int signum(T val) {
        return (T(0) < val) - (val < T(0));
    }

    juce::HashMap<juce::String, Var::NativeFunction> registeredFunctions;
};  

class Interpreter : AstVisitor
{
public:
    Interpreter()
    {
    }

    juce::Array<Var> generate(const juce::String source, int numSamples)
    {
        return generateRange(source, 0, numSamples);
    }

    juce::Array<Var> generateRange(const juce::String source, int startSample, int numSamples)
    {
        std::unique_ptr<Expr> expr(parse(source));
        return evaluateRange(expr.get(), startSample, numSamples);
    }

    juce::Array<Var> evaluateRange(const Expr* expr, int startSample, int numSamples)
    {
        reset();

        juce::Array<Var> output;

        for (int i = 0; i < numSamples; ++i)
        {
            t = startSample + i;
            output.add(evaluate(expr));
        }

        return output;
    }

    static Expr* parse(const juce::String source)
    {
        Scanner scanner(source);
        Parser parser(scanner.scanTokens());
        return parser.parse();
    }

private:
    void reset()
    {
        assignedValues.clear();
    }

    Var evaluate(const Expr* expr)
    {
        return expr->accept(this);
    }

    Var visitLiteral(const LiteralExpr& expr) override
    {
        return expr.value;
    }

    Var visitTernary(const TernaryConditionalExpr& expr) override
    {
        Var cond = evaluate(expr.condition.get());

        if (cond.coercedToBool())
        {
            return evaluate(expr.ifExpr.get());
        }
        else
        {
            return evaluate(expr.elseExpr.get());
        }
    }

    Var visitBinary(const BinaryExpr& expr) override
    {
        Var left = evaluate(expr.left.get());
        Var right = evaluate(expr.right.get());

        bool leftBool = left.coercedToBool();
        bool rightBool = right.coercedToBool();

        int leftInt = left.coercedToInt();
        int rightInt = right.coercedToInt();

        double leftDouble = left.coercedToDouble();
        double rightDouble = right.coercedToDouble();

        bool asDoubles = left.isDouble() || right.isDouble();

        if (asDoubles)
        {
            // DBG("doubles right here!!");
        }
        
        switch (expr.op.type)
        {
        case TokenType::MINUS:              return asDoubles ? Var(leftDouble - rightDouble) : Var(leftInt - rightInt);
        case TokenType::STAR:               return asDoubles ? Var(leftDouble * rightDouble) : Var(leftInt * rightInt);
        case TokenType::PLUS:               return asDoubles ? Var(leftDouble + rightDouble) : Var(leftInt + rightInt);
        case TokenType::MODULUS:            return rightInt == 0 ? 0 : leftInt % rightInt;
        case TokenType::SLASH:              return asDoubles ? (rightDouble == 0.0 ? Var(0.0) : Var(leftDouble / rightDouble)) : (rightInt == 0 ? 0 : Var(leftInt / rightInt));

        case TokenType::BITWISE_AND:        return leftInt & rightInt;
        case TokenType::BITWISE_OR:         return leftInt | rightInt;
        case TokenType::BITWISE_XOR:        return leftInt ^ rightInt;

        case TokenType::BIT_SHIFT_LEFT:     return leftInt << rightInt;
        case TokenType::BIT_SHIFT_RIGHT:    return leftInt >> rightInt;

        case TokenType::GREATER:            return leftDouble >  rightDouble;
        case TokenType::GREATER_EQUAL:      return leftDouble >= rightDouble;
        case TokenType::LESS:               return leftDouble <  rightDouble;
        case TokenType::LESS_EQUAL:         return leftDouble <= rightDouble;

        case TokenType::EQUAL_EQUAL:        return leftDouble == rightDouble;
        case TokenType::NOT_EQUAL:          return leftDouble != rightDouble;
        }

        return 0;
    }

    Var visitUnary(const UnaryExpr& expr) override
    {
        Var right = evaluate(expr.right.get());

        switch (expr.op.type)
        {
        case TokenType::BANG:
            return Var(! right.coercedToBool());
            break;
        case TokenType::MINUS:
            return right.isDouble() ? Var(-right.getDoubleValue()) : Var(-right.coercedToInt());
            break;
        case TokenType::BITWISE_COMPLEMENT:
            return ~(int)right.coercedToInt();
            break;
        }

        return 0;
    }

    Var visitGrouping(const GroupingExpr& expr) override
    {
        return evaluate(expr.expression.get());
    }

    Var visitIdentifier(const IdentifierExpr& expr) override
    {
        if (expr.name.equalsIgnoreCase("t"))
        {
            return t;
        }

        if (assignedValues.contains(expr.name))
        {
            return assignedValues[expr.name];
        }

        return mathLibrary.getFunctionVar(expr.name);
    }

    Var visitCall(const CallExpr& expr) override
    {
        Var callee = evaluate(expr.callee.get());

        juce::Array<Var> arguments;

        for (Expr* argument : expr.arguments)
        {
            arguments.add(evaluate(argument));
        }

        const Var::Args args(arguments.begin(), arguments.size());
        return callee.call(args);
    }

    Var visitAssignment(const AssignExpr& expr) override
    {
        Var value = evaluate(expr.value.get());
        assignedValues.set(expr.assignee, value);
        return value;
    }

    MathLibrary mathLibrary;
    juce::HashMap<juce::String, Var> assignedValues;

    int t = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Interpreter)
};