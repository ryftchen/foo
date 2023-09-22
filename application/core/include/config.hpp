//! @file config.hpp
//! @author ryftchen
//! @brief The declarations (config) in the application module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen. All rights reserved.

#pragma once

#ifndef __PRECOMPILED_HEADER
#include <string>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER
#include "utility/include/json.hpp"

//! @brief Get the config instance.
#define CONFIG_GET_INSTANCE application::config::Config::getInstance()
//! @brief "activeHelper" configuration.
#define CONFIG_ACTIVE_HELPER CONFIG_GET_INSTANCE.getData()["activeHelper"].toBool()
//! @brief "filePath" configuration in the logger properties.
#define CONFIG_LOGGER_PATH CONFIG_GET_INSTANCE.getData()["helperSetting"]["logger"]["properties"]["filePath"].toString()
//! @brief "writeType" configuration in the logger properties.
#define CONFIG_LOGGER_TYPE CONFIG_GET_INSTANCE.getData()["helperSetting"]["logger"]["properties"]["writeType"].toInt()
//! @brief "minimumLevel" configuration in the logger properties.
#define CONFIG_LOGGER_LEVEL \
    CONFIG_GET_INSTANCE.getData()["helperSetting"]["logger"]["properties"]["minimumLevel"].toInt()
//! @brief "usedMedium" configuration in the logger properties.
#define CONFIG_LOGGER_MEDIUM \
    CONFIG_GET_INSTANCE.getData()["helperSetting"]["logger"]["properties"]["usedMedium"].toInt()
//! @brief "tcpHost" configuration in the viewer properties.
#define CONFIG_VIEWER_TCP_HOST \
    CONFIG_GET_INSTANCE.getData()["helperSetting"]["viewer"]["properties"]["tcpHost"].toString()
//! @brief "tcpPort" configuration in the viewer properties.
#define CONFIG_VIEWER_TCP_PORT CONFIG_GET_INSTANCE.getData()["helperSetting"]["viewer"]["properties"]["tcpPort"].toInt()
//! @brief "udpHost" configuration in the viewer properties.
#define CONFIG_VIEWER_UDP_HOST \
    CONFIG_GET_INSTANCE.getData()["helperSetting"]["viewer"]["properties"]["udpHost"].toString()
//! @brief "udpPort" configuration in the viewer properties.
#define CONFIG_VIEWER_UDP_PORT CONFIG_GET_INSTANCE.getData()["helperSetting"]["viewer"]["properties"]["udpPort"].toInt()
//! @brief Config file path.
#define CONFIG_FILE_PATH CONFIG_GET_INSTANCE.getFilePath()

//! @brief Configuration-related functions in the application module.
namespace application::config
{
//! @brief Default config filename.
constexpr std::string_view defaultConfigFile = "foo.json";

std::string getFullDefaultConfigPath();

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
    //! @brief Get the data.
    //! @return reference of data
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
};

extern utility::json::JSON getDefaultConfiguration();
extern void initializeConfiguration(const std::string& filename = getFullDefaultConfigPath());
} // namespace application::config
