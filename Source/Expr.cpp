/*
  ==============================================================================

    Expr.cpp
    Created: 11 Nov 2020 2:48:29pm
    Author:  Pierre

  ==============================================================================
*/

#include "Expr.h"
#include "AstVisitor.h"

Var Expr::accept(AstVisitor* visitor) const
{
    throw std::exception();
}

Var TernaryConditionalExpr::accept(AstVisitor* visitor) const
{
    return visitor->visitTernary(*this);
}

Var BinaryExpr::accept(AstVisitor* visitor) const
{
    return visitor->visitBinary(*this);
}

Var UnaryExpr::accept(AstVisitor* visitor) const
{
    return visitor->visitUnary(*this);
}

Var LiteralExpr::accept(AstVisitor* visitor) const
{
    return visitor->visitLiteral(*this);
}

Var GroupingExpr::accept(AstVisitor* visitor) const
{
    return visitor->visitGrouping(*this);
}

Var IdentifierExpr::accept(AstVisitor* visitor) const
{
    return visitor->visitIdentifier(*this);
}

Var CallExpr::accept(AstVisitor* visitor) const
{
    return visitor->visitCall(*this);
}

Var AssignExpr::accept(AstVisitor* visitor) const
{
    return visitor->visitAssignment(*this);
}

Var ArrayExpr::accept(AstVisitor* visitor) const
{
    return visitor->visitArray(*this);
}
