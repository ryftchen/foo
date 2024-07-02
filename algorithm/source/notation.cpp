//! @file notation.cpp
//! @author ryftchen
//! @brief The definitions (notation) in the algorithm module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#include "notation.hpp"

#include <algorithm>
#include <stack>

namespace algorithm::notation
{
//! @brief Function version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    static const char* const ver = "0.1.0";
    return ver;
}

std::string Notation::prefix(const std::string& infixNotation)
{
    std::string infix = infixNotation;

    std::reverse(infix.begin(), infix.end());
    for (std::uint32_t i = 0; i < infix.size(); ++i)
    {
        if ('(' == infix[i])
        {
            infix[i] = ')';
        }
        else if (')' == infix[i])
        {
            infix[i] = '(';
        }
    }
    std::string prefixNotation = infixToPostfix(infix);
    std::reverse(prefixNotation.begin(), prefixNotation.end());

    return prefixNotation;
}

std::string Notation::postfix(const std::string& infixNotation)
{
    std::string postfixNotation = infixToPostfix(infixNotation);

    return postfixNotation;
}

std::string Notation::infixToPostfix(const std::string& infix)
{
    std::string postfix;
    std::stack<char> charStack;

    for (std::uint32_t i = 0; i < infix.size(); ++i)
    {
        if (!isOperator(infix[i]))
        {
            postfix += infix[i];
        }
        else if ('(' == infix[i])
        {
            charStack.push('(');
        }
        else if (')' == infix[i])
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
            while (!charStack.empty() && (getPriority(infix[i]) <= getPriority(charStack.top())))
            {
                postfix += charStack.top();
                charStack.pop();
            }
            charStack.push(infix[i]);
        }
    }

    while (!charStack.empty())
    {
        postfix += charStack.top();
        charStack.pop();
    }

    return postfix;
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

bool Notation::isOperator(const char c)
{
    return !std::isalpha(c) && !std::isdigit(c);
}
} // namespace algorithm::notation
