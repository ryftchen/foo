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

//! @brief Get the config instance.
#define CONFIG_GET_INSTANCE application::config::Config::getInstance()
//! @brief Get configuration about "active_helper".
#define CONFIG_ACTIVE_HELPER CONFIG_GET_INSTANCE.getBool("active_helper")
//! @brief Get configuration about "logger_path".
#define CONFIG_LOGGER_PATH CONFIG_GET_INSTANCE.getString("logger_path")
//! @brief Get configuration about "logger_type".
#define CONFIG_LOGGER_TYPE CONFIG_GET_INSTANCE.getUnsignedInteger("logger_type")
//! @brief Get configuration about "logger_level".
#define CONFIG_LOGGER_LEVEL CONFIG_GET_INSTANCE.getUnsignedInteger("logger_level")
//! @brief Get configuration about "logger_target".
#define CONFIG_LOGGER_TARGET CONFIG_GET_INSTANCE.getUnsignedInteger("logger_target")
//! @brief Get configuration about "viewer_tcp_host".
#define CONFIG_VIEWER_TCP_HOST CONFIG_GET_INSTANCE.getString("viewer_tcp_host")
//! @brief Get configuration about "viewer_tcp_port".
#define CONFIG_VIEWER_TCP_PORT CONFIG_GET_INSTANCE.getUnsignedInteger("viewer_tcp_port")
//! @brief Get configuration about "viewer_udp_host".
#define CONFIG_VIEWER_UDP_HOST CONFIG_GET_INSTANCE.getString("viewer_udp_host")
//! @brief Get configuration about "viewer_udp_port".
#define CONFIG_VIEWER_UDP_PORT CONFIG_GET_INSTANCE.getUnsignedInteger("viewer_udp_port")

//! @brief Configuration-related functions in the application module.
namespace application::config
{
//! @brief Default configuration file path.
constexpr std::string_view defaultConfigFile = "./foo.cfg";
//! @brief Default configuration content.
constexpr std::string_view defaultConfiguration = "# foo configuration\n"
                                                  "\n"
                                                  "# active logger & viewer (0 = disable 1 = enable)\n"
                                                  "active_helper = 1\n"
                                                  "# logger file path (character string)\n"
                                                  "logger_path = ./log/foo.log\n"
                                                  "# logger write type (0 = add 1 = over)\n"
                                                  "logger_type = 0\n"
                                                  "# logger minimum level (0 = debug 1 = info 2 = warn 3 = error)\n"
                                                  "logger_level = 0\n"
                                                  "# logger actual target (0 = file 1 = terminal 2 = all)\n"
                                                  "logger_target = 2\n"
                                                  "# viewer tcp server host (character string)\n"
                                                  "viewer_tcp_host = localhost\n"
                                                  "# viewer tcp server port (0-65535)\n"
                                                  "viewer_tcp_port = 61501\n"
                                                  "# viewer udp server host (character string)\n"
                                                  "viewer_udp_host = localhost\n"
                                                  "# viewer udp server port (0-65535)\n"
                                                  "viewer_udp_port = 61502\n"
                                                  "\n";

//! @brief Configuration.
class Config
{
public:
    //! @brief Destroy the Config object.
    virtual ~Config() = default;
    //! @brief Construct a new Config object.
    Config(const Config&) = delete;
    //! @brief The operator (=) overloading of Config class.
    //! @return reference of Config object
    Config& operator=(const Config&) = delete;

    //! @brief Get the Config instance.
    //! @return reference of Config object
    static Config& getInstance();
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
    //! @brief Construct a new Config object.
    //! @param filename - configuration file
    //! @param delim - delimiter
    //! @param commentDelim - comment delimiter
    explicit Config(
        const std::string& filename = std::string{defaultConfigFile},
        const char delim = '=',
        const char commentDelim = '#');

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

extern void initializeConfiguration(const std::string& filename = std::string{defaultConfigFile});
} // namespace application::config
