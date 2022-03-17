#pragma once
#define NDEBUG
#include <cassert>
#include <climits>
#include <cstring>
#include <fstream>
#include <iostream>
#include "time.hpp"

extern class Log logger;
void printFile(
    const char *const pathname,
    const bool reverse = false,
    const uint32_t maxLine = 1000);

#define FILENAME(x) (strrchr(x, '/') ? strrchr(x, '/') + 1 : x)
#define LOG_DIR "./temp"
#define LOG_PATH "./temp/foo.log"
#define LOG_PATHNAME_LENGTH 32
#define LOGGER(level, format, args...) logger.outputLog(level, __FILE__, __LINE__, format, ##args)

#define FORMAT_STRING(format, args...)                                                             \
    ({                                                                                             \
        const int bufferSize = std::snprintf(nullptr, 0, format, ##args);                          \
        assert(bufferSize >= 0);                                                                   \
        char buffer[bufferSize + 1];                                                               \
        buffer[0] = '\0';                                                                          \
        std::snprintf(buffer, bufferSize + 1, format, ##args);                                     \
        const std::string str(buffer);                                                             \
        str;                                                                                       \
    })

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
    Log(const std::string &logFile,
        const Type type,
        const Level level,
        const Target target) noexcept;
    virtual ~Log();
    const std::ofstream &getOfs() const;
    template <typename... Args>
    void outputLog(
        const uint32_t level,
        const std::string &codeFile,
        const uint32_t codeLine,
        const char *const format,
        const Args... args);

private:
    Level minLevel;
    Target realTarget;
    std::ofstream ofs;
    char pathname[LOG_PATHNAME_LENGTH + 1];
    friend std::string getCurrentSystemTime(char *const date);
};

template <typename... Args>
void Log::outputLog(
    const uint32_t level,
    const std::string &codeFile,
    const uint32_t codeLine,
    const char *const format,
    const Args... args)
{
    if (level >= minLevel)
    {
        std::string prefix = "";
        switch (level)
        {
            case Level::levelDebug:
                prefix = "[DEBUG]:";
                break;
            case Level::levelInfo:
                prefix = "[INFO] :";
                break;
            case Level::levelWarn:
                prefix = "[WARN] :";
                break;
            case Level::levelError:
                prefix = "[ERROR]:";
                break;
            default:
                break;
        }
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
        std::string output = prefix + "[" + GET_CURRENT_TIME + "]:" + "[" +
            FILENAME(codeFile.c_str()) + ":" + std::to_string(codeLine) +
            "]: " + FORMAT_STRING(format, args...);
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
