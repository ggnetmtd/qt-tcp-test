#ifndef MATHEXPRESSION_H
#define MATHEXPRESSION_H

#include <vector>
#include <sstream>

#include "mathexpressiontoken.h"


class MathExpression
{
public:
    std::vector<MathExpressionToken> m_tokens;

    MathExpression(const std::string& expr);
    ~MathExpression();
    double calculate();

private:
    bool getTokens(const std::string& str, std::vector<MathExpressionToken>& tokens);
    double evaluateExpression(MathExpressionToken op, MathExpressionToken lval, MathExpressionToken rval);
    bool isOperator(const char& symbol);
    bool isBracket(const char& symbol);
    double getResult(char op, double a, double b);
};

#endif // MATHEXPRESSION_H
