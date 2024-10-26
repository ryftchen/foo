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
    //! @brief Alias for the functor of option.
    using OptionFunctor = std::function<int(const Args&)>;
    //! @brief Register console option.
    //! @param name - option name
    //! @param func - callable function
    //! @param prompt - help prompt
    void registerOption(const std::string_view name, const OptionFunctor& func, const std::string_view prompt);
    //! @brief Set greeting information.
    //! @param greeting - greeting information
    void setGreeting(const std::string_view greeting);
    //! @brief Get greeting information.
    //! @return greeting information
    [[nodiscard]] std::string getGreeting() const;
    //! @brief Execute the target console option.
    //! @param option - option to be executed
    //! @return value of RetCode
    int optionExecutor(const std::string_view option);
    //! @brief Execute all console options in the target file.
    //! @param filename - file to be executed
    //! @return value of RetCode
    int fileExecutor(const std::string_view filename);
    //! @brief Read console option line.
    //! @return value of RetCode
    int readLine();

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
    //! @brief Alias for the option name.
    using Option = std::string;
    //! @brief Alias for the help prompt.
    using Help = std::string;
    //! @brief Alias for option help pair.
    using OptionHelpPair = std::pair<Option, Help>;
    //! @brief Alias for the functor of option completer.
    using OptionCompleterFunctor = char**(const char* text, int start, int end);
    //! @brief Alias for the functor of option iterator.
    using OptionIteratorFunctor = char*(const char* text, int state);

    //! @brief Saved empty history state.
    HistoryState* const emptyHistory{::history_get_history_state()};
    //! @brief Implementation of running console.
    struct Impl
    {
        //! @brief Construct a new Impl object.
        //! @param greeting - default greeting information
        explicit Impl(const std::string_view greeting) : greeting(greeting) {}
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

        //! @brief Alias for the map of option and function in console.
        using RegisteredOption = std::unordered_map<Option, std::pair<OptionFunctor, Help>>;

        //! @brief Greeting information.
        std::string greeting{};
        //! @brief Mapping table of all registered options.
        RegisteredOption regTable{};
        //! @brief Register order.
        std::list<Option> orderList{};
        //! @brief Saved history state.
        HistoryState* history{nullptr};
    };
    //! @brief Implementation instance.
    std::unique_ptr<Impl> impl{};

    //! @brief Get all registered options with help prompts.
    //! @return all registered options with help prompts
    [[nodiscard]] std::vector<OptionHelpPair> getOptionHelpPairs() const;
    //! @brief Save current state.
    void saveState();
    //! @brief Reserve usage to the calling console instance.
    void reserveConsole();

    //! @brief Get the option completer. Wrap the interface.
    static OptionCompleterFunctor getOptionCompleter;
    //! @brief Get the option iterator. Wrap the interface.
    static OptionIteratorFunctor getOptionIterator;
};
} // namespace console
} // namespace application
