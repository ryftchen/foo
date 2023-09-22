//! @file config.cpp
//! @author ryftchen
//! @brief The definitions (config) in the application module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen. All rights reserved.

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
    isVerified &= data.at("helperSetting").isObjectType();
    if (!isVerified)
    {
        throw std::runtime_error("Illegal configuration: " + data.toUnescapedString());
    }

    using utility::common::operator""_bkdrHash;
    using utility::common::bkdrHash;

    const auto loggerObject = data.at("helperSetting").at("logger");
    isVerified &= loggerObject.isObjectType();
    const auto loggerProperties = loggerObject.at("properties"), loggerRequired = loggerObject.at("required");
    isVerified &= loggerProperties.isObjectType();
    isVerified &= loggerRequired.isArrayType();
    isVerified &= (loggerProperties.size() == loggerRequired.length());
    for (const auto& elem : loggerRequired.arrayRange())
    {
        isVerified &= elem.isStringType();
        isVerified &= loggerProperties.hasKey(elem.toString());
    }
    for (const auto& [name, elem] : loggerProperties.objectRange())
    {
        switch (bkdrHash(name.data()))
        {
            case "filePath"_bkdrHash:
                isVerified &= elem.isStringType();
                break;
            case "minimumLevel"_bkdrHash:
                isVerified &= elem.isIntegralType();
                break;
            case "usedMedium"_bkdrHash:
                isVerified &= elem.isIntegralType();
                break;
            case "writeType"_bkdrHash:
                isVerified &= elem.isIntegralType();
                break;
            default:
                isVerified &= false;
                break;
        }
    }
    if (!isVerified)
    {
        throw std::runtime_error(
            R"(Illegal configuration, "logger" object in "helperSetting" object: )" + loggerObject.toUnescapedString());
    }

    const auto viewerObject = data.at("helperSetting").at("viewer");
    isVerified &= viewerObject.isObjectType();
    const auto viewerProperties = viewerObject.at("properties"), viewerRequired = viewerObject.at("required");
    isVerified &= viewerProperties.isObjectType();
    isVerified &= viewerRequired.isArrayType();
    isVerified &= (viewerProperties.size() == viewerRequired.length());
    for (const auto& elem : viewerRequired.arrayRange())
    {
        isVerified &= elem.isStringType();
        isVerified &= viewerProperties.hasKey(elem.toString());
    }
    for (const auto& [name, elem] : viewerProperties.objectRange())
    {
        switch (bkdrHash(name.data()))
        {
            case "tcpHost"_bkdrHash:
                isVerified &= elem.isStringType();
                break;
            case "tcpPort"_bkdrHash:
                isVerified &= elem.isIntegralType();
                break;
            case "udpHost"_bkdrHash:
                isVerified &= elem.isStringType();
                break;
            case "udpPort"_bkdrHash:
                isVerified &= elem.isIntegralType();
                break;
            default:
                isVerified &= false;
                break;
        }
    }
    if (!isVerified)
    {
        throw std::runtime_error(
            R"(Illegal configuration, "viewer" object in "helperSetting" object: )" + viewerObject.toUnescapedString());
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
    viewerProperties.at("tcpPort") = 61501; // NOLINT(readability-magic-numbers)
    viewerProperties.at("udpHost") = "localhost";
    viewerProperties.at("udpPort") = 61502; // NOLINT(readability-magic-numbers)
    auto viewerRequired = json::array();
    viewerRequired.append("tcpHost", "tcpPort", "udpHost", "udpPort");
    assert(viewerProperties.size() == viewerRequired.length());

    return utility::json::JSON(
        {"activeHelper",
         true,
         "helperSetting",
         {"logger",
          {"properties", loggerProperties, "required", loggerRequired},
          "viewer",
          {"properties", viewerProperties, "required", viewerRequired}}});
}

//! @brief Initialize the configuration.
//! @param filename - config file
void initializeConfiguration(const std::string& filename)
{
    if (!std::filesystem::exists(filename))
    {
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
