/*
  ==============================================================================

    Parser.cpp
    Created: 11 Nov 2020 11:07:33am
    Author:  Pierre

  ==============================================================================
*/

#include "Parser.h"

Expr* Parser::parse()
{
    return expression();
}

Expr* Parser::expression()
{
    // TODO: add ternary conditional
    return bitwiseOr();
}

Expr* Parser::bitwiseOr()
{
    std::unique_ptr<Expr> expr (bitwiseXor());

    while (match(juce::Array<TokenType>(TokenType::BITWISE_OR)))
    {
        Token oper = previous();
        std::unique_ptr<Expr> right(bitwiseXor());
        expr.reset(new BinaryExpr(expr.release(), oper, right.release()));
    }

    return expr.release();
}

Expr* Parser::bitwiseXor()
{
    std::unique_ptr<Expr> expr (bitwiseAnd());

    while (match(juce::Array<TokenType>(TokenType::BITWISE_XOR)))
    {
        Token oper = previous();
        std::unique_ptr<Expr> right(bitwiseAnd());
        expr.reset(new BinaryExpr(expr.release(), oper, right.release()));
    }

    return expr.release();
}

Expr* Parser::bitwiseAnd()
{
    std::unique_ptr<Expr> expr (equality());

    while (match(juce::Array<TokenType>(TokenType::BITWISE_AND)))
    {
        Token oper = previous();
        std::unique_ptr<Expr> right(equality());
        expr.reset(new BinaryExpr(expr.release(), oper, right.release()));
    }

    return expr.release();
}

Expr* Parser::equality()
{
    std::unique_ptr<Expr> expr (comparison());

    while (match(juce::Array<TokenType>(TokenType::NOT_EQUAL, TokenType::EQUAL_EQUAL)))
    {
        Token oper = previous();
        std::unique_ptr<Expr> right(comparison());
        expr.reset(new BinaryExpr(expr.release(), oper, right.release()));
    }

    return expr.release();
}

Expr* Parser::comparison()
{
    std::unique_ptr<Expr> expr(bitshift());

    while (match(juce::Array<TokenType>(
        TokenType::GREATER, TokenType::GREATER_EQUAL,
        TokenType::LESS, TokenType::LESS_EQUAL)))
    {
        Token oper = previous();
        std::unique_ptr<Expr> right(bitshift());
        expr.reset(new BinaryExpr(expr.release(), oper, right.release()));
    }

    return expr.release();
}

Expr* Parser::bitshift()
{
    std::unique_ptr<Expr> expr(term());

    while (match(juce::Array<TokenType>(TokenType::BIT_SHIFT_LEFT, TokenType::BIT_SHIFT_RIGHT)))
    {
        Token oper = previous();
        std::unique_ptr<Expr> right(term());
        expr.reset(new BinaryExpr(expr.release(), oper, right.release()));
    }

    return expr.release();
}

Expr* Parser::term()
{
    std::unique_ptr<Expr> expr(factor());

    while (match(juce::Array<TokenType>(TokenType::MINUS, TokenType::PLUS)))
    {
        Token oper = previous();
        std::unique_ptr<Expr> right(factor());
        expr.reset(new BinaryExpr(expr.release(), oper, right.release()));
    }

    return expr.release();
}

Expr* Parser::factor()
{
    std::unique_ptr<Expr> expr(unary());

    while (match(juce::Array<TokenType>(TokenType::SLASH, TokenType::STAR, TokenType::MODULUS)))
    {
        Token oper = previous();
        std::unique_ptr<Expr> right(factor());
        expr.reset(new BinaryExpr(expr.release(), oper, right.release()));
    }

    return expr.release();
}

Expr* Parser::unary()
{
    if (match(juce::Array<TokenType>(TokenType::BANG, TokenType::MINUS, TokenType::BITWISE_COMPLEMENT)))
    {
        Token oper = previous();
        std::unique_ptr<Expr> right(unary());
        return new UnaryExpr(oper, right.release());
    }

    return primary();
}

Expr* Parser::primary()
{
    if (match(juce::Array<TokenType>(TokenType::NUMBER)))
    {
        return new LiteralExpr(previous().literal);
    }

    if (match(juce::Array<TokenType>(TokenType::T)))
    {
        return new TimeExpr();
    }

    if (match(juce::Array<TokenType>(TokenType::BITWISE_COMPLEMENT)))
    {
        return new UnaryExpr(previous(), expression());
    }

    if (match(juce::Array<TokenType>(TokenType::LEFT_PAREN)))
    {
        std::unique_ptr<Expr> expr (expression());
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return new GroupingExpr(expr.release());
    }

    throw error(peek(), "Expect expression.");
}

bool Parser::match(juce::Array<TokenType> types)
{
    for (TokenType type : types)
    {
        if (check(type))
        {
            advance();
            return true;
        }
    }

    return false;
}

bool Parser::check(TokenType type)
{
    if (isAtEnd()) return false;
    return peek().type == type;
}

Token Parser::consume(TokenType type, juce::String message)
{
    if (check(type)) return advance();

    const Token token = peek();
    throw error(token, message);
}

ParseError Parser::error(Token token, juce::String message)
{
    return ParseError();
}

Token Parser::advance()
{
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::isAtEnd()
{
    return peek().type == TokenType::EOF_;
}

Token Parser::peek()
{
    return tokens[current];
}

Token Parser::previous()
{
    return tokens[current - 1];
}
