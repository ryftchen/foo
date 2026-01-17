//! @file view.cpp
//! @author ryftchen
//! @brief The definitions (view) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2026 ryftchen. All rights reserved.

#include "view.hpp"
#include "data.hpp"
#include "log.hpp"

#ifndef _PRECOMPILED_HEADER
#if __has_include(<gsl/gsl_version.h>)
#include <gsl/gsl_version.h>
#endif
#include <openssl/evp.h>
#include <readline/readline.h>
#include <sys/shm.h>
#include <sys/stat.h>
#if __has_include(<gmp.h>)
#include <gmp.h>
#endif
#include <lz4.h>
#include <mpfr.h>
#if __has_include(<ncurses.h>)
#include <ncurses.h>
#endif
#include <cassert>
#include <iterator>
#include <numeric>
#else
#include "application/pch/precompiled_header.hpp"
#endif

#include "utility/include/benchmark.hpp"
#include "utility/include/time.hpp"

namespace application::view // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Type-length-value scheme.
namespace tlv
{
//! @brief Invalid shared memory id.
constexpr int invalidShmId = -1;
//! @brief Default information size.
constexpr std::uint16_t defInfoSize = 256;

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
} // namespace application::view

template <>
struct std::is_error_code_enum<application::view::tlv::ErrCode> : public std::true_type
{
};

namespace application::view
{
namespace tlv
{
//! @brief Encode the TLV packet.
//! @param buf - TLV packet buffer
//! @param len - buffer length
//! @param val - value of TLV to encode
//! @return error code
static std::error_code encodeTLV(char* const buf, std::size_t& len, const TLVValue& val)
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
static std::error_code decodeTLV(char* const buf, const std::size_t len, TLVValue& val)
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
        processEvent(Relaunch{});

        assert(currentState() == State::initial);
        processEvent(CreateServer{});

        assert(currentState() == State::active);
        awaitNotification2Proceed();
        processEvent(GoViewing{});

        assert(currentState() == State::established);
        notificationLoop();
        if (inResetting.load())
        {
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
            goto retry;
        }
    }
}
// NOLINTEND(cppcoreguidelines-avoid-goto)

void View::Access::startup() const
try
{
    waitOr(State::active, []() { throw std::runtime_error{"The " + View::name + " did not setup successfully ..."}; });
    notifyVia([this]() { inst->isOngoing.store(true); });
    waitOr(
        State::established,
        []() { throw std::runtime_error{"The " + View::name + " did not start successfully ..."}; });
}
catch (const std::exception& err)
{
    LOG_ERR << err.what();
}

void View::Access::shutdown() const
try
{
    notifyVia([this]() { inst->isOngoing.store(false); });
    waitOr(State::inactive, []() { throw std::runtime_error{"The " + View::name + " did not stop successfully ..."}; });
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
                "The " + View::name + " did not reset properly in " + std::to_string(inst->timeoutPeriod) + " ms ..."};
        });
}
catch (const std::exception& err)
{
    LOG_ERR << err.what();
}

bool View::Access::onParsing(char* const bytes, const std::size_t size) const
{
    data::decryptMessage(bytes, size);
    tlv::TLVValue value{};
    if (const auto ec = tlv::decodeTLV(bytes, size, value); ec)
    {
        throw std::runtime_error{
            "Invalid message content \"" + data::toHexString(bytes, size) + "\" (" + ec.message() + ")."};
    }

    if (const std::size_t actLibLen = ::strnlen(value.libInfo, sizeof(value.libInfo));
        (actLibLen != 0) && (actLibLen < sizeof(value.libInfo)))
    {
        std::cout << value.libInfo << std::endl;
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
    if (const std::size_t actConfigLen = ::strnlen(value.configInfo, sizeof(value.configInfo));
        (actConfigLen != 0) && (actConfigLen < sizeof(value.configInfo)))
    {
        using utility::json::JSON;
        std::cout << JSON::load(value.configInfo) << std::endl;
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
//! @param buffer - TLV packet buffer
//! @return buffer length
template <>
std::size_t View::buildCustomTLVPacket<View::OptDepend>(const Args& /*args*/, Buffer& buffer)
{
    tlv::TLVValue val{};
    std::string extLibraries{};
#if defined(__GLIBC__) && defined(__GLIBC_MINOR__)
    extLibraries += "GNU C Library " MACRO_STRINGIFY(__GLIBC__) "." MACRO_STRINGIFY(__GLIBC_MINOR__) "\n";
#else
#error Could not find the GNU C Library version.
#endif
#if defined(_GLIBCXX_RELEASE) && defined(__GLIBCXX__)
    extLibraries +=
        "GNU C++ Standard Library " MACRO_STRINGIFY(_GLIBCXX_RELEASE) " (" MACRO_STRINGIFY(__GLIBCXX__) ")\n";
#else
#error Could not find the GNU C++ Standard Library version.
#endif
#if defined(__GNU_MP_VERSION) && defined(__GNU_MP_VERSION_MINOR) && defined(__GNU_MP_VERSION_PATCHLEVEL)
    extLibraries += "GNU MP Library " MACRO_STRINGIFY(__GNU_MP_VERSION) "." MACRO_STRINGIFY(
        __GNU_MP_VERSION_MINOR) "." MACRO_STRINGIFY(__GNU_MP_VERSION_PATCHLEVEL) "\n";
#else
#pragma message("Could not find the GNU MP Library version.")
#endif
#if defined(MPFR_VERSION_STRING)
    extLibraries += "GNU MPFR Library " MPFR_VERSION_STRING "\n";
#else
#error Could not find the GNU MPFR Library version.
#endif
#if defined(GSL_VERSION)
    extLibraries += "GNU Scientific Library " GSL_VERSION " (CBLAS)\n";
#else
#error Could not find the GNU Scientific Library (CBLAS) version.
#endif
#if defined(RL_VERSION_MAJOR) && defined(RL_VERSION_MINOR)
    extLibraries +=
        "GNU Readline Library " MACRO_STRINGIFY(RL_VERSION_MAJOR) "." MACRO_STRINGIFY(RL_VERSION_MINOR) "\n";
#else
#error Could not find the GNU Readline Library version.
#endif
#if defined(LZ4_VERSION_STRING)
    extLibraries += "LZ4 Library " LZ4_VERSION_STRING "\n";
#else
#error Could not find the LZ4 Library version.
#endif
#if defined(NCURSES_VERSION)
    extLibraries += "Ncurses Library " NCURSES_VERSION "\n";
#else
#pragma message("Could not find the Ncurses Library version.")
#endif
#if defined(OPENSSL_VERSION_STR)
    extLibraries += "OpenSSL Library " OPENSSL_VERSION_STR "";
#else
#error Could not find the OpenSSL Library version.
#endif
    std::strncpy(val.libInfo, extLibraries.c_str(), sizeof(val.libInfo) - 1);
    val.libInfo[sizeof(val.libInfo) - 1] = '\0';
    char* const buf = buffer.data();
    std::size_t len = buffer.size();
    if (const auto ec = tlv::encodeTLV(buf, len, val); ec)
    {
        throw std::runtime_error{
            "Failed to build packet for the " + std::string{OptDepend::name} + " option (" + ec.message() + ")."};
    }
    data::encryptMessage(buf, len);
    return len;
}

//! @brief Build the TLV packet of the response message to get bash outputs.
//! @param args - container of arguments
//! @param buffer - TLV packet buffer
//! @return buffer length
template <>
std::size_t View::buildCustomTLVPacket<View::OptExecute>(const Args& args, Buffer& buffer)
{
    const auto cmd = std::accumulate(
        args.cbegin(),
        args.cend(),
        std::string{},
        [](const auto& acc, const auto& arg) { return acc.empty() ? arg : (acc + ' ' + arg); });
    if (((cmd.find_first_not_of('\'') == 0) || (cmd.find_last_not_of('\'') == (cmd.length() - 1)))
        && ((cmd.find_first_not_of('"') == 0) || (cmd.find_last_not_of('"') == (cmd.length() - 1))))
    {
        throw std::runtime_error{
            "Please enter the \"" + std::string{OptExecute::name} + "\" and append with 'CMD' (include quotes)."};
    }

    char* const buf = buffer.data();
    std::size_t len = buffer.size();
    const int shmId = fillSharedMemory(utility::io::executeCommand("/bin/bash -c " + cmd));
    if (const auto ec = tlv::encodeTLV(buf, len, tlv::TLVValue{.bashShmId = shmId}); ec)
    {
        throw std::runtime_error{
            "Failed to build packet for the " + std::string{OptExecute::name} + " option (" + ec.message() + ")."};
    }
    data::encryptMessage(buf, len);
    return len;
}

//! @brief Build the TLV packet of the response message to get log contents.
//! @param buffer - TLV packet buffer
//! @return buffer length
template <>
std::size_t View::buildCustomTLVPacket<View::OptJournal>(const Args& /*args*/, Buffer& buffer)
{
    char* const buf = buffer.data();
    std::size_t len = buffer.size();
    const int shmId = fillSharedMemory(logContentsPreview());
    if (const auto ec = tlv::encodeTLV(buf, len, tlv::TLVValue{.logShmId = shmId}); ec)
    {
        throw std::runtime_error{
            "Failed to build packet for the " + std::string{OptJournal::name} + " option (" + ec.message() + ")."};
    }
    data::encryptMessage(buf, len);
    return len;
}

//! @brief Build the TLV packet of the response message to get status reports.
//! @param args - container of arguments
//! @param buffer - TLV packet buffer
//! @return buffer length
template <>
std::size_t View::buildCustomTLVPacket<View::OptMonitor>(const Args& args, Buffer& buffer)
{
    if (!args.empty())
    {
        if (const auto& input = args.front(); (input.length() != 1) || (std::isdigit(input.front()) == 0))
        {
            throw std::runtime_error{
                "Please enter the \"" + std::string{OptMonitor::name} + "\" and append with or without NUM (0 to 9)."};
        }
    }

    char* const buf = buffer.data();
    std::size_t len = buffer.size();
    const int shmId = fillSharedMemory(statusReportsPreview(args.empty() ? 0 : std::stoul(args.front())));
    if (const auto ec = tlv::encodeTLV(buf, len, tlv::TLVValue{.statusShmId = shmId}); ec)
    {
        throw std::runtime_error{
            "Failed to build packet for the " + std::string{OptMonitor::name} + " option (" + ec.message() + ")."};
    }
    data::encryptMessage(buf, len);
    return len;
}

//! @brief Build the TLV packet of the response message to get current configuration.
//! @param buffer - TLV packet buffer
//! @return buffer length
template <>
std::size_t View::buildCustomTLVPacket<View::OptProfile>(const Args& /*args*/, Buffer& buffer)
{
    tlv::TLVValue val{};
    std::strncpy(
        val.configInfo,
        (static_cast<const utility::json::JSON&>(configure::retrieveDataRepo())).asUnescapedString().c_str(),
        sizeof(val.configInfo) - 1);
    val.configInfo[sizeof(val.configInfo) - 1] = '\0';
    char* const buf = buffer.data();
    std::size_t len = buffer.size();
    if (const auto ec = tlv::encodeTLV(buf, len, val); ec)
    {
        throw std::runtime_error{
            "Failed to build packet for the " + std::string{OptProfile::name} + " option (" + ec.message() + ")."};
    }
    data::encryptMessage(buf, len);
    return len;
}

std::size_t View::buildResponse(const std::string& reqPlaintext, Buffer& respBuffer)
{
    return utility::common::patternMatch(
        extractOption(reqPlaintext),
        [&respBuffer](const OptDepend& opt) { return buildCustomTLVPacket<OptDepend>(opt.args, respBuffer); },
        [&respBuffer](const OptExecute& opt) { return buildCustomTLVPacket<OptExecute>(opt.args, respBuffer); },
        [&respBuffer](const OptJournal& opt) { return buildCustomTLVPacket<OptJournal>(opt.args, respBuffer); },
        [&respBuffer](const OptMonitor& opt) { return buildCustomTLVPacket<OptMonitor>(opt.args, respBuffer); },
        [&respBuffer](const OptProfile& opt) { return buildCustomTLVPacket<OptProfile>(opt.args, respBuffer); },
        [](const auto& opt)
        {
            if (const auto* origPtr = std::addressof(opt); dynamic_cast<const OptBase*>(origPtr))
            {
                throw std::runtime_error{
                    "The option is unprocessed due to unregistered or potential registration failures (typeid: "
                    + std::string{typeid(opt).name()} + ")."};
            }
            return 0UL;
        });
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

std::size_t View::buildAckTLVPacket(Buffer& buffer)
{
    char* const buf = buffer.data();
    std::size_t len = buffer.size();
    std::ignore = tlv::encodeTLV(buf, len, tlv::TLVValue{});
    data::encryptMessage(buf, len);
    return len;
}

std::size_t View::buildFinTLVPacket(Buffer& buffer)
{
    char* const buf = buffer.data();
    std::size_t len = buffer.size();
    std::ignore = tlv::encodeTLV(buf, len, tlv::TLVValue{.stopTag = true});
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

void View::segmentedOutput(const std::string& cache)
{
    constexpr std::uint8_t terminalRows = 24;
    constexpr std::string_view prompt = "--- Type <CR> for more, c to continue, n to show next page, q to quit ---: ";
    constexpr std::string_view escapeClear = "\033[1A\033[2K\r";
    constexpr std::string_view escapeMoveUp = "\n\033[1A\033[";
    std::istringstream transfer(cache);
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
            std::ranges::for_each(logRows, [](auto& line) { log::changeToLogStyle(line); });
            std::ranges::copy(logRows, std::ostream_iterator<std::string>(transfer, "\n"));
        });
    return std::move(transfer).str();
}

std::string View::statusReportsPreview(const std::uint16_t frame)
{
    if ((frame > 0)
        && (::system( // NOLINT(cert-env33-c, concurrency-mt-unsafe)
                "command -v eu-stack >/dev/null 2>&1 "
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
                "&& (timeout --preserve-status -s SIGINT 1 stdbuf -o0 eu-stack -1v -n %u -p %d 2>&1 | grep '#' "
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
            const std::weak_ptr weakSock = client;
            client->subscribeMessage(
                [weakSock](const std::string_view message)
                {
                    auto currSock = weakSock.lock();
                    if (!currSock || message.empty())
                    {
                        return;
                    }

                    Buffer respBuffer{};
                    try
                    {
                        const auto reqPlaintext = utility::common::base64Decode(message);
                        currSock->toSend(
                            respBuffer.data(),
                            (reqPlaintext != exitSymbol) ? buildResponse(reqPlaintext, respBuffer)
                                                         : buildFinTLVPacket(respBuffer));
                    }
                    catch (const std::exception& err)
                    {
                        LOG_WRN << err.what();
                        currSock->toSend(respBuffer.data(), buildAckTLVPacket(respBuffer));
                    }
                });
        });
}

//! @brief Renew the UDP server.
template <>
void View::renewServer<utility::socket::UDPServer>()
{
    udpServer = std::make_shared<utility::socket::UDPServer>();
    const std::weak_ptr weakSock = udpServer;
    udpServer->subscribeMessage(
        [weakSock](const std::string_view message, const std::string& ip, const std::uint16_t port)
        {
            auto currSock = weakSock.lock();
            if (!currSock || message.empty())
            {
                return;
            }

            Buffer respBuffer{};
            try
            {
                const auto reqPlaintext = utility::common::base64Decode(message);
                currSock->toSendTo(
                    respBuffer.data(),
                    (reqPlaintext != exitSymbol) ? buildResponse(reqPlaintext, respBuffer)
                                                 : buildFinTLVPacket(respBuffer),
                    ip,
                    port);
            }
            catch (const std::exception& err)
            {
                LOG_WRN << err.what();
                currSock->toSendTo(respBuffer.data(), buildAckTLVPacket(respBuffer), ip, port);
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
    if (tcpServer)
    {
        tcpServer->toClose();
        tcpServer->toJoin();
    }
    tcpServer.reset();

    if (udpServer)
    {
        udpServer->toClose();
        udpServer->toJoin();
    }
    udpServer.reset();
}

void View::startViewing()
{
    if (tcpServer)
    {
        tcpServer->toBind(tcpPort);
        tcpServer->toListen();
        tcpServer->toAccept();
    }

    if (udpServer)
    {
        udpServer->toBind(udpPort);
        udpServer->toReceiveFrom();
    }
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
    utility::benchmark::escape(this);
}

void View::doRollback()
{
    const std::scoped_lock locks(daemonMtx, outputMtx);
    isOngoing.store(false);

    destroyViewServer();

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
