//! @file configure.hpp
//! @author ryftchen
//! @brief The declarations (configure) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include "utility/include/json.hpp"

//! @brief The application module.
namespace application // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Configuration-related functions in the application module.
namespace configure
{
//! @brief Default configuration filename.
constexpr std::string_view defaultConfigFile = "configure/foo.json";
//! @brief Maximum access limit.
constexpr std::uint8_t maxAccessLimit = 10;

//! @brief The literal string of the field.
namespace field
{
//! @brief The literal string of the "activateHelper" field.
constexpr std::string_view activateHelper = "activateHelper";
//! @brief The literal string of the "helperList" field.
constexpr std::string_view helperList = "helperList";
//! @brief The literal string of the "properties" field in the object of "helpList".
constexpr std::string_view properties = "properties";
//! @brief The literal string of the "required" field in the object of "helpList".
constexpr std::string_view required = "required";
//! @brief The literal string of the "logger" field of "helpList".
constexpr std::string_view logger = "logger";
//! @brief The literal string of the "filePath" field in the properties of "logger" object of "helpList".
constexpr std::string_view filePath = "filePath";
//! @brief The literal string of the "priorityLevel" field in the properties of "logger" object of "helpList".
constexpr std::string_view priorityLevel = "priorityLevel";
//! @brief The literal string of the "targetType" field in the properties of "logger" object of "helpList".
constexpr std::string_view targetType = "targetType";
//! @brief The literal string of the "writeMode" field in the properties of "logger" object of "helpList".
constexpr std::string_view writeMode = "writeMode";
//! @brief The literal string of the "viewer" field of "helpList".
constexpr std::string_view viewer = "viewer";
//! @brief The literal string of the "tcpHost" field in the properties of "viewer" object of "helpList".
constexpr std::string_view tcpHost = "tcpHost";
//! @brief The literal string of the "tcpPort" field in the properties of "viewer" object of "helpList".
constexpr std::string_view tcpPort = "tcpPort";
//! @brief The literal string of the "udpHost" field in the properties of "viewer" object of "helpList".
constexpr std::string_view udpHost = "udpHost";
//! @brief The literal string of the "udpPort" field in the properties of "viewer" object of "helpList".
constexpr std::string_view udpPort = "udpPort";
//! @brief The literal string of the "helperTimeout" field.
constexpr std::string_view helperTimeout = "helperTimeout";
} // namespace field

extern std::string getFullDefaultConfigPath();

//! @brief Configuration.
class Configure final
{
public:
    //! @brief Destroy the Configure object.
    virtual ~Configure() = default;
    //! @brief Construct a new Configure object.
    Configure(const Configure&) = delete;
    //! @brief Construct a new Configure object.
    Configure(Configure&&) = delete;
    //! @brief The operator (=) overloading of Configure class.
    //! @return reference of the Configure object
    Configure& operator=(const Configure&) = delete;
    //! @brief The operator (=) overloading of Configure class.
    //! @return reference of the Configure object
    Configure& operator=(Configure&&) = delete;

    //! @brief Get the Configure instance.
    //! @return reference of the Configure object
    static Configure& getInstance();
    //! @brief Interface used to retrieve.
    //! @return data repository
    [[nodiscard]] const utility::json::JSON& retrieve() const;

private:
    //! @brief Construct a new Configure object.
    Configure() : dataRepo(parseConfigFile(filePath)) {}

    //! @brief Full path to the configuration file.
    const std::string filePath{getFullDefaultConfigPath()};
    //! @brief Configuration data repository.
    const utility::json::JSON dataRepo{};

    //! @brief Parse the configuration file.
    //! @param configFile - configuration file
    //! @return configuration data
    static utility::json::JSON parseConfigFile(const std::string_view configFile);
    //! @brief Verify the configuration data.
    //! @param configData - configuration data
    static void verifyConfigData(const utility::json::JSON& configData);
    //! @brief Check the object in the helper list.
    //! @tparam T - type of helper
    //! @param helperList - object in the helper list
    template <class T>
    static void checkObjectInHelperList(const utility::json::JSON& helperList);
};

extern utility::json::JSON getDefaultConfiguration();
extern bool loadConfiguration(const std::string_view filename = getFullDefaultConfigPath());

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
    return (retrieveDataRepo() / field::activateHelper).toBoolean();
}
//! @brief Query the "filePath" configuration in the properties of "logger" object of "helpList".
//! @return "filePath" configuration in the properties of "logger" object of "helpList"
inline std::string filePath4Logger()
{
    return (retrieveDataRepo() / field::helperList / field::logger / field::properties / field::filePath).toString();
}
//! @brief Query the "priorityLevel" configuration in the properties of "logger" object of "helpList".
//! @return "priorityLevel" configuration in the properties of "logger" object of "helpList"
inline int priorityLevel4Logger()
{
    return (retrieveDataRepo() / field::helperList / field::logger / field::properties / field::priorityLevel)
        .toIntegral();
}
//! @brief Query the "targetType" configuration in the properties of "logger" object of "helpList".
//! @return "targetType" configuration in the properties of "logger" object of "helpList"
inline int targetType4Logger()
{
    return (retrieveDataRepo() / field::helperList / field::logger / field::properties / field::targetType)
        .toIntegral();
}
//! @brief Query the "writeMode" configuration in the properties of "logger" object of "helpList".
//! @return "writeMode" configuration in the properties of "logger" object of "helpList"
inline int writeMode4Logger()
{
    return (retrieveDataRepo() / field::helperList / field::logger / field::properties / field::writeMode).toIntegral();
}
//! @brief Query the "tcpHost" configuration in the properties of "viewer" object of "helpList".
//! @return "tcpHost" configuration in the properties of "viewer" object of "helpList"
inline std::string tcpHost4Viewer()
{
    return (retrieveDataRepo() / field::helperList / field::viewer / field::properties / field::tcpHost).toString();
}
//! @brief Query the "tcpPort" configuration in the properties of "viewer" object of "helpList".
//! @return "tcpPort" configuration in the properties of "viewer" object of "helpList"
inline int tcpPort4Viewer()
{
    return (retrieveDataRepo() / field::helperList / field::viewer / field::properties / field::tcpPort).toIntegral();
}
//! @brief Query the "udpHost" configuration in the properties of "viewer" object of "helpList".
//! @return "udpHost" configuration in the properties of "viewer" object of "helpList"
inline std::string udpHost4Viewer()
{
    return (retrieveDataRepo() / field::helperList / field::viewer / field::properties / field::udpHost).toString();
}
//! @brief Query the "udpPort" configuration in the properties of "viewer" object of "helpList".
//! @return "udpPort" configuration in the properties of "viewer" object of "helpList"
inline int udpPort4Viewer()
{
    return (retrieveDataRepo() / field::helperList / field::viewer / field::properties / field::udpPort).toIntegral();
}
//! @brief Query the "helperTimeout" configuration.
//! @return "helperTimeout" configuration
inline int helperTimeout()
{
    return (retrieveDataRepo() / field::helperTimeout).toIntegral();
}
} // namespace detail
} // namespace configure
} // namespace application
