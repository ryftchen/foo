//! @file log.cpp
//! @author ryftchen
//! @brief The definitions (log) in the application module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#include "log.hpp"
#ifndef __PRECOMPILED_HEADER
#include <filesystem>
#include <regex>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

namespace application::log
{
Log::Log(
    const std::string& logFile,
    const OutputType type,
    const OutputLevel level,
    const OutputTarget target,
    const StateType initState) noexcept :
    writeType(type), minLevel(level), actualTarget(target), FSM(initState)
{
    std::strncpy(pathname, logFile.c_str(), logPathLength);
    pathname[logPathLength] = '\0';
}

Log& Log::getInstance()
{
    static Log logger;
    return logger;
}

void Log::runLogger()
{
    State expectedState = State::init;
    auto checkIfExceptedFSMState = [&](const State state) -> void
    {
        expectedState = state;
        if (currentState() != expectedState)
        {
            throw std::logic_error("<LOG> Abnormal logger state.");
        }
    };

    try
    {
        checkIfExceptedFSMState(State::init);
        processEvent(OpenFile());

        checkIfExceptedFSMState(State::idle);
        processEvent(GoLogging());

        checkIfExceptedFSMState(State::work);
        while (isLogging.load())
        {
            if (std::unique_lock<std::mutex> lock(mtx); true)
            {
                cv.wait(
                    lock,
                    [this]() -> decltype(auto)
                    {
                        return (!isLogging.load() || !logQueue.empty());
                    });

                utility::file::FileReadWriteGuard guard(utility::file::FileReadWriteGuard::LockMode::write, fileLock);
                while (!logQueue.empty())
                {
                    switch (actualTarget)
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
            }
        }

        processEvent(CloseFile());

        checkIfExceptedFSMState(State::idle);
        processEvent(NoLogging());

        checkIfExceptedFSMState(State::done);
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << " Expected state: " << expectedState
                  << ", current state: " << State(currentState()) << "." << std::endl;
        stopLogging();
    }
}

void Log::interfaceToStart()
{
    utility::time::BlockingTimer expiryTimer;
    std::uint16_t waitCount = 0;
    expiryTimer.set(
        [&]()
        {
            if ((State::work == currentState()) || (maxTimesOfWaitLogger == waitCount))
            {
                expiryTimer.reset();
            }
            else
            {
                ++waitCount;
#ifndef NDEBUG
                std::cout << "<LOG> Wait for the logger to start... (" << waitCount << ")" << std::endl;
#endif // NDEBUG
            }
        },
        intervalOfWaitLogger);
    expiryTimer.reset();
}

void Log::interfaceToStop()
{
    if (std::unique_lock<std::mutex> lock(mtx); true)
    {
        isLogging.store(false);

        lock.unlock();
        cv.notify_one();
        utility::time::millisecondLevelSleep(1);
        lock.lock();
    }

    utility::time::BlockingTimer expiryTimer;
    std::uint16_t waitCount = 0;
    expiryTimer.set(
        [&]()
        {
            if ((State::done == currentState()) || (maxTimesOfWaitLogger == waitCount))
            {
                expiryTimer.reset();
            }
            else
            {
                ++waitCount;
#ifndef NDEBUG
                std::cout << "<LOG> Wait for the logger to stop... (" << waitCount << ")" << std::endl;
#endif // NDEBUG
            }
        },
        intervalOfWaitLogger);
    expiryTimer.reset();
}

void Log::openLogFile()
{
    if (!std::filesystem::exists(logDirectory))
    {
        std::filesystem::create_directory(logDirectory);
        std::filesystem::permissions(
            logDirectory, std::filesystem::perms::owner_all, std::filesystem::perm_options::add);
    }

    switch (writeType)
    {
        case OutputType::add:
            ofs = utility::file::openFile(pathname, false);
            break;
        case OutputType::over:
            ofs = utility::file::openFile(pathname, true);
            break;
        default:
            break;
    }
};

void Log::startLogging()
{
    if (std::unique_lock<std::mutex> lock(mtx); true)
    {
        isLogging.store(true);
    }
};

void Log::closeLogFile()
{
    utility::file::closeFile(ofs);
};

void Log::stopLogging()
{
    if (std::unique_lock<std::mutex> lock(mtx); true)
    {
        isLogging.store(false);
        while (!logQueue.empty())
        {
            logQueue.pop();
        }
    }
}

bool Log::isLogFileOpen(const GoLogging& /*unused*/) const
{
    return ofs.is_open();
}

bool Log::isLogFileClose(const NoLogging& /*unused*/) const
{
    return !ofs.is_open();
}

//! @brief The operator (<<) overloading of the State enum.
//! @param os - output stream object
//! @param state - the specific value of State enum
//! @return reference of output stream object
std::ostream& operator<<(std::ostream& os, const Log::State& state)
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

    if (const std::regex date(std::string{dateRegex}); std::regex_search(line, date))
    {
        const auto searchIter = std::sregex_iterator(line.begin(), line.end(), date);
        const std::string timeColorForLog = std::string{utility::common::colorGray}
            + std::string{utility::common::colorBold} + std::string{utility::common::colorForBackground}
            + (*searchIter).str() + std::string{utility::common::colorOff};
        line = std::regex_replace(line, std::regex(std::string{dateRegex}), timeColorForLog);
    }

    if (const std::regex codeFile(std::string{codeFileRegex}); std::regex_search(line, codeFile))
    {
        const auto searchIter = std::sregex_iterator(line.begin(), line.end(), codeFile);
        const std::string codeFileColorForLog = std::string{utility::common::colorUnderLine}
            + std::string{utility::common::colorForBackground} + (*searchIter).str()
            + std::string{utility::common::colorOff};
        line = std::regex_replace(line, std::regex(std::string{codeFileRegex}), codeFileColorForLog);
    }

    return line;
}
} // namespace application::log
