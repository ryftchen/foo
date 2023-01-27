//! @file console.cpp
//! @author ryftchen
//! @brief The definitions (console) in the utility module.
//! @version 0.1
//! @copyright Copyright (c) 2022
#include "console.hpp"
#include <readline/readline.h>
#include <fstream>
#include <iomanip>
#include <iostream>

namespace utility::console
{
//! @brief Save current console instance.
Console* currentConsole = nullptr;

Console::Console(const std::string& greeting) : impl(std::make_unique<Impl>(greeting))
{
    rl_attempted_completion_function = &Console::getCommandCompleter;

    impl->RegCmds["help"] = std::make_pair(
        [this](const Args& /*unused*/)
        {
            const auto commandsHelp = getHelpOfRegisteredCommands();
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
            return ReturnCode::success;
        },
        "show help");

    impl->RegCmds["quit"] = std::make_pair(
        [this](const Args& /*unused*/)
        {
            return ReturnCode::quit;
        },
        "exit console mode");

    impl->RegCmds["batch"] = std::make_pair(
        [this](const Args& input)
        {
            if (input.size() < 2)
            {
                std::cerr << "console: Please input \"" << input[0] << " Filename\" to run." << std::endl;
                return ReturnCode::error;
            }
            return ReturnCode(fileExecutor(input[1]));
        },
        "run batch commands from the file");
}

Console::~Console()
{
    rl_free(emptyHistory);

    rl_clear_history();
    rl_restore_prompt();
}

void Console::registerCommand(const std::string& command, CommandFunctor func, const std::string& help)
{
    impl->RegCmds[command] = std::make_pair(func, help);
}

std::vector<std::pair<std::string, std::string>> Console::getHelpOfRegisteredCommands() const
{
    std::vector<std::pair<std::string, std::string>> allCommandsHelp;
    for (const auto& pair : impl->RegCmds)
    {
        allCommandsHelp.emplace_back(pair.first, std::get<1>(pair.second));
    }

    return allCommandsHelp;
}

void Console::saveState()
{
    rl_free(impl->history);
    impl->history = history_get_history_state();
}

void Console::reserveConsole()
{
    if (this == currentConsole)
    {
        return;
    }

    if (currentConsole)
    {
        currentConsole->saveState();
    }

    if (!impl->history)
    {
        history_set_history_state(emptyHistory);
    }
    else
    {
        history_set_history_state(impl->history);
    }

    currentConsole = this;
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

    if (!inputs.size())
    {
        return ReturnCode::success;
    }

    Impl::RegisteredCommands::iterator iterator = impl->RegCmds.find(inputs[0]);
    if (std::end(impl->RegCmds) != iterator)
    {
        return ReturnCode(static_cast<int>(std::get<0>(iterator->second)(inputs)));
    }

    std::cerr << "console: Command on console \"" << inputs[0] << "\" not found." << std::endl;
    return ReturnCode::error;
}

int Console::fileExecutor(const std::string& filename)
{
    std::ifstream input(filename);
    if (!input)
    {
        std::cerr << "console: Can not find batch file to run." << std::endl;
        return ReturnCode::error;
    }

    std::string command;
    int counter = 0, result = 0;
    while (std::getline(input, command))
    {
        if ('#' == command[0])
        {
            continue;
        }
        std::cout << "[" << counter << "] " << command << std::endl;

        result = commandExecutor(command);
        if (result)
        {
            return ReturnCode(result);
        }
        ++counter;
        std::cout << std::endl;
    }

    return ReturnCode::success;
}

int Console::readCommandLine()
{
    reserveConsole();

    char* buffer = readline(getGreeting().c_str());
    if (!buffer)
    {
        std::cout << std::endl;
        return ReturnCode::quit;
    }

    if ('\0' != buffer[0])
    {
        add_history(buffer);
    }

    std::string line(buffer);
    rl_free(buffer);
    return ReturnCode(commandExecutor(line));
}

char** Console::getCommandCompleter(const char* text, int start, int /*unused*/)
{
    char** completionList = nullptr;
    if (!start)
    {
        completionList = rl_completion_matches(text, &Console::getCommandIterator);
    }

    return completionList;
}

char* Console::getCommandIterator(const char* text, int state)
{
    static Impl::RegisteredCommands::iterator iterator;
    if (!currentConsole)
    {
        return nullptr;
    }

    auto& commands = currentConsole->impl->RegCmds;
    if (!state)
    {
        iterator = std::begin(commands);
    }

    while (std::end(commands) != iterator)
    {
        const auto& command = iterator->first;
        ++iterator;
        if (std::string::npos != command.find(text))
        {
            return strdup(command.c_str());
        }
    }

    return nullptr;
}
} // namespace utility::console
