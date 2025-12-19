//! @file configure.cpp
//! @author ryftchen
//! @brief The definitions (configure) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "configure.hpp"
#include "log.hpp"
#include "view.hpp"

#ifndef _PRECOMPILED_HEADER
#include <filesystem>
#include <iterator>
#else
#include "application/pch/precompiled_header.hpp"
#endif // _PRECOMPILED_HEADER

namespace application::configure
{
//! @brief Anonymous namespace.
inline namespace
{
//! @brief The semaphore that controls the maximum access limit.
std::counting_semaphore<maxAccessLimit> configSem(maxAccessLimit);
} // namespace

std::string getFullConfigPath(const std::string_view filename)
{
    const char* const processHome = std::getenv("FOO_HOME"); // NOLINT(concurrency-mt-unsafe)
    if (!processHome)
    {
        throw std::runtime_error{"The environment variable FOO_HOME is not set."};
    }
    return std::string{processHome} + '/' + filename.data();
}

//! @brief Get the Configure instance.
//! @param filename - configure file path
//! @return const reference of the Configure object
const Configure& getInstance(const std::string_view filename = defaultConfigFile)
{
    static const Configure configurator(filename);
    return configurator;
}

utility::json::JSON Configure::parseConfigFile(const std::string_view configFile)
{
    if (!std::filesystem::is_regular_file(configFile))
    {
        throw std::runtime_error{"Configuration file " + std::string{configFile} + " is missing."};
    }

    using utility::json::JSON;
    const auto configRows = utility::io::readFileLines(configFile, true);
    std::ostringstream transfer{};
    std::ranges::copy(configRows, std::ostream_iterator<std::string>(transfer, ""));
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
            + std::string{field::helperList} + "\" field (" + helper.asUnescapedString() + ")."};
    }

    bool isVerified = helper.isObjectType();
    const auto& loggerProperties = helper.at(field::properties);
    const auto& loggerRequired = helper.at(field::required);
    isVerified &= loggerProperties.isObjectType() && loggerRequired.isArrayType()
        && (loggerProperties.size() == loggerRequired.length());
    for (const auto& item : loggerRequired.arrayRange())
    {
        isVerified &= item.isStringType() && loggerProperties.hasKey(item.asString());
    }
    for (const auto& [key, item] : loggerProperties.objectRange())
    {
        switch (utility::common::bkdrHash(key.c_str()))
        {
            using utility::common::operator""_bkdrHash, utility::common::EnumCheck;
            case operator""_bkdrHash(field::filePath):
                isVerified &= item.isStringType();
                break;
            case operator""_bkdrHash(field::priorityLevel):
                using OutputLevel = log::Log::OutputLevel;
                isVerified &= item.isIntegralType()
                    && EnumCheck<OutputLevel,
                                 OutputLevel::debug,
                                 OutputLevel::info,
                                 OutputLevel::warning,
                                 OutputLevel::error>::has(item.asIntegral());
                break;
            case operator""_bkdrHash(field::targetType):
                using OutputType = log::Log::OutputType;
                isVerified &= item.isIntegralType()
                    && EnumCheck<OutputType, OutputType::file, OutputType::terminal, OutputType::all>::has(
                                  item.asIntegral());
                break;
            case operator""_bkdrHash(field::writeMode):
                using OutputMode = log::Log::OutputMode;
                isVerified &= item.isIntegralType()
                    && EnumCheck<OutputMode, OutputMode::append, OutputMode::overwrite>::has(item.asIntegral());
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
            + std::string{field::helperList} + "\" field (" + helper.asUnescapedString() + ")."};
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
            + std::string{field::helperList} + "\" field (" + helper.asUnescapedString() + ")."};
    }

    bool isVerified = helper.isObjectType();
    const auto& viewerProperties = helper.at(field::properties);
    const auto& viewerRequired = helper.at(field::required);
    isVerified &= viewerProperties.isObjectType() && viewerRequired.isArrayType()
        && (viewerProperties.size() == viewerRequired.length());
    for (const auto& item : viewerRequired.arrayRange())
    {
        isVerified &= item.isStringType() && viewerProperties.hasKey(item.asString());
    }
    for (const auto& [key, item] : viewerProperties.objectRange())
    {
        switch (utility::common::bkdrHash(key.c_str()))
        {
            using utility::common::operator""_bkdrHash;
            case operator""_bkdrHash(field::tcpHost):
                isVerified &= item.isStringType();
                break;
            case operator""_bkdrHash(field::tcpPort):
                isVerified &= item.isIntegralType()
                    && ((item.asIntegral() >= view::minPortNumber) && (item.asIntegral() <= view::maxPortNumber));
                break;
            case operator""_bkdrHash(field::udpHost):
                isVerified &= item.isStringType();
                break;
            case operator""_bkdrHash(field::udpPort):
                isVerified &= item.isIntegralType()
                    && ((item.asIntegral() >= view::minPortNumber) && (item.asIntegral() <= view::maxPortNumber));
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
            + std::string{field::helperList} + "\" field (" + helper.asUnescapedString() + ")."};
    }
}

void Configure::verifyConfigData(const utility::json::JSON& configData)
{
    if (!configData.hasKey(field::activateHelper) || !configData.hasKey(field::helperList)
        || !configData.hasKey(field::helperTimeout))
    {
        throw std::runtime_error{"Incomplete 1st level configuration (" + configData.asUnescapedString() + ")."};
    }

    if (!configData.at(field::activateHelper).isBooleanType() || !configData.at(field::helperList).isObjectType()
        || !configData.at(field::helperTimeout).isIntegralType()
        || configData.at(field::helperTimeout).asIntegral() < 0)
    {
        throw std::runtime_error{"Illegal 1st level configuration (" + configData.asUnescapedString() + ")."};
    }

    const auto& helperListObject = configData.at(field::helperList);
    if (!helperListObject.hasKey(field::logger) || !helperListObject.hasKey(field::viewer))
    {
        throw std::runtime_error{
            "Incomplete 2nd level configuration in \"" + std::string{field::helperList} + "\" field ("
            + helperListObject.asUnescapedString() + ")."};
    }
    for (const auto& [key, item] : helperListObject.objectRange())
    {
        switch (utility::common::bkdrHash(key.c_str()))
        {
            using utility::common::operator""_bkdrHash;
            case operator""_bkdrHash(field::logger):
                checkObjectInHelperList<log::Log>(item);
                break;
            case operator""_bkdrHash(field::viewer):
                checkObjectInHelperList<view::View>(item);
                break;
            default:
                throw std::runtime_error{
                    "Illegal 2nd level configuration in \"" + std::string{field::helperList} + "\" field ("
                    + helperListObject.asUnescapedString() + ")."};
        }
    }
}

Retrieve::Retrieve(std::counting_semaphore<maxAccessLimit>& sem) : sem{sem}
{
    sem.acquire();
}

Retrieve::~Retrieve()
{
    sem.release();
}

const utility::json::JSON& Retrieve::operator/(const std::string& field) const
{
    return getInstance().dataRepo.at(field);
}

Retrieve::operator const utility::json::JSON&() const
{
    return getInstance().dataRepo;
}

//! @brief Safely retrieve the configuration data repository.
//! @return current configuration data repository
Retrieve retrieveDataRepo()
{
    return Retrieve(configSem);
}

// NOLINTBEGIN(readability-magic-numbers)
//! @brief Dump the default configuration.
//! @return default configuration
utility::json::JSON dumpDefaultConfig()
{
    using log::Log;
    auto loggerProperties = utility::json::object();
    loggerProperties.at(field::filePath) = "log/foo.log";
    loggerProperties.at(field::priorityLevel) =
        static_cast<std::underlying_type_t<Log::OutputLevel>>(Log::OutputLevel::debug);
    loggerProperties.at(field::targetType) = static_cast<std::underlying_type_t<Log::OutputType>>(Log::OutputType::all);
    loggerProperties.at(field::writeMode) =
        static_cast<std::underlying_type_t<Log::OutputMode>>(Log::OutputMode::append);
    auto loggerRequired = utility::json::array();
    loggerRequired.append(field::filePath, field::priorityLevel, field::targetType, field::writeMode);
    MACRO_ASSERT(loggerProperties.size() == loggerRequired.length());

    auto viewerProperties = utility::json::object();
    viewerProperties.at(field::tcpHost) = "localhost";
    viewerProperties.at(field::tcpPort) = 61501;
    viewerProperties.at(field::udpHost) = "localhost";
    viewerProperties.at(field::udpPort) = 61502;
    auto viewerRequired = utility::json::array();
    viewerRequired.append(field::tcpHost, field::tcpPort, field::udpHost, field::udpPort);
    MACRO_ASSERT(viewerProperties.size() == viewerRequired.length());
    // clang-format off
    return utility::json::JSON
    (
    {
      field::activateHelper, true,
      field::helperList, {
        field::logger, {
          field::properties, std::move(loggerProperties),
          field::required, std::move(loggerRequired)
        },
        field::viewer, {
          field::properties, std::move(viewerProperties),
          field::required, std::move(viewerRequired)
        }
      },
      field::helperTimeout, 1000
    }
    );
    // clang-format on
}
// NOLINTEND(readability-magic-numbers)

//! @brief Forced configuration update by default.
//! @param filePath - full path to the configuration file
static void forcedConfigUpdateByDefault(const std::string_view filePath)
{
    utility::io::FileWriter configWriter(filePath);
    configWriter.open(true);
    configWriter.lock();
    configWriter.stream() << configure::dumpDefaultConfig() << std::endl;
    configWriter.unlock();
    configWriter.close();
}

//! @brief Initialize the configuration.
//! @param filePath - full path to the configuration file
static void initializeConfig(const std::string_view filePath)
{
    const auto configFolderPath = std::filesystem::absolute(filePath).parent_path();
    std::filesystem::create_directories(configFolderPath);
    std::filesystem::permissions(
        configFolderPath, std::filesystem::perms::owner_all, std::filesystem::perm_options::add);

    forcedConfigUpdateByDefault(filePath);
}

//! @brief Show prompt and wait for input on configuration exception.
//! @param filePath - full path to the configuration file
//! @return whether to continue throwing exception
static bool handleConfigException(const std::string_view filePath)
{
    constexpr std::string_view prompt = "Type y to force an update to the default configuration, n to exit: ";
    constexpr std::string_view escapeClear = "\x1b[1A\x1b[2K\r";
    constexpr std::string_view escapeMoveUp = "\n\x1b[1A\x1b[";
    std::cout << prompt << escapeMoveUp << prompt.length() << 'C' << std::flush;

    bool keepThrowing = true;
    constexpr std::uint16_t timeout = 5000;
    utility::io::waitForUserInput(
        utility::common::wrapClosure(
            [&](const std::string& input)
            {
                switch (utility::common::bkdrHash(input.c_str()))
                {
                    using utility::common::operator""_bkdrHash;
                    case "y"_bkdrHash:
                        forcedConfigUpdateByDefault(filePath);
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
        timeout);
    return keepThrowing;
}

//! @brief Load the settings.
//! @param filename - configure file path
//! @return successful or failed to load
bool loadSettings(const std::string_view filename)
{
    const auto filePath = getFullConfigPath(filename);
    try
    {
        if (!std::filesystem::is_regular_file(filePath))
        {
            initializeConfig(filePath);
        }
        getInstance(filename);
        return true;
    }
    catch (...)
    {
        std::cerr << "Configuration load exception ..." << std::endl;
        if (handleConfigException(filePath))
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
