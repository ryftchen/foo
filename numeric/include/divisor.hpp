//! @file divisor.hpp
//! @author ryftchen
//! @brief The declarations (divisor) in the numeric module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#pragma once

#include <string>
#include <vector>

//! @brief Divisor-related functions in the numeric module.
namespace numeric::divisor
{
//! @brief Maximum alignment length per element of printing.
constexpr uint8_t maxAlignOfPrint = 16;
//! @brief Maximum columns per row of printing.
constexpr uint8_t maxColumnOfPrint = 10;

//! @brief Solution of divisor.
class DivisorSolution
{
public:
    //! @brief Destroy the DivisorSolution object.
    virtual ~DivisorSolution() = default;

    //! @brief The Euclidean method.
    //! @param a - first integer
    //! @param b - second integer
    //! @return all common divisors of two integers
    static std::vector<int> euclideanMethod(int a, int b);
    //! @brief The Stein method.
    //! @param a - first integer
    //! @param b - second integer
    //! @return all common divisors of two integers
    static std::vector<int> steinMethod(int a, int b);

private:
    //! @brief Recursive for the Stein method.
    //! @param a - first integer
    //! @param b - second integer
    //! @return greatest common divisor
    static int steinRecursive(int a, int b);
    //! @brief Get all common divisors by the greatest common divisor.
    //! @param greatestCommonDivisor - greatest common divisor
    //! @return all common divisors
    static std::vector<int> getAllDivisors(const int greatestCommonDivisor);
};

//! @brief Builder for the target.
class TargetBuilder
{
public:
    //! @brief Construct a new TargetBuilder object.
    //! @param integer1 - first integer
    //! @param integer2 - second integer
    TargetBuilder(const int integer1, const int integer2);
    //! @brief Destroy the TargetBuilder object.
    virtual ~TargetBuilder() = default;

    //! @brief Get the pair of integers.
    //! @return pair of integers
    [[nodiscard]] inline std::pair<int, int> getIntegers() const;
    //! @brief Format integer vector for printing.
    //! @tparam T - type of vector
    //! @param vector - vector to be formatted
    //! @param buffer - buffer for filling the formatted vector
    //! @param bufferSize - size of buffer
    //! @return buffer after format
    template <typename T>
    requires std::is_integral<T>::value static char* formatIntegerVector(
        const std::vector<T>& vector,
        char* const buffer,
        const uint32_t bufferSize);

private:
    //! @brief First integer.
    const int integer1;
    //! @brief Second integer.
    const int integer2;
};

inline std::pair<int, int> TargetBuilder::getIntegers() const
{
    return std::make_pair(integer1, integer2);
}

template <typename T>
requires std::is_integral<T>::value char* TargetBuilder::formatIntegerVector(
    const std::vector<T>& vector,
    char* const buffer,
    const uint32_t bufferSize)
{
    uint32_t align = 0;
    for (uint32_t i = 0; i < vector.size(); ++i)
    {
        align = std::max(static_cast<uint32_t>(std::to_string(vector.at(i)).length()), align);
    }

    int formatSize = 0;
    uint32_t completeSize = 0;
    for (uint32_t i = 0; i < vector.size(); ++i)
    {
        formatSize = std::snprintf(buffer + completeSize, bufferSize - completeSize, "%*d ", align + 1, vector.at(i));
        if ((formatSize < 0) || (formatSize >= static_cast<int>(bufferSize - completeSize)))
        {
            break;
        }
        completeSize += formatSize;

        if ((0 == (i + 1) % maxColumnOfPrint) && ((i + 1) != vector.size()))
        {
            formatSize = std::snprintf(buffer + completeSize, bufferSize - completeSize, "\n");
            if ((formatSize < 0) || (formatSize >= static_cast<int>(bufferSize - completeSize)))
            {
                break;
            }
            completeSize += formatSize;
        }
    }

    return buffer;
}
} // namespace numeric::divisor
