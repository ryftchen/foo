#pragma once

#include <condition_variable>
#include <queue>
#include "utility/include/common.hpp"
#include "utility/include/fsm.hpp"
#include "utility/include/time.hpp"

#define LOG_TO_START util_log::Log::getInstance().waitStartForExternalUse()
#define LOG_TO_STOP util_log::Log::getInstance().waitStopForExternalUse()
#define LOG_DBG(format, args...) \
    util_log::Log::getInstance().flush(util_log::Log::OutputLevel::debug, __FILE__, __LINE__, format, ##args)
#define LOG_INF(format, args...) \
    util_log::Log::getInstance().flush(util_log::Log::OutputLevel::info, __FILE__, __LINE__, format, ##args)
#define LOG_WRN(format, args...) \
    util_log::Log::getInstance().flush(util_log::Log::OutputLevel::warn, __FILE__, __LINE__, format, ##args)
#define LOG_ERR(format, args...) \
    util_log::Log::getInstance().flush(util_log::Log::OutputLevel::error, __FILE__, __LINE__, format, ##args)

namespace util_log
{
inline constexpr std::string_view logPath{"./temporary/foo.log"};
constexpr uint32_t logPathLength = 32;
constexpr uint32_t maxCountOfWaitLogger = 10;
constexpr std::string_view logDirectory{"./temporary"};
constexpr std::string_view debugPrefix{"[DBG]"};
constexpr std::string_view infoPrefix{"[INF]"};
constexpr std::string_view warnPrefix{"[WRN]"};
constexpr std::string_view errorPrefix{"[ERR]"};
constexpr std::string_view debugRegex{R"(^\[DBG\])"};
constexpr std::string_view infoRegex{R"(^\[INF\])"};
constexpr std::string_view warnRegex{R"(^\[WRN\])"};
constexpr std::string_view errorRegex{R"(^\[ERR\])"};
constexpr auto debugColorForLog{util_common::joinStr<util_common::colorBold, debugPrefix, util_common::colorEscape>};
constexpr auto infoColorForLog{
    util_common::joinStr<util_common::colorGreen, util_common::colorBold, infoPrefix, util_common::colorEscape>};
constexpr auto warnColorForLog{
    util_common::joinStr<util_common::colorYellow, util_common::colorBold, warnPrefix, util_common::colorEscape>};
constexpr auto errorColorForLog{
    util_common::joinStr<util_common::colorRed, util_common::colorBold, errorPrefix, util_common::colorEscape>};

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

    Log(Log const&) = delete;
    Log& operator=(Log const&) = delete;
    virtual ~Log() = default;

    static inline Log& getInstance();
    template <typename... Args>
    void flush(
        const OutputLevel level,
        const std::string& codeFile,
        const uint32_t codeLine,
        const char* const __restrict format,
        Args&&... args);
    void runLogger();
    void waitStartForExternalUse();
    void waitStopForExternalUse();

private:
    explicit Log(const StateType initState = State::init) noexcept : FSM(initState){};
    Log(const std::string& logFile,
        const OutputType type,
        const OutputLevel level,
        const OutputTarget target,
        const StateType initState = State::init) noexcept;

    mutable std::mutex queueMutex;
    std::queue<std::string> logQueue;
    std::condition_variable logCondition;
    std::atomic<bool> isLogging{false};
    std::ofstream ofs;
    OutputType writeType{OutputType::add};
    OutputLevel minLevel{OutputLevel::debug};
    OutputTarget actualTarget{OutputTarget::all};
    char pathname[logPathLength + 1]{"./temporary/foo.log"};

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
            output.append(COMMON_FORMAT_TO_STRING(format, std::forward<Args>(args)...));
#pragma GCC diagnostic pop
            logQueue.push(std::move(output));

            lock.unlock();
            logCondition.notify_one();
            util_time::millisecondLevelSleep(1);
            lock.lock();
        }
    }
}

extern std::string& changeLogLevelStyle(std::string& line);
} // namespace util_log
