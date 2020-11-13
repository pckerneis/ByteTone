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
    juce::Array<int> generate(const juce::String source, int numSamples);

    BaseValue* evaluate(const Expr* expr);

    BaseValue* visitIntLiteral(const IntLiteralExpr& expr) override;

    BaseValue* visitTernary(const TernaryConditionalExpr& expr) override;
    
    BaseValue* visitBinary(const BinaryExpr& expr) override;

    BaseValue* visitUnary(const UnaryExpr& expr) override;

    BaseValue* visitGrouping(const GroupingExpr& expr) override;

    BaseValue* visitTime(const TimeExpr& expr) override;

    //BaseValue visitCall(const CallExpr& expr) override
    //{
    //    Value callee = evaluate(expr.callee);

    //    List<Object> arguments = new ArrayList<>();
    //    for (Expr argument : expr.arguments) {
    //        arguments.add(evaluate(argument));
    //    }

    //    LoxCallable function = (LoxCallable)callee;
    //    return function.call(this, arguments);
    //}

private:
    int t = 0;
};