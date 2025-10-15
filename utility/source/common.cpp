//! @file common.cpp
//! @author ryftchen
//! @brief The definitions (common) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "common.hpp"

#include <cstdarg>
#include <vector>

namespace utility::common
{
//! @brief Function version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    static const char* const ver = "0.1.0";
    return ver;
}

//! @brief Anonymous namespace.
inline namespace
{
//! @brief The Base64 alphabet.
constexpr std::string_view base64Alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                            "abcdefghijklmnopqrstuvwxyz"
                                            "0123456789+/";
} // namespace

//! @brief The Brian-Kernighan Dennis-Ritchie hash function.
//! @param str - input data
//! @return hash value
std::size_t bkdrHash(const char* str) noexcept
{
    if (!str)
    {
        return 0;
    }

    std::size_t hash = 0;
    while (*str)
    {
        hash = hash * bkdrHashSeed + (*str++);
    }

    return hash & bkdrHashMask;
}

// NOLINTBEGIN(readability-magic-numbers)
//! @brief Base64 encoding.
//! @param data - decoded data
//! @return encoded data
std::string base64Encode(const std::string_view data)
{
    std::size_t counter = 0, offset = 0;
    std::uint32_t bitStream = 0;
    std::string encoded{};

    for (const auto c : data)
    {
        const auto numVal = static_cast<unsigned int>(static_cast<unsigned char>(c));
        offset = 16 - counter % 3 * 8;
        bitStream += numVal << offset;
        switch (offset)
        {
            case 16:
                encoded += base64Alphabet.at((bitStream >> 18) & 0x3F);
                break;
            case 8:
                encoded += base64Alphabet.at((bitStream >> 12) & 0x3F);
                break;
            case 0:
                if (counter != 3)
                {
                    encoded += base64Alphabet.at((bitStream >> 6) & 0x3F);
                    encoded += base64Alphabet.at(bitStream & 0x3F);
                    bitStream = 0;
                }
                break;
            default:
                break;
        }
        ++counter;
    }

    switch (offset)
    {
        case 16:
            encoded += base64Alphabet.at((bitStream >> 12) & 0x3F);
            encoded += "==";
            break;
        case 8:
            encoded += base64Alphabet.at((bitStream >> 6) & 0x3F);
            encoded += '=';
            break;
        default:
            break;
    }

    return encoded;
}

//! @brief Base64 decoding.
//! @param data - encoded data
//! @return decoded data
std::string base64Decode(const std::string_view data)
{
    std::size_t counter = 0, offset = 0;
    std::uint32_t bitStream = 0;
    std::string decoded{};

    for (const auto c : data)
    {
        const auto uc = static_cast<unsigned char>(c);
        if (const auto numVal = base64Alphabet.find(uc); numVal != std::string::npos)
        {
            offset = 18 - counter % 4 * 6;
            bitStream += numVal << offset;
            switch (offset)
            {
                case 12:
                    decoded += static_cast<char>((bitStream >> 16) & 0xFF);
                    break;
                case 6:
                    decoded += static_cast<char>((bitStream >> 8) & 0xFF);
                    break;
                case 0:
                    if (counter != 4)
                    {
                        decoded += static_cast<char>(bitStream & 0xFF);
                        bitStream = 0;
                    }
                    break;
                default:
                    break;
            }
        }
        else if (uc != '=')
        {
            throw std::runtime_error{"Invalid base64 encoded data."};
        }
        ++counter;
    }

    return decoded;
}
// NOLINTEND(readability-magic-numbers)

//! @brief Format as a string (printf style).
//! @param fmt - null-terminated multibyte string specifying how to interpret the data
//! @param ... - arguments
//! @return string after formatting
std::string printfString(const char* const fmt, ...) // NOLINT(cert-dcl50-cpp)
{
    if (!fmt)
    {
        return {};
    }

    std::va_list argList{};
    ::va_start(argList, fmt);
    const int reservedSize = std::vsnprintf(nullptr, 0, fmt, argList);
    ::va_end(argList);
    if (reservedSize < 0)
    {
        throw std::runtime_error{"Unable to reserve size for formatting string."};
    }

    ::va_start(argList, fmt);
    std::vector<char> buffer(reservedSize + 1);
    std::vsnprintf(buffer.data(), reservedSize + 1, fmt, argList);
    ::va_end(argList);

    return std::string{buffer.cbegin(), buffer.cbegin() + reservedSize};
}

void SpinLock::lock()
{
    while (flag.test_and_set(std::memory_order_acquire))
    {
        std::this_thread::yield();
    }
}

void SpinLock::unlock()
{
    flag.clear(std::memory_order_release);
}

bool SpinLock::tryLock()
{
    return !flag.test_and_set(std::memory_order_acquire);
}

void ReadWriteLock::readLock()
{
    const std::shared_lock<std::shared_mutex> rLock(rwLock);
    if (std::unique_lock<std::mutex> lock(mtx); true)
    {
        cond.wait(lock, [this]() { return writer.load() == 0; });
        reader.fetch_add(1);
    }
}

void ReadWriteLock::readUnlock()
{
    std::unique_lock<std::mutex> lock(mtx);
    reader.fetch_sub(1);
    lock.unlock();
    cond.notify_all();
}

void ReadWriteLock::writeLock()
{
    const std::unique_lock<std::shared_mutex> wLock(rwLock);
    if (std::unique_lock<std::mutex> lock(mtx); true)
    {
        cond.wait(lock, [this]() { return (reader.load() == 0) && (writer.load() == 0); });
        writer.fetch_add(1);
    }
}

void ReadWriteLock::writeUnlock()
{
    std::unique_lock<std::mutex> lock(mtx);
    writer.fetch_sub(1);
    lock.unlock();
    cond.notify_all();
}

LockGuard::LockGuard(ReadWriteLock& lock, const LockMode mode) : lock{lock}, mode{mode}
{
    switch (mode)
    {
        case LockMode::read:
            lock.readLock();
            break;
        case LockMode::write:
            lock.writeLock();
            break;
        default:
            break;
    }
}

LockGuard::~LockGuard()
{
    switch (mode)
    {
        case LockMode::read:
            lock.readUnlock();
            break;
        case LockMode::write:
            lock.writeUnlock();
            break;
        default:
            break;
    }
}
} // namespace utility::common
