//! @file log.hpp
//! @author ryftchen
//! @brief The declarations (log) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#pragma once

#include "config.hpp"

#ifndef __PRECOMPILED_HEADER
#include <format>
#include <iostream>
#include <queue>
#include <sstream>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

#include "utility/include/common.hpp"
#include "utility/include/fsm.hpp"
#include "utility/include/io.hpp"
#include "utility/include/time.hpp"

//! @brief Log with debug level.
#define LOG_DBG                            \
    if (CONFIG_ACTIVATE_HELPER) [[likely]] \
    application::log::Log::Holder<application::log::Log::OutputLevel::debug>(__FILE__, __LINE__).getStream()
//! @brief Log with debug level (formatted).
#define LOG_DBG_F(fmt, ...)                     \
    if (CONFIG_ACTIVATE_HELPER) [[likely]]      \
    application::log::Log::getInstance().flush( \
        application::log::Log::OutputLevel::debug, __FILE__, __LINE__, fmt __VA_OPT__(, ) __VA_ARGS__)
//! @brief Log with info level.
#define LOG_INF                            \
    if (CONFIG_ACTIVATE_HELPER) [[likely]] \
    application::log::Log::Holder<application::log::Log::OutputLevel::info>(__FILE__, __LINE__).getStream()
//! @brief Log with info level (formatted).
#define LOG_INF_F(fmt, ...)                     \
    if (CONFIG_ACTIVATE_HELPER) [[likely]]      \
    application::log::Log::getInstance().flush( \
        application::log::Log::OutputLevel::info, __FILE__, __LINE__, fmt __VA_OPT__(, ) __VA_ARGS__)
//! @brief Log with warning level.
#define LOG_WRN                            \
    if (CONFIG_ACTIVATE_HELPER) [[likely]] \
    application::log::Log::Holder<application::log::Log::OutputLevel::warning>(__FILE__, __LINE__).getStream()
//! @brief Log with warning level (formatted).
#define LOG_WRN_F(fmt, ...)                     \
    if (CONFIG_ACTIVATE_HELPER) [[likely]]      \
    application::log::Log::getInstance().flush( \
        application::log::Log::OutputLevel::warning, __FILE__, __LINE__, fmt __VA_OPT__(, ) __VA_ARGS__)
//! @brief Log with error level.
#define LOG_ERR                            \
    if (CONFIG_ACTIVATE_HELPER) [[likely]] \
    application::log::Log::Holder<application::log::Log::OutputLevel::error>(__FILE__, __LINE__).getStream()
//! @brief Log with error level (formatted).
#define LOG_ERR_F(fmt, ...)                     \
    if (CONFIG_ACTIVATE_HELPER) [[likely]]      \
    application::log::Log::getInstance().flush( \
        application::log::Log::OutputLevel::error, __FILE__, __LINE__, fmt __VA_OPT__(, ) __VA_ARGS__)
//! @brief Log file path.
#define LOG_FILE_PATH application::log::Log::getInstance().loggerFilePath()
//! @brief Log file lock.
#define LOG_FILE_LOCK application::log::Log::getInstance().loggerFileLock()

//! @brief The application module.
namespace application // NOLINT (modernize-concat-nested-namespaces)
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
//! @brief Prefix of unknown level in log.
constexpr std::string_view unknownLevelPrefix = "[UNK]";
//! @brief Regular expression of debug level in log.
constexpr std::string_view debugLevelPrefixRegex = R"(^\[DBG\])";
//! @brief Regular expression of info level in log.
constexpr std::string_view infoLevelPrefixRegex = R"(^\[INF\])";
//! @brief Regular expression of warning level in log.
constexpr std::string_view warnLevelPrefixRegex = R"(^\[WRN\])";
//! @brief Regular expression of error level in log.
constexpr std::string_view errorLevelPrefixRegex = R"(^\[ERR\])";
//! @brief Regular expression of unknown level in log.
constexpr std::string_view unknownLevelPrefixRegex = R"(^\[UNK\])";
//! @brief Regular expression of date time in log.
constexpr std::string_view dateTimeRegex = R"(\[(\d{4})-(\d{2})-(\d{2}) (\d{2}):(\d{2}):(\d{2})\.(\d{6}) (\w{3})\])";
//! @brief Regular expression of code file in log.
constexpr std::string_view codeFileRegex = R"(\[[^:]+\.(c|h|cc|hh|cpp|hpp|tpp|cxx|hxx|C|H)#\d+\])";
//! @brief Debug level prefix with color. Include ANSI escape codes.
constinit const auto debugLevelPrefixWithColor = utility::common::joinString<
    utility::common::colorBlue,
    utility::common::colorBold,
    utility::common::colorForBackground,
    debugLevelPrefix,
    utility::common::colorOff>;
//! @brief Info level prefix with color. Include ANSI escape codes.
constinit const auto infoLevelPrefixWithColor = utility::common::joinString<
    utility::common::colorGreen,
    utility::common::colorBold,
    utility::common::colorForBackground,
    infoLevelPrefix,
    utility::common::colorOff>;
//! @brief Warning level prefix with color. Include ANSI escape codes.
constinit const auto warningLevelPrefixWithColor = utility::common::joinString<
    utility::common::colorYellow,
    utility::common::colorBold,
    utility::common::colorForBackground,
    warningLevelPrefix,
    utility::common::colorOff>;
//! @brief Error level prefix with color. Include ANSI escape codes.
constinit const auto errorLevelPrefixWithColor = utility::common::joinString<
    utility::common::colorRed,
    utility::common::colorBold,
    utility::common::colorForBackground,
    errorLevelPrefix,
    utility::common::colorOff>;
//! @brief Unknown level prefix with color. Include ANSI escape codes.
constinit const auto unknownLevelPrefixWithColor = utility::common::joinString<
    utility::common::colorInverse,
    utility::common::colorBold,
    utility::common::colorForBackground,
    unknownLevelPrefix,
    utility::common::colorOff>;
//! @brief Base color of the date time. Include ANSI escape codes.
constinit const auto dateTimeBaseColor = utility::common::joinString<
    utility::common::colorForForeground,
    utility::common::colorBold,
    utility::common::colorDim,
    utility::common::colorForBackground>;
//! @brief Base color of the code file. Include ANSI escape codes.
constinit const auto codeFileBaseColor = utility::common::joinString<
    utility::common::colorForForeground,
    utility::common::colorBold,
    utility::common::colorUnderline,
    utility::common::colorForBackground>;

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
    //! @brief Enumerate specific output types.
    enum class OutputType : std::uint8_t
    {
        //! @brief Add.
        add,
        //! @brief Over.
        over
    };
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
    //! @brief Enumerate specific output medium.
    enum class OutputMedium : std::uint8_t
    {
        //! @brief File.
        file,
        //! @brief Terminal.
        terminal,
        //! @brief Both.
        both
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
    //! @brief State controller for running logger.
    void stateController();
    //! @brief Wait for the logger to start. Interface controller for external use.
    void waitForStart();
    //! @brief Wait for the logger to stop. Interface controller for external use.
    void waitForStop();
    //! @brief Request to reset the logger. Interface controller for external use.
    void requestToReset();
    //! @brief Get the log file path.
    //! @return log file path
    std::string loggerFilePath() const;
    //! @brief Get the log file lock.
    //! @return log file lock
    utility::common::ReadWriteLock& loggerFileLock();

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
        const std::uint32_t codeLine,
        const std::string& format,
        Args&&... args);
    //! @brief Log holder for flushing.
    //! @tparam Lv - type of output level
    template <OutputLevel Lv>
    class Holder
    {
    public:
        //! @brief Construct a new Holder object.
        //! @param codeFile - current code file
        //! @param codeLine - current code line
        Holder(const std::string& codeFile, const std::uint32_t codeLine) : file(codeFile), line(codeLine) {}
        //! @brief Destroy the Holder object.
        virtual ~Holder() { flush(); }

        //! @brief Get the output stream for flushing.
        //! @return reference of the output stream object, which is on string based
        inline std::ostringstream& getStream() { return stream; }

    private:
        //! @brief Output stream for flushing.
        std::ostringstream stream{};
        //! @brief Code file.
        const std::string& file{};
        //! @brief Code line.
        const std::uint32_t line{};

        //! @brief Flush the output stream.
        inline void flush() { getInstance().flush(Lv, file, line, stream.str()); }
    };

private:
    //! @brief Construct a new Log object.
    //! @param initState - initialization value of state
    explicit Log(const StateType initState = State::init) noexcept :
        FSM(initState),
        filePath(getFullLogPath(CONFIG_HELPER_LOGGER_FILE_PATH)),
        writeType(OutputType(CONFIG_HELPER_LOGGER_WRITE_TYPE)),
        minimumLevel(OutputLevel(CONFIG_HELPER_LOGGER_MINIMUM_LEVEL)),
        usedMedium(OutputMedium(CONFIG_HELPER_LOGGER_USED_MEDIUM))
    {
    }

    //! @brief Timeout period (ms) to waiting for the logger to change to the target state.
    const std::uint32_t timeoutPeriod{static_cast<std::uint32_t>(CONFIG_HELPER_TIMEOUT)};
    //! @brief The queue of logs.
    std::queue<std::string> logQueue{};
    //! @brief Mutex for controlling daemon.
    mutable std::mutex daemonMtx{};
    //! @brief The synchronization condition for daemon. Use with daemonMtx.
    std::condition_variable daemonCv{};
    //! @brief Flag to indicate whether it is logging.
    std::atomic<bool> ongoing{false};
    //! @brief Flag for rollback request.
    std::atomic<bool> toReset{false};
    //! @brief Log file absolute path.
    const std::string filePath{getFullLogPath()};
    //! @brief Write type.
    const OutputType writeType{OutputType::add};
    //! @brief Minimum level.
    const OutputLevel minimumLevel{OutputLevel::debug};
    //! @brief Used medium.
    const OutputMedium usedMedium{OutputMedium::both};
    //! @brief Writer for the log file.
    utility::io::FileWriter logWriter{filePath};
    //! @brief Log file lock.
    utility::common::ReadWriteLock fileLock{};
    //! @brief Spin lock for controlling state.
    mutable utility::common::SpinLock stateLock{};

    //! @brief Filter break line.
    //! @param line - target line
    //! @return single row
    static std::string filterBreakLine(const std::string& line);
    //! @brief Safely retrieve the current state.
    //! @return current state
    State safeCurrentState() const;
    //! @brief Safely process an event.
    //! @tparam T - type of target event
    //! @param event - target event
    template <class T>
    void safeProcessEvent(const T& event);
    //! @brief Check whether it is in the uninterrupted target state.
    //! @param state - target state
    //! @return in the uninterrupted target state or not
    bool isInUninterruptedState(const State state) const;
    //! @brief Get the full path to the log file.
    //! @param filename - log file
    //! @return full path to the log file
    static std::string getFullLogPath(const std::string& filename = "log/foo.log");
    //! @brief Try to create the log folder.
    void tryToCreateLogFolder() const;

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
    //! @brief Alias for the transition map of the logger.
    using TransitionMap = Map
    <
        // --- Source ---+--- Event ---+--- Target ---+------ Action ------+--- Guard (Optional) ---
        // --------------+-------------+--------------+--------------------+------------------------
        Row< State::init ,  OpenFile   , State::idle  , &Log::openLogFile                         >,
        Row< State::idle ,  GoLogging  , State::work  , &Log::startLogging , &Log::isLogFileOpen  >,
        Row< State::work ,  CloseFile  , State::idle  , &Log::closeLogFile                        >,
        Row< State::idle ,  NoLogging  , State::done  , &Log::stopLogging  , &Log::isLogFileClose >,
        Row< State::init ,  Standby    , State::hold  , &Log::doToggle                            >,
        Row< State::idle ,  Standby    , State::hold  , &Log::doToggle                            >,
        Row< State::work ,  Standby    , State::hold  , &Log::doToggle                            >,
        Row< State::done ,  Standby    , State::hold  , &Log::doToggle                            >,
        Row< State::work ,  Relaunch   , State::init  , &Log::doRollback                          >,
        Row< State::hold ,  Relaunch   , State::init  , &Log::doRollback                          >
        // --------------+-------------+--------------+--------------------+------------------------
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

extern const std::string& changeToLogStyle(std::string& line);

template <typename... Args>
void Log::flush(
    const OutputLevel level,
    const std::string& codeFile,
    const std::uint32_t codeLine,
    const std::string& format,
    Args&&... args)
{
    if (level < minimumLevel)
    {
        return;
    }

    std::unique_lock<std::mutex> daemonLock(daemonMtx, std::defer_lock);
    try
    {
        std::string_view prefix = unknownLevelPrefix;
        if (isInUninterruptedState(State::work))
        {
            daemonLock.lock();
            switch (level)
            {
                case OutputLevel::debug:
                    prefix = debugLevelPrefix;
                    break;
                case OutputLevel::info:
                    prefix = infoLevelPrefix;
                    break;
                case OutputLevel::warning:
                    prefix = warningLevelPrefix;
                    break;
                case OutputLevel::error:
                    prefix = errorLevelPrefix;
                    break;
                default:
                    break;
            }
        }

        std::string output = std::format(
            "{}:[{}]:[{}#{}]: {}",
            prefix,
            utility::time::getCurrentSystemTime(),
            codeFile.substr(codeFile.find("foo/") + 4, codeFile.length()),
            codeLine,
            utility::common::formatString(filterBreakLine(format).data(), std::forward<Args>(args)...));
        if (daemonLock.owns_lock())
        {
            logQueue.push(std::move(output));
            daemonLock.unlock();
            daemonCv.notify_one();
        }
        else
        {
            std::cerr << changeToLogStyle(output) << std::endl;
        }
    }
    catch (...)
    {
        if (daemonLock.owns_lock())
        {
            daemonLock.unlock();
        }
        throw;
    }
}
} // namespace log
} // namespace application
