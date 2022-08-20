#include "file.hpp"
#include <cstring>
#include <iterator>
#include <list>

void executeCommand(const char* const cmd)
{
    try
    {
        FILE* file = popen(cmd, "r");
        if (nullptr == file)
        {
            throwCallSystemApiException("popen");
        }

        char resultBuffer[BUFFER_SIZE_MAX + 1] = {'\0'};
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
            throwCallSystemApiException("pclose");
        }
        else if (WIFEXITED(status))
        {
            if (WEXITSTATUS(status))
            {
                throwRunCommandLineException(cmd);
            }
        }
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << std::endl;
    }
}

void printFile(
    const char* const pathname, const bool reverse, const uint32_t maxLine, PrintStyle style)
{
    try
    {
        std::ifstream file;
        file.open(pathname, std::ios_base::in);
        if (!file)
        {
            throwOperateFileException(std::filesystem::path(pathname).filename().string(), true);
        }
        tryToOperateFileLock(file, pathname, true, true);

        PrintStyle formatStyle = style;
        if (nullStyle == formatStyle)
        {
            formatStyle = [](std::string& line)
            {
                return line;
            };
        }

        std::string line;
        std::list<std::string> context(0);
        if (!reverse)
        {
            while ((context.size() < maxLine) && std::getline(file, line))
            {
                context.emplace_back(formatStyle(line));
            }
        }
        else
        {
            std::ifstream fileTmp(pathname);
            uint32_t lineNum = std::count(
                std::istreambuf_iterator<char>(fileTmp), std::istreambuf_iterator<char>(), '\n');
            uint32_t currentLine = 0;
            uint32_t startLine = (lineNum > maxLine) ? (lineNum - maxLine + 1) : 1;
            while (std::getline(file, line))
            {
                ++currentLine;
                if (currentLine >= startLine)
                {
                    context.emplace_front(formatStyle(line));
                }
            }
            assert(maxLine >= context.size());
        }
        std::copy(
            context.cbegin(), context.cend(), std::ostream_iterator<std::string>(std::cout, "\n"));

        tryToOperateFileLock(file, pathname, false, true);
        file.close();
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << std::endl;
    }
}

void throwRunCommandLineException(const std::string& str)
{
    throw std::runtime_error("Failed to run common line: " + str);
}

void throwCallSystemApiException(const std::string& str)
{
    throw std::runtime_error("Failed to call system api: " + str);
}

void throwOperateLockException(const std::string& name, const bool isToLock, const bool isReader)
{
    std::string operate = isToLock ? "lock" : "unlock";
    std::string type = isReader ? "reader" : "writer";
    throw std::runtime_error("Failed to " + operate + " " + type + " lock: " + name);
}

void throwOperateFileException(const std::string& name, const bool isToOpen)
{
    std::string operate = isToOpen ? "open" : "close";
    throw std::runtime_error("Failed to " + operate + " file: " + name);
}
