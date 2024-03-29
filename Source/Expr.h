/*
  ==============================================================================

    Expr.h
    Created: 11 Nov 2020 2:10:15pm
    Author:  Pierre

  ==============================================================================
*/

#pragma once

#include "Token.h"

class AstVisitor;

struct Expr
{
    virtual ~Expr() {}
    virtual Var accept(AstVisitor* visitor) const = 0;
};

struct TernaryConditionalExpr : Expr
{
    TernaryConditionalExpr(Expr* cond, Expr* ifBranch, Expr* elseBranch) : condition(cond), ifExpr(ifBranch), elseExpr(elseBranch) {}
    virtual ~TernaryConditionalExpr() {}

    Var accept(AstVisitor* visitor) const override;

    const std::unique_ptr<Expr> condition;
    const std::unique_ptr<Expr> ifExpr;
    const std::unique_ptr<Expr> elseExpr;
    const Token op;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TernaryConditionalExpr)
};

struct BinaryExpr : Expr
{
    BinaryExpr(Expr* l, Token oper, Expr* r) : left(l), right(r), op(oper) {}
    virtual ~BinaryExpr() {}

    Var accept(AstVisitor* visitor) const override;

    const std::unique_ptr<Expr> left;
    const std::unique_ptr<Expr> right;
    const Token op;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BinaryExpr)
};

struct UnaryExpr : Expr
{
    UnaryExpr(Token oper, Expr* r) : right(r), op(oper) {}
    virtual ~UnaryExpr() {}

    Var accept(AstVisitor* visitor) const override;

    const std::unique_ptr <Expr> right;
    const Token op;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UnaryExpr)
};

struct LiteralExpr : Expr
{
    LiteralExpr(Var v) : value(v) {}
    virtual ~LiteralExpr() {}

    Var accept(AstVisitor* visitor) const override;

    Var value;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LiteralExpr)
};

struct GroupingExpr : Expr
{
    GroupingExpr(Expr* expr) : expression(expr) {}
    virtual ~GroupingExpr() {}

    Var accept(AstVisitor* visitor) const override;

    const std::unique_ptr <Expr> expression;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GroupingExpr)
};

struct IdentifierExpr : Expr
{
    IdentifierExpr(juce::String n) : name(n) {}
    virtual ~IdentifierExpr() {}

    Var accept(AstVisitor* visitor) const override;

    juce::String name;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IdentifierExpr)
};

struct CallExpr : Expr
{
    CallExpr(Expr* calleeExpr, Token parenToken, juce::OwnedArray<Expr>& args) 
        : callee(calleeExpr), paren(parenToken)
    {
        arguments.swapWith(args);
    }

    virtual ~CallExpr() {}

    Var accept(AstVisitor* visitor) const override;

    const std::unique_ptr <Expr> callee;
    Token paren;
    juce::OwnedArray<Expr> arguments;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CallExpr)
};
