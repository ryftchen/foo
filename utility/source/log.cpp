#include "log.hpp"
#include <regex>

namespace util_log
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

void Log::runLogger()
{
    try
    {
        util_fsm::checkIfExceptedFSMState(currentState(), State::init);
        processEvent(OpenFile());

        util_fsm::checkIfExceptedFSMState(currentState(), State::idle);
        processEvent(GoLogging());

        util_fsm::checkIfExceptedFSMState(currentState(), State::work);
        while (isLogging.load())
        {
            if (std::unique_lock<std::mutex> lock(queueMutex); true)
            {
                condition.wait(
                    lock,
                    [this]() -> decltype(auto)
                    {
                        return (!isLogging.load() || !logQueue.empty());
                    });

                while (!logQueue.empty())
                {
                    switch (actualTarget)
                    {
                        case OutputTarget::file:
                            ofs << logQueue.front() << std::endl;
                            break;
                        case OutputTarget::terminal:
                            std::cout << changeLogLevelStyle(logQueue.front()) << std::endl;
                            break;
                        case OutputTarget::all:
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

        processEvent(CloseFile());

        util_fsm::checkIfExceptedFSMState(currentState(), State::idle);
        processEvent(NoLogging());

        util_fsm::checkIfExceptedFSMState(currentState(), State::done);
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << std::endl;
        stopLogging();
    }
}

void Log::waitStartForExternalUse()
{
    util_time::Time timer;
    uint32_t waitCount = 0;
    timer.setBlockingTimer(
        [&]()
        {
            if ((State::work == currentState()) || (maxCountOfWaitLogger == waitCount))
            {
                timer.resetBlockingTimer();
            }
            else
            {
                ++waitCount;
                std::cout << "log: Wait logger start... (" << waitCount << ")" << std::endl;
            }
        },
        intervalOfWaitLogger);
    timer.resetBlockingTimer();
}

void Log::waitStopForExternalUse()
{
    if (std::unique_lock<std::mutex> lock(queueMutex); true)
    {
        isLogging.store(false);

        lock.unlock();
        condition.notify_one();
        util_time::millisecondLevelSleep(1);
        lock.lock();
    }

    util_time::Time timer;
    uint32_t waitCount = 0;
    timer.setBlockingTimer(
        [&]()
        {
            if ((State::done == currentState()) || (maxCountOfWaitLogger == waitCount))
            {
                timer.resetBlockingTimer();
            }
            else
            {
                ++waitCount;
                std::cout << "log: Wait logger stop... (" << waitCount << ")" << std::endl;
            }
        },
        intervalOfWaitLogger);
    timer.resetBlockingTimer();
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
            ofs.open(pathname, std::ios_base::out | std::ios_base::app);
            break;
        case OutputType::over:
            ofs.open(pathname, std::ios_base::out | std::ios_base::trunc);
            break;
        default:
            break;
    }

    if (!ofs)
    {
        util_common::throwOperateFileException(std::filesystem::path(pathname).filename().string(), true);
    }
    util_common::tryToOperateFileLock(
        ofs, pathname, util_common::LockOperateType::lock, util_common::FileLockType::writerLock);
};

void Log::startLogging()
{
    if (std::unique_lock<std::mutex> lock(queueMutex); true)
    {
        isLogging.store(true);
    }
};

void Log::closeLogFile()
{
    util_common::tryToOperateFileLock(
        ofs, pathname, util_common::LockOperateType::unlock, util_common::FileLockType::writerLock);
    if (ofs.is_open())
    {
        ofs.close();
    }
};

void Log::stopLogging()
{
    if (std::unique_lock<std::mutex> lock(queueMutex); true)
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

std::string& changeLogLevelStyle(std::string& line)
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

    if (const std::regex time(std::string{timeRegex}); std::regex_search(line, time))
    {
        const auto searchIter = std::sregex_iterator(line.begin(), line.end(), time);
        const std::string timeColorForLog = std::string{util_common::colorGray} + std::string{util_common::colorBold}
            + std::string{util_common::colorForBackground} + (*searchIter).str() + std::string{util_common::colorOff};
        line = std::regex_replace(line, std::regex(std::string{timeRegex}), timeColorForLog);
    }

    if (const std::regex codeFile(std::string{codeFileRegex}); std::regex_search(line, codeFile))
    {
        const auto searchIter = std::sregex_iterator(line.begin(), line.end(), codeFile);
        const std::string codeFileColorForLog = std::string{util_common::colorUnderLine}
            + std::string{util_common::colorForBackground} + (*searchIter).str() + std::string{util_common::colorOff};
        line = std::regex_replace(line, std::regex(std::string{codeFileRegex}), codeFileColorForLog);
    }

    return line;
}
} // namespace util_log
