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
    auto& regTable = impl->regTable;
    auto& orderList = impl->orderList;

    regTable["usage"] = std::make_pair(
        [this](const Args& /*input*/)
        {
            const auto pairs = getOptionHelpPairs();
            std::size_t maxLen = 0;
            for ([[maybe_unused]] const auto& [option, help] : pairs)
            {
                maxLen = std::max(maxLen, option.length());
            }

            std::cout << "console option:\n" << std::endl;
            for (const auto& [option, help] : pairs)
            {
                std::cout << std::setiosflags(std::ios_base::left) << std::setw(maxLen) << option << "    " << help
                          << std::resetiosflags(std::ios_base::left) << '\n';
            }

            std::cout << std::flush;
            return RetCode::success;
        },
        "how to use the console");
    orderList.emplace_back("usage");

    regTable["quit"] = std::make_pair(
        [](const Args& /*input*/)
        {
            std::cout << "exit" << std::endl;
            return RetCode::quit;
        },
        "exit the console");
    orderList.emplace_back("quit");

    regTable["batch"] = std::make_pair(
        [this](const Args& input)
        {
            if (input.size() < 2)
            {
                throw std::runtime_error("Please enter the \"" + input.at(0) + "\" and append with FILE.");
            }
            return RetCode(fileExecutor(input.at(1)));
        },
        "run lines from the file [inputs: FILE]");
    orderList.emplace_back("batch");
}

Console::~Console()
{
    ::rl_free(emptyHistory);

    ::rl_clear_history();
    ::rl_restore_prompt();
}

void Console::registerOption(const std::string_view name, const OptionFunctor& func, const std::string_view prompt)
{
    impl->regTable[name.data()] = std::make_pair(func, prompt);
    impl->orderList.emplace_back(name);
}

void Console::setGreeting(const std::string_view greeting)
{
    impl->greeting = greeting;
}

int Console::optionExecutor(const std::string_view option)
{
    std::vector<std::string> inputs{};
    std::istringstream trans(option.data());
    std::copy(
        std::istream_iterator<std::string>(trans), std::istream_iterator<std::string>(), std::back_inserter(inputs));

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

    return RetCode(std::get<0>(regIter->second)(inputs));
}

int Console::fileExecutor(const std::string_view filename)
{
    std::ifstream input(filename.data());
    if (!input)
    {
        throw std::runtime_error("Could not find the batch file to run.");
    }

    std::string option{};
    std::uint32_t counter = 0;
    int result = 0;
    while (std::getline(input, option))
    {
        if ('#' == option.at(0))
        {
            continue;
        }
        std::cout << '[' << counter << "] " << option << std::endl;

        result = optionExecutor(option);
        if (result)
        {
            return RetCode(result);
        }
        ++counter;
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
    std::string line(buffer);
    ::rl_free(buffer);

    return RetCode(optionExecutor(line));
}

std::vector<Console::OptionHelpPair> Console::getOptionHelpPairs() const
{
    const auto transformed = impl->orderList
        | std::views::transform([this](const auto& option)
                                { return std::make_pair(option, impl->regTable.at(option).second); });
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
