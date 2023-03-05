//! @file notation.hpp
//! @author ryftchen
//! @brief The declarations (notation) in the algorithm module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#pragma once

#ifndef __PRECOMPILED_HEADER
#include <string>
#else
#include "pch_algorithm.hpp"
#endif

//! @brief Notation-related functions in the algorithm module.
namespace algorithm::notation
{
//! @brief Notation methods.
class Notation
{
public:
    //! @brief Destroy the Notation object.
    virtual ~Notation() = default;

    //! @brief Prefix.
    //! @param infixNotation - infix notation
    //! @return prefix notation
    static std::string prefix(const std::string& infixNotation);
    //! @brief Postfix.
    //! @param infixNotation - infix notation
    //! @return postfix notation
    static std::string postfix(const std::string& infixNotation);

private:
    //! @brief Enumerate specific operator priorities.
    enum class Priority : uint8_t
    {
        none,
        low,
        medium,
        high
    };

    //! @brief Convert infix notation to postfix notation.
    //! @param infix - infix notation
    //! @return postfix notation
    static std::string infixToPostfix(const std::string& infix);
    //! @brief Get the operator priority.
    //! @param c - character
    //! @return operator priority
    static Priority getPriority(const char c);
    //! @brief Check whether the character is the operator.
    //! @param c - character
    //! @return be operator or not
    static inline bool isOperator(const char c);
};

inline bool Notation::isOperator(const char c)
{
    return (!std::isalpha(c) && !std::isdigit(c));
}
} // namespace algorithm::notation
