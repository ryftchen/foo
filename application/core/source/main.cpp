//! @file main.cpp
//! @author ryftchen
//! @brief The definitions (main) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#include "main.hpp"
#include "command.hpp"
#include "config.hpp"
#include "log.hpp"
#include "view.hpp"

#ifndef __PRECOMPILED_HEADER
#include <sys/prctl.h>
#include <sys/wait.h>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

namespace application
{
//! @brief Anonymous namespace.
inline namespace
{
//! @brief The run function.
//! @param argc - argument count
//! @param argv - argument vector
//! @return the argument to the implicit call to exit()
static int run(int argc, char* argv[])
try
{
    if (!config::loadConfiguration())
    {
        return EXIT_FAILURE;
    }

    using command::Command, log::Log, view::View;

    constexpr std::uint32_t childThdNum = 3;
    auto threads = std::make_shared<utility::thread::Thread>(childThdNum);
    threads->enqueue("commander", &Command::runCommander, &Command::getInstance(), argc, argv);
    if (CONFIG_ACTIVATE_HELPER)
    {
        threads->enqueue("logger", &Log::runLogger, &Log::getInstance());
        threads->enqueue("viewer", &View::runViewer, &View::getInstance());
    }
    return EXIT_SUCCESS;
}
catch (const std::exception& error)
{
    std::cerr << getExecutableName() << ": " << error.what() << std::endl;
    return EXIT_FAILURE;
}
} // namespace

//! @brief Interrupt flag for the SIGALRM signal.
volatile std::sig_atomic_t alarmInterrupted = 0;
//! @brief Interrupt flag for the SIGCHLD signal.
volatile std::sig_atomic_t childInterrupted = 0;

//! @brief Parent process signal handler for the SIGALRM signal.
//! @param sig - signal type
static void setAlarmInterrupted(int sig)
{
    alarmInterrupted = sig;
}

//! @brief Parent process signal handler for the SIGCHLD signal.
//! @param sig - signal type
static void setChildInterrupted(int sig)
{
    childInterrupted = sig;
}
} // namespace application

//! @brief The main function.
//! @param argc - argument count
//! @param argv - argument vector
//! @return the argument to the implicit call to exit()
int main(int argc, char* argv[])
{
    if (1 == argc)
    {
        return application::run(argc, argv);
    }

    const ::pid_t ppidBeforeFork = ::getpid(), pid = ::fork();
    if (pid < 0)
    {
        std::cerr << application::getExecutableName() << ": Failed to create a child process." << std::endl;
        return EXIT_FAILURE;
    }
    else if (0 == pid)
    {
        ::prctl(PR_SET_PDEATHSIG, SIGTERM);
        if (::getppid() != ppidBeforeFork)
        {
            return EXIT_FAILURE;
        }
        return application::run(argc, argv);
    }

    std::signal(SIGALRM, application::setAlarmInterrupted);
    std::signal(SIGCHLD, application::setChildInterrupted);
    constexpr std::uint8_t timeout = 120;
    ::alarm(timeout);
    ::pause();
    if (application::alarmInterrupted)
    {
        if (::waitpid(pid, nullptr, WNOHANG) == 0)
        {
            ::kill(pid, SIGKILL);
            std::cerr << application::getExecutableName() << ": Kill the child process due to timeout." << std::endl;
        }
        return EXIT_FAILURE;
    }
    else if (application::childInterrupted)
    {
        int status = 0;
        ::wait(&status);
        if (WIFEXITED(status) && (WEXITSTATUS(status) != EXIT_SUCCESS))
        {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
