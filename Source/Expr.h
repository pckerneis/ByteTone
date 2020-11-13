/*
  ==============================================================================

    Expr.h
    Created: 11 Nov 2020 2:10:15pm
    Author:  Pierre

  ==============================================================================
*/

#pragma once

#include "Token.h"
#include "Value.h"

typedef BaseValue* T;

class AstVisitor;

struct Expr
{
    virtual ~Expr();
    virtual T accept(AstVisitor* visitor) const = 0;
};

struct TernaryConditionalExpr : Expr
{
    TernaryConditionalExpr(Expr* cond, Expr* ifBranch, Expr* elseBranch);
    virtual ~TernaryConditionalExpr();

    T accept(AstVisitor* visitor) const override;

    const std::unique_ptr<Expr> condition;
    const std::unique_ptr<Expr> ifExpr;
    const std::unique_ptr<Expr> elseExpr;
    const Token op;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TernaryConditionalExpr)
};

struct BinaryExpr : Expr
{
    BinaryExpr(Expr* l, Token oper, Expr* r);
    virtual ~BinaryExpr();

    T accept(AstVisitor* visitor) const override;

    const std::unique_ptr<Expr> left;
    const std::unique_ptr<Expr> right;
    const Token op;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BinaryExpr)
};

struct UnaryExpr : Expr
{
    UnaryExpr(Token oper, Expr* r);
    virtual ~UnaryExpr();

    T accept(AstVisitor* visitor) const override;

    const std::unique_ptr <Expr> right;
    const Token op;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UnaryExpr)
};

struct IntLiteralExpr : Expr
{
    IntLiteralExpr(int v);
    virtual ~IntLiteralExpr();

    T accept(AstVisitor* visitor) const override;

    int value;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IntLiteralExpr)
};

struct GroupingExpr : Expr
{
    GroupingExpr(Expr* expr);
    virtual ~GroupingExpr();

    T accept(AstVisitor* visitor) const override;

    const std::unique_ptr <Expr> expression;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GroupingExpr)
};

struct TimeExpr : Expr
{
    TimeExpr();
    virtual ~TimeExpr();

    T accept(AstVisitor* visitor) const override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimeExpr)
};
/*
struct CallExpr : Expr
{
    CallExpr(Expr* calleeExpr, Token parenToken, juce::OwnedArray<Expr>& args) 
        : callee(calleeExpr), paren(parenToken)
    {
        arguments.swapWith(args);
    }

    virtual ~CallExpr() {}

    T accept(AstVisitor* visitor) const override;

    const std::unique_ptr <Expr> callee;
    Token paren;
    juce::OwnedArray<Expr> arguments;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CallExpr)
};*/
