#pragma once
#define NDEBUG
#include <cassert>
#include <climits>
#include <cstring>
#include <fstream>
#include "main.hpp"
#include "time.hpp"

extern class Log logger;
void printFile(
    const char* const pathname, const bool reverse = false, const uint32_t maxLine = 1000);

#define FILENAME(x) (strrchr(x, '/') ? strrchr(x, '/') + 1 : x)
#define LOG_DIR "./temp"
#define LOG_PATH "./temp/foo.log"
#define LOG_PATHNAME_LENGTH 32
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
    const std::ofstream& getOfs() const;
    template <typename... Args>
    void outputLog(
        const uint32_t level, const std::string& codeFile, const uint32_t codeLine,
        const char* const __restrict format, const Args... args);

private:
    Level minLevel;
    Target realTarget;
    std::ofstream ofs;
    char pathname[LOG_PATHNAME_LENGTH + 1];
    friend std::string getCurrentSystemTime(char* const date);
};

template <typename... Args>
void Log::outputLog(
    const uint32_t level, const std::string& codeFile, const uint32_t codeLine,
    const char* const __restrict format, const Args... args)
{
    if (level >= minLevel)
    {
        std::string prefix = "";
        switch (level)
        {
            case Level::levelDebug:
                prefix = "[DBG]:";
                break;
            case Level::levelInfo:
                prefix = "[INF]:";
                break;
            case Level::levelWarn:
                prefix = "[WRN]:";
                break;
            case Level::levelError:
                prefix = "[ERR]:";
                break;
            default:
                break;
        }
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
        std::string output = prefix + "[" + GET_CURRENT_TIME + "]:" + "["
            + FILENAME(codeFile.c_str()) + ":" + std::to_string(codeLine)
            + "]: " + FORMAT_TO_STRING(format, args...);
#pragma GCC diagnostic pop
        switch (realTarget)
        {
            case Target::targetFile:
                ofs << output << std::endl;
                break;
            case Target::targetTerminal:
                std::cout << output << std::endl;
                break;
            case Target::targetAll:
                ofs << output << std::endl;
                std::cout << output << std::endl;
                break;
            default:
                break;
        }
    }
}
