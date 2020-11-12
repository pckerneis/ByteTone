/*
  ==============================================================================

    Expr.cpp
    Created: 11 Nov 2020 2:48:29pm
    Author:  Pierre

  ==============================================================================
*/

#include "Expr.h"
#include "AstVisitor.h"

T Expr::accept(AstVisitor* visitor) const
{
    throw std::exception();
}

T TernaryConditionalExpr::accept(AstVisitor* visitor) const
{
    return visitor->visitTernary(*this);
}

T BinaryExpr::accept(AstVisitor* visitor) const
{
    return visitor->visitBinary(*this);
}

T UnaryExpr::accept(AstVisitor* visitor) const
{
    return visitor->visitUnary(*this);
}

T LiteralExpr::accept(AstVisitor* visitor) const
{
    return visitor->visitLiteral(*this);
}

T GroupingExpr::accept(AstVisitor* visitor) const
{
    return visitor->visitGrouping(*this);
}

T TimeExpr::accept(AstVisitor* visitor) const
{
    return visitor->visitTime(*this);
}
