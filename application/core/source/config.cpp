//! @file config.cpp
//! @author ryftchen
//! @brief The definitions (config) in the application module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#include "config.hpp"

#ifndef __PRECOMPILED_HEADER
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iterator>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

#include "utility/include/common.hpp"
#include "utility/include/file.hpp"

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
        throw std::runtime_error("Config file " + filename + " is missing.");
    }

    const auto configs = utility::file::getFileContents(filename);
    std::ostringstream os;
    std::copy(configs.cbegin(), configs.cend(), std::ostream_iterator<std::string>(os, ""));
    data = utility::json::JSON::load(os.str());
    verifyData();
}

void Config::verifyData()
{
    bool isVerified = data.at("activeHelper").isBooleanType();
    isVerified &= data.at("helperTable").isObjectType();
    if (!isVerified)
    {
        throw std::runtime_error("Illegal configuration: " + data.toUnescapedString());
    }

    using utility::common::operator""_bkdrHash;
    using utility::common::bkdrHash;

    const auto loggerObject = data.at("helperTable").at("logger");
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
    // NOLINTBEGIN(readability-magic-numbers)
    for (const auto& [key, item] : loggerProperties.objectRange())
    {
        switch (bkdrHash(key.data()))
        {
            case "filePath"_bkdrHash:
                isVerified &= item.isStringType();
                break;
            case "minimumLevel"_bkdrHash:
                isVerified &= item.isIntegralType();
                isVerified &= ((item.toIntegral() >= 0) && (item.toIntegral() <= 3));
                break;
            case "usedMedium"_bkdrHash:
                isVerified &= item.isIntegralType();
                isVerified &= ((item.toIntegral() >= 0) && (item.toIntegral() <= 2));
                break;
            case "writeType"_bkdrHash:
                isVerified &= item.isIntegralType();
                isVerified &= ((item.toIntegral() >= 0) && (item.toIntegral() <= 1));
                break;
            default:
                isVerified &= false;
                break;
        }
    }
    // NOLINTEND(readability-magic-numbers)
    if (!isVerified)
    {
        throw std::runtime_error(
            R"(Illegal configuration, "logger" object in "helperTable" object: )" + loggerObject.toUnescapedString());
    }

    const auto viewerObject = data.at("helperTable").at("viewer");
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
    // NOLINTBEGIN(readability-magic-numbers)
    for (const auto& [key, item] : viewerProperties.objectRange())
    {
        switch (bkdrHash(key.data()))
        {
            case "tcpHost"_bkdrHash:
                isVerified &= item.isStringType();
                break;
            case "tcpPort"_bkdrHash:
                isVerified &= item.isIntegralType();
                isVerified &= ((item.toIntegral() >= 0) && (item.toIntegral() <= 65535));
                break;
            case "udpHost"_bkdrHash:
                isVerified &= item.isStringType();
                break;
            case "udpPort"_bkdrHash:
                isVerified &= item.isIntegralType();
                isVerified &= ((item.toIntegral() >= 0) && (item.toIntegral() <= 65535));
                break;
            default:
                isVerified &= false;
                break;
        }
    }
    // NOLINTEND(readability-magic-numbers)
    if (!isVerified)
    {
        throw std::runtime_error(
            R"(Illegal configuration, "viewer" object in "helperTable" object: )" + viewerObject.toUnescapedString());
    }
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

    // NOLINTBEGIN(readability-magic-numbers)
    auto loggerProperties = json::object();
    loggerProperties.at("filePath") = "log/foo.log";
    loggerProperties.at("minimumLevel") = 0;
    loggerProperties.at("usedMedium") = 2;
    loggerProperties.at("writeType") = 0;
    auto loggerRequired = json::array();
    loggerRequired.append("filePath", "minimumLevel", "usedMedium", "writeType");
    assert(loggerProperties.size() == loggerRequired.length());

    auto viewerProperties = json::object();
    viewerProperties.at("tcpHost") = "localhost";
    viewerProperties.at("tcpPort") = 61501;
    viewerProperties.at("udpHost") = "localhost";
    viewerProperties.at("udpPort") = 61502;
    auto viewerRequired = json::array();
    viewerRequired.append("tcpHost", "tcpPort", "udpHost", "udpPort");
    assert(viewerProperties.size() == viewerRequired.length());
    // NOLINTEND(readability-magic-numbers)

    // clang-format off
    return utility::json::JSON(
    {
        "activeHelper", true,
        "helperTable", {
            "logger", {
                "properties", loggerProperties,
                "required", loggerRequired
            },
            "viewer", {
                "properties", viewerProperties,
                "required", viewerRequired
            }
        }
    });
    // clang-format on
}

//! @brief Initialize the configuration.
//! @param filename - config file
void initializeConfiguration(const std::string& filename)
{
    if (!std::filesystem::exists(filename))
    {
        const std::filesystem::path configFolderPath = std::filesystem::absolute(filename).parent_path();
        std::filesystem::create_directories(configFolderPath);
        std::filesystem::permissions(
            configFolderPath, std::filesystem::perms::owner_all, std::filesystem::perm_options::add);

        namespace file = utility::file;
        std::ofstream ofs = file::openFile(filename, false);
        file::fdLock(ofs, file::LockMode::write);
        ofs << getDefaultConfiguration();
        file::fdUnlock(ofs);
        file::closeFile(ofs);
    }

    Config::getInstance();
}
} // namespace application::config
