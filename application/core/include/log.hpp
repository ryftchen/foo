//! @file log.hpp
//! @author ryftchen
//! @brief The declarations (log) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include "configure.hpp"

#ifndef __PRECOMPILED_HEADER
#include <format>
#include <forward_list>
#include <iostream>
#include <source_location>
#include <sstream>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

#include "utility/include/common.hpp"
#include "utility/include/fsm.hpp"
#include "utility/include/io.hpp"
#include "utility/include/time.hpp"

//! @brief Log with debug level.
#define LOG_DBG application::log::Log::Holder<application::log::Log::OutputLevel::debug>().stream()
//! @brief Log with debug level (printf style).
#define LOG_DBG_P(fmt, ...)             \
    application::log::Log::printfStyle( \
        application::log::Log::OutputLevel::debug, __FILE__, __LINE__, fmt __VA_OPT__(, ) __VA_ARGS__)
//! @brief Log with debug level (format style).
#define LOG_DBG_F(fmt, ...)             \
    application::log::Log::formatStyle( \
        application::log::Log::OutputLevel::debug, __FILE__, __LINE__, fmt __VA_OPT__(, ) __VA_ARGS__)
//! @brief Log with info level.
#define LOG_INF application::log::Log::Holder<application::log::Log::OutputLevel::info>().stream()
//! @brief Log with info level (printf style).
#define LOG_INF_P(fmt, ...)             \
    application::log::Log::printfStyle( \
        application::log::Log::OutputLevel::info, __FILE__, __LINE__, fmt __VA_OPT__(, ) __VA_ARGS__)
//! @brief Log with info level (format style).
#define LOG_INF_F(fmt, ...)             \
    application::log::Log::formatStyle( \
        application::log::Log::OutputLevel::info, __FILE__, __LINE__, fmt __VA_OPT__(, ) __VA_ARGS__)
//! @brief Log with warning level.
#define LOG_WRN application::log::Log::Holder<application::log::Log::OutputLevel::warning>().stream()
//! @brief Log with warning level (printf style).
#define LOG_WRN_P(fmt, ...)             \
    application::log::Log::printfStyle( \
        application::log::Log::OutputLevel::warning, __FILE__, __LINE__, fmt __VA_OPT__(, ) __VA_ARGS__)
//! @brief Log with warning level (format style).
#define LOG_WRN_F(fmt, ...)             \
    application::log::Log::formatStyle( \
        application::log::Log::OutputLevel::warning, __FILE__, __LINE__, fmt __VA_OPT__(, ) __VA_ARGS__)
//! @brief Log with error level.
#define LOG_ERR application::log::Log::Holder<application::log::Log::OutputLevel::error>().stream()
//! @brief Log with error level (printf style).
#define LOG_ERR_P(fmt, ...)             \
    application::log::Log::printfStyle( \
        application::log::Log::OutputLevel::error, __FILE__, __LINE__, fmt __VA_OPT__(, ) __VA_ARGS__)
//! @brief Log with error level (format style).
#define LOG_ERR_F(fmt, ...)             \
    application::log::Log::formatStyle( \
        application::log::Log::OutputLevel::error, __FILE__, __LINE__, fmt __VA_OPT__(, ) __VA_ARGS__)

//! @brief The application module.
namespace application // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Log-related functions in the application module.
namespace log
{
//! @brief Prefix of debug level in log.
constexpr std::string_view debugLevelPrefix = "[DBG]";
//! @brief Prefix of info level in log.
constexpr std::string_view infoLevelPrefix = "[INF]";
//! @brief Prefix of warning level in log.
constexpr std::string_view warningLevelPrefix = "[WRN]";
//! @brief Prefix of error level in log.
constexpr std::string_view errorLevelPrefix = "[ERR]";
//! @brief Prefix of trace level in log.
constexpr std::string_view traceLevelPrefix = "[TRC]";
//! @brief Regular expression of debug level in log.
constexpr std::string_view debugLevelPrefixRegex = R"(\[DBG\])";
//! @brief Regular expression of info level in log.
constexpr std::string_view infoLevelPrefixRegex = R"(\[INF\])";
//! @brief Regular expression of warning level in log.
constexpr std::string_view warningLevelPrefixRegex = R"(\[WRN\])";
//! @brief Regular expression of error level in log.
constexpr std::string_view errorLevelPrefixRegex = R"(\[ERR\])";
//! @brief Regular expression of trace level in log.
constexpr std::string_view traceLevelPrefixRegex = R"(\[TRC\])";
//! @brief Regular expression of date time in log.
constexpr std::string_view dateTimeRegex = R"(\[(\d{4})-(\d{2})-(\d{2}) (\d{2}):(\d{2}):(\d{2})\.(\d{6}) (\w{3})\])";
//! @brief Regular expression of code file in log.
constexpr std::string_view codeFileRegex = R"(\[[^ ]+\.(c|h|cc|hh|cpp|hpp|tpp|cxx|hxx|C|H)#\d+\])";
//! @brief Directory of the source code.
constexpr std::string_view sourceDirectory = R"(/foo/)";
//! @brief Debug level prefix with color. Include ANSI escape codes.
constexpr auto debugLevelPrefixWithColor = utility::common::concatString<
    utility::common::escColorBlue,
    utility::common::escFontBold,
    utility::common::escBgColor,
    debugLevelPrefix,
    utility::common::escOff>;
//! @brief Info level prefix with color. Include ANSI escape codes.
constexpr auto infoLevelPrefixWithColor = utility::common::concatString<
    utility::common::escColorGreen,
    utility::common::escFontBold,
    utility::common::escBgColor,
    infoLevelPrefix,
    utility::common::escOff>;
//! @brief Warning level prefix with color. Include ANSI escape codes.
constexpr auto warningLevelPrefixWithColor = utility::common::concatString<
    utility::common::escColorYellow,
    utility::common::escFontBold,
    utility::common::escBgColor,
    warningLevelPrefix,
    utility::common::escOff>;
//! @brief Error level prefix with color. Include ANSI escape codes.
constexpr auto errorLevelPrefixWithColor = utility::common::concatString<
    utility::common::escColorRed,
    utility::common::escFontBold,
    utility::common::escBgColor,
    errorLevelPrefix,
    utility::common::escOff>;
//! @brief Trace level prefix with color. Include ANSI escape codes.
constexpr auto traceLevelPrefixWithColor = utility::common::concatString<
    utility::common::escFontInverse,
    utility::common::escFontBold,
    utility::common::escBgColor,
    traceLevelPrefix,
    utility::common::escOff>;
//! @brief Base color of the date time. Include ANSI escape codes.
constexpr auto dateTimeBaseColor = utility::common::concatString<
    utility::common::escFgColor,
    utility::common::escFontBold,
    utility::common::escFontDim,
    utility::common::escBgColor>;
//! @brief Base color of the code file. Include ANSI escape codes.
constexpr auto codeFileBaseColor = utility::common::concatString<
    utility::common::escFgColor,
    utility::common::escFontBold,
    utility::common::escFontUnderline,
    utility::common::escBgColor>;
//! @brief Base color of the history cache. Include ANSI escape codes.
constexpr auto historyCacheBaseColor = utility::common::
    concatString<utility::common::escFontInverse, utility::common::escFontItalic, utility::common::escBgColor>;

//! @brief Logger.
class Log final : public utility::fsm::FSM<Log>
{
public:
    //! @brief Destroy the Log object.
    virtual ~Log() = default;
    //! @brief Construct a new Log object.
    Log(const Log&) = delete;
    //! @brief Construct a new Log object.
    Log(Log&&) = delete;
    //! @brief The operator (=) overloading of Log class.
    //! @return reference of the Log object
    Log& operator=(const Log&) = delete;
    //! @brief The operator (=) overloading of Log class.
    //! @return reference of the Log object
    Log& operator=(Log&&) = delete;

    friend class FSM<Log>;
    //! @brief Enumerate specific output levels.
    enum class OutputLevel : std::uint8_t
    {
        //! @brief Debug.
        debug,
        //! @brief Info.
        info,
        //! @brief Warning.
        warning,
        //! @brief Error.
        error
    };
    //! @brief Enumerate specific output modes.
    enum class OutputMode : std::uint8_t
    {
        //! @brief Append.
        append,
        //! @brief Overwrite.
        overwrite
    };
    //! @brief Enumerate specific output types.
    enum class OutputType : std::uint8_t
    {
        //! @brief File.
        file,
        //! @brief Terminal.
        terminal,
        //! @brief All.
        all
    };
    //! @brief Enumerate specific states for FSM.
    enum State : std::uint8_t
    {
        //! @brief Init.
        init,
        //! @brief Idle.
        idle,
        //! @brief Work.
        work,
        //! @brief Done.
        done,
        //! @brief Hold.
        hold
    };

    //! @brief Get the Log instance.
    //! @return reference of the Log object
    static Log& getInstance();
    //! @brief Instance name.
    static constexpr std::string_view name{configure::field::logger};
    //! @brief Service for running.
    void service();

    //! @brief Access for the instance.
    class Access
    {
    public:
        //! @brief Construct a new Access object.
        Access() : inst{getInstance()} {}
        //! @brief Destroy the Access object.
        virtual ~Access() = default;

        //! @brief Wait for the logger to start. Interface controller for external use.
        void startup() const;
        //! @brief Wait for the logger to stop. Interface controller for external use.
        void shutdown() const;
        //! @brief Request to reset the logger. Interface controller for external use.
        void reload() const;

        //! @brief Get the log file path.
        //! @return log file path
        [[nodiscard]] inline std::string getFilePath() const noexcept { return inst.filePath; }
        //! @brief Get the log file lock.
        //! @return log file lock
        [[nodiscard]] inline utility::common::ReadWriteLock& getFileLock() const noexcept { return inst.fileLock; }

    private:
        //! @brief Instance to be accessed.
        Log& inst;
    };

    //! @brief Log output for legacy (printf style).
    //! @tparam Args - type of arguments of log format
    //! @param severity - level of severity
    //! @param srcFile - current code file
    //! @param srcLine - current code line
    //! @param format - log format to be flushed
    //! @param args - arguments of log format
    template <typename... Args>
    static void printfStyle(
        const OutputLevel severity,
        const std::string_view srcFile,
        const std::uint32_t srcLine,
        const std::string_view format,
        Args&&... args);
    //! @brief Log output for modern (format style).
    //! @tparam Args - type of arguments of log format
    //! @param severity - level of severity
    //! @param srcFile - current code file
    //! @param srcLine - current code line
    //! @param format - log format to be flushed
    //! @param args - arguments of log format
    template <typename... Args>
    static void formatStyle(
        const OutputLevel severity,
        const std::string_view srcFile,
        const std::uint32_t srcLine,
        const std::format_string<Args...>& format,
        Args&&... args);
    //! @brief Log holder for flushing.
    //! @tparam Lv - output level
    template <OutputLevel Lv>
    class Holder
    {
    public:
        //! @brief Construct a new Holder object.
        //! @param srcLoc - current source location
        explicit Holder(const std::source_location& srcLoc = std::source_location::current()) : location{srcLoc} {}
        //! @brief Destroy the Holder object.
        virtual ~Holder() { printfStyle(Lv, location.file_name(), location.line(), output.str()); }

        //! @brief Get the output stream for flushing.
        //! @return reference of the output stream object, which is on string based
        inline std::ostringstream& stream() { return output; }

    private:
        //! @brief Output stream for flushing.
        std::ostringstream output{};
        //! @brief Source location.
        const std::source_location location{};
    };

    static_assert((sourceDirectory.front() == '/') && (sourceDirectory.back() == '/'));

private:
    //! @brief Construct a new Log object.
    //! @param initState - initialization value of state
    explicit Log(const StateType initState = State::init) noexcept :
        FSM(initState),
        filePath{getFullLogPath(configure::detail::filePath4Logger())},
        priorityLevel{OutputLevel(configure::detail::priorityLevel4Logger())},
        targetType{OutputType(configure::detail::targetType4Logger())},
        writeMode{OutputMode(configure::detail::writeMode4Logger())}
    {
    }

    //! @brief Timeout period (ms) to waiting for the logger to change to the target state.
    const std::uint32_t timeoutPeriod{static_cast<std::uint32_t>(configure::detail::helperTimeout())};
    //! @brief The queue of logs.
    std::queue<std::string> logQueue{};
    //! @brief Mutex for controlling daemon.
    mutable std::mutex daemonMtx{};
    //! @brief The synchronization condition for daemon. Use with daemonMtx.
    std::condition_variable daemonCond{};
    //! @brief Flag to indicate whether it is logging.
    std::atomic<bool> ongoing{false};
    //! @brief Flag for rollback request.
    std::atomic<bool> toReset{false};
    //! @brief Full path to the log file.
    const std::string filePath{getFullLogPath()};
    //! @brief Priority level.
    const OutputLevel priorityLevel{OutputLevel::debug};
    //! @brief Target type.
    const OutputType targetType{OutputType::all};
    //! @brief Write mode.
    const OutputMode writeMode{OutputMode::append};
    //! @brief Writer of the log content.
    utility::io::FileWriter logWriter{filePath};
    //! @brief Operation lock for the log file.
    utility::common::ReadWriteLock fileLock{};
    //! @brief Spin lock for controlling state.
    mutable utility::common::SpinLock stateLock{};
    //! @brief The cache logs that could not be processed properly.
    std::forward_list<std::string> unprocessedCache{};
    //! @brief Mutex for controlling cache.
    std::recursive_mutex cacheMtx{};

    //! @brief Alias for the lock mode.
    using LockMode = utility::common::ReadWriteLock::LockMode;
    //! @brief Flush log to queue.
    //! @param severity - level of severity
    //! @param srcFile - current code file
    //! @param srcLine - current code line
    //! @param formatted - formatted body
    void flush(
        const OutputLevel severity,
        const std::string_view srcFile,
        const std::uint32_t srcLine,
        const std::string_view formatted);
    //! @brief Get the prefix corresponding to the level.
    //! @param level - output level
    //! @return output prefix
    static std::string_view getPrefix(const OutputLevel level);
    //! @brief Reformat log contents.
    //! @param label - label information
    //! @param formatted - formatted body
    //! @return log contents
    static std::vector<std::string> reformatContents(const std::string_view label, const std::string_view formatted);

    //! @brief Safely retrieve the current state.
    //! @return current state
    State safeCurrentState() const;
    //! @brief Safely process an event.
    //! @tparam T - type of target event
    //! @param event - target event
    template <typename T>
    void safeProcessEvent(const T& event);
    //! @brief Check whether it is in the uninterrupted serving state.
    //! @param state - target state
    //! @return in the uninterrupted serving state or not
    bool isInServingState(const State state) const;
    //! @brief Get the full path to the log file.
    //! @param filename - log file path
    //! @return full path to the log file
    static std::string getFullLogPath(const std::string_view filename = "log/foo.log");
    //! @brief Try to create the log folder.
    void tryCreateLogFolder() const;
    //! @brief Back up the log file if needed.
    void backUpLogFileIfNeeded() const;
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
    //! @brief FSM event. Standby.
    struct Standby
    {
    };
    //! @brief FSM event. Relaunch.
    struct Relaunch
    {
    };
    //! @brief Open the log file.
    void openLogFile();
    //! @brief Close the log file.
    void closeLogFile();
    //! @brief Start logging.
    void startLogging();
    //! @brief Stop logging.
    void stopLogging();
    //! @brief Do toggle.
    void doToggle();
    //! @brief Do rollback.
    void doRollback();
    //! @brief Check whether the log file is opened.
    //! @param event - FSM event
    //! @return whether the log file is open or not
    bool isLogFileOpen(const GoLogging& event) const;
    //! @brief Check whether the log file is closed.
    //! @param event - FSM event
    //! @return whether the log file is close or not
    bool isLogFileClose(const NoLogging& event) const;
    // clang-format off
    //! @brief Alias for the transition table of the logger.
    using TransitionTable = Table
    <
        // --- Source ---+-- Event --+--- Target ---+------ Action ------+--- Guard (Optional) ---
        // --------------+-----------+--------------+--------------------+------------------------
        Row< State::init , OpenFile  , State::idle  , &Log::openLogFile                         >,
        Row< State::idle , GoLogging , State::work  , &Log::startLogging , &Log::isLogFileOpen  >,
        Row< State::work , CloseFile , State::idle  , &Log::closeLogFile                        >,
        Row< State::idle , NoLogging , State::done  , &Log::stopLogging  , &Log::isLogFileClose >,
        Row< State::init , Standby   , State::hold  , &Log::doToggle                            >,
        Row< State::idle , Standby   , State::hold  , &Log::doToggle                            >,
        Row< State::work , Standby   , State::hold  , &Log::doToggle                            >,
        Row< State::done , Standby   , State::hold  , &Log::doToggle                            >,
        Row< State::work , Relaunch  , State::init  , &Log::doRollback                          >,
        Row< State::hold , Relaunch  , State::init  , &Log::doRollback                          >
        // --------------+-----------+--------------+--------------------+------------------------
    >;
    // clang-format on
    //! @brief Await notification to ongoing.
    void awaitNotification2Ongoing();
    //! @brief Await notification to log.
    void awaitNotification2Log();
    //! @brief Await notification to retry.
    //! @return whether retry is required or not
    bool awaitNotification2Retry();

protected:
    friend std::ostream& operator<<(std::ostream& os, const State state);
};

template <typename... Args>
void Log::printfStyle(
    const OutputLevel severity,
    const std::string_view srcFile,
    const std::uint32_t srcLine,
    const std::string_view format,
    Args&&... args)
{
    if (configure::detail::activateHelper())
    {
        getInstance().flush(
            severity, srcFile, srcLine, utility::common::formatString(format.data(), std::forward<Args>(args)...));
        return;
    }

    const auto rows = reformatContents(
        std::string{sourceDirectory.substr(1, sourceDirectory.length() - 2)} + ": ",
        utility::common::formatString(format.data(), std::forward<Args>(args)...));
    std::for_each(rows.cbegin(), rows.cend(), [](const auto& output) { std::clog << output << std::endl; });
}

template <typename... Args>
void Log::formatStyle(
    const OutputLevel severity,
    const std::string_view srcFile,
    const std::uint32_t srcLine,
    const std::format_string<Args...>& format,
    Args&&... args)
{
    if (configure::detail::activateHelper())
    {
        getInstance().flush(severity, srcFile, srcLine, std::vformat(format.get(), std::make_format_args(args...)));
        return;
    }

    const auto rows = reformatContents(
        std::string{sourceDirectory.substr(1, sourceDirectory.length() - 2)} + ": ",
        std::vformat(format.get(), std::make_format_args(args...)));
    std::for_each(rows.cbegin(), rows.cend(), [](const auto& output) { std::clog << output << std::endl; });
}

//! @brief Instance information, if enabled.
namespace info
{
//! @brief Get the current log file path.
//! @return current log file path
inline std::string loggerFilePath()
{
    return Log::Access().getFilePath();
}
//! @brief Get the current log file lock.
//! @return current log file lock
inline utility::common::ReadWriteLock& loggerFileLock()
{
    return Log::Access().getFileLock();
}
} // namespace info

extern std::string& changeToLogStyle(std::string& line);
} // namespace log
} // namespace application
