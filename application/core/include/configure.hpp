//! @file configure.hpp
//! @author ryftchen
//! @brief The declarations (configure) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2026 ryftchen. All rights reserved.

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
constexpr const char* const activateHelper = MACRO_STRINGIFY(activateHelper);
//! @brief The literal string of the "helperList" field.
constexpr const char* const helperList = MACRO_STRINGIFY(helperList);
//! @brief The literal string of the "properties" field in the object of "helpList".
constexpr const char* const properties = MACRO_STRINGIFY(properties);
//! @brief The literal string of the "required" field in the object of "helpList".
constexpr const char* const required = MACRO_STRINGIFY(required);
//! @brief The literal string of the "logger" field of "helpList".
constexpr const char* const logger = MACRO_STRINGIFY(logger);
//! @brief The literal string of the "filePath" field in the properties of "logger" object of "helpList".
constexpr const char* const filePath = MACRO_STRINGIFY(filePath);
//! @brief The literal string of the "priorityLevel" field in the properties of "logger" object of "helpList".
constexpr const char* const priorityLevel = MACRO_STRINGIFY(priorityLevel);
//! @brief The literal string of the "targetType" field in the properties of "logger" object of "helpList".
constexpr const char* const targetType = MACRO_STRINGIFY(targetType);
//! @brief The literal string of the "writeMode" field in the properties of "logger" object of "helpList".
constexpr const char* const writeMode = MACRO_STRINGIFY(writeMode);
//! @brief The literal string of the "viewer" field of "helpList".
constexpr const char* const viewer = MACRO_STRINGIFY(viewer);
//! @brief The literal string of the "tcpHost" field in the properties of "viewer" object of "helpList".
constexpr const char* const tcpHost = MACRO_STRINGIFY(tcpHost);
//! @brief The literal string of the "tcpPort" field in the properties of "viewer" object of "helpList".
constexpr const char* const tcpPort = MACRO_STRINGIFY(tcpPort);
//! @brief The literal string of the "udpHost" field in the properties of "viewer" object of "helpList".
constexpr const char* const udpHost = MACRO_STRINGIFY(udpHost);
//! @brief The literal string of the "udpPort" field in the properties of "viewer" object of "helpList".
constexpr const char* const udpPort = MACRO_STRINGIFY(udpPort);
//! @brief The literal string of the "helperTimeout" field.
constexpr const char* const helperTimeout = MACRO_STRINGIFY(helperTimeout);
} // namespace field

//! @brief Maximum access limit.
constexpr std::uint8_t maxAccessLimit = 10;
//! @brief Default configuration filename.
constexpr std::string_view defaultConfigFile = "configure/foo.json";
//! @brief Get the full path to the configuration file.
//! @param filename - configuration file path
//! @return full path to the configuration file
std::string getFullConfigPath(const std::string_view filename = defaultConfigFile);

//! @brief Configuration.
class Configure final
{
public:
    //! @brief Destroy the Configure object.
    ~Configure() = default;
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

    friend const Configure& getInstance(const std::string_view filename);

private:
    //! @brief Construct a new Configure object.
    //! @param filename - configure file path
    explicit Configure(const std::string_view filename) :
        filePath{getFullConfigPath(filename)}, dataRepo(parseConfigFile(filePath))
    {
    }

    //! @brief Full path to the configuration file.
    const std::string filePath;
    //! @brief Configuration data repository.
    const utility::json::JSON dataRepo;

    friend class Retrieve;
    //! @brief Parse the configuration file.
    //! @param configFile - configuration file
    //! @return configuration data
    static utility::json::JSON parseConfigFile(const std::string_view configFile);
    //! @brief Verify the configuration data.
    //! @param configData - configuration data
    static void verifyConfigData(const utility::json::JSON& configData);
    //! @brief Check the object in the helper list.
    //! @tparam Helper - type of helper
    //! @param helper - object of helper
    template <typename Helper>
    static void checkObjectInHelperList(const utility::json::JSON& helper);
};

//! @brief Guard for retrieve configuration.
class Retrieve final
{
public:
    //! @brief Construct a new Retrieve object.
    //! @param sem - semaphore for configuration access control
    explicit Retrieve(std::counting_semaphore<maxAccessLimit>& sem);
    //! @brief Destroy the Retrieve object.
    ~Retrieve();
    //! @brief Construct a new Retrieve object.
    Retrieve(const Retrieve&) = default;
    //! @brief Construct a new Retrieve object.
    Retrieve(Retrieve&&) noexcept = default;
    //! @brief The operator (=) overloading of Retrieve class.
    //! @return reference of the Retrieve object
    Retrieve& operator=(const Retrieve&) = delete;
    //! @brief The operator (=) overloading of Retrieve class.
    //! @return reference of the Retrieve object
    Retrieve& operator=(Retrieve&&) = delete;

    //! @brief The operator (/) overloading of Retrieve class.
    //! @param field - field name
    //! @return const reference of the JSON object
    const utility::json::JSON& operator/(const std::string& field) const;
    //! @brief The operator (()) overloading of Retrieve class.
    //! @return const reference of the JSON object
    explicit operator const utility::json::JSON&() const;

private:
    //! @brief Semaphore that controls the maximum access limit.
    std::counting_semaphore<maxAccessLimit>& sem;
};

extern Retrieve retrieveDataRepo();
extern utility::json::JSON dumpDefaultConfig();
extern bool loadSettings(const std::string_view filename = defaultConfigFile);

//! @brief Configuration details.
namespace detail
{
//! @brief The operator (/) overloading of JSON class.
//! @param json - specific JSON object
//! @param field - field name
//! @return const reference of the JSON object
inline const utility::json::JSON& operator/(const utility::json::JSON& json, const std::string& field)
{
    return json.at(field);
}

//! @brief Query the "activateHelper" configuration.
//! @return "activateHelper" configuration
inline auto activateHelper() noexcept
{
    return (retrieveDataRepo() / field::activateHelper).asBoolean();
}
//! @brief Query the "filePath" configuration in the properties of "logger" object of "helpList".
//! @return "filePath" configuration in the properties of "logger" object of "helpList"
inline auto filePath4Logger() noexcept
{
    return (retrieveDataRepo() / field::helperList / field::logger / field::properties / field::filePath).asString();
}
//! @brief Query the "priorityLevel" configuration in the properties of "logger" object of "helpList".
//! @return "priorityLevel" configuration in the properties of "logger" object of "helpList"
inline auto priorityLevel4Logger() noexcept
{
    return (retrieveDataRepo() / field::helperList / field::logger / field::properties / field::priorityLevel)
        .asIntegral();
}
//! @brief Query the "targetType" configuration in the properties of "logger" object of "helpList".
//! @return "targetType" configuration in the properties of "logger" object of "helpList"
inline auto targetType4Logger() noexcept
{
    return (retrieveDataRepo() / field::helperList / field::logger / field::properties / field::targetType)
        .asIntegral();
}
//! @brief Query the "writeMode" configuration in the properties of "logger" object of "helpList".
//! @return "writeMode" configuration in the properties of "logger" object of "helpList"
inline auto writeMode4Logger() noexcept
{
    return (retrieveDataRepo() / field::helperList / field::logger / field::properties / field::writeMode).asIntegral();
}
//! @brief Query the "tcpHost" configuration in the properties of "viewer" object of "helpList".
//! @return "tcpHost" configuration in the properties of "viewer" object of "helpList"
inline auto tcpHost4Viewer() noexcept
{
    return (retrieveDataRepo() / field::helperList / field::viewer / field::properties / field::tcpHost).asString();
}
//! @brief Query the "tcpPort" configuration in the properties of "viewer" object of "helpList".
//! @return "tcpPort" configuration in the properties of "viewer" object of "helpList"
inline auto tcpPort4Viewer() noexcept
{
    return (retrieveDataRepo() / field::helperList / field::viewer / field::properties / field::tcpPort).asIntegral();
}
//! @brief Query the "udpHost" configuration in the properties of "viewer" object of "helpList".
//! @return "udpHost" configuration in the properties of "viewer" object of "helpList"
inline auto udpHost4Viewer() noexcept
{
    return (retrieveDataRepo() / field::helperList / field::viewer / field::properties / field::udpHost).asString();
}
//! @brief Query the "udpPort" configuration in the properties of "viewer" object of "helpList".
//! @return "udpPort" configuration in the properties of "viewer" object of "helpList"
inline auto udpPort4Viewer() noexcept
{
    return (retrieveDataRepo() / field::helperList / field::viewer / field::properties / field::udpPort).asIntegral();
}
//! @brief Query the "helperTimeout" configuration.
//! @return "helperTimeout" configuration
inline auto helperTimeout() noexcept
{
    return (retrieveDataRepo() / field::helperTimeout).asIntegral();
}
} // namespace detail

//! @brief Activity configuration applied to tasks.
namespace task
{
//! @brief Alias for the memory pool for task when making multi-threading.
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
