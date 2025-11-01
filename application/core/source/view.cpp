//! @file view.cpp
//! @author ryftchen
//! @brief The definitions (view) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "view.hpp"
#include "data.hpp"
#include "log.hpp"

#ifndef _PRECOMPILED_HEADER
#if __has_include(<gsl/gsl_version.h>)
#include <gsl/gsl_version.h>
#endif // __has_include(<gsl/gsl_version.h>)
#include <openssl/evp.h>
#include <readline/readline.h>
#include <sys/shm.h>
#include <sys/stat.h>
#if __has_include(<gmp.h>)
#include <gmp.h>
#endif // __has_include(<gmp.h>)
#include <lz4.h>
#include <mpfr.h>
#if __has_include(<ncurses.h>)
#include <ncurses.h>
#endif // __has_include(<ncurses.h>)
#include <cassert>
#include <iterator>
#include <numeric>
#else
#include "application/pch/precompiled_header.hpp"
#endif // _PRECOMPILED_HEADER

#include "utility/include/time.hpp"

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
//! @tparam Data - type of target payload
//! @param pkt - encoding packet that was filled type
//! @param val - value of TLV to encode
//! @param pl - target payload that has been included in the value of TLV
//! @return summary offset of length-value
template <typename Data>
requires std::is_arithmetic_v<Data>
static int serialize(data::Packet& pkt, const TLVValue& val, const Data TLVValue::* const pl)
{
    if (!pl)
    {
        return 0;
    }

    constexpr int length = sizeof(Data);
    pkt.write<int>(length);
    pkt.write<Data>(val.*pl);
    return sizeof(int) + length;
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
    return sizeof(int) + length;
}

//! @brief TLV value deserialization.
//! @tparam Data - type of target payload
//! @param pkt - decoding packet that was filled type
//! @param val - value of TLV to decode
//! @param pl - target payload that has been included in the value of TLV
//! @return summary offset of length-value
template <typename Data>
requires std::is_arithmetic_v<Data>
static int deserialize(data::Packet& pkt, TLVValue& val, Data TLVValue::* const pl)
{
    if (!pl)
    {
        return 0;
    }

    int length = 0;
    pkt.read<int>(&length);
    pkt.read<Data>(&(val.*pl));
    return sizeof(int) + length;
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
    return sizeof(int) + length;
}

//! @brief Encode the TLV packet.
//! @param buf - TLV packet buffer
//! @param len - buffer length
//! @param val - value of TLV to encode
//! @return success or failure
static bool encodeTLV(char* const buf, std::size_t& len, const TLVValue& val)
{
    if (!buf)
    {
        return false;
    }

    data::Packet enc(buf, len);
    int sum = 0;
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

    int temp = ::htonl(sum);
    std::memcpy(buf + sizeof(int), &temp, sizeof(temp));
    len = sizeof(int) + sizeof(int) + sum;
    return true;
}

//! @brief Decode the TLV packet.
//! @param buf - TLV packet buffer
//! @param len - buffer length
//! @param val - value of TLV to decode
//! @return success or failure
static bool decodeTLV(char* const buf, const std::size_t len, TLVValue& val)
{
    if (!buf || (len == 0))
    {
        return false;
    }

    data::Packet dec(buf, len);
    int type = 0;
    dec.read<int>(&type);
    if (type != TLVType::header)
    {
        return false;
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
    return true;
}
} // namespace tlv

View::View() : FSM(State::initial)
{
    if (!configure::detail::activateHelper()) [[unlikely]]
    {
        throw std::logic_error{"The " + name + " is disabled."};
    }
}

std::shared_ptr<View> View::getInstance()
{
    static const std::shared_ptr<View> viewer(::new View{});
    return viewer;
}

// NOLINTBEGIN(cppcoreguidelines-avoid-goto)
void View::service()
{
retry:
    try
    {
        assert(currentState() == State::initial);
        processEvent(CreateServer{});

        assert(currentState() == State::active);
        awaitNotification2Proceed();
        processEvent(GoViewing{});

        assert(currentState() == State::established);
        notificationLoop();
        if (inResetting.load())
        {
            processEvent(Relaunch{});
            goto retry;
        }
        processEvent(DestroyServer{});

        assert(currentState() == State::active);
        processEvent(NoViewing{});

        assert(currentState() == State::inactive);
    }
    catch (const std::exception& err)
    {
        LOG_ERR << "Suspend the " << name << " during " << static_cast<State>(currentState()) << " state. "
                << err.what();

        processEvent(Standby{});
        if (awaitNotification2Retry())
        {
            processEvent(Relaunch{});
            goto retry;
        }
    }
}
// NOLINTEND(cppcoreguidelines-avoid-goto)

void View::Access::startup() const
try
{
    waitOr(
        State::active, [this]() { throw std::runtime_error{"The " + inst->name + " did not setup successfully ..."}; });
    notifyVia([this]() { inst->isOngoing.store(true); });
    waitOr(
        State::established,
        [this]() { throw std::runtime_error{"The " + inst->name + " did not start successfully ..."}; });
}
catch (const std::exception& err)
{
    LOG_ERR << err.what();
}

void View::Access::shutdown() const
try
{
    notifyVia([this]() { inst->isOngoing.store(false); });
    waitOr(
        State::inactive,
        [this]() { throw std::runtime_error{"The " + inst->name + " did not stop successfully ..."}; });
}
catch (const std::exception& err)
{
    LOG_ERR << err.what();
}

void View::Access::reload() const
try
{
    notifyVia([this]() { inst->inResetting.store(true); });
    countdownIf(
        [this]() { return inst->inResetting.load(); },
        [this]()
        {
            throw std::runtime_error{
                "The " + inst->name + " did not reset properly in " + std::to_string(inst->timeoutPeriod) + " ms ..."};
        });
}
catch (const std::exception& err)
{
    LOG_ERR << err.what();
}

bool View::Access::onParsing(char* const buffer, const std::size_t length) const
{
    data::decryptMessage(buffer, length);

    tlv::TLVValue value{};
    if (!tlv::decodeTLV(buffer, length, value))
    {
        throw std::runtime_error{"Invalid message content (" + data::toHexString(buffer, length) + ")."};
    }

    if (const std::size_t libLen = ::strnlen(value.libDetail, sizeof(value.libDetail));
        (libLen != 0) && (libLen < sizeof(value.libDetail)))
    {
        std::cout << value.libDetail << std::endl;
    }
    if (value.bashShmId != tlv::invalidShmId)
    {
        printSharedMemory(value.bashShmId);
    }
    if (value.logShmId != tlv::invalidShmId)
    {
        printSharedMemory(value.logShmId, !inst->isInServingState(State::established));
    }
    if (value.statusShmId != tlv::invalidShmId)
    {
        printSharedMemory(value.statusShmId);
    }
    if (const std::size_t configLen = ::strnlen(value.configDetail, sizeof(value.configDetail));
        (configLen != 0) && (configLen < sizeof(value.configDetail)))
    {
        using utility::json::JSON;
        std::cout << JSON::load(value.configDetail) << std::endl;
    }
    return !value.stopTag;
}

void View::Access::waitOr(const State state, const std::function<void()>& handling) const
{
    do
    {
        if (inst->isInServingState(State::idle) && handling)
        {
            handling();
        }
        std::this_thread::yield();
    }
    while (!inst->isInServingState(state));
}

void View::Access::notifyVia(const std::function<void()>& action) const
{
    std::unique_lock<std::mutex> daemonLock(inst->daemonMtx);
    if (action)
    {
        action();
    }
    daemonLock.unlock();
    inst->daemonCond.notify_one();
}

void View::Access::countdownIf(const std::function<bool()>& condition, const std::function<void()>& handling) const
{
    for (const utility::time::Stopwatch timing{}; timing.elapsedTime() <= inst->timeoutPeriod;)
    {
        if (!condition || !condition())
        {
            return;
        }
        std::this_thread::yield();
    }
    if (handling)
    {
        handling();
    }
}

void View::Sync::waitTaskDone() const
{
    std::unique_lock<std::mutex> outputLock(inst->outputMtx);
    inst->outputCompleted.store(false);

    const auto maxWaitTime = std::chrono::milliseconds{inst->timeoutPeriod};
    utility::time::Timer expiryTimer(
        inst->isInServingState(State::established) ? []() {} : []() { Sync().notifyTaskDone(); });
    expiryTimer.start(maxWaitTime);
    inst->outputCond.wait(outputLock, [this]() { return inst->outputCompleted.load(); });
    expiryTimer.stop();
}

void View::Sync::notifyTaskDone() const
{
    std::unique_lock<std::mutex> outputLock(inst->outputMtx);
    inst->outputCompleted.store(true);
    outputLock.unlock();
    inst->outputCond.notify_one();
}

//! @brief Build the TLV packet of the response message to get library information.
//! @param buf - TLV packet buffer
//! @return buffer length
template <>
std::size_t View::buildCustomTLVPacket<View::OptDepend>(const Args& /*args*/, char* const buf)
{
    tlv::TLVValue val{};
    std::string extLibraries{};
#if defined(__GLIBC__) && defined(__GLIBC_MINOR__)
    extLibraries += "GNU C Library " MACRO_STRINGIFY(__GLIBC__) "." MACRO_STRINGIFY(__GLIBC_MINOR__) "\n";
#else
#error Could not find the GNU C Library version.
#endif // defined(__GLIBC__) && defined(__GLIBC_MINOR__)
#if defined(_GLIBCXX_RELEASE) && defined(__GLIBCXX__)
    extLibraries +=
        "GNU C++ Standard Library " MACRO_STRINGIFY(_GLIBCXX_RELEASE) " (" MACRO_STRINGIFY(__GLIBCXX__) ")\n";
#else
#error Could not find the GNU C++ Standard Library version.
#endif // defined(_GLIBCXX_RELEASE) && defined(__GLIBCXX__)
#if defined(__GNU_MP_VERSION) && defined(__GNU_MP_VERSION_MINOR) && defined(__GNU_MP_VERSION_PATCHLEVEL)
    extLibraries += "GNU MP Library " MACRO_STRINGIFY(__GNU_MP_VERSION) "." MACRO_STRINGIFY(
        __GNU_MP_VERSION_MINOR) "." MACRO_STRINGIFY(__GNU_MP_VERSION_PATCHLEVEL) "\n";
#else
#error Could not find the GNU MP Library version.
#endif // defined(__GNU_MP_VERSION) && defined(__GNU_MP_VERSION_MINOR) && defined(__GNU_MP_VERSION_PATCHLEVEL)
#if defined(MPFR_VERSION_STRING)
    extLibraries += "GNU MPFR Library " MPFR_VERSION_STRING "\n";
#else
#error Could not find the GNU MPFR Library version.
#endif // defined(MPFR_VERSION_STRING)
#if defined(GSL_VERSION)
    extLibraries += "GNU Scientific Library " GSL_VERSION " (CBLAS)\n";
#else
#error Could not find the GNU Scientific Library (CBLAS) version.
#endif // defined(GSL_VERSION)
#if defined(RL_VERSION_MAJOR) && defined(RL_VERSION_MINOR)
    extLibraries +=
        "GNU Readline Library " MACRO_STRINGIFY(RL_VERSION_MAJOR) "." MACRO_STRINGIFY(RL_VERSION_MINOR) "\n";
#else
#error Could not find the GNU Readline Library version.
#endif // defined(RL_VERSION_MAJOR) && defined(RL_VERSION_MINOR)
#if defined(LZ4_VERSION_STRING)
    extLibraries += "LZ4 Library " LZ4_VERSION_STRING "\n";
#else
#error Could not find the LZ4 Library version.
#endif // defined(LZ4_VERSION_STRING)
#if defined(NCURSES_VERSION)
    extLibraries += "Ncurses Library " NCURSES_VERSION "\n";
#else
#error Could not find the Ncurses Library version.
#endif // defined(NCURSES_VERSION)
#if defined(OPENSSL_VERSION_STR)
    extLibraries += "OpenSSL Library " OPENSSL_VERSION_STR "";
#else
#error Could not find the OpenSSL Library version.
#endif // defined(OPENSSL_VERSION_STR)
    std::strncpy(val.libDetail, extLibraries.c_str(), sizeof(val.libDetail) - 1);
    val.libDetail[sizeof(val.libDetail) - 1] = '\0';
    std::size_t len = 0;
    if (!tlv::encodeTLV(buf, len, val))
    {
        throw std::runtime_error{"Failed to build packet for the " + std::string{OptDepend::name} + " option."};
    }
    data::encryptMessage(buf, len);
    return len;
}

//! @brief Build the TLV packet of the response message to get bash outputs.
//! @param args - container of arguments
//! @param buf - TLV packet buffer
//! @return buffer length
template <>
std::size_t View::buildCustomTLVPacket<View::OptExecute>(const Args& args, char* const buf)
{
    const auto cmd = std::accumulate(
        args.cbegin(),
        args.cend(),
        std::string{},
        [](const auto& acc, const auto& arg) { return acc.empty() ? arg : (acc + ' ' + arg); });
    if (((cmd.find_first_not_of('\'') == 0) || (cmd.find_last_not_of('\'') == (cmd.length() - 1)))
        && ((cmd.find_first_not_of('"') == 0) || (cmd.find_last_not_of('"') == (cmd.length() - 1))))
    {
        throw std::runtime_error{"Please enter the \"execute\" and append with 'CMD' (include quotes)."};
    }

    std::size_t len = 0;
    if (const int shmId = fillSharedMemory(utility::io::executeCommand("/bin/bash -c " + cmd));
        !tlv::encodeTLV(buf, len, tlv::TLVValue{.bashShmId = shmId}))
    {
        throw std::runtime_error{"Failed to build packet for the " + std::string{OptExecute::name} + " option."};
    }
    data::encryptMessage(buf, len);
    return len;
}

//! @brief Build the TLV packet of the response message to get log contents.
//! @param buf - TLV packet buffer
//! @return buffer length
template <>
std::size_t View::buildCustomTLVPacket<View::OptJournal>(const Args& /*args*/, char* const buf)
{
    std::size_t len = 0;
    if (const int shmId = fillSharedMemory(logContentsPreview());
        !tlv::encodeTLV(buf, len, tlv::TLVValue{.logShmId = shmId}))
    {
        throw std::runtime_error{"Failed to build packet for the " + std::string{OptJournal::name} + " option."};
    }
    data::encryptMessage(buf, len);
    return len;
}

//! @brief Build the TLV packet of the response message to get status reports.
//! @param args - container of arguments
//! @param buf - TLV packet buffer
//! @return buffer length
template <>
std::size_t View::buildCustomTLVPacket<View::OptMonitor>(const Args& args, char* const buf)
{
    if (!args.empty())
    {
        if (const auto& input = args.front(); (input.length() != 1) || !std::isdigit(input.front()))
        {
            throw std::runtime_error{"Please enter the \"monitor\" and append with or without NUM (0 to 9)."};
        }
    }

    std::size_t len = 0;
    if (const int shmId = fillSharedMemory(statusReportsPreview(args.empty() ? 0 : std::stoul(args.front())));
        !tlv::encodeTLV(buf, len, tlv::TLVValue{.statusShmId = shmId}))
    {
        throw std::runtime_error{"Failed to build packet for the " + std::string{OptMonitor::name} + " option."};
    }
    data::encryptMessage(buf, len);
    return len;
}

//! @brief Build the TLV packet of the response message to get current configuration.
//! @param buf - TLV packet buffer
//! @return buffer length
template <>
std::size_t View::buildCustomTLVPacket<View::OptProfile>(const Args& /*args*/, char* const buf)
{
    tlv::TLVValue val{};
    std::strncpy(
        val.configDetail, configure::retrieveDataRepo().asUnescapedString().c_str(), sizeof(val.configDetail) - 1);
    val.configDetail[sizeof(val.configDetail) - 1] = '\0';
    std::size_t len = 0;
    if (!tlv::encodeTLV(buf, len, val))
    {
        throw std::runtime_error{"Failed to build packet for the " + std::string{OptProfile::name} + " option."};
    }
    data::encryptMessage(buf, len);
    return len;
}

std::size_t View::buildResponse(const std::string& reqPlaintext, char* const respBuffer)
{
    return std::visit(
        OptionVisitor{
            [&respBuffer](const OptDepend& opt) { return buildCustomTLVPacket<OptDepend>(opt.args, respBuffer); },
            [&respBuffer](const OptExecute& opt) { return buildCustomTLVPacket<OptExecute>(opt.args, respBuffer); },
            [&respBuffer](const OptJournal& opt) { return buildCustomTLVPacket<OptJournal>(opt.args, respBuffer); },
            [&respBuffer](const OptMonitor& opt) { return buildCustomTLVPacket<OptMonitor>(opt.args, respBuffer); },
            [&respBuffer](const OptProfile& opt) { return buildCustomTLVPacket<OptProfile>(opt.args, respBuffer); },
            [](const auto& opt)
            {
                if (const auto* origPtr = &opt; dynamic_cast<const OptBase*>(origPtr))
                {
                    throw std::runtime_error{
                        "The option is unprocessed due to unregistered or potential registration failures (typeid: "
                        + std::string{typeid(opt).name()} + ")."};
                }
                return static_cast<std::size_t>(0);
            }},
        extractOption(reqPlaintext));
}

View::OptionType View::extractOption(const std::string& reqPlaintext)
{
    auto args = splitString(reqPlaintext);
    const auto optName = args.empty() ? std::string{} : args.at(0);
    switch (utility::common::bkdrHash(optName.c_str()))
    {
        using utility::common::operator""_bkdrHash;
        case operator""_bkdrHash(OptDepend::name):
            return OptDepend{};
        case operator""_bkdrHash(OptExecute::name):
            args.erase(args.cbegin());
            return OptExecute{std::move(args)};
        case operator""_bkdrHash(OptJournal::name):
            return OptJournal{};
        case operator""_bkdrHash(OptMonitor::name):
            args.erase(args.cbegin());
            return OptMonitor{std::move(args)};
        case operator""_bkdrHash(OptProfile::name):
            return OptProfile{};
        default:
            break;
    }
    return {};
}

std::vector<std::string> View::splitString(const std::string& str)
{
    std::vector<std::string> split{};
    std::istringstream transfer(str);
    std::string token{};
    while (transfer >> token)
    {
        split.emplace_back(token);
    }
    return split;
}

std::size_t View::buildAckTLVPacket(char* const buf)
{
    std::size_t len = 0;
    tlv::encodeTLV(buf, len, tlv::TLVValue{});
    data::encryptMessage(buf, len);
    return len;
}

std::size_t View::buildFinTLVPacket(char* const buf)
{
    std::size_t len = 0;
    tlv::encodeTLV(buf, len, tlv::TLVValue{.stopTag = true});
    data::encryptMessage(buf, len);
    return len;
}

// NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
int View::fillSharedMemory(const std::string_view contents)
{
    const int shmId = ::shmget(
        0, sizeof(ShrMemBlock), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (shmId == -1)
    {
        throw std::runtime_error{"Failed to create shared memory (" + std::to_string(shmId) + ")."};
    }
    void* const shm = ::shmat(shmId, nullptr, 0);
    if (!shm)
    {
        throw std::runtime_error{"Failed to attach shared memory (" + std::to_string(shmId) + ")."};
    }

    auto* const shrMem = reinterpret_cast<ShrMemBlock*>(shm);
    for (shrMem->signal.store(false);;)
    {
        if (shrMem->signal.load())
        {
            std::this_thread::yield();
            continue;
        }

        std::vector<char> processed(contents.data(), contents.data() + contents.length());
        data::compressData(processed);
        if (processed.size() > sizeof(shrMem->data))
        {
            shrMem->signal.store(true);
            break;
        }
        data::encryptMessage(processed.data(), processed.size());
        std::memset(shrMem->data, 0, sizeof(shrMem->data));
        std::memcpy(shrMem->data, processed.data(), processed.size() * sizeof(char));
        shrMem->size = processed.size();

        shrMem->signal.store(true);
        break;
    }
    ::shmdt(shm);
    return shmId;
}

void View::fetchSharedMemory(const int shmId, std::string& contents)
{
    void* const shm = ::shmat(shmId, nullptr, 0);
    if (!shm)
    {
        throw std::runtime_error{"Failed to attach shared memory (" + std::to_string(shmId) + ")."};
    }

    auto* const shrMem = reinterpret_cast<ShrMemBlock*>(shm);
    for (shrMem->signal.store(true);;)
    {
        if (!shrMem->signal.load())
        {
            std::this_thread::yield();
            continue;
        }

        if (shrMem->size > sizeof(shrMem->data))
        {
            shrMem->signal.store(false);
            break;
        }
        std::vector<char> processed(shrMem->size);
        shrMem->size = 0;
        std::memcpy(processed.data(), shrMem->data, processed.size() * sizeof(char));
        std::memset(shrMem->data, 0, sizeof(shrMem->data));
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
    if (!withoutPaging)
    {
        segmentedOutput(output);
        return;
    }

    std::istringstream transfer(output.c_str());
    std::string line{};
    while (std::getline(transfer, line))
    {
        std::cout << line << '\n';
    }
    std::cout << "\033[0m" << std::flush;
}

void View::segmentedOutput(const std::string& buffer)
{
    constexpr std::uint8_t terminalRows = 24;
    constexpr std::string_view prompt = "--- Type <CR> for more, c to continue, n to show next page, q to quit ---: ";
    constexpr std::string_view escapeClear = "\x1b[1A\x1b[2K\r";
    constexpr std::string_view escapeMoveUp = "\n\x1b[1A\x1b[";
    std::istringstream transfer(buffer);
    const std::size_t lineNum =
        std::count(std::istreambuf_iterator<char>(transfer), std::istreambuf_iterator<char>{}, '\n');
    transfer.seekg(std::ios::beg);

    bool moreRows = false;
    bool forcedCancel = false;
    bool withoutPaging = (lineNum <= terminalRows);
    std::string line{};
    std::size_t counter = 0;
    const auto handling = utility::common::wrapClosure(
        [&](const std::string& input)
        {
            std::cout << escapeClear << std::flush;
            if (input.empty())
            {
                moreRows = true;
                counter = 0;
                return true;
            }

            moreRows = false;
            switch (utility::common::bkdrHash(input.c_str()))
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
                    std::cout << prompt << std::flush;
                    return false;
            }
            return true;
        });
    while (std::getline(transfer, line) && !forcedCancel)
    {
        std::cout << line << '\n';
        ++counter;
        if (!withoutPaging && (moreRows || (counter == terminalRows)))
        {
            std::cout << prompt << escapeMoveUp << prompt.length() << 'C' << std::flush;
            utility::io::waitForUserInput(handling);
        }
    }

    std::cout << "\033[0m" << std::flush;
    if (lineNum > terminalRows)
    {
        std::cout << std::endl;
    }
}

std::string View::logContentsPreview()
{
    std::ostringstream transfer{};
    log::Log::Access().onPreviewing(
        [&transfer](const std::string& filePath)
        {
            constexpr std::uint16_t lineLimit = 24 * 100;
            auto logRows = utility::io::readFileLines(filePath, false, true, lineLimit);
            std::for_each(logRows.begin(), logRows.end(), [](auto& line) { log::changeToLogStyle(line); });
            std::copy(logRows.cbegin(), logRows.cend(), std::ostream_iterator<std::string>(transfer, "\n"));
        });
    return std::move(transfer).str();
}

std::string View::statusReportsPreview(const std::uint16_t frame)
{
    if ((frame > 0)
        && (::system( // NOLINT(cert-env33-c, concurrency-mt-unsafe)
                "which eu-stack >/dev/null 2>&1 "
                "&& grep -qx '0' /proc/sys/kernel/yama/ptrace_scope >/dev/null 2>&1")
            != EXIT_SUCCESS))
    {
        throw std::runtime_error{"Please confirm that the eu-stack program has been installed and "
                                 "that the classic ptrace permissions have been set."};
    }

    const ::pid_t pid = ::getpid();
    constexpr std::uint16_t totalLen = 1024;
    std::array<char, totalLen> queryStmt{};
    std::snprintf(
        queryStmt.data(), queryStmt.size(), "ps -T -p %d | awk 'NR>1 {split($0, a, \" \"); print a[2]}'", pid);

    constexpr const char* const focusField = "Name|State|Tgid|Pid|PPid|TracerPid|Uid|Gid|VmSize|VmRSS|CoreDumping|"
                                             "Threads|SigQ|voluntary_ctxt_switches|nonvoluntary_ctxt_switches";
    const auto queryResult = utility::io::executeCommand(queryStmt.data());
    std::vector<std::string> statements{};
    std::size_t pos = 0;
    std::size_t prev = 0;
    while ((pos = queryResult.find('\n', prev)) != std::string::npos)
    {
        const int tid = std::stoi(queryResult.substr(prev, pos - prev + 1));
        std::array<char, totalLen> execStmt{};
        if (const int usedLen = std::snprintf(
                execStmt.data(),
                execStmt.size(),
                "/bin/bash -c "
                "\"if [[ -f /proc/%d/task/%d/status ]]; then cat /proc/%d/task/%d/status | grep -E '^(%s):'",
                pid,
                tid,
                pid,
                tid,
                focusField);
            frame == 0)
        {
            std::strncpy(execStmt.data() + usedLen, "; fi\"", execStmt.size() - usedLen);
            execStmt[totalLen - 1] = '\0';
        }
        else
        {
            std::snprintf(
                execStmt.data() + usedLen,
                execStmt.size() - usedLen,
                " && echo 'Stack:' "
                "&& (timeout --preserve-status --signal=2 1 stdbuf -o0 eu-stack -1v -n %d -p %d 2>&1 | grep '#' "
                "|| exit 0); fi\"",
                frame,
                tid);
        }
        statements.emplace_back(execStmt.data());
        prev += pos - prev + 1;
    }
    return std::accumulate(
        statements.cbegin(),
        statements.cend(),
        std::string{},
        [](const auto& acc, const auto& stmt)
        { return acc.empty() ? utility::io::executeCommand(stmt) : (acc + '\n' + utility::io::executeCommand(stmt)); });
}

//! @brief Renew the TCP server.
template <>
void View::renewServer<utility::socket::TCPServer>()
{
    tcpServer = std::make_shared<utility::socket::TCPServer>();
    tcpServer->subscribeConnection(
        [](const std::shared_ptr<utility::socket::TCPSocket> client) // NOLINT(performance-unnecessary-value-param)
        {
            const std::weak_ptr<utility::socket::TCPSocket> weakSock = client;
            client->subscribeMessage(
                [weakSock](const std::string_view message)
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

                    std::array<char, 1024> respBuffer{};
                    try
                    {
                        const auto reqPlaintext = utility::common::base64Decode(message);
                        newSocket->toSend(
                            respBuffer.data(),
                            (reqPlaintext != exitSymbol) ? buildResponse(reqPlaintext, respBuffer.data())
                                                         : buildFinTLVPacket(respBuffer.data()));
                    }
                    catch (const std::exception& err)
                    {
                        LOG_WRN << err.what();
                        newSocket->toSend(respBuffer.data(), buildAckTLVPacket(respBuffer.data()));
                    }
                });
        });
}

//! @brief Renew the UDP server.
template <>
void View::renewServer<utility::socket::UDPServer>()
{
    udpServer = std::make_shared<utility::socket::UDPServer>();
    udpServer->subscribeMessage(
        [this](const std::string_view message, const std::string& ip, const std::uint16_t port)
        {
            if (message.empty())
            {
                return;
            }

            std::array<char, 1024> respBuffer{};
            try
            {
                const auto reqPlaintext = utility::common::base64Decode(message);
                udpServer->toSendTo(
                    respBuffer.data(),
                    (reqPlaintext != exitSymbol) ? buildResponse(reqPlaintext, respBuffer.data())
                                                 : buildFinTLVPacket(respBuffer.data()),
                    ip,
                    port);
            }
            catch (const std::exception& err)
            {
                LOG_WRN << err.what();
                udpServer->toSendTo(respBuffer.data(), buildAckTLVPacket(respBuffer.data()), ip, port);
            }
        });
}

bool View::isInServingState(const State state) const
{
    return (currentState() == state) && !inResetting.load();
}

void View::createViewServer()
{
    renewServer<utility::socket::TCPServer>();
    renewServer<utility::socket::UDPServer>();
}

void View::destroyViewServer()
{
    tcpServer->toClose();
    tcpServer->toJoin();
    tcpServer.reset();
    udpServer->toClose();
    udpServer->toJoin();
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
    isOngoing.store(false);
    inResetting.store(false);
    outputCompleted.store(true);
}

void View::doToggle()
{
}

void View::doRollback()
{
    const std::scoped_lock locks(daemonMtx, outputMtx);
    isOngoing.store(false);

    if (tcpServer)
    {
        try
        {
            tcpServer->toClose();
            tcpServer->toJoin();
        }
        catch (...) // NOLINT(bugprone-empty-catch)
        {
        }
        tcpServer.reset();
    }
    if (udpServer)
    {
        try
        {
            udpServer->toClose();
            udpServer->toJoin();
        }
        catch (...) // NOLINT(bugprone-empty-catch)
        {
        }
        udpServer.reset();
    }

    inResetting.store(false);
    outputCompleted.store(true);
}

void View::notificationLoop()
{
    while (isOngoing.load())
    {
        std::unique_lock<std::mutex> daemonLock(daemonMtx);
        daemonCond.wait(daemonLock, [this]() { return !isOngoing.load() || inResetting.load(); });
        if (inResetting.load())
        {
            break;
        }
    }
}

void View::awaitNotification2Proceed()
{
    std::unique_lock<std::mutex> daemonLock(daemonMtx);
    daemonCond.wait(daemonLock, [this]() { return isOngoing.load(); });
}

bool View::awaitNotification2Retry()
{
    std::unique_lock<std::mutex> daemonLock(daemonMtx);
    daemonCond.wait(daemonLock);
    return inResetting.load();
}

//! @brief The operator (<<) overloading of the State enum.
//! @param os - output stream object
//! @param state - current state
//! @return reference of the output stream object
std::ostream& operator<<(std::ostream& os, const View::State state)
{
    using enum View::State;
    switch (state)
    {
        case initial:
            os << "INITIAL";
            break;
        case active:
            os << "ACTIVE";
            break;
        case established:
            os << "ESTABLISHED";
            break;
        case inactive:
            os << "INACTIVE";
            break;
        case idle:
            os << "IDLE";
            break;
        default:
            os << "UNKNOWN (" << static_cast<std::underlying_type_t<View::State>>(state) << ')';
            break;
    }
    return os;
}
} // namespace application::view
