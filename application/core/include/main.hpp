//! @file main.hpp
//! @author ryftchen
//! @brief The declarations (main) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#ifndef _PRECOMPILED_HEADER
#include <cxxabi.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <array>
#include <csignal>
#include <filesystem>
#else
#include "application/pch/precompiled_header.hpp"
#endif // _PRECOMPILED_HEADER

//! @brief The application module.
namespace application
{
//! @brief Status of the signal.
static volatile std::sig_atomic_t signalStatus = 0;

//! @brief Get the executable name.
//! @return executable name
[[gnu::always_inline]] static inline std::string executableName()
{
    return std::filesystem::canonical(std::filesystem::path{"/proc/self/exe"}).filename().string();
}

// NOLINTBEGIN(cppcoreguidelines-no-malloc, cppcoreguidelines-owning-memory)
//! @brief Signal handler for SIGSEGV signal, etc.
//! @param sig - signal type
static void signalHandler(const int sig)
{
    signalStatus = sig;
    constexpr std::uint16_t maxFrame = 128;
    std::array<void*, maxFrame> callStack{};
    const int numOfFrame = ::backtrace(callStack.data(), callStack.size());
    char** const symbols = ::backtrace_symbols(callStack.data(), numOfFrame);

    std::array<char, 1024> buffer{};
    std::ostringstream originalTrace{}, detailedTrace{};
    for (int i = 1; i < numOfFrame; ++i)
    {
        originalTrace << symbols[i] << '\n';
        ::Dl_info info{};
        if (::dladdr(callStack[i], &info) && info.dli_sname)
        {
            char* demangle = nullptr;
            int status = -1;
            if (info.dli_sname[0] == '_')
            {
                demangle = ::abi::__cxa_demangle(info.dli_sname, nullptr, nullptr, &status);
            }
            std::snprintf(
                buffer.data(),
                buffer.size(),
                "%-3d %*p %.960s + %zd\n",
                i,
                static_cast<int>(2 + (sizeof(void*) * 2)),
                callStack[i],
                (status == 0) ? demangle : (info.dli_sname ? info.dli_sname : symbols[i]),
                static_cast<char*>(callStack[i]) - static_cast<char*>(info.dli_saddr));
            std::free(demangle);
        }
        else
        {
            std::snprintf(
                buffer.data(),
                buffer.size(),
                "%-3d %*p %.960s\n",
                i,
                static_cast<int>(2 + (sizeof(void*) * 2)),
                callStack[i],
                symbols[i]);
        }
        detailedTrace << buffer.data();
    }
    std::free(static_cast<void*>(symbols));

    if (numOfFrame == maxFrame)
    {
        detailedTrace << "\n<TRUNCATED>\n";
    }
    std::fprintf(
        ::stderr,
        "%s: Crash occurred.\n\n<SIGNAL>\n%d\n\n<BACKTRACE>\n%s\n<VERBOSE>\n%s\n",
        executableName().c_str(),
        sig,
        originalTrace.str().c_str(),
        detailedTrace.str().c_str());

    if (signalStatus != SIGINT)
    {
        std::signal(sig, SIG_DFL);
        std::raise(sig);
    }
}
// NOLINTEND(cppcoreguidelines-no-malloc, cppcoreguidelines-owning-memory)

// NOLINTBEGIN(concurrency-mt-unsafe)
//! @brief The constructor function before starting the main function. Switch to the target path.
[[using gnu: constructor, noinline]] static void onInitial()
{
    std::signal(SIGABRT, signalHandler);
    std::signal(SIGSEGV, signalHandler);
    ::setenv("TERM", "linux", true);
    ::setenv("TERMINFO", "/etc/terminfo", true);

    const char* const homeEnv = std::getenv("HOME");
    const std::string_view defaultHome = homeEnv ? homeEnv : "/root";
    const auto homePath = std::filesystem::absolute(defaultHome);
    if (!std::filesystem::is_directory(homePath))
    {
        std::fprintf(::stdout, "%s: Could not find the home directory.\n", executableName().c_str());
        std::exit(EXIT_FAILURE);
    }

    const auto processPath = homePath / ".foo";
    if (!std::filesystem::is_directory(processPath))
    {
        std::filesystem::create_directory(processPath);
        std::filesystem::permissions(
            processPath, std::filesystem::perms::owner_all, std::filesystem::perm_options::add);
    }
    ::setenv("FOO_HOME", processPath.string().c_str(), true);
}
// NOLINTEND(concurrency-mt-unsafe)

//! @brief The destructor function before finishing the main function. Check the signal status.
[[using gnu: destructor, noinline]] static void onFinal()
{
    if (signalStatus)
    {
        std::fprintf(::stdout, "%s: Signal %d was the last signal received.\n", executableName().c_str(), signalStatus);
    }
}
} // namespace application
