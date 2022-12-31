//! @file log.hpp
//! @author ryftchen
//! @brief The declarations (log) in the utility module.
//! @version 0.1
//! @copyright Copyright (c) 2022
#pragma once

#include <condition_variable>
#include <queue>
#include "common.hpp"
#include "fsm.hpp"
#include "time.hpp"

//! @brief Macro for starting to log.
#define LOG_TO_START utility::log::Log::getInstance().waitStartForExternalUse()
//! @brief Macro for stopping to log.
#define LOG_TO_STOP utility::log::Log::getInstance().waitStopForExternalUse()
//! @brief Macro for logging with debug level.
#define LOG_DBG(format, args...) \
    utility::log::Log::getInstance().flush(utility::log::Log::OutputLevel::debug, __FILE__, __LINE__, format, ##args)
//! @brief Macro for logging with info level.
#define LOG_INF(format, args...) \
    utility::log::Log::getInstance().flush(utility::log::Log::OutputLevel::info, __FILE__, __LINE__, format, ##args)
//! @brief Macro for logging with warning level.
#define LOG_WRN(format, args...) \
    utility::log::Log::getInstance().flush(utility::log::Log::OutputLevel::warn, __FILE__, __LINE__, format, ##args)
//! @brief Macro for logging with error level.
#define LOG_ERR(format, args...) \
    utility::log::Log::getInstance().flush(utility::log::Log::OutputLevel::error, __FILE__, __LINE__, format, ##args)

//! @brief Namespace for log-related functions in the utility module.
namespace utility::log
{
//! @brief Log file path.
inline constexpr std::string_view logPath{"./temporary/foo.log"};
//! @brief Length of the log file path.
constexpr uint32_t logPathLength = 32;
//! @brief The maximum number of times to wait for the logger to change to the target state.
constexpr uint32_t maxTimesOfWaitLogger = 10;
//! @brief The time interval (ms) to wait for the logger to change to the target state.
constexpr uint32_t intervalOfWaitLogger = 5;
//! @brief Log file directory.
constexpr std::string_view logDirectory{"./temporary"};
//! @brief Prefix of debug level in log.
constexpr std::string_view debugLevelPrefix{"[DBG]"};
//! @brief Prefix of info level in log.
constexpr std::string_view infoLevelPrefix{"[INF]"};
//! @brief Prefix of warning level in log.
constexpr std::string_view warnLevelPrefix{"[WRN]"};
//! @brief Prefix of error level in log.
constexpr std::string_view errorLevelPrefix{"[ERR]"};
//! @brief Regular expression of debug level in log.
constexpr std::string_view debugLevelPrefixRegex{R"(^\[DBG\])"};
//! @brief Regular expression of info level in log.
constexpr std::string_view infoLevelPrefixRegex{R"(^\[INF\])"};
//! @brief Regular expression of warning level in log.
constexpr std::string_view warnLevelPrefixRegex{R"(^\[WRN\])"};
//! @brief Regular expression of error level in log.
constexpr std::string_view errorLevelPrefixRegex{R"(^\[ERR\])"};
//! @brief Regular expression of date in log.
constexpr std::string_view dateRegex{R"(\[(\d{4})-(\d{2})-(\d{2}) (\d{2}):(\d{2}):(\d{2})\.(\d{6}) (\w{3})\])"};
//! @brief Regular expression of code file in log.
constexpr std::string_view codeFileRegex{R"(\[[^:]+\.(c|h|cc|hh|cpp|hpp|tpp|cxx|hxx|C|H)+#\d+\])"};
//! @brief Prefix of debug level. Include ANSI escape codes.
constexpr auto debugLevelPrefixColorForLog{utility::common::joinStr<
    utility::common::colorBlue,
    utility::common::colorBold,
    utility::common::colorForBackground,
    debugLevelPrefix,
    utility::common::colorOff>};
//! @brief Prefix of info level. Include ANSI escape codes.
constexpr auto infoLevelPrefixColorForLog{utility::common::joinStr<
    utility::common::colorGreen,
    utility::common::colorBold,
    utility::common::colorForBackground,
    infoLevelPrefix,
    utility::common::colorOff>};
//! @brief Prefix of warning level. Include ANSI escape codes.
constexpr auto warnLevelPrefixColorForLog{utility::common::joinStr<
    utility::common::colorYellow,
    utility::common::colorBold,
    utility::common::colorForBackground,
    warnLevelPrefix,
    utility::common::colorOff>};
//! @brief Prefix of error level. Include ANSI escape codes.
constexpr auto errorLevelPrefixColorForLog{utility::common::joinStr<
    utility::common::colorRed,
    utility::common::colorBold,
    utility::common::colorForBackground,
    errorLevelPrefix,
    utility::common::colorOff>};

//! @brief Class for log.
class Log final : public utility::fsm::FSM<Log>
{
public:
    //! @brief Destroy the Log object.
    virtual ~Log() = default;
    //! @brief Construct a new Log object.
    Log(const Log&) = delete;
    //! @brief The operator (=) overloading of Log class.
    //! @return reference of Log object
    Log& operator=(const Log&) = delete;

    friend class FSM<Log>;
    //! @brief Enum for enumerating specific output types.
    enum class OutputType
    {
        add,
        over
    };
    //! @brief Enum for enumerating specific output levels.
    enum class OutputLevel
    {
        debug,
        info,
        warn,
        error
    };
    //! @brief Enum for enumerating specific output targets.
    enum class OutputTarget
    {
        file,
        terminal,
        all
    };
    //! @brief Enum for enumerating specific states for FSM.
    enum State
    {
        init,
        idle,
        work,
        done
    };

    //! @brief Get the Log instance.
    //! @return reference of Log object
    static inline Log& getInstance();
    //! @brief Flush log to queue.
    //! @tparam Args type of arguments of format
    //! @param level output level
    //! @param codeFile current code file
    //! @param codeLine current code line
    //! @param format log format to be flushed
    //! @param args arguments of format
    template <typename... Args>
    void flush(
        const OutputLevel level,
        const std::string& codeFile,
        const uint32_t codeLine,
        const char* const __restrict format,
        Args&&... args);
    //! @brief Interface for running logger.
    void runLogger();
    //! @brief Wait until the logger starts. External use.
    void waitStartForExternalUse();
    //! @brief Wait until the logger stops. External use.
    void waitStopForExternalUse();

private:
    //! @brief Construct a new Log object.
    //! @param initState initialization value of state
    explicit Log(const StateType initState = State::init) noexcept;
    //! @brief Construct a new Log object.
    //! @param logFile log file
    //! @param type output type
    //! @param level output level
    //! @param target output target
    //! @param initState initialization value of state
    Log(const std::string& logFile,
        const OutputType type,
        const OutputLevel level,
        const OutputTarget target,
        const StateType initState = State::init) noexcept;

    //! @brief The queue of logs.
    std::queue<std::string> logQueue;
    //! @brief Mutex for controlling queue.
    mutable std::mutex queueMutex;
    //! @brief The synchronization condition for queue. Use with queueMutex.
    std::condition_variable condition;
    //! @brief Flag to indicate whether it is logging.
    std::atomic<bool> isLogging{false};
    //! @brief Output file stream.
    std::ofstream ofs;
    //! @brief Write type.
    OutputType writeType{OutputType::add};
    //! @brief Minimum level.
    OutputLevel minLevel{OutputLevel::debug};
    //! @brief Actual target.
    OutputTarget actualTarget{OutputTarget::all};
    //! @brief Log file path.
    char pathname[logPathLength + 1]{"./temporary/foo.log"};

    //! @brief Struct for FSM event. Open file.
    struct OpenFile
    {
    };
    //! @brief Struct for FSM event. Close file.
    struct CloseFile
    {
    };
    //! @brief Struct for FSM event. Go logging.
    struct GoLogging
    {
    };
    //! @brief Struct for FSM event. No logging.
    struct NoLogging
    {
    };

    //! @brief Open the log file.
    void openLogFile();
    //! @brief Start logging.
    void startLogging();
    //! @brief Close the log file.
    void closeLogFile();
    //! @brief Stop logging.
    void stopLogging();

    //! @brief Check whether the log file is opened.
    bool isLogFileOpen(const GoLogging& /*unused*/) const;
    //! @brief Check whether the log file is closed.
    bool isLogFileClose(const NoLogging& /*unused*/) const;
    // clang-format off
    //! @brief Typedef for the transition map of the logger.
    using TransitionMap = Map<
        // --- Source ---+--- Event ---+--- Target ---+------ Action ------+--- Guard(Optional) ---
        // --------------+-------------+--------------+--------------------+-----------------------
        Row< State::init ,  OpenFile   , State::idle  , &Log::openLogFile                         >,
        Row< State::idle ,  GoLogging  , State::work  , &Log::startLogging , &Log::isLogFileOpen  >,
        Row< State::work ,  CloseFile  , State::idle  , &Log::closeLogFile                        >,
        Row< State::idle ,  NoLogging  , State::done  , &Log::stopLogging  , &Log::isLogFileClose >
        // --------------+-------------+--------------+--------------------+-----------------------
        >;
    // clang-format on

protected:
    friend std::ostream& operator<<(std::ostream& os, const Log::State& state);
};

inline Log& Log::getInstance()
{
    static Log logger;
    return logger;
}

template <typename... Args>
void Log::flush(
    const OutputLevel level,
    const std::string& codeFile,
    const uint32_t codeLine,
    const char* const __restrict format,
    Args&&... args)
{
    if (State::work != currentState())
    {
        return;
    }

    if (std::unique_lock<std::mutex> lock(queueMutex); true)
    {
        if (level >= minLevel)
        {
            std::string_view prefix;
            switch (level)
            {
                case OutputLevel::debug:
                    prefix = debugLevelPrefix;
                    break;
                case OutputLevel::info:
                    prefix = infoLevelPrefix;
                    break;
                case OutputLevel::warn:
                    prefix = warnLevelPrefix;
                    break;
                case OutputLevel::error:
                    prefix = errorLevelPrefix;
                    break;
                default:
                    break;
            }

            std::string output = std::string{prefix} + ":[" + utility::time::getCurrentSystemTime() + "]:["
                + codeFile.substr(codeFile.find("/") + 1, codeFile.length()) + "#" + std::to_string(codeLine) + "]: ";
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
            output.append(COMMON_FORMAT_TO_STRING(format, std::forward<Args>(args)...));
#pragma GCC diagnostic pop
            logQueue.push(std::move(output));

            lock.unlock();
            condition.notify_one();
            utility::time::millisecondLevelSleep(1);
            lock.lock();
        }
    }
}

extern std::string& changeToLogStyle(std::string& line);
} // namespace utility::log
