#pragma once

#include <readline/history.h>
#include <functional>
#include <memory>

namespace util_console
{
class Console
{
public:
    enum ReturnCode : int
    {
        quit = -1,
        success = 0,
        error = 1
    };

    explicit Console(const std::string& greeting);
    virtual ~Console();
    Console(const Console&) = delete;
    Console(Console&&) = delete;
    Console& operator=(Console const&) = delete;
    Console& operator=(Console&&) = delete;

    using Args = std::vector<std::string>;
    using CommandFunction = std::function<int(const Args&)>;
    void registerCommand(const std::string& command, CommandFunction func, const std::string& help);
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> getHelpOfRegisteredCommands() const;
    void setGreeting(const std::string& greeting);
    [[nodiscard]] std::string getGreeting() const;
    int commandExecutor(const std::string& command);
    int fileExecutor(const std::string& filename);
    int readCommandLine();

private:
    using HistoryState = HISTORY_STATE;
    HistoryState* emptyHistory = history_get_history_state();

    struct Impl
    {
        explicit Impl(const std::string& greeting) : greeting(greeting), RegCmds() {}
        ~Impl() { delete history; }
        Impl(const Impl&) = delete;
        Impl(Impl&&) = delete;
        Impl& operator=(const Impl&) = delete;
        Impl& operator=(Impl&&) = delete;

        std::string greeting;
        using RegisteredCommands = std::unordered_map<std::string, std::pair<CommandFunction, std::string>>;
        RegisteredCommands RegCmds;
        HistoryState* history{nullptr};
    };
    std::unique_ptr<Impl> impl;

    void saveState();
    void reserveConsole();

    using CommandCompleterFunction = char**(const char* text, int start, int end);
    using CommandIteratorFunction = char*(const char* text, int state);
    static CommandCompleterFunction getCommandCompletions;
    static CommandIteratorFunction commandIterator;
};
} // namespace util_console
