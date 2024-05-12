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

//! @brief Get the config instance.
#define CONFIG_GET_INSTANCE application::config::Config::getInstance()
//! @brief "activateHelper" configuration.
#define CONFIG_ACTIVATE_HELPER CONFIG_GET_INSTANCE.getData()["activateHelper"].toBoolean()
//! @brief "helperTimeout" configuration.
#define CONFIG_HELPER_TIMEOUT CONFIG_GET_INSTANCE.getData()["helperTimeout"].toIntegral()
//! @brief Logger properties in "helperList" configuration.
#define CONFIG_HELPER_LOGGER_PROPERTIES CONFIG_GET_INSTANCE.getData()["helperList"]["logger"]["properties"]
//! @brief Viewer properties in "helperList" configuration.
#define CONFIG_HELPER_VIEWER_PROPERTIES CONFIG_GET_INSTANCE.getData()["helperList"]["viewer"]["properties"]
//! @brief "filePath" configuration in the logger properties.
#define CONFIG_HELPER_LOGGER_FILE_PATH CONFIG_HELPER_LOGGER_PROPERTIES["filePath"].toString()
//! @brief "minimumLevel" configuration in the logger properties.
#define CONFIG_HELPER_LOGGER_MINIMUM_LEVEL CONFIG_HELPER_LOGGER_PROPERTIES["minimumLevel"].toIntegral()
//! @brief "usedMedium" configuration in the logger properties.
#define CONFIG_HELPER_LOGGER_USED_MEDIUM CONFIG_HELPER_LOGGER_PROPERTIES["usedMedium"].toIntegral()
//! @brief "writeType" configuration in the logger properties.
#define CONFIG_HELPER_LOGGER_WRITE_TYPE CONFIG_HELPER_LOGGER_PROPERTIES["writeType"].toIntegral()
//! @brief "tcpHost" configuration in the viewer properties.
#define CONFIG_HELPER_VIEWER_TCP_HOST CONFIG_HELPER_VIEWER_PROPERTIES["tcpHost"].toString()
//! @brief "tcpPort" configuration in the viewer properties.
#define CONFIG_HELPER_VIEWER_TCP_PORT CONFIG_HELPER_VIEWER_PROPERTIES["tcpPort"].toIntegral()
//! @brief "udpHost" configuration in the viewer properties.
#define CONFIG_HELPER_VIEWER_UDP_HOST CONFIG_HELPER_VIEWER_PROPERTIES["udpHost"].toString()
//! @brief "udpPort" configuration in the viewer properties.
#define CONFIG_HELPER_VIEWER_UDP_PORT CONFIG_HELPER_VIEWER_PROPERTIES["udpPort"].toIntegral()
//! @brief Config file path.
#define CONFIG_FILE_PATH CONFIG_GET_INSTANCE.getFilePath()

//! @brief The application module.
namespace application // NOLINT (modernize-concat-nested-namespaces)
{
//! @brief Configuration-related functions in the application module.
namespace config
{
//! @brief Default config filename.
constexpr std::string_view defaultConfigFile = "config/foo.json";

extern std::string getFullDefaultConfigPath();

//! @brief Configuration.
class Config
{
public:
    //! @brief Destroy the Config object.
    virtual ~Config() = default;
    //! @brief Construct a new Config object.
    Config(const Config&) = delete;
    //! @brief The operator (=) overloading of Config class.
    //! @return reference of the Config object
    Config& operator=(const Config&) = delete;

    //! @brief Get the Config instance.
    //! @return reference of the Config object
    static Config& getInstance();
    //! @brief Get the data.
    //! @return reference of the data
    utility::json::JSON& getData();
    //! @brief Get config file path.
    //! @return config file path
    [[nodiscard]] std::string getFilePath() const;

private:
    //! @brief Construct a new Config object.
    explicit Config();

    //! @brief Configuration data.
    utility::json::JSON data;
    //! @brief Config file absolute path.
    const std::string filePath{getFullDefaultConfigPath()};

    //! @brief Parse the configuration file.
    //! @param filename - configuration file
    void parseFile(const std::string& filename);
    //! @brief Verify the data.
    void verifyData();
    //! @brief Check the logger configuration in the helper list.
    //! @return be legal or illegal
    bool checkLoggerConfigInHelperList();
    //! @brief Check the viewer configuration in the helper list.
    //! @return be legal or illegal
    bool checkViewerConfigInHelperList();
};

extern utility::json::JSON getDefaultConfiguration();
extern void loadConfiguration(const std::string& filename = getFullDefaultConfigPath());
} // namespace config
} // namespace application
