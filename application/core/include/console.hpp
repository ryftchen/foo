//! @file console.hpp
//! @author ryftchen
//! @brief The declarations (console) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

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
namespace application // NOLINT(modernize-concat-nested-namespaces)
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

    //! @brief Enumerate specific return code.
    enum class RetCode : int
    {
        //! @brief Quit.
        quit = -1,
        //! @brief Success.
        success = 0,
        //! @brief Error.
        error = 1
    };
    //! @brief Alias for the container of arguments.
    using Args = std::vector<std::string>;
    //! @brief Alias for the callback of option.
    using Callback = std::function<RetCode(const Args&)>;
    //! @brief Register console option.
    //! @param name - option name
    //! @param prompt - help prompt
    //! @param func - callable function
    void registerOption(const std::string_view name, const std::string_view prompt, Callback func);
    //! @brief Set greeting information.
    //! @param greeting - greeting information
    void setGreeting(const std::string_view greeting);
    //! @brief Execute the target console option.
    //! @param option - option to be executed
    //! @return value of RetCode
    [[nodiscard]] RetCode optionExecutor(const std::string_view option) const;
    //! @brief Execute all console options in the target file.
    //! @param filename - file path to be executed
    //! @return value of RetCode
    [[nodiscard]] RetCode fileExecutor(const std::string_view filename) const;
    //! @brief Read console option line.
    //! @return value of RetCode
    RetCode readLine();

private:
    //! @brief Alias for the history state.
    using HistoryState = ::HISTORY_STATE;
    //! @brief Saved empty history state.
    HistoryState* const emptyHistory{::history_get_history_state()};
    //! @brief Terminal for interaction.
    class Terminal
    {
    public:
        //! @brief Construct a new Terminal object.
        //! @param greeting - default greeting information
        explicit Terminal(const std::string_view greeting) : greeting{greeting} {}
        //! @brief Destroy the Terminal object.
        virtual ~Terminal() { delete history; }
        //! @brief Construct a new Terminal object.
        Terminal(const Terminal&) = delete;
        //! @brief Construct a new Terminal object.
        Terminal(Terminal&&) = delete;
        //! @brief The operator (=) overloading of Terminal struct.
        //! @return reference of the Terminal object
        Terminal& operator=(const Terminal&) = delete;
        //! @brief The operator (=) overloading of Terminal struct.
        //! @return reference of the Terminal object
        Terminal& operator=(Terminal&&) = delete;

        //! @brief Greeting information.
        std::string greeting{};
        //! @brief Alias for the map of option and callback in console.
        using RegisteredOption = std::unordered_map<std::string, std::pair<std::string, Callback>>;
        //! @brief Mapping table of all registered options.
        RegisteredOption regTable{};
        //! @brief Register order.
        std::list<std::string> orderList{};
        //! @brief Saved history state.
        HistoryState* history{nullptr};
    };
    //! @brief Internal terminal.
    const std::unique_ptr<Terminal> terminal{};

    //! @brief Get all registered options with help prompts.
    //! @return all registered options with help prompts
    [[nodiscard]] auto getOptionHelpPairs() const;
    //! @brief Set the default options.
    void setDefaultOptions();
    //! @brief Save current state.
    void saveState();
    //! @brief Reserve usage to the calling console instance.
    void reserveConsole();

    //! @brief Alias for the functor of option completer.
    using OptionCompleterFunctor = char**(const char*, int, int);
    //! @brief Alias for the functor of option compentry.
    using OptionCompentryFunctor = char*(const char*, int);
    //! @brief Get the option completer. Wrap the interface.
    static OptionCompleterFunctor customCompleter;
    //! @brief Get the option compentry. Wrap the interface.
    static OptionCompentryFunctor customCompentry;
};
} // namespace console
} // namespace application
