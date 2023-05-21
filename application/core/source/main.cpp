//! @file main.cpp
//! @author ryftchen
//! @brief The definitions (main) in the application module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#include "main.hpp"
#include "command.hpp"
#include "log.hpp"
#include "observe.hpp"

//! @brief The main function.
//! @param argc - argument count
//! @param argv - argument vector
//! @return the argument to the implicit call to exit()
int main(int argc, char* argv[])
{
    using application::command::Command;
    using application::log::Log;
    using application::observe::Observe;

    constexpr uint32_t threadNum = 3;
    std::shared_ptr<utility::thread::Thread> thread = std::make_shared<utility::thread::Thread>(threadNum);
    thread->enqueue("commander", &Command::runCommander, &Command::getInstance(), argc, argv);
    thread->enqueue("logger", &Log::runLogger, &Log::getInstance());
    thread->enqueue("observer", &Observe::runObserver, &Observe::getInstance());

    return 0;
}
