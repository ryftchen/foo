#pragma once

#include <condition_variable>
#include <queue>
#include <thread>
#include "file.hpp"
#include "fsm.hpp"
#include "time.hpp"

namespace util_log
{
extern class Log logger;
std::string& changeLogLevelStyle(std::string& line);

#define LOG_DIR "./temp"
#define LOG_PATH "./temp/foo.log"
#define LOG_PATHNAME_LENGTH 32
#define LOG_PREFIX_DEBUG "[DBG]"
#define LOG_PREFIX_INFO "[INF]"
#define LOG_PREFIX_WARN "[WRN]"
#define LOG_PREFIX_ERROR "[ERR]"
#define LOG_REGEX_INFO R"(^\[INF\])"
#define LOG_REGEX_WARN R"(^\[WRN\])"
#define LOG_REGEX_ERROR R"(^\[ERR\])"
#define LOG_COLOR_INFO \
    (std::string(PRINT_COLOR_GREEN) + std::string(LOG_PREFIX_INFO) + std::string(PRINT_COLOR_END))
#define LOG_COLOR_WARN \
    (std::string(PRINT_COLOR_YELLOW) + std::string(LOG_PREFIX_WARN) + std::string(PRINT_COLOR_END))
#define LOG_COLOR_ERROR \
    (std::string(PRINT_COLOR_RED) + std::string(LOG_PREFIX_ERROR) + std::string(PRINT_COLOR_END))

#define LOGGER_DBG(format, args...) \
    util_log::logger.output(util_log::Log::OutputLevel::debug, __FILE__, __LINE__, format, ##args)
#define LOGGER_INF(format, args...) \
    util_log::logger.output(util_log::Log::OutputLevel::info, __FILE__, __LINE__, format, ##args)
#define LOGGER_WRN(format, args...) \
    util_log::logger.output(util_log::Log::OutputLevel::warn, __FILE__, __LINE__, format, ##args)
#define LOGGER_ERR(format, args...) \
    util_log::logger.output(util_log::Log::OutputLevel::error, __FILE__, __LINE__, format, ##args)
#define LOGGER_START util_log::logger.waitLoggerStart();
#define LOGGER_STOP util_log::logger.waitLoggerStop();

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
    Log(const std::string& logFile, const OutputType type, const OutputLevel level,
        const OutputTarget target, const StateType initState = State::init) noexcept;
    virtual ~Log() = default;
    template <typename... Args>
    void output(
        const OutputLevel level, const std::string& codeFile, const uint32_t codeLine,
        const char* const __restrict format, Args&&... args);
    void runLogger();
    void waitLoggerStart();
    void waitLoggerStop();

private:
    std::ofstream ofs;
    OutputType writeType{OutputType::add};
    OutputLevel minLevel{OutputLevel::debug};
    OutputTarget actualTarget{OutputTarget::all};
    char pathname[LOG_PATHNAME_LENGTH + 1]{LOG_PATH};

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
    const OutputLevel level, const std::string& codeFile, const uint32_t codeLine,
    const char* const __restrict format, Args&&... args)
{
    if (State::work != currentState())
    {
        return;
    }

    if (std::unique_lock<std::mutex> lock(queueMutex); true)
    {
        if (level >= minLevel)
        {
            std::string prefix;
            switch (level)
            {
                case OutputLevel::debug:
                    prefix = LOG_PREFIX_DEBUG;
                    break;
                case OutputLevel::info:
                    prefix = LOG_PREFIX_INFO;
                    break;
                case OutputLevel::warn:
                    prefix = LOG_PREFIX_WARN;
                    break;
                case OutputLevel::error:
                    prefix = LOG_PREFIX_ERROR;
                    break;
                default:
                    break;
            }

            std::string output = prefix + ":[" + util_time::getCurrentSystemTime() + "]:["
                + std::filesystem::path(codeFile.c_str()).filename().string() + "#"
                + std::to_string(codeLine) + "]: ";
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
            output.append(FORMAT_TO_STRING(format, std::forward<Args>(args)...));
#pragma GCC diagnostic pop
            logQueue.push(std::move(output));

            lock.unlock();
            loggingCondition.notify_one();
            TIME_SLEEP_MILLISECOND(1);
            lock.lock();
        }
    }
}
} // namespace util_log
