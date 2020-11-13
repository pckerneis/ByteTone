/*
  ==============================================================================

    Expr.cpp
    Created: 11 Nov 2020 2:48:29pm
    Author:  Pierre

  ==============================================================================
*/

#include "Expr.h"
#include "AstVisitor.h"

Expr::~Expr() {}

T Expr::accept(AstVisitor* visitor) const
{
    throw std::exception();
}

TernaryConditionalExpr::TernaryConditionalExpr(Expr* cond, Expr* ifBranch, Expr* elseBranch)
    : condition(cond), ifExpr(ifBranch), elseExpr(elseBranch) {}

TernaryConditionalExpr::~TernaryConditionalExpr() {}

T TernaryConditionalExpr::accept(AstVisitor* visitor) const
{
    return visitor->visitTernary(*this);
}

BinaryExpr::BinaryExpr(Expr* l, Token oper, Expr* r) : left(l), op(oper), right(r) {}

BinaryExpr::~BinaryExpr() {}

T BinaryExpr::accept(AstVisitor* visitor) const
{
    return visitor->visitBinary(*this);
}

UnaryExpr::UnaryExpr(Token oper, Expr* r) : op(oper), right(r) {}

UnaryExpr::~UnaryExpr() {}

T UnaryExpr::accept(AstVisitor* visitor) const
{
    return visitor->visitUnary(*this);
}

IntLiteralExpr::IntLiteralExpr(int v) : value(v) {}

IntLiteralExpr::~IntLiteralExpr() {}

T IntLiteralExpr::accept(AstVisitor* visitor) const
{
    return visitor->visitIntLiteral(*this);
}

GroupingExpr::GroupingExpr(Expr* expr) : expression(expr) {}

GroupingExpr::~GroupingExpr() {}

T GroupingExpr::accept(AstVisitor* visitor) const
{
    return visitor->visitGrouping(*this);
}

TimeExpr::TimeExpr() {}

TimeExpr::~TimeExpr() {}

T TimeExpr::accept(AstVisitor* visitor) const
{
    return visitor->visitTime(*this);
}

//T CallExpr::accept(AstVisitor* visitor) const
//{
//    return visitor->visitCall(*this);
//}
