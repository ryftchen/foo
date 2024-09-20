//! @file console.cpp
//! @author ryftchen
//! @brief The definitions (console) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#include "console.hpp"

#include <readline/readline.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>

namespace utility::console
{
//! @brief Function version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    static const char* const ver = "0.1.0";
    return ver;
}

//! @brief Anonymous namespace.
inline namespace
{
//! @brief Current console instance.
static Console* currentSession = nullptr;
} // namespace

Console::Console(const std::string& greeting) : impl(std::make_unique<Impl>(greeting))
{
    ::rl_attempted_completion_function = &Console::getCommandCompleter;

    impl->regMap["usage"] = std::make_pair(
        [this](const Args& /*input*/)
        {
            const auto commandsHelp = getHelpOfRegisteredCommand();
            std::size_t maxLength = 0;
            for ([[maybe_unused]] const auto& [command, help] : commandsHelp)
            {
                maxLength = std::max(maxLength, command.length());
            }

            std::cout << "console command:\n" << std::endl;
            for (const auto& [command, help] : commandsHelp)
            {
                std::cout << std::setiosflags(std::ios_base::left) << std::setw(maxLength) << command << "    " << help
                          << std::resetiosflags(std::ios_base::left) << '\n';
            }

            std::cout << std::flush;
            return RetCode::success;
        },
        "show usage");
    impl->regOrder.emplace_back("usage");

    impl->regMap["quit"] = std::make_pair(
        [](const Args& /*input*/)
        {
            std::cout << "exit" << std::endl;
            return RetCode::quit;
        },
        "exit console mode");
    impl->regOrder.emplace_back("quit");

    impl->regMap["batch"] = std::make_pair(
        [this](const Args& input)
        {
            if (input.size() < 2)
            {
                throw std::invalid_argument("Please enter the \"" + input.at(0) + "\" and append with FILE.");
            }
            return RetCode(fileExecutor(input.at(1)));
        },
        "run batch commands in file [inputs: FILE]");
    impl->regOrder.emplace_back("batch");
}

Console::~Console()
{
    ::rl_free(emptyHistory);

    ::rl_clear_history();
    ::rl_restore_prompt();
}

void Console::registerCommand(const std::string& name, const CommandFunctor& func, const std::string& help)
{
    impl->regMap[name] = std::make_pair(func, help);
    impl->regOrder.emplace_back(name);
}

void Console::setGreeting(const std::string& greeting)
{
    impl->greeting = greeting;
}

std::string Console::getGreeting() const
{
    return impl->greeting;
}

int Console::commandExecutor(const std::string& command)
{
    std::vector<std::string> inputs;
    std::istringstream is(command);
    std::copy(std::istream_iterator<std::string>(is), std::istream_iterator<std::string>(), std::back_inserter(inputs));

    if (inputs.empty())
    {
        return RetCode::success;
    }

    const auto iterator = impl->regMap.find(inputs.at(0));
    if (std::cend(impl->regMap) == iterator)
    {
        throw std::invalid_argument(
            "The console command \"" + inputs.at(0) + R"(" could not be found. Enter the "usage" for help.)");
    }

    return RetCode(static_cast<int>(std::get<0>(iterator->second)(inputs)));
}

int Console::fileExecutor(const std::string& filename)
{
    std::ifstream input(filename);
    if (!input)
    {
        throw std::runtime_error("Could not find the batch file to run.");
    }

    std::string command;
    std::uint32_t counter = 0;
    int result = 0;
    while (std::getline(input, command))
    {
        if ('#' == command.at(0))
        {
            continue;
        }
        std::cout << '[' << counter << "] " << command << std::endl;

        result = commandExecutor(command);
        if (result)
        {
            return RetCode(result);
        }
        ++counter;
        std::cout << std::endl;
    }

    return RetCode::success;
}

int Console::readCommandLine()
{
    reserveConsole();

    char* const buffer = ::readline(getGreeting().c_str());
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
    return RetCode(commandExecutor(line));
}

std::vector<Console::CommandHelpPair> Console::getHelpOfRegisteredCommand() const
{
    std::vector<Console::CommandHelpPair> allCommandsHelp;
    for (const auto& command : impl->regOrder)
    {
        allCommandsHelp.emplace_back(command, impl->regMap.at(command).second);
    }

    return allCommandsHelp;
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

char** Console::getCommandCompleter(const char* text, int start, int /*end*/)
{
    char** completionList = nullptr;
    if (0 == start)
    {
        completionList = ::rl_completion_matches(text, &Console::getCommandIterator);
    }

    return completionList;
}

char* Console::getCommandIterator(const char* text, int state)
{
    static Impl::RegisteredCommand::iterator iterator;
    if (nullptr == currentSession)
    {
        return nullptr;
    }

    auto& commands = currentSession->impl->regMap;
    if (0 == state)
    {
        iterator = std::begin(commands);
    }

    while (std::end(commands) != iterator)
    {
        const auto& command = iterator->first;
        ++iterator;
        if (command.find(text) != std::string::npos)
        {
            return ::strdup(command.c_str());
        }
    }

    return nullptr;
}
} // namespace utility::console
