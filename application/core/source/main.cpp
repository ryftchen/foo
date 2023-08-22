//! @file main.cpp
//! @author ryftchen
//! @brief The definitions (main) in the application module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen. All rights reserved.

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
    try
    {
        using application::command::Command;
        using application::log::Log;
        using application::view::View;

        constexpr std::uint32_t childThdNum = 3;
        auto threads = std::make_shared<utility::thread::Thread>(childThdNum);
        threads->enqueue("commander", &Command::runCommander, &Command::getInstance(), argc, argv);
        threads->enqueue("logger", &Log::runLogger, &Log::getInstance());
        threads->enqueue("viewer", &View::runViewer, &View::getInstance());
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
