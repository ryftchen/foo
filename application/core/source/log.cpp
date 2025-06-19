//! @file log.cpp
//! @author ryftchen
//! @brief The definitions (log) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "log.hpp"

#ifndef _PRECOMPILED_HEADER
#include <cassert>
#include <filesystem>
#include <fstream>
#include <ranges>
#include <regex>
#else
#include "application/pch/precompiled_header.hpp"
#endif // _PRECOMPILED_HEADER

#include "utility/include/time.hpp"

namespace application::log
{
//! @brief Anonymous namespace.
inline namespace
{
//! @brief ANSI escape codes for red foreground color.
constexpr std::string_view escColorRed = "\033[0;31;40m";
//! @brief ANSI escape codes for green foreground color.
constexpr std::string_view escColorGreen = "\033[0;32;40m";
//! @brief ANSI escape codes for yellow foreground color.
constexpr std::string_view escColorYellow = "\033[0;33;40m";
//! @brief ANSI escape codes for blue foreground color.
constexpr std::string_view escColorBlue = "\033[0;34;40m";
//! @brief ANSI escape codes for the bold font.
constexpr std::string_view escFontBold = "\033[1m";
//! @brief ANSI escape codes for the dim font.
constexpr std::string_view escFontDim = "\033[2m";
//! @brief ANSI escape codes for the italic font.
constexpr std::string_view escFontItalic = "\033[3m";
//! @brief ANSI escape codes for the underline font.
constexpr std::string_view escFontUnderline = "\033[4m";
//! @brief ANSI escape codes for the inverse font.
constexpr std::string_view escFontInverse = "\033[7m";
//! @brief ANSI escape codes for default foreground color.
constexpr std::string_view escFgColor = "\033[39m";
//! @brief ANSI escape codes for default background color.
constexpr std::string_view escBgColor = "\033[49m";
//! @brief ANSI escape codes for ending.
constexpr std::string_view escOff = "\033[0m";

//! @brief Prefix of debug level in log.
constexpr std::string_view debugLevelPrefix = "[DBG]";
//! @brief Prefix of info level in log.
constexpr std::string_view infoLevelPrefix = "[INF]";
//! @brief Prefix of warning level in log.
constexpr std::string_view warningLevelPrefix = "[WRN]";
//! @brief Prefix of error level in log.
constexpr std::string_view errorLevelPrefix = "[ERR]";
//! @brief Prefix of trace level in log.
constexpr std::string_view traceLevelPrefix = "[TRC]";
//! @brief Regular expression of debug level in log.
constexpr std::string_view debugLevelPrefixRegex = R"(\[DBG\])";
//! @brief Regular expression of info level in log.
constexpr std::string_view infoLevelPrefixRegex = R"(\[INF\])";
//! @brief Regular expression of warning level in log.
constexpr std::string_view warningLevelPrefixRegex = R"(\[WRN\])";
//! @brief Regular expression of error level in log.
constexpr std::string_view errorLevelPrefixRegex = R"(\[ERR\])";
//! @brief Regular expression of trace level in log.
constexpr std::string_view traceLevelPrefixRegex = R"(\[TRC\])";
//! @brief Regular expression of date time in log.
constexpr std::string_view dateTimeRegex = R"(\[(\d{4})-(\d{2})-(\d{2}) (\d{2}):(\d{2}):(\d{2})\.(\d{6}) (\w{3})\])";
//! @brief Regular expression of code file in log.
constexpr std::string_view codeFileRegex = R"(\[[^ ]+\.(c|h|cc|hh|cpp|hpp|tpp|cxx|hxx|C|H)#\d+\])";

//! @brief Debug level prefix with color. Include ANSI escape codes.
constexpr auto debugLevelPrefixWithColor =
    utility::common::concatString<escColorBlue, escFontBold, escBgColor, debugLevelPrefix, escOff>;
//! @brief Info level prefix with color. Include ANSI escape codes.
constexpr auto infoLevelPrefixWithColor =
    utility::common::concatString<escColorGreen, escFontBold, escBgColor, infoLevelPrefix, escOff>;
//! @brief Warning level prefix with color. Include ANSI escape codes.
constexpr auto warningLevelPrefixWithColor =
    utility::common::concatString<escColorYellow, escFontBold, escBgColor, warningLevelPrefix, escOff>;
//! @brief Error level prefix with color. Include ANSI escape codes.
constexpr auto errorLevelPrefixWithColor =
    utility::common::concatString<escColorRed, escFontBold, escBgColor, errorLevelPrefix, escOff>;
//! @brief Trace level prefix with color. Include ANSI escape codes.
constexpr auto traceLevelPrefixWithColor =
    utility::common::concatString<escFontInverse, escFontBold, escBgColor, traceLevelPrefix, escOff>;
//! @brief Base color of the date time. Include ANSI escape codes.
constexpr auto dateTimeBaseColor = utility::common::concatString<escFgColor, escFontBold, escFontDim, escBgColor>;
//! @brief Base color of the code file. Include ANSI escape codes.
constexpr auto codeFileBaseColor = utility::common::concatString<escFgColor, escFontBold, escFontUnderline, escBgColor>;
//! @brief Base color of the history cache. Include ANSI escape codes.
constexpr auto historyCacheBaseColor = utility::common::concatString<escFontInverse, escFontItalic, escBgColor>;

//! @brief Regular expressions for log highlighting.
struct HlRegex
{
    //! @brief Alternatives to predefined level prefix highlighting.
    const std::vector<std::pair<std::regex, std::string>> predefinedLevelPrefixes{
        {std::regex{std::string{debugLevelPrefixRegex}}, std::string{debugLevelPrefixWithColor}},
        {std::regex{std::string{infoLevelPrefixRegex}}, std::string{infoLevelPrefixWithColor}},
        {std::regex{std::string{warningLevelPrefixRegex}}, std::string{warningLevelPrefixWithColor}},
        {std::regex{std::string{errorLevelPrefixRegex}}, std::string{errorLevelPrefixWithColor}},
        {std::regex{std::string{traceLevelPrefixRegex}}, std::string{traceLevelPrefixWithColor}}};
    //! @brief Identity segments for basic highlighting (date time, code file, etc.).
    const std::vector<std::pair<std::regex, std::string>> identitySegments{
        {std::regex{std::string{dateTimeRegex}}, std::string{dateTimeBaseColor}},
        {std::regex{std::string{codeFileRegex}}, std::string{codeFileBaseColor}}};
};
} // namespace

//! @brief Log style.
static const HlRegex& logStyle()
{
    static const HlRegex highlight{};
    return highlight;
};

Log& Log::getInstance()
{
    if (configure::detail::activateHelper()) [[likely]]
    {
        static Log logger{};
        return logger;
    }
    throw std::logic_error{"The " + std::string{name} + " is disabled."};
}

// NOLINTBEGIN(cppcoreguidelines-avoid-goto)
void Log::service()
{
retry:
    try
    {
        logStyle();
        assert(currentState() == State::init);
        processEvent(OpenFile{});

        assert(currentState() == State::idle);
        awaitNotification2Ongoing();
        processEvent(GoLogging{});

        assert(currentState() == State::work);
        awaitNotification2Log();
        if (toReset.load())
        {
            processEvent(Relaunch{});
            goto retry;
        }
        processEvent(CloseFile{});

        assert(currentState() == State::idle);
        processEvent(NoLogging{});

        assert(currentState() == State::done);
    }
    catch (const std::exception& err)
    {
        LOG_ERR << "Suspend the " << name << " during " << State(currentState()) << " state. " << err.what();

        processEvent(Standby{});
        if (awaitNotification2Retry())
        {
            processEvent(Relaunch{});
            goto retry;
        }
    }
}
// NOLINTEND(cppcoreguidelines-avoid-goto)

void Log::Access::startup() const
try
{
    utility::time::blockingTimer(
        [this]()
        {
            if (inst.isInServingState(State::hold))
            {
                throw std::runtime_error{"The " + std::string{name} + " did not initialize successfully ..."};
            }
            return inst.isInServingState(State::idle);
        });

    if (std::unique_lock<std::mutex> daemonLock(inst.daemonMtx); true)
    {
        inst.ongoing.store(true);
        daemonLock.unlock();
        inst.daemonCond.notify_one();
    }

    utility::time::blockingTimer(
        [this]()
        {
            if (inst.isInServingState(State::hold))
            {
                throw std::runtime_error{"The " + std::string{name} + " did not start successfully ..."};
            }
            return inst.isInServingState(State::work);
        });
}
catch (const std::exception& err)
{
    LOG_ERR << err.what();
}

void Log::Access::shutdown() const
try
{
    if (std::unique_lock<std::mutex> daemonLock(inst.daemonMtx); true)
    {
        inst.ongoing.store(false);
        daemonLock.unlock();
        inst.daemonCond.notify_one();
    }

    utility::time::blockingTimer(
        [this]()
        {
            if (inst.isInServingState(State::hold))
            {
                throw std::runtime_error{"The " + std::string{name} + " did not stop successfully ..."};
            }
            return inst.isInServingState(State::done);
        });
}
catch (const std::exception& err)
{
    LOG_ERR << err.what();
}

void Log::Access::reload() const
try
{
    if (std::unique_lock<std::mutex> daemonLock(inst.daemonMtx); true)
    {
        inst.toReset.store(true);
        daemonLock.unlock();
        inst.daemonCond.notify_one();
    }

    if (utility::time::blockingTimer([this]() { return !inst.toReset.load(); }, inst.timeoutPeriod))
    {
        throw std::runtime_error{
            "The " + std::string{name} + " did not reset properly in " + std::to_string(inst.timeoutPeriod)
            + " ms ..."};
    }
}
catch (const std::exception& err)
{
    LOG_ERR << err.what();
}

void Log::flush(const OutputLevel severity, const std::string_view labelTpl, const std::string_view formatted)
{
    if (severity < priorityLevel)
    {
        return;
    }

    std::unique_lock<std::mutex> daemonLock(daemonMtx, std::defer_lock);
    try
    {
        if (auto rows = reformatContents(
                utility::common::formatString(
                    labelTpl,
                    isInServingState(State::work) ? (daemonLock.lock(), getPrefix(severity)) : traceLevelPrefix),
                formatted);
            daemonLock.owns_lock())
        {
            std::for_each(rows.begin(), rows.end(), [this](auto& output) { logQueue.push(std::move(output)); });
            daemonLock.unlock();
            daemonCond.notify_one();
        }
        else
        {
            cacheSwitch.lock();
            std::for_each(
                rows.begin(),
                rows.end(),
                [this](auto& output)
                {
                    unprocessedCache.emplace_front(output);
                    std::cerr << changeToLogStyle(output) << std::endl;
                });
            cacheSwitch.unlock();
        }
    }
    catch (...)
    {
        if (daemonLock.owns_lock())
        {
            daemonLock.unlock();
        }
        throw;
    }
}

std::string Log::createLabelTemplate(const std::string_view srcFile, const std::uint32_t srcLine)
{
    return std::format(
        "[{}] {{}} [{}#{}] ",
        utility::time::currentSystemTime(),
        (srcFile.rfind(sourceDirectory) != std::string_view::npos)
            ? srcFile.substr(srcFile.rfind(sourceDirectory) + sourceDirectory.length(), srcFile.length())
            : srcFile,
        srcLine);
}

std::string_view Log::getPrefix(const OutputLevel level)
{
    switch (level)
    {
        case OutputLevel::debug:
            return debugLevelPrefix;
        case OutputLevel::info:
            return infoLevelPrefix;
        case OutputLevel::warning:
            return warningLevelPrefix;
        case OutputLevel::error:
            return errorLevelPrefix;
        default:
            break;
    }

    return traceLevelPrefix;
}

std::vector<std::string> Log::reformatContents(const std::string_view label, const std::string_view formatted)
{
    std::vector<std::string> rows{};
    if (formatted.find('\n') == std::string_view::npos)
    {
        rows.emplace_back(formatted);
    }
    else
    {
        std::size_t pos = 0, prev = 0;
        while ((pos = formatted.find('\n', prev)) != std::string_view::npos)
        {
            rows.emplace_back(formatted.substr(prev, pos - prev + 1));
            prev += pos - prev + 1;
        }
        if (prev < formatted.length())
        {
            rows.emplace_back(formatted.substr(prev));
        }
    }
    auto reformatted =
        rows
        | std::views::transform(
            [](auto& line)
            {
                line.erase(
                    std::remove_if(line.begin(), line.end(), [](const auto c) { return (c == '\n') || (c == '\r'); }),
                    line.cend());
                return line;
            })
        | std::views::filter([](const auto& line) { return !line.empty(); })
        | std::views::transform([&label](const auto& line) { return label.data() + line; });

    return std::vector<std::string>{std::ranges::begin(reformatted), std::ranges::end(reformatted)};
}

bool Log::isInServingState(const State state) const
{
    return (currentState() == state) && !toReset.load();
}

std::string Log::getFullLogPath(const std::string_view filename)
{
    const char* const processHome = std::getenv("FOO_HOME"); // NOLINT(concurrency-mt-unsafe)
    if (!processHome)
    {
        throw std::runtime_error{"The environment variable FOO_HOME is not set."};
    }

    return std::string{processHome} + '/' + filename.data();
}

void Log::tryCreateLogFolder() const
{
    if (const auto logFolderPath = std::filesystem::absolute(filePath).parent_path();
        !std::filesystem::exists(logFolderPath))
    {
        std::filesystem::create_directories(logFolderPath);
        std::filesystem::permissions(
            logFolderPath, std::filesystem::perms::owner_all, std::filesystem::perm_options::add);
    }
}

void Log::backUpLogFileIfNeeded() const
{
    if (constexpr std::uint32_t maxFileSize = 512 * 1024;
        !std::filesystem::is_regular_file(filePath) || (std::filesystem::file_size(filePath) < maxFileSize))
    {
        return;
    }

    const std::regex pattern(
        std::regex_replace(
            std::filesystem::path(filePath).filename().string(), std::regex(R"([-[\]{}()*+?.,\^$|#\s])"), R"(\$&)")
        + R"(\.(\d+))");
    auto transformed = std::filesystem::directory_iterator(std::filesystem::absolute(filePath).parent_path())
        | std::views::transform(
                           [&pattern, match = std::smatch{}](const auto& entry) mutable
                           {
                               const auto& filename = entry.path().filename().string();
                               return std::regex_match(filename, match, pattern) ? std::stoi(match[1].str()) : 0;
                           });
    const int index = std::ranges::max(transformed, std::less<int>{}, [](const auto value) { return value; });
    std::filesystem::rename(filePath, filePath + '.' + std::to_string(index + 1));
}

void Log::openLogFile()
{
    const utility::common::LockGuard guard(fileLock, LockMode::write);
    tryCreateLogFolder();
    backUpLogFileIfNeeded();
    switch (writeMode)
    {
        case OutputMode::append:
            logWriter.open();
            break;
        case OutputMode::overwrite:
            logWriter.open(true);
            break;
        default:
            break;
    }
};

void Log::closeLogFile()
{
    const utility::common::LockGuard guard(fileLock, LockMode::write);
    logWriter.unlock();
    logWriter.close();
}

void Log::startLogging()
{
    logWriter.lock();

    cacheSwitch.lock();
    while (!unprocessedCache.empty())
    {
        std::cout << historyCacheBaseColor.data() + unprocessedCache.front() + escOff.data() << std::endl;
        unprocessedCache.pop_front();
    }
    cacheSwitch.unlock();
}

void Log::stopLogging()
{
    const std::lock_guard<std::mutex> daemonLock(daemonMtx);
    ongoing.store(false);
    toReset.store(false);

    while (!logQueue.empty())
    {
        logQueue.pop();
    }
}

void Log::doToggle()
{
}

void Log::doRollback()
{
    const std::lock_guard<std::mutex> daemonLock(daemonMtx);
    ongoing.store(false);

    while (!logQueue.empty())
    {
        logQueue.pop();
    }
    if (logWriter.isOpen())
    {
        try
        {
            logWriter.lock();
        }
        catch (...)
        {
            toReset.store(false);
            return;
        }

        closeLogFile();
        tryCreateLogFolder();
        backUpLogFileIfNeeded();
        std::ofstream tempOfs{};
        tempOfs.open(filePath, std::ios_base::out | std::ios_base::trunc);
        tempOfs.close();
    }

    toReset.store(false);
}

bool Log::isLogFileOpen(const GoLogging& /*event*/) const
{
    return logWriter.isOpen();
}

bool Log::isLogFileClose(const NoLogging& /*event*/) const
{
    return !logWriter.isOpen();
}

void Log::awaitNotification2Ongoing()
{
    if (std::unique_lock<std::mutex> daemonLock(daemonMtx); true)
    {
        daemonCond.wait(daemonLock, [this]() { return ongoing.load(); });
    }
}

void Log::awaitNotification2Log()
{
    while (ongoing.load())
    {
        std::unique_lock<std::mutex> daemonLock(daemonMtx);
        daemonCond.wait(daemonLock, [this]() { return !ongoing.load() || !logQueue.empty() || toReset.load(); });
        if (toReset.load())
        {
            break;
        }

        const utility::common::LockGuard guard(fileLock, LockMode::write);
        while (!logQueue.empty())
        {
            switch (targetType)
            {
                case OutputType::file:
                    logWriter.stream() << logQueue.front() << std::endl;
                    break;
                case OutputType::terminal:
                    std::cout << changeToLogStyle(logQueue.front()) << std::endl;
                    break;
                case OutputType::all:
                    logWriter.stream() << logQueue.front() << std::endl;
                    std::cout << changeToLogStyle(logQueue.front()) << std::endl;
                    break;
                default:
                    break;
            }
            logQueue.pop();
        }
    }
}

bool Log::awaitNotification2Retry()
{
    if (std::unique_lock<std::mutex> daemonLock(daemonMtx); true)
    {
        daemonCond.wait(daemonLock);
    }

    return toReset.load();
}

//! @brief The operator (<<) overloading of the State enum.
//! @param os - output stream object
//! @param state - specific value of State enum
//! @return reference of the output stream object
std::ostream& operator<<(std::ostream& os, const Log::State state)
{
    using enum Log::State;
    switch (state)
    {
        case init:
            os << "INIT";
            break;
        case idle:
            os << "IDLE";
            break;
        case work:
            os << "WORK";
            break;
        case done:
            os << "DONE";
            break;
        case hold:
            os << "HOLD";
            break;
        default:
            os << "UNKNOWN (" << static_cast<std::underlying_type_t<Log::State>>(state) << ')';
            break;
    }

    return os;
}

//! @brief Change line string to log style.
//! @param line - target line to be changed
//! @return changed line
std::string& changeToLogStyle(std::string& line)
{
    const auto& style = logStyle();
    if (const auto segIter = std::find_if(
            style.predefinedLevelPrefixes.cbegin(),
            style.predefinedLevelPrefixes.cend(),
            [&line](const auto& predefined) { return std::regex_search(line, std::get<std::regex>(predefined)); });
        segIter != style.predefinedLevelPrefixes.cend())
    {
        line = std::regex_replace(line, std::get<std::regex>(*segIter), std::get<std::string>(*segIter));
    }

    for (std::smatch match{}; [[maybe_unused]] const auto& [segment, scheme] : style.identitySegments)
    {
        if (std::regex_search(line, match, segment))
        {
            line = match.prefix().str() + scheme + match.str() + escOff.data() + match.suffix().str();
        }
    }

    return line;
}
} // namespace application::log
