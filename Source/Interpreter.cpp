/*
  ==============================================================================

    Interepreter.cpp
    Created: 11 Nov 2020 2:09:05pm
    Author:  Pierre

  ==============================================================================
*/

#include "Interpreter.h"

juce::Array<int> Interpreter::generate(const juce::String source, int numSamples)
{
    Scanner scanner(source);
    Parser parser(scanner.scanTokens());
    std::unique_ptr<Expr> expr(parser.parse());

    juce::Array<int> output;

    for (int i = 0; i < numSamples; ++i)
    {
        t = i;
        std::unique_ptr<BaseValue> eval(evaluate(expr.get()));
        output.add(eval->toInt());
    }

    return output;
}

BaseValue* Interpreter::evaluate(const Expr* expr)
{
    return expr->accept(this);
}

BaseValue* Interpreter::visitIntLiteral(const IntLiteralExpr& expr)
{
    return new Int(expr.value);
}

BaseValue* Interpreter::visitTernary(const TernaryConditionalExpr& expr)
{
    std::unique_ptr<BaseValue> cond(evaluate(expr.condition.get()));

    if (cond->isTruthy())
    {
        return evaluate(expr.ifExpr.get());
    }
    else
    {
        return evaluate(expr.elseExpr.get());
    }
}

BaseValue* Interpreter::visitBinary(const BinaryExpr& expr)
{
    std::unique_ptr<BaseValue> left(evaluate(expr.left.get()));
    std::unique_ptr<BaseValue> right(evaluate(expr.right.get()));

    std::unique_ptr<BaseValue> result(new Int(0));

    switch (expr.op.type)
    {
    case TokenType::MINUS:              result.reset(left->substract(right.get())); break;
    case TokenType::SLASH:              result.reset(left->divide(right.get())); break;
    case TokenType::STAR:               result.reset(left->multiply(right.get())); break;
    case TokenType::PLUS:               result.reset(left->add(right.get())); break;
    case TokenType::MODULUS:            result.reset(left->modulus(right.get())); break;

    case TokenType::BITWISE_AND:        result.reset(left->bitwiseAnd(right.get())); break;
    case TokenType::BITWISE_OR:         result.reset(left->bitwiseOr(right.get())); break;
    case TokenType::BITWISE_XOR:        result.reset(left->bitwiseXor(right.get())); break;

    case TokenType::BIT_SHIFT_LEFT:     result.reset(left->bitShiftLeft(right.get())); break;
    case TokenType::BIT_SHIFT_RIGHT:    result.reset(left->bitShiftRight(right.get())); break;

    case TokenType::GREATER:            result.reset(left->greater(right.get())); break;
    case TokenType::GREATER_EQUAL:      result.reset(left->greaterEqual(right.get())); break;
    case TokenType::LESS:               result.reset(left->less(right.get())); break;
    case TokenType::LESS_EQUAL:         result.reset(left->lessEqual(right.get())); break;

    case TokenType::EQUAL_EQUAL:        result.reset(left->equalEqual(right.get())); break;
    case TokenType::NOT_EQUAL:          result.reset(left->notEqual(right.get())); break;
    }

    return result.release();
}

BaseValue* Interpreter::visitUnary(const UnaryExpr& expr)
{
    std::unique_ptr<BaseValue> right(evaluate(expr.right.get()));

    switch (expr.op.type)
    {
    case TokenType::BANG:
        return new Bool(!right->isTruthy());
        break;
    case TokenType::MINUS:
        return right->negate();
        break;
    case TokenType::BITWISE_COMPLEMENT:
        return right->bitwiseComplement();
        break;
    }

    return 0;
}

BaseValue* Interpreter::visitGrouping(const GroupingExpr& expr)
{
    return evaluate(expr.expression.get());
}

BaseValue* Interpreter::visitTime(const TimeExpr& expr)
{
    return new Int(t);
}
