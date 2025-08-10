//! @file log.hpp
//! @author ryftchen
//! @brief The declarations (log) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include "configure.hpp"

#ifndef _PRECOMPILED_HEADER
#include <forward_list>
#include <iostream>
#include <source_location>
#include <sstream>
#else
#include "application/pch/precompiled_header.hpp"
#endif // _PRECOMPILED_HEADER

#include "utility/include/common.hpp"
#include "utility/include/fsm.hpp"
#include "utility/include/io.hpp"

//! @brief Log with debug level.
#define LOG_DBG application::log::Holder<application::log::Log::OutputLevel::debug>().stream()
//! @brief Log with debug level (printf style).
#define LOG_DBG_P(fmt, ...)        \
    application::log::printfStyle( \
        application::log::Log::OutputLevel::debug, __FILE__, __LINE__, fmt __VA_OPT__(, ) __VA_ARGS__)
//! @brief Log with debug level (format style).
#define LOG_DBG_F(fmt, ...)        \
    application::log::formatStyle( \
        application::log::Log::OutputLevel::debug, __FILE__, __LINE__, fmt __VA_OPT__(, ) __VA_ARGS__)
//! @brief Log with info level.
#define LOG_INF application::log::Holder<application::log::Log::OutputLevel::info>().stream()
//! @brief Log with info level (printf style).
#define LOG_INF_P(fmt, ...)        \
    application::log::printfStyle( \
        application::log::Log::OutputLevel::info, __FILE__, __LINE__, fmt __VA_OPT__(, ) __VA_ARGS__)
//! @brief Log with info level (format style).
#define LOG_INF_F(fmt, ...)        \
    application::log::formatStyle( \
        application::log::Log::OutputLevel::info, __FILE__, __LINE__, fmt __VA_OPT__(, ) __VA_ARGS__)
//! @brief Log with warning level.
#define LOG_WRN application::log::Holder<application::log::Log::OutputLevel::warning>().stream()
//! @brief Log with warning level (printf style).
#define LOG_WRN_P(fmt, ...)        \
    application::log::printfStyle( \
        application::log::Log::OutputLevel::warning, __FILE__, __LINE__, fmt __VA_OPT__(, ) __VA_ARGS__)
//! @brief Log with warning level (format style).
#define LOG_WRN_F(fmt, ...)        \
    application::log::formatStyle( \
        application::log::Log::OutputLevel::warning, __FILE__, __LINE__, fmt __VA_OPT__(, ) __VA_ARGS__)
//! @brief Log with error level.
#define LOG_ERR application::log::Holder<application::log::Log::OutputLevel::error>().stream()
//! @brief Log with error level (printf style).
#define LOG_ERR_P(fmt, ...)        \
    application::log::printfStyle( \
        application::log::Log::OutputLevel::error, __FILE__, __LINE__, fmt __VA_OPT__(, ) __VA_ARGS__)
//! @brief Log with error level (format style).
#define LOG_ERR_F(fmt, ...)        \
    application::log::formatStyle( \
        application::log::Log::OutputLevel::error, __FILE__, __LINE__, fmt __VA_OPT__(, ) __VA_ARGS__)

//! @brief The application module.
namespace application // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Log-related functions in the application module.
namespace log
{
//! @brief Directory of the source code.
constexpr std::string_view sourceDirectory = "/foo/";

//! @brief Logger.
class Log final : public utility::fsm::FSM<Log>
{
public:
    friend class FSM<Log>;
    //! @brief Destroy the Log object.
    ~Log() override = default;
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

    //! @brief Instance name.
    static constexpr std::string name{configure::field::logger};
    //! @brief Get the Log instance.
    //! @return reference of the Log object
    static Log& getInstance();
    //! @brief Service for running.
    void service();

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
    //! @brief Access for the instance.
    class Access
    {
    public:
        //! @brief Wait for the logger to start. Interface controller for external use.
        void startup() const;
        //! @brief Wait for the logger to stop. Interface controller for external use.
        void shutdown() const;
        //! @brief Request to reset the logger. Interface controller for external use.
        void reload() const;

        //! @brief Preview the log.
        //! @param peeking - further handling for peeking
        void onPreviewing(const std::function<void(const std::string&)>& peeking) const;

    private:
        //! @brief Instance to be accessed.
        Log& inst{getInstance()};

        //! @brief Wait until the logger reaches the target state.
        //! @param state - target state
        //! @param handling - handling if unexpected state
        void waitOr(const State state, const std::function<void()>& handling) const;
        //! @brief Notify the logger daemon to change the state.
        //! @param action - action to be executed
        void notifyVia(const std::function<void()>& action) const;
        //! @brief Start the reset timer.
        void startResetTimer() const;
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
    template <typename... Args>
    friend void printfStyle(
        const OutputLevel severity,
        const std::string_view srcFile,
        const std::uint32_t srcLine,
        const std::string& format,
        Args&&... args);
    template <typename... Args>
    friend void formatStyle(
        const OutputLevel severity,
        const std::string_view srcFile,
        const std::uint32_t srcLine,
        const std::string& format,
        Args&&... args);
    static_assert((sourceDirectory.front() == '/') && (sourceDirectory.back() == '/'));

private:
    //! @brief Construct a new Log object.
    //! @param filePath -full path to the log file
    //! @param priorityLevel - priority level
    //! @param targetType - target type
    //! @param writeMode - write mode
    explicit Log(
        const std::string_view filePath = getFullLogPath(configure::detail::filePath4Logger()),
        const OutputLevel priorityLevel = static_cast<OutputLevel>(configure::detail::priorityLevel4Logger()),
        const OutputType targetType = static_cast<OutputType>(configure::detail::targetType4Logger()),
        const OutputMode writeMode = static_cast<OutputMode>(configure::detail::writeMode4Logger())) :
        FSM(State::init), filePath{filePath}, priorityLevel{priorityLevel}, targetType{targetType}, writeMode{writeMode}
    {
    }

    //! @brief Full path to the log file.
    const std::string filePath{getFullLogPath()};
    //! @brief Priority level.
    const OutputLevel priorityLevel{OutputLevel::debug};
    //! @brief Target type.
    const OutputType targetType{OutputType::all};
    //! @brief Write mode.
    const OutputMode writeMode{OutputMode::append};
    //! @brief Timeout period (ms) to waiting for the logger to change to the target state.
    const std::uint32_t timeoutPeriod{static_cast<std::uint32_t>(configure::detail::helperTimeout())};
    //! @brief The queue of logs.
    std::queue<std::string> logQueue;
    //! @brief Mutex for controlling daemon.
    mutable std::mutex daemonMtx;
    //! @brief The synchronization condition for daemon. Use with daemonMtx.
    std::condition_variable daemonCond;
    //! @brief Flag to indicate whether it is logging.
    std::atomic_bool ongoing{false};
    //! @brief Flag for rollback request.
    std::atomic_bool toReset{false};
    //! @brief Writer of the log content.
    utility::io::FileWriter logWriter{filePath};
    //! @brief Operation lock for the log file.
    utility::common::ReadWriteLock fileLock;
    //! @brief The cache logs that could not be processed properly.
    std::forward_list<std::string> unprocessedCache;
    //! @brief Spin lock for controlling cache.
    mutable utility::common::SpinLock cacheSwitch;

    //! @brief Alias for the lock mode.
    using LockMode = utility::common::ReadWriteLock::LockMode;
    //! @brief Flush log to queue.
    //! @param severity - level of severity
    //! @param labelTpl - label template
    //! @param formatted - formatted body
    void flush(const OutputLevel severity, const std::string_view labelTpl, const std::string_view formatted);
    //! @brief Create the dynamic label template.
    //! @param srcFile - current code file
    //! @param srcLine - current code line
    //! @return dynamic label template
    static std::string createLabelTemplate(const std::string_view srcFile, const std::uint32_t srcLine);
    //! @brief Get the prefix corresponding to the level.
    //! @param level - output level
    //! @return output prefix
    static std::string_view getPrefix(const OutputLevel level);
    //! @brief Reformat log contents.
    //! @param label - label information
    //! @param formatted - formatted body
    //! @return log contents
    static std::vector<std::string> reformatContents(const std::string_view label, const std::string_view formatted);

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

//! @brief Log output for legacy (printf style).
//! @tparam Args - type of arguments of log format
//! @param severity - level of severity
//! @param srcFile - current code file
//! @param srcLine - current code line
//! @param format - log format to be flushed
//! @param args - arguments of log format
template <typename... Args>
void printfStyle(
    const Log::OutputLevel severity,
    const std::string_view srcFile,
    const std::uint32_t srcLine,
    const std::string& format,
    Args&&... args)
{
    if (configure::detail::activateHelper())
    {
        Log::getInstance().flush(
            severity,
            Log::createLabelTemplate(srcFile, srcLine),
            utility::common::printfString(format.c_str(), std::forward<Args>(args)...));
        return;
    }

    const auto rows = Log::reformatContents(
        std::string{sourceDirectory.substr(1, sourceDirectory.length() - 2)} + ": ",
        utility::common::printfString(format.c_str(), std::forward<Args>(args)...));
    std::for_each(rows.cbegin(), rows.cend(), [](const auto& output) { std::clog << output << std::endl; });
}

//! @brief Log output for modern (format style).
//! @tparam Args - type of arguments of log format
//! @param severity - level of severity
//! @param srcFile - current code file
//! @param srcLine - current code line
//! @param format - log format to be flushed
//! @param args - arguments of log format
template <typename... Args>
void formatStyle(
    const Log::OutputLevel severity,
    const std::string_view srcFile,
    const std::uint32_t srcLine,
    const std::string& format,
    Args&&... args)
{
    if (configure::detail::activateHelper())
    {
        Log::getInstance().flush(
            severity,
            Log::createLabelTemplate(srcFile, srcLine),
            utility::common::formatString(format, std::forward<Args>(args)...));
        return;
    }

    const auto rows = Log::reformatContents(
        std::string{sourceDirectory.substr(1, sourceDirectory.length() - 2)} + ": ",
        utility::common::formatString(format, std::forward<Args>(args)...));
    std::for_each(rows.cbegin(), rows.cend(), [](const auto& output) { std::clog << output << std::endl; });
}

//! @brief Log holder for flushing.
//! @tparam Lv - output level
template <Log::OutputLevel Lv>
class Holder
{
public:
    //! @brief Construct a new Holder object.
    //! @param srcLoc - current source location
    explicit Holder(const std::source_location& srcLoc = std::source_location::current()) : location{srcLoc} {}
    //! @brief Destroy the Holder object.
    virtual ~Holder() { printfStyle(Lv, location.file_name(), location.line(), buffer.str()); }
    //! @brief Construct a new Holder object.
    Holder(const Holder&) = default;
    //! @brief Construct a new Holder object.
    Holder(Holder&&) noexcept = default;
    //! @brief The operator (=) overloading of Holder class.
    //! @return reference of the Holder object
    Holder& operator=(const Holder&) = default;
    //! @brief The operator (=) overloading of Holder class.
    //! @return reference of the Holder object
    Holder& operator=(Holder&&) noexcept = default;

    //! @brief Get the buffer stream for flushing.
    //! @return reference of the buffer stream object, which is on string based
    std::ostringstream& stream() noexcept { return buffer; }

private:
    //! @brief Output stream for flushing.
    std::ostringstream buffer;
    //! @brief Source location.
    const std::source_location location;
};
extern template class Holder<Log::OutputLevel::debug>;
extern template class Holder<Log::OutputLevel::info>;
extern template class Holder<Log::OutputLevel::warning>;
extern template class Holder<Log::OutputLevel::error>;

extern std::string& changeToLogStyle(std::string& line);
} // namespace log
} // namespace application
