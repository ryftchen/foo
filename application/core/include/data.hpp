//! @file data.hpp
//! @author ryftchen
//! @brief The declarations (data) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#ifndef _PRECOMPILED_HEADER
#include <netinet/in.h>
#include <string>
#include <vector>
#else
#include "application/pch/precompiled_header.hpp"
#endif // _PRECOMPILED_HEADER

//! @brief The application module.
namespace application // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Data-processing-related functions in the application module.
namespace data
{
//! @brief Data packet.
class Packet
{
public:
    //! @brief Construct a new Packet object.
    //! @param pktBuf - packet buffer
    //! @param pktLen - buffer length
    Packet(char* const pktBuf, const std::size_t pktLen);

    //! @brief Write data to the packet buffer.
    //! @tparam Data - type of data to be written
    //! @param data - original data
    //! @return whether it is continuously writable
    template <typename Data>
    bool write(const Data data);
    //! @brief Write data to the packet buffer.
    //! @param dst - data after conversion
    //! @param offset - data offset
    //! @return whether it is continuously writable
    bool write(const void* const dst, const std::size_t offset);
    //! @brief Read data to the packet buffer.
    //! @tparam Data - type of data to be read
    //! @param data - original data
    //! @return whether it is continuously readable
    template <typename Data>
    bool read(Data* const data);
    //! @brief Read data to the packet buffer.
    //! @param dst - data after conversion
    //! @param offset - data offset
    //! @return whether it is continuously readable
    bool read(void* const dst, const std::size_t offset);

private:
    //! @brief Pointer to the beginning of the buffer.
    char* const head{nullptr};
    //! @brief Pointer to the ending of the buffer.
    const char* const tail{nullptr};
    //! @brief Pointer to the current writing location.
    char* writer{nullptr};
    //! @brief Pointer to the current reading location.
    const char* reader{nullptr};
};

template <typename Data>
bool Packet::write(const Data data)
{
    Data temp{};
    if constexpr (sizeof(Data) == sizeof(int))
    {
        temp = ::htonl(data);
    }
    else if constexpr (sizeof(Data) == sizeof(short))
    {
        temp = ::htons(data);
    }
    else
    {
        temp = data;
    }
    return write(&temp, sizeof(Data));
}

template <typename Data>
bool Packet::read(Data* const data)
{
    if (!data)
    {
        return false;
    }

    const bool isEnd = read(data, sizeof(Data));
    if constexpr (sizeof(Data) == sizeof(int))
    {
        *data = ::ntohl(*data);
    }
    else if constexpr (sizeof(Data) == sizeof(short))
    {
        *data = ::ntohs(*data);
    }
    return isEnd;
}

extern void encryptMessage(char* const buffer, const std::size_t length);
extern void decryptMessage(char* const buffer, const std::size_t length);

extern void compressData(std::vector<char>& cache);
extern void decompressData(std::vector<char>& cache);

extern std::string toHexString(const char* const buffer, const std::size_t length);
} // namespace data
} // namespace application
