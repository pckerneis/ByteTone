/*
  ==============================================================================

    AstVisitor.h
    Created: 11 Nov 2020 2:44:53pm
    Author:  Pierre

  ==============================================================================
*/

#pragma once

#include "Expr.h"

class AstVisitor
{
public:
    virtual int visitLiteral(const LiteralExpr& expr) = 0;
    virtual int visitUnary(const UnaryExpr& expr) = 0;
    virtual int visitBinary(const BinaryExpr& expr) = 0;
    virtual int visitTernary(const TernaryConditionalExpr& expr) = 0;
    virtual int visitGrouping(const GroupingExpr& expr) = 0;
    virtual int visitTime(const TimeExpr& expr) = 0;
};
