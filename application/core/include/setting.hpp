//! @file setting.hpp
//! @author ryftchen
//! @brief The declarations (setting) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2026 ryftchen. All rights reserved.

#pragma once

#include "build.hpp"

#include "utility/include/json.hpp"
#include "utility/include/macro.hpp"
#include "utility/include/memory.hpp"
#include "utility/include/thread.hpp"

//! @brief The application module.
namespace application // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Settings-related functions in the application module.
namespace setting
{
//! @brief The literal string of the field.
namespace field
{
//! @brief The literal string of the "helper_enabled" field.
constexpr const char* const helperEnabled = MACRO_STRINGIFY(helper_enabled);
//! @brief The literal string of the "helper_registry" field.
constexpr const char* const helperRegistry = MACRO_STRINGIFY(helper_registry);
//! @brief The literal string of the "properties" field in the object of "helper_registry".
constexpr const char* const properties = MACRO_STRINGIFY(properties);
//! @brief The literal string of the "required" field in the object of "helper_registry".
constexpr const char* const required = MACRO_STRINGIFY(required);
//! @brief The literal string of the "logger" field of "helper_registry".
constexpr const char* const logger = MACRO_STRINGIFY(logger);
//! @brief The literal string of the "file_path" field in the properties of "logger" object of "helper_registry".
constexpr const char* const filePath = MACRO_STRINGIFY(file_path);
//! @brief The literal string of the "priority_level" field in the properties of "logger" object of "helper_registry".
constexpr const char* const priorityLevel = MACRO_STRINGIFY(priority_level);
//! @brief The literal string of the "target_type" field in the properties of "logger" object of "helper_registry".
constexpr const char* const targetType = MACRO_STRINGIFY(target_type);
//! @brief The literal string of the "write_mode" field in the properties of "logger" object of "helper_registry".
constexpr const char* const writeMode = MACRO_STRINGIFY(write_mode);
//! @brief The literal string of the "viewer" field of "helper_registry".
constexpr const char* const viewer = MACRO_STRINGIFY(viewer);
//! @brief The literal string of the "tcp_host" field in the properties of "viewer" object of "helper_registry".
constexpr const char* const tcpHost = MACRO_STRINGIFY(tcp_host);
//! @brief The literal string of the "tcp_port" field in the properties of "viewer" object of "helper_registry".
constexpr const char* const tcpPort = MACRO_STRINGIFY(tcp_port);
//! @brief The literal string of the "udp_host" field in the properties of "viewer" object of "helper_registry".
constexpr const char* const udpHost = MACRO_STRINGIFY(udp_host);
//! @brief The literal string of the "udp_port" field in the properties of "viewer" object of "helper_registry".
constexpr const char* const udpPort = MACRO_STRINGIFY(udp_port);
//! @brief The literal string of the "helper_timeout" field.
constexpr const char* const helperTimeout = MACRO_STRINGIFY(helper_timeout);
} // namespace field

//! @brief Maximum access limit.
constexpr std::uint8_t maxAccessLimit = 10;
//! @brief Default configuration filename.
constexpr std::string_view defaultConfigFile = "conf/" BUILD_APP_NAME ".json";

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
    Retrieve(const Retrieve&) = delete;
    //! @brief Construct a new Retrieve object.
    Retrieve(Retrieve&&) = delete;
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

//! @brief Access controller. For the instance.
//! @tparam Helper - type of helper
template <typename Helper>
class Controller
{
public:
    //! @brief Wait for the helper to start.
    void startup() const;
    //! @brief Wait for the helper to stop.
    void shutdown() const;
    //! @brief Request to reset the helper.
    void reload() const;

private:
    //! @brief Instance to be accessed.
    const std::shared_ptr<Helper> srv{Helper::getInstance()};
};

//! @brief Semantic versioning (major.minor.patch).
class Version
{
public:
    //! @brief Construct a new Version object.
    //! @param version - version string
    explicit Version(const std::string_view version);

    //! @brief The operator (==) overloading of Version class.
    //! @return be equal or not
    bool operator==(const Version&) const = default;
    //! @brief The operator (<=>) overloading of Version class.
    //! @param rhs - right-hand side
    //! @return comparison result
    std::strong_ordering operator<=>(const Version& rhs) const;

    //! @brief Convert the version to a string.
    //! @return version string (major.minor.patch)
    [[nodiscard]] std::string toString() const;

private:
    //! @brief The version parts.
    std::array<std::string, 3> parts{};
};

extern Retrieve retrieveDataRepo();
extern utility::json::JSON dumpDefaultConfig();
extern bool loadSettings(const std::string_view filename = defaultConfigFile);

//! @brief Query the configuration.
namespace config
{
//! @brief The operator (/) overloading of JSON class.
//! @param json - specific JSON object
//! @param field - field name
//! @return const reference of the JSON object
inline const utility::json::JSON& operator/(const utility::json::JSON& json, const std::string& field)
{
    return json.at(field);
}

//! @brief Query the "helper_enabled" configuration.
//! @return "helper_enabled" configuration
inline auto helperEnabled() noexcept
{
    return (retrieveDataRepo() / field::helperEnabled).asBoolean();
}
//! @brief Query the "file_path" configuration in the properties of "logger" object of "helper_registry".
//! @return "file_path" configuration in the properties of "logger" object of "helper_registry"
inline auto filePath4Logger() noexcept
{
    return (retrieveDataRepo() / field::helperRegistry / field::logger / field::properties / field::filePath)
        .asString();
}
//! @brief Query the "priority_level" configuration in the properties of "logger" object of "helper_registry".
//! @return "priority_level" configuration in the properties of "logger" object of "helper_registry"
inline auto priorityLevel4Logger() noexcept
{
    return (retrieveDataRepo() / field::helperRegistry / field::logger / field::properties / field::priorityLevel)
        .asIntegral();
}
//! @brief Query the "target_type" configuration in the properties of "logger" object of "helper_registry".
//! @return "target_type" configuration in the properties of "logger" object of "helper_registry"
inline auto targetType4Logger() noexcept
{
    return (retrieveDataRepo() / field::helperRegistry / field::logger / field::properties / field::targetType)
        .asIntegral();
}
//! @brief Query the "write_mode" configuration in the properties of "logger" object of "helper_registry".
//! @return "write_mode" configuration in the properties of "logger" object of "helper_registry"
inline auto writeMode4Logger() noexcept
{
    return (retrieveDataRepo() / field::helperRegistry / field::logger / field::properties / field::writeMode)
        .asIntegral();
}
//! @brief Query the "tcp_host" configuration in the properties of "viewer" object of "helper_registry".
//! @return "tcp_host" configuration in the properties of "viewer" object of "helper_registry"
inline auto tcpHost4Viewer() noexcept
{
    return (retrieveDataRepo() / field::helperRegistry / field::viewer / field::properties / field::tcpHost).asString();
}
//! @brief Query the "tcp_port" configuration in the properties of "viewer" object of "helper_registry".
//! @return "tcp_port" configuration in the properties of "viewer" object of "helper_registry"
inline auto tcpPort4Viewer() noexcept
{
    return (retrieveDataRepo() / field::helperRegistry / field::viewer / field::properties / field::tcpPort)
        .asIntegral();
}
//! @brief Query the "udp_host" configuration in the properties of "viewer" object of "helper_registry".
//! @return "udp_host" configuration in the properties of "viewer" object of "helper_registry"
inline auto udpHost4Viewer() noexcept
{
    return (retrieveDataRepo() / field::helperRegistry / field::viewer / field::properties / field::udpHost).asString();
}
//! @brief Query the "udp_port" configuration in the properties of "viewer" object of "helper_registry".
//! @return "udp_port" configuration in the properties of "viewer" object of "helper_registry"
inline auto udpPort4Viewer() noexcept
{
    return (retrieveDataRepo() / field::helperRegistry / field::viewer / field::properties / field::udpPort)
        .asIntegral();
}
//! @brief Query the "helper_timeout" configuration.
//! @return "helper_timeout" configuration
inline auto helperTimeout() noexcept
{
    return (retrieveDataRepo() / field::helperTimeout).asFloating();
}
} // namespace config

//! @brief Activity settings applied to tasks.
namespace activity
{
//! @brief Alias for the memory pool for task when making multi-threading.
using Pooling = utility::memory::Memory<utility::thread::Thread>;
extern Pooling& cyclePool();
//! @brief Preset full name for the task.
//! @param cli - sub-cli name
//! @param cat - category name
//! @param cho - choice name
//! @return full name
inline std::string presetName(const std::string_view cli, const std::string_view cat, const std::string_view cho)
{
    return '@' + std::string{cli} + '_' + std::string{cat} + '_' + std::string{cho};
}
} // namespace activity
} // namespace setting
} // namespace application
