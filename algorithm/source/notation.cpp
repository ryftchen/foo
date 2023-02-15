//! @file notation.cpp
//! @author ryftchen
//! @brief The definitions (notation) in the algorithm module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#include "notation.hpp"
#ifndef __PRECOMPILED_HEADER
#include <stack>
#endif
#ifdef __RUNTIME_PRINTING
#include "utility/include/common.hpp"

//! @brief Display notation result.
#define NOTATION_RESULT "\r\n*%-7s method:\r\n%s: %s\n"
//! @brief Print notation result content.
#define NOTATION_PRINT_RESULT_CONTENT(method, notationType, notationString) \
    COMMON_PRINT(NOTATION_RESULT, method, notationType, notationString)
#else

//! @brief Print notation result content.
#define NOTATION_PRINT_RESULT_CONTENT(method, notationType, notationString)
#endif

namespace algorithm::notation
{
std::string NotationSolution::prefixMethod(const std::string& infixNotation)
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

    NOTATION_PRINT_RESULT_CONTENT("Prefix", "Polish notation", prefixNotation.data());
    return prefixNotation;
}

std::string NotationSolution::postfixMethod(const std::string& infixNotation)
{
    std::string postfixNotation = infixToPostfix(infixNotation);

    NOTATION_PRINT_RESULT_CONTENT("Postfix", "Reverse polish notation", postfixNotation.data());
    return postfixNotation;
}

std::string NotationSolution::infixToPostfix(const std::string& infix)
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

NotationSolution::Priority NotationSolution::getPriority(const char c)
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

TargetBuilder::TargetBuilder(const std::string_view infixNotation) : infixNotation(infixNotation)
{
#ifdef __RUNTIME_PRINTING
    std::cout << "\r\nInfix notation: " << infixNotation << std::endl;
#endif
}
} // namespace algorithm::notation
