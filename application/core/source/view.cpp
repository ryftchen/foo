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
//! @tparam T - type of target payload
//! @param pkt - encoding packet that was filled type
//! @param val - value of TLV to encode
//! @param pl - target payload that has been included in the value of TLV
//! @return summary offset of length-value
template <typename T>
requires std::is_arithmetic_v<T>
static int serialize(data::Packet& pkt, const TLVValue& val, T TLVValue::* pl)
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
static int deserialize(data::Packet& pkt, TLVValue& val, T TLVValue::* pl)
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
//! @return success or failure
static bool encodeTLV(char* buf, int& len, const TLVValue& val)
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

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    *reinterpret_cast<int*>(buf + sizeof(int)) = ::htonl(sum);
    len = sizeof(int) + sizeof(int) + sum;

    return true;
}

//! @brief Decode the TLV packet.
//! @param buf - TLV packet buffer
//! @param len - buffer length
//! @param val - value of TLV to decode
//! @return success or failure
static bool decodeTLV(char* buf, const int len, TLVValue& val)
{
    if (!buf)
    {
        return false;
    }

    data::Packet dec(buf, len);
    int type = 0;
    dec.read<int>(&type);
    if (type != TLVType::header)
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

    return true;
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

// NOLINTBEGIN(cppcoreguidelines-avoid-goto)
void View::service()
{
retry:
    try
    {
        assert(currentState() == State::init);
        processEvent(CreateServer{});

        assert(currentState() == State::idle);
        awaitNotification2Ongoing();
        processEvent(GoViewing{});

        assert(currentState() == State::work);
        awaitNotification2View();
        if (toReset.load())
        {
            processEvent(Relaunch{});
            goto retry;
        }
        processEvent(DestroyServer{});

        assert(currentState() == State::idle);
        processEvent(NoViewing{});

        assert(currentState() == State::done);
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
        State::idle,
        [this]() { throw std::runtime_error{"The " + std::string{inst.name} + " did not setup successfully ..."}; });
    toNotify([this]() { inst.ongoing.store(true); });
    waitOr(
        State::work,
        [this]() { throw std::runtime_error{"The " + std::string{inst.name} + " did not start successfully ..."}; });
}
catch (const std::exception& err)
{
    LOG_ERR << err.what();
}

void View::Access::shutdown() const
try
{
    toNotify([this]() { inst.ongoing.store(false); });
    waitOr(
        State::done,
        [this]() { throw std::runtime_error{"The " + std::string{inst.name} + " did not stop successfully ..."}; });
}
catch (const std::exception& err)
{
    LOG_ERR << err.what();
}

void View::Access::reload() const
try
{
    toNotify([this]() { inst.toReset.store(true); });
    startResetTimer();
}
catch (const std::exception& err)
{
    LOG_ERR << err.what();
}

bool View::Access::onParsing(char* buffer, const int length) const
{
    data::decryptMessage(buffer, length);

    tlv::TLVValue value{};
    if (!tlv::decodeTLV(buffer, length, value))
    {
        throw std::runtime_error{"Invalid message content (" + data::toHexString(buffer, length) + ")."};
    }

    if (std::strlen(value.libDetail) != 0)
    {
        std::cout << value.libDetail << std::endl;
    }
    if (value.bashShmId != tlv::invalidShmId)
    {
        printSharedMemory(value.bashShmId);
    }
    if (value.logShmId != tlv::invalidShmId)
    {
        printSharedMemory(value.logShmId, !inst.isInServingState(State::work));
    }
    if (value.statusShmId != tlv::invalidShmId)
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

void View::Access::waitOr(const State state, const std::function<void()>& handling) const
{
    do
    {
        if (inst.isInServingState(State::hold))
        {
            handling();
        }
        std::this_thread::yield();
    }
    while (!inst.isInServingState(state));
}

void View::Access::toNotify(const std::function<void()>& action) const
{
    std::unique_lock<std::mutex> daemonLock(inst.daemonMtx);
    action();
    daemonLock.unlock();
    inst.daemonCond.notify_one();
}

void View::Access::startResetTimer() const
{
    for (const utility::time::Stopwatch timing{}; timing.elapsedTime() <= inst.timeoutPeriod;)
    {
        if (!inst.toReset.load())
        {
            return;
        }
        std::this_thread::yield();
    }
    throw std::runtime_error{
        "The " + std::string{inst.name} + " did not reset properly in " + std::to_string(inst.timeoutPeriod)
        + " ms ..."};
}

void View::Sync::waitTaskDone() const
{
    std::unique_lock<std::mutex> outputLock(inst.outputMtx);
    const auto maxWaitTime = std::chrono::milliseconds{inst.timeoutPeriod};
    utility::time::Timer expiryTimer(
        inst.isInServingState(State::work) ? []() {}
                                           : (inst.outputCompleted.store(false), []() { Sync().notifyTaskDone(); }));
    expiryTimer.start(maxWaitTime);

    inst.outputCond.wait(outputLock, [this]() { return inst.outputCompleted.load(); });
    inst.outputCompleted.store(false);

    expiryTimer.stop();
}

void View::Sync::notifyTaskDone() const
{
    std::unique_lock<std::mutex> outputLock(inst.outputMtx);
    inst.outputCompleted.store(true);
    outputLock.unlock();
    inst.outputCond.notify_one();
}

int View::buildResponse(const std::string& reqPlaintext, char* respBuffer)
{
    return std::visit(
        OptionVisitor{
            [&respBuffer](const OptDepend& opt) { return buildTLVPacket4Depend(opt.args, respBuffer); },
            [&respBuffer](const OptExecute& opt) { return buildTLVPacket4Execute(opt.args, respBuffer); },
            [&respBuffer](const OptJournal& opt) { return buildTLVPacket4Journal(opt.args, respBuffer); },
            [&respBuffer](const OptMonitor& opt) { return buildTLVPacket4Monitor(opt.args, respBuffer); },
            [&respBuffer](const OptProfile& opt) { return buildTLVPacket4Profile(opt.args, respBuffer); },
            [](const auto& opt)
            {
                if (const auto* origPtr = &opt; dynamic_cast<const OptBase*>(origPtr))
                {
                    throw std::runtime_error{
                        "The option is unprocessed due to unregistered or potential registration failures (typeid: "
                        + std::string{typeid(opt).name()} + ")."};
                }
                return 0;
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

int View::buildAckTLVPacket(char* buf)
{
    int len = 0;
    if (!tlv::encodeTLV(buf, len, tlv::TLVValue{}))
    {
        throw std::runtime_error{"Failed to build acknowledgement packet."};
    }
    data::encryptMessage(buf, len);

    return len;
}

int View::buildFinTLVPacket(char* buf)
{
    int len = 0;
    if (!tlv::encodeTLV(buf, len, tlv::TLVValue{.stopTag = true}))
    {
        throw std::runtime_error{"Failed to build finish packet."};
    }
    data::encryptMessage(buf, len);

    return len;
}

int View::buildTLVPacket4Depend(const Args& /*args*/, char* buf)
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
    int len = 0;
    if (!tlv::encodeTLV(buf, len, val))
    {
        throw std::runtime_error{"Failed to build packet for the depend option."};
    }
    data::encryptMessage(buf, len);

    return len;
}

int View::buildTLVPacket4Execute(const Args& args, char* buf)
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

    int len = 0;
    if (const int shmId = fillSharedMemory(utility::io::executeCommand("/bin/bash -c " + cmd));
        !tlv::encodeTLV(buf, len, tlv::TLVValue{.bashShmId = shmId}))
    {
        throw std::runtime_error{"Failed to build packet for the execute option."};
    }
    data::encryptMessage(buf, len);

    return len;
}

int View::buildTLVPacket4Journal(const Args& /*args*/, char* buf)
{
    int len = 0;
    if (const int shmId = fillSharedMemory(logContentsPreview());
        !tlv::encodeTLV(buf, len, tlv::TLVValue{.logShmId = shmId}))
    {
        throw std::runtime_error{"Failed to build packet for the journal option."};
    }
    data::encryptMessage(buf, len);

    return len;
}

int View::buildTLVPacket4Monitor(const Args& args, char* buf)
{
    if (!args.empty())
    {
        if (const auto& input = args.front(); (input.length() != 1) || !std::isdigit(input.front()))
        {
            throw std::runtime_error{"Please enter the \"monitor\" and append with or without NUM (0 to 9)."};
        }
    }

    int len = 0;
    if (const int shmId = fillSharedMemory(statusReportsPreview(args.empty() ? 0 : std::stoul(args.front())));
        !tlv::encodeTLV(buf, len, tlv::TLVValue{.statusShmId = shmId}))
    {
        throw std::runtime_error{"Failed to build packet for the monitor option."};
    }
    data::encryptMessage(buf, len);

    return len;
}

int View::buildTLVPacket4Profile(const Args& /*args*/, char* buf)
{
    tlv::TLVValue val{};
    std::strncpy(
        val.configDetail, configure::retrieveDataRepo().toUnescapedString().c_str(), sizeof(val.configDetail) - 1);
    val.configDetail[sizeof(val.configDetail) - 1] = '\0';
    int len = 0;
    if (!tlv::encodeTLV(buf, len, val))
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
        data::encryptMessage(processed.data(), processed.size());
        std::memset(shrMem->buffer, 0, sizeof(shrMem->buffer));
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

        std::vector<char> processed(*reinterpret_cast<int*>(shrMem->buffer));
        std::memcpy(
            processed.data(), shrMem->buffer + sizeof(int), std::min(maxShmSize, processed.size()) * sizeof(char));
        std::memset(shrMem->buffer, 0, sizeof(shrMem->buffer));
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
    constexpr std::string_view prompt = "--- Type <CR> for more, c to continue, n to show next page, q to quit ---: ",
                               escapeClear = "\x1b[1A\x1b[2K\r", escapeMoveUp = "\n\x1b[1A\x1b[";
    std::istringstream transfer(buffer);
    const std::size_t lineNum =
        std::count(std::istreambuf_iterator<char>(transfer), std::istreambuf_iterator<char>{}, '\n');
    transfer.seekg(std::ios::beg);

    bool moreRows = false, forcedCancel = false, withoutPaging = (lineNum <= terminalRows);
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
            constexpr std::uint16_t maxRows = 24 * 100;
            auto contents = utility::io::getFileContents(filePath, false, true, maxRows);
            std::for_each(contents.begin(), contents.end(), [](auto& line) { log::changeToLogStyle(line); });
            std::copy(contents.cbegin(), contents.cend(), std::ostream_iterator<std::string>(transfer, "\n"));
        });

    return std::move(transfer).str();
}

std::string View::statusReportsPreview(const std::uint16_t frame)
{
    // NOLINTNEXTLINE(cert-env33-c, concurrency-mt-unsafe)
    if ((frame > 0) && (::system("which eu-stack >/dev/null 2>&1") != EXIT_SUCCESS))
    {
        throw std::runtime_error{"No eu-stack program. Please install it."};
    }

    const int pid = ::getpid();
    constexpr std::uint16_t totalLen = 1024;
    char cmd[totalLen] = {'\0'};
    std::snprintf(cmd, totalLen, "ps -T -p %d | awk 'NR>1 {split($0, a, \" \"); print a[2]}'", pid);

    constexpr const char* const focusField = "Name|State|Tgid|Pid|PPid|TracerPid|Uid|Gid|VmSize|VmRSS|CoreDumping|"
                                             "Threads|SigQ|voluntary_ctxt_switches|nonvoluntary_ctxt_switches";
    const auto queryResult = utility::io::executeCommand(cmd);
    std::vector<std::string> cmdColl{};
    std::size_t pos = 0, prev = 0;
    while ((pos = queryResult.find('\n', prev)) != std::string::npos)
    {
        const int tid = std::stoi(queryResult.substr(prev, pos - prev + 1));
        char cmd[totalLen] = {'\0'};
        if (const int usedLen = std::snprintf(
                cmd,
                totalLen,
                "/bin/bash -c "
                "\"if [[ -f /proc/%d/task/%d/status ]]; then cat /proc/%d/task/%d/status | grep -E '^(%s):'",
                pid,
                tid,
                pid,
                tid,
                focusField);
            frame == 0)
        {
            std::strncpy(cmd + usedLen, "; fi\"", totalLen - usedLen);
        }
        else
        {
            std::snprintf(
                cmd + usedLen,
                totalLen - usedLen,
                " && echo 'Stack:' "
                "&& (timeout --preserve-status --signal=2 1 stdbuf -o0 eu-stack -1v -n %d -p %d 2>&1 | grep '#' "
                "|| exit 0); fi\"",
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
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    tcpServer->onNewConnection = [](const std::shared_ptr<utility::socket::TCPSocket> newSocket)
    {
        const std::weak_ptr<utility::socket::TCPSocket> weakSock = newSocket;
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
                (reqPlaintext != exitSymbol) ? newSocket->toSend(respBuffer, buildResponse(reqPlaintext, respBuffer))
                                             : newSocket->toSend(respBuffer, buildFinTLVPacket(respBuffer));
            }
            catch (const std::exception& err)
            {
                LOG_WRN << err.what();
                newSocket->toSend(respBuffer, buildAckTLVPacket(respBuffer));
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
        [this](const std::string_view message, const std::string& ip, const std::uint16_t port)
    {
        if (message.empty())
        {
            return;
        }

        char respBuffer[1024] = {'\0'};
        try
        {
            const auto reqPlaintext = utility::common::base64Decode(message);
            (reqPlaintext != exitSymbol)
                ? udpServer->toSendTo(respBuffer, buildResponse(reqPlaintext, respBuffer), ip, port)
                : udpServer->toSendTo(respBuffer, buildFinTLVPacket(respBuffer), ip, port);
        }
        catch (const std::exception& err)
        {
            LOG_WRN << err.what();
            udpServer->toSendTo(respBuffer, buildAckTLVPacket(respBuffer), ip, port);
        }
    };
}

bool View::isInServingState(const State state) const
{
    return (currentState() == state) && !toReset.load();
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

    toReset.store(false);
    outputCompleted.store(false);
}

void View::awaitNotification2Ongoing()
{
    std::unique_lock<std::mutex> daemonLock(daemonMtx);
    daemonCond.wait(daemonLock, [this]() { return ongoing.load(); });
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
    std::unique_lock<std::mutex> daemonLock(daemonMtx);
    daemonCond.wait(daemonLock);

    return toReset.load();
}

//! @brief The operator (<<) overloading of the State enum.
//! @param os - output stream object
//! @param state - specific value of State enum
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
