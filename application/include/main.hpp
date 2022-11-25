#pragma once

#include <cxxabi.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <unistd.h>
#include <csignal>
#include <filesystem>

namespace app_main
{
[[using gnu: constructor]] static void switchToProjectPath();

static void signalHandler(int sig)
{
    void* callStack[128];
    const int maxFrame = sizeof(callStack) / sizeof(callStack[0]), numOfFrame = backtrace(callStack, maxFrame);
    char** symbols = backtrace_symbols(callStack, numOfFrame);
    char buffer[1024];
    std::ostringstream originalTrace, realTrace;

    for (int i = 1; i < numOfFrame; ++i)
    {
        originalTrace << symbols[i] << "\n";

        Dl_info info;
        if (dladdr(callStack[i], &info) && info.dli_sname)
        {
            char* demangle = nullptr;
            int status = -1;
            if ('_' == info.dli_sname[0])
            {
                demangle = abi::__cxa_demangle(info.dli_sname, nullptr, nullptr, &status);
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
            free(demangle); // NOLINT(cppcoreguidelines-no-malloc)
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
    free(symbols); // NOLINT(cppcoreguidelines-no-malloc)

    if (numOfFrame == maxFrame)
    {
        realTrace << "\r\n<TRUNCATED...>\n";
    }

    std::fprintf(
        stderr,
        "\r\n<SIGNAL %d>\r\n<ORIGINAL BACKTRACE>\r\n%s\r\n<REAL BACKTRACE>\r\n%s\n",
        sig,
        originalTrace.str().c_str(),
        realTrace.str().c_str());
    kill(getpid(), SIGKILL);
}

static void switchToProjectPath()
{
    std::signal(SIGSEGV, signalHandler);
    setenv("TERM", "linux", true);
    setenv("TERMINFO", "/etc/terminfo", true);

    const std::filesystem::path absolutePath = std::filesystem::canonical(std::filesystem::path{"/proc/self/exe"});
    const std::size_t pos = absolutePath.string().find("build");
    if (std::string::npos == pos)
    {
        std::fprintf(stderr, "The build directory isn't exist. Please check it.\n");
        std::exit(-1);
    }
    const std::filesystem::path buildPath(std::filesystem::path{absolutePath.string().substr(0, pos)});

    if (!buildPath.has_parent_path())
    {
        std::fprintf(stderr, "The project directory isn't exist. Please check it.\n");
        std::exit(-1);
    }
    std::filesystem::current_path(buildPath.parent_path());
}
} // namespace app_main
