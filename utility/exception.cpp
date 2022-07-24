#include "exception.hpp"

const char* ExecuteCommandError::what() const noexcept
{
    return message.c_str();
}

const char* CallFunctionError::what() const noexcept
{
    return message.c_str();
}

const char* OpenFileError::what() const noexcept
{
    return message.c_str();
}

const char* LockFileError::what() const noexcept
{
    return message.c_str();
}

[[noreturn]] void throwLockFileException(
    const std::string& str, const bool isToLock, const bool isReader)
{
    std::string operate, type;
    isToLock ? operate = "lock" : operate = "unlock";
    isReader ? type = "reader" : type = "writer";

    throw LockFileError(operate + " " + type + " lock " + str + ".");
}
