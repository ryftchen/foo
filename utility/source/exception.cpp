#include "exception.hpp"
#include <stdexcept>

void throwLogicErrorException(const std::string& str)
{
    throw std::logic_error("Logic error, because: " + str);
}

void throwRunCommandLineException(const std::string& str)
{
    throw std::runtime_error("Failed to run common line: " + str);
}

void throwCallSystemApiException(const std::string& str)
{
    throw std::runtime_error("Failed to call system api: " + str);
}

void throwOperateLockException(const std::string& name, const bool isToLock, const bool isReader)
{
    std::string operate = isToLock ? "lock" : "unlock";
    std::string type = isReader ? "reader" : "writer";
    throw std::runtime_error("Failed to " + operate + " " + type + " lock: " + name);
}

void throwOperateFileException(const std::string& name, const bool isToOpen)
{
    std::string operate = isToOpen ? "open" : "unlock";
    throw std::runtime_error("Failed to " + operate + " file: " + name);
}
