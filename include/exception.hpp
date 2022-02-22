#pragma once
#include <exception>
#include <string>

class RunScriptError: public std::exception
{
public:
    RunScriptError(): message("Failed to run script.") {};
    explicit RunScriptError(const std::string &str):
        message("Failed to run script " + str + ".") {};
    virtual ~RunScriptError() throw() {}
    virtual const char *what() const throw();
private:
    std::string message;
};

class CreateFolderError: public std::exception
{
public:
    CreateFolderError(): message("Failed to create folder.") {};
    explicit CreateFolderError(const std::string &str):
        message("Failed to create folder " + str + ".") {};
    virtual ~CreateFolderError() throw() {}
    virtual const char *what() const throw();
private:
    std::string message;
};

class OpenFileError: public std::exception
{
public:
    OpenFileError(): message("Failed to open file.") {};
    explicit OpenFileError(const std::string &str): message("Failed to open file " + str + ".") {};
    virtual ~OpenFileError() throw() {}
    virtual const char *what() const throw();
private:
    std::string message;
};

class LockReaderLockError: public std::exception
{
public:
    LockReaderLockError(): message("Failed to lock reader lock.") {};
    explicit LockReaderLockError(const std::string &str):
        message("Failed to lock reader lock " + str + ".") {};
    virtual ~LockReaderLockError() throw() {}
    virtual const char *what() const throw();
private:
    std::string message;
};

class UnlockReaderLockError: public std::exception
{
public:
    UnlockReaderLockError(): message("Failed to unlock reader lock.") {};
    explicit UnlockReaderLockError(const std::string &str):
        message("Failed to unlock reader lock " + str + ".") {};
    virtual ~UnlockReaderLockError() throw() {}
    virtual const char *what() const throw();
private:
    std::string message;
};

class LockWriterLockError: public std::exception
{
public:
    LockWriterLockError(): message("Failed to lock writer lock.") {};
    explicit LockWriterLockError(const std::string &str):
        message("Failed to lock writer lock " + str + ".") {};
    virtual ~LockWriterLockError() throw() {}
    virtual const char *what() const throw();
private:
    std::string message;
};

class UnlockWriterLockError: public std::exception
{
public:
    UnlockWriterLockError(): message("Failed to unlock writer lock.") {};
    explicit UnlockWriterLockError(const std::string &str):
        message("Failed to unlock writer lock " + str + ".") {};
    virtual ~UnlockWriterLockError() throw() {}
    virtual const char *what() const throw();
private:
    std::string message;
};
