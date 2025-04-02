//! @file view.cpp
//! @author ryftchen
//! @brief The definitions (view) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "view.hpp"
#include "data.hpp"
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
//! @brief Type-length-value scheme.
namespace tlv
{
//! @brief Invalid shared memory id.
constexpr int invalidShmId = -1;
//! @brief Default detail size.
constexpr std::uint16_t detailSize = 256;

//! @brief Enumerate the types in TLV.
enum TLVType : int
{
    //! @brief Header.
    header = 0x3b9aca07,
    //! @brief Stop.
    stop = 0,
    //! @brief Depend.
    depend,
    //! @brief Execute.
    execute,
    //! @brief Journal.
    journal,
    //! @brief Monitor.
    monitor,
    //! @brief Profile.
    profile
};

//! @brief Value in TLV.
struct TLVValue
{
    //! @brief Flag for stopping the connection.
    bool stopTag{false};
    //! @brief Information about the runtime library.
    char libDetail[detailSize]{'\0'};
    //! @brief Shared memory id of the bash outputs.
    int bashShmId{invalidShmId};
    //! @brief Shared memory id of the log contents.
    int logShmId{invalidShmId};
    //! @brief Shared memory id of the status reports.
    int statusShmId{invalidShmId};
    //! @brief Information about the current configuration.
    char configDetail[detailSize * 2]{'\0'};
};

//! @brief TLV value serialization.
//! @tparam T - type of target payload
//! @param pkt - encoding packet that was filled type
//! @param val - value of TLV to encode
//! @param pl - target payload that has been included in the value of TLV
//! @return summary offset of length-value
template <typename T>
requires std::is_arithmetic_v<T>
static int serialize(data::Packet& pkt, const TLVValue& val, T TLVValue::*pl)
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
static int serialize(data::Packet& pkt, const TLVValue& val, char (TLVValue::*pl)[])
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
static int deserialize(data::Packet& pkt, TLVValue& val, T TLVValue::*pl)
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
static int deserialize(data::Packet& pkt, TLVValue& val, char (TLVValue::*pl)[])
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
    data::Packet enc(buf, len);
    enc.write<int>(TLVType::header);
    enc.write<int>(sum);

    enc.write<int>(TLVType::stop);
    sum += sizeof(int) + serialize(enc, val, &TLVValue::stopTag);
    enc.write<int>(TLVType::depend);
    sum += sizeof(int) + serialize(enc, val, &TLVValue::libDetail);
    enc.write<int>(TLVType::execute);
    sum += sizeof(int) + serialize(enc, val, &TLVValue::bashShmId);
    enc.write<int>(TLVType::journal);
    sum += sizeof(int) + serialize(enc, val, &TLVValue::logShmId);
    enc.write<int>(TLVType::monitor);
    sum += sizeof(int) + serialize(enc, val, &TLVValue::statusShmId);
    enc.write<int>(TLVType::profile);
    sum += sizeof(int) + serialize(enc, val, &TLVValue::configDetail);

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

    data::Packet dec(buf, len);
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
                sum -= sizeof(int) + deserialize(dec, val, &TLVValue::libDetail);
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
                sum -= sizeof(int) + deserialize(dec, val, &TLVValue::configDetail);
                break;
            default:
                sum -= sizeof(int);
                break;
        }
    }

    return 0;
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
    data::decryptMessage(buffer, length);

    tlv::TLVValue value{};
    if (tlv::decodeTLV(buffer, length, value) < 0)
    {
        throw std::runtime_error{"Invalid message content."};
    }

    if (std::strlen(value.libDetail) != 0)
    {
        std::cout << value.libDetail << std::endl;
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
    if (std::strlen(value.configDetail) != 0)
    {
        using utility::json::JSON;
        std::cout << JSON::load(value.configDetail) << std::endl;
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

void View::buildResponse(const std::string_view reqPlaintext, char* respBuffer)
{
    std::visit(
        OptionVisitor{
            [&respBuffer](const OptDepend& opt) { buildTLVPacket4Depend(opt.args, respBuffer); },
            [&respBuffer](const OptExecute& opt) { buildTLVPacket4Execute(opt.args, respBuffer); },
            [&respBuffer](const OptJournal& opt) { buildTLVPacket4Journal(opt.args, respBuffer); },
            [&respBuffer](const OptMonitor& opt) { buildTLVPacket4Monitor(opt.args, respBuffer); },
            [&respBuffer](const OptProfile& opt) { buildTLVPacket4Profile(opt.args, respBuffer); },
            [](const auto& opt)
            {
                // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
                if (auto* optPtr = const_cast<std::remove_const_t<std::remove_reference_t<decltype(opt)>>*>(&opt);
                    nullptr != dynamic_cast<OptBase*>(optPtr))
                {
                    throw std::runtime_error{"Unknown option type (" + std::string{typeid(opt).name()} + ")."};
                }
            }},
        extractOption(reqPlaintext));
}

View::OptionType View::extractOption(const std::string_view reqPlaintext)
{
    auto args = splitString(reqPlaintext);
    const auto option = args.at(0);
    args.erase(args.cbegin());
    switch (utility::common::bkdrHash(option.c_str()))
    {
        using utility::common::operator""_bkdrHash;
        case operator""_bkdrHash(OptDepend::name.data()):
            return OptDepend{};
        case operator""_bkdrHash(OptExecute::name.data()):
            return OptExecute{std::move(args)};
        case operator""_bkdrHash(OptJournal::name.data()):
            return OptJournal{};
        case operator""_bkdrHash(OptMonitor::name.data()):
            return OptMonitor{std::move(args)};
        case operator""_bkdrHash(OptProfile::name.data()):
            return OptProfile{};
        default:
            break;
    }

    return {};
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
    data::encryptMessage(buf, len);

    return len;
}

int View::buildTLVPacket4Stop(char* buf)
{
    int len = 0;
    if (tlv::encodeTLV(buf, len, tlv::TLVValue{.stopTag = true}) < 0)
    {
        throw std::runtime_error{"Failed to build packet to stop"};
    }
    data::encryptMessage(buf, len);

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
    std::strncpy(val.libDetail, extLibraries.c_str(), sizeof(val.libDetail) - 1);
    val.libDetail[sizeof(val.libDetail) - 1] = '\0';
    if (tlv::encodeTLV(buf, len, val) < 0)
    {
        throw std::runtime_error{"Failed to build packet for the depend option."};
    }
    data::encryptMessage(buf, len);

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
    data::encryptMessage(buf, len);

    return len;
}

int View::buildTLVPacket4Journal(const std::vector<std::string>& args, char* buf)
{
    if (!args.empty())
    {
        throw std::runtime_error{"Excessive arguments."};
    }

    int len = 0;
    if (const int shmId = fillSharedMemory(logContentsPreview());
        tlv::encodeTLV(buf, len, tlv::TLVValue{.logShmId = shmId}) < 0)
    {
        throw std::runtime_error{"Failed to build packet for the journal option."};
    }
    data::encryptMessage(buf, len);

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
            throw std::runtime_error{"Only 0 through 9 are supported for the specified number of stack frames."};
        }
    }

    int len = 0;
    if (const int shmId = fillSharedMemory(statusReportsPreview(args.empty() ? 0 : std::stoul(args.front())));
        tlv::encodeTLV(buf, len, tlv::TLVValue{.statusShmId = shmId}) < 0)
    {
        throw std::runtime_error{"Failed to build packet for the monitor option."};
    }
    data::encryptMessage(buf, len);

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
    std::strncpy(
        val.configDetail, configure::retrieveDataRepo().toUnescapedString().c_str(), sizeof(val.configDetail) - 1);
    val.configDetail[sizeof(val.configDetail) - 1] = '\0';
    if (tlv::encodeTLV(buf, len, val) < 0)
    {
        throw std::runtime_error{"Failed to build packet for the profile option."};
    }
    data::encryptMessage(buf, len);

    return len;
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
        if (shrMem->signal.load())
        {
            std::this_thread::yield();
            continue;
        }

        std::vector<char> processed(contents.data(), contents.data() + contents.length());
        data::compressData(processed);
        data::encryptMessage(processed.data(), processed.size());
        *reinterpret_cast<int*>(shrMem->buffer) = processed.size();
        std::memcpy(
            shrMem->buffer + sizeof(int), processed.data(), std::min(maxShmSize, processed.size()) * sizeof(char));

        shrMem->signal.store(true);
        break;
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
        if (!shrMem->signal.load())
        {
            std::this_thread::yield();
            continue;
        }

        std::vector<char> processed(*reinterpret_cast<int*>(shrMem->buffer));
        std::memcpy(
            processed.data(), shrMem->buffer + sizeof(int), std::min(maxShmSize, processed.size()) * sizeof(char));
        data::decryptMessage(processed.data(), processed.size());
        data::decompressData(processed);
        contents = std::string{processed.data(), processed.data() + processed.size()};

        shrMem->signal.store(false);
        break;
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
        std::cout << utility::common::escOff << std::flush;
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

    std::cout << utility::common::escOff << std::flush;
    if (lineNum > terminalRows)
    {
        std::cout << std::endl;
    }
}

std::string View::logContentsPreview()
{
    utility::common::LockGuard guard(log::info::loggerFileLock(), LockMode::read);
    constexpr std::uint16_t maxRows = 24 * 100;
    auto contents = utility::io::getFileContents(log::info::loggerFilePath(), false, true, maxRows);
    std::for_each(contents.begin(), contents.end(), [](auto& line) { return log::changeToLogStyle(line); });
    std::ostringstream transfer{};
    std::copy(contents.cbegin(), contents.cend(), std::ostream_iterator<std::string>(transfer, "\n"));

    return std::move(transfer).str();
}

std::string View::statusReportsPreview(const std::uint16_t frame)
{
    if ((frame > 0) && (::system("which eu-stack >/dev/null 2>&1") != EXIT_SUCCESS))
    {
        throw std::runtime_error{"No eu-stack program. Please install it."};
    }

    const int pid = ::getpid();
    constexpr std::uint16_t totalLen = 1024;
    char cmd[totalLen] = {'\0'};
    std::snprintf(cmd, totalLen, "ps -T -p %d | awk 'NR>1 {split($0, a, \" \"); print a[2]}'", pid);

    constexpr std::string_view focusField = "Name|State|Tgid|Pid|PPid|TracerPid|Uid|Gid|VmSize|VmRSS|CoreDumping|"
                                            "Threads|SigQ|voluntary_ctxt_switches|nonvoluntary_ctxt_switches";
    const auto queryResult = utility::io::executeCommand(cmd);
    std::vector<std::string> cmdColl{};
    std::size_t pos = 0, prev = 0;
    while (std::string::npos != (pos = queryResult.find('\n', prev)))
    {
        const int tid = std::stoi(queryResult.substr(prev, pos - prev + 1));
        char cmd[totalLen] = {'\0'};
        if (const int usedLen = std::snprintf(
                cmd,
                totalLen,
                "if [ -f /proc/%d/task/%d/status ] ; then cat /proc/%d/task/%d/status | grep -E '^(%s):'",
                pid,
                tid,
                pid,
                tid,
                focusField.data());
            0 == frame)
        {
            std::strncpy(cmd + usedLen, "; fi", totalLen - usedLen);
        }
        else
        {
            std::snprintf(
                cmd + usedLen,
                totalLen - usedLen,
                "&& echo 'Stack:' "
                "&& (timeout --preserve-status --signal=2 1 stdbuf -o0 eu-stack -1v -n %d -p %d 2>&1 | grep '#' "
                "|| exit 0) ; fi",
                frame,
                tid);
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

//! @brief Renew the TCP server.
template <>
void View::renewServer<utility::socket::TCPServer>()
{
    tcpServer = std::make_shared<utility::socket::TCPServer>();
    tcpServer->onNewConnection = [](const std::shared_ptr<utility::socket::TCPSocket> newSocket)
    {
        std::weak_ptr<utility::socket::TCPSocket> weakSock = newSocket;
        newSocket->onMessageReceived = [weakSock](const std::string_view message)
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

            char respBuffer[1024] = {'\0'};
            try
            {
                const auto reqPlaintext = utility::common::base64Decode(message);
                if (reqPlaintext == exitSymbol)
                {
                    buildTLVPacket4Stop(respBuffer);
                    newSocket->toSend(respBuffer, sizeof(respBuffer));
                    newSocket->asyncExit();
                    return;
                }

                buildResponse(reqPlaintext, respBuffer);
                newSocket->toSend(respBuffer, sizeof(respBuffer));
            }
            catch (const std::exception& err)
            {
                LOG_WRN << err.what();
                buildAckTLVPacket(respBuffer);
                newSocket->toSend(respBuffer, sizeof(respBuffer));
            }
        };
    };
}

//! @brief Renew the UDP server.
template <>
void View::renewServer<utility::socket::UDPServer>()
{
    udpServer = std::make_shared<utility::socket::UDPServer>();
    udpServer->onMessageReceived =
        [this](const std::string_view message, const std::string_view ip, const std::uint16_t port)
    {
        if (message.empty())
        {
            return;
        }

        char respBuffer[1024] = {'\0'};
        try
        {
            const auto reqPlaintext = utility::common::base64Decode(message);
            if (reqPlaintext == exitSymbol)
            {
                buildTLVPacket4Stop(respBuffer);
                udpServer->toSendTo(respBuffer, sizeof(respBuffer), ip, port);
                return;
            }

            buildResponse(reqPlaintext, respBuffer);
            udpServer->toSendTo(respBuffer, sizeof(respBuffer), ip, port);
        }
        catch (const std::exception& err)
        {
            LOG_WRN << err.what();
            buildAckTLVPacket(respBuffer);
            udpServer->toSendTo(respBuffer, sizeof(respBuffer), ip, port);
        }
    };
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
    renewServer<utility::socket::TCPServer>();
    renewServer<utility::socket::UDPServer>();
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
