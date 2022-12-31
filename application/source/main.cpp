//! @file main.cpp
//! @author ryftchen
//! @brief The definitions (main) in the application module.
//! @version 0.1
//! @copyright Copyright (c) 2022
#include "main.hpp"
#include "command.hpp"
#include "utility/include/log.hpp"
#include "utility/include/thread.hpp"

//! @brief The main function.
//! @param argc argument count
//! @param argv argument vector
//! @return the argument to the implicit call to exit()
int main(int argc, char* argv[])
{
    using application::command::Command;
    using utility::log::Log;

    std::shared_ptr<utility::thread::Thread> thread = std::make_shared<utility::thread::Thread>(2);
    thread->enqueue("commander", &Command::runCommander, &Command::getInstance(), argc, argv);
    thread->enqueue("logger", &Log::runLogger, &Log::getInstance());

    return 0;
}
