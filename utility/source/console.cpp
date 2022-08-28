#include "console.hpp"
#include <readline/readline.h>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <unordered_map>

Console* currentConsole = nullptr;

Console::Console(std::string const& greeting) : impl(std::make_unique<Impl>(greeting))
{
    rl_attempted_completion_function = &Console::getCommandCompletions;

    impl->RegCmds["help"] = std::make_pair(
        [this](const Arguments& /*unused*/)
        {
            auto commandsHelp = getHelpOfRegisteredCommands();
            std::size_t maxLength = 0;
            for ([[maybe_unused]] const auto& [command, help] : commandsHelp)
            {
                maxLength = std::max(maxLength, command.length());
            }

            std::cout << "Console command:" << std::endl;
            for (auto iterReverse = commandsHelp.rbegin(); commandsHelp.rend() != iterReverse;
                 ++iterReverse)
            {
                std::cout << std::setiosflags(std::ios_base::left) << std::setw(maxLength)
                          << iterReverse->first << "    " << iterReverse->second
                          << std::resetiosflags(std::ios_base::left) << std::endl;
            }
            return ReturnCode::success;
        },
        "show help");

    impl->RegCmds["quit"] = std::make_pair(
        [this](const Arguments& /*unused*/)
        {
            return ReturnCode::quit;
        },
        "exit console");

    impl->RegCmds["run"] = std::make_pair(
        [this](const Arguments& input)
        {
            if (input.size() < 2)
            {
                std::cout << "Please input \"" << input[0] << " ScriptFilename\"" << std::endl;
                return ReturnCode::error;
            }
            return ReturnCode(runFile(input[1]));
        },
        "run script file");
}

Console::~Console()
{
    // NOLINTNEXTLINE(cppcoreguidelines-no-malloc)
    free(emptyHistory);
}

void Console::registerCommand(
    const std::string& command, CommandFunction func, const std::string& help)
{
    impl->RegCmds[command] = std::make_pair(func, help);
}

std::vector<std::pair<std::string, std::string>> Console::getHelpOfRegisteredCommands() const
{
    std::vector<std::pair<std::string, std::string>> allCommandsHelp;
    for (const auto& pair : impl->RegCmds)
    {
        allCommandsHelp.emplace_back(std::make_pair(pair.first, std::get<1>(pair.second)));
    }

    return allCommandsHelp;
}

void Console::saveState()
{
    // NOLINTNEXTLINE(cppcoreguidelines-no-malloc)
    free(impl->history);
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

int Console::runCommand(const std::string& command)
{
    std::vector<std::string> inputs;
    std::istringstream is(command);
    std::copy(
        std::istream_iterator<std::string>(is), std::istream_iterator<std::string>(),
        std::back_inserter(inputs));

    if (!inputs.size())
    {
        return ReturnCode::success;
    }

    Impl::RegisteredCommands::iterator iterator = impl->RegCmds.find(inputs[0]);
    if (std::end(impl->RegCmds) != iterator)
    {
        return ReturnCode(static_cast<int>(std::get<0>(iterator->second)(inputs)));
    }

    std::cout << "Command on console \"" << inputs[0] << "\" not found." << std::endl;
    return ReturnCode::error;
}

int Console::runFile(const std::string& filename)
{
    std::ifstream input(filename);
    if (!input)
    {
        std::cout << "Can not find the file to run." << std::endl;
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

        result = runCommand(command);
        if (result)
        {
            return ReturnCode(result);
        }
        ++counter;
        std::cout << std::endl;
    }

    return ReturnCode::success;
}

int Console::readLine()
{
    reserveConsole();

    char* buffer = readline(impl->greeting.c_str());
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
    // NOLINTNEXTLINE(cppcoreguidelines-no-malloc)
    free(buffer);
    return ReturnCode(runCommand(line));
}

char** Console::getCommandCompletions(const char* text, int start, int /*unused*/)
{
    char** completionList = nullptr;

    if (!start)
    {
        completionList = rl_completion_matches(text, &Console::commandIterator);
    }

    return completionList;
}

char* Console::commandIterator(const char* text, int state)
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
