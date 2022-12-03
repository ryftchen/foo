#pragma once

#include <string>

namespace algorithm::notation
{
class NotationSolution
{
public:
    explicit NotationSolution(const std::string_view infixNotation);
    virtual ~NotationSolution() = default;

    [[nodiscard]] std::string prefixMethod(const std::string& infixNotation) const;
    [[nodiscard]] std::string postfixMethod(const std::string& infixNotation) const;

    [[nodiscard]] inline std::string_view getInfixNotation() const;

private:
    const std::string_view infixNotation;

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

inline std::string_view NotationSolution::getInfixNotation() const
{
    return infixNotation;
}

inline bool NotationSolution::isOperator(const char c)
{
    return (!std::isalpha(c) && !std::isdigit(c));
}
} // namespace algorithm::notation
