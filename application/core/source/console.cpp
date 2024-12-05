//! @file console.cpp
//! @author ryftchen
//! @brief The definitions (console) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#include "console.hpp"

#ifndef __PRECOMPILED_HEADER
#include <readline/readline.h>
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
static Console* currentSession = nullptr;
} // namespace

Console::Console(const std::string_view greeting) : impl(std::make_unique<Impl>(greeting))
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
    impl->regTable[name.data()] = std::make_pair(prompt, std::move(func));
    impl->orderList.emplace_back(name);
}

void Console::setGreeting(const std::string_view greeting)
{
    impl->greeting = greeting;
}

int Console::optionExecutor(const std::string_view option)
{
    std::vector<std::string> inputs{};
    std::istringstream transfer(option.data());
    std::copy(
        std::istream_iterator<std::string>(transfer), std::istream_iterator<std::string>{}, std::back_inserter(inputs));
    if (inputs.empty())
    {
        return RetCode::success;
    }

    const auto regIter = impl->regTable.find(inputs.front());
    if (impl->regTable.cend() == regIter)
    {
        throw std::runtime_error(
            "The console option \"" + inputs.front() + R"(" could not be found. Enter the "usage" for help.)");
    }

    return RetCode(regIter->second.second(inputs));
}

int Console::fileExecutor(const std::string_view filename)
{
    std::ifstream batch(filename.data());
    if (!batch)
    {
        throw std::runtime_error("Could not find the batch file to run.");
    }

    std::string input{};
    std::uint32_t counter = 0;
    while (std::getline(batch, input))
    {
        if (input.empty() || ('#' == input.front()))
        {
            continue;
        }

        ++counter;
        std::ostringstream out{};
        out << '#' << counter << ' ' << input << '\n';
        std::cout << out.str() << std::flush;
        if (const int result = optionExecutor(input))
        {
            return RetCode(result);
        }
        std::cout << std::endl;
    }

    return RetCode::success;
}

int Console::readLine()
{
    reserveConsole();

    char* const buffer = ::readline(impl->greeting.c_str());
    if (nullptr == buffer)
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

    return RetCode(optionExecutor(input));
}

void Console::setDefaultOptions()
{
    auto& regTable = impl->regTable;
    auto& orderList = impl->orderList;

    regTable["usage"] = std::make_pair(
        "how to use the console",
        [this](const Args& /*inputs*/)
        {
            const auto pairs = getOptionHelpPairs();
            std::size_t maxLen = 0;
            std::for_each(
                pairs.cbegin(),
                pairs.cend(),
                [&maxLen](const auto& pair) { maxLen = std::max(pair.first.length(), maxLen); });

            std::ostringstream out{};
            out << "console option:\n\n";
            for (const auto& [option, help] : pairs)
            {
                out << std::setiosflags(std::ios_base::left) << std::setw(maxLen) << option << "    " << help
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
                throw std::runtime_error("Please enter the \"" + inputs.at(0) + "\" and append with FILE.");
            }
            return RetCode(fileExecutor(inputs.at(1)));
        });
    orderList.emplace_back("batch");
}

std::vector<Console::OptionHelpPair> Console::getOptionHelpPairs() const
{
    const auto transformed = impl->orderList
        | std::views::transform([this](const auto& option)
                                { return std::make_pair(option, impl->regTable.at(option).first); });
    return std::vector<OptionHelpPair>{transformed.begin(), transformed.end()};
}

void Console::saveState()
{
    ::rl_free(impl->history);
    impl->history = ::history_get_history_state();
}

void Console::reserveConsole()
{
    if (this == currentSession)
    {
        return;
    }

    if (nullptr != currentSession)
    {
        currentSession->saveState();
    }

    if (nullptr == impl->history)
    {
        ::history_set_history_state(emptyHistory);
    }
    else
    {
        ::history_set_history_state(impl->history);
    }

    currentSession = this;
}

char** Console::getOptionCompleter(const char* text, int start, int /*end*/)
{
    char** completionList = nullptr;
    if (0 == start)
    {
        completionList = ::rl_completion_matches(text, &Console::getOptionIterator);
    }

    return completionList;
}

char* Console::getOptionIterator(const char* text, int state)
{
    static Impl::RegisteredOption::iterator iterator{};
    if (nullptr == currentSession)
    {
        return nullptr;
    }

    auto& regTable = currentSession->impl->regTable;
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
