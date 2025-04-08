//! @file console.cpp
//! @author ryftchen
//! @brief The definitions (console) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "console.hpp"

#ifndef __PRECOMPILED_HEADER
#include <readline/readline.h>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <ranges>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

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
    ::rl_attempted_completion_function = &Console::getOptionCompleter;
    setDefaultOptions();
}

Console::~Console()
{
    ::rl_free(emptyHistory);
    ::rl_clear_history();
    ::rl_restore_prompt();
}

void Console::registerOption(const std::string_view name, const std::string_view prompt, Callback func)
{
    terminal->regTable[name.data()] = std::make_pair(prompt, std::move(func));
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
        std::istream_iterator<std::string>(transfer), std::istream_iterator<std::string>{}, std::back_inserter(inputs));
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

    return regIter->second.second(inputs);
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
    std::string input(buffer);
    ::rl_free(buffer);

    return optionExecutor(input);
}

auto Console::getOptionHelpPairs() const
{
    const auto transformed = terminal->orderList
        | std::views::transform([this](const auto& option)
                                { return std::make_pair(option, terminal->regTable.at(option).first); });
    return std::vector<std::ranges::range_value_t<decltype(transformed)>>{transformed.begin(), transformed.end()};
}

void Console::setDefaultOptions()
{
    auto& regTable = terminal->regTable;
    auto& orderList = terminal->orderList;

    regTable["usage"] = std::make_pair(
        "how to use the console",
        [this](const Args& /*inputs*/)
        {
            const auto pairs = getOptionHelpPairs();
            const std::size_t align =
                std::ranges::max(pairs, std::less<std::size_t>{}, [](const auto& pair) { return pair.first.length(); })
                    .first.length();
            std::ostringstream out{};
            for (const auto& [option, help] : pairs)
            {
                out << "- " << std::setiosflags(std::ios_base::left) << std::setw(align) << option << "    " << help
                    << std::resetiosflags(std::ios_base::left) << '\n';
            }
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

    regTable["batch"] = std::make_pair(
        "run lines from the file [inputs: FILE]",
        [this](const Args& inputs)
        {
            if (inputs.size() < 2)
            {
                throw std::runtime_error{"Please enter the \"" + inputs.at(0) + "\" and append with FILE."};
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

char** Console::getOptionCompleter(const char* text, int start, int /*end*/)
{
    return (0 == start) ? ::rl_completion_matches(text, &Console::getOptionIterator) : nullptr;
}

char* Console::getOptionIterator(const char* text, int state)
{
    static thread_local Terminal::RegisteredOption::iterator iterator{};
    if (!currentSession)
    {
        return nullptr;
    }

    auto& regTable = currentSession->terminal->regTable;
    if (0 == state)
    {
        iterator = regTable.begin();
    }

    while (regTable.end() != iterator)
    {
        const auto& option = iterator->first;
        ++iterator;
        if (option.find(text) != std::string::npos)
        {
            return ::strdup(option.c_str());
        }
    }

    return nullptr;
}
} // namespace application::console
