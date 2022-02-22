#include "../include/exception.hpp"

const char *RunScriptError::what() const throw()
{
    return message.c_str();
}

const char *CreateFolderError::what() const throw()
{
    return message.c_str();
}

const char *OpenFileError::what() const throw()
{
    return message.c_str();
}

const char *LockReaderLockError::what() const throw()
{
    return message.c_str();
}

const char *UnlockReaderLockError::what() const throw()
{
    return message.c_str();
}

const char *LockWriterLockError::what() const throw()
{
    return message.c_str();
}

const char *UnlockWriterLockError::what() const throw()
{
    return message.c_str();
}
