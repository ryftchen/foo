//! @file main.hpp
//! @author ryftchen
//! @brief The declarations (main) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

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
//! @brief Status of the signal.
volatile std::sig_atomic_t signalStatus = 0; // NOLINT (misc-definitions-in-headers)

//! @brief Get the executable name.
//! @return executable name
static inline std::string getExecutableName()
{
    return std::filesystem::canonical(std::filesystem::path{"/proc/self/exe"}).filename();
}

//! @brief Signal handler for SIGSEGV signal, etc.
//! @param sig - signal type
static void signalHandler(int sig)
{
    signalStatus = sig;
    void* callStack[128];
    const int maxFrame = sizeof(callStack) / sizeof(callStack[0]), numOfFrame = ::backtrace(callStack, maxFrame);
    char** const symbols = ::backtrace_symbols(callStack, numOfFrame);

    constexpr std::uint16_t bufferSize = 1024;
    char buffer[bufferSize] = {'\0'};
    std::ostringstream originalTrace, detailedTrace;
    for (int i = 1; i < numOfFrame; ++i)
    {
        originalTrace << symbols[i] << '\n';
        ::Dl_info info{};
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
                bufferSize,
                "%-3d %*p %.960s + %zd\n",
                i,
                static_cast<int>(2 + sizeof(void*) * 2),
                callStack[i],
                (0 == status) ? demangle : ((nullptr == info.dli_sname) ? symbols[i] : info.dli_sname),
                static_cast<char*>(callStack[i]) - static_cast<char*>(info.dli_saddr));
            std::free(demangle); // NOLINT (cppcoreguidelines-no-malloc)
        }
        else
        {
            std::snprintf(
                buffer,
                bufferSize,
                "%-3d %*p %.960s\n",
                i,
                static_cast<int>(2 + sizeof(void*) * 2),
                callStack[i],
                symbols[i]);
        }
        detailedTrace << buffer;
    }
    std::free(symbols); // NOLINT (cppcoreguidelines-no-malloc)

    if (numOfFrame == maxFrame)
    {
        detailedTrace << "\r\n<TRUNCATED...>\n";
    }
    std::fprintf(
        ::stderr,
        "\r\n%s:\n<SIGNAL>\n%d\n\n<BACKTRACE>\n%s\n<VERBOSE>\n%s\n",
        getExecutableName().c_str(),
        sig,
        originalTrace.str().c_str(),
        detailedTrace.str().c_str());

    if (SIGINT != signalStatus)
    {
        std::signal(sig, SIG_DFL);
        std::raise(sig);
    }
}

//! @brief The constructor function before starting the main function. Switch to the target path.
[[using gnu: constructor]] static void init()
{
    std::signal(SIGABRT, signalHandler);
    std::signal(SIGSEGV, signalHandler);
    ::setenv("TERM", "linux", true);
    ::setenv("TERMINFO", "/etc/terminfo", true);

    std::string home = "/root";
    if (nullptr != std::getenv("HOME"))
    {
        home = std::getenv("HOME");
    }
    const std::filesystem::path targetPath{home};
    if (!std::filesystem::exists(targetPath))
    {
        std::fprintf(::stdout, "%s: Could not find the home directory.\n", getExecutableName().c_str());
        std::exit(EXIT_FAILURE);
    }

    const std::filesystem::path processPath{targetPath / ".foo"};
    if (!std::filesystem::exists(processPath))
    {
        std::filesystem::create_directory(processPath);
        std::filesystem::permissions(
            processPath, std::filesystem::perms::owner_all, std::filesystem::perm_options::add);
    }
    ::setenv("FOO_HOME", processPath.string().c_str(), true);
}

//! @brief The destructor function before finishing the main function. Check the signal status.
[[using gnu: destructor]] static void fini()
{
    if (signalStatus)
    {
        std::fprintf(
            ::stdout, "%s: Last signal ever received: signal %d.\n", getExecutableName().c_str(), signalStatus);
    }
}
} // namespace application
