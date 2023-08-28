//! @file config.hpp
//! @author ryftchen
//! @brief The declarations (config) in the application module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen. All rights reserved.

#pragma once

#ifndef __PRECOMPILED_HEADER
#include <map>
#include <string>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

//! @brief Configuration-related functions in the application module.
namespace config
{
//! @brief Configuration.
class Config
{
public:
    //! @brief Construct a new Config object.
    //! @param filename - configuration file
    //! @param delim - delimiter
    //! @param commentDelim - comment delimiter
    explicit Config(const std::string& filename, const char delim = '=', const char commentDelim = '#');
    //! @brief Destroy the Config object.
    virtual ~Config() = default;

    //! @brief The operator ([]) overloading of the Config class.
    //! @param key - index key
    //! @return value of the string type
    std::string operator[](const std::string& key);
    //! @brief Get the value of the string type.
    //! @param key - index key
    //! @return value of the string type
    std::string getString(const std::string& key);
    //! @brief Get the value of the integer type.
    //! @param key - index key
    //! @return value of the integer type
    int getInteger(const std::string& key);
    //! @brief Get the value of the unsigned integer type.
    //! @param key - index key
    //! @return value of the unsigned integer type
    unsigned int getUnsignedInteger(const std::string& key);
    //! @brief Get the value of the float type.
    //! @param key - index key
    //! @return value of the float type
    float getFloat(const std::string& key);
    //! @brief Get the value of the double type.
    //! @param key - index key
    //! @return value of the double type
    double getDouble(const std::string& key);
    //! @brief Get the value of the bool type.
    //! @param key - index key
    //! @return value of the bool type
    bool getBool(const std::string& key);

private:
    //! @brief Delimiter.
    const char delimiter{'='};
    //! @brief Comment delimiter.
    const char commentDelimiter{'#'};
    //! @brief Configuration data.
    std::map<std::string, std::string> data;

    //! @brief Get the numeric value.
    //! @tparam T type of value
    //! @param key - index key
    //! @return numeric value
    template <class T>
    T getNumericValue(const std::string& key);
    //! @brief Check if the element exists.
    //! @param key - index key
    //! @return element exists or does not exist
    [[nodiscard]] bool elementExists(const std::string& key) const;
    //! @brief Parse the configuration file.
    //! @param filename - configuration file
    void parseFile(const std::string& filename);
    //! @brief Parse the line.
    //! @param line - target line
    void parseLine(const std::string& line);
    //! @brief Check if the format of the line is valid.
    //! @param line - target line
    //! @return format is valid or invalid
    [[nodiscard]] bool isFormatValid(const std::string& line) const;
    //! @brief Check if the line is a comment.
    //! @param line - target line
    //! @return line is comment or not
    [[nodiscard]] bool isComment(const std::string& line) const;
    //! @brief Trim the line.
    //! @param line - target line
    //! @return string after removing spaces from both ends
    static std::string trimLine(const std::string& line);
};
} // namespace config
