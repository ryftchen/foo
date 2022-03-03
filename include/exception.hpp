#pragma once
#include <exception>
#include <string>

class RunCommandError : public std::exception
{
public:
    RunCommandError() : message("Failed to run command."){};
    explicit RunCommandError(const std::string &str)
        : message("Failed to run command " + str + "."){};
    ~RunCommandError() noexcept override {}
    [[nodiscard]] const char *what() const noexcept override;

private:
    std::string message;
};

class CreateFolderError : public std::exception
{
public:
    CreateFolderError() : message("Failed to create folder."){};
    explicit CreateFolderError(const std::string &str)
        : message("Failed to create folder " + str + "."){};
    ~CreateFolderError() noexcept override {}
    [[nodiscard]] const char *what() const noexcept override;

private:
    std::string message;
};

class OpenFileError : public std::exception
{
public:
    OpenFileError() : message("Failed to open file."){};
    explicit OpenFileError(const std::string &str) : message("Failed to open file " + str + "."){};
    ~OpenFileError() noexcept override {}
    [[nodiscard]] const char *what() const noexcept override;

private:
    std::string message;
};

class LockReaderLockError : public std::exception
{
public:
    LockReaderLockError() : message("Failed to lock reader lock."){};
    explicit LockReaderLockError(const std::string &str)
        : message("Failed to lock reader lock " + str + "."){};
    ~LockReaderLockError() noexcept override {}
    [[nodiscard]] const char *what() const noexcept override;

private:
    std::string message;
};

class UnlockReaderLockError : public std::exception
{
public:
    UnlockReaderLockError() : message("Failed to unlock reader lock."){};
    explicit UnlockReaderLockError(const std::string &str)
        : message("Failed to unlock reader lock " + str + "."){};
    ~UnlockReaderLockError() noexcept override {}
    [[nodiscard]] const char *what() const noexcept override;

private:
    std::string message;
};

class LockWriterLockError : public std::exception
{
public:
    LockWriterLockError() : message("Failed to lock writer lock."){};
    explicit LockWriterLockError(const std::string &str)
        : message("Failed to lock writer lock " + str + "."){};
    ~LockWriterLockError() noexcept override {}
    [[nodiscard]] const char *what() const noexcept override;

private:
    std::string message;
};

class UnlockWriterLockError : public std::exception
{
public:
    UnlockWriterLockError() : message("Failed to unlock writer lock."){};
    explicit UnlockWriterLockError(const std::string &str)
        : message("Failed to unlock writer lock " + str + "."){};
    ~UnlockWriterLockError() noexcept override {}
    [[nodiscard]] const char *what() const noexcept override;

private:
    std::string message;
};
