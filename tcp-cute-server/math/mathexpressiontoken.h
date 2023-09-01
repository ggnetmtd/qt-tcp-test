#ifndef MATHEXPRESSIONTOKEN_H
#define MATHEXPRESSIONTOKEN_H

#include <string_view>
#include <sstream>

#define TOKEN_ARGUMENT false
#define TOKEN_OPERATOR true

enum TokenType {
    ArgumentToken,
    OperatorToken,
    LeftBracketToken,
    RightBracketToken
};

class MathExpressionToken
{
public:
    TokenType Type;
    std::string_view Value;

    MathExpressionToken(TokenType T, std::string_view V) : Type(T), Value(V)
    {}

    int getPrecedence();
    bool isFloating() noexcept;
    bool getValue(double &ret);
};

#endif // MATHEXPRESSIONTOKEN_H
