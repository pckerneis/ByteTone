/*
  ==============================================================================

    Expr.h
    Created: 11 Nov 2020 2:10:15pm
    Author:  Pierre

  ==============================================================================
*/

#pragma once

#include "Token.h"

typedef int T;

class AstVisitor;

struct Expr
{
    virtual ~Expr() {}
    virtual T accept(AstVisitor* visitor) const = 0;
};

struct BinaryExpr : Expr
{
    BinaryExpr(Expr* l, Token oper, Expr* r) : left(l), op(oper), right(r) {}
    virtual ~BinaryExpr() {}

    T accept(AstVisitor* visitor) const override;

    const std::unique_ptr<Expr> left;
    const std::unique_ptr <Expr> right;
    const Token op;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BinaryExpr)
};

struct UnaryExpr : Expr
{
    UnaryExpr(Token oper, Expr* r) : op(oper), right(r) {}
    virtual ~UnaryExpr() {}

    T accept(AstVisitor* visitor) const override;

    const std::unique_ptr <Expr> right;
    const Token op;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UnaryExpr)
};

struct LiteralExpr : Expr
{
    LiteralExpr(int v) : value(v) {}
    virtual ~LiteralExpr() {}

    T accept(AstVisitor* visitor) const override;

    const int value;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LiteralExpr)
};

struct GroupingExpr : Expr
{
    GroupingExpr(Expr* expr) : expression(expr) {}
    virtual ~GroupingExpr() {}

    T accept(AstVisitor* visitor) const override;

    const std::unique_ptr <Expr> expression;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GroupingExpr)
};

struct TimeExpr : Expr
{
    TimeExpr() {}
    virtual ~TimeExpr() {}

    T accept(AstVisitor* visitor) const override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimeExpr)
};
