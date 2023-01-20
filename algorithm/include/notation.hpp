//! @file notation.hpp
//! @author ryftchen
//! @brief The declarations (notation) in the algorithm module.
//! @version 0.1
//! @copyright Copyright (c) 2022
#pragma once

#include <string>

//! @brief Notation-related functions in the algorithm module.
namespace algorithm::notation
{
//! @brief Solution of notation.
class NotationSolution
{
public:
    //! @brief Construct a new NotationSolution object.
    //! @param infixNotation infix notation
    explicit NotationSolution(const std::string_view infixNotation);
    //! @brief Destroy the NotationSolution object.
    virtual ~NotationSolution() = default;

    //! @brief The prefix method.
    //! @param infixNotation infix notation
    //! @return prefix notation
    [[nodiscard]] std::string prefixMethod(const std::string& infixNotation) const;
    //! @brief The postfix method.
    //! @param infixNotation infix notation
    //! @return postfix notation
    [[nodiscard]] std::string postfixMethod(const std::string& infixNotation) const;

    //! @brief Get the infix notation.
    //! @return infix notation
    [[nodiscard]] inline std::string_view getInfixNotation() const;

private:
    //! @brief Infix notation.
    const std::string_view infixNotation;

    //! @brief Enumerate specific operator priorities.
    enum class Priority
    {
        none,
        low,
        medium,
        high
    };

    //! @brief Convert infix notation to postfix notation.
    //! @param infix infix notation
    //! @return postfix notation
    static std::string infixToPostfix(const std::string& infix);
    //! @brief Get the operator priority.
    //! @param c character
    //! @return operator priority
    static Priority getPriority(const char c);
    //! @brief Check whether the character is the operator.
    //! @param c character
    //! @return be operator or not
    static inline bool isOperator(const char c);
};

inline std::string_view NotationSolution::getInfixNotation() const
{
    return infixNotation;
}

inline bool NotationSolution::isOperator(const char c)
{
    return (!std::isalpha(c) && !std::isdigit(c));
}
} // namespace algorithm::notation
