//! @file log.hpp
//! @author ryftchen
//! @brief The declarations (log) in the application module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#pragma once

#ifndef __PRECOMPILED_HEADER
#include <fstream>
#include <queue>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER
#include "utility/include/common.hpp"
#include "utility/include/file.hpp"
#include "utility/include/fsm.hpp"
#include "utility/include/time.hpp"

//! @brief Start to log.
#define LOG_TO_START application::log::Log::getInstance().interfaceToStart()
//! @brief Stop to log.
#define LOG_TO_STOP application::log::Log::getInstance().interfaceToStop()
//! @brief Log with debug level.
#define LOG_DBG(format, args...)                \
    application::log::Log::getInstance().flush( \
        application::log::Log::OutputLevel::debug, __FILE__, __LINE__, format, ##args)
//! @brief Log with info level.
#define LOG_INF(format, args...)                \
    application::log::Log::getInstance().flush( \
        application::log::Log::OutputLevel::info, __FILE__, __LINE__, format, ##args)
//! @brief Log with warning level.
#define LOG_WRN(format, args...)                \
    application::log::Log::getInstance().flush( \
        application::log::Log::OutputLevel::warn, __FILE__, __LINE__, format, ##args)
//! @brief Log with error level.
#define LOG_ERR(format, args...)                \
    application::log::Log::getInstance().flush( \
        application::log::Log::OutputLevel::error, __FILE__, __LINE__, format, ##args)
//! @brief Log file path.
#define LOG_PATHNAME application::log::Log::getInstance().getPathname()
//! @brief Log file lock.
#define LOG_FILE_LOCK application::log::Log::getInstance().getFileLock()

//! @brief Log-related functions in the application module.
namespace application::log
{
//! @brief Length of the log file path.
constexpr uint16_t logPathLength = 32;
//! @brief The maximum number of times to wait for the logger to change to the target state.
constexpr uint16_t maxTimesOfWaitLogger = 10;
//! @brief The time interval (ms) to wait for the logger to change to the target state.
constexpr uint16_t intervalOfWaitLogger = 5;
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

//! @brief Logger.
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
    //! @brief Enumerate specific output types.
    enum class OutputType : uint8_t
    {
        add,
        over
    };
    //! @brief Enumerate specific output levels.
    enum class OutputLevel : uint8_t
    {
        debug,
        info,
        warn,
        error
    };
    //! @brief Enumerate specific output targets.
    enum class OutputTarget : uint8_t
    {
        file,
        terminal,
        all
    };
    //! @brief Enumerate specific states for FSM.
    enum State : uint8_t
    {
        init,
        idle,
        work,
        done
    };

    //! @brief Get the Log instance.
    //! @return reference of Log object
    static Log& getInstance();
    //! @brief Flush log to queue.
    //! @tparam Args - type of arguments of format
    //! @param level - output level
    //! @param codeFile - current code file
    //! @param codeLine - current code line
    //! @param format - log format to be flushed
    //! @param args - arguments of format
    template <typename... Args>
    void flush(
        const OutputLevel level,
        const std::string& codeFile,
        const uint32_t codeLine,
        const char* const format,
        Args&&... args);
    //! @brief Interface for running logger.
    void runLogger();
    //! @brief Wait until the logger starts. External use.
    void interfaceToStart();
    //! @brief Wait until the logger stops. External use.
    void interfaceToStop();
    //! @brief Get log file path.
    //! @return log file path
    inline std::string getPathname() const;
    //! @brief Get log file lock.
    //! @return log file lock
    inline utility::file::FileReadWriteLock& getFileLock();

private:
    //! @brief Construct a new Log object.
    //! @param initState - initialization value of state
    explicit Log(const StateType initState = State::init) noexcept : FSM(initState){};
    //! @brief Construct a new Log object.
    //! @param logFile - log file
    //! @param type - output type
    //! @param level - output level
    //! @param target - output target
    //! @param initState - initialization value of state
    Log(const std::string& logFile,
        const OutputType type,
        const OutputLevel level,
        const OutputTarget target,
        const StateType initState = State::init) noexcept;

    //! @brief The queue of logs.
    std::queue<std::string> logQueue;
    //! @brief Mutex for controlling queue.
    mutable std::mutex mtx;
    //! @brief The synchronization condition for queue. Use with mtx.
    std::condition_variable cv;
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
    //! @brief Log file lock.
    utility::file::FileReadWriteLock fileLock;

    //! @brief FSM event. Open file.
    struct OpenFile
    {
    };
    //! @brief FSM event. Close file.
    struct CloseFile
    {
    };
    //! @brief FSM event. Go logging.
    struct GoLogging
    {
    };
    //! @brief FSM event. No logging.
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
    //! @brief Alias for the transition map of the logger.
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

template <typename... Args>
void Log::flush(
    const OutputLevel level,
    const std::string& codeFile,
    const uint32_t codeLine,
    const char* const format,
    Args&&... args)
{
    if (State::work != currentState())
    {
        return;
    }

    if (std::unique_lock<std::mutex> lock(mtx); true)
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
            cv.notify_one();
            utility::time::millisecondLevelSleep(1);
            lock.lock();
        }
    }
}

inline std::string Log::getPathname() const
{
    return pathname;
}

inline utility::file::FileReadWriteLock& Log::getFileLock()
{
    return fileLock;
}

extern const std::string& changeToLogStyle(std::string& line);
} // namespace application::log
