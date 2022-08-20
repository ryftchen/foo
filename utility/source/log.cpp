#include "log.hpp"
#include <regex>

Log logger;

Log::Log(
    const std::string& logFile, const Type type, const Level level, const Target target) noexcept :
    writeType(type),
    minLevel(level), realTarget(target)
{
    std::strncpy(pathname, logFile.c_str(), LOG_PATHNAME_LENGTH);
    pathname[LOG_PATHNAME_LENGTH] = '\0';
}

void Log::runLogger()
{
    try
    {
        if (!std::filesystem::exists(LOG_DIR))
        {
            std::filesystem::create_directory(LOG_DIR);
            std::filesystem::permissions(
                LOG_DIR, std::filesystem::perms::owner_all, std::filesystem::perm_options::add);
        }

        switch (writeType)
        {
            case Type::add:
                ofs.open(pathname, std::ios_base::out | std::ios_base::app);
                break;
            case Type::over:
                ofs.open(pathname, std::ios_base::out | std::ios_base::trunc);
                break;
            default:
                break;
        }

        if (!ofs)
        {
            throwOperateFileException(std::filesystem::path(pathname).filename().string(), true);
        }
        tryToOperateFileLock(ofs, pathname, true, false);

        isLogging = true;
        while (isLogging)
        {
            if (std::unique_lock<std::mutex> lock(logQueueMutex); true)
            {
                logCondition.wait(
                    lock,
                    [this]() -> decltype(auto)
                    {
                        return !isLogging || !logQueue.empty();
                    });

                while (!logQueue.empty())
                {
                    switch (realTarget)
                    {
                        case Target::file:
                            ofs << logQueue.front() << std::endl;
                            break;
                        case Target::terminal:
                            std::cout << changeLogLevelStyle(logQueue.front()) << std::endl;
                            break;
                        case Target::all:
                            ofs << logQueue.front() << std::endl;
                            std::cout << changeLogLevelStyle(logQueue.front()) << std::endl;
                            break;
                        default:
                            break;
                    }
                    logQueue.pop();
                }
            }
        }

        if (!isLogging)
        {
            tryToOperateFileLock(ofs, pathname, false, false);

            if (ofs.is_open())
            {
                ofs.close();
            }
        }
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << std::endl;
    }
}

void Log::exit()
{
    if (isLogging)
    {
        isLogging = false;
        logCondition.notify_one();
    }
}

std::string changeLogLevelStyle(std::string& line)
{
    if (std::regex_search(line, std::regex(LOG_REGEX_INFO)))
    {
        line = std::regex_replace(line, std::regex(LOG_REGEX_INFO), LOG_COLOR_INFO);
    }
    else if (std::regex_search(line, std::regex(LOG_REGEX_WARN)))
    {
        line = std::regex_replace(line, std::regex(LOG_REGEX_WARN), LOG_COLOR_WARN);
    }
    else if (std::regex_search(line, std::regex(LOG_REGEX_ERROR)))
    {
        line = std::regex_replace(line, std::regex(LOG_REGEX_ERROR), LOG_COLOR_ERROR);
    }

    return line;
}
