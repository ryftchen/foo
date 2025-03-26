//! @file view.cpp
//! @author ryftchen
//! @brief The definitions (view) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "view.hpp"
#include "log.hpp"

#ifndef __PRECOMPILED_HEADER
#include <openssl/evp.h>
#include <readline/readline.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <lz4.h>
#include <mpfr.h>
#if defined(__has_include) && __has_include(<ncurses.h>)
#include <ncurses.h>
#endif // defined(__has_include) && __has_include(<ncurses.h>)
#include <cassert>
#include <iterator>
#include <numeric>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

namespace application::view
{
namespace tlv
{
//! @brief Invalid shared memory id in TLV value.
constexpr int invalidShmId = -1;
//! @brief Default information size in TLV value.
constexpr std::uint16_t defaultInfoSize = 256;

//! @brief Value in TLV.
struct TLVValue
{
    //! @brief Flag for stopping the connection.
    bool stopTag{false};
    //! @brief Information about the runtime library.
    char libInfo[defaultInfoSize]{'\0'};
    //! @brief Shared memory id of the bash outputs.
    int bashShmId{invalidShmId};
    //! @brief Shared memory id of the log contents.
    int logShmId{invalidShmId};
    //! @brief Shared memory id of the status reports.
    int statusShmId{invalidShmId};
    //! @brief Information about the current configuration.
    char configInfo[defaultInfoSize * 2]{'\0'};
};

//! @brief TLV value serialization.
//! @tparam T - type of target payload
//! @param pkt - encoding packet that was filled type
//! @param val - value of TLV to encode
//! @param pl - target payload that has been included in the value of TLV
//! @return summary offset of length-value
template <typename T>
requires std::is_arithmetic_v<T>
static int serialize(Packet& pkt, const TLVValue& val, T TLVValue::*pl)
{
    constexpr int length = sizeof(T);
    pkt.write<int>(length);
    pkt.write<T>(val.*pl);

    return sizeof(int) + length;
}

//! @brief TLV value serialization.
//! @param pkt - encoding packet that was filled type
//! @param val - value of TLV to encode
//! @param pl - target payload that has been included in the value of TLV
//! @return summary offset of length-value
static int serialize(Packet& pkt, const TLVValue& val, char (TLVValue::*pl)[])
{
    const int length = std::strlen(val.*pl);
    pkt.write<int>(length);
    pkt.write(val.*pl, length);

    return sizeof(int) + length;
}

//! @brief TLV value deserialization.
//! @tparam T - type of target payload
//! @param pkt - decoding packet that was filled type
//! @param val - value of TLV to decode
//! @param pl - target payload that has been included in the value of TLV
//! @return summary offset of length-value
template <typename T>
requires std::is_arithmetic_v<T>
static int deserialize(Packet& pkt, TLVValue& val, T TLVValue::*pl)
{
    int length = 0;
    pkt.read<int>(&length);
    pkt.read<T>(&(val.*pl));

    return sizeof(int) + length;
}

//! @brief TLV value deserialization.
//! @param pkt - decoding packet that was filled type
//! @param val - value of TLV to decode
//! @param pl - target payload that has been included in the value of TLV
//! @return summary offset of length-value
static int deserialize(Packet& pkt, TLVValue& val, char (TLVValue::*pl)[])
{
    int length = 0;
    pkt.read<int>(&length);
    pkt.read(&(val.*pl), length);

    return sizeof(int) + length;
}

//! @brief Encode the TLV packet.
//! @param buf - TLV packet buffer
//! @param len - buffer length
//! @param val - value of TLV to encode
//! @return 0 if successful, otherwise -1
static int encodeTLV(char* buf, int& len, const TLVValue& val)
{
    if (nullptr == buf)
    {
        return -1;
    }

    int sum = 0;
    Packet enc(buf, len);
    enc.write<int>(TLVType::header);
    enc.write<int>(sum);

    enc.write<int>(TLVType::stop);
    sum += sizeof(int) + serialize(enc, val, &TLVValue::stopTag);

    enc.write<int>(TLVType::depend);
    sum += sizeof(int) + serialize(enc, val, &TLVValue::libInfo);
    enc.write<int>(TLVType::execute);
    sum += sizeof(int) + serialize(enc, val, &TLVValue::bashShmId);
    enc.write<int>(TLVType::journal);
    sum += sizeof(int) + serialize(enc, val, &TLVValue::logShmId);
    enc.write<int>(TLVType::monitor);
    sum += sizeof(int) + serialize(enc, val, &TLVValue::statusShmId);
    enc.write<int>(TLVType::profile);
    sum += sizeof(int) + serialize(enc, val, &TLVValue::configInfo);

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    *reinterpret_cast<int*>(buf + sizeof(int)) = ::htonl(sum);
    len = sizeof(int) + sizeof(int) + sum;

    return 0;
}

//! @brief Decode the TLV packet.
//! @param buf - TLV packet buffer
//! @param len - buffer length
//! @param val - value of TLV to decode
//! @return 0 if successful, otherwise -1
static int decodeTLV(char* buf, const int len, TLVValue& val)
{
    if (nullptr == buf)
    {
        return -1;
    }

    Packet dec(buf, len);
    int type = 0;
    dec.read<int>(&type);
    if (TLVType::header != type)
    {
        return -1;
    }

    int sum = 0;
    dec.read<int>(&sum);
    while (sum > 0)
    {
        dec.read<int>(&type);
        switch (type)
        {
            case TLVType::stop:
                sum -= sizeof(int) + deserialize(dec, val, &TLVValue::stopTag);
                break;
            case TLVType::depend:
                sum -= sizeof(int) + deserialize(dec, val, &TLVValue::libInfo);
                break;
            case TLVType::execute:
                sum -= sizeof(int) + deserialize(dec, val, &TLVValue::bashShmId);
                break;
            case TLVType::journal:
                sum -= sizeof(int) + deserialize(dec, val, &TLVValue::logShmId);
                break;
            case TLVType::monitor:
                sum -= sizeof(int) + deserialize(dec, val, &TLVValue::statusShmId);
                break;
            case TLVType::profile:
                sum -= sizeof(int) + deserialize(dec, val, &TLVValue::configInfo);
                break;
            default:
                sum -= sizeof(int);
                break;
        }
    }

    return 0;
}

template <typename T>
bool Packet::write(const T data)
{
    T temp{};
    if constexpr (sizeof(T) == sizeof(int))
    {
        temp = ::htonl(data);
    }
    else if constexpr (sizeof(T) == sizeof(short))
    {
        temp = ::htons(data);
    }
    else
    {
        temp = data;
    }

    return write(&temp, sizeof(T));
}

bool Packet::write(const void* const dst, const int offset)
{
    std::memcpy(writer, dst, offset);
    writer += offset;

    return (writer < tail) ? true : false;
}

template <typename T>
bool Packet::read(T* const data)
{
    const bool isEnd = read(data, sizeof(T));
    if constexpr (sizeof(T) == sizeof(int))
    {
        *data = ::ntohl(*data);
    }
    else if constexpr (sizeof(T) == sizeof(short))
    {
        *data = ::ntohs(*data);
    }

    return isEnd;
}

bool Packet::read(void* const dst, const int offset)
{
    std::memcpy(dst, reader, offset);
    reader += offset;

    return (reader < tail) ? true : false;
}
} // namespace tlv

View& View::getInstance()
{
    if (configure::detail::activateHelper()) [[likely]]
    {
        static View viewer{};
        return viewer;
    }
    throw std::logic_error{"The " + std::string{name} + " is disabled."};
}

void View::service()
{
retry:
    try
    {
        assert(safeCurrentState() == State::init);
        safeProcessEvent(CreateServer{});

        assert(safeCurrentState() == State::idle);
        awaitNotification2Ongoing();
        safeProcessEvent(GoViewing{});

        assert(safeCurrentState() == State::work);
        awaitNotification2View();
        if (toReset.load())
        {
            safeProcessEvent(Relaunch{});
            goto retry; // NOLINT(cppcoreguidelines-avoid-goto,hicpp-avoid-goto)
        }
        safeProcessEvent(DestroyServer{});

        assert(safeCurrentState() == State::idle);
        safeProcessEvent(NoViewing{});

        assert(safeCurrentState() == State::done);
    }
    catch (const std::exception& err)
    {
        LOG_ERR << "Suspend the " << name << " during " << safeCurrentState() << " state. " << err.what();

        safeProcessEvent(Standby{});
        if (awaitNotification2Retry())
        {
            safeProcessEvent(Relaunch{});
            goto retry; // NOLINT(cppcoreguidelines-avoid-goto,hicpp-avoid-goto)
        }
    }
}

void View::Access::startup() const
try
{
    utility::time::blockingTimer(
        [this]()
        {
            if (inst.isInServingState(State::hold))
            {
                throw std::runtime_error{"The " + std::string{name} + " did not initialize successfully ..."};
            }
            return inst.isInServingState(State::idle);
        });

    if (std::unique_lock<std::mutex> daemonLock(inst.daemonMtx); true)
    {
        inst.ongoing.store(true);
        daemonLock.unlock();
        inst.daemonCond.notify_one();
    }

    utility::time::blockingTimer(
        [this]()
        {
            if (inst.isInServingState(State::hold))
            {
                throw std::runtime_error{"The " + std::string{name} + " did not start successfully ..."};
            }
            return inst.isInServingState(State::work);
        });
}
catch (const std::exception& err)
{
    LOG_ERR << err.what();
}

void View::Access::shutdown() const
try
{
    if (std::unique_lock<std::mutex> daemonLock(inst.daemonMtx); true)
    {
        inst.ongoing.store(false);
        daemonLock.unlock();
        inst.daemonCond.notify_one();
    }

    utility::time::blockingTimer(
        [this]()
        {
            if (inst.isInServingState(State::hold))
            {
                throw std::runtime_error{"The " + std::string{name} + " did not stop successfully ..."};
            }
            return inst.isInServingState(State::done);
        });
}
catch (const std::exception& err)
{
    LOG_ERR << err.what();
}

void View::Access::reload() const
try
{
    if (std::unique_lock<std::mutex> daemonLock(inst.daemonMtx); true)
    {
        inst.toReset.store(true);
        daemonLock.unlock();
        inst.daemonCond.notify_one();
    }

    if (utility::time::blockingTimer([this]() { return !inst.toReset.load(); }, inst.timeoutPeriod))
    {
        throw std::runtime_error{
            "The " + std::string{name} + " did not reset properly in " + std::to_string(inst.timeoutPeriod)
            + " ms ..."};
    }
}
catch (const std::exception& err)
{
    LOG_ERR << err.what();
}

bool View::Access::onParsing(char* buffer, const int length) const
{
    decryptMessage(buffer, length);

    tlv::TLVValue value{};
    if (tlv::decodeTLV(buffer, length, value) < 0)
    {
        throw std::runtime_error{"Invalid message content."};
    }

    if (std::strlen(value.libInfo) != 0)
    {
        std::cout << value.libInfo << std::endl;
    }
    if (tlv::invalidShmId != value.bashShmId)
    {
        printSharedMemory(value.bashShmId);
    }
    if (tlv::invalidShmId != value.logShmId)
    {
        printSharedMemory(value.logShmId, !inst.isInServingState(State::work));
    }
    if (tlv::invalidShmId != value.statusShmId)
    {
        printSharedMemory(value.statusShmId);
    }
    if (std::strlen(value.configInfo) != 0)
    {
        using utility::json::JSON;
        std::cout << JSON::load(value.configInfo) << std::endl;
    }

    return value.stopTag;
}

void View::Access::enableWait() const
{
    if (inst.isInServingState(State::work))
    {
        std::unique_lock<std::mutex> outputLock(inst.outputMtx);
        inst.outputCond.wait(outputLock, [this]() { return inst.outputCompleted.load(); });
        inst.outputCompleted.store(false);
    }
    else
    {
        const std::uint32_t maxWaitTime = inst.timeoutPeriod;
        utility::time::millisecondLevelSleep(maxWaitTime);
    }
}

void View::Access::disableWait() const
{
    std::unique_lock<std::mutex> outputLock(inst.outputMtx);
    inst.outputCompleted.store(true);
    outputLock.unlock();
    inst.outputCond.notify_one();
}

std::vector<std::string> View::splitString(const std::string_view str)
{
    std::vector<std::string> split{};
    std::istringstream transfer(str.data());
    std::string token{};
    while (transfer >> token)
    {
        split.emplace_back(token);
    }

    return split;
}

int View::buildAckTLVPacket(char* buf)
{
    int len = 0;
    if (tlv::encodeTLV(buf, len, tlv::TLVValue{}) < 0)
    {
        throw std::runtime_error{"Failed to build acknowledge packet."};
    }
    encryptMessage(buf, len);

    return len;
}

int View::buildTLVPacket4Stop(char* buf)
{
    int len = 0;
    if (tlv::encodeTLV(buf, len, tlv::TLVValue{.stopTag = true}) < 0)
    {
        throw std::runtime_error{"Failed to build packet to stop"};
    }
    encryptMessage(buf, len);

    return len;
}

int View::buildTLVPacket4Depend(const std::vector<std::string>& args, char* buf)
{
    if (!args.empty())
    {
        throw std::runtime_error{"Excessive arguments."};
    }

    int len = 0;
    tlv::TLVValue val{};
    std::string extLibraries{};
#if defined(__GLIBC__) && defined(__GLIBC_MINOR__)
    extLibraries += "GNU C Library " COMMON_STRINGIFY(__GLIBC__) "." COMMON_STRINGIFY(__GLIBC_MINOR__) "\n";
#else
#error Could not find the GNU C library version.
#endif // defined(__GLIBC__) && defined(__GLIBC_MINOR__)
#if defined(_GLIBCXX_RELEASE) && defined(__GLIBCXX__)
    extLibraries +=
        "GNU C++ Standard Library " COMMON_STRINGIFY(_GLIBCXX_RELEASE) " (" COMMON_STRINGIFY(__GLIBCXX__) ")\n";
#else
#error Could not find the GNU C++ Standard library version.
#endif // defined(_GLIBCXX_RELEASE) && defined(__GLIBCXX__)
#if defined(__GNU_MP_VERSION) && defined(__GNU_MP_VERSION_MINOR) && defined(__GNU_MP_VERSION_PATCHLEVEL)
    extLibraries += "GNU MP Library " COMMON_STRINGIFY(__GNU_MP_VERSION) "." COMMON_STRINGIFY(
        __GNU_MP_VERSION_MINOR) "." COMMON_STRINGIFY(__GNU_MP_VERSION_PATCHLEVEL) "\n";
#else
#error Could not find the GNU MP library version.
#endif // defined(__GNU_MP_VERSION) && defined(__GNU_MP_VERSION_MINOR) && defined(__GNU_MP_VERSION_PATCHLEVEL)
#if defined(MPFR_VERSION_STRING)
    extLibraries += "GNU MPFR Library " MPFR_VERSION_STRING "\n";
#else
#error Could not find the GNU MPFR library version.
#endif // defined(MPFR_VERSION_STRING)
#if defined(RL_VERSION_MAJOR) && defined(RL_VERSION_MINOR)
    extLibraries +=
        "GNU Readline Library " COMMON_STRINGIFY(RL_VERSION_MAJOR) "." COMMON_STRINGIFY(RL_VERSION_MINOR) "\n";
#else
#error Could not find the GNU Readline library version.
#endif // defined(RL_VERSION_MAJOR) && defined(RL_VERSION_MINOR)
#if defined(LZ4_VERSION_STRING)
    extLibraries += "LZ4 Library " LZ4_VERSION_STRING "\n";
#else
#error Could not find the LZ4 library version.
#endif // defined(LZ4_VERSION_STRING)
#if defined(NCURSES_VERSION)
    extLibraries += "Ncurses Library " NCURSES_VERSION "\n";
#else
#error Could not find the Ncurses library version.
#endif // defined(NCURSES_VERSION)
#if defined(OPENSSL_VERSION_STR)
    extLibraries += "OpenSSL Library " OPENSSL_VERSION_STR "";
#else
#error Could not find the OpenSSL library version.
#endif // defined(OPENSSL_VERSION_STR)
    std::strncpy(val.libInfo, extLibraries.c_str(), sizeof(val.libInfo) - 1);
    val.libInfo[sizeof(val.libInfo) - 1] = '\0';
    if (tlv::encodeTLV(buf, len, val) < 0)
    {
        throw std::runtime_error{"Failed to build packet for the depend option."};
    }
    encryptMessage(buf, len);

    return len;
}

int View::buildTLVPacket4Execute(const std::vector<std::string>& args, char* buf)
{
    const auto cmd = std::accumulate(
        args.cbegin(),
        args.cend(),
        std::string{},
        [](const auto& acc, const auto& arg) { return acc.empty() ? arg : (acc + ' ' + arg); });
    if (((cmd.length() == 2)
         && (std::all_of(cmd.cbegin(), cmd.cend(), [](const auto c) { return '\'' == c; })
             || std::all_of(cmd.cbegin(), cmd.cend(), [](const auto c) { return '"' == c; })))
        || cmd.empty())
    {
        throw std::runtime_error{"Please enter the \"execute\" and append with 'CMD' (include quotes)."};
    }
    if ((cmd.length() <= 1)
        || (((cmd.find_first_not_of('\'') == 0) || (cmd.find_last_not_of('\'') == (cmd.length() - 1)))
            && ((cmd.find_first_not_of('"') == 0) || (cmd.find_last_not_of('"') == (cmd.length() - 1)))))
    {
        throw std::runtime_error{"Missing full quotes around the pending command."};
    }

    int len = 0;
    if (const int shmId = fillSharedMemory(utility::io::executeCommand("/bin/bash -c " + cmd));
        tlv::encodeTLV(buf, len, tlv::TLVValue{.bashShmId = shmId}) < 0)
    {
        throw std::runtime_error{"Failed to build packet for the execute option."};
    }
    encryptMessage(buf, len);

    return len;
}

int View::buildTLVPacket4Journal(const std::vector<std::string>& args, char* buf)
{
    if (!args.empty())
    {
        throw std::runtime_error{"Excessive arguments."};
    }

    int len = 0;
    if (const int shmId = fillSharedMemory(previewLogContents());
        tlv::encodeTLV(buf, len, tlv::TLVValue{.logShmId = shmId}) < 0)
    {
        throw std::runtime_error{"Failed to build packet for the journal option."};
    }
    encryptMessage(buf, len);

    return len;
}

int View::buildTLVPacket4Monitor(const std::vector<std::string>& args, char* buf)
{
    if (args.size() > 1)
    {
        throw std::runtime_error{"Please enter the \"monitor\" and append with or without NUM."};
    }
    else if (args.size() == 1)
    {
        if (const auto& input = args.front(); (input.length() != 1) || !std::isdigit(input.front()))
        {
            throw std::runtime_error{"Only decimal bases are supported for the specified number of stack frames."};
        }
    }

    int len = 0;
    if (const int shmId = fillSharedMemory(previewStatusReports(!args.empty() ? std::stoul(args.front()) : 1));
        tlv::encodeTLV(buf, len, tlv::TLVValue{.statusShmId = shmId}) < 0)
    {
        throw std::runtime_error{"Failed to build packet for the monitor option."};
    }
    encryptMessage(buf, len);

    return len;
}

int View::buildTLVPacket4Profile(const std::vector<std::string>& args, char* buf)
{
    if (!args.empty())
    {
        throw std::runtime_error{"Excessive arguments."};
    }

    int len = 0;
    tlv::TLVValue val{};
    std::strncpy(val.configInfo, configure::retrieveDataRepo().toUnescapedString().c_str(), sizeof(val.configInfo) - 1);
    val.configInfo[sizeof(val.configInfo) - 1] = '\0';
    if (tlv::encodeTLV(buf, len, val) < 0)
    {
        throw std::runtime_error{"Failed to build packet for the profile option."};
    }
    encryptMessage(buf, len);

    return len;
}

// NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
void View::encryptMessage(char* buffer, const int length)
{
    ::EVP_CIPHER_CTX* const ctx = ::EVP_CIPHER_CTX_new();
    do
    {
        if (constexpr std::array<unsigned char, 16> key =
                {0x37, 0x47, 0x10, 0x33, 0x6F, 0x18, 0xC8, 0x9A, 0x4B, 0xC1, 0x2B, 0x97, 0x92, 0x19, 0x25, 0x6D},
            iv = {0x9F, 0x7B, 0x0E, 0x68, 0x2D, 0x2F, 0x4E, 0x7F, 0x1A, 0xFA, 0x61, 0xD3, 0xC6, 0x18, 0xF4, 0xC1};
            !::EVP_EncryptInit_ex(ctx, ::EVP_aes_128_cfb128(), nullptr, key.data(), iv.data()))
        {
            break;
        }

        int outLen = 0;
        if (!::EVP_EncryptUpdate(
                ctx,
                reinterpret_cast<unsigned char*>(buffer),
                &outLen,
                reinterpret_cast<unsigned char*>(buffer),
                length))
        {
            break;
        }

        if (int tempLen = 0; !::EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(buffer) + outLen, &tempLen))
        {
            break;
        }
    }
    while (0);
    ::EVP_CIPHER_CTX_free(ctx);
}

void View::decryptMessage(char* buffer, const int length)
{
    ::EVP_CIPHER_CTX* const ctx = ::EVP_CIPHER_CTX_new();
    do
    {
        if (constexpr std::array<unsigned char, 16> key =
                {0x37, 0x47, 0x10, 0x33, 0x6F, 0x18, 0xC8, 0x9A, 0x4B, 0xC1, 0x2B, 0x97, 0x92, 0x19, 0x25, 0x6D},
            iv = {0x9F, 0x7B, 0x0E, 0x68, 0x2D, 0x2F, 0x4E, 0x7F, 0x1A, 0xFA, 0x61, 0xD3, 0xC6, 0x18, 0xF4, 0xC1};
            !::EVP_DecryptInit_ex(ctx, ::EVP_aes_128_cfb128(), nullptr, key.data(), iv.data()))
        {
            break;
        }

        int outLen = 0;
        if (!::EVP_DecryptUpdate(
                ctx,
                reinterpret_cast<unsigned char*>(buffer),
                &outLen,
                reinterpret_cast<unsigned char*>(buffer),
                length))
        {
            break;
        }

        if (int tempLen = 0; !::EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(buffer) + outLen, &tempLen))
        {
            break;
        }
    }
    while (0);
    ::EVP_CIPHER_CTX_free(ctx);
}
// NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)

void View::compressData(std::vector<char>& cache)
{
    const int compressedCap = ::LZ4_compressBound(cache.size());
    std::vector<char> compressed(compressedCap);

    const int compressedSize = ::LZ4_compress_default(cache.data(), compressed.data(), cache.size(), compressedCap);
    if (compressedSize < 0)
    {
        throw std::runtime_error{"Failed to compress data."};
    }
    compressed.resize(compressedSize);
    cache = std::move(compressed);
}

void View::decompressData(std::vector<char>& cache)
{
    constexpr int decompressedCap = 65536 * 10 * 10;
    std::vector<char> decompressed(decompressedCap);

    const int decompressedSize =
        ::LZ4_decompress_safe(cache.data(), decompressed.data(), cache.size(), decompressedCap);
    if (decompressedSize < 0)
    {
        throw std::runtime_error{"Failed to decompress data."};
    }
    decompressed.resize(decompressedSize);
    cache = std::move(decompressed);
}

// NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
int View::fillSharedMemory(const std::string_view contents)
{
    const int shmId = ::shmget(
        0, sizeof(SharedMemory), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (-1 == shmId)
    {
        throw std::runtime_error{"Failed to create shared memory."};
    }
    void* const shm = ::shmat(shmId, nullptr, 0);
    if (nullptr == shm)
    {
        throw std::runtime_error{"Failed to attach shared memory."};
    }

    auto* const shrMem = reinterpret_cast<SharedMemory*>(shm);
    for (shrMem->signal.store(false);;)
    {
        if (!shrMem->signal.load())
        {
            std::vector<char> processed(contents.data(), contents.data() + contents.length());
            compressData(processed);
            encryptMessage(processed.data(), processed.size());
            *reinterpret_cast<int*>(shrMem->buffer) = processed.size();
            std::memcpy(
                shrMem->buffer + sizeof(int), processed.data(), std::min(maxShmSize, processed.size()) * sizeof(char));

            shrMem->signal.store(true);
            break;
        }
        std::this_thread::yield();
    }
    ::shmdt(shm);

    return shmId;
}

void View::fetchSharedMemory(const int shmId, std::string& contents)
{
    void* const shm = ::shmat(shmId, nullptr, 0);
    if (nullptr == shm)
    {
        throw std::runtime_error{"Failed to attach shared memory."};
    }

    auto* const shrMem = reinterpret_cast<SharedMemory*>(shm);
    for (shrMem->signal.store(true);;)
    {
        if (shrMem->signal.load())
        {
            std::vector<char> processed(*reinterpret_cast<int*>(shrMem->buffer));
            std::memcpy(
                processed.data(), shrMem->buffer + sizeof(int), std::min(maxShmSize, processed.size()) * sizeof(char));
            decryptMessage(processed.data(), processed.size());
            decompressData(processed);
            contents = std::string{processed.data(), processed.data() + processed.size()};

            shrMem->signal.store(false);
            break;
        }
        std::this_thread::yield();
    }
    ::shmdt(shm);
    ::shmctl(shmId, IPC_RMID, nullptr);
}
// NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)

void View::printSharedMemory(const int shmId, const bool withoutPaging)
{
    std::string output{};
    fetchSharedMemory(shmId, output);
    if (withoutPaging)
    {
        std::istringstream transfer(output.c_str());
        std::string line{};
        while (std::getline(transfer, line))
        {
            std::cout << line << '\n';
        }
        std::cout << utility::common::colorOff << std::flush;
    }
    else
    {
        segmentedOutput(output);
    }
}

void View::segmentedOutput(const std::string_view buffer)
{
    constexpr std::uint8_t terminalRows = 24;
    constexpr std::string_view hint = "--- Type <CR> for more, c to continue, n to show next page, q to quit ---: ",
                               clearEscape = "\x1b[1A\x1b[2K\r";
    std::istringstream transfer(buffer.data());
    const std::size_t lineNum =
        std::count(std::istreambuf_iterator<char>(transfer), std::istreambuf_iterator<char>{}, '\n');
    transfer.seekg(std::ios::beg);

    bool moreRows = false, forcedCancel = false, withoutPaging = (lineNum <= terminalRows);
    std::string line{};
    std::size_t counter = 0;
    const auto handling = [&](const std::string_view input)
    {
        std::cout << clearEscape << std::flush;
        if (input.empty())
        {
            moreRows = true;
            counter = 0;
        }
        else
        {
            moreRows = false;
            switch (utility::common::bkdrHash(input.data()))
            {
                using utility::common::operator""_bkdrHash;
                case "c"_bkdrHash:
                    withoutPaging = true;
                    break;
                case "n"_bkdrHash:
                    counter = 0;
                    break;
                case "q"_bkdrHash:
                    forcedCancel = true;
                    break;
                default:
                    std::cout << hint << std::flush;
                    return false;
            }
        }
        return true;
    };
    while (std::getline(transfer, line) && !forcedCancel)
    {
        std::cout << line << '\n';
        ++counter;
        if (!withoutPaging && (moreRows || (terminalRows == counter)))
        {
            std::cout << hint << "\n\x1b[1A\x1b[" << hint.length() << 'C' << std::flush;
            utility::io::waitForUserInput(handling);
        }
    }

    std::cout << utility::common::colorOff << std::flush;
    if (lineNum > terminalRows)
    {
        std::cout << std::endl;
    }
}

std::string View::previewLogContents()
{
    utility::common::ReadWriteGuard guard(log::info::loggerFileLock(), LockMode::read);
    constexpr std::uint16_t maxRows = 24 * 100;
    auto contents = utility::io::getFileContents(log::info::loggerFilePath(), false, true, maxRows);
    std::for_each(contents.begin(), contents.end(), [](auto& line) { return log::changeToLogStyle(line); });
    std::ostringstream transfer{};
    std::copy(contents.cbegin(), contents.cend(), std::ostream_iterator<std::string>(transfer, "\n"));

    return std::move(transfer).str();
}

std::string View::previewStatusReports(const std::uint16_t frame)
{
    const int pid = ::getpid();
    constexpr std::uint16_t totalLen = 512;
    char cmd[totalLen] = {'\0'};
    std::snprintf(cmd, totalLen, "ps -T -p %d | awk 'NR>1 {split($0, a, \" \"); print a[2]}'", pid);
    const auto queryResult = utility::io::executeCommand(cmd);

    std::vector<std::string> cmdColl{};
    std::size_t pos = 0, prev = 0;
    const int currTid = ::gettid();
    const bool showStack = (::system("which eu-stack >/dev/null 2>&1") == EXIT_SUCCESS);
    while (std::string::npos != (pos = queryResult.find('\n', prev)))
    {
        const int tid = std::stoi(queryResult.substr(prev, pos - prev + 1));
        char cmd[totalLen] = {'\0'};
        if (const int usedLen = std::snprintf(
                cmd,
                totalLen,
                "if [ -f /proc/%d/task/%d/status ] ; then head -n 10 /proc/%d/task/%d/status ",
                pid,
                tid,
                pid,
                tid);
            showStack)
        {
            if (currTid != tid)
            {
                std::snprintf(
                    cmd + usedLen,
                    totalLen - usedLen,
                    "&& echo 'Stack:' "
                    "&& (timeout --preserve-status --signal=2 0.%d stdbuf -o0 eu-stack -1v -n %d -p %d 2>&1 | grep '#' "
                    "|| exit 0) ; fi",
                    frame,
                    frame,
                    tid);
            }
            else
            {
                std::strncpy(cmd + usedLen, "&& echo 'Stack:' && echo 'N/A' ; fi", totalLen - usedLen);
            }
        }
        else
        {
            std::strncpy(cmd + usedLen, "; fi", totalLen - usedLen);
        }
        cmdColl.emplace_back(cmd);
        prev += pos - prev + 1;
    }
    cmd[totalLen - 1] = '\0';

    return std::accumulate(
        cmdColl.cbegin(),
        cmdColl.cend(),
        std::string{},
        [](const auto& acc, const auto& cmd)
        { return acc.empty() ? utility::io::executeCommand(cmd) : (acc + '\n' + utility::io::executeCommand(cmd)); });
}

View::State View::safeCurrentState() const
{
    stateLock.lock();
    const auto state = State(currentState());
    stateLock.unlock();

    return state;
}

template <typename T>
void View::safeProcessEvent(const T& event)
{
    stateLock.lock();
    try
    {
        processEvent(event);
    }
    catch (...)
    {
        stateLock.unlock();
        throw;
    }
    stateLock.unlock();
}

bool View::isInServingState(const State state) const
{
    return (safeCurrentState() == state) && !toReset.load();
}

void View::createViewServer()
{
    tcpServer = std::make_shared<utility::socket::TCPServer>();
    tcpServer->onNewConnection = [this](const std::shared_ptr<utility::socket::TCPSocket> newSocket)
    {
        std::weak_ptr<utility::socket::TCPSocket> weakSock = newSocket;
        newSocket->onMessageReceived = [this, weakSock](const std::string_view message)
        {
            if (message.empty())
            {
                return;
            }

            auto newSocket = weakSock.lock();
            if (!newSocket)
            {
                return;
            }

            char buffer[1024] = {'\0'};
            try
            {
                const auto plaintext = utility::common::base64Decode(message);
                if (plaintext == exitSymbol)
                {
                    buildTLVPacket4Stop(buffer);
                    newSocket->toSend(buffer, sizeof(buffer));
                    newSocket->asyncExit();
                    return;
                }

                auto args = splitString(plaintext);
                const auto optIter = supportedOptions.find(args.at(0));
                if (supportedOptions.cend() == optIter)
                {
                    throw std::runtime_error{"Dropped unknown request message from TCP client."};
                }
                args.erase(args.cbegin());
                optIter->second.functor(args, buffer);
                newSocket->toSend(buffer, sizeof(buffer));
            }
            catch (const std::exception& err)
            {
                LOG_WRN << err.what();
                buildAckTLVPacket(buffer);
                newSocket->toSend(buffer, sizeof(buffer));
            }
        };
    };

    udpServer = std::make_shared<utility::socket::UDPServer>();
    udpServer->onMessageReceived =
        [this](const std::string_view message, const std::string_view ip, const std::uint16_t port)
    {
        if (message.empty())
        {
            return;
        }

        char buffer[1024] = {'\0'};
        try
        {
            const auto plaintext = utility::common::base64Decode(message);
            if (plaintext == exitSymbol)
            {
                buildTLVPacket4Stop(buffer);
                udpServer->toSendTo(buffer, sizeof(buffer), ip, port);
                return;
            }

            auto args = splitString(plaintext);
            const auto optIter = supportedOptions.find(args.at(0));
            if (supportedOptions.cend() == optIter)
            {
                throw std::runtime_error{
                    "Dropped unknown request message from " + std::string{ip} + ':' + std::to_string(port)
                    + " UDP client."};
            }
            args.erase(args.cbegin());
            optIter->second.functor(args, buffer);
            udpServer->toSendTo(buffer, sizeof(buffer), ip, port);
        }
        catch (const std::exception& err)
        {
            LOG_WRN << err.what();
            buildAckTLVPacket(buffer);
            udpServer->toSendTo(buffer, sizeof(buffer), ip, port);
        }
    };
}

void View::destroyViewServer()
{
    tcpServer->toClose();
    tcpServer->waitIfAlive();
    tcpServer.reset();
    udpServer->toClose();
    udpServer->waitIfAlive();
    udpServer.reset();
}

void View::startViewing()
{
    tcpServer->toBind(tcpPort);
    tcpServer->toListen();
    tcpServer->toAccept();
    udpServer->toBind(udpPort);
    udpServer->toReceiveFrom();
}

void View::stopViewing()
{
    const std::scoped_lock locks(daemonMtx, outputMtx);
    ongoing.store(false);
    toReset.store(false);
    outputCompleted.store(false);
}

void View::doToggle()
{
}

void View::doRollback()
{
    const std::scoped_lock locks(daemonMtx, outputMtx);
    ongoing.store(false);

    if (tcpServer)
    {
        try
        {
            tcpServer->toClose();
            tcpServer->waitIfAlive();
        }
        catch (...)
        {
        }
        tcpServer.reset();
    }
    if (udpServer)
    {
        try
        {
            udpServer->toClose();
            udpServer->waitIfAlive();
        }
        catch (...)
        {
        }
        udpServer.reset();
    }

    toReset.store(false);
    outputCompleted.store(false);
}

void View::awaitNotification2Ongoing()
{
    if (std::unique_lock<std::mutex> daemonLock(daemonMtx); true)
    {
        daemonCond.wait(daemonLock, [this]() { return ongoing.load(); });
    }
}

void View::awaitNotification2View()
{
    while (ongoing.load())
    {
        std::unique_lock<std::mutex> daemonLock(daemonMtx);
        daemonCond.wait(daemonLock, [this]() { return !ongoing.load() || toReset.load(); });
        if (toReset.load())
        {
            break;
        }
    }
}

bool View::awaitNotification2Retry()
{
    if (std::unique_lock<std::mutex> daemonLock(daemonMtx); true)
    {
        daemonCond.wait(daemonLock);
    }

    return toReset.load();
}

//! @brief The operator (<<) overloading of the State enum.
//! @param os - output stream object
//! @param state - the specific value of State enum
//! @return reference of the output stream object
std::ostream& operator<<(std::ostream& os, const View::State state)
{
    using enum View::State;
    switch (state)
    {
        case init:
            os << "INIT";
            break;
        case idle:
            os << "IDLE";
            break;
        case work:
            os << "WORK";
            break;
        case done:
            os << "DONE";
            break;
        case hold:
            os << "HOLD";
            break;
        default:
            os << "UNKNOWN (" << static_cast<std::underlying_type_t<View::State>>(state) << ')';
            break;
    }

    return os;
}
} // namespace application::view
