//! @file log.hpp
//! @author ryftchen
//! @brief The declarations (log) in the application module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#pragma once

#include "config.hpp"

#ifndef __PRECOMPILED_HEADER
#include <algorithm>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

#include "utility/include/common.hpp"
#include "utility/include/file.hpp"
#include "utility/include/fsm.hpp"
#include "utility/include/time.hpp"

//! @brief Flush log if enabled.
#define LOG_FLUSH_IF_ENABLED(level)      \
    if (CONFIG_ACTIVE_HELPER) [[likely]] \
    application::log::Log::Holder<application::log::Log::OutputLevel::level>(__FILE__, __LINE__).getStream()
//! @brief Log with debug level.
#define LOG_DBG LOG_FLUSH_IF_ENABLED(debug)
//! @brief Log with info level.
#define LOG_INF LOG_FLUSH_IF_ENABLED(info)
//! @brief Log with warning level.
#define LOG_WRN LOG_FLUSH_IF_ENABLED(warning)
//! @brief Log with error level.
#define LOG_ERR LOG_FLUSH_IF_ENABLED(error)
//! @brief Get the existing logger instance.
#define LOG_GET_EXISTING_INSTANCE application::log::Log::getExistingInstance()
//! @brief Log file path.
#define LOG_FILE_PATH LOG_GET_EXISTING_INSTANCE.getFilePath()
//! @brief Log file lock.
#define LOG_FILE_LOCK LOG_GET_EXISTING_INSTANCE.getFileLock()

//! @brief The application module.
namespace application // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Log-related functions in the application module.
namespace log
{
//! @brief Default log filename.
constexpr std::string_view defaultLogFile = "log/foo.log";
//! @brief Prefix of debug level in log.
constexpr std::string_view debugLevelPrefix = "[DBG]";
//! @brief Prefix of info level in log.
constexpr std::string_view infoLevelPrefix = "[INF]";
//! @brief Prefix of warning level in log.
constexpr std::string_view warnLevelPrefix = "[WRN]";
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
//! @brief Regular expression of date in log.
constexpr std::string_view dateRegex = R"(\[(\d{4})-(\d{2})-(\d{2}) (\d{2}):(\d{2}):(\d{2})\.(\d{6}) (\w{3})\])";
//! @brief Regular expression of code file in log.
constexpr std::string_view codeFileRegex = R"(\[[^:]+\.(c|h|cc|hh|cpp|hpp|tpp|cxx|hxx|C|H)+#\d+\])";
//! @brief Prefix of debug level. Include ANSI escape codes.
constinit const auto debugLevelPrefixColorForLog = utility::common::joinString<
    utility::common::colorBlue,
    utility::common::colorBold,
    utility::common::colorForBackground,
    debugLevelPrefix,
    utility::common::colorOff>;
//! @brief Prefix of info level. Include ANSI escape codes.
constinit const auto infoLevelPrefixColorForLog = utility::common::joinString<
    utility::common::colorGreen,
    utility::common::colorBold,
    utility::common::colorForBackground,
    infoLevelPrefix,
    utility::common::colorOff>;
//! @brief Prefix of warning level. Include ANSI escape codes.
constinit const auto warnLevelPrefixColorForLog = utility::common::joinString<
    utility::common::colorYellow,
    utility::common::colorBold,
    utility::common::colorForBackground,
    warnLevelPrefix,
    utility::common::colorOff>;
//! @brief Prefix of error level. Include ANSI escape codes.
constinit const auto errorLevelPrefixColorForLog = utility::common::joinString<
    utility::common::colorRed,
    utility::common::colorBold,
    utility::common::colorForBackground,
    errorLevelPrefix,
    utility::common::colorOff>;
//! @brief Prefix of unknown level. Include ANSI escape codes.
constinit const auto unknownLevelPrefixColorForLog = utility::common::joinString<
    utility::common::colorBold,
    utility::common::colorForBackground,
    unknownLevelPrefix,
    utility::common::colorOff>;

//! @brief Logger.
class Log final : public utility::fsm::FSM<Log>
{
public:
    //! @brief Destroy the Log object.
    virtual ~Log() = default;
    //! @brief Construct a new Log object.
    Log(const Log&) = delete;
    //! @brief The operator (=) overloading of Log class.
    //! @return reference of the Log object
    Log& operator=(const Log&) = delete;

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
    //! @brief Get the existing Log instance. Include configuration validation.
    //! @return reference of the Log object
    static Log& getExistingInstance();
    //! @brief Interface for running logger.
    void runLogger();
    //! @brief Wait for the logger to start. External use.
    void waitToStart();
    //! @brief Wait for the logger to stop. External use.
    void waitToStop();
    //! @brief Request to rollback the logger. External use.
    void requestToRollback();
    //! @brief Get log file path.
    //! @return log file path
    std::string getFilePath() const;
    //! @brief Get log file lock.
    //! @return log file lock
    utility::file::ReadWriteLock& getFileLock();

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
        std::ostringstream stream;
        //! @brief Code file.
        const std::string& file;
        //! @brief Code line.
        const std::uint32_t line;

        //! @brief Flush the output stream.
        inline void flush() { getInstance().flush(Lv, file, line, stream.str()); }
    };

private:
    //! @brief Construct a new Log object.
    //! @param initState - initialization value of state
    explicit Log(const StateType initState = State::init) noexcept :
        FSM(initState),
        filePath(getFullDefaultLogPath(CONFIG_HELPER_LOGGER_FILE_PATH)),
        writeType(OutputType(CONFIG_HELPER_LOGGER_WRITE_TYPE)),
        minimumLevel(OutputLevel(CONFIG_HELPER_LOGGER_MINIMUM_LEVEL)),
        usedMedium(OutputMedium(CONFIG_HELPER_LOGGER_USED_MEDIUM))
    {
    }

    //! @brief Maximum number of times to wait for the logger to change to the target state.
    static constexpr std::uint16_t maxTimesOfWaitLogger{20};
    //! @brief Time interval (ms) to wait for the logger to change to the target state.
    static constexpr std::uint16_t intervalOfWaitLogger{10};
    //! @brief The queue of logs.
    std::queue<std::string> logQueue;
    //! @brief Mutex for controlling queue.
    mutable std::mutex mtx{};
    //! @brief The synchronization condition for queue. Use with mtx.
    std::condition_variable cv{};
    //! @brief Flag to indicate whether it is logging.
    std::atomic<bool> isLogging{false};
    //! @brief Flag for rollback request.
    std::atomic<bool> rollbackRequest{false};
    //! @brief Output file stream.
    std::ofstream ofs;
    //! @brief Log file absolute path.
    const std::string filePath{getFullDefaultLogPath()};
    //! @brief Write type.
    const OutputType writeType{OutputType::add};
    //! @brief Minimum level.
    const OutputLevel minimumLevel{OutputLevel::debug};
    //! @brief Used medium.
    const OutputMedium usedMedium{OutputMedium::both};
    //! @brief Log file lock.
    utility::file::ReadWriteLock fileLock;

    //! @brief Handle the log queue.
    void handleLogQueue();
    //! @brief Get the full path to the default log file.
    //! @param filename - default filename
    //! @return full path to the default log file
    static std::string getFullDefaultLogPath(const std::string& filename = std::string{defaultLogFile});
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
    using TransitionMap = Map<
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
    //! @brief Await notification for rollback.
    //! @return whether rollback is required or not
    bool awaitNotification4Rollback();

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
    const auto outputFormatter = [&](const std::string_view& prefix)
    {
        std::string validFormat = format;
        validFormat.erase(
            std::remove_if(
                std::begin(validFormat),
                std::end(validFormat),
                [l = std::locale{}](const auto c)
                {
                    return ((' ' != c) && std::isspace(c, l));
                }),
            std::end(validFormat));
        std::string output = std::string{prefix} + ":[" + utility::time::getCurrentSystemTime() + "]:["
            + codeFile.substr(codeFile.find("foo/") + 4, codeFile.length()) + '#' + std::to_string(codeLine)
            + "]: " + utility::common::formatString(validFormat.c_str(), std::forward<Args>(args)...);
        return output;
    };

    if (!((State::work == currentState()) && !rollbackRequest.load()))
    {
        std::string output = outputFormatter(unknownLevelPrefix);
        std::cerr << changeToLogStyle(output) << std::endl;
        return;
    }

    if (std::unique_lock<std::mutex> lock(mtx); true)
    {
        if (level >= minimumLevel)
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
                case OutputLevel::warning:
                    prefix = warnLevelPrefix;
                    break;
                case OutputLevel::error:
                    prefix = errorLevelPrefix;
                    break;
                default:
                    break;
            }

            std::string output = outputFormatter(prefix);
            logQueue.push(std::move(output));
            lock.unlock();
            cv.notify_one();
        }
    }
}
} // namespace log
} // namespace application
