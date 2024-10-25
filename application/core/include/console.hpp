//! @file console.hpp
//! @author ryftchen
//! @brief The declarations (console) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#pragma once

#ifndef __PRECOMPILED_HEADER
#include <readline/history.h>
#include <functional>
#include <list>
#include <memory>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

//! @brief The application module.
namespace application // NOLINT (modernize-concat-nested-namespaces)
{
//! @brief Console-mode-related functions in the application module.
namespace console
{
//! @brief Console mode.
class Console
{
public:
    //! @brief Construct a new Console object.
    //! @param greeting - default greeting information
    explicit Console(const std::string_view greeting);
    //! @brief Destroy the Console object.
    virtual ~Console();
    //! @brief Construct a new Console object.
    Console(const Console&) = delete;
    //! @brief Construct a new Console object.
    Console(Console&&) = delete;
    //! @brief The operator (=) overloading of Console class.
    //! @return reference of the Console object
    Console& operator=(const Console&) = delete;
    //! @brief The operator (=) overloading of Console class.
    //! @return reference of the Console object
    Console& operator=(Console&&) = delete;

    //! @brief Alias for the container of arguments.
    using Args = std::vector<std::string>;
    //! @brief Alias for the functor of command.
    using CommandFunctor = std::function<int(const Args&)>;
    //! @brief Register console command.
    //! @param name - command name
    //! @param func - callable function
    //! @param help - help message
    void registerCommand(const std::string_view name, const CommandFunctor& func, const std::string_view help);
    //! @brief Set greeting information.
    //! @param greeting - greeting information
    void setGreeting(const std::string_view greeting);
    //! @brief Get greeting information.
    //! @return greeting information
    [[nodiscard]] std::string getGreeting() const;
    //! @brief Execute the target console command.
    //! @param command - command to be executed
    //! @return value of RetCode
    int commandExecutor(const std::string_view command);
    //! @brief Execute all console commands in the target file.
    //! @param filename - file to be executed
    //! @return value of RetCode
    int fileExecutor(const std::string_view filename);
    //! @brief Read console command line.
    //! @return value of RetCode
    int readCommandLine();

    //! @brief Enumerate specific return code.
    enum RetCode : int
    {
        //! @brief Quit.
        quit = -1,
        //! @brief Success.
        success = 0,
        //! @brief Error.
        error = 1
    };

private:
    //! @brief Alias for the history state.
    using HistoryState = ::HISTORY_STATE;
    //! @brief Alias for the command name.
    using Command = std::string;
    //! @brief Alias for the help message.
    using Help = std::string;
    //! @brief Alias for command help pair.
    using CommandHelpPair = std::pair<Command, Help>;
    //! @brief Alias for the functor of command completer.
    using CommandCompleterFunctor = char**(const char* text, int start, int end);
    //! @brief Alias for the functor of command iterator.
    using CommandIteratorFunctor = char*(const char* text, int state);

    //! @brief Saved empty history state.
    HistoryState* const emptyHistory{::history_get_history_state()};
    //! @brief Implementation of running console.
    struct Impl
    {
        //! @brief Construct a new Impl object.
        //! @param greeting - default greeting information
        explicit Impl(const std::string_view greeting) : greeting(greeting), regMap() {}
        //! @brief Destroy the Impl object.
        ~Impl() { delete history; }
        //! @brief Construct a new Impl object.
        Impl(const Impl&) = delete;
        //! @brief Construct a new Impl object.
        Impl(Impl&&) = delete;
        //! @brief The operator (=) overloading of Impl struct.
        //! @return reference of the Impl object
        Impl& operator=(const Impl&) = delete;
        //! @brief The operator (=) overloading of Impl struct.
        //! @return reference of the Impl object
        Impl& operator=(Impl&&) = delete;

        //! @brief Alias for the map of command and function in console.
        using RegisteredCommand = std::unordered_map<Command, std::pair<CommandFunctor, Help>>;

        //! @brief Greeting information.
        std::string greeting{};
        //! @brief Mapping table of all registered commands.
        RegisteredCommand regMap{};
        //! @brief Register order.
        std::list<Command> regOrder{};
        //! @brief Saved history state.
        HistoryState* history{nullptr};
    };
    //! @brief Implementation instance.
    std::unique_ptr<Impl> impl{};

    //! @brief Get help messages for all registered commands.
    //! @return help messages for all registered commands
    [[nodiscard]] std::vector<CommandHelpPair> getHelpOfRegisteredCommand() const;
    //! @brief Save current state.
    void saveState();
    //! @brief Reserve usage to the calling console instance.
    void reserveConsole();

    //! @brief Get the command completer. Wrap the interface.
    static CommandCompleterFunctor getCommandCompleter;
    //! @brief Get the command iterator.
    static CommandIteratorFunctor getCommandIterator;
};
} // namespace console
} // namespace application
