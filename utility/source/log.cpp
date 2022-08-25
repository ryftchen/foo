#include "log.hpp"
#include <regex>

Log logger;

Log::Log(
    const std::string& logFile, const OutputType type, const OutputLevel level,
    const OutputTarget target, StateType initState) noexcept :
    writeType(type),
    minLevel(level), actualTarget(target), FSM(initState)
{
    std::strncpy(pathname, logFile.c_str(), LOG_PATHNAME_LENGTH);
    pathname[LOG_PATHNAME_LENGTH] = '\0';
}

Log::~Log()
{
    waitLoggerStop();
}

void Log::runLogger()
{
    State expectedState = State::idle;
    auto targetState = [&](const State state) -> bool
    {
        expectedState = state;
        return (currentState() == expectedState);
    };

    try
    {
        checkIfExceptedFSMState(targetState(State::init));
        processEvent(OpenFile());

        checkIfExceptedFSMState(targetState(State::idle));
        processEvent(GoLogging());

        checkIfExceptedFSMState(targetState(State::work));
        while (isLogging)
        {
            if (std::unique_lock<std::mutex> lock(logQueueMutex); true)
            {
                loggingCondition.wait(
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

        checkIfExceptedFSMState(targetState(State::idle));
        processEvent(NoLogging());

        checkIfExceptedFSMState(targetState(State::done));
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << std::endl;
        std::cerr << "FSM's expected state: " << expectedState
                  << ", FSM's current state: " << Log::State(currentState()) << std::endl;
        stopLogging();
    }
}

void Log::waitLoggerStart()
{
    while (State::work != currentState())
    {
        TIME_SLEEP_MILLISECOND(1);
    }
}

void Log::waitLoggerStop()
{
    if (std::unique_lock<std::mutex> lock(logQueueMutex); true)
    {
        isLogging = false;

        lock.unlock();
        loggingCondition.notify_one();
        TIME_SLEEP_MILLISECOND(1);
        lock.lock();
    }

    while (State::done != currentState())
    {
        TIME_SLEEP_MILLISECOND(1);
    }
}

void Log::openLogFile()
{
    if (!std::filesystem::exists(LOG_DIR))
    {
        std::filesystem::create_directory(LOG_DIR);
        std::filesystem::permissions(
            LOG_DIR, std::filesystem::perms::owner_all, std::filesystem::perm_options::add);
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
        throwOperateFileException(std::filesystem::path(pathname).filename().string(), true);
    }
    tryToOperateFileLock(ofs, pathname, true, false);
};

void Log::startLogging()
{
    if (std::unique_lock<std::mutex> lock(logQueueMutex); true)
    {
        isLogging = true;
    }
};

void Log::closeLogFile()
{
    tryToOperateFileLock(ofs, pathname, false, false);
    if (ofs.is_open())
    {
        ofs.close();
    }
};

void Log::stopLogging()
{
    if (std::unique_lock<std::mutex> lock(logQueueMutex); true)
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
