//! @file main.hpp
//! @author ryftchen
//! @brief The declarations (main) in the application module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen.

#pragma once

#ifndef __PRECOMPILED_HEADER
#include <cxxabi.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <csignal>
#include <filesystem>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

//! @brief The application module.
namespace application
{
volatile std::sig_atomic_t signalStatus = 0; // NOLINT(misc-definitions-in-headers)

[[using gnu: constructor]] static void init();
[[using gnu: destructor]] static void fini();

//! @brief Signal handler for SIGSEGV signal, etc.
//! @param sig - signal type
static void signalHandler(int sig)
{
    signalStatus = sig;
    void* callStack[128];
    const int maxFrame = sizeof(callStack) / sizeof(callStack[0]), numOfFrame = ::backtrace(callStack, maxFrame);
    char** symbols = ::backtrace_symbols(callStack, numOfFrame);
    char buffer[1024] = {'\0'};
    std::ostringstream originalTrace, realTrace;

    for (int i = 1; i < numOfFrame; ++i)
    {
        originalTrace << symbols[i] << '\n';
        ::Dl_info info;
        if (::dladdr(callStack[i], &info) && info.dli_sname)
        {
            char* demangle = nullptr;
            int status = -1;
            if ('_' == info.dli_sname[0])
            {
                demangle = ::abi::__cxa_demangle(info.dli_sname, nullptr, nullptr, &status);
            }
            std::snprintf(
                buffer,
                sizeof(buffer),
                "%-3d %*p %s + %zd\n",
                i,
                static_cast<int>(2 + sizeof(void*) * 2),
                callStack[i],
                (0 == status) ? demangle : ((nullptr == info.dli_sname) ? symbols[i] : info.dli_sname),
                static_cast<char*>(callStack[i]) - static_cast<char*>(info.dli_saddr));
            std::free(demangle); // NOLINT(cppcoreguidelines-no-malloc)
        }
        else
        {
            std::snprintf(
                buffer,
                sizeof(buffer),
                "%-3d %*p %s\n",
                i,
                static_cast<int>(2 + sizeof(void*) * 2),
                callStack[i],
                symbols[i]);
        }
        realTrace << buffer;
    }
    std::free(symbols); // NOLINT(cppcoreguidelines-no-malloc)

    if (numOfFrame == maxFrame)
    {
        realTrace << "\r\n[TRUNCATED...]\n";
    }
    std::fprintf(
        ::stderr,
        "\r\n\r\n<MAIN>\n\n[SIGNAL]\n%d\n\n[BACKTRACE]\n%s\n[VERBOSE]\n%s\n",
        sig,
        originalTrace.str().c_str(),
        realTrace.str().c_str());

    if (SIGINT != signalStatus)
    {
        std::signal(sig, SIG_DFL);
        std::raise(sig);
    }
}

//! @brief The constructor function before starting the main function. Switch to the target path.
static void init()
{
    std::signal(SIGABRT, signalHandler);
    std::signal(SIGSEGV, signalHandler);
    ::setenv("TERM", "linux", true);
    ::setenv("TERMINFO", "/etc/terminfo", true);

    const std::filesystem::path absolutePath = std::filesystem::canonical(std::filesystem::path{"/proc/self/exe"});
    const std::size_t pos = absolutePath.string().find("build/bin");
    if (std::string::npos != pos)
    {
        const std::filesystem::path buildPath(std::filesystem::path{absolutePath.string().substr(0, pos)});
        if (!buildPath.has_parent_path())
        {
            std::fprintf(::stderr, "The code project path doesn't exist. Please check it.\n");
            std::exit(-1);
        }
        std::filesystem::current_path(buildPath.parent_path());
    }
    else
    {
        const std::filesystem::path homePath(
            std::filesystem::path{(nullptr != std::getenv("HOME")) ? std::getenv("HOME") : "/root"});
        if (homePath.empty())
        {
            std::fprintf(::stderr, "The home path doesn't exist. Please check it.\n");
            std::exit(-1);
        }

        const std::string procTempPath = homePath / ".foo";
        if (!std::filesystem::exists(procTempPath))
        {
            std::filesystem::create_directory(procTempPath);
            std::filesystem::permissions(
                procTempPath, std::filesystem::perms::owner_all, std::filesystem::perm_options::add);
        }
        std::filesystem::current_path(procTempPath);
    }
}

//! @brief The destructor function before finishing the main function. Check the signal status.
static void fini()
{
    if (signalStatus)
    {
        std::fprintf(::stdout, "Last signal ever received: signal %d.\n", signalStatus);
    }
}
} // namespace application
