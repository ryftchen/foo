//! @file common.cpp
//! @author ryftchen
//! @brief The definitions (common) in the utility module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#include "common.hpp"
#include <cstring>
#include <iterator>
#include <list>

namespace utility::common
{
//! @brief Execute the command line.
//! @param cmd - target command line to be executed
void executeCommand(const char* const cmd)
{
    FILE* file = popen(cmd, "r");
    if (nullptr == file)
    {
        throw std::runtime_error("common: Failed to call the system API: popen().");
    }

    char resultBuffer[maxBufferSize + 1] = {'\0'};
    while (nullptr != std::fgets(resultBuffer, sizeof(resultBuffer), file))
    {
        if ('\n' == resultBuffer[std::strlen(resultBuffer) - 1])
        {
            resultBuffer[std::strlen(resultBuffer) - 1] = '\0';
        }
        std::cout << resultBuffer << std::endl;
    }

    const int status = pclose(file);
    if (-1 == status)
    {
        throw std::runtime_error("common: Failed to call the system API: pclose().");
    }
    else if (WIFEXITED(status))
    {
        if (WEXITSTATUS(status))
        {
            throw std::runtime_error("common: Failed to run common line: " + std::string{cmd} + ".");
        }
    }
}

//! @brief Print file contents.
//! @param pathname - target file to be printed
//! @param property - display property
void displayContentOfFile(const char* const pathname, const DisplayProperty& property)
{
    std::ifstream file;
    file.open(pathname, std::ios_base::in);
    if (!file)
    {
        throwOperateFileException(std::filesystem::path(pathname).filename().string(), true);
    }
    tryToOperateFileLock(file, pathname, LockOperationType::lock, FileLockType::readerLock);

    DisplayStyle formatStyle = property.style;
    if (nullStyle == formatStyle)
    {
        formatStyle = [](std::string& line) -> std::string&
        {
            return line;
        };
    }

    std::string line;
    std::list<std::string> content(0);
    file.seekg(std::ios::beg);
    if (!property.IsInverted)
    {
        while ((content.size() < property.maxLine) && std::getline(file, line))
        {
            content.emplace_back(formatStyle(line));
        }
    }
    else
    {
        std::ifstream statistics(pathname);
        const uint32_t lineNum = std::count(
                           std::istreambuf_iterator<char>(statistics), std::istreambuf_iterator<char>(), '\n'),
                       startLine = (lineNum > property.maxLine) ? (lineNum - property.maxLine + 1) : 1;
        for (uint32_t i = 0; i < (startLine - 1); ++i)
        {
            file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        while (std::getline(file, line))
        {
            content.emplace_front(formatStyle(line));
        }
    }
    std::copy(content.cbegin(), content.cend(), std::ostream_iterator<std::string>(std::cout, "\n"));

    tryToOperateFileLock(file, pathname, LockOperationType::unlock, FileLockType::readerLock);
    file.close();
}
} // namespace utility::common
