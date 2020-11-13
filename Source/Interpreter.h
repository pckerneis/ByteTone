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

class MathLibrary : public juce::DynamicObject
{
    typedef const juce::var::NativeFunctionArgs& Args;

public:
    MathLibrary()
    {
        setMethod("sin",    sin);
        setMethod("asin",   asin);
        setMethod("sinh",   sinh);
        setMethod("asinh",  asinh);
        setMethod("cos",    cos);
        setMethod("acos",   acos);
        setMethod("tan",    tan);
        setMethod("atan",   atan);
        setMethod("tanh",   tanh);
        setMethod("atanh",  atanh);
        setMethod("ceil",   ceil);
        setMethod("floor",  floor);
        setMethod("round",  round);
        setMethod("max",    max);
        setMethod("min",    min);
        setMethod("sign",   sign);
        setMethod("log",    log);
        setMethod("log10",  log10);
        setMethod("exp",    exp);
        setMethod("pow",    pow);
        setMethod("sqrt",   sqrt);
        setMethod("rand",   rand);
    }

    static double getDouble(Args a, int pos) { return a.arguments[pos]; }

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

    template <typename T>
    static int signum(T val) {
        return (T(0) < val) - (val < T(0));
    }
};  

class Interpreter : AstVisitor
{
public:
    Interpreter()
    {
    }

    juce::Array<Var> generate(const juce::String source, int numSamples)
    {
        Scanner scanner(source);
        Parser parser(scanner.scanTokens());
        std::unique_ptr<Expr> expr(parser.parse());

        juce::Array<Var> output;

        for (int i = 0; i < numSamples; ++i)
        {
            t = i;
            output.add(evaluate(expr.get()));
        }

        return output;
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

        if (cond)
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

        bool asInts = left.isInt();
        
        switch (expr.op.type)
        {
        case TokenType::MINUS:              return asInts ? (int)left - (int)right : (double)left - (double)right;
        case TokenType::STAR:               return asInts ? (int)left * (int)right : (double)left * (double)right;
        case TokenType::PLUS:               return asInts ? (int)left + (int)right : (double)left + (double)right;
        case TokenType::MODULUS:            return asInts ? (int)left % (int)right : 0;
        case TokenType::SLASH:              return (double)right == 0.0 ? 0 : (double)left / (double)right;

        case TokenType::BITWISE_AND:        return asInts ? (int)left & (int)right : 0;
        case TokenType::BITWISE_OR:         return asInts ? (int)left | (int)right : 0;
        case TokenType::BITWISE_XOR:        return asInts ? (int)left ^ (int)right : 0;

        case TokenType::BIT_SHIFT_LEFT:     return asInts ? (int)left << (int)right : 0;
        case TokenType::BIT_SHIFT_RIGHT:    return asInts ? (int)left >> (int)right : 0;

        case TokenType::GREATER:            return left > right;
        case TokenType::GREATER_EQUAL:      return left >= right;
        case TokenType::LESS:               return left < right;
        case TokenType::LESS_EQUAL:         return left <= right;

        case TokenType::EQUAL_EQUAL:        return left == right;
        case TokenType::NOT_EQUAL:          return left != right;
        }

        return 0;
    }

    Var visitUnary(const UnaryExpr& expr) override
    {
        Var right = evaluate(expr.right.get());

        switch (expr.op.type)
        {
        case TokenType::BANG:
            return ! right;
            break;
        case TokenType::MINUS:
            return right.isInt() ? -(int)right : -(double)right;
            break;
        case TokenType::BITWISE_COMPLEMENT:
            return ~right.isInt() ? right : 0;
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

        juce::String test("test");

        return mathLibrary.getProperty(expr.name);
    }

    Var visitCall(const CallExpr& expr) override
    {
        Var callee = evaluate(expr.callee.get());

        juce::Array<Var> arguments;

        for (Expr* argument : expr.arguments)
        {
            arguments.add(evaluate(argument));
        }

        if (!callee.isMethod())
        {
            return 0;
        }

        const juce::var::NativeFunctionArgs args(callee, arguments.begin(), arguments.size());

        if (auto function = callee.getNativeFunction())
            return function(args);
    }

private:
    MathLibrary mathLibrary;

    int t = 0;
};