//! @file config.hpp
//! @author ryftchen
//! @brief The declarations (config) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#pragma once

#include "utility/include/json.hpp"

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
    //! @brief Get the data repository.
    //! @return data repository
    [[nodiscard]] const utility::json::JSON& getDataRepo() const;

private:
    //! @brief Construct a new Config object.
    Config() : dataRepo(parseConfigFile(filePath)) {}

    //! @brief Configuration file absolute path.
    const std::string filePath{getFullDefaultConfigurationPath()};
    //! @brief Configuration data repository.
    const utility::json::JSON dataRepo{};

    //! @brief Parse the configuration file.
    //! @param configFile - configuration file
    //! @return configuration data
    static utility::json::JSON parseConfigFile(const std::string_view configFile);
    //! @brief Verify the configuration data.
    //! @param configData - configuration data
    static void verifyConfigData(const utility::json::JSON& configData);
    //! @brief Check the logger configuration in the helper list.
    //! @param helperList - object of the helper list
    static void checkLoggerConfigInHelperList(const utility::json::JSON& helperList);
    //! @brief Check the viewer configuration in the helper list.
    //! @param helperList - object of the helper list
    static void checkViewerConfigInHelperList(const utility::json::JSON& helperList);
};

extern utility::json::JSON getDefaultConfiguration();
extern bool loadConfiguration(const std::string_view filename = getFullDefaultConfigurationPath());

const utility::json::JSON& retrieveDataRepo();
//! @brief Configuration details.
namespace detail
{
//! @brief The operator (/) overloading of JSON class.
//! @param json - specific JSON object
//! @param field - field name
//! @return const reference of the JSON object
inline const utility::json::JSON& operator/(const utility::json::JSON& json, const std::string_view field)
{
    return json.at(field);
}

//! @brief Query the "activateHelper" configuration.
//! @return "activateHelper" configuration
inline bool activateHelper()
{
    return (retrieveDataRepo() / "activateHelper").toBoolean();
}
//! @brief Query the "helperTimeout" configuration.
//! @return "helperTimeout" configuration
inline int helperTimeout()
{
    return (retrieveDataRepo() / "helperTimeout").toIntegral();
}
//! @brief Query the "filePath" configuration in the "logger" properties of "helpList".
//! @return "filePath" configuration in the "logger" properties of "helpList"
inline std::string filePath4Logger()
{
    return (retrieveDataRepo() / "helperList" / "logger" / "properties" / "filePath").toString();
}
//! @brief Query the "minimumLevel" configuration in the "logger" properties of "helpList".
//! @return "minimumLevel" configuration in the "logger" properties of "helpList"
inline int minimumLevel4Logger()
{
    return (retrieveDataRepo() / "helperList" / "logger" / "properties" / "minimumLevel").toIntegral();
}
//! @brief Query the "usedMedium" configuration in the "logger" properties of "helpList".
//! @return "usedMedium" configuration in the "logger" properties of "helpList"
inline int usedMedium4Logger()
{
    return (retrieveDataRepo() / "helperList" / "logger" / "properties" / "usedMedium").toIntegral();
}
//! @brief Query the "writeType" configuration in the "logger" properties of "helpList".
//! @return "writeType" configuration in the "logger" properties of "helpList"
inline int writeType4Logger()
{
    return (retrieveDataRepo() / "helperList" / "logger" / "properties" / "writeType").toIntegral();
}
//! @brief Query the "tcpHost" configuration in the "viewer" properties of "helpList".
//! @return "tcpHost" configuration in the "viewer" properties of "helpList"
inline std::string tcpHost4Viewer()
{
    return (retrieveDataRepo() / "helperList" / "viewer" / "properties" / "tcpHost").toString();
}
//! @brief Query the "tcpPort" configuration in the "viewer" properties of "helpList".
//! @return "tcpPort" configuration in the "viewer" properties of "helpList"
inline int tcpPort4Viewer()
{
    return (retrieveDataRepo() / "helperList" / "viewer" / "properties" / "tcpPort").toIntegral();
}
//! @brief Query the "udpHost" configuration in the "viewer" properties of "helpList".
//! @return "udpHost" configuration in the "viewer" properties of "helpList"
inline std::string udpHost4Viewer()
{
    return (retrieveDataRepo() / "helperList" / "viewer" / "properties" / "udpHost").toString();
}
//! @brief Query the "udpPort" configuration in the "viewer" properties of "helpList".
//! @return "udpPort" configuration in the "viewer" properties of "helpList"
inline int udpPort4Viewer()
{
    return (retrieveDataRepo() / "helperList" / "viewer" / "properties" / "udpPort").toIntegral();
}
} // namespace detail
} // namespace config
} // namespace application
