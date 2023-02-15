//! @file prime.hpp
//! @author ryftchen
//! @brief The declarations (prime) in the numeric module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#pragma once

#include <string>
#include <vector>

//! @brief Prime-related functions in the numeric module.
namespace numeric::prime
{
//! @brief Maximum alignment length per element of printing.
constexpr uint8_t maxAlignOfPrint = 16;
//! @brief Maximum columns per row of printing.
constexpr uint8_t maxColumnOfPrint = 10;

//! @brief Solution of prime.
class PrimeSolution
{
public:
    //! @brief Destroy the PrimeSolution object.
    virtual ~PrimeSolution() = default;

    //! @brief The Eratosthenes method.
    //! @param max - maximum positive integer
    //! @return all prime numbers that are not greater than the maximum positive integer
    static std::vector<uint32_t> eratosthenesMethod(const uint32_t max);
    //! @brief The Euler method.
    //! @param max - maximum positive integer
    //! @return all prime numbers that are not greater than the maximum positive integer
    static std::vector<uint32_t> eulerMethod(const uint32_t max);
};

//! @brief Builder for the target.
class TargetBuilder
{
public:
    //! @brief Construct a new TargetBuilder object.
    //! @param maxPositiveInteger - maximum positive integer
    explicit TargetBuilder(const uint32_t maxPositiveInteger);
    //! @brief Destroy the TargetBuilder object.
    virtual ~TargetBuilder() = default;

    //! @brief Get the Maximum positive integer.
    //! @return maximum positive integer
    [[nodiscard]] inline uint32_t getMaxPositiveInteger() const;
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
    //! @brief Maximum positive integer.
    const uint32_t maxPositiveInteger;
};

inline uint32_t TargetBuilder::getMaxPositiveInteger() const
{
    return maxPositiveInteger;
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
} // namespace numeric::prime
