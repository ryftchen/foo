#include "notation.hpp"
#include <stack>
#include "utility/include/file.hpp"

#define NOTATION_RESULT "\r\n*%-7s method:\r\n%s: %s\n"

namespace algo_notation
{
Notation::Notation()
{
    std::cout << "\r\nInfix notation: " << infixNotation << std::endl;
}

std::string Notation::infixToPostfix(const std::string& infix)
{
    std::string postfix;
    std::stack<char> charStack;

    for (uint32_t i = 0; i < infix.size(); ++i)
    {
        if (!isOperator(infix.at(i)))
        {
            postfix += infix.at(i);
        }
        else if ('(' == infix.at(i))
        {
            charStack.push('(');
        }
        else if (')' == infix.at(i))
        {
            while ('(' != charStack.top())
            {
                postfix += charStack.top();
                charStack.pop();
            }
            charStack.pop();
        }
        else
        {
            while (!charStack.empty() && getPriority(infix.at(i)) <= getPriority(charStack.top()))
            {
                postfix += charStack.top();
                charStack.pop();
            }
            charStack.push(infix.at(i));
        }
    }

    while (!charStack.empty())
    {
        postfix += charStack.top();
        charStack.pop();
    }

    return std::move(postfix);
}

Notation::Priority Notation::getPriority(const char c)
{
    switch (c)
    {
        case '+':
            [[fallthrough]];
        case '-':
            return Priority::low;
        case '*':
            [[fallthrough]];
        case '/':
            return Priority::medium;
        case '^':
            return Priority::high;
        default:
            return Priority::none;
    }
}

// Prefix method
std::string Notation::prefixMethod(const std::string& infixNotation)
{
    std::string infix = infixNotation;

    std::reverse(infix.begin(), infix.end());
    for (uint32_t i = 0; i < infix.size(); ++i)
    {
        if ('(' == infix.at(i))
        {
            infix.at(i) = ')';
        }
        else if (')' == infix.at(i))
        {
            infix.at(i) = '(';
        }
    }
    std::string prefixNotation = infixToPostfix(infix);
    std::reverse(prefixNotation.begin(), prefixNotation.end());

    FORMAT_PRINT(NOTATION_RESULT, "Prefix", "Polish notation", prefixNotation.data());
    return std::move(prefixNotation);
}

// Postfix method
std::string Notation::postfixMethod(const std::string& infixNotation)
{
    std::string postfixNotation = infixToPostfix(infixNotation);

    FORMAT_PRINT(NOTATION_RESULT, "Postfix", "Reverse polish notation", postfixNotation.data());
    return std::move(postfixNotation);
}
} // namespace algo_notation
