#include "log.hpp"
#include <ext/stdio_filebuf.h>
#include <sys/file.h>
#include <list>
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

        const int fd = static_cast<__gnu_cxx::stdio_filebuf<char>* const>(ofs.rdbuf())->fd();
        if (flock(fd, LOCK_EX | LOCK_NB))
        {
            throwLockFileException(
                std::filesystem::path(pathname).filename().string(), true, false);
        }
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

        const int fd = static_cast<__gnu_cxx::stdio_filebuf<char>* const>(ofs.rdbuf())->fd();
        if (flock(fd, LOCK_EX | LOCK_NB))
        {
            throwLockFileException(
                std::filesystem::path(pathname).filename().string(), true, false);
        }
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
        const int fd = static_cast<__gnu_cxx::stdio_filebuf<char>* const>(ofs.rdbuf())->fd();
        if (flock(fd, LOCK_UN))
        {
            throwLockFileException(
                std::filesystem::path(pathname).filename().string(), false, false);
        }
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

const std::ofstream& Log::getOfs() const
{
    return ofs;
}

void printFile(
    const char* const pathname, const bool reverse, const uint32_t maxLine, PrintStyle style)
{
    std::ifstream file;

    file.open(pathname, std::ios_base::in);
    if (!file)
    {
        throw OpenFileError(std::filesystem::path(pathname).filename().string());
    }

    const int fd = static_cast<__gnu_cxx::stdio_filebuf<char>* const>(file.rdbuf())->fd();
    if (flock(fd, LOCK_SH | LOCK_NB))
    {
        file.close();
        throwLockFileException(std::filesystem::path(pathname).filename().string(), true, true);
    }

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
        uint32_t currentLine = 0, startLine = 0;
        (lineNum > maxLine) ? (startLine = lineNum - maxLine + 1) : (startLine = 1);
        while (std::getline(file, line))
        {
            ++currentLine;
            if (currentLine >= startLine)
            {
                context.emplace_front(formatStyle(line));
            }
        }
        assert(context.size() <= maxLine);
    }

    for (const auto& printLine : context)
    {
        std::cout << printLine << std::endl;
    }

    if (flock(fd, LOCK_UN))
    {
        file.close();
        throwLockFileException(std::filesystem::path(pathname).filename().string(), false, true);
    }
    file.close();
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
