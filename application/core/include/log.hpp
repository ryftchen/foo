//! @file log.hpp
//! @author ryftchen
//! @brief The declarations (log) in the application module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen. All rights reserved.

#pragma once

#ifndef __PRECOMPILED_HEADER
#include <filesystem>
#include <fstream>
#include <iostream>
#include <queue>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER
#include "utility/include/common.hpp"
#include "utility/include/file.hpp"
#include "utility/include/fsm.hpp"
#include "utility/include/time.hpp"

//! @brief Log with debug level.
#define LOG_DBG application::log::Log::Flush<application::log::Log::OutputLevel::debug>(__FILE__, __LINE__).getStream()
//! @brief Log with info level.
#define LOG_INF application::log::Log::Flush<application::log::Log::OutputLevel::info>(__FILE__, __LINE__).getStream()
//! @brief Log with warning level.
#define LOG_WRN application::log::Log::Flush<application::log::Log::OutputLevel::warn>(__FILE__, __LINE__).getStream()
//! @brief Log with error level.
#define LOG_ERR application::log::Log::Flush<application::log::Log::OutputLevel::error>(__FILE__, __LINE__).getStream()
//! @brief Try to start logging.
#define LOG_WAIT_TO_START application::log::Log::getInstance().waitToStart()
//! @brief Try to stop logging.
#define LOG_WAIT_TO_STOP application::log::Log::getInstance().waitToStop()
//! @brief Try to restart logging.
#define LOG_REQUEST_TO_RESTART application::log::Log::getInstance().requestToRestart()
//! @brief Log file path.
#define LOG_FILE_PATH application::log::Log::getInstance().getFilePath()
//! @brief Log file lock.
#define LOG_FILE_LOCK application::log::Log::getInstance().getFileLock()

//! @brief Log-related functions in the application module.
namespace application::log
{
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
constexpr auto debugLevelPrefixColorForLog = utility::common::joinStr<
    utility::common::colorBlue,
    utility::common::colorBold,
    utility::common::colorForBackground,
    debugLevelPrefix,
    utility::common::colorOff>;
//! @brief Prefix of info level. Include ANSI escape codes.
constexpr auto infoLevelPrefixColorForLog = utility::common::joinStr<
    utility::common::colorGreen,
    utility::common::colorBold,
    utility::common::colorForBackground,
    infoLevelPrefix,
    utility::common::colorOff>;
//! @brief Prefix of warning level. Include ANSI escape codes.
constexpr auto warnLevelPrefixColorForLog = utility::common::joinStr<
    utility::common::colorYellow,
    utility::common::colorBold,
    utility::common::colorForBackground,
    warnLevelPrefix,
    utility::common::colorOff>;
//! @brief Prefix of error level. Include ANSI escape codes.
constexpr auto errorLevelPrefixColorForLog = utility::common::joinStr<
    utility::common::colorRed,
    utility::common::colorBold,
    utility::common::colorForBackground,
    errorLevelPrefix,
    utility::common::colorOff>;
//! @brief Prefix of unknown level. Include ANSI escape codes.
constexpr auto unknownLevelPrefixColorForLog = utility::common::joinStr<
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
    //! @return reference of Log object
    Log& operator=(const Log&) = delete;

    friend class FSM<Log>;
    //! @brief Enumerate specific output types.
    enum class OutputType : std::uint8_t
    {
        add,
        over
    };
    //! @brief Enumerate specific output levels.
    enum class OutputLevel : std::uint8_t
    {
        debug,
        info,
        warn,
        error
    };
    //! @brief Enumerate specific output targets.
    enum class OutputTarget : std::uint8_t
    {
        file,
        terminal,
        all
    };
    //! @brief Enumerate specific states for FSM.
    enum State : std::uint8_t
    {
        init,
        idle,
        work,
        done
    };

    //! @brief Get the Log instance.
    //! @return reference of Log object
    static Log& getInstance();
    //! @brief Interface for running logger.
    void runLogger();
    //! @brief Wait for the logger to start. External use.
    void waitToStart();
    //! @brief Wait for the logger to stop. External use.
    void waitToStop();
    //! @brief Request to restart the logger. External use.
    void requestToRestart();
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
        const char* const format,
        Args&&... args);
    //! @brief Log flush helper.
    //! @tparam Lv - type of output level
    template <OutputLevel Lv>
    class Flush
    {
    public:
        //! @brief Construct a new Flush object.
        //! @param codeFile - current code file
        //! @param codeLine - current code line
        Flush(const std::string& codeFile, const std::uint32_t codeLine) : file(codeFile), line(codeLine){};
        //! @brief Destroy the Flush object.
        virtual ~Flush() { flush(); };

        //! @brief Get the output stream for flushing.
        //! @return reference of output stream object, which is on string based
        inline std::ostringstream& getStream() { return stream; };
        //! @brief Flush the output stream.
        inline void flush() { getInstance().flush(Lv, file, line, stream.str().c_str()); };

    private:
        //! @brief Output stream for flushing.
        std::ostringstream stream;
        //! @brief Code file.
        const std::string& file;
        //! @brief Code line.
        const std::uint32_t line;
    };
    //! @brief Get log file path.
    //! @return log file path
    inline std::string getFilePath() const;
    //! @brief Get log file lock.
    //! @return log file lock
    inline utility::file::ReadWriteLock& getFileLock();

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
    Log(const char* const logFile,
        const OutputType type,
        const OutputLevel level,
        const OutputTarget target,
        const StateType initState = State::init) noexcept :
        filePath(logFile), writeType(type), minLevel(level), actTarget(target), FSM(initState){};

    //! @brief Maximum number of times to wait for the logger to change to the target state.
    static constexpr std::uint16_t maxTimesOfWaitLogger{20};
    //! @brief Time interval (ms) to wait for the logger to change to the target state.
    static constexpr std::uint16_t intervalOfWaitLogger{10};
    //! @brief The queue of logs.
    std::queue<std::string> logQueue;
    //! @brief Mutex for controlling queue.
    mutable std::mutex mtx;
    //! @brief The synchronization condition for queue. Use with mtx.
    std::condition_variable cv;
    //! @brief Flag to indicate whether it is logging.
    std::atomic<bool> isLogging{false};
    //! @brief Flag for restart request.
    std::atomic<bool> restartRequest{false};
    //! @brief Output file stream.
    std::ofstream ofs;
    //! @brief Log file path.
    std::string filePath{"./log/foo.log"};
    //! @brief Write type.
    OutputType writeType{OutputType::add};
    //! @brief Minimum level.
    OutputLevel minLevel{OutputLevel::debug};
    //! @brief Actual target.
    OutputTarget actTarget{OutputTarget::all};
    //! @brief Log file lock.
    utility::file::ReadWriteLock fileLock;

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
    //! @brief FSM event. Relaunch.
    struct Relaunch
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
    //! @brief Roll back.
    void rollBack();

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
        // --- Source ---+--- Event ---+--- Target ---+------ Action ------+--- Guard(Optional) ---
        // --------------+-------------+--------------+--------------------+-----------------------
        Row< State::init ,  OpenFile   , State::idle  , &Log::openLogFile                         >,
        Row< State::idle ,  GoLogging  , State::work  , &Log::startLogging , &Log::isLogFileOpen  >,
        Row< State::work ,  CloseFile  , State::idle  , &Log::closeLogFile                        >,
        Row< State::idle ,  NoLogging  , State::done  , &Log::stopLogging  , &Log::isLogFileClose >,
        Row< State::idle ,  Relaunch   , State::init  , &Log::rollBack                            >,
        Row< State::work ,  Relaunch   , State::init  , &Log::rollBack                            >
        // --------------+-------------+--------------+--------------------+-----------------------
        >;
    // clang-format on

protected:
    friend std::ostream& operator<<(std::ostream& os, const State state);
};

extern const std::string& changeToLogStyle(std::string& line);

template <typename... Args>
void Log::flush(
    const OutputLevel level,
    const std::string& codeFile,
    const std::uint32_t codeLine,
    const char* const format,
    Args&&... args)
{
    const auto outputFormatter = [&](const std::string_view& prefix)
    {
        std::string output = std::string{prefix} + ":[" + utility::time::getCurrentSystemTime() + "]:["
            + codeFile.substr(codeFile.find('/') + 1, codeFile.length()) + '#' + std::to_string(codeLine)
            + "]: " + utility::common::formatString(format, std::forward<Args>(args)...);
        return output;
    };

    if (State::work != currentState())
    {
        std::string output = outputFormatter(unknownLevelPrefix);
        std::cerr << changeToLogStyle(output) << std::endl;
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

            std::string output = outputFormatter(prefix);
            logQueue.push(std::move(output));
            lock.unlock();
            cv.notify_one();
            utility::time::millisecondLevelSleep(1);
            lock.lock();
        }
    }
}

inline std::string Log::getFilePath() const
{
    return std::filesystem::absolute(filePath).string();
}

inline utility::file::ReadWriteLock& Log::getFileLock()
{
    return fileLock;
}
} // namespace application::log
