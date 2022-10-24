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
    State expectedState = State::idle;
    auto isTargetState = [&](const State state) -> bool
    {
        expectedState = state;
        return (currentState() == expectedState);
    };

    try
    {
        util_fsm::checkIfExceptedFSMState(isTargetState(State::init));
        processEvent(OpenFile());

        util_fsm::checkIfExceptedFSMState(isTargetState(State::idle));
        processEvent(GoLogging());

        util_fsm::checkIfExceptedFSMState(isTargetState(State::work));
        while (isLogging)
        {
            if (std::unique_lock<std::mutex> lock(queueMutex); true)
            {
                logCondition.wait(
                    lock,
                    [this]() -> decltype(auto)
                    {
                        return !isLogging || !logQueue.empty();
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

        util_fsm::checkIfExceptedFSMState(isTargetState(State::idle));
        processEvent(NoLogging());

        util_fsm::checkIfExceptedFSMState(isTargetState(State::done));
    }
    catch (const std::exception& error)
    {
        std::cerr << "log: " << error.what() << ", expected state: " << expectedState
                  << ", current state: " << State(currentState()) << std::endl;
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
        1);
    timer.resetBlockingTimer();
}

void Log::waitStopForExternalUse()
{
    if (std::unique_lock<std::mutex> lock(queueMutex); true)
    {
        isLogging = false;

        lock.unlock();
        logCondition.notify_one();
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
        1);
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
        isLogging = true;
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
        isLogging = false;
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
    if (std::regex_search(line, std::regex(std::string{infoRegex})))
    {
        line = std::regex_replace(line, std::regex(std::string{infoRegex}), std::string{infoColorForLog});
    }
    else if (std::regex_search(line, std::regex(std::string{warnRegex})))
    {
        line = std::regex_replace(line, std::regex(std::string{warnRegex}), std::string{warnColorForLog});
    }
    else if (std::regex_search(line, std::regex(std::string{errorRegex})))
    {
        line = std::regex_replace(line, std::regex(std::string{errorRegex}), std::string{errorColorForLog});
    }

    return line;
}

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
} // namespace util_log
