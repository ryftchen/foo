//! @file main.cpp
//! @author ryftchen
//! @brief The definitions (main) in the application module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen.

#include "main.hpp"
#include "command.hpp"
#include "log.hpp"
#include "view.hpp"

//! @brief The main function.
//! @param argc - argument count
//! @param argv - argument vector
//! @return the argument to the implicit call to exit()
int main(int argc, char* argv[])
{
    using application::command::Command;
    using application::log::Log;
    using application::view::View;

    constexpr std::uint32_t threadNum = 3;
    std::shared_ptr<utility::thread::Thread> thread = std::make_shared<utility::thread::Thread>(threadNum);
    thread->enqueue("commander", &Command::runCommander, &Command::getInstance(), argc, argv);
    thread->enqueue("logger", &Log::runLogger, &Log::getInstance());
    thread->enqueue("viewer", &View::runViewer, &View::getInstance());

    return 0;
}
