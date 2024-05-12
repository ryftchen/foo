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
#include <fstream>
#include <iterator>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

namespace application::config
{
Config::Config()
{
    parseFile(filePath);
}

Config& Config::getInstance()
{
    static Config configuration{};
    return configuration;
}

utility::json::JSON& Config::getData()
{
    return data;
}

std::string Config::getFilePath() const
{
    return filePath;
}

void Config::parseFile(const std::string& filename)
{
    if (!std::filesystem::exists(filename))
    {
        throw std::runtime_error("Configuration file " + filename + " is missing.");
    }

    const auto configs = utility::file::getFileContents(filename);
    std::ostringstream os;
    std::copy(configs.cbegin(), configs.cend(), std::ostream_iterator<std::string>(os, ""));
    data = utility::json::JSON::load(os.str());
    verifyData();
}

void Config::verifyData()
{
    bool isVerified = data.at("activateHelper").isBooleanType();
    isVerified &= data.at("helperList").isObjectType();
    isVerified &= (data.at("helperTimeout").isIntegralType() && (data.at("helperTimeout").toIntegral() >= 0));
    if (!isVerified)
    {
        throw std::runtime_error("Illegal configuration: " + data.toUnescapedString());
    }

    checkLoggerConfigInHelperList();
    checkViewerConfigInHelperList();
}

bool Config::checkLoggerConfigInHelperList()
{
    using utility::common::operator""_bkdrHash;
    using utility::common::bkdrHash;

    bool isVerified = true;
    const auto loggerObject = data.at("helperList").at("logger");
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
        switch (bkdrHash(key.data()))
        {
            case "filePath"_bkdrHash:
                isVerified &= item.isStringType();
                break;
            case "minimumLevel"_bkdrHash:
                using OutputLevel = log::Log::OutputLevel;
                isVerified &= item.isIntegralType();
                isVerified &= utility::common::EnumCheck<
                    OutputLevel,
                    OutputLevel::debug,
                    OutputLevel::info,
                    OutputLevel::warning,
                    OutputLevel::error>::isValue(item.toIntegral());
                break;
            case "usedMedium"_bkdrHash:
                using OutputMedium = log::Log::OutputMedium;
                isVerified &= item.isIntegralType();
                isVerified &= utility::common::
                    EnumCheck<OutputMedium, OutputMedium::file, OutputMedium::terminal, OutputMedium::both>::isValue(
                        item.toIntegral());
                break;
            case "writeType"_bkdrHash:
                using OutputType = log::Log::OutputType;
                isVerified &= item.isIntegralType();
                isVerified &= utility::common::EnumCheck<OutputType, OutputType::add, OutputType::over>::isValue(
                    item.toIntegral());
                break;
            default:
                isVerified &= false;
                break;
        }
    }

    if (!isVerified)
    {
        throw std::runtime_error(
            R"(Illegal configuration, "logger" object in "helperList" object: )" + loggerObject.toUnescapedString());
    }
    return isVerified;
}

bool Config::checkViewerConfigInHelperList()
{
    using utility::common::operator""_bkdrHash;
    using utility::common::bkdrHash;

    bool isVerified = true;
    const auto viewerObject = data.at("helperList").at("viewer");
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
        switch (bkdrHash(key.data()))
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
            R"(Illegal configuration, "viewer" object in "helperList" object: )" + viewerObject.toUnescapedString());
    }
    return isVerified;
}

//! @brief Get the full path to the default config file.
//! @return full path to the default config file
std::string getFullDefaultConfigPath()
{
    std::string processHome;
    if (nullptr != std::getenv("FOO_HOME"))
    {
        processHome = std::getenv("FOO_HOME");
    }
    else
    {
        throw std::runtime_error("The environment variable FOO_HOME is not set.");
    }
    return processHome + '/' + std::string{defaultConfigFile};
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
    return utility::json::JSON(
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
        "helperTimeout", 200
    });
    // clang-format on
    // NOLINTEND (readability-magic-numbers)
}

//! @brief Forced configuration update by default.
//! @param filename - config file
void forcedConfigurationUpdateByDefault(const std::string& filename)
{
    namespace file = utility::file;

    std::ofstream ofs = file::openFile(filename, true);
    file::fdLock(ofs, file::LockMode::write);
    ofs << config::getDefaultConfiguration();
    file::fdUnlock(ofs);
    file::closeFile(ofs);
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
    Config::getInstance();

    return true;
}
catch (const std::exception& error)
{
    std::cerr << "Loading configuration exception. Please confirm whether to force an update to the default "
                 "configuration before exiting. (y or n)"
              << std::endl;

    std::string input;
    while (std::cin >> input)
    {
        if ("y" == input)
        {
            forcedConfigurationUpdateByDefault(filename);
            return false;
        }
        else if ("n" == input)
        {
            throw std::runtime_error(error.what());
        }
    }

    return false;
}
} // namespace application::config
