//! @file io.hpp
//! @author ryftchen
//! @brief The declarations (io) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include <functional>
#include <istream>

//! @brief The utility module.
namespace utility // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief I/O-operation-related functions in the utility module.
namespace io
{
//! @brief Brief function description.
//! @return function description (module_function)
inline static const char* description() noexcept
{
    return "UTIL_IO";
}
extern const char* version() noexcept;

extern std::string executeCommand(const std::string& command);
extern void waitForUserInput(const std::function<bool(const std::string&)>& operation, const int timeout = -1);
extern std::vector<std::string> readFileLines(
    const std::string_view filename, const bool lock = false, const bool reverse = false, const int limit = -1);

//! @brief Custom stream buffer for file descriptors.
class FDStreamBuffer : public std::streambuf
{
public:
    //! @brief Construct a new FDStreamBuffer object.
    //! @param fd - file descriptor to associate with the stream buffer
    explicit FDStreamBuffer(const int fd = -1) : fd{fd} {}
    //! @brief Destroy the FDStreamBuffer object.
    ~FDStreamBuffer() override;
    //! @brief Construct a new FDStreamBuffer object.
    FDStreamBuffer(const FDStreamBuffer&) = delete;
    //! @brief Construct a new FDStreamBuffer object.
    FDStreamBuffer(FDStreamBuffer&&) noexcept = default;
    //! @brief The operator (=) overloading of FDStreamBuffer class.
    //! @return reference of the FDStreamBuffer object
    FDStreamBuffer& operator=(const FDStreamBuffer&) = delete;
    //! @brief The operator (=) overloading of FDStreamBuffer class.
    //! @return reference of the FDStreamBuffer object
    FDStreamBuffer& operator=(FDStreamBuffer&&) noexcept = default;

    //! @brief Set the file descriptor.
    //! @param newFD - new file descriptor to associate with the stream buffer
    void set(const int newFD);
    //! @brief Reset the file descriptor.
    void reset();

private:
    //! @brief File descriptor associated with the stream buffer.
    int fd{-1};
    //! @brief Size of the buffer.
    static constexpr std::size_t bufferSize{4096};
    //! @brief Buffer for reading data.
    std::array<char, bufferSize> readBuffer{};
    //! @brief Buffer for writing data.
    std::array<char, bufferSize> writeBuffer{};

    //! @brief Flush the output buffer with the file descriptor.
    //! @return 0 if successful, otherwise -1
    int flush();

protected:
    //! @brief Read data into the input buffer.
    //! @return next character from the input buffer
    int_type underflow() override;
    //! @brief Write data from the output buffer.
    //! @param c - character to write to the buffer
    //! @return character written
    int_type overflow(const int_type c) override;
    //! @brief Synchronize the output buffer with the file descriptor.
    //! @return 0 if successful, otherwise -1
    int sync() override;
    //! @brief Set the position indicator for the stream buffer.
    //! @param off - offset to move the position indicator
    //! @param way - direction to move the position indicator
    //! @param mode - mode for seeking
    //! @return new position if successful, otherwise -1
    std::streampos seekoff(
        const std::streamoff off, const std::ios_base::seekdir way, const std::ios_base::openmode mode) override;
    //! @brief Set the position indicator to an absolute position.
    //! @param sp - absolute position to set
    //! @param mode - mode for seeking
    //! @return new position if successful, otherwise -1
    std::streampos seekpos(const std::streampos sp, const std::ios_base::openmode mode) override;
    //! @brief Get the number of characters available for reading.
    //! @return number of characters available in the input buffer
    std::streamsize showmanyc() override;
};

//! @brief Handle reading from a file using a custom stream buffer.
class FileReader
{
public:
    //! @brief Construct a new FileReader object.
    //! @param filename - file path to be read
    explicit FileReader(const std::string_view filename) : name{filename} {}
    //! @brief Destroy the FileReader object.
    virtual ~FileReader();
    //! @brief Construct a new FileReader object.
    FileReader(const FileReader&) = delete;
    //! @brief Construct a new FileReader object.
    FileReader(FileReader&&) = delete;
    //! @brief The operator (=) overloading of FileReader class.
    //! @return reference of the FileReader object
    FileReader& operator=(const FileReader&) = delete;
    //! @brief The operator (=) overloading of FileReader class.
    //! @return reference of the FileReader object
    FileReader& operator=(FileReader&&) = delete;

    //! @brief Checks if the file is currently opened.
    //! @return be opened or not
    bool isOpened() const;
    //! @brief Open the file for reading.
    void open();
    //! @brief Close the file.
    void close();
    //! @brief Checks if the file is currently locked.
    //! @return be locked or not
    bool isLocked() const;
    //! @brief Lock the file for shared reading.
    void lock();
    //! @brief Unlock the file.
    void unlock();
    //! @brief Get the input stream.
    //! @return reference to the input stream
    std::istream& stream() noexcept;

private:
    //! @brief Name of the file being read.
    const std::string name;
    //! @brief File descriptor associated with the file.
    int fd{-1};
    //! @brief Custom stream buffer for reading.
    FDStreamBuffer strBuf;
    //! @brief Input stream associated with the file.
    std::istream input{&strBuf};
    //! @brief Locking status.
    bool lockActive{false};
};

//! @brief Handle writing from a file using a custom stream buffer.
class FileWriter
{
public:
    //! @brief Construct a new FileWriter object.
    //! @param filename - file path to be written
    explicit FileWriter(const std::string_view filename) : name{filename} {}
    //! @brief Destroy the FileWriter object.
    virtual ~FileWriter();
    //! @brief Construct a new FileWriter object.
    FileWriter(const FileWriter&) = delete;
    //! @brief Construct a new FileWriter object.
    FileWriter(FileWriter&&) = delete;
    //! @brief The operator (=) overloading of FileWriter class.
    //! @return reference of the FileWriter object
    FileWriter& operator=(const FileWriter&) = delete;
    //! @brief The operator (=) overloading of FileWriter class.
    //! @return reference of the FileWriter object
    FileWriter& operator=(FileWriter&&) = delete;

    //! @brief Checks if the file is currently opened.
    //! @return be opened or not
    bool isOpened() const;
    //! @brief Open the file for reading.
    //! @param overwrite - overwrite or not
    void open(const bool overwrite = false);
    //! @brief Close the file.
    void close();
    //! @brief Checks if the file is currently locked.
    //! @return be locked or not
    bool isLocked() const;
    //! @brief Lock the file for exclusive reading.
    void lock();
    //! @brief Unlock the file.
    void unlock();
    //! @brief Get the output stream.
    //! @return reference to the output stream
    std::ostream& stream() noexcept;

private:
    //! @brief Name of the file being written.
    const std::string name;
    //! @brief File descriptor associated with the file.
    int fd{-1};
    //! @brief Custom stream buffer for writing.
    FDStreamBuffer strBuf;
    //! @brief Output stream associated with the file.
    std::ostream output{&strBuf};
    //! @brief Locking status.
    bool lockActive{false};
};
} // namespace io
} // namespace utility
