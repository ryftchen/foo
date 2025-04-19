//! @file configure.cpp
//! @author ryftchen
//! @brief The definitions (configure) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "configure.hpp"
#include "log.hpp"
#include "view.hpp"

#ifndef __PRECOMPILED_HEADER
#include <cassert>
#include <filesystem>
#include <iterator>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

namespace application::configure
{
//! @brief Anonymous namespace.
inline namespace
{
//! @brief Maximum access limit.
constexpr std::uint8_t maxAccessLimit = 10;
//! @brief The semaphore that controls the maximum access limit.
std::counting_semaphore<maxAccessLimit> configSem(maxAccessLimit);
} // namespace

std::string getFullConfigPath(const std::string_view filename)
{
    const char* const processHome = std::getenv("FOO_HOME");
    if (!processHome)
    {
        throw std::runtime_error{"The environment variable FOO_HOME is not set."};
    }

    return std::string{processHome} + '/' + filename.data();
}

const utility::json::JSON& retrieveDataRepo()
try
{
    configSem.acquire();
    const auto& dataRepo = Configure::getInstance().retrieve();
    configSem.release();

    return dataRepo;
}
catch (...)
{
    configSem.release();
    throw;
}

Configure& Configure::getInstance(const std::string_view filename)
{
    static Configure configurator(filename);
    return configurator;
}

const utility::json::JSON& Configure::retrieve() const
{
    return dataRepo;
}

utility::json::JSON Configure::parseConfigFile(const std::string_view configFile)
{
    if (!std::filesystem::is_regular_file(configFile))
    {
        throw std::runtime_error{"Configuration file " + std::string{configFile} + " is missing."};
    }

    using utility::json::JSON;
    const auto configRows = utility::io::getFileContents(configFile, true);
    std::ostringstream transfer{};
    std::copy(configRows.cbegin(), configRows.cend(), std::ostream_iterator<std::string>(transfer, ""));
    auto preprocessedData = JSON::load(transfer.str());
    verifyConfigData(preprocessedData);

    return preprocessedData;
}

//! @brief Check the "logger" object in the helper list.
//! @param helper - object of helper
template <>
void Configure::checkObjectInHelperList<log::Log>(const utility::json::JSON& helper)
{
    if (!helper.hasKey(field::properties) || !helper.hasKey(field::required))
    {
        throw std::runtime_error{
            "Incomplete 3rd level configuration in \"" + std::string{field::logger} + "\" field in \""
            + std::string{field::helperList} + "\" field (" + helper.toUnescapedString() + ")."};
    }

    bool isVerified = helper.isObjectType();
    const auto &loggerProperties = helper.at(field::properties), &loggerRequired = helper.at(field::required);
    isVerified &= loggerProperties.isObjectType() && loggerRequired.isArrayType()
        && (loggerProperties.size() == loggerRequired.length());
    for (const auto& item : loggerRequired.arrayRange())
    {
        isVerified &= item.isStringType() && loggerProperties.hasKey(item.toString());
    }
    for (const auto& [key, item] : loggerProperties.objectRange())
    {
        switch (utility::common::bkdrHash(key.c_str()))
        {
            using utility::common::operator""_bkdrHash, utility::common::EnumCheck;
            case operator""_bkdrHash(field::filePath.data()):
                isVerified &= item.isStringType();
                break;
            case operator""_bkdrHash(field::priorityLevel.data()):
                using OutputLevel = log::Log::OutputLevel;
                isVerified &= item.isIntegralType()
                    && EnumCheck<OutputLevel,
                                 OutputLevel::debug,
                                 OutputLevel::info,
                                 OutputLevel::warning,
                                 OutputLevel::error>::has(item.toIntegral());
                break;
            case operator""_bkdrHash(field::targetType.data()):
                using OutputType = log::Log::OutputType;
                isVerified &= item.isIntegralType()
                    && EnumCheck<OutputType, OutputType::file, OutputType::terminal, OutputType::all>::has(
                                  item.toIntegral());
                break;
            case operator""_bkdrHash(field::writeMode.data()):
                using OutputMode = log::Log::OutputMode;
                isVerified &= item.isIntegralType()
                    && EnumCheck<OutputMode, OutputMode::append, OutputMode::overwrite>::has(item.toIntegral());
                break;
            default:
                isVerified &= false;
                break;
        }
    }

    if (!isVerified)
    {
        throw std::runtime_error{
            "Illegal 3rd level configuration in \"" + std::string{field::logger} + "\" field in \""
            + std::string{field::helperList} + "\" field (" + helper.toUnescapedString() + ")."};
    }
}

//! @brief Check the "viewer" object in the helper list.
//! @param helper - object of helper
template <>
void Configure::checkObjectInHelperList<view::View>(const utility::json::JSON& helper)
{
    if (!helper.hasKey(field::properties) || !helper.hasKey(field::required))
    {
        throw std::runtime_error{
            "Incomplete 3rd level configuration in \"" + std::string{field::viewer} + "\" field in \""
            + std::string{field::helperList} + "\" field (" + helper.toUnescapedString() + ")."};
    }

    bool isVerified = helper.isObjectType();
    const auto &viewerProperties = helper.at(field::properties), &viewerRequired = helper.at(field::required);
    isVerified &= viewerProperties.isObjectType() && viewerRequired.isArrayType()
        && (viewerProperties.size() == viewerRequired.length());
    for (const auto& item : viewerRequired.arrayRange())
    {
        isVerified &= item.isStringType() && viewerProperties.hasKey(item.toString());
    }
    for (const auto& [key, item] : viewerProperties.objectRange())
    {
        switch (utility::common::bkdrHash(key.c_str()))
        {
            using utility::common::operator""_bkdrHash;
            case operator""_bkdrHash(field::tcpHost.data()):
                isVerified &= item.isStringType();
                break;
            case operator""_bkdrHash(field::tcpPort.data()):
                isVerified &= item.isIntegralType()
                    && ((item.toIntegral() >= view::minPortNumber) && (item.toIntegral() <= view::maxPortNumber));
                break;
            case operator""_bkdrHash(field::udpHost.data()):
                isVerified &= item.isStringType();
                break;
            case operator""_bkdrHash(field::udpPort.data()):
                isVerified &= item.isIntegralType()
                    && ((item.toIntegral() >= view::minPortNumber) && (item.toIntegral() <= view::maxPortNumber));
                break;
            default:
                isVerified &= false;
                break;
        }
    }

    if (!isVerified)
    {
        throw std::runtime_error{
            "Illegal 3rd level configuration in \"" + std::string{field::viewer} + "\" field in \""
            + std::string{field::helperList} + "\" field (" + helper.toUnescapedString() + ")."};
    }
}

void Configure::verifyConfigData(const utility::json::JSON& configData)
{
    if (!configData.hasKey(field::activateHelper) || !configData.hasKey(field::helperList)
        || !configData.hasKey(field::helperTimeout))
    {
        throw std::runtime_error{"Incomplete 1st level configuration (" + configData.toUnescapedString() + ")."};
    }

    if (!configData.at(field::activateHelper).isBooleanType() || !configData.at(field::helperList).isObjectType()
        || !configData.at(field::helperTimeout).isIntegralType()
        || configData.at(field::helperTimeout).toIntegral() < 0)
    {
        throw std::runtime_error{"Illegal 1st level configuration (" + configData.toUnescapedString() + ")."};
    }

    const auto& helperListObject = configData.at(field::helperList);
    if (!helperListObject.hasKey(field::logger) || !helperListObject.hasKey(field::viewer))
    {
        throw std::runtime_error{
            "Incomplete 2nd level configuration in \"" + std::string{field::helperList} + "\" field ("
            + helperListObject.toUnescapedString() + ")."};
    }
    for (const auto& [key, item] : helperListObject.objectRange())
    {
        switch (utility::common::bkdrHash(key.c_str()))
        {
            using utility::common::operator""_bkdrHash;
            case operator""_bkdrHash(field::logger.data()):
                checkObjectInHelperList<log::Log>(item);
                break;
            case operator""_bkdrHash(field::viewer.data()):
                checkObjectInHelperList<view::View>(item);
                break;
            default:
                throw std::runtime_error{
                    "Illegal 2nd level configuration in \"" + std::string{field::helperList} + "\" field ("
                    + helperListObject.toUnescapedString() + ")."};
        }
    }
}

// NOLINTBEGIN(readability-magic-numbers)
//! @brief Get the default configuration.
//! @return default configuration
utility::json::JSON getDefaultConfiguration()
{
    using log::Log;
    auto loggerProperties = utility::json::object();
    loggerProperties.at(field::filePath) = "log/foo.log";
    loggerProperties.at(field::priorityLevel) = static_cast<int>(Log::OutputLevel::debug);
    loggerProperties.at(field::targetType) = static_cast<int>(Log::OutputType::all);
    loggerProperties.at(field::writeMode) = static_cast<int>(Log::OutputMode::append);
    auto loggerRequired = utility::json::array();
    loggerRequired.append(
        field::filePath.data(), field::priorityLevel.data(), field::targetType.data(), field::writeMode.data());
    assert(loggerProperties.size() == loggerRequired.length());

    auto viewerProperties = utility::json::object();
    viewerProperties.at(field::tcpHost) = "localhost";
    viewerProperties.at(field::tcpPort) = 61501;
    viewerProperties.at(field::udpHost) = "localhost";
    viewerProperties.at(field::udpPort) = 61502;
    auto viewerRequired = utility::json::array();
    viewerRequired.append(field::tcpHost.data(), field::tcpPort.data(), field::udpHost.data(), field::udpPort.data());
    assert(viewerProperties.size() == viewerRequired.length());

    // clang-format off
    return utility::json::JSON
    (
    {
        field::activateHelper.data(), true,
        field::helperList.data(), {
            field::logger.data(), {
                field::properties.data(), std::move(loggerProperties),
                field::required.data(), std::move(loggerRequired)
            },
            field::viewer.data(), {
                field::properties.data(), std::move(viewerProperties),
                field::required.data(), std::move(viewerRequired)
            }
        },
        field::helperTimeout.data(), 1000
    }
    );
    // clang-format on
}
// NOLINTEND(readability-magic-numbers)

//! @brief Forced configuration update by default.
//! @param filePath - full path to the configuration file
static void forcedConfigurationUpdateByDefault(const std::string_view filePath)
{
    utility::io::FileWriter fileWriter(filePath);
    fileWriter.open(true);
    fileWriter.lock();
    fileWriter.stream() << configure::getDefaultConfiguration();
    fileWriter.unlock();
    fileWriter.close();
}

//! @brief Initialize the configuration.
//! @param filePath - full path to the configuration file
static void initializeConfiguration(const std::string_view filePath)
{
    const auto configFolderPath = std::filesystem::absolute(filePath).parent_path();
    std::filesystem::create_directories(configFolderPath);
    std::filesystem::permissions(
        configFolderPath, std::filesystem::perms::owner_all, std::filesystem::perm_options::add);

    forcedConfigurationUpdateByDefault(filePath);
}

//! @brief Show prompt and wait for input on configuration exception.
//! @param filePath - full path to the configuration file
//! @return whether to continue throwing exception
static bool handleConfigurationException(const std::string_view filePath)
{
    constexpr std::string_view prompt = "Type y to force an update to the default configuration, n to exit: ",
                               escapeClear = "\x1b[1A\x1b[2K\r";
    std::cout << prompt << "\n\x1b[1A\x1b[" << prompt.length() << 'C' << std::flush;

    bool keepThrowing = true;
    constexpr std::uint16_t timeoutPeriod = 5000;
    utility::io::waitForUserInput(
        utility::common::wrapClosure(
            [&](const std::string_view input)
            {
                switch (utility::common::bkdrHash(input.data()))
                {
                    using utility::common::operator""_bkdrHash;
                    case "y"_bkdrHash:
                        forcedConfigurationUpdateByDefault(filePath);
                        [[fallthrough]];
                    case "n"_bkdrHash:
                        keepThrowing = false;
                        break;
                    default:
                        std::cout << escapeClear << prompt << std::flush;
                        return false;
                }
                return true;
            }),
        timeoutPeriod);

    return keepThrowing;
}

//! @brief Load the configuration.
//! @param filename - configure file path
//! @return successful or failed to load
bool loadConfiguration(const std::string_view filename)
{
    const auto filePath = getFullConfigPath(filename);
    try
    {
        if (!std::filesystem::is_regular_file(filePath))
        {
            initializeConfiguration(filePath);
        }
        static_cast<void>(Configure::getInstance(filename));

        return true;
    }
    catch (...)
    {
        std::cerr << "Configuration load exception ..." << std::endl;
        if (handleConfigurationException(filePath))
        {
            std::cout << '\n' << std::endl;
            throw;
        }
        std::cout << std::endl;
    }

    return false;
}

namespace task
{
//! @brief Get memory pool for task when making multi-threading.
//! @return reference of the ResourcePool object
ResourcePool& resourcePool()
{
    static ResourcePool pooling{};
    return pooling;
}
} // namespace task
} // namespace application::configure
