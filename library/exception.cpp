#include "exception.hpp"

const char *CreateFolderError::what() const noexcept
{
    return message.c_str();
}

const char *OpenFileError::what() const noexcept
{
    return message.c_str();
}

const char *LockReaderLockError::what() const noexcept
{
    return message.c_str();
}

const char *UnlockReaderLockError::what() const noexcept
{
    return message.c_str();
}

const char *LockWriterLockError::what() const noexcept
{
    return message.c_str();
}

const char *UnlockWriterLockError::what() const noexcept
{
    return message.c_str();
}
