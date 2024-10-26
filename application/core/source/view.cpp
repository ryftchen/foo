//! @file view.cpp
//! @author ryftchen
//! @brief The definitions (view) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#include "view.hpp"
#include "log.hpp"

#ifndef __PRECOMPILED_HEADER
#include <openssl/evp.h>
#include <readline/readline.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <mpfr.h>
#if defined(__has_include) && __has_include(<ncurses.h>)
#include <ncurses.h>
#endif // defined(__has_include) && __has_include(<ncurses.h>)
#include <cassert>
#include <iterator>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

namespace application::view
{
namespace tlv
{
//! @brief TLV value serialization.
//! @tparam T - type of target payload
//! @param pkt - encoding packet that was filled type
//! @param val - value of TLV to encode
//! @param pl - target payload that has been included in the value of TLV
//! @return summary offset of length-value
template <typename T>
requires std::is_arithmetic<T>::value
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
requires std::is_arithmetic<T>::value
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
//! @return the value is 0 if successful, otherwise -1
static int tlvEncoding(char* buf, int& len, const TLVValue& val)
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

    *reinterpret_cast<int*>(buf + sizeof(int)) = ::htonl(sum);
    len = sizeof(int) + sizeof(int) + sum;

    return 0;
}

//! @brief Decode the TLV packet.
//! @param buf - TLV packet buffer
//! @param len - buffer length
//! @param val - value of TLV to decode
//! @return the value is 0 if successful, otherwise -1
static int tlvDecoding(char* buf, const int len, TLVValue& val)
{
    if (nullptr == buf)
    {
        return -1;
    }

    Packet dec(buf, len);
    int type = 0, sum = 0;

    dec.read<int>(&type);
    if (TLVType::header != type)
    {
        return -1;
    }
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
    if (!config::detail::activateHelper()) [[unlikely]]
    {
        throw std::logic_error("The viewer is disabled.");
    }

    static View viewer{};
    return viewer;
}

void View::stateController()
{
retry:
    try
    {
        assert(safeCurrentState() == State::init);
        safeProcessEvent(CreateServer());

        assert(safeCurrentState() == State::idle);
        awaitNotification2Ongoing();
        safeProcessEvent(GoViewing());

        assert(safeCurrentState() == State::work);
        awaitNotification2View();
        if (toReset.load())
        {
            safeProcessEvent(Relaunch());
            goto retry; // NOLINT (hicpp-avoid-goto)
        }
        safeProcessEvent(DestroyServer());

        assert(safeCurrentState() == State::idle);
        safeProcessEvent(NoViewing());

        assert(safeCurrentState() == State::done);
    }
    catch (const std::exception& err)
    {
        LOG_ERR << "Suspend the viewer during " << safeCurrentState() << " state: " << err.what();

        safeProcessEvent(Standby());
        if (awaitNotification2Retry())
        {
            goto retry; // NOLINT (hicpp-avoid-goto)
        }
    }
}

void View::waitForStart()
try
{
    utility::time::blockingTimer(
        [this]()
        {
            if (isInUninterruptedState(State::hold))
            {
                throw std::runtime_error("The viewer did not initialize successfully ...");
            }
            return isInUninterruptedState(State::idle);
        });

    if (std::unique_lock<std::mutex> daemonLock(daemonMtx); true)
    {
        ongoing.store(true);

        daemonLock.unlock();
        daemonCond.notify_one();
    }

    utility::time::blockingTimer(
        [this]()
        {
            if (isInUninterruptedState(State::hold))
            {
                throw std::runtime_error("The viewer did not start successfully ...");
            }
            return isInUninterruptedState(State::work);
        });
}
catch (const std::exception& err)
{
    LOG_ERR << err.what();
}

void View::waitForStop()
try
{
    if (std::unique_lock<std::mutex> daemonLock(daemonMtx); true)
    {
        ongoing.store(false);
        daemonLock.unlock();
        daemonCond.notify_one();
    }

    utility::time::blockingTimer(
        [this]()
        {
            if (isInUninterruptedState(State::hold))
            {
                throw std::runtime_error("The viewer did not stop successfully ...");
            }
            return isInUninterruptedState(State::done);
        });
}
catch (const std::exception& err)
{
    LOG_ERR << err.what();
}

void View::requestToReset()
try
{
    if (std::unique_lock<std::mutex> daemonLock(daemonMtx); true)
    {
        toReset.store(true);
        daemonLock.unlock();
        daemonCond.notify_one();
    }

    if (utility::time::blockingTimer(
            [this]()
            {
                return !toReset.load();
            },
            timeoutPeriod))
    {
        throw std::runtime_error("The viewer did not reset properly in " + std::to_string(timeoutPeriod) + " ms ...");
    }
}
catch (const std::exception& err)
{
    LOG_ERR << err.what();
}

const View::OptionMap& View::getOptions() const
{
    return optionDispatcher;
}

std::string View::getTCPHost() const
{
    return tcpHost;
}

std::uint16_t View::getTCPPort() const
{
    return tcpPort;
}

std::string View::getUDPHost() const
{
    return udpHost;
}

std::uint16_t View::getUDPPort() const
{
    return udpPort;
}

tlv::TLVValue View::parseTLVPacket(char* buffer, const int length) const
{
    decryptMessage(buffer, length);

    tlv::TLVValue value{};
    if (tlv::tlvDecoding(buffer, length, value) < 0)
    {
        throw std::runtime_error("Invalid message content.");
    }

    if (std::strlen(value.libInfo) != 0)
    {
        std::cout << value.libInfo << std::endl;
    }
    if (invalidShmId != value.bashShmId)
    {
        printSharedMemory(value.bashShmId, true);
    }
    if (invalidShmId != value.logShmId)
    {
        printSharedMemory(value.logShmId, !isInUninterruptedState(State::work));
    }
    if (invalidShmId != value.statusShmId)
    {
        printSharedMemory(value.statusShmId, true);
    }
    if (std::strlen(value.configInfo) != 0)
    {
        std::cout << utility::json::JSON::load(value.configInfo) << std::endl;
    }

    return value;
}

void View::awaitDueToOutput()
{
    if (isInUninterruptedState(State::work))
    {
        std::unique_lock<std::mutex> outputLock(outputMtx);
        outputCond.wait(
            outputLock,
            [this]()
            {
                return outputCompleted.load();
            });
        outputCompleted.store(false);
    }
    else
    {
        constexpr std::uint16_t maxLatency = 500;
        utility::time::millisecondLevelSleep(maxLatency);
    }
}

void View::awakenDueToOutput()
{
    std::unique_lock<std::mutex> outputLock(outputMtx);
    outputCompleted.store(true);
    outputLock.unlock();
    outputCond.notify_one();
}

std::vector<std::string> View::splitString(const std::string_view str)
{
    std::vector<std::string> split{};
    std::istringstream is(str.data());
    std::string token{};
    while (is >> token)
    {
        split.emplace_back(token);
    }

    return split;
}

int View::buildNullTLVPacket(char* buf)
{
    int len = 0;
    if (tlv::tlvEncoding(buf, len, tlv::TLVValue{}) < 0)
    {
        throw std::runtime_error("Failed to build null packet.");
    }
    encryptMessage(buf, len);
    return len;
}

int View::buildTLVPacket4Stop(char* buf)
{
    int len = 0;
    if (tlv::tlvEncoding(buf, len, tlv::TLVValue{.stopTag = true}) < 0)
    {
        throw std::runtime_error("Failed to build packet to stop");
    }
    encryptMessage(buf, len);
    return len;
}

int View::buildTLVPacket4Depend(const std::vector<std::string>& args, char* buf)
{
    if (!args.empty())
    {
        throw std::runtime_error("Excessive arguments.");
    }

    int len = 0;
    tlv::TLVValue val{};
    std::string extLibraries{};
#if defined(__GLIBC__) && defined(__GLIBC_MINOR__)
    extLibraries += "GNU C Library " COMMON_TO_STRING(__GLIBC__) "." COMMON_TO_STRING(__GLIBC_MINOR__) "\n";
#else
#error Could not find the GNU C library version.
#endif // defined(__GLIBC__) && defined(__GLIBC_MINOR__)
#if defined(_GLIBCXX_RELEASE) && defined(__GLIBCXX__)
    extLibraries +=
        "GNU C++ Standard Library " COMMON_TO_STRING(_GLIBCXX_RELEASE) " (" COMMON_TO_STRING(__GLIBCXX__) ")\n";
#else
#error Could not find the GNU C++ Standard library version.
#endif // defined(_GLIBCXX_RELEASE) && defined(__GLIBCXX__)
#if defined(__GNU_MP_VERSION) && defined(__GNU_MP_VERSION_MINOR) && defined(__GNU_MP_VERSION_PATCHLEVEL)
    extLibraries += "GNU MP Library " COMMON_TO_STRING(__GNU_MP_VERSION) "." COMMON_TO_STRING(
        __GNU_MP_VERSION_MINOR) "." COMMON_TO_STRING(__GNU_MP_VERSION_PATCHLEVEL) "\n";
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
        "GNU Readline Library " COMMON_TO_STRING(RL_VERSION_MAJOR) "." COMMON_TO_STRING(RL_VERSION_MINOR) "\n";
#else
#error Could not find the GNU Readline library version.
#endif // defined(RL_VERSION_MAJOR) && defined(RL_VERSION_MINOR)
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
    std::strncpy(val.libInfo, extLibraries.data(), sizeof(val.libInfo) - 1);
    val.libInfo[sizeof(val.libInfo) - 1] = '\0';
    if (tlv::tlvEncoding(buf, len, val) < 0)
    {
        throw std::runtime_error("Failed to build packet for the depend option.");
    }
    encryptMessage(buf, len);
    return len;
}

int View::buildTLVPacket4Execute(const std::vector<std::string>& args, char* buf)
{
    std::string entry{};
    for (const auto& arg : args)
    {
        entry += arg + ' ';
    }
    if (!entry.empty())
    {
        entry.pop_back();
    }
    if (entry.empty()
        || ((entry.length() == 2)
            && (std::all_of(
                    entry.cbegin(),
                    entry.cend(),
                    [](const auto c)
                    {
                        return '\'' == c;
                    })
                || std::all_of(
                    entry.cbegin(),
                    entry.cend(),
                    [](const auto c)
                    {
                        return '"' == c;
                    }))))
    {
        throw std::runtime_error("Please enter the \"execute\" and append with 'CMD' (include quotes).");
    }
    if ((entry.length() <= 1)
        || (((entry.find_first_not_of('\'') == 0) || (entry.find_last_not_of('\'') == (entry.length() - 1)))
            && ((entry.find_first_not_of('"') == 0) || (entry.find_last_not_of('"') == (entry.length() - 1)))))
    {
        throw std::runtime_error("Missing full quotes around the pending command.");
    }

    int len = 0;
    const int shmId = fillSharedMemory(utility::io::executeCommand("/bin/bash -c " + entry, 5000));
    if (tlv::tlvEncoding(buf, len, tlv::TLVValue{.bashShmId = shmId}) < 0)
    {
        throw std::runtime_error("Failed to build packet for the execute option.");
    }
    encryptMessage(buf, len);
    return len;
}

int View::buildTLVPacket4Journal(const std::vector<std::string>& args, char* buf)
{
    if (!args.empty())
    {
        throw std::runtime_error("Excessive arguments.");
    }

    int len = 0;
    const int shmId = fillSharedMemory(getLogContents());
    if (tlv::tlvEncoding(buf, len, tlv::TLVValue{.logShmId = shmId}) < 0)
    {
        throw std::runtime_error("Failed to build packet for the journal option.");
    }
    encryptMessage(buf, len);
    return len;
}

int View::buildTLVPacket4Monitor(const std::vector<std::string>& args, char* buf)
{
    if (args.size() > 1)
    {
        throw std::runtime_error("Please enter the \"monitor\" and append with or without NUM.");
    }
    else if (args.size() == 1)
    {
        const std::string input = args.front();
        if ((input.length() != 1) || !std::isdigit(input.front()))
        {
            throw std::runtime_error("Only decimal bases are supported for the specified number of stack frames.");
        }
    }
    const std::uint16_t frameNum = !args.empty() ? std::stoul(args.front()) : 1;

    int len = 0;
    const int shmId = fillSharedMemory(getStatusReports(frameNum));
    if (tlv::tlvEncoding(buf, len, tlv::TLVValue{.statusShmId = shmId}) < 0)
    {
        throw std::runtime_error("Failed to build packet for the monitor option.");
    }
    encryptMessage(buf, len);
    return len;
}

int View::buildTLVPacket4Profile(const std::vector<std::string>& args, char* buf)
{
    if (!args.empty())
    {
        throw std::runtime_error("Excessive arguments.");
    }

    int len = 0;
    tlv::TLVValue val{};
    const std::string currConfig = config::retrieveData().toUnescapedString();
    std::strncpy(val.configInfo, currConfig.c_str(), sizeof(val.configInfo) - 1);
    val.configInfo[sizeof(val.configInfo) - 1] = '\0';
    if (tlv::tlvEncoding(buf, len, val) < 0)
    {
        throw std::runtime_error("Failed to build packet for the profile option.");
    }
    encryptMessage(buf, len);
    return len;
}

void View::encryptMessage(char* buffer, const int length)
{
    constexpr unsigned char
        key[16] = {0x37, 0x47, 0x10, 0x33, 0x6F, 0x18, 0xC8, 0x9A, 0x4B, 0xC1, 0x2B, 0x97, 0x92, 0x19, 0x25, 0x6D},
        iv[16] = {0x9F, 0x7B, 0x0E, 0x68, 0x2D, 0x2F, 0x4E, 0x7F, 0x1A, 0xFA, 0x61, 0xD3, 0xC6, 0x18, 0xF4, 0xC1};
    ::EVP_CIPHER_CTX* const ctx = ::EVP_CIPHER_CTX_new();
    do
    {
        int outLen = 0, tempLen = 0;
        if (!::EVP_EncryptInit_ex(ctx, ::EVP_aes_128_cfb128(), nullptr, key, iv))
        {
            break;
        }
        if (!::EVP_EncryptUpdate(
                ctx,
                reinterpret_cast<unsigned char*>(buffer),
                &outLen,
                reinterpret_cast<unsigned char*>(buffer),
                length))
        {
            break;
        }
        if (!::EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(buffer) + outLen, &tempLen))
        {
            break;
        }
    }
    while (0);
    ::EVP_CIPHER_CTX_free(ctx);
}

void View::decryptMessage(char* buffer, const int length)
{
    constexpr unsigned char
        key[16] = {0x37, 0x47, 0x10, 0x33, 0x6F, 0x18, 0xC8, 0x9A, 0x4B, 0xC1, 0x2B, 0x97, 0x92, 0x19, 0x25, 0x6D},
        iv[16] = {0x9F, 0x7B, 0x0E, 0x68, 0x2D, 0x2F, 0x4E, 0x7F, 0x1A, 0xFA, 0x61, 0xD3, 0xC6, 0x18, 0xF4, 0xC1};
    ::EVP_CIPHER_CTX* const ctx = ::EVP_CIPHER_CTX_new();
    do
    {
        int outLen = 0, tempLen = 0;
        if (!::EVP_DecryptInit_ex(ctx, ::EVP_aes_128_cfb128(), nullptr, key, iv))
        {
            break;
        }
        if (!::EVP_DecryptUpdate(
                ctx,
                reinterpret_cast<unsigned char*>(buffer),
                &outLen,
                reinterpret_cast<unsigned char*>(buffer),
                length))
        {
            break;
        }
        if (!::EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(buffer) + outLen, &tempLen))
        {
            break;
        }
    }
    while (0);
    ::EVP_CIPHER_CTX_free(ctx);
}

int View::fillSharedMemory(const std::string_view contents)
{
    const int shmId = ::shmget(
        static_cast<::key_t>(0),
        sizeof(SharedMemory),
        IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (-1 == shmId)
    {
        throw std::runtime_error("Failed to create shared memory.");
    }
    void* const shm = ::shmat(shmId, nullptr, 0);
    if (nullptr == shm)
    {
        throw std::runtime_error("Failed to attach shared memory.");
    }

    auto* const shrMem = reinterpret_cast<SharedMemory*>(shm);
    shrMem->signal.store(false);
    for (;;)
    {
        if (!shrMem->signal.load())
        {
            std::memset(shrMem->buffer, 0, sizeof(shrMem->buffer));
            std::strncpy(shrMem->buffer, contents.data(), sizeof(shrMem->buffer) - 1);
            shrMem->buffer[sizeof(shrMem->buffer) - 1] = '\0';
            encryptMessage(shrMem->buffer, sizeof(shrMem->buffer));

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
        throw std::runtime_error("Failed to attach shared memory.");
    }

    auto* const shrMem = reinterpret_cast<SharedMemory*>(shm);
    shrMem->signal.store(true);
    for (;;)
    {
        if (shrMem->signal.load())
        {
            decryptMessage(shrMem->buffer, sizeof(shrMem->buffer));
            contents = shrMem->buffer;
            shrMem->signal.store(false);
            break;
        }
        std::this_thread::yield();
    }
    ::shmdt(shm);
    ::shmctl(shmId, IPC_RMID, nullptr);
}

void View::printSharedMemory(const int shmId, const bool withoutPaging)
{
    std::string output{};
    fetchSharedMemory(shmId, output);
    if (withoutPaging)
    {
        std::cout << output;
    }
    else
    {
        segmentedOutput(output);
    }
}

void View::segmentedOutput(const std::string_view buffer)
{
    constexpr std::uint8_t terminalRows = 24;
    constexpr std::string_view hint = "----- Type <CR> for more, c to continue without paging, q to quit -----: ",
                               clearEscape = "\x1b[1A\x1b[2K\r";
    std::istringstream is(buffer.data());
    const std::uint64_t lineNum =
        std::count(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>(), '\n');
    is.seekg(std::ios::beg);

    bool forcedCancel = false, withoutPaging = (lineNum <= terminalRows);
    std::string line{};
    std::uint64_t counter = 0;
    while (std::getline(is, line) && !forcedCancel)
    {
        std::cout << line << '\n';
        ++counter;
        if (!withoutPaging && (0 == (counter % terminalRows)))
        {
            std::cout << hint << "\n\x1b[1A\x1b[" << hint.length() << 'C' << std::flush;
            utility::io::waitForUserInput(
                [&](const std::string_view input)
                {
                    std::cout << clearEscape << std::flush;
                    if (input.empty())
                    {
                        --counter;
                    }
                    else if ("c" == input)
                    {
                        withoutPaging = true;
                    }
                    else if ("q" == input)
                    {
                        forcedCancel = true;
                    }
                    else
                    {
                        std::cout << hint << std::flush;
                        return false;
                    }
                    return true;
                },
                -1);
        }
    }

    if (lineNum > terminalRows)
    {
        std::cout << std::endl;
    }
}

std::string View::getLogContents()
{
    utility::common::ReadWriteGuard guard(log::info::loggerFileLock(), LockMode::read);
    auto contents = utility::io::getFileContents(log::info::loggerFilePath(), false, true);
    std::for_each(
        contents.begin(),
        contents.end(),
        [](auto& line)
        {
            return log::changeToLogStyle(line);
        });
    std::ostringstream os{};
    std::copy(contents.cbegin(), contents.cend(), std::ostream_iterator<std::string>(os, "\n"));
    return std::move(os).str();
}

std::string View::getStatusReports(const std::uint16_t frame)
{
    const int pid = ::getpid();
    constexpr std::uint16_t totalLen = 512;
    char cmd[totalLen] = {'\0'};
    std::snprintf(cmd, totalLen, "ps -T -p %d | awk 'NR>1 {split($0, a, \" \"); print a[2]}'", pid);
    const std::string queryResult = utility::io::executeCommand(cmd);

    std::vector<std::string> cmdColl{};
    std::size_t pos = 0, prev = 0;
    const int currTid = ::gettid();
    const bool showStack = (::system("which eu-stack >/dev/null 2>&1") == EXIT_SUCCESS);
    while (std::string::npos != (pos = queryResult.find('\n', prev)))
    {
        const int tid = std::stoi(queryResult.substr(prev, pos - prev));
        char cmd[totalLen] = {'\0'};
        const int usedLen = std::snprintf(
            cmd,
            totalLen,
            "if [ -f /proc/%d/task/%d/status ] ; then head -n 10 /proc/%d/task/%d/status ",
            pid,
            tid,
            pid,
            tid);
        if (showStack)
        {
            if (currTid != tid)
            {
                std::snprintf(
                    cmd + usedLen,
                    totalLen - usedLen,
                    "&& echo 'Stack:' && (timeout --preserve-status --signal=2 0.1 stdbuf -o0 eu-stack -1v -n %d -p %d "
                    "2>&1 | grep '#' || exit 0) ; fi",
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
        prev = pos + 1;
    }
    cmd[totalLen - 1] = '\0';

    std::string statRep{};
    std::for_each(
        cmdColl.cbegin(),
        cmdColl.cend(),
        [&statRep](const auto& cmd)
        {
            statRep += utility::io::executeCommand(cmd) + '\n';
        });
    if (!statRep.empty())
    {
        statRep.pop_back();
    }

    return statRep;
}

View::State View::safeCurrentState() const
{
    stateLock.lock();
    const auto state = State(currentState());
    stateLock.unlock();
    return state;
}

template <class T>
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

bool View::isInUninterruptedState(const State state) const
{
    return (safeCurrentState() == state) && !toReset.load();
}

void View::createViewServer()
{
    tcpServer = std::make_shared<utility::socket::TCPServer>();
    tcpServer->onNewConnection = [this](const std::shared_ptr<utility::socket::TCPSocket> newSocket)
    {
        std::weak_ptr<utility::socket::TCPSocket> weakSocket = newSocket;
        newSocket->onMessageReceived = [this, weakSocket](const std::string_view message)
        {
            auto newSocket = weakSocket.lock();
            if (!newSocket)
            {
                return;
            }

            if (message.empty())
            {
                return;
            }
            char buffer[maxMsgLen] = {'\0'};
            try
            {
                const auto telling = utility::common::base64Decode(message);
                if ("stop" == telling)
                {
                    buildTLVPacket4Stop(buffer);
                    newSocket->toSend(buffer, sizeof(buffer));
                    newSocket->asyncExit();
                    return;
                }

                auto args = splitString(telling);
                const auto optionIter = optionDispatcher.find(args.at(0));
                if (optionDispatcher.cend() == optionIter)
                {
                    throw std::runtime_error("Unknown TCP message.");
                }
                args.erase(args.begin());
                (optionIter->second.functor)(args, buffer);
                newSocket->toSend(buffer, sizeof(buffer));
            }
            catch (const std::exception& err)
            {
                LOG_WRN << err.what();
                buildNullTLVPacket(buffer);
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

        char buffer[maxMsgLen] = {'\0'};
        try
        {
            const auto telling = utility::common::base64Decode(message);
            if ("stop" == telling)
            {
                buildTLVPacket4Stop(buffer);
                udpServer->toSendTo(buffer, sizeof(buffer), ip, port);
                return;
            }

            auto args = splitString(telling);
            const auto optionIter = optionDispatcher.find(args.at(0));
            if (optionDispatcher.cend() == optionIter)
            {
                throw std::runtime_error("Unknown UDP message.");
            }
            args.erase(args.begin());
            (optionIter->second.functor)(args, buffer);
            udpServer->toSendTo(buffer, sizeof(buffer), ip, port);
        }
        catch (const std::exception& err)
        {
            LOG_WRN << err.what();
            buildNullTLVPacket(buffer);
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
    std::scoped_lock locks(daemonMtx, outputMtx);
    ongoing.store(false);
    toReset.store(false);
    outputCompleted.store(false);
}

void View::doToggle()
{
}

void View::doRollback()
{
    std::scoped_lock locks(daemonMtx, outputMtx);
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
        daemonCond.wait(
            daemonLock,
            [this]()
            {
                return ongoing.load();
            });
    }
}

void View::awaitNotification2View()
{
    while (ongoing.load())
    {
        std::unique_lock<std::mutex> daemonLock(daemonMtx);
        daemonCond.wait(
            daemonLock,
            [this]()
            {
                return !ongoing.load() || toReset.load();
            });

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

    if (toReset.load())
    {
        safeProcessEvent(Relaunch());
        if (safeCurrentState() == State::init)
        {
            return true;
        }
        LOG_ERR << "Failed to rollback viewer.";
    }

    return false;
}

//! @brief The operator (<<) overloading of the State enum.
//! @param os - output stream object
//! @param state - the specific value of State enum
//! @return reference of the output stream object
std::ostream& operator<<(std::ostream& os, const View::State state)
{
    switch (state)
    {
        case View::State::init:
            os << "INIT";
            break;
        case View::State::idle:
            os << "IDLE";
            break;
        case View::State::work:
            os << "WORK";
            break;
        case View::State::done:
            os << "DONE";
            break;
        case View::State::hold:
            os << "HOLD";
            break;
        default:
            os << "UNKNOWN (" << static_cast<std::underlying_type_t<View::State>>(state) << ')';
            break;
    }

    return os;
}
} // namespace application::view
