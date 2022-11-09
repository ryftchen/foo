#pragma once

#include <string>
#include <vector>

namespace num_prime
{
constexpr uint32_t maxAlignOfPrint = 16;
constexpr uint32_t maxColumnOfPrint = 10;

class PrimeSolution
{
public:
    explicit PrimeSolution(const uint32_t maxPositiveInteger);
    virtual ~PrimeSolution() = default;

    [[nodiscard]] std::vector<uint32_t> eratosthenesMethod(const uint32_t max) const;
    [[nodiscard]] std::vector<uint32_t> eulerMethod(const uint32_t max) const;

protected:
    template <typename T>
    requires std::is_integral<T>::value char* formatIntegerVector(
        const std::vector<T>& vector,
        char* const buffer,
        const uint32_t bufferSize) const;
};

template <typename T>
requires std::is_integral<T>::value char* PrimeSolution::formatIntegerVector(
    const std::vector<T>& vector,
    char* const buffer,
    const uint32_t bufferSize) const
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
} // namespace num_prime
