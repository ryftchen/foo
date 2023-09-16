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
//! @brief "active_helper" configuration.
#define CONFIG_ACTIVE_HELPER CONFIG_GET_INSTANCE.getBool("active_helper")
//! @brief "logger_path" configuration.
#define CONFIG_LOGGER_PATH CONFIG_GET_INSTANCE.getString("logger_path")
//! @brief "logger_type" configuration.
#define CONFIG_LOGGER_TYPE CONFIG_GET_INSTANCE.getUnsignedInteger("logger_type")
//! @brief "logger_level" configuration.
#define CONFIG_LOGGER_LEVEL CONFIG_GET_INSTANCE.getUnsignedInteger("logger_level")
//! @brief "logger_destination" configuration.
#define CONFIG_LOGGER_DESTINATION CONFIG_GET_INSTANCE.getUnsignedInteger("logger_destination")
//! @brief "viewer_tcp_host" configuration.
#define CONFIG_VIEWER_TCP_HOST CONFIG_GET_INSTANCE.getString("viewer_tcp_host")
//! @brief "viewer_tcp_port" configuration.
#define CONFIG_VIEWER_TCP_PORT CONFIG_GET_INSTANCE.getUnsignedInteger("viewer_tcp_port")
//! @brief "viewer_udp_host" configuration.
#define CONFIG_VIEWER_UDP_HOST CONFIG_GET_INSTANCE.getString("viewer_udp_host")
//! @brief "viewer_udp_port" configuration.
#define CONFIG_VIEWER_UDP_PORT CONFIG_GET_INSTANCE.getUnsignedInteger("viewer_udp_port")
//! @brief Config file path.
#define CONFIG_FILE_PATH CONFIG_GET_INSTANCE.getFilePath()

//! @brief Configuration-related functions in the application module.
namespace application::config
{
//! @brief Default config filename.
constexpr std::string_view defaultConfigFile = "foo.cfg";
//! @brief Default config file content.
constexpr std::string_view defaultConfiguration = "# foo configuration\n"
                                                  "\n"
                                                  "# active logger & viewer (0 = disable 1 = enable)\n"
                                                  "active_helper = 1\n"
                                                  "# logger file path (character string)\n"
                                                  "logger_path = log/foo.log\n"
                                                  "# logger write type (0 = add 1 = over)\n"
                                                  "logger_type = 0\n"
                                                  "# logger minimum level (0 = debug 1 = info 2 = warning 3 = error)\n"
                                                  "logger_level = 0\n"
                                                  "# logger actual destination (0 = file 1 = terminal 2 = both)\n"
                                                  "logger_destination = 2\n"
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
    //! @brief Get config file path.
    //! @return config file path
    [[nodiscard]] std::string getFilePath() const;
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
    //! @brief Get the full path to the default config file.
    //! @return full path to the default config file
    static std::string getFullDefaultConfigPath();

private:
    //! @brief Construct a new Config object.
    explicit Config();

    //! @brief Config file absolute path.
    const std::string filePath{getFullDefaultConfigPath()};
    //! @brief Basic delimiter.
    static constexpr char basicDelimiter{'='};
    //! @brief Comment delimiter.
    static constexpr char commentDelimiter{'#'};
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
    [[nodiscard]] static bool isFormatValid(const std::string& line);
    //! @brief Check if the line is a comment.
    //! @param line - target line
    //! @return line is comment or not
    [[nodiscard]] static bool isComment(const std::string& line);
    //! @brief Trim the line.
    //! @param line - target line
    //! @return string after removing spaces from both ends
    static std::string trimLine(const std::string& line);
};

extern void initializeConfiguration(const std::string& filename = Config::getFullDefaultConfigPath());
} // namespace application::config
