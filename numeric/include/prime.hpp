#pragma once

#include <string>
#include <vector>

namespace numeric::prime
{
constexpr uint32_t maxAlignOfPrint = 16;
constexpr uint32_t maxColumnOfPrint = 10;

class PrimeSolution
{
public:
    virtual ~PrimeSolution() = default;

    static std::vector<uint32_t> eratosthenesMethod(const uint32_t max);
    static std::vector<uint32_t> eulerMethod(const uint32_t max);
};

class TargetBuilder
{
public:
    explicit TargetBuilder(const uint32_t maxPositiveInteger);
    virtual ~TargetBuilder() = default;

    [[nodiscard]] inline uint32_t getMaxPositiveInteger() const;
    template <typename T>
    requires std::is_integral<T>::value static char* formatIntegerVector(
        const std::vector<T>& vector,
        char* const buffer,
        const uint32_t bufferSize);

private:
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
