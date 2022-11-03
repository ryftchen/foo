#pragma once

#include <string>

namespace algo_notation
{
inline constexpr std::string_view infixNotation{"a+b*(c^d-e)^(f+g*h)-i"};

class NotationSolution
{
public:
    NotationSolution();
    virtual ~NotationSolution() = default;

    [[nodiscard]] std::string prefixMethod(const std::string& infixNotation) const;
    [[nodiscard]] std::string postfixMethod(const std::string& infixNotation) const;

private:
    enum class Priority
    {
        none,
        low,
        medium,
        high
    };

    static std::string infixToPostfix(const std::string& infix);
    static Priority getPriority(const char c);
    static inline bool isOperator(const char c);
};

inline bool NotationSolution::isOperator(const char c)
{
    return (!std::isalpha(c) && !std::isdigit(c));
}
} // namespace algo_notation
