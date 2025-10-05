//! @file notation.hpp
//! @author ryftchen
//! @brief The declarations (notation) in the algorithm module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include <cstdint>
#include <string>

//! @brief The algorithm module.
namespace algorithm // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Notation-related functions in the algorithm module.
namespace notation
{
//! @brief Brief function description.
//! @return function description (module_function)
inline static const char* description() noexcept
{
    return "ALGO_NOTATION";
}
extern const char* version() noexcept;

//! @brief Notation methods.
class Notation
{
public:
    //! @brief Prefix.
    //! @param infix - infix notation
    //! @return prefix notation
    static std::string prefix(const std::string_view infix);
    //! @brief Postfix.
    //! @param infix - infix notation
    //! @return postfix notation
    static std::string postfix(const std::string_view infix);

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
    static std::string infixToPostfix(const std::string_view infix);
    //! @brief Get the operator priority.
    //! @param c - character
    //! @return operator priority
    static Priority getPriority(const char c);
    //! @brief Check whether the character is the operator.
    //! @param c - character
    //! @return be operator or not
    static bool isOperator(const char c);
};
} // namespace notation
} // namespace algorithm
