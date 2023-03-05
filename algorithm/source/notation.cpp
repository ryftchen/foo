//! @file notation.cpp
//! @author ryftchen
//! @brief The definitions (notation) in the algorithm module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#include "notation.hpp"
#ifndef __PRECOMPILED_HEADER
#include <stack>
#endif

namespace algorithm::notation
{
std::string Notation::prefix(const std::string& infixNotation)
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
} // namespace algorithm::notation
