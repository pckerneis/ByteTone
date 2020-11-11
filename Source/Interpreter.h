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

class Interpreter : AstVisitor
{
public:
    juce::Array<int> generate(const juce::String source, int numSamples)
    {
        try
        {
            Scanner scanner(source);
            Parser parser(scanner.scanTokens());
            std::unique_ptr<Expr> expr(parser.parse());

            juce::Array<int> output;

            for (int i = 0; i < numSamples; ++i)
            {
                t = i;
                output.add(evaluate(expr.get()));
            }


            return output;
        }
        catch (std::exception& exception)
        {
            // TODO
        }
    }

    int evaluate(const Expr* expr)
    {
        return expr->accept(this);
    }

    int visitLiteral(const LiteralExpr& expr) override
    {
        return expr.value;
    }

    int visitBinary(const BinaryExpr& expr) override
    {
        int left = evaluate(expr.left.get());
        int right = evaluate(expr.right.get());
        
        switch (expr.op.type)
        {
        case TokenType::MINUS:              return left - right;
        case TokenType::SLASH:              return left / right;
        case TokenType::STAR:               return left * right;
        case TokenType::PLUS:               return left + right;
        case TokenType::MODULUS:            return left % right;

        case TokenType::BITWISE_AND:        return left & right;
        case TokenType::BITWISE_OR:         return left | right;
        case TokenType::BITWISE_XOR:        return left ^ right;

        case TokenType::BIT_SHIFT_LEFT:     return left << right;
        case TokenType::BIT_SHIFT_RIGHT:    return left >> right;

        case TokenType::GREATER:            return left > right;
        case TokenType::GREATER_EQUAL:      return left >= right;
        case TokenType::LESS:               return left < right;
        case TokenType::LESS_EQUAL:         return left <= right;

        case TokenType::EQUAL_EQUAL:        return left == right;
        case TokenType::NOT_EQUAL:          return left != right;
        }

        return 0;
    }


    int visitUnary(const UnaryExpr& expr) override
    {
        int right = evaluate(expr.right.get());

        switch (expr.op.type)
        {
        case TokenType::BANG:
            return ! right;
            break;
        case TokenType::MINUS:
            return -right;
            break;
        }

        return 0;
    }

    int visitGrouping(const GroupingExpr& expr) override
    {
        return evaluate(expr.expression.get());
    }

    int visitTime(const TimeExpr& expr) override
    {
        return t;
    }

private:
    int t = 0;
};