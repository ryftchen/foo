//! @file log.cpp
//! @author ryftchen
//! @brief The definitions (log) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "log.hpp"

#ifndef __PRECOMPILED_HEADER
#include <cassert>
#include <filesystem>
#include <fstream>
#include <ranges>
#include <regex>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

namespace application::log
{
//! @brief Anonymous namespace.
inline namespace
{
//! @brief Regular expressions for log highlighting.
struct HlRegex
{
    //! @brief Alternatives to predefined level prefix highlighting.
    const std::vector<std::pair<std::regex, std::string>> predefinedLevelPrefixes{
        {std::regex{debugLevelPrefixRegex.data()}, std::string{debugLevelPrefixWithColor}},
        {std::regex{infoLevelPrefixRegex.data()}, std::string{infoLevelPrefixWithColor}},
        {std::regex{warningLevelPrefixRegex.data()}, std::string{warningLevelPrefixWithColor}},
        {std::regex{errorLevelPrefixRegex.data()}, std::string{errorLevelPrefixWithColor}},
        {std::regex{traceLevelPrefixRegex.data()}, std::string{traceLevelPrefixWithColor}}};
    //! @brief Identity segments for basic highlighting (date time, code file, etc.).
    const std::vector<std::pair<std::regex, std::string>> identitySegments{
        {std::regex{dateTimeRegex.data()}, std::string{dateTimeBaseColor}},
        {std::regex{codeFileRegex.data()}, std::string{codeFileBaseColor}}};
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

void Log::service()
{
retry:
    try
    {
        static_cast<void>(logStyle());
        assert(safeCurrentState() == State::init);
        safeProcessEvent(OpenFile{});

        assert(safeCurrentState() == State::idle);
        awaitNotification2Ongoing();
        safeProcessEvent(GoLogging{});

        assert(safeCurrentState() == State::work);
        awaitNotification2Log();
        if (toReset.load())
        {
            safeProcessEvent(Relaunch{});
            goto retry; // NOLINT(cppcoreguidelines-avoid-goto,hicpp-avoid-goto)
        }
        safeProcessEvent(CloseFile{});

        assert(safeCurrentState() == State::idle);
        safeProcessEvent(NoLogging{});

        assert(safeCurrentState() == State::done);
    }
    catch (const std::exception& err)
    {
        LOG_ERR << "Suspend the " << name << " during " << safeCurrentState() << " state. " << err.what();

        safeProcessEvent(Standby{});
        if (awaitNotification2Retry())
        {
            safeProcessEvent(Relaunch{});
            goto retry; // NOLINT(cppcoreguidelines-avoid-goto,hicpp-avoid-goto)
        }
    }
}

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
            const std::lock_guard<std::recursive_mutex> cacheLock(cacheMtx);
            std::for_each(
                rows.begin(),
                rows.end(),
                [this](auto& output)
                {
                    unprocessedCache.emplace_front(output);
                    std::cerr << changeToLogStyle(output) << std::endl;
                });
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
        (std::string_view::npos != srcFile.rfind(sourceDirectory))
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
    if (std::string_view::npos == formatted.find('\n'))
    {
        rows.emplace_back(formatted);
    }
    else
    {
        std::size_t pos = 0, prev = 0;
        while (std::string_view::npos != (pos = formatted.find('\n', prev)))
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
                    std::remove_if(line.begin(), line.end(), [](const auto c) { return ('\n' == c) || ('\r' == c); }),
                    line.cend());
                return line;
            })
        | std::views::filter([](const auto& line) { return !line.empty(); })
        | std::views::transform([&label](const auto& line) { return label.data() + line; });

    return std::vector<std::string>{std::ranges::begin(reformatted), std::ranges::end(reformatted)};
}

Log::State Log::safeCurrentState() const
{
    stateLock.lock();
    const auto state = State(currentState());
    stateLock.unlock();

    return state;
}

template <typename T>
void Log::safeProcessEvent(const T& event)
{
    stateLock.lock();
    try
    {
        processEvent(event);
    }
    catch (...)
    {
        stateLock.unlock();
        throw;
    }
    stateLock.unlock();
}

bool Log::isInServingState(const State state) const
{
    return (safeCurrentState() == state) && !toReset.load();
}

std::string Log::getFullLogPath(const std::string_view filename)
{
    const char* const processHome = std::getenv("FOO_HOME");
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
    utility::common::LockGuard guard(fileLock, LockMode::write);
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
    utility::common::LockGuard guard(fileLock, LockMode::write);
    logWriter.unlock();
    logWriter.close();
}

void Log::startLogging()
{
    logWriter.lock();

    const std::lock_guard<std::recursive_mutex> cacheLock(cacheMtx);
    while (!unprocessedCache.empty())
    {
        std::cout << historyCacheBaseColor.data() + unprocessedCache.front() + utility::common::escOff.data()
                  << std::endl;
        unprocessedCache.pop_front();
    }
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

        utility::common::LockGuard guard(fileLock, LockMode::write);
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
//! @param state - the specific value of State enum
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
        style.predefinedLevelPrefixes.cend() != segIter)
    {
        line = std::regex_replace(line, std::get<std::regex>(*segIter), std::get<std::string>(*segIter));
    }

    for (std::smatch match{}; [[maybe_unused]] const auto& [segment, scheme] : style.identitySegments)
    {
        if (std::regex_search(line, match, segment))
        {
            line = match.prefix().str() + scheme + match.str() + utility::common::escOff.data() + match.suffix().str();
        }
    }

    return line;
}
} // namespace application::log
