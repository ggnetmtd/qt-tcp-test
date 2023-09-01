#include "mathexpressiontoken.h"

int MathExpressionToken::getPrecedence()
{
    char op;

    if(Value.size() == 1 && Type == OperatorToken)
        op = Value[0];

    if(op == '+' || op == '-')
        return 1;

    if(op == '*' || op == '/')
        return 2;

    if(op == '(' || op == ')')
        return 3;

    return 0;
}

bool MathExpressionToken::isFloating() noexcept
{
    if(std::string_view::npos == Value.find("."))
        return false;

    return true;
}

bool MathExpressionToken::getValue(double &ret)
{
    std::stringstream stream;

    stream << Value;

    try {
        stream >> ret;
    }
    catch (const std::ios_base::failure& fail)
    {
        return false;
    }

    return true;
}
