//! @file main.cpp
//! @author ryftchen
//! @brief The definitions (main) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "main.hpp"
#include "command.hpp"
#include "configure.hpp"

#ifndef _PRECOMPILED_HEADER
#include <sys/prctl.h>
#include <sys/wait.h>
#include <iostream>
#else
#include "application/pch/precompiled_header.hpp"
#endif // _PRECOMPILED_HEADER

namespace application
{
//! @brief Anonymous namespace.
inline namespace
{
//! @brief Interrupt flag for the SIGALRM signal.
volatile std::sig_atomic_t alarmInterrupted = 0;
//! @brief Interrupt flag for the SIGCHLD signal.
volatile std::sig_atomic_t childInterrupted = 0;
} // namespace

//! @brief The run function.
//! @param argc - argument count
//! @param argv - argument vector
//! @return 0 if successful, otherwise 1
static int run(const int argc, const char* const argv[])
try
{
    if (!configure::loadConfiguration())
    {
        return EXIT_FAILURE;
    }
    auto running = std::async(
        std::launch::async,
        [=]()
        {
            using command::Command;
            ::pthread_setname_np(::pthread_self(), Command::title.c_str());
            return Command::getInstance().execute(argc, argv);
        });

    return running.get() ? EXIT_SUCCESS : EXIT_FAILURE;
}
catch (const std::exception& err)
{
    std::cerr << executableName() << ": " << err.what() << std::endl;
    return EXIT_FAILURE;
}

//! @brief The watchdog function.
//! @param pid - pid by fork
//! @return 0 if successful, otherwise 1
static int watchdog(const ::pid_t pid)
{
    std::signal(SIGALRM, [](const int sig) { application::alarmInterrupted = sig; });
    std::signal(SIGCHLD, [](const int sig) { application::childInterrupted = sig; });

    constexpr std::uint8_t timeout = 60;
    ::alarm(timeout);
    ::pause();
    if (application::alarmInterrupted)
    {
        if (::waitpid(pid, nullptr, WNOHANG) == 0)
        {
            ::kill(pid, SIGKILL);
            std::cerr << application::executableName() << ": Kill the child process (" << pid << ") due to timeout."
                      << std::endl;
        }
        return EXIT_FAILURE;
    }
    if (application::childInterrupted)
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
} // namespace application

//! @brief The main function.
//! @param argc - argument count
//! @param argv - argument vector
//! @return the argument to the implicit call to exit()
int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        return application::run(argc, argv);
    }

    const ::pid_t ppidBeforeFork = ::getpid(), pid = ::fork();
    if (pid < 0)
    {
        return EXIT_FAILURE;
    }
    if (pid == 0)
    {
        ::prctl(PR_SET_PDEATHSIG, SIGTERM);
        return (::getppid() == ppidBeforeFork) ? application::run(argc, argv) : EXIT_FAILURE;
    }

    return application::watchdog(pid);
}
