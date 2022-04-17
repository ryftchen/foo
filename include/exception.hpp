#pragma once
#include <exception>
#include <string>

[[noreturn]] void throwLockFileException(
    const std::string& str, const bool isLock, const bool isReader);

class ExecuteCommandError : public std::exception
{
public:
    ExecuteCommandError() : message("Failed to execute command."){};
    explicit ExecuteCommandError(const std::string& str) :
        message("Failed to execute command " + str + "."){};
    ~ExecuteCommandError() noexcept override = default;
    [[nodiscard]] const char* what() const noexcept override;

private:
    std::string message;
};

class CallFunctionError : public std::exception
{
public:
    CallFunctionError() : message("Failed to call function."){};
    explicit CallFunctionError(const std::string& str) :
        message("Failed to call function " + str + "."){};
    ~CallFunctionError() noexcept override = default;
    [[nodiscard]] const char* what() const noexcept override;

private:
    std::string message;
};

class CreateFolderError : public std::exception
{
public:
    CreateFolderError() : message("Failed to create folder."){};
    explicit CreateFolderError(const std::string& str) :
        message("Failed to create folder " + str + "."){};
    ~CreateFolderError() noexcept override = default;
    [[nodiscard]] const char* what() const noexcept override;

private:
    std::string message;
};

class OpenFileError : public std::exception
{
public:
    OpenFileError() : message("Failed to open file."){};
    explicit OpenFileError(const std::string& str) : message("Failed to open file " + str + "."){};
    ~OpenFileError() noexcept override = default;
    [[nodiscard]] const char* what() const noexcept override;

private:
    std::string message;
};

class LockFileError : public std::exception
{
public:
    LockFileError() : message("Failed to lock/unlock reader/writer lock."){};
    explicit LockFileError(const std::string& str) : message("Failed to " + str){};
    ~LockFileError() noexcept override = default;
    [[nodiscard]] const char* what() const noexcept override;

private:
    std::string message;
};