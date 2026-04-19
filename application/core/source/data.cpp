//! @file data.cpp
//! @author ryftchen
//! @brief The definitions (data) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2026 ryftchen. All rights reserved.

#include "data.hpp"

#ifndef _PRECOMPILED_HEADER
#include <openssl/evp.h>
#include <lz4.h>
#include <array>
#include <cstring>
#include <iomanip>
#else
#include "application/pch/precompiled_header.hpp"
#endif

namespace application::data
{
Packet::Packet(char* const pktBuf, const std::size_t pktLen) :
    head{pktBuf}, tail{head + pktLen}, writer{head}, reader{head}
{
    if (!pktBuf)
    {
        throw std::runtime_error{"The attempt to create the data packet failed."};
    }
}

bool Packet::write(const void* const dst, const std::size_t offset)
{
    if (!dst || (offset == 0))
    {
        return false;
    }

    std::memcpy(writer, dst, offset);
    writer += offset;
    return writer < tail;
}

bool Packet::read(void* const dst, const std::size_t offset)
{
    if (!dst || (offset == 0))
    {
        return false;
    }

    std::memcpy(dst, reader, offset);
    reader += offset;
    return reader < tail;
}

namespace tlv
{
//! @brief Enumerate the types in TLV.
enum TLVType : int
{
    //! @brief Header.
    header = 0x3B9ACA07,
    //! @brief Stop.
    stop = 0,
    //! @brief Depend.
    depend = 1,
    //! @brief Execute.
    execute = 2,
    //! @brief Journal.
    journal = 3,
    //! @brief Monitor.
    monitor = 4,
    //! @brief Profile.
    profile = 5
};

//! @brief TLV value serialization.
//! @tparam Data - type of target payload
//! @param pkt - encoding packet that was filled type
//! @param val - value of TLV to encode
//! @param pl - target payload that has been included in the value of TLV
//! @return summary offset of length-value
template <typename Data>
static int serialize(data::Packet& pkt, const TLVValue& val, const Data TLVValue::* const pl)
{
    if (!pl)
    {
        return 0;
    }

    constexpr int length = sizeof(Data);
    pkt.write<int>(length);
    pkt.write<Data>(val.*pl);
    return static_cast<int>(sizeof(int) + length);
}

//! @brief TLV value serialization.
//! @tparam Size - size of target payload
//! @param pkt - encoding packet that was filled type
//! @param val - value of TLV to encode
//! @param pl - target payload that has been included in the value of TLV
//! @return summary offset of length-value
template <std::size_t Size>
static int serialize(data::Packet& pkt, const TLVValue& val, const char (TLVValue::* const pl)[Size])
{
    if (!pl)
    {
        return 0;
    }

    const int length = ::strnlen(val.*pl, Size);
    pkt.write<int>(length);
    pkt.write(val.*pl, length);
    return static_cast<int>(sizeof(int) + length);
}

//! @brief TLV value deserialization.
//! @tparam Data - type of target payload
//! @param pkt - decoding packet that was filled type
//! @param val - value of TLV to decode
//! @param pl - target payload that has been included in the value of TLV
//! @return summary offset of length-value
template <typename Data>
static int deserialize(data::Packet& pkt, TLVValue& val, Data TLVValue::* const pl)
{
    if (!pl)
    {
        return 0;
    }

    int length = 0;
    pkt.read<int>(&length);
    pkt.read<Data>(&(val.*pl));
    return static_cast<int>(sizeof(int) + length);
}

//! @brief TLV value deserialization.
//! @tparam Size - size of target payload
//! @param pkt - decoding packet that was filled type
//! @param val - value of TLV to decode
//! @param pl - target payload that has been included in the value of TLV
//! @return summary offset of length-value
template <std::size_t Size>
static int deserialize(data::Packet& pkt, TLVValue& val, char (TLVValue::* const pl)[Size])
{
    if (!pl)
    {
        return 0;
    }

    int length = 0;
    pkt.read<int>(&length);
    pkt.read(val.*pl, std::min<std::size_t>(length, Size));
    return static_cast<int>(sizeof(int) + length);
}

//! @brief Enumerate the error codes for TLV error handling.
enum class ErrCode : std::uint8_t
{
    //! @brief No error.
    noError = 0,
    //! @brief Null buffer.
    nullBuffer,
    //! @brief Insufficient length.
    insufficientLength,
    //! @brief Bad header.
    badHeader,
    //! @brief Unknown type.
    unknownType,
    //! @brief Fail serialize.
    failSerialize,
    //! @brief Fail deserialize.
    failDeserialize,
};

//! @brief Error category for TLV error handling.
class ErrCategory : public std::error_category
{
public:
    //! @brief Get the name of the error category.
    //! @return name of the error category
    [[nodiscard]] const char* name() const noexcept override { return "TLV error category"; }
    //! @brief Get the message of the error code value.
    //! @param value - error code value
    //! @return message of the error code value
    [[nodiscard]] std::string message(const int value) const override
    {
        switch (static_cast<ErrCode>(value))
        {
            case ErrCode::noError:
                return "no TLV error";
            case ErrCode::nullBuffer:
                return "null TLV buffer";
            case ErrCode::insufficientLength:
                return "insufficient TLV-length";
            case ErrCode::badHeader:
                return "invalid TLV header";
            case ErrCode::unknownType:
                return "unknown TLV-type";
            case ErrCode::failSerialize:
                return "TLV-value serialization failure";
            case ErrCode::failDeserialize:
                return "TLV-value deserialization failure";
            default:
                break;
        }
        return "unknown TLV error";
    }
};

//! @brief Make error code from the custom error code for TLV error handling.
//! @param errCode - custom error code
//! @return error code
[[maybe_unused]] static std::error_code make_error_code(const ErrCode errCode) // NOLINT(readability-identifier-naming)
{
    static const ErrCategory errCategory{};
    return std::error_code{static_cast<int>(errCode), errCategory};
}
} // namespace tlv
} // namespace application::data

template <>
struct std::is_error_code_enum<application::data::tlv::ErrCode> : public std::true_type
{
};

namespace application::data
{
namespace tlv
{
//! @brief Encode the TLV packet.
//! @param buf - TLV packet buffer
//! @param len - buffer length
//! @param val - value of TLV to encode
//! @return error code
std::error_code encodeTLV(char* const buf, std::size_t& len, const TLVValue& val)
{
    if (!buf)
    {
        return ErrCode::nullBuffer;
    }

    data::Packet enc(buf, len);
    int sum = 0;
    if (!enc.write<int>(TLVType::header) || !enc.write<int>(sum))
    {
        return ErrCode::insufficientLength;
    }

    const auto serializer = [&enc, &val, &sum](const auto pl) -> std::error_code
    {
        if (const int res = serialize(enc, val, pl); res > 0)
        {
            sum += sizeof(int) + res;
            return ErrCode::noError;
        }
        return ErrCode::failSerialize;
    };
    enc.write<int>(TLVType::stop);
    if (const auto ec = serializer(&TLVValue::stopTag); ec)
    {
        return ec;
    }
    enc.write<int>(TLVType::depend);
    if (const auto ec = serializer(&TLVValue::libInfo); ec)
    {
        return ec;
    }
    enc.write<int>(TLVType::execute);
    if (const auto ec = serializer(&TLVValue::bashShmId); ec)
    {
        return ec;
    }
    enc.write<int>(TLVType::journal);
    if (const auto ec = serializer(&TLVValue::logShmId); ec)
    {
        return ec;
    }
    enc.write<int>(TLVType::monitor);
    if (const auto ec = serializer(&TLVValue::statusShmId); ec)
    {
        return ec;
    }
    enc.write<int>(TLVType::profile);
    if (const auto ec = serializer(&TLVValue::configInfo); ec)
    {
        return ec;
    }

    auto temp = static_cast<int>(::htonl(sum));
    std::memcpy(buf + sizeof(int), &temp, sizeof(temp));
    len = sizeof(int) + sizeof(int) + sum;
    return ErrCode::noError;
}

//! @brief Decode the TLV packet.
//! @param buf - TLV packet buffer
//! @param len - buffer length
//! @param val - value of TLV to decode
//! @return error code
std::error_code decodeTLV(char* const buf, const std::size_t len, TLVValue& val)
{
    if (!buf || (len == 0))
    {
        return ErrCode::nullBuffer;
    }

    data::Packet dec(buf, len);
    int type = 0;
    int sum = 0;
    if (!dec.read<int>(&type) || !dec.read<int>(&sum))
    {
        return ErrCode::insufficientLength;
    }
    if (type != TLVType::header)
    {
        return ErrCode::badHeader;
    }

    const auto deserializer = [&dec, &val, &sum](const auto pl) -> std::error_code
    {
        if (const int res = deserialize(dec, val, pl); res > 0)
        {
            sum -= sizeof(int) + res;
            return ErrCode::noError;
        }
        return ErrCode::failDeserialize;
    };
    while (sum > 0)
    {
        dec.read<int>(&type);
        std::error_code ec = ErrCode::noError;
        switch (type)
        {
            case TLVType::stop:
                ec = deserializer(&TLVValue::stopTag);
                break;
            case TLVType::depend:
                ec = deserializer(&TLVValue::libInfo);
                break;
            case TLVType::execute:
                ec = deserializer(&TLVValue::bashShmId);
                break;
            case TLVType::journal:
                ec = deserializer(&TLVValue::logShmId);
                break;
            case TLVType::monitor:
                ec = deserializer(&TLVValue::statusShmId);
                break;
            case TLVType::profile:
                ec = deserializer(&TLVValue::configInfo);
                break;
            default:
                ec = ErrCode::unknownType;
                break;
        }
        if (ec)
        {
            return ec;
        }
    }
    return ErrCode::noError;
}
} // namespace tlv

// NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
//! @brief Encrypt the message with AES-128-CFB-128.
//! @param buffer - message buffer
//! @param length - buffer length
void encryptMessage(char* const buffer, const std::size_t length)
{
    if (!buffer || (length == 0))
    {
        return;
    }

    auto* const ctx = ::EVP_CIPHER_CTX_new();
    do
    {
        if (constexpr std::array<unsigned char, 16> key =
                {0x37, 0x47, 0x10, 0x33, 0x6F, 0x18, 0xC8, 0x9A, 0x4B, 0xC1, 0x2B, 0x97, 0x92, 0x19, 0x25, 0x6D},
            iv = {0x9F, 0x7B, 0x0E, 0x68, 0x2D, 0x2F, 0x4E, 0x7F, 0x1A, 0xFA, 0x61, 0xD3, 0xC6, 0x18, 0xF4, 0xC1};
            ::EVP_EncryptInit_ex(ctx, ::EVP_aes_128_cfb128(), nullptr, key.data(), iv.data()) == 0)
        {
            break;
        }

        int outLen = 0;
        if (::EVP_EncryptUpdate(
                ctx,
                reinterpret_cast<unsigned char*>(buffer),
                &outLen,
                reinterpret_cast<unsigned char*>(buffer),
                static_cast<int>(length))
            == 0)
        {
            break;
        }

        if (int tempLen = 0;
            ::EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(buffer) + outLen, &tempLen) == 0)
        {
            break;
        }
    }
    while (false);
    ::EVP_CIPHER_CTX_free(ctx);
}

//! @brief Decrypt the message with AES-128-CFB-128.
//! @param buffer - message buffer
//! @param length - buffer length
void decryptMessage(char* const buffer, const std::size_t length)
{
    if (!buffer || (length == 0))
    {
        return;
    }

    auto* const ctx = ::EVP_CIPHER_CTX_new();
    do
    {
        if (constexpr std::array<unsigned char, 16> key =
                {0x37, 0x47, 0x10, 0x33, 0x6F, 0x18, 0xC8, 0x9A, 0x4B, 0xC1, 0x2B, 0x97, 0x92, 0x19, 0x25, 0x6D},
            iv = {0x9F, 0x7B, 0x0E, 0x68, 0x2D, 0x2F, 0x4E, 0x7F, 0x1A, 0xFA, 0x61, 0xD3, 0xC6, 0x18, 0xF4, 0xC1};
            ::EVP_DecryptInit_ex(ctx, ::EVP_aes_128_cfb128(), nullptr, key.data(), iv.data()) == 0)
        {
            break;
        }

        int outLen = 0;
        if (::EVP_DecryptUpdate(
                ctx,
                reinterpret_cast<unsigned char*>(buffer),
                &outLen,
                reinterpret_cast<unsigned char*>(buffer),
                static_cast<int>(length))
            == 0)
        {
            break;
        }

        if (int tempLen = 0;
            ::EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(buffer) + outLen, &tempLen) == 0)
        {
            break;
        }
    }
    while (false);
    ::EVP_CIPHER_CTX_free(ctx);
}
// NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)

//! @brief Compress the data with LZ4.
//! @param cache - data cache
void compressData(std::vector<char>& cache)
{
    if (cache.empty())
    {
        return;
    }

    const int compressedCap = ::LZ4_compressBound(static_cast<int>(cache.size()));
    std::vector<char> compressed(compressedCap);
    const int compressedSize =
        ::LZ4_compress_default(cache.data(), compressed.data(), static_cast<int>(cache.size()), compressedCap);
    if (compressedSize < 0)
    {
        throw std::runtime_error{"Failed to compress data, return " + std::to_string(compressedSize) + '.'};
    }
    compressed.resize(compressedSize);
    compressed.shrink_to_fit();
    cache = std::move(compressed);
}

//! @brief Decompress the data with LZ4.
//! @param cache - data cache
void decompressData(std::vector<char>& cache)
{
    if (cache.empty())
    {
        return;
    }

    constexpr int decompressedCap = 65536 * 10 * 10;
    std::vector<char> decompressed(decompressedCap);
    const int decompressedSize =
        ::LZ4_decompress_safe(cache.data(), decompressed.data(), static_cast<int>(cache.size()), decompressedCap);
    if (decompressedSize < 0)
    {
        throw std::runtime_error{"Failed to decompress data, return " + std::to_string(decompressedSize) + '.'};
    }
    decompressed.resize(decompressedSize);
    decompressed.shrink_to_fit();
    cache = std::move(decompressed);
}

//! @brief Convert a byte buffer to a space-separated hexadecimal string.
//! @param buffer - byte buffer
//! @param length - buffer length
//! @return hexadecimal string representation of the buffer
std::string toHexString(const char* const buffer, const std::size_t length)
{
    if (!buffer || (length == 0))
    {
        return {};
    }

    std::ostringstream body{};
    for (std::size_t i = 0; i < length; ++i)
    {
        body << "0x" << std::setfill('0') << std::setw(2) << std::hex
             << static_cast<int>(static_cast<unsigned char>(buffer[i]));
        if ((i + 1) != length)
        {
            body << ' ';
        }
    }
    return body.str();
}
} // namespace application::data
