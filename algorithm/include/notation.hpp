//! @file notation.hpp
//! @author ryftchen
//! @brief The declarations (notation) in the algorithm module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen. All rights reserved.

#pragma once

#include <string>

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
    enum class Priority : std::uint8_t
    {
        //! @brief None.
        none,
        //! @brief Low.
        low,
        //! @brief Medium.
        medium,
        //! @brief High.
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
    static bool isOperator(const char c);
};
} // namespace algorithm::notation
