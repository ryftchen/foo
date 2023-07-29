//! @file console.hpp
//! @author ryftchen
//! @brief The declarations (console) in the utility module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen. All rights reserved.

#pragma once

#include <readline/history.h>
#include <functional>
#include <memory>

//! @brief Console-mode-related functions in the utility module.
namespace utility::console
{
//! @brief Console mode.
class Console
{
public:
    //! @brief Construct a new Console object.
    //! @param greeting - default greeting information
    explicit Console(const std::string& greeting);
    //! @brief Destroy the Console object.
    virtual ~Console();
    //! @brief Construct a new Console object.
    Console(const Console&) = delete;
    //! @brief Construct a new Console object.
    Console(Console&&) = delete;
    //! @brief The operator (=) overloading of Console class.
    //! @return reference of Console object
    Console& operator=(const Console&) = delete;
    //! @brief The operator (=) overloading of Console class.
    //! @return reference of Console object
    Console& operator=(Console&&) = delete;

    //! @brief Alias for the container of arguments.
    using Args = std::vector<std::string>;
    //! @brief Alias for the functor of command.
    using CommandFunctor = std::function<int(const Args&)>;
    //! @brief Register console command.
    //! @param command - command to be registered
    //! @param func - callable function
    //! @param help - help message
    void registerCommand(const std::string& command, CommandFunctor func, const std::string& help);
    //! @brief Get help messages from all registered commands.
    //! @return all help messages
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> getHelpOfRegisteredCommands() const;
    //! @brief Set greeting information.
    //! @param greeting - greeting information
    void setGreeting(const std::string& greeting);
    //! @brief Get greeting information.
    //! @return greeting information
    [[nodiscard]] std::string getGreeting() const;
    //! @brief Execute the target console command.
    //! @param command - command to be executed
    //! @return value of ReturnCode
    int commandExecutor(const std::string& command);
    //! @brief Execute all console commands in the target file.
    //! @param filename - file to be executed
    //! @return value of ReturnCode
    int fileExecutor(const std::string& filename);
    //! @brief Read console command line.
    //! @return value of ReturnCode
    int readCommandLine();

    //! @brief Enumerate specific return code.
    enum ReturnCode : int
    {
        quit = -1,
        success = 0,
        error = 1
    };

private:
    //! @brief Alias for the history state.
    using HistoryState = ::HISTORY_STATE;
    //! @brief Saved empty history state.
    HistoryState* emptyHistory = ::history_get_history_state();

    //! @brief Implementation of running console.
    struct Impl
    {
        //! @brief Construct a new Impl object.
        //! @param greeting - default greeting information
        explicit Impl(const std::string& greeting) : greeting(greeting), RegCmds() {}
        //! @brief Destroy the Impl object.
        ~Impl() { delete history; }
        //! @brief Construct a new Impl object.
        Impl(const Impl&) = delete;
        //! @brief Construct a new Impl object.
        Impl(Impl&&) = delete;
        //! @brief The operator (=) overloading of Impl class.
        //! @return reference of Impl object
        Impl& operator=(const Impl&) = delete;
        //! @brief The operator (=) overloading of Impl class.
        //! @return reference of Impl object
        Impl& operator=(Impl&&) = delete;

        //! @brief Greeting information.
        std::string greeting;
        //! @brief Alias for the map of command and function in console.
        using RegisteredCommands = std::unordered_map<std::string, std::pair<CommandFunctor, std::string>>;
        //! @brief Mapping table of all registered commands.
        RegisteredCommands RegCmds;
        //! @brief Saved history state.
        HistoryState* history{nullptr};
    };
    //! @brief Implementation instance.
    std::unique_ptr<Impl> impl;

    //! @brief Save current state.
    void saveState();
    //! @brief Reserve usage to the calling console instance.
    void reserveConsole();

    //! @brief Alias for the functor of command completer.
    using CommandCompleterFunctor = char**(const char* text, int start, int end);
    //! @brief Alias for the functor of command iterator.
    using CommandIteratorFunctor = char*(const char* text, int state);
    //! @brief Get the command completer. Wrap the interface.
    static CommandCompleterFunctor getCommandCompleter;
    //! @brief Get the command iterator.
    static CommandIteratorFunctor getCommandIterator;
};
} // namespace utility::console
