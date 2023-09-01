#include "mathexpression.h"

MathExpression::MathExpression(std::string& expr)
{
    if(!getTokens(expr, m_tokens))
        throw std::invalid_argument("Wrong input. Can't create tokens.");
}

MathExpression::~MathExpression()
{
    m_tokens.clear();
}


double MathExpression::calculate()
{
    const int MAX_OPERATIONS = 16;  // we'll lose string_view pointers on vector realloc.

    std::vector<MathExpressionToken> tokens = m_tokens;

    if(tokens.size() == 1)
    {
        double ret;
        tokens[0].getValue(ret);
        return ret;
    }

    std::vector<std::string> results;

    results.resize(MAX_OPERATIONS); // prealloc

    int top = 0;

    while(tokens.size() != 1)
    {
        auto precedence = tokens[0].getPrecedence();
        auto top_precedence = precedence;

        // evaluate where to start
        for(size_t index = 0; index < tokens.size(); index++)
        {
            precedence = tokens[index].getPrecedence();

            if(precedence > top_precedence)
                top = index;

            top_precedence = tokens[top].getPrecedence();
        }

        // top = operator, top - 1 = lval, top + 1 = rval
        double res = evaluateExpression(tokens[top], tokens[top-1], tokens[top+1]);

        // temporary result string for argument token
        results.push_back(std::to_string(res));
        auto ri = results.end() - results.begin() - 1;

        // change one token into the new
        tokens[top + 1] = MathExpressionToken(ArgumentToken, std::string_view(results[ri]));
        // delete two tokens
        tokens.erase(tokens.begin() + top - 1, tokens.begin() + top + 1);

        top = 0;
    }

    // our last token is the result
    double result;
    tokens[0].getValue(result);

    return result;
}


bool MathExpression::getTokens(const std::string& str, std::vector<MathExpressionToken>& tokens)
{
    if(str.size() == 0)
        return false;

    std::string_view strv = str;

    size_t index = 0, token_pos = 0;

    const size_t length = str.size();

    // check if the first argument is with +/-
    if(str[0] == '+' || str[0] == '-')
        index++;

    // it's not a valid case
    if(str[0] == '*' || str[0] == '/')
        return false;

    for(; index < length; index++)
    {
        if(isOperator(str[index]))
        {
            // push arg before operator
            tokens.push_back(MathExpressionToken(ArgumentToken, strv.substr(token_pos, index - token_pos)));

            // push operator
            if(index != length)
              tokens.push_back(MathExpressionToken(OperatorToken, strv.substr(index, 1)));

            // change token position
            token_pos = index + 1;
        }
    }

    // end with operator isn't a valid case.
    if(index == length && isOperator(str[index]))
    {
        return false;
    }

    // push last argument
    tokens.push_back(MathExpressionToken(ArgumentToken, strv.substr(token_pos, index)));

    return true;
}



bool MathExpression::isOperator(const char& symbol)
{
    return (symbol == '+' || symbol == '-' || symbol == '*' || symbol == '/');
}

bool MathExpression::isBracket(const char& symbol)
{
    return (symbol == '(' || symbol == ')');
}

double MathExpression::getResult(char op, double a, double b)
{
    switch(op)
    {
        case '+':
            return a + b;
        case '-':
            return a - b;
        case '*':
            return a * b;
        case '/':
            if(b == 0)
              throw std::invalid_argument("Null division.");
            return a / b;
        default:
            throw std::invalid_argument("Unknown operator.");
    }
}


double MathExpression::evaluateExpression(MathExpressionToken op, MathExpressionToken lval, MathExpressionToken rval)
{
    if(op.Type != OperatorToken || lval.Type != ArgumentToken || rval.Type != ArgumentToken)
        throw std::invalid_argument("Token type mismatch.");

    double a, b;

    if(lval.getValue(a) && rval.getValue(b))
        return this->getResult(op.Value[0], a, b);
    else
        throw std::invalid_argument("Args are not valid.");


    return 0;
}
