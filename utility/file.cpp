#include "file.hpp"
#include <iterator>
#include <list>

void executeCommand(const char* const cmd)
{
    try
    {
        FILE* file = popen(cmd, "r");
        if (nullptr == file)
        {
            throw CallFunctionError("popen()");
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
            throw CallFunctionError("pclose()");
        }
        else if (WIFEXITED(status))
        {
            if (WEXITSTATUS(status))
            {
                throw ExecuteCommandError(cmd);
            }
        }
    }
    catch (CallFunctionError const& error)
    {
        std::cerr << error.what() << std::endl;
    }
    catch (ExecuteCommandError const& error)
    {
        std::cerr << error.what() << std::endl;
    }
}

void printFile(
    const char* const pathname, const bool reverse, const uint32_t maxLine, PrintStyle style)
{
    std::ifstream file;
    try
    {
        file.open(pathname, std::ios_base::in);
        if (!file)
        {
            throw OpenFileError(std::filesystem::path(pathname).filename().string());
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
    catch (OpenFileError const& error)
    {
        std::cerr << error.what() << std::endl;
    }
    catch (LockFileError const& error)
    {
        file.close();
        std::cerr << error.what() << std::endl;
    }
}
