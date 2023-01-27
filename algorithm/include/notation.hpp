//! @file notation.hpp
//! @author ryftchen
//! @brief The declarations (notation) in the algorithm module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023
#pragma once

#include <string>

//! @brief Notation-related functions in the algorithm module.
namespace algorithm::notation
{
//! @brief Solution of notation.
class NotationSolution
{
public:
    //! @brief Destroy the NotationSolution object.
    virtual ~NotationSolution() = default;

    //! @brief The prefix method.
    //! @param infixNotation - infix notation
    //! @return prefix notation
    static std::string prefixMethod(const std::string& infixNotation);
    //! @brief The postfix method.
    //! @param infixNotation - infix notation
    //! @return postfix notation
    static std::string postfixMethod(const std::string& infixNotation);

private:
    //! @brief Enumerate specific operator priorities.
    enum class Priority
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

inline bool NotationSolution::isOperator(const char c)
{
    return (!std::isalpha(c) && !std::isdigit(c));
}

//! @brief Builder for the target.
class TargetBuilder
{
public:
    //! @brief Construct a new TargetBuilder object.
    //! @param infixNotation - infix notation
    explicit TargetBuilder(const std::string_view infixNotation);
    //! @brief Destroy the TargetBuilder object.
    virtual ~TargetBuilder() = default;

    //! @brief Get the infix notation.
    //! @return infix notation
    [[nodiscard]] inline std::string_view getInfixNotation() const;

private:
    //! @brief Infix notation.
    const std::string_view infixNotation;
};

inline std::string_view TargetBuilder::getInfixNotation() const
{
    return infixNotation;
}
} // namespace algorithm::notation
