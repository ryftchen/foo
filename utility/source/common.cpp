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
        throwCallSystemAPIException("popen()");
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
        throwCallSystemAPIException("pclose()");
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
//! @param reverse - reverse or not reverse
//! @param maxLine - maximum number of lines to print
//! @param style - print style
void printFile(const char* const pathname, const bool reverse, const uint32_t maxLine, PrintStyle style)
{
    std::ifstream file;
    file.open(pathname, std::ios_base::in);
    if (!file)
    {
        throwOperateFileException(std::filesystem::path(pathname).filename().string(), true);
    }
    tryToOperateFileLock(file, pathname, LockOperationType::lock, FileLockType::readerLock);

    PrintStyle formatStyle = style;
    if (nullStyle == formatStyle)
    {
        formatStyle = [](std::string& line) -> std::string&
        {
            return line;
        };
    }

    std::string line;
    std::list<std::string> content(0);
    if (!reverse)
    {
        while ((content.size() < maxLine) && std::getline(file, line))
        {
            content.emplace_back(formatStyle(line));
        }
    }
    else
    {
        std::ifstream fileTmp(pathname);
        uint32_t lineNum = std::count(std::istreambuf_iterator<char>(fileTmp), std::istreambuf_iterator<char>(), '\n'),
                 startLine = (lineNum > maxLine) ? (lineNum - maxLine + 1) : 1, currentLine = 0;
        while (std::getline(file, line))
        {
            ++currentLine;
            if (currentLine >= startLine)
            {
                content.emplace_front(formatStyle(line));
            }
        }
        assert(maxLine >= content.size());
    }
    std::copy(content.cbegin(), content.cend(), std::ostream_iterator<std::string>(std::cout, "\n"));

    tryToOperateFileLock(file, pathname, LockOperationType::unlock, FileLockType::readerLock);
    file.close();
}
} // namespace utility::common
