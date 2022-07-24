#pragma once
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
    logger.outputLog(Log::Level::levelDebug, __FILE__, __LINE__, format, ##args)
#define LOGGER_INF(format, args...) \
    logger.outputLog(Log::Level::levelInfo, __FILE__, __LINE__, format, ##args)
#define LOGGER_WRN(format, args...) \
    logger.outputLog(Log::Level::levelWarn, __FILE__, __LINE__, format, ##args)
#define LOGGER_ERR(format, args...) \
    logger.outputLog(Log::Level::levelError, __FILE__, __LINE__, format, ##args)

class Log final
{
public:
    enum Type
    {
        typeAdd,
        typeOver
    };
    enum Level
    {
        levelDebug,
        levelInfo,
        levelWarn,
        levelError
    };
    enum Target
    {
        targetFile,
        targetTerminal,
        targetAll
    };
    Log() noexcept;
    Log(const std::string& logFile, const Type type, const Level level,
        const Target target) noexcept;
    virtual ~Log();
    template <typename... Args>
    void outputLog(
        const uint32_t level, const std::string& codeFile, const uint32_t codeLine,
        const char* const __restrict format, Args&&... args);
    std::ofstream& getOfs() const;

private:
    mutable std::ofstream ofs;
    Level minLevel{Level::levelDebug};
    Target realTarget{Target::targetAll};
    char pathname[LOG_PATHNAME_LENGTH + 1]{LOG_PATH};
};

template <typename... Args>
void Log::outputLog(
    const uint32_t level, const std::string& codeFile, const uint32_t codeLine,
    const char* const __restrict format, Args&&... args)
{
    if (level >= minLevel)
    {
        std::string prefix;
        switch (level)
        {
            case Level::levelDebug:
                prefix = LOG_PREFIX_DEBUG;
                break;
            case Level::levelInfo:
                prefix = LOG_PREFIX_INFO;
                break;
            case Level::levelWarn:
                prefix = LOG_PREFIX_WARN;
                break;
            case Level::levelError:
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
        switch (realTarget)
        {
            case Target::targetFile:
                ofs << output << std::endl;
                break;
            case Target::targetTerminal:
                std::cout << changeLogLevelStyle(output) << std::endl;
                break;
            case Target::targetAll:
                ofs << output << std::endl;
                std::cout << changeLogLevelStyle(output) << std::endl;
                break;
            default:
                break;
        }
    }
}
