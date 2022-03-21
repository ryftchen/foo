#include "log.hpp"
#include <ext/stdio_filebuf.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <list>
#include <regex>
#include <vector>

Log logger;

Log::Log() noexcept
{
    minLevel = Level::levelDebug;
    realTarget = Target::targetAll;
    strncpy(pathname, LOG_PATH, LOG_PATHNAME_LENGTH);
    pathname[LOG_PATHNAME_LENGTH] = '\0';
    try
    {
        if (-1 == access(LOG_DIR, F_OK))
        {
            if (0 != mkdir(LOG_DIR, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)) // rwx r-x r-x
            {
                throw CreateFolderError(basename(LOG_DIR));
            }
        }

        ofs.open(pathname, std::ios_base::out | std::ios_base::app);
        if (!ofs)
        {
            throw OpenFileError(basename(pathname));
        }

        const int fd = static_cast<__gnu_cxx::stdio_filebuf<char>* const>(ofs.rdbuf())->fd();
        if (flock(fd, LOCK_EX | LOCK_NB))
        {
            throw LockWriterLockError(basename(pathname));
        }
    }
    catch (CreateFolderError const& error)
    {
        std::cerr << error.what() << std::endl;
        realTarget = Target::targetTerminal;
    }
    catch (OpenFileError const& error)
    {
        std::cerr << error.what() << std::endl;
        realTarget = Target::targetTerminal;
    }
    catch (LockWriterLockError const& error)
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
    strncpy(pathname, logFile.c_str(), LOG_PATHNAME_LENGTH);
    pathname[LOG_PATHNAME_LENGTH] = '\0';
    try
    {
        if (-1 == access(LOG_DIR, F_OK))
        {
            if (0 != mkdir(LOG_DIR, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)) // rwx r-x r-x
            {
                throw CreateFolderError(basename(LOG_DIR));
            }
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
            throw OpenFileError(basename(pathname));
        }

        const int fd = static_cast<__gnu_cxx::stdio_filebuf<char>* const>(ofs.rdbuf())->fd();
        if (flock(fd, LOCK_EX | LOCK_NB))
        {
            throw LockWriterLockError(basename(pathname));
        }
    }
    catch (CreateFolderError const& error)
    {
        std::cerr << error.what() << std::endl;
        realTarget = Target::targetTerminal;
    }
    catch (OpenFileError const& error)
    {
        std::cerr << error.what() << std::endl;
        realTarget = Target::targetTerminal;
    }
    catch (LockWriterLockError const& error)
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
            throw UnlockWriterLockError(basename(pathname));
        }
    }
    catch (UnlockWriterLockError const& error)
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
    try
    {
        file.open(pathname, std::ios_base::in);
        if (!file)
        {
            throw OpenFileError(basename(pathname));
        }

        const int fd = static_cast<__gnu_cxx::stdio_filebuf<char>* const>(file.rdbuf())->fd();
        if (flock(fd, LOCK_SH | LOCK_NB))
        {
            throw LockReaderLockError(basename(pathname));
        }

        PrintStyle formatStyle = style;
        if (nullStyle == formatStyle)
        {
            formatStyle = [](std::string& line)
            {
                return line;
            };
        }

        std::string line = "";
        std::list<std::string> context(0);
        if (false == reverse)
        {
            while ((context.size() < maxLine) && getline(file, line))
            {
                context.emplace_back(formatStyle(line));
            }
        }
        else
        {
            while ((context.size() < maxLine) && getline(file, line))
            {
                context.emplace_front(formatStyle(line));
            }
        }

        for (const auto& printLine : context)
        {
            std::cout << printLine << std::endl;
        }

        if (flock(fd, LOCK_UN))
        {
            throw UnlockReaderLockError(basename(pathname));
        }
        file.close();
    }
    catch (OpenFileError const& error)
    {
        LOGGER_ERR(error.what());
    }
    catch (LockReaderLockError const& error)
    {
        LOGGER_ERR(error.what());
        file.close();
    }
    catch (UnlockReaderLockError const& error)
    {
        LOGGER_ERR(error.what());
        file.close();
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
