#pragma once

#include <string>

namespace algo_notation
{
inline constexpr std::string_view infixNotation{"a+b*(c^d-e)^(f+g*h)-i"};

class Notation
{
public:
    Notation();
    virtual ~Notation() = default;

    [[nodiscard]] static std::string prefixMethod(const std::string& infixNotation);
    [[nodiscard]] static std::string postfixMethod(const std::string& infixNotation);

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

inline bool Notation::isOperator(const char c)
{
    return (!std::isalpha(c) && !std::isdigit(c));
}
} // namespace algo_notation
