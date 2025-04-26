//! @file console.cpp
//! @author ryftchen
//! @brief The definitions (console) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "console.hpp"
#include "note.hpp"

#ifndef _PRECOMPILED_HEADER
#include <readline/readline.h>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <ranges>
#else
#include "application/pch/precompiled_header.hpp"
#endif // _PRECOMPILED_HEADER

namespace application::console
{
//! @brief Anonymous namespace.
inline namespace
{
//! @brief Current console instance.
thread_local constinit Console* currentSession = nullptr;
} // namespace

Console::Console(const std::string_view greeting) : terminal{std::make_unique<Terminal>(greeting)}
{
    ::rl_attempted_completion_function = &Console::customCompleter;
    setDefaultOptions();
}

Console::~Console()
{
    ::rl_attempted_completion_function = nullptr;
    ::rl_free(emptyHistory);
    ::rl_clear_history();
    ::rl_set_prompt(nullptr);
    ::rl_restore_prompt();
}

void Console::registerOption(const std::string_view name, const std::string_view description, Callback callable)
{
    terminal->regTable[name.data()] = std::make_pair(description, std::move(callable));
    terminal->orderList.emplace_back(name);
}

void Console::setGreeting(const std::string_view greeting)
{
    terminal->greeting = greeting;
}

Console::RetCode Console::optionExecutor(const std::string_view option) const
{
    std::vector<std::string> inputs{};
    std::istringstream transfer(option.data());
    std::copy(
        std::istream_iterator<std::string>{transfer}, std::istream_iterator<std::string>{}, std::back_inserter(inputs));
    if (inputs.empty())
    {
        return RetCode::success;
    }

    const auto regIter = terminal->regTable.find(inputs.front());
    if (terminal->regTable.cend() == regIter)
    {
        throw std::runtime_error{
            "The console option (" + inputs.front() + ") could not be found. Enter the \"usage\" for help."};
    }

    return std::get<Callback>(regIter->second)(inputs);
}

Console::RetCode Console::fileExecutor(const std::string_view filename) const
{
    std::ifstream batch(filename.data());
    if (!batch)
    {
        throw std::runtime_error{"Could not find the batch file to run."};
    }

    std::string input{};
    std::uint32_t counter = 0;
    std::ostringstream out{};
    while (std::getline(batch, input))
    {
        if (input.empty() || ('#' == input.front()))
        {
            continue;
        }

        ++counter;
        out << '#' << counter << ' ' << input << '\n';
        std::cout << out.str() << std::flush;
        out.str("");
        out.clear();
        if (const auto result = optionExecutor(input); RetCode::success != result)
        {
            return result;
        }
        std::cout << std::endl;
    }

    return RetCode::success;
}

Console::RetCode Console::readLine()
{
    reserveConsole();
    char* const buffer = ::readline(terminal->greeting.c_str());
    if (!buffer)
    {
        std::cout << std::endl;
        return RetCode::quit;
    }

    if ('\0' != buffer[0])
    {
        ::add_history(buffer);
    }
    const auto input = std::string{buffer};
    ::rl_free(buffer);

    return optionExecutor(input);
}

auto Console::getOptionHelpPairs() const
{
    const auto transformed =
        terminal->orderList
        | std::views::transform(
            [this](const auto& option)
            { return std::make_pair(option, std::get<std::string>(terminal->regTable.at(option))); });
    return std::vector<std::ranges::range_value_t<decltype(transformed)>>{transformed.begin(), transformed.end()};
}

void Console::setDefaultOptions()
{
    auto& regTable = terminal->regTable;
    auto& orderList = terminal->orderList;

    regTable["usage"] = std::make_pair(
        "show help message",
        [this](const Args& /*inputs*/)
        {
            const auto pairs = getOptionHelpPairs();
            const auto align =
                std::ranges::max(pairs, std::less<std::size_t>{}, [](const auto& pair) { return pair.first.length(); })
                    .first.length();
            std::ostringstream out{};
            out << std::setiosflags(std::ios_base::left);
            for (const auto& [option, help] : pairs)
            {
                out << "- " << std::setw(align) << option << "    " << help << '\n';
            }
            out << std::resetiosflags(std::ios_base::left);
            std::cout << out.str() << std::flush;
            return RetCode::success;
        });
    orderList.emplace_back("usage");

    regTable["quit"] = std::make_pair(
        "exit the console",
        [](const Args& /*inputs*/)
        {
            std::cout << "exit" << std::endl;
            return RetCode::quit;
        });
    orderList.emplace_back("quit");

    regTable["trace"] = std::make_pair(
        "get history of applied options",
        [](const Args& /*inputs*/)
        {
            if (const auto* const* const historyList = ::history_list())
            {
                const auto align = std::to_string(::history_length).length() + 1;
                std::ostringstream out{};
                out << std::setiosflags(std::ios_base::right);
                for (std::uint32_t index = 0;
                     const auto& history : std::span{historyList, static_cast<std::size_t>(::history_length)})
                {
                    out << std::setw(align) << (index++) + ::history_base << "  " << history->line << '\n';
                }
                out << std::resetiosflags(std::ios_base::right);
                std::cout << out.str() << std::flush;
            }
            else
            {
                std::cout << "no history" << std::endl;
            }
            return RetCode::success;
        });
    orderList.emplace_back("trace");

    regTable["clean"] = std::make_pair(
        "clear full screen",
        [](const Args& /*inputs*/)
        {
            std::cout << "\033[2J\033[1;1H" << std::flush;
            std::cout << note::icon() << std::endl;
            return RetCode::success;
        });
    orderList.emplace_back("clean");

    regTable["batch"] = std::make_pair(
        "run lines from the file [inputs: FILE]",
        [this](const Args& inputs)
        {
            if (inputs.size() < 2)
            {
                throw std::runtime_error{"Please enter the \"" + inputs.at(0) + "\" and append with FILE (full path)."};
            }
            return fileExecutor(inputs.at(1));
        });
    orderList.emplace_back("batch");
}

void Console::saveState()
{
    ::rl_free(terminal->history);
    terminal->history = ::history_get_history_state();
}

void Console::reserveConsole()
{
    if (this == currentSession)
    {
        return;
    }

    if (currentSession)
    {
        currentSession->saveState();
    }
    ::history_set_history_state(terminal->history ? terminal->history : emptyHistory);
    currentSession = this;
}

char** Console::customCompleter(const char* text, int start, int /*end*/)
{
    return (0 == start) ? ::rl_completion_matches(text, &Console::customCompentry) : nullptr;
}

char* Console::customCompentry(const char* text, int state)
{
    static thread_local Terminal::RegisteredOption::iterator optionIterator{};
    if (!currentSession)
    {
        return nullptr;
    }

    auto& regTable = currentSession->terminal->regTable;
    if (0 == state)
    {
        optionIterator = regTable.begin();
    }

    while (regTable.end() != optionIterator)
    {
        const auto& option = optionIterator->first;
        ++optionIterator;
        if (const std::string_view input = text; option.compare(0, input.length(), input) == 0)
        {
            return ::strdup(option.c_str());
        }
    }

    return nullptr;
}
} // namespace application::console
