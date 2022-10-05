#pragma once

#include <condition_variable>
#include <queue>
#include "file.hpp"
#include "fsm.hpp"
#include "time.hpp"

#define LOG_DEFAULT_LOG_PATH "./temp/foo.log"
#define LOG_DBG(logObj, format, args...) \
    util_log::logObj.output(util_log::Log::OutputLevel::debug, __FILE__, __LINE__, format, ##args)
#define LOG_INF(logObj, format, args...) \
    util_log::logObj.output(util_log::Log::OutputLevel::info, __FILE__, __LINE__, format, ##args)
#define LOG_WRN(logObj, format, args...) \
    util_log::logObj.output(util_log::Log::OutputLevel::warn, __FILE__, __LINE__, format, ##args)
#define LOG_ERR(logObj, format, args...) \
    util_log::logObj.output(util_log::Log::OutputLevel::error, __FILE__, __LINE__, format, ##args)
#define LOG_TO_START(logObj) util_log::logObj.waitLoggerStartForExternalUse();
#define LOG_TO_STOP(logObj) util_log::logObj.waitLoggerStopForExternalUse();

namespace util_log
{
inline constexpr std::string_view logPath{LOG_DEFAULT_LOG_PATH};
constexpr uint32_t logPathLength = 32;
constexpr uint32_t maxCountOfWaitLogger = 10;
constexpr std::string_view logDirectory{"./temp"};
constexpr std::string_view debugPrefix{"[DBG]"};
constexpr std::string_view infoPrefix{"[INF]"};
constexpr std::string_view warnPrefix{"[WRN]"};
constexpr std::string_view errorPrefix{"[ERR]"};
constexpr std::string_view infoRegex{R"(^\[INF\])"};
constexpr std::string_view warnRegex{R"(^\[WRN\])"};
constexpr std::string_view errorRegex{R"(^\[ERR\])"};
constexpr auto infoColorForLog{util_file::joinStr<util_file::greenForeground, infoPrefix, util_file::colorEnd>};
constexpr auto warnColorForLog{util_file::joinStr<util_file::yellowForeground, warnPrefix, util_file::colorEnd>};
constexpr auto errorColorForLog{util_file::joinStr<util_file::redForeground, errorPrefix, util_file::colorEnd>};

class Log final : public util_fsm::FSM<Log>
{
public:
    friend class FSM<Log>;
    enum class OutputType
    {
        add,
        over
    };
    enum class OutputLevel
    {
        debug,
        info,
        warn,
        error
    };
    enum class OutputTarget
    {
        file,
        terminal,
        all
    };
    enum State
    {
        init,
        idle,
        work,
        done
    };

    explicit Log(const StateType initState = State::init) noexcept;
    Log(const std::string& logFile,
        const OutputType type,
        const OutputLevel level,
        const OutputTarget target,
        const StateType initState = State::init) noexcept;
    virtual ~Log() = default;

    template <typename... Args>
    void output(
        const OutputLevel level,
        const std::string& codeFile,
        const uint32_t codeLine,
        const char* const __restrict format,
        Args&&... args);
    void runLogger();
    void waitLoggerStartForExternalUse();
    void waitLoggerStopForExternalUse();

private:
    std::ofstream ofs;
    OutputType writeType{OutputType::add};
    OutputLevel minLevel{OutputLevel::debug};
    OutputTarget actualTarget{OutputTarget::all};
    char pathname[logPathLength + 1]{LOG_DEFAULT_LOG_PATH};

    mutable std::mutex queueMutex;
    std::queue<std::string> logQueue;
    std::condition_variable loggingCondition;
    std::atomic<bool> isLogging{false};

    struct OpenFile
    {
    };
    struct CloseFile
    {
    };
    struct GoLogging
    {
    };
    struct NoLogging
    {
    };

    void openLogFile();
    void startLogging();
    void closeLogFile();
    void stopLogging();

    bool isLogFileOpen(const GoLogging& /*unused*/) const;
    bool isLogFileClose(const NoLogging& /*unused*/) const;
    // clang-format off
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
void Log::output(
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
                    prefix = debugPrefix;
                    break;
                case OutputLevel::info:
                    prefix = infoPrefix;
                    break;
                case OutputLevel::warn:
                    prefix = warnPrefix;
                    break;
                case OutputLevel::error:
                    prefix = errorPrefix;
                    break;
                default:
                    break;
            }

            std::string output = std::string{prefix} + ":[" + util_time::getCurrentSystemTime() + "]:["
                + std::filesystem::path(codeFile.c_str()).filename().string() + "#" + std::to_string(codeLine) + "]: ";
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
            output.append(FORMAT_TO_STRING(format, std::forward<Args>(args)...));
#pragma GCC diagnostic pop
            logQueue.push(std::move(output));

            lock.unlock();
            loggingCondition.notify_one();
            util_time::millisecondLevelSleep(1);
            lock.lock();
        }
    }
}

extern class Log logger;
std::string& changeLogLevelStyle(std::string& line);
} // namespace util_log
