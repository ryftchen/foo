//! @file configure.hpp
//! @author ryftchen
//! @brief The declarations (configure) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include "utility/include/json.hpp"
#include "utility/include/macro.hpp"
#include "utility/include/memory.hpp"
#include "utility/include/thread.hpp"

//! @brief The application module.
namespace application // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Configuration-related functions in the application module.
namespace configure
{
//! @brief The literal string of the field.
namespace field
{
//! @brief The literal string of the "activateHelper" field.
constexpr std::string_view activateHelper = MACRO_STRINGIFY(activateHelper);
//! @brief The literal string of the "helperList" field.
constexpr std::string_view helperList = MACRO_STRINGIFY(helperList);
//! @brief The literal string of the "properties" field in the object of "helpList".
constexpr std::string_view properties = MACRO_STRINGIFY(properties);
//! @brief The literal string of the "required" field in the object of "helpList".
constexpr std::string_view required = MACRO_STRINGIFY(required);
//! @brief The literal string of the "logger" field of "helpList".
constexpr std::string_view logger = MACRO_STRINGIFY(logger);
//! @brief The literal string of the "filePath" field in the properties of "logger" object of "helpList".
constexpr std::string_view filePath = MACRO_STRINGIFY(filePath);
//! @brief The literal string of the "priorityLevel" field in the properties of "logger" object of "helpList".
constexpr std::string_view priorityLevel = MACRO_STRINGIFY(priorityLevel);
//! @brief The literal string of the "targetType" field in the properties of "logger" object of "helpList".
constexpr std::string_view targetType = MACRO_STRINGIFY(targetType);
//! @brief The literal string of the "writeMode" field in the properties of "logger" object of "helpList".
constexpr std::string_view writeMode = MACRO_STRINGIFY(writeMode);
//! @brief The literal string of the "viewer" field of "helpList".
constexpr std::string_view viewer = MACRO_STRINGIFY(viewer);
//! @brief The literal string of the "tcpHost" field in the properties of "viewer" object of "helpList".
constexpr std::string_view tcpHost = MACRO_STRINGIFY(tcpHost);
//! @brief The literal string of the "tcpPort" field in the properties of "viewer" object of "helpList".
constexpr std::string_view tcpPort = MACRO_STRINGIFY(tcpPort);
//! @brief The literal string of the "udpHost" field in the properties of "viewer" object of "helpList".
constexpr std::string_view udpHost = MACRO_STRINGIFY(udpHost);
//! @brief The literal string of the "udpPort" field in the properties of "viewer" object of "helpList".
constexpr std::string_view udpPort = MACRO_STRINGIFY(udpPort);
//! @brief The literal string of the "helperTimeout" field.
constexpr std::string_view helperTimeout = MACRO_STRINGIFY(helperTimeout);
} // namespace field

//! @brief Default configuration filename.
constexpr std::string_view defaultConfigFile = "configure/foo.json";
//! @brief Get the full path to the configuration file.
//! @param filename - configuration file path
//! @return full path to the configuration file
std::string getFullConfigPath(const std::string_view filename = defaultConfigFile);
//! @brief Retrieve data repository.
//! @return current configuration data repository
const utility::json::JSON& retrieveDataRepo();

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
    //! @param filename - configure file path
    //! @return reference of the Configure object
    static Configure& getInstance(const std::string_view filename = defaultConfigFile);
    //! @brief Interface used to retrieve.
    //! @return data repository
    [[nodiscard]] const utility::json::JSON& retrieve() const;

private:
    //! @brief Construct a new Configure object.
    //! @param filename - configure file path
    explicit Configure(const std::string_view filename) :
        filePath{getFullConfigPath(filename)}, dataRepo(parseConfigFile(filePath))
    {
    }

    //! @brief Full path to the configuration file.
    const std::string filePath{};
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
    //! @param helper - object of helper
    template <typename T>
    static void checkObjectInHelperList(const utility::json::JSON& helper);
};

extern utility::json::JSON getDefaultConfiguration();
extern bool loadConfiguration(const std::string_view filename = defaultConfigFile);

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

//! @brief Activity configuration applied to tasks.
namespace task
{
//! @brief Alias for memory pool for task when making multi-threading.
using ResourcePool = utility::memory::Memory<utility::thread::Thread>;
extern ResourcePool& resourcePool();
//! @brief Preset full name for the task.
//! @param cli - sub-cli name
//! @param cat - category name
//! @param cho - choice name
//! @return full name
inline std::string presetName(const std::string_view cli, const std::string_view cat, const std::string_view cho)
{
    return '@' + std::string{cli} + '_' + std::string{cat} + '_' + std::string{cho};
}
} // namespace task
} // namespace configure
} // namespace application
