#pragma once
#include <atomic>
#include <condition_variable>
#include <queue>
#include <thread>
#include "file.hpp"
#include "time.hpp"

extern class Log logger;

std::string changeLogLevelStyle(std::string& line);

#define LOG_DIR "./temp"
#define LOG_PATH "./temp/foo.log"
#define LOG_PATHNAME_LENGTH BUFFER_SIZE_32
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
    logger.output(Log::Level::debug, __FILE__, __LINE__, format, ##args)
#define LOGGER_INF(format, args...) \
    logger.output(Log::Level::info, __FILE__, __LINE__, format, ##args)
#define LOGGER_WRN(format, args...) \
    logger.output(Log::Level::warn, __FILE__, __LINE__, format, ##args)
#define LOGGER_ERR(format, args...) \
    logger.output(Log::Level::error, __FILE__, __LINE__, format, ##args)
#define LOGGER_EXIT logger.exit();

class Log final
{
public:
    enum Type
    {
        add,
        over
    };
    enum Level
    {
        debug,
        info,
        warn,
        error
    };
    enum Target
    {
        file,
        terminal,
        all
    };
    Log() noexcept = default;
    Log(const std::string& logFile, const Type type, const Level level,
        const Target target) noexcept;
    virtual ~Log();
    void runLogger();
    template <typename... Args>
    void output(
        const uint32_t level, const std::string& codeFile, const uint32_t codeLine,
        const char* const __restrict format, Args&&... args);
    void exit();

private:
    std::ofstream ofs;
    Type writeType{Type::add};
    Level minLevel{Level::debug};
    Target realTarget{Target::all};
    char pathname[LOG_PATHNAME_LENGTH + 1]{LOG_PATH};

    std::atomic<bool> isLogging{false};
    std::condition_variable loggingCondition;
    mutable std::mutex logQueueMutex;
    std::queue<std::string> logQueue;
};

template <typename... Args>
void Log::output(
    const uint32_t level, const std::string& codeFile, const uint32_t codeLine,
    const char* const __restrict format, Args&&... args)
{
    if (std::unique_lock<std::mutex> lock(logQueueMutex); true)
    {
        if (level >= minLevel)
        {
            std::string prefix;
            switch (level)
            {
                case Level::debug:
                    prefix = LOG_PREFIX_DEBUG;
                    break;
                case Level::info:
                    prefix = LOG_PREFIX_INFO;
                    break;
                case Level::warn:
                    prefix = LOG_PREFIX_WARN;
                    break;
                case Level::error:
                    prefix = LOG_PREFIX_ERROR;
                    break;
                default:
                    break;
            }

            std::string output = prefix + ":[" + TIME_GET_CURRENT_DATE + "]:["
                + std::filesystem::path(codeFile.c_str()).filename().string() + "#"
                + std::to_string(codeLine) + "]: ";
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
            output.append(FORMAT_TO_STRING(format, std::forward<Args>(args)...));
#pragma GCC diagnostic pop
            logQueue.push(std::move(output));

            lock.unlock();
            loggingCondition.notify_one();
            std::this_thread::sleep_until(
                std::chrono::steady_clock::now() + std::chrono::operator""ms(1));
            lock.lock();
        }
    }
}
