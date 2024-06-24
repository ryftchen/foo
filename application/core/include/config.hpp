//! @file config.hpp
//! @author ryftchen
//! @brief The declarations (config) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#pragma once

#ifndef __PRECOMPILED_HEADER
#include <string>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

#include "utility/include/json.hpp"

//! @brief "activateHelper" configuration.
#define CONFIG_ACTIVATE_HELPER application::config::queryConfiguration().at("activateHelper").toBoolean()
//! @brief "helperTimeout" configuration.
#define CONFIG_HELPER_TIMEOUT application::config::queryConfiguration().at("helperTimeout").toIntegral()
//! @brief "filePath" configuration in the logger properties.
#define CONFIG_HELPER_LOGGER_FILE_PATH \
    application::config::queryConfiguration().at("helperList").at("logger").at("properties").at("filePath").toString()
//! @brief "minimumLevel" configuration in the logger properties.
#define CONFIG_HELPER_LOGGER_MINIMUM_LEVEL    \
    application::config::queryConfiguration() \
        .at("helperList")                     \
        .at("logger")                         \
        .at("properties")                     \
        .at("minimumLevel")                   \
        .toIntegral()
//! @brief "usedMedium" configuration in the logger properties.
#define CONFIG_HELPER_LOGGER_USED_MEDIUM      \
    application::config::queryConfiguration() \
        .at("helperList")                     \
        .at("logger")                         \
        .at("properties")                     \
        .at("usedMedium")                     \
        .toIntegral()
//! @brief "writeType" configuration in the logger properties.
#define CONFIG_HELPER_LOGGER_WRITE_TYPE       \
    application::config::queryConfiguration() \
        .at("helperList")                     \
        .at("logger")                         \
        .at("properties")                     \
        .at("writeType")                      \
        .toIntegral()
//! @brief "tcpHost" configuration in the viewer properties.
#define CONFIG_HELPER_VIEWER_TCP_HOST \
    application::config::queryConfiguration().at("helperList").at("viewer").at("properties").at("tcpHost").toString()
//! @brief "tcpPort" configuration in the viewer properties.
#define CONFIG_HELPER_VIEWER_TCP_PORT \
    application::config::queryConfiguration().at("helperList").at("viewer").at("properties").at("tcpPort").toIntegral()
//! @brief "udpHost" configuration in the viewer properties.
#define CONFIG_HELPER_VIEWER_UDP_HOST \
    application::config::queryConfiguration().at("helperList").at("viewer").at("properties").at("udpHost").toString()
//! @brief "udpPort" configuration in the viewer properties.
#define CONFIG_HELPER_VIEWER_UDP_PORT \
    application::config::queryConfiguration().at("helperList").at("viewer").at("properties").at("udpPort").toIntegral()
//! @brief Config file path.
#define CONFIG_FILE_PATH application::config::Config::getInstance().cfgFilePath()

//! @brief The application module.
namespace application // NOLINT (modernize-concat-nested-namespaces)
{
//! @brief Configuration-related functions in the application module.
namespace config
{
//! @brief Default configuration filename.
constexpr std::string_view defaultConfigurationFile = "config/foo.json";
//! @brief Maximum access limit.
constexpr std::uint8_t maxAccessLimit = 10;

extern std::string getFullDefaultConfigurationPath();

//! @brief Configuration.
class Config
{
public:
    //! @brief Destroy the Config object.
    virtual ~Config() = default;
    //! @brief Construct a new Config object.
    Config(const Config&) = delete;
    //! @brief Construct a new Config object.
    Config(Config&&) = delete;
    //! @brief The operator (=) overloading of Config class.
    //! @return reference of the Config object
    Config& operator=(const Config&) = delete;
    //! @brief The operator (=) overloading of Config class.
    //! @return reference of the Config object
    Config& operator=(Config&&) = delete;

    //! @brief Get the Config instance.
    //! @return reference of the Config object
    static Config& getInstance();
    //! @brief Get the config data.
    //! @return config data
    [[nodiscard]] const utility::json::JSON& cfgData() const;
    //! @brief Get the config file path.
    //! @return config file path
    [[nodiscard]] std::string cfgFilePath() const;

private:
    //! @brief Construct a new Config object.
    Config() : data(parseConfigFile(filePath)) {}

    //! @brief Config file absolute path.
    const std::string filePath{getFullDefaultConfigurationPath()};
    //! @brief Config data table.
    const utility::json::JSON data{};

    //! @brief Parse the configuration file.
    //! @param configFile - configuration file
    //! @return configuration data
    static utility::json::JSON parseConfigFile(const std::string& configFile);
    //! @brief Verify the configuration data.
    //! @param configData - configuration data
    static void verifyConfigData(const utility::json::JSON& configData);
    //! @brief Check the logger configuration in the helper list.
    //! @param configData - configuration data
    static void checkLoggerConfigInHelperList(const utility::json::JSON& configData);
    //! @brief Check the viewer configuration in the helper list.
    //! @param configData - configuration data
    static void checkViewerConfigInHelperList(const utility::json::JSON& configData);
};

extern void forcedConfigurationUpdateByDefault(const std::string& filename);
extern bool loadConfiguration(const std::string& filename = getFullDefaultConfigurationPath());
extern const utility::json::JSON& queryConfiguration();
} // namespace config
} // namespace application
