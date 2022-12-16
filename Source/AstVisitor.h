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
    virtual Var visitLiteral(const LiteralExpr& expr) = 0;
    virtual Var visitUnary(const UnaryExpr& expr) = 0;
    virtual Var visitBinary(const BinaryExpr& expr) = 0;
    virtual Var visitTernary(const TernaryConditionalExpr& expr) = 0;
    virtual Var visitGrouping(const GroupingExpr& expr) = 0;
    virtual Var visitIdentifier(const IdentifierExpr& expr) = 0;
    virtual Var visitCall(const CallExpr& expr) = 0;
    virtual Var visitAssignment(const AssignExpr& expr) = 0;
    virtual Var visitArray(const ArrayExpr& expr) = 0;
    virtual Var visitGetIndex(const GetIndexExpr& expr) = 0;
};
