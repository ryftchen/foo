#pragma once

#include <string>
#include <vector>

namespace num_divisor
{
inline constexpr int integer1 = 2 * 2 * 3 * 3 * 5 * 5 * 7 * 7;
inline constexpr int integer2 = 2 * 3 * 5 * 7 * 11 * 13 * 17;
constexpr uint32_t maxAlignOfPrint = 16;
constexpr uint32_t maxColumnOfPrint = 10;

class Divisor
{
public:
    Divisor();
    virtual ~Divisor() = default;

    [[nodiscard]] std::vector<int> euclidMethod(int a, int b) const;
    [[nodiscard]] std::vector<int> steinMethod(int a, int b) const;

private:
    static int steinRecursive(int a, int b);
    static std::vector<int> getAllDivisors(const int greatestCommonDivisor);

protected:
    template <typename T>
    requires std::is_integral<T>::value char* formatIntegerVector(
        const std::vector<T>& vector,
        char* const buffer,
        const uint32_t bufferSize) const;
};

template <typename T>
requires std::is_integral<T>::value char* Divisor::formatIntegerVector(
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
} // namespace num_divisor