//! @file main.cpp
//! @author ryftchen
//! @brief The definitions (main) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2026 ryftchen. All rights reserved.

#include "main.hpp"
#include "command.hpp"
#include "setting.hpp"

#ifndef _PRECOMPILED_HEADER
#include <sys/prctl.h>
#include <sys/wait.h>
#include <iostream>
#else
#include "application/pch/precompiled_header.hpp"
#endif

namespace application
{
//! @brief Anonymous namespace.
inline namespace
{
// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
//! @brief Interrupt flag for the SIGCHLD signal.
volatile std::sig_atomic_t childInterrupted = 0;
//! @brief Interrupt flag for the SIGALRM signal.
volatile std::sig_atomic_t alarmInterrupted = 0;
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)
} // namespace

//! @brief Make the signal set.
//! @tparam Sigs - type of signal numbers
//! @return signal set
template <auto... Sigs>
static ::sigset_t signalSet()
{
    ::sigset_t set{};
    ::sigemptyset(&set);
    (::sigaddset(&set, static_cast<int>(Sigs)), ...);
    return set;
}

//! @brief The run function.
//! @param argc - argument count
//! @param argv - argument vector
//! @return 0 if successful, otherwise 1
static int run(const int argc, const char* const argv[])
try
{
    auto running = std::async(
        std::launch::async,
        [=]()
        {
            ::pthread_setname_np(::pthread_self(), command::title.c_str());
            return setting::loadSettings() && command::executeCLI(argc, argv);
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
try
{
    std::signal(SIGCHLD, [](const int sig) { application::childInterrupted = sig; });
    std::signal(SIGALRM, [](const int sig) { application::alarmInterrupted = sig; });
    const auto unblockMask = signalSet<SIGCHLD, SIGALRM>();
    ::pthread_sigmask(SIG_UNBLOCK, &unblockMask, nullptr);

    constexpr std::uint8_t timeout = 60;
    ::alarm(timeout);
    for (;;)
    {
        ::pause();

        if (application::childInterrupted != 0)
        {
            int status = 0;
            ::waitpid(pid, &status, 0);
            return (WIFEXITED(status) && (WEXITSTATUS(status) == EXIT_SUCCESS)) ? EXIT_SUCCESS : EXIT_FAILURE;
        }
        if (application::alarmInterrupted != 0)
        {
            if (::waitpid(pid, nullptr, WNOHANG) == 0)
            {
                ::kill(pid, SIGKILL);
            }
            throw std::runtime_error{"The child process (" + std::to_string(pid) + ") has timed out."};
        }
    }
}
catch (const std::exception& err)
{
    std::cerr << executableName() << ": " << err.what() << std::endl;
    return EXIT_FAILURE;
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

    const auto blockMask = application::signalSet<SIGCHLD, SIGALRM>();
    ::sigset_t prevMask{};
    ::pthread_sigmask(SIG_BLOCK, &blockMask, &prevMask);

    const auto ppidBeforeFork = ::getpid();
    const auto pid = ::fork();
    if (pid < 0)
    {
        return EXIT_FAILURE;
    }
    if (pid == 0)
    {
        ::pthread_sigmask(SIG_SETMASK, &prevMask, nullptr);
        ::prctl(PR_SET_PDEATHSIG, SIGTERM);
        return (::getppid() == ppidBeforeFork) ? application::run(argc, argv) : EXIT_FAILURE;
    }
    return application::watchdog(pid);
}
