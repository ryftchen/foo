//! @file log.cpp
//! @author ryftchen
//! @brief The definitions (log) in the application module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen. All rights reserved.

#include "log.hpp"
#ifndef __PRECOMPILED_HEADER
#include <regex>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

namespace application::log
{
Log::Log(
    const char* const logFile,
    const OutputType type,
    const OutputLevel level,
    const OutputTarget target,
    const StateType initState) noexcept :
    writeType(type), minLevel(level), actTarget(target), FSM(initState)
{
    if (std::filesystem::absolute(std::filesystem::path{defaultLogFolderPath})
        != std::filesystem::absolute(logFile).parent_path())
    {
        std::strncpy(filePath, logFile, logPathLength);
        filePath[logPathLength] = '\0';
    }
    else
    {
        std::cerr << "Illegal log file path." << std::endl;
    }
}

Log& Log::getInstance()
{
    static Log logger{};
    return logger;
}

void Log::runLogger()
{
    State expectedState = State::init;
    const auto checkIfExceptedFSMState = [this, &expectedState](const State state)
    {
        expectedState = state;
        if (currentState() != expectedState)
        {
            throw std::logic_error("Abnormal logger state.");
        }
    };

    for (;;)
    {
        try
        {
            checkIfExceptedFSMState(State::init);
            processEvent(OpenFile());

            checkIfExceptedFSMState(State::idle);
            processEvent(GoLogging());

            checkIfExceptedFSMState(State::work);
            while (isLogging.load())
            {
                std::unique_lock<std::mutex> lock(mtx);
                cv.wait(
                    lock,
                    [this]()
                    {
                        return (!isLogging.load() || !logQueue.empty() || restartRequest.load());
                    });

                utility::file::ReadWriteGuard guard(utility::file::LockMode::write, fileLock);
                while (!logQueue.empty())
                {
                    switch (actTarget)
                    {
                        case OutputTarget::file:
                            ofs << logQueue.front() << std::endl;
                            break;
                        case OutputTarget::terminal:
                            std::cout << changeToLogStyle(logQueue.front()) << std::endl;
                            break;
                        case OutputTarget::all:
                            ofs << logQueue.front() << std::endl;
                            std::cout << changeToLogStyle(logQueue.front()) << std::endl;
                            break;
                        default:
                            break;
                    }
                    logQueue.pop();
                }

                if (restartRequest.load())
                {
                    break;
                }
            }

            if (restartRequest.load())
            {
                processEvent(Relaunch());
                continue;
            }
            processEvent(CloseFile());

            checkIfExceptedFSMState(State::idle);
            processEvent(NoLogging());

            checkIfExceptedFSMState(State::done);
        }
        catch (const std::exception& error)
        {
            LOG_ERR << error.what() << " Expected logger state: " << expectedState
                    << ", current logger state: " << State(currentState()) << '.';
            if (std::unique_lock<std::mutex> lock(mtx); true)
            {
                cv.wait(lock);
            }

            if (restartRequest.load())
            {
                processEvent(Relaunch());
                if (currentState() == State::init)
                {
                    continue;
                }
                else
                {
                    LOG_ERR << "Failed to restart logger.";
                }
            }
        }
        break;
    }
}

void Log::waitToStart()
{
    utility::time::BlockingTimer expiryTimer;
    std::uint16_t waitCount = 0;
    expiryTimer.set(
        [this, &expiryTimer, &waitCount]()
        {
            if (State::work == currentState())
            {
                expiryTimer.reset();
            }
            else
            {
                ++waitCount;
            }

            if (maxTimesOfWaitLogger == waitCount)
            {
                LOG_ERR << "The logger did not start properly...";
                expiryTimer.reset();
            }
        },
        intervalOfWaitLogger);
    expiryTimer.reset();
}

void Log::waitToStop()
{
    if (std::unique_lock<std::mutex> lock(mtx); true)
    {
        isLogging.store(false);
        restartRequest.store(false);

        lock.unlock();
        cv.notify_one();
        utility::time::millisecondLevelSleep(1);
        lock.lock();
    }

    utility::time::BlockingTimer expiryTimer;
    std::uint16_t waitCount = 0;
    expiryTimer.set(
        [this, &expiryTimer, &waitCount]()
        {
            if (State::done == currentState())
            {
                expiryTimer.reset();
            }
            else
            {
                ++waitCount;
            }

            if (maxTimesOfWaitLogger == waitCount)
            {
                LOG_ERR << "The logger did not stop properly...";
                expiryTimer.reset();
            }
        },
        intervalOfWaitLogger);
    expiryTimer.reset();
}

void Log::requestToRestart()
{
    std::unique_lock<std::mutex> lock(mtx);
    restartRequest.store(true);
    lock.unlock();
    cv.notify_one();
    utility::time::millisecondLevelSleep(1);
    lock.lock();
}

void Log::openLogFile()
{
    const std::filesystem::path logFolderPath = std::filesystem::absolute(filePath).parent_path();
    if (!std::filesystem::exists(logFolderPath))
    {
        std::filesystem::create_directory(logFolderPath);
        std::filesystem::permissions(
            logFolderPath, std::filesystem::perms::owner_all, std::filesystem::perm_options::add);
    }

    namespace file = utility::file;
    switch (writeType)
    {
        case OutputType::add:
            ofs = file::openFile(filePath, false);
            break;
        case OutputType::over:
            ofs = file::openFile(filePath, true);
            break;
        default:
            break;
    }
    file::fdLock(ofs, file::LockMode::write);
};

void Log::startLogging()
{
    std::unique_lock<std::mutex> lock(mtx);
    isLogging.store(true);
};

void Log::closeLogFile()
{
    utility::file::fdUnlock(ofs);
    utility::file::closeFile(ofs);
    if (std::filesystem::exists(filePath) && (std::filesystem::file_size(filePath) == 0))
    {
        std::filesystem::remove_all(std::filesystem::absolute(filePath).parent_path());
    }
};

void Log::stopLogging()
{
    std::unique_lock<std::mutex> lock(mtx);
    isLogging.store(false);
    restartRequest.store(false);
    while (!logQueue.empty())
    {
        logQueue.pop();
    }
}

void Log::rollBack()
{
    if (ofs.is_open())
    {
        closeLogFile();

        auto tempOfs = utility::file::openFile(filePath, true);
        utility::file::closeFile(tempOfs);
    }
    stopLogging();
}

bool Log::isLogFileOpen(const GoLogging& /*event*/) const
{
    return ofs.is_open();
}

bool Log::isLogFileClose(const NoLogging& /*event*/) const
{
    return !ofs.is_open();
}

//! @brief The operator (<<) overloading of the State enum.
//! @param os - output stream object
//! @param state - the specific value of State enum
//! @return reference of output stream object
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
    if (std::regex_search(line, std::regex(std::string{debugLevelPrefixRegex})))
    {
        line = std::regex_replace(
            line, std::regex(std::string{debugLevelPrefixRegex}), std::string{debugLevelPrefixColorForLog});
    }
    else if (std::regex_search(line, std::regex(std::string{infoLevelPrefixRegex})))
    {
        line = std::regex_replace(
            line, std::regex(std::string{infoLevelPrefixRegex}), std::string{infoLevelPrefixColorForLog});
    }
    else if (std::regex_search(line, std::regex(std::string{warnLevelPrefixRegex})))
    {
        line = std::regex_replace(
            line, std::regex(std::string{warnLevelPrefixRegex}), std::string{warnLevelPrefixColorForLog});
    }
    else if (std::regex_search(line, std::regex(std::string{errorLevelPrefixRegex})))
    {
        line = std::regex_replace(
            line, std::regex(std::string{errorLevelPrefixRegex}), std::string{errorLevelPrefixColorForLog});
    }

    namespace common = utility::common;
    if (const std::regex date(std::string{dateRegex}); std::regex_search(line, date))
    {
        const auto searchIter = std::sregex_iterator(line.begin(), line.end(), date);
        const std::string timeColorForLog = std::string{common::colorGray} + std::string{common::colorBold}
            + std::string{common::colorForBackground} + (*searchIter).str() + std::string{common::colorOff};
        line = std::regex_replace(line, std::regex(std::string{dateRegex}), timeColorForLog);
    }
    if (const std::regex codeFile(std::string{codeFileRegex}); std::regex_search(line, codeFile))
    {
        const auto searchIter = std::sregex_iterator(line.begin(), line.end(), codeFile);
        const std::string codeFileColorForLog = std::string{common::colorUnderLine}
            + std::string{common::colorForBackground} + (*searchIter).str() + std::string{common::colorOff};
        line = std::regex_replace(line, std::regex(std::string{codeFileRegex}), codeFileColorForLog);
    }

    return line;
}
} // namespace application::log
