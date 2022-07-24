#include "log.hpp"
#include <regex>

Log logger;

Log::Log() noexcept : minLevel(Level::levelDebug), realTarget(Target::targetAll)
{
    try
    {
        if (!std::filesystem::exists(LOG_DIR))
        {
            std::filesystem::create_directory(LOG_DIR);
            std::filesystem::permissions(
                LOG_DIR, std::filesystem::perms::owner_all, std::filesystem::perm_options::add);
        }

        ofs.open(pathname, std::ios_base::out | std::ios_base::app);
        if (!ofs)
        {
            throw OpenFileError(std::filesystem::path(pathname).filename().string());
        }
        tryToOperateFileLock(ofs, pathname, true, false);
    }
    catch (OpenFileError const& error)
    {
        std::cerr << error.what() << std::endl;
        realTarget = Target::targetTerminal;
    }
    catch (LockFileError const& error)
    {
        std::cerr << error.what() << std::endl;
        realTarget = Target::targetTerminal;
    }
}

Log::Log(
    const std::string& logFile, const Type type, const Level level, const Target target) noexcept
{
    minLevel = level;
    realTarget = target;
    std::strncpy(pathname, logFile.c_str(), LOG_PATHNAME_LENGTH);
    pathname[LOG_PATHNAME_LENGTH] = '\0';
    try
    {
        if (!std::filesystem::exists(LOG_DIR))
        {
            std::filesystem::create_directory(LOG_DIR);
            std::filesystem::permissions(
                LOG_DIR, std::filesystem::perms::owner_all, std::filesystem::perm_options::add);
        }

        switch (type)
        {
            case Type::typeAdd:
                ofs.open(pathname, std::ios_base::out | std::ios_base::app);
                break;
            case Type::typeOver:
                ofs.open(pathname, std::ios_base::out | std::ios_base::trunc);
                break;
            default:
                break;
        }

        if (!ofs)
        {
            throw OpenFileError(std::filesystem::path(pathname).filename().string());
        }
        tryToOperateFileLock(ofs, pathname, true, false);
    }
    catch (OpenFileError const& error)
    {
        std::cerr << error.what() << std::endl;
        realTarget = Target::targetTerminal;
    }
    catch (LockFileError const& error)
    {
        std::cerr << error.what() << std::endl;
        realTarget = Target::targetTerminal;
    }
}

Log::~Log()
{
    try
    {
        tryToOperateFileLock(ofs, pathname, false, false);
    }
    catch (LockFileError const& error)
    {
        std::cerr << error.what() << std::endl;
    }

    if (ofs.is_open())
    {
        ofs.close();
    }
}

std::ofstream& Log::getOfs() const
{
    return ofs;
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
