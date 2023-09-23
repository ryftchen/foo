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
#define CONFIG_ACTIVE_HELPER CONFIG_GET_INSTANCE.getData()["activeHelper"].toBoolean()
//! @brief "filePath" configuration in the logger properties.
#define CONFIG_LOGGER_FILE_PATH \
    CONFIG_GET_INSTANCE.getData()["helperTable"]["logger"]["properties"]["filePath"].toString()
//! @brief "minimumLevel" configuration in the logger properties.
#define CONFIG_LOGGER_MINIMUM_LEVEL \
    CONFIG_GET_INSTANCE.getData()["helperTable"]["logger"]["properties"]["minimumLevel"].toIntegral()
//! @brief "usedMedium" configuration in the logger properties.
#define CONFIG_LOGGER_USED_MEDIUM \
    CONFIG_GET_INSTANCE.getData()["helperTable"]["logger"]["properties"]["usedMedium"].toIntegral()
//! @brief "writeType" configuration in the logger properties.
#define CONFIG_LOGGER_WRITE_TYPE \
    CONFIG_GET_INSTANCE.getData()["helperTable"]["logger"]["properties"]["writeType"].toIntegral()
//! @brief "tcpHost" configuration in the viewer properties.
#define CONFIG_VIEWER_TCP_HOST \
    CONFIG_GET_INSTANCE.getData()["helperTable"]["viewer"]["properties"]["tcpHost"].toString()
//! @brief "tcpPort" configuration in the viewer properties.
#define CONFIG_VIEWER_TCP_PORT \
    CONFIG_GET_INSTANCE.getData()["helperTable"]["viewer"]["properties"]["tcpPort"].toIntegral()
//! @brief "udpHost" configuration in the viewer properties.
#define CONFIG_VIEWER_UDP_HOST \
    CONFIG_GET_INSTANCE.getData()["helperTable"]["viewer"]["properties"]["udpHost"].toString()
//! @brief "udpPort" configuration in the viewer properties.
#define CONFIG_VIEWER_UDP_PORT \
    CONFIG_GET_INSTANCE.getData()["helperTable"]["viewer"]["properties"]["udpPort"].toIntegral()
//! @brief Config file path.
#define CONFIG_FILE_PATH CONFIG_GET_INSTANCE.getFilePath()

//! @brief Configuration-related functions in the application module.
namespace application::config
{
//! @brief Default config filename.
constexpr std::string_view defaultConfigFile = "foo.json";

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
};

extern utility::json::JSON getDefaultConfiguration();
extern void initializeConfiguration(const std::string& filename = getFullDefaultConfigPath());
} // namespace application::config
