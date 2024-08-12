//! @file log.cpp
//! @author ryftchen
//! @brief The definitions (log) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#include "log.hpp"

#ifndef __PRECOMPILED_HEADER
#include <cassert>
#include <filesystem>
#include <fstream>
#include <regex>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

namespace application::log
{
//! @brief Anonymous namespace.
inline namespace
{
//! @brief Regular expressions for log highlighting.
struct HlRegex
{
    //! @brief Construct a new HlRegex object.
    HlRegex() noexcept = default;

    //! @brief Debug level prefix highlighting.
    const std::regex debugLevel{std::string{debugLevelPrefixRegex}};
    //! @brief Info level prefix highlighting.
    const std::regex infoLevel{std::string{infoLevelPrefixRegex}};
    //! @brief Warning level prefix highlighting.
    const std::regex warnLevel{std::string{warnLevelPrefixRegex}};
    //! @brief Error level prefix highlighting.
    const std::regex errorLevel{std::string{errorLevelPrefixRegex}};
    //! @brief Unknown level prefix highlighting.
    const std::regex unknownLevel{std::string{unknownLevelPrefixRegex}};
    //! @brief Date time highlighting.
    const std::regex dateTime{std::string{dateTimeRegex}};
    //! @brief Code file highlighting.
    const std::regex codeFile{std::string{codeFileRegex}};
};
//! @brief Log style.
const HlRegex& logStyle()
{
    static const HlRegex highlight{};
    return highlight;
};
} // namespace

Log& Log::getInstance()
{
    static Log logger{};
    return logger;
}

void Log::stateController()
{
retry:
    try
    {
        static_cast<void>(logStyle());
        assert(safeCurrentState() == State::init);
        safeProcessEvent(OpenFile());

        assert(safeCurrentState() == State::idle);
        if (std::unique_lock<std::mutex> lock(daemonMtx); true)
        {
            daemonCv.wait(
                lock,
                [this]()
                {
                    return ongoing.load();
                });
        }
        safeProcessEvent(GoLogging());

        assert(safeCurrentState() == State::work);
        while (ongoing.load())
        {
            std::unique_lock<std::mutex> lock(daemonMtx);
            daemonCv.wait(
                lock,
                [this]()
                {
                    return !ongoing.load() || !logQueue.empty() || toReset.load();
                });

            if (toReset.load())
            {
                break;
            }
            handleLogQueue();
        }

        if (toReset.load())
        {
            safeProcessEvent(Relaunch());
            goto retry; // NOLINT (hicpp-avoid-goto)
        }
        safeProcessEvent(CloseFile());

        assert(safeCurrentState() == State::idle);
        safeProcessEvent(NoLogging());

        assert(safeCurrentState() == State::done);
    }
    catch (const std::exception& err)
    {
        LOG_ERR << err.what() << " Current logger state: " << safeCurrentState() << '.';
        safeProcessEvent(Standby());

        if (awaitNotification4Rollback())
        {
            goto retry; // NOLINT (hicpp-avoid-goto)
        }
    }
}

void Log::waitForStart()
{
    while (!isInUninterruptedState(State::idle))
    {
        if (isInUninterruptedState(State::hold))
        {
            LOG_ERR << "The logger did not initialize successfully ...";
            return;
        }
        std::this_thread::yield();
    }

    if (std::unique_lock<std::mutex> lock(daemonMtx); true)
    {
        ongoing.store(true);

        lock.unlock();
        daemonCv.notify_one();
    }

    utility::time::BlockingTimer expiryTimer;
    std::uint32_t waitCounter = 0;
    expiryTimer.set(
        [this, &expiryTimer, &waitCounter]()
        {
            if (isInUninterruptedState(State::work))
            {
                expiryTimer.reset();
            }
            else
            {
                ++waitCounter;
            }

            if (timeoutPeriod == waitCounter)
            {
                LOG_ERR << "The logger did not start properly in " << timeoutPeriod << "ms ...";
                expiryTimer.reset();
            }
        },
        1);
}

void Log::waitForStop()
{
    if (std::unique_lock<std::mutex> lock(daemonMtx); true)
    {
        ongoing.store(false);

        lock.unlock();
        daemonCv.notify_one();
    }

    utility::time::BlockingTimer expiryTimer;
    std::uint32_t waitCounter = 0;
    expiryTimer.set(
        [this, &expiryTimer, &waitCounter]()
        {
            if (isInUninterruptedState(State::done))
            {
                expiryTimer.reset();
            }
            else
            {
                ++waitCounter;
            }

            if (timeoutPeriod == waitCounter)
            {
                LOG_ERR << "The logger did not stop properly in " << timeoutPeriod << "ms ...";
                expiryTimer.reset();
            }
        },
        1);
}

void Log::requestToReset()
{
    if (std::unique_lock<std::mutex> lock(daemonMtx); true)
    {
        toReset.store(true);
        lock.unlock();
        daemonCv.notify_one();
    }

    for (;;)
    {
        if (!toReset.load())
        {
            break;
        }
        std::this_thread::yield();
    }
}

std::string Log::loggerFilePath() const
{
    return filePath;
}

utility::common::ReadWriteLock& Log::loggerFileLock()
{
    return fileLock;
}

Log::State Log::safeCurrentState() const
{
    stateLock.lock();
    const auto state = State(currentState());
    stateLock.unlock();
    return state;
}

template <class T>
void Log::safeProcessEvent(const T& event)
{
    stateLock.lock();
    processEvent(event);
    stateLock.unlock();
}

bool Log::isInUninterruptedState(const State state) const
{
    return (safeCurrentState() == state) && !toReset.load();
}

void Log::handleLogQueue()
{
    namespace common = utility::common;

    common::ReadWriteGuard guard(fileLock, common::LockMode::write);
    while (!logQueue.empty())
    {
        switch (usedMedium)
        {
            case OutputMedium::file:
                logWriter.stream() << logQueue.front() << std::endl;
                break;
            case OutputMedium::terminal:
                std::cout << changeToLogStyle(logQueue.front()) << std::endl;
                break;
            case OutputMedium::both:
                logWriter.stream() << logQueue.front() << std::endl;
                std::cout << changeToLogStyle(logQueue.front()) << std::endl;
                break;
            default:
                break;
        }
        logQueue.pop();
    }
}

std::string Log::getFullLogPath(const std::string& filename)
{
    const char* const processHome = std::getenv("FOO_HOME");
    if (nullptr == processHome)
    {
        throw std::runtime_error("The environment variable FOO_HOME is not set.");
    }
    return std::string{processHome} + '/' + filename;
}

void Log::tryToCreateLogFolder() const
{
    const std::filesystem::path logFolderPath = std::filesystem::absolute(filePath).parent_path();
    std::filesystem::create_directories(logFolderPath);
    std::filesystem::permissions(logFolderPath, std::filesystem::perms::owner_all, std::filesystem::perm_options::add);
}

void Log::openLogFile()
{
    namespace common = utility::common;

    common::ReadWriteGuard guard(fileLock, common::LockMode::write);
    tryToCreateLogFolder();
    switch (writeType)
    {
        case OutputType::add:
            logWriter.open();
            break;
        case OutputType::over:
            logWriter.open(true);
            break;
        default:
            break;
    }
    logWriter.lock();
};

void Log::closeLogFile()
{
    namespace common = utility::common;

    common::ReadWriteGuard guard(fileLock, common::LockMode::write);
    logWriter.unlock();
    logWriter.close();
    if (std::filesystem::exists(filePath) && (std::filesystem::file_size(filePath) == 0))
    {
        std::filesystem::remove_all(std::filesystem::absolute(filePath).parent_path());
    }
}

void Log::startLogging()
{
}

void Log::stopLogging()
{
    std::unique_lock<std::mutex> lock(daemonMtx);
    ongoing.store(false);
    toReset.store(false);
    while (!logQueue.empty())
    {
        logQueue.pop();
    }
}

void Log::doToggle()
{
}

void Log::doRollback()
{
    std::unique_lock<std::mutex> lock(daemonMtx);
    ongoing.store(false);
    while (!logQueue.empty())
    {
        logQueue.pop();
    }

    if (logWriter.isOpen())
    {
        try
        {
            logWriter.lock();
        }
        catch (...)
        {
            toReset.store(false);
            return;
        }

        closeLogFile();
        tryToCreateLogFolder();
        std::ofstream tempOfs;
        tempOfs.open(filePath, std::ios_base::out | std::ios_base::trunc);
        tempOfs.close();
    }

    toReset.store(false);
}

bool Log::isLogFileOpen(const GoLogging& /*event*/) const
{
    return logWriter.isOpen();
}

bool Log::isLogFileClose(const NoLogging& /*event*/) const
{
    return !logWriter.isOpen();
}

bool Log::awaitNotification4Rollback()
{
    if (std::unique_lock<std::mutex> lock(daemonMtx); true)
    {
        daemonCv.wait(lock);
    }

    if (toReset.load())
    {
        safeProcessEvent(Relaunch());
        if (safeCurrentState() == State::init)
        {
            return true;
        }
        LOG_ERR << "Failed to rollback logger.";
    }

    return false;
}

//! @brief The operator (<<) overloading of the State enum.
//! @param os - output stream object
//! @param state - the specific value of State enum
//! @return reference of the output stream object
std::ostream& operator<<(std::ostream& os, const Log::State state)
{
    switch (state)
    {
        case Log::State::init:
            os << "INIT";
            break;
        case Log::State::idle:
            os << "IDLE";
            break;
        case Log::State::work:
            os << "WORK";
            break;
        case Log::State::done:
            os << "DONE";
            break;
        case Log::State::hold:
            os << "HOLD";
            break;
        default:
            os << "UNKNOWN: " << static_cast<std::underlying_type_t<Log::State>>(state);
    }

    return os;
}

//! @brief Change line string to log style.
//! @param line - target line to be changed
//! @return changed line
const std::string& changeToLogStyle(std::string& line)
{
    const auto& style = logStyle();
    if (std::regex_search(line, style.debugLevel))
    {
        line = std::regex_replace(line, style.debugLevel, std::string{debugLevelPrefixWithColor});
    }
    else if (std::regex_search(line, style.infoLevel))
    {
        line = std::regex_replace(line, style.infoLevel, std::string{infoLevelPrefixWithColor});
    }
    else if (std::regex_search(line, style.warnLevel))
    {
        line = std::regex_replace(line, style.warnLevel, std::string{warnLevelPrefixWithColor});
    }
    else if (std::regex_search(line, style.errorLevel))
    {
        line = std::regex_replace(line, style.errorLevel, std::string{errorLevelPrefixWithColor});
    }
    else if (std::regex_search(line, style.unknownLevel))
    {
        line = std::regex_replace(line, style.unknownLevel, std::string{unknownLevelPrefixWithColor});
    }

    namespace common = utility::common;
    if (std::regex_search(line, style.dateTime))
    {
        const auto searchIter = std::sregex_iterator(line.begin(), line.end(), style.dateTime);
        const std::string dateTimeWithColor =
            std::string{dateTimeBaseColor} + (*searchIter).str() + std::string{common::colorOff};
        line = std::regex_replace(line, style.dateTime, dateTimeWithColor);
    }
    if (std::regex_search(line, style.codeFile))
    {
        const auto searchIter = std::sregex_iterator(line.begin(), line.end(), style.codeFile);
        const std::string codeFileWithColor =
            std::string{codeFileBaseColor} + (*searchIter).str() + std::string{common::colorOff};
        line = std::regex_replace(line, style.codeFile, codeFileWithColor);
    }

    return line;
}
} // namespace application::log
