/*
  ==============================================================================

    AstVisitor.h
    Created: 11 Nov 2020 2:44:53pm
    Author:  Pierre

  ==============================================================================
*/

#pragma once

#include "Expr.h"
#include "Value.h"

class AstVisitor
{
public:
    virtual BaseValue* visitIntLiteral(const IntLiteralExpr& expr) = 0;
    virtual BaseValue* visitUnary(const UnaryExpr& expr) = 0;
    virtual BaseValue* visitBinary(const BinaryExpr& expr) = 0;
    virtual BaseValue* visitTernary(const TernaryConditionalExpr& expr) = 0;
    virtual BaseValue* visitGrouping(const GroupingExpr& expr) = 0;
    virtual BaseValue* visitTime(const TimeExpr& expr) = 0;
    //virtual BaseValue* visitCall(const CallExpr& expr) = 0;
};
