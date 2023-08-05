//! @file console.cpp
//! @author ryftchen
//! @brief The definitions (console) in the utility module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen. All rights reserved.

#include "console.hpp"
#include <readline/readline.h>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>

namespace utility::console
{
//! @brief Save current console instance.
static Console* currentConsole = nullptr;

Console::Console(const std::string& greeting) : impl(std::make_unique<Impl>(greeting))
{
    ::rl_attempted_completion_function = &Console::getCmdCompleter;

    impl->regCmds["help"] = std::make_pair(
        [this](const Args& /*input*/)
        {
            const auto commandsHelp = getHelpOfRegisteredCmds();
            std::size_t maxLength = 0;
            for ([[maybe_unused]] const auto& [command, help] : commandsHelp)
            {
                maxLength = std::max(maxLength, command.length());
            }

            std::cout << "Console command:\n" << std::endl;
            for (auto reverseIter = commandsHelp.rbegin(); commandsHelp.rend() != reverseIter; ++reverseIter)
            {
                std::cout << std::setiosflags(std::ios_base::left) << std::setw(maxLength) << reverseIter->first
                          << "    " << reverseIter->second << std::resetiosflags(std::ios_base::left) << std::endl;
            }
            return RetCode::success;
        },
        "show help");

    impl->regCmds["quit"] = std::make_pair(
        [this](const Args& /*input*/)
        {
            std::cout << "Exit." << std::endl;
            return RetCode::quit;
        },
        "exit console mode");

    impl->regCmds["batch"] = std::make_pair(
        [this](const Args& input)
        {
            if (input.size() < 2)
            {
                std::cerr << "Please input \"" << input[0] << " FILENAME\" to run." << std::endl;
                return RetCode::error;
            }
            return RetCode(fileExecutor(input[1]));
        },
        "run batch commands from the file");
}

Console::~Console()
{
    ::rl_free(emptyHistory);

    ::rl_clear_history();
    ::rl_restore_prompt();
}

void Console::registerCmd(const std::string& command, CmdFunctor func, const std::string& help)
{
    impl->regCmds[command] = std::make_pair(func, help);
}

int Console::cmdExecutor(const std::string& command)
{
    std::vector<std::string> inputs;
    std::istringstream is(command);
    std::copy(std::istream_iterator<std::string>(is), std::istream_iterator<std::string>(), std::back_inserter(inputs));

    if (inputs.size() == 0)
    {
        return RetCode::success;
    }

    Impl::RegisteredCmds::iterator iterator = impl->regCmds.find(inputs[0]);
    if (std::end(impl->regCmds) != iterator)
    {
        return RetCode(static_cast<int>(std::get<0>(iterator->second)(inputs)));
    }

    std::cerr << "Console command \"" << inputs[0] << "\" not found." << std::endl;
    return RetCode::error;
}

int Console::fileExecutor(const std::string& filename)
{
    std::ifstream input(filename);
    if (!input)
    {
        std::cerr << "Could not find the batch file to run." << std::endl;
        return RetCode::error;
    }

    std::string command;
    int counter = 0, result = 0;
    while (std::getline(input, command))
    {
        if ('#' == command[0])
        {
            continue;
        }
        std::cout << '[' << counter << "] " << command << std::endl;

        result = cmdExecutor(command);
        if (result)
        {
            return RetCode(result);
        }
        ++counter;
        std::cout << std::endl;
    }

    return RetCode::success;
}

int Console::readCmdLine()
{
    reserveConsole();

    char* buffer = ::readline(getGreeting().c_str());
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
    return RetCode(cmdExecutor(line));
}

Console::CmdsHelp Console::getHelpOfRegisteredCmds() const
{
    CmdsHelp allCommandsHelp;
    for (const auto& pair : impl->regCmds)
    {
        allCommandsHelp.emplace_back(pair.first, std::get<1>(pair.second));
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
    if (this == currentConsole)
    {
        return;
    }

    if (nullptr != currentConsole)
    {
        currentConsole->saveState();
    }

    if (nullptr == impl->history)
    {
        ::history_set_history_state(emptyHistory);
    }
    else
    {
        ::history_set_history_state(impl->history);
    }

    currentConsole = this;
}

char** Console::getCmdCompleter(const char* text, int start, int /*end*/)
{
    char** completionList = nullptr;
    if (0 == start)
    {
        completionList = ::rl_completion_matches(text, &Console::getCmdIterator);
    }

    return completionList;
}

char* Console::getCmdIterator(const char* text, int state)
{
    static Impl::RegisteredCmds::iterator iterator;
    if (nullptr == currentConsole)
    {
        return nullptr;
    }

    auto& commands = currentConsole->impl->regCmds;
    if (0 == state)
    {
        iterator = std::begin(commands);
    }

    while (std::end(commands) != iterator)
    {
        const auto& command = iterator->first;
        ++iterator;
        if (std::string::npos != command.find(text))
        {
            return ::strdup(command.c_str());
        }
    }

    return nullptr;
}
} // namespace utility::console
