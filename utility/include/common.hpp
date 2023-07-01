//! @file common.hpp
//! @author ryftchen
//! @brief The declarations (common) in the utility module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023

#pragma once

#include <cassert>
#include <shared_mutex>

//! @brief Format as a string.
#define COMMON_FORMAT_TO_STRING(format, args...)                              \
    (                                                                         \
        {                                                                     \
            const int bufferSize = std::snprintf(nullptr, 0, format, ##args); \
            assert(bufferSize >= 0);                                          \
            char buffer[bufferSize + 1];                                      \
            buffer[0] = '\0';                                                 \
            std::snprintf(buffer, bufferSize + 1, format, ##args);            \
            const std::string str(buffer);                                    \
            str;                                                              \
        })
//! @brief Format as a string and printing.
#define COMMON_PRINT(format, args...) std::cout << COMMON_FORMAT_TO_STRING(format, ##args) << std::flush

//! @brief Common-interface-related functions in the utility module.
namespace utility::common
{
//! @brief ANSI escape codes for red foreground color.
inline constexpr std::string_view colorRed = "\033[0;31;40m";
//! @brief ANSI escape codes for green foreground color.
inline constexpr std::string_view colorGreen = "\033[0;32;40m";
//! @brief ANSI escape codes for yellow foreground color.
inline constexpr std::string_view colorYellow = "\033[0;33;40m";
//! @brief ANSI escape codes for blue foreground color.
inline constexpr std::string_view colorBlue = "\033[0;34;40m";
//! @brief ANSI escape codes for gray foreground color.
inline constexpr std::string_view colorGray = "\033[0;37;40m";
//! @brief ANSI escape codes for the bold font.
inline constexpr std::string_view colorBold = "\033[1m";
//! @brief ANSI escape codes for the underline font.
inline constexpr std::string_view colorUnderLine = "\033[4m";
//! @brief ANSI escape codes for default background color.
inline constexpr std::string_view colorForBackground = "\033[49m";
//! @brief ANSI escape codes for ending.
inline constexpr std::string_view colorOff = "\033[0m";
//! @brief The hash seed for BKDR hash function.
constexpr std::uint64_t bkdrHashSeed = 131;
//! @brief The hash size for BKDR hash function.
constexpr std::uint64_t bkdrHashSize = 0x7FFFFFFF;
//! @brief Maximum size of output per line.
constexpr std::uint32_t maxBufferSize = 4096;

//! @brief The Brian-Kernighan Dennis-Ritchie hash function in compile time.
//! @param str - input data
//! @param hash - previous hash value
//! @return hash value
constexpr std::uint64_t bkdrHashInCompile(const char* const str, const std::uint64_t hash = 0) noexcept
{
    return (*str ? bkdrHashInCompile(str + 1, (hash * bkdrHashSeed + *str) & bkdrHashSize) : hash);
}

//! @brief The operator ("") overloading with BKDR hash function.
//! @param str - input data
//! @return hash value
constexpr std::uint64_t operator""_bkdrHash(const char* const str, const std::size_t /*unused*/) noexcept
{
    return bkdrHashInCompile(str);
}

//! @brief Splice strings into constexpr type.
//! @tparam Strings - target strings to be spliced
template <std::string_view const&... Strings>
struct Join
{
    //! @brief Implementation of splicing strings.
    //! @return character array
    static constexpr auto impl() noexcept
    {
        constexpr std::size_t length = (Strings.size() + ... + 0);
        std::array<char, length + 1> array{};
        auto append = [i = 0, &array](const auto& str) mutable
        {
            for (const auto ch : str)
            {
                array[i++] = ch;
            }
        };
        (append(Strings), ...);
        array[length] = 0;
        return array;
    }
    //! @brief A sequence of characters.
    static constexpr auto array = impl();
    //! @brief The splicing result. Converted from a sequence of characters.
    static constexpr std::string_view value{array.data(), array.size() - 1};
};
//! @brief Get the result of splicing strings.
//! @tparam target - strings to be spliced
template <std::string_view const&... Strings>
static constexpr auto joinStr = Join<Strings...>::value;

extern std::uint64_t bkdrHash(const char* str);
extern std::string executeCommand(const std::string& cmd, const std::uint32_t timeout = 0);
} // namespace utility::common
