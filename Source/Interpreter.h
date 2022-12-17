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
#include "MathLibrary.h"
#include "AudioLibrary.h"
#include "Environment.h"

class Interpreter : AstVisitor
{
public:
    Interpreter(): env(Environment::withTickRate(1))
    {
    }

    juce::Array<Var> generate(const juce::String source, int numSamples, Environment env)
    {
        return generateRange(source, 0, numSamples, env);
    }

    juce::Array<Var> generateRange(const juce::String source, int startSample, int numSamples, Environment env)
    {
        std::unique_ptr<Expr> expr(parse(source));
        return evaluateRange(expr.get(), startSample, numSamples, env);
    }

    juce::Array<Var> evaluateRange(const Expr* expr, int startSample, int numSamples, Environment env)
    {
        reset();

        juce::Array<Var> output;

        for (int i = 0; i < numSamples; ++i)
        {
            t = startSample + i;
            output.add(evaluate(expr, env));
        }

        return output;
    }

    static Expr* parse(const juce::String source)
    {
        Scanner scanner(source);
        Parser parser(scanner.scanTokens());
        return parser.parse();
    }

    Var evaluate(const Expr* expr, Environment environment)
    {
        env = environment;
        return expr->accept(this);
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

        if (mathLibrary.hasFunction(expr.name))
        {
            return mathLibrary.getFunctionVar(expr.name);
        }

        if (audioLibrary.hasFunction(expr.name))
        {
            return audioLibrary.getFunctionVar(expr.name);
        }

        return Var();
    }

    Var visitCall(const CallExpr& expr) override
    {
        Var callee = evaluate(expr.callee.get());

        std::vector<Var> arguments;

        for (Expr* argument : expr.arguments)
        {
            arguments.push_back(evaluate(argument));
        }

        const Var::Args args(arguments, arguments.size(), &env);
        return callee.call(args);
    }

    Var visitAssignment(const AssignExpr& expr) override
    {
        Var value = evaluate(expr.value.get());
        assignedValues.set(expr.assignee, value);
        return value;
    }

    Var visitArray(const ArrayExpr& expr) override
    {
        std::vector<Var> elements;

        for (auto elemExpr : expr.elements)
        {
            elements.push_back(evaluate(elemExpr));
        }

        return Var(elements);
    }

    Var visitGetIndex(const GetIndexExpr& expr) override
    {
        Var callee = evaluate(expr.callee.get());
        Var indexValue = evaluate(expr.index.get());
        return callee.getAt(indexValue);
    }

    Var visitSequence(const SequenceExpr& expr) override
    {
        evaluate(expr.left.get());
        return evaluate(expr.right.get());
    }

    MathLibrary mathLibrary;
    AudioLibrary audioLibrary;
    juce::HashMap<juce::String, Var> assignedValues;
    Environment env;

    int t = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Interpreter)
};