//! @file io.hpp
//! @author ryftchen
//! @brief The declarations (io) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#pragma once

#include <functional>
#include <istream>
#include <limits>
#include <list>
#include <memory>

//! @brief The utility module.
namespace utility // NOLINT (modernize-concat-nested-namespaces)
{
//! @brief I/O-operation-related functions in the utility module.
namespace io
{
extern const char* version() noexcept;

extern std::string executeCommand(const std::string& command, const std::uint32_t timeout = 0);
extern void waitForUserInput(const std::function<bool(const std::string&)>& action, const int timeout = -1);
extern std::list<std::string> getFileContents(
    const std::string& filename,
    const bool toLock,
    const bool toReverse = false,
    const std::uint64_t totalRows = std::numeric_limits<std::uint64_t>::max());

//! @brief Custom stream buffer for file descriptors.
class FDStreamBuffer : public std::streambuf
{
public:
    //! @brief Construct a new FDStreamBuffer object.
    //! @param fd - file descriptor to associate with the stream buffer
    explicit FDStreamBuffer(const int fd = -1) : fileDescriptor(fd) {}
    //! @brief Destroy the FDStreamBuffer object.
    ~FDStreamBuffer() override;

    //! @brief Get the file descriptor.
    //! @return file descriptor associated with the stream buffer
    [[nodiscard]] int fd() const;
    //! @brief Set the file descriptor.
    //! @param newFD - new file descriptor to associate with the stream buffer
    void fd(const int newFD);
    //! @brief Closes the file descriptor.
    void close();

private:
    //! @brief File descriptor associated with the stream buffer.
    int fileDescriptor{-1};
    //! @brief Buffer for reading data.
    std::unique_ptr<char[]> readBuffer{};
    //! @brief Buffer for writing data.
    std::unique_ptr<char[]> writeBuffer{};
    //! @brief Size of the buffer.
    static constexpr std::size_t bufferSize{4096};

    //! @brief Flush the output buffer with the file descriptor.
    //! @return the value is 0 if successful, otherwise -1
    int flush();

protected:
    //! @brief Read data into the input buffer.
    //! @return next character from the input buffer
    int_type underflow() override;
    //! @brief Write data from the output buffer.
    //! @param c - character to write to the buffer
    //! @return character written
    int_type overflow(int_type c) override;
    //! @brief Synchronize the output buffer with the file descriptor.
    //! @return the value is 0 if successful, otherwise -1
    int sync() override;
    //! @brief Set the position indicator for the stream buffer.
    //! @param off - offset to move the position indicator
    //! @param way - direction to move the position indicator
    //! @param mode - mode for seeking
    //! @return the value is the new position if successful, otherwise -1
    std::streampos seekoff(std::streamoff off, std::ios_base::seekdir way, std::ios_base::openmode mode) override;
    //! @brief Set the position indicator to an absolute position.
    //! @param sp - absolute position to set
    //! @param mode - mode for seeking
    //! @return the value is the new position if successful, otherwise -1
    std::streampos seekpos(std::streampos sp, std::ios_base::openmode mode) override;
    //! @brief Get the number of characters available for reading.
    //! @return number of characters available in the input buffer
    std::streamsize showmanyc() override;
};

//! @brief Handle reading from a file using a custom stream buffer.
class FileReader
{
public:
    //! @brief Construct a new FileReader object.
    //! @param filename - name of the file to be read
    explicit FileReader(const std::string& filename) : name(filename) {}
    //! @brief Destroy the FileReader object.
    ~FileReader();
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

    //! @brief Checks if the file is currently open.
    //! @return be open or not open
    bool isOpen() const;
    //! @brief Open the file for reading.
    void open();
    //! @brief Close the file.
    void close();
    //! @brief Lock the file for shared reading.
    void lock() const;
    //! @brief Unlock the file.
    void unlock() const;
    //! @brief Get the input stream.
    //! @return reference to the input stream
    std::istream& stream();

private:
    //! @brief Name of the file being read.
    const std::string name{};
    //! @brief File descriptor associated with the file.
    int fd{-1};
    //! @brief Custom stream buffer for reading.
    FDStreamBuffer strBuf{};
    //! @brief Input stream associated with the file.
    std::istream input{&strBuf};
};

//! @brief Handle writing from a file using a custom stream buffer.
class FileWriter
{
public:
    //! @brief Construct a new FileWriter object.
    //! @param filename - name of the file to be written
    explicit FileWriter(const std::string& filename) : name(filename) {}
    //! @brief Destroy the FileWriter object.
    ~FileWriter();
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

    //! @brief Checks if the file is currently open.
    //! @return be open or not open
    bool isOpen() const;
    //! @brief Open the file for reading.
    //! @param overwrite - overwrite or not
    void open(const bool overwrite = false);
    //! @brief Close the file.
    void close();
    //! @brief Lock the file for exclusive reading.
    void lock() const;
    //! @brief Unlock the file.
    void unlock() const;
    //! @brief Get the output stream.
    //! @return reference to the output stream
    std::ostream& stream();

private:
    //! @brief Name of the file being written.
    const std::string name{};
    //! @brief File descriptor associated with the file.
    int fd{-1};
    //! @brief Custom stream buffer for writing.
    FDStreamBuffer strBuf{};
    //! @brief Output stream associated with the file.
    std::ostream output{&strBuf};
};
} // namespace io
} // namespace utility
