//! @file config.cpp
//! @author ryftchen
//! @brief The definitions (config) in the application module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen. All rights reserved.

#include "config.hpp"
#ifndef __PRECOMPILED_HEADER
#include <filesystem>
#include <fstream>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER
#include "utility/include/file.hpp"

namespace application::config
{
Config::Config(const std::string& filename, const char delim, const char commentDelim) :
    delimiter(delim), commentDelimiter(commentDelim)
{
    parseFile(filename);
}

Config& Config::getInstance()
{
    static Config config{};
    return config;
}

std::string Config::operator[](const std::string& key)
{
    if (!elementExists(key))
    {
        throw std::runtime_error("Key '" + key + "' not found in config file.");
    }

    std::string value;
    const auto pos = data.find(key);
    if (data.end() != pos)
    {
        value = pos->second;
    }

    return value;
}

std::string Config::getString(const std::string& key)
{
    return (*this)[key];
}

int Config::getInteger(const std::string& key)
{
    return getNumericValue<int>(key);
}

unsigned int Config::getUnsignedInteger(const std::string& key)
{
    return getNumericValue<unsigned int>(key);
}

float Config::getFloat(const std::string& key)
{
    return getNumericValue<float>(key);
}

double Config::getDouble(const std::string& key)
{
    return getNumericValue<double>(key);
}

bool Config::getBool(const std::string& key)
{
    return getInteger(key);
}

template <class T>
T Config::getNumericValue(const std::string& key)
{
    T value = 0;

    try
    {
        if (typeid(int) == typeid(T))
        {
            value = std::stoi((*this)[key]);
        }
        else if (typeid(unsigned int) == typeid(T))
        {
            value = static_cast<unsigned int>(std::stoul((*this)[key]));
        }
        else if (typeid(float) == typeid(T))
        {
            value = static_cast<float>(std::stof((*this)[key]));
        }
        else if (typeid(double) == typeid(T))
        {
            value = static_cast<double>(std::stod((*this)[key]));
        }
    }
    catch (const std::invalid_argument& error)
    {
        throw std::runtime_error("Numeric config value is malformed, value for key '" + key + "' cannot be converted.");
    }
    catch (const std::out_of_range& error)
    {
        throw std::runtime_error("Numeric config value is malformed, value for key '" + key + "' is out of range.");
    }

    return value;
}

bool Config::elementExists(const std::string& key) const
{
    return (data.find(key) != data.cend());
}

void Config::parseFile(const std::string& filename)
{
    if (!std::filesystem::exists(filename))
    {
        throw std::runtime_error("Config file " + filename + " is missing.");
    }

    namespace file = utility::file;
    std::ifstream ifs = utility::file::openFile(filename);
    file::fdLock(ifs, file::LockMode::read);
    std::string line;
    while (std::getline(ifs, line))
    {
        parseLine(line);
    }
    file::fdUnlock(ifs);
    utility::file::closeFile(ifs);
}

void Config::parseLine(const std::string& line)
{
    if (isComment(line))
    {
        return;
    }

    if (isFormatValid(line))
    {
        std::string key, value;
        std::istringstream is(line);
        std::getline(is, key, delimiter);
        key = trimLine(key);

        std::getline(is, value);
        const std::size_t lineEnd = value.find_first_of("\r\n");
        if (std::string::npos != lineEnd)
        {
            value.substr(0, lineEnd);
        }

        value = trimLine(value);
        if (!elementExists(key))
        {
            data[key] = value;
        }
        else
        {
            throw std::runtime_error("Config file is malformed, '" + key + "' has multiple occurrences.");
        }
    }
    else
    {
        throw std::runtime_error("Config file is malformed, line " + line);
    }
}

bool Config::isFormatValid(const std::string& line) const
{
    const auto tempLine = trimLine(line);
    return (tempLine.find(delimiter) != 0);
}

bool Config::isComment(const std::string& line) const
{
    const auto tempLine = trimLine(line);
    if (tempLine.size())
    {
        return (commentDelimiter == tempLine.at(0));
    }
    return true;
}

std::string Config::trimLine(const std::string& line)
{
    const std::size_t firstChar = line.find_first_not_of(' ');
    if (std::string::npos == firstChar)
    {
        return "";
    }

    const std::size_t lastChar = line.find_last_not_of(' ');
    return line.substr(firstChar, (lastChar - firstChar + 1));
}

//! @brief Initialize the configuration.
//! @param filename - configuration file
void initializeConfiguration(const std::string& filename)
{
    if (!std::filesystem::exists(filename))
    {
        namespace file = utility::file;
        std::ofstream ofs = file::openFile(filename, false);
        file::fdLock(ofs, file::LockMode::write);
        ofs << defaultConfiguration;
        file::fdUnlock(ofs);
        file::closeFile(ofs);
    }

    Config::getInstance();
}
} // namespace application::config