//! @file common.hpp
//! @author ryftchen
//! @brief The declarations (common) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#pragma once

#include <array>
#include <cstring>
#include <string>

//! @brief Format as a string and printing.
#define COMMON_PRINT(format, ...) \
    std::cout << utility::common::formatString(format __VA_OPT__(, ) __VA_ARGS__) << std::flush
//! @brief Do stringify.
#define COMMON_STRINGIFY(x) #x
//! @brief Convert to string.
#define COMMON_TO_STRING(x) COMMON_STRINGIFY(x)

//! @brief The utility module.
namespace utility // NOLINT (modernize-concat-nested-namespaces)
{
//! @brief Common-interface-related functions in the utility module.
namespace common
{
extern const char* version() noexcept;

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
//! @brief Hash seed for BKDR hash function.
constexpr std::size_t bkdrHashSeed = 131;
//! @brief Hash size for BKDR hash function.
constexpr std::size_t bkdrHashSize = 0x7FFFFFFF;

//! @brief The Brian-Kernighan Dennis-Ritchie hash function in compile time.
//! @param str - input data
//! @param hash - previous hash value
//! @return hash value
constexpr std::size_t bkdrHashInCompile(const char* const str, const std::size_t hash = 0) noexcept
{
    return *str ? bkdrHashInCompile(str + 1, (hash * bkdrHashSeed + *str) & bkdrHashSize) : hash;
}

//! @brief The operator ("") overloading with BKDR hash function.
//! @param str - input data
//! @return hash value
constexpr std::size_t operator""_bkdrHash(const char* const str, const std::size_t /*len*/) noexcept
{
    return bkdrHashInCompile(str);
}

//! @brief Splice strings into constexpr type.
//! @tparam Strings - target strings to be spliced
template <const std::string_view&... Strings>
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
            for (const auto c : str)
            {
                array.at(i++) = c;
            }
        };
        (append(Strings), ...);
        array.at(length) = 0;
        return array;
    }
    //! @brief A sequence of characters.
    static constexpr auto array{impl()};
    //! @brief The splicing result. Converted from a sequence of characters.
    static constexpr std::string_view value{array.data(), array.size() - 1};
};
//! @brief Get the result of splicing strings.
//! @tparam Strings - target strings to be spliced
template <const std::string_view&... Strings>
static constexpr auto joinString = Join<Strings...>::value;

//! @brief Compare whether two strings are equal.
//! @param str1 - string 1
//! @param str2 - string 2
//! @return be equal or not equal
inline bool allStrEqual(const char* const str1, const char* const str2)
{
    return std::strcmp(str1, str2) == 0;
}

//! @brief Compare whether multiple strings are equal.
//! @tparam Others - type of arguments of string
//! @param str1 - string 1
//! @param str2 - string 2
//! @param others - arguments of string
//! @return be equal or not equal
template <typename... Others>
inline bool allStrEqual(const char* const str1, const char* const str2, Others const&... others)
{
    return allStrEqual(str1, str2) && allStrEqual(str2, others...);
}

//! @brief Check that the target value is part of the enumeration.
//! @tparam EnumType - type of enumeration
//! @tparam Values - arguments of enumeration
template <typename EnumType, EnumType... Values>
class EnumCheck;
//! @brief Check that the target value is part of the enumeration.
//! @tparam EnumType - type of enumeration
template <typename EnumType>
class EnumCheck<EnumType>
{
public:
    //! @brief Check whether it is an enumeration value.
    //! @tparam IntType - type of integral
    //! @return be an enumeration value or not
    template <typename IntType>
    static inline constexpr bool isValue(const IntType /*val*/)
    {
        return false;
    }
};
//! @brief Check that the target value is part of the enumeration.
//! @tparam EnumType - type of enumeration
//! @tparam Value - current value
//! @tparam Next - next enumeration value
template <typename EnumType, EnumType Value, EnumType... Next>
class EnumCheck<EnumType, Value, Next...> : private EnumCheck<EnumType, Next...>
{
public:
    //! @brief Check whether it is an enumeration value.
    //! @tparam IntType - type of integral
    //! @param val - target value
    //! @return be an enumeration value or not
    template <typename IntType>
    static inline constexpr bool isValue(const IntType val)
    {
        return (static_cast<IntType>(Value) == val) || EnumCheck<EnumType, Next...>::isValue(val);
    }
};

extern std::size_t bkdrHash(const char* str);
extern std::string base64Encode(const std::string& data);
extern std::string base64Decode(const std::string& data);
extern std::string formatString(const char* const format, ...);
extern std::string executeCommand(const std::string& command, const std::uint32_t timeout = 0);
} // namespace common
} // namespace utility
