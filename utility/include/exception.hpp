#pragma once
#include <string>

[[noreturn]] void throwLogicErrorException(const std::string& str);
[[noreturn]] void throwRunCommandLineException(const std::string& str);
[[noreturn]] void throwCallSystemApiException(const std::string& str);
[[noreturn]] void throwOperateFileException(const std::string& name, const bool isToOpen);
[[noreturn]] void throwOperateLockException(
    const std::string& name, const bool isToLock, const bool isReader);
