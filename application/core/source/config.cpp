//! @file config.cpp
//! @author ryftchen
//! @brief The definitions (config) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#include "config.hpp"
#include "log.hpp"

#ifndef __PRECOMPILED_HEADER
#include <cassert>
#include <filesystem>
#include <iterator>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

namespace application::config
{
Config& Config::getInstance()
{
    static Config cfg{};
    return cfg;
}

const utility::json::JSON& Config::getData() const
{
    return data;
}

utility::json::JSON Config::parseConfigFile(const std::string& configFile)
{
    if (!std::filesystem::exists(configFile))
    {
        throw std::runtime_error("Configuration file " + configFile + " is missing.");
    }

    const auto& configRows = utility::io::getFileContents(configFile, true);
    std::ostringstream os;
    std::copy(configRows.cbegin(), configRows.cend(), std::ostream_iterator<std::string>(os, ""));
    const auto& preprocessedData = utility::json::JSON::load(os.str());
    verifyConfigData(preprocessedData);

    return preprocessedData;
}

void Config::verifyConfigData(const utility::json::JSON& configData)
{
    bool isVerified = configData.at("activateHelper").isBooleanType();
    isVerified &= configData.at("helperList").isObjectType();
    isVerified &=
        (configData.at("helperTimeout").isIntegralType() && (configData.at("helperTimeout").toIntegral() >= 0));
    if (!isVerified)
    {
        throw std::runtime_error("Illegal configuration: " + configData.toUnescapedString() + '.');
    }

    checkLoggerConfigInHelperList(configData);
    checkViewerConfigInHelperList(configData);
}

void Config::checkLoggerConfigInHelperList(const utility::json::JSON& configData)
{
    using utility::common::EnumCheck, utility::common::operator""_bkdrHash;

    bool isVerified = true;
    const auto loggerObject = configData.at("helperList").at("logger");
    isVerified &= loggerObject.isObjectType();
    const auto loggerProperties = loggerObject.at("properties"), loggerRequired = loggerObject.at("required");
    isVerified &= loggerProperties.isObjectType();
    isVerified &= loggerRequired.isArrayType();
    isVerified &= (loggerProperties.size() == loggerRequired.length());
    for (const auto& item : loggerRequired.arrayRange())
    {
        isVerified &= item.isStringType();
        isVerified &= loggerProperties.hasKey(item.toString());
    }

    for (const auto& [key, item] : loggerProperties.objectRange())
    {
        switch (utility::common::bkdrHash(key.data()))
        {
            case "filePath"_bkdrHash:
                isVerified &= item.isStringType();
                break;
            case "minimumLevel"_bkdrHash:
                using OutputLevel = log::Log::OutputLevel;
                isVerified &= item.isIntegralType();
                isVerified &= EnumCheck<
                    OutputLevel,
                    OutputLevel::debug,
                    OutputLevel::info,
                    OutputLevel::warning,
                    OutputLevel::error>::isValue(item.toIntegral());
                break;
            case "usedMedium"_bkdrHash:
                using OutputMedium = log::Log::OutputMedium;
                isVerified &= item.isIntegralType();
                isVerified &=
                    EnumCheck<OutputMedium, OutputMedium::file, OutputMedium::terminal, OutputMedium::both>::isValue(
                        item.toIntegral());
                break;
            case "writeType"_bkdrHash:
                using OutputType = log::Log::OutputType;
                isVerified &= item.isIntegralType();
                isVerified &= EnumCheck<OutputType, OutputType::add, OutputType::over>::isValue(item.toIntegral());
                break;
            default:
                isVerified &= false;
                break;
        }
    }

    if (!isVerified)
    {
        throw std::runtime_error(
            R"(Illegal configuration, "logger" object in "helperList" object: )" + loggerObject.toUnescapedString()
            + '.');
    }
}

void Config::checkViewerConfigInHelperList(const utility::json::JSON& configData)
{
    using utility::common::operator""_bkdrHash;

    bool isVerified = true;
    const auto viewerObject = configData.at("helperList").at("viewer");
    isVerified &= viewerObject.isObjectType();
    const auto viewerProperties = viewerObject.at("properties"), viewerRequired = viewerObject.at("required");
    isVerified &= viewerProperties.isObjectType();
    isVerified &= viewerRequired.isArrayType();
    isVerified &= (viewerProperties.size() == viewerRequired.length());
    for (const auto& item : viewerRequired.arrayRange())
    {
        isVerified &= item.isStringType();
        isVerified &= viewerProperties.hasKey(item.toString());
    }

    constexpr std::uint16_t minPortNum = 0, maxPortNum = 65535;
    for (const auto& [key, item] : viewerProperties.objectRange())
    {
        switch (utility::common::bkdrHash(key.data()))
        {
            case "tcpHost"_bkdrHash:
                isVerified &= item.isStringType();
                break;
            case "tcpPort"_bkdrHash:
                isVerified &= item.isIntegralType();
                isVerified &= ((item.toIntegral() >= minPortNum) && (item.toIntegral() <= maxPortNum));
                break;
            case "udpHost"_bkdrHash:
                isVerified &= item.isStringType();
                break;
            case "udpPort"_bkdrHash:
                isVerified &= item.isIntegralType();
                isVerified &= ((item.toIntegral() >= minPortNum) && (item.toIntegral() <= maxPortNum));
                break;
            default:
                isVerified &= false;
                break;
        }
    }

    if (!isVerified)
    {
        throw std::runtime_error(
            R"(Illegal configuration, "viewer" object in "helperList" object: )" + viewerObject.toUnescapedString()
            + '.');
    }
}

//! @brief Get the full path to the default configuration file.
//! @return full path to the default configuration file
std::string getFullDefaultConfigurationPath()
{
    const char* const processHome = std::getenv("FOO_HOME");
    if (nullptr == processHome)
    {
        throw std::runtime_error("The environment variable FOO_HOME is not set.");
    }
    return std::string{processHome} + '/' + std::string{defaultConfigurationFile};
}

//! @brief Get the default configuration.
//! @return default configuration
utility::json::JSON getDefaultConfiguration()
{
    namespace json = utility::json;

    auto loggerProperties = json::object();
    loggerProperties.at("filePath") = "log/foo.log";
    loggerProperties.at("minimumLevel") = static_cast<int>(log::Log::OutputLevel::debug);
    loggerProperties.at("usedMedium") = static_cast<int>(log::Log::OutputMedium::both);
    loggerProperties.at("writeType") = static_cast<int>(log::Log::OutputType::add);
    auto loggerRequired = json::array();
    loggerRequired.append("filePath", "minimumLevel", "usedMedium", "writeType");
    assert(loggerProperties.size() == loggerRequired.length());

    auto viewerProperties = json::object();
    viewerProperties.at("tcpHost") = "localhost";
    viewerProperties.at("tcpPort") = 61501; // NOLINT (readability-magic-numbers)
    viewerProperties.at("udpHost") = "localhost";
    viewerProperties.at("udpPort") = 61502; // NOLINT (readability-magic-numbers)
    auto viewerRequired = json::array();
    viewerRequired.append("tcpHost", "tcpPort", "udpHost", "udpPort");
    assert(viewerProperties.size() == viewerRequired.length());

    // NOLINTBEGIN (readability-magic-numbers)
    // clang-format off
    return utility::json::JSON
    (
    {
        "activateHelper", true,
        "helperList", {
            "logger", {
                "properties", loggerProperties,
                "required", loggerRequired
            },
            "viewer", {
                "properties", viewerProperties,
                "required", viewerRequired
            }
        },
        "helperTimeout", 500
    }
    );
    // clang-format on
    // NOLINTEND (readability-magic-numbers)
}

//! @brief Forced configuration update by default.
//! @param filename - config file
static void forcedConfigurationUpdateByDefault(const std::string& filename)
{
    utility::io::FileWriter fileWriter(filename);
    fileWriter.open(true);
    fileWriter.lock();
    fileWriter.stream() << config::getDefaultConfiguration();
    fileWriter.unlock();
    fileWriter.close();
}

//! @brief Initialize the configuration.
//! @param filename - config file
static void initializeConfiguration(const std::string& filename)
{
    const std::filesystem::path configFolderPath = std::filesystem::absolute(filename).parent_path();
    std::filesystem::create_directories(configFolderPath);
    std::filesystem::permissions(
        configFolderPath, std::filesystem::perms::owner_all, std::filesystem::perm_options::add);

    forcedConfigurationUpdateByDefault(filename);
}

//! @brief Load the configuration.
//! @param filename - config file
//! @return successful or failed to load
bool loadConfiguration(const std::string& filename)
try
{
    if (!std::filesystem::exists(filename))
    {
        initializeConfiguration(filename);
    }
    static_cast<void>(Config::getInstance());

    return true;
}
catch (...)
{
    std::cerr << "Configuration load exception ..." << std::endl;
    constexpr std::string_view hint = "Type y to force an update to the default configuration, n to exit: ",
                               clearEscape = "\x1b[1A\x1b[2K\r";
    std::cout << hint << "\n\x1b[1A\x1b[" << hint.length() << 'C' << std::flush;

    bool keepThrowing = true;
    constexpr std::uint16_t timeoutPeriod = 5000;
    utility::io::waitForUserInput(
        [&](const std::string& input)
        {
            if (("y" != input) && ("n" != input))
            {
                std::cout << clearEscape << hint << std::flush;
                return false;
            }
            else if ("y" == input)
            {
                forcedConfigurationUpdateByDefault(filename);
            }
            keepThrowing = false;
            return true;
        },
        timeoutPeriod);

    if (keepThrowing)
    {
        std::cout << '\n' << std::endl;
        throw;
    }
    std::cout << std::endl;
    return false;
}

//! @brief Anonymous namespace.
inline namespace
{
//! @brief The semaphore that controls the maximum access limit.
static std::counting_semaphore<maxAccessLimit> configSem(maxAccessLimit);
} // namespace

//! @brief Retrieve data.
//! @return current configuration data
const utility::json::JSON& retrieveData()
try
{
    configSem.acquire();
    const auto& data = Config::getInstance().getData();
    configSem.release();
    return data;
}
catch (...)
{
    configSem.release();
    throw;
}
} // namespace application::config
