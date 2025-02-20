//! @file main.cpp
//! @author ryftchen
//! @brief The definitions (main) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "main.hpp"
#include "command.hpp"
#include "configure.hpp"

#ifndef __PRECOMPILED_HEADER
#include <sys/prctl.h>
#include <sys/wait.h>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

namespace application
{
//! @brief The run function.
//! @param argc - argument count
//! @param argv - argument vector
//! @return the argument to the implicit call to exit()
static int run(int argc, char* argv[])
try
{
    if (!configure::loadConfiguration())
    {
        return EXIT_FAILURE;
    }

    using command::Command;
    std::future<bool> running = std::async(
        std::launch::async,
        [=]()
        {
            ::pthread_setname_np(::pthread_self(), Command::name.c_str());
            return Command::getInstance().execute(argc, argv);
        });

    return running.get() ? EXIT_SUCCESS : EXIT_FAILURE;
}
catch (const std::exception& err)
{
    std::cerr << getExecutableName() << ": " << err.what() << std::endl;
    return EXIT_FAILURE;
}

//! @brief Anonymous namespace.
inline namespace
{
//! @brief Interrupt flag for the SIGALRM signal.
volatile std::sig_atomic_t alarmInterrupted = 0;
//! @brief Interrupt flag for the SIGCHLD signal.
volatile std::sig_atomic_t childInterrupted = 0;
} // namespace

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
        return EXIT_FAILURE;
    }
    else if (0 == pid)
    {
        ::prctl(PR_SET_PDEATHSIG, SIGTERM);
        return (::getppid() == ppidBeforeFork) ? application::run(argc, argv) : EXIT_FAILURE;
    }

    std::signal(SIGALRM, application::setAlarmInterrupted);
    std::signal(SIGCHLD, application::setChildInterrupted);
    constexpr std::uint8_t timeout = 60;
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
