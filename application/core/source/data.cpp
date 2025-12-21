//! @file data.cpp
//! @author ryftchen
//! @brief The definitions (data) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "data.hpp"

#ifndef _PRECOMPILED_HEADER
#include <openssl/evp.h>
#include <lz4.h>
#include <array>
#include <cstring>
#include <iomanip>
#else
#include "application/pch/precompiled_header.hpp"
#endif // _PRECOMPILED_HEADER

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
