//! @file data.hpp
//! @author ryftchen
//! @brief The declarations (data) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2026 ryftchen. All rights reserved.

#pragma once

#ifndef _PRECOMPILED_HEADER
#include <netinet/in.h>
#include <cstdint>
#include <system_error>
#include <vector>
#else
#include "application/pch/precompiled_header.hpp"
#endif

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
    requires std::is_trivially_copyable_v<Data>
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
    requires std::is_trivially_copyable_v<Data>
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
requires std::is_trivially_copyable_v<Data>
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
requires std::is_trivially_copyable_v<Data>
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

//! @brief Type-length-value scheme.
namespace tlv
{
//! @brief Invalid shared memory id.
inline constexpr int invalidShmId = -1;
//! @brief Default information size.
inline constexpr std::uint16_t defInfoSize = 256;

//! @brief Value in TLV.
struct TLVValue
{
    //! @brief Flag for stopping the connection.
    bool stopTag{false};
    //! @brief Information about the runtime library.
    char libInfo[defInfoSize]{'\0'};
    //! @brief Shared memory id of the bash outputs.
    int bashShmId{invalidShmId};
    //! @brief Shared memory id of the log contents.
    int logShmId{invalidShmId};
    //! @brief Shared memory id of the status reports.
    int statusShmId{invalidShmId};
    //! @brief Information about the current configuration.
    char configInfo[defInfoSize * 2]{'\0'};
};

extern std::error_code encodeTLV(char* const buf, std::size_t& len, const TLVValue& val);
extern std::error_code decodeTLV(char* const buf, const std::size_t len, TLVValue& val);
} // namespace tlv

extern void encryptMessage(char* const buffer, const std::size_t length);
extern void decryptMessage(char* const buffer, const std::size_t length);

extern void compressData(std::vector<char>& cache);
extern void decompressData(std::vector<char>& cache);

extern std::string toHexString(const char* const buffer, const std::size_t length);
} // namespace data
} // namespace application
