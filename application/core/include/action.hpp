//! @file action.hpp
//! @author ryftchen
//! @brief The declarations (action) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#ifndef _PRECOMPILED_HEADER
#include <coroutine>
#else
#include "application/pch/precompiled_header.hpp"
#endif // _PRECOMPILED_HEADER

#include "application/example/include/register_algorithm.hpp"
#include "application/example/include/register_data_structure.hpp"
#include "application/example/include/register_design_pattern.hpp"
#include "application/example/include/register_numeric.hpp"

//! @brief The application module.
namespace application // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Applied-action-related functions in the application module.
namespace action
{
//! @brief Awaitable coroutine.
class Awaitable final
{
public:
    // NOLINTBEGIN(readability-identifier-naming)
    //! @brief Promise type for use in coroutines.
    struct promise_type
    {
        //! @brief Get the return object for the coroutine.
        //! @return awaitable instance
        Awaitable get_return_object() { return Awaitable{std::coroutine_handle<promise_type>::from_promise(*this)}; }
        //! @brief Initial suspend point of the coroutine.
        //! @return std::suspend_never object indicating that the coroutine should not be suspended initially
        static std::suspend_never initial_suspend() noexcept { return {}; }
        //! @brief Final suspend point of the coroutine.
        //! @return std::suspend_always object indicating that the coroutine should be suspended finally
        static std::suspend_always final_suspend() noexcept { return {}; }
        //! @brief Complete the coroutine without returning a value.
        static void return_void() noexcept {}
        //! @brief Handle exceptions thrown within the coroutine.
        static void unhandled_exception() { std::rethrow_exception(std::current_exception()); }
    };
    // NOLINTEND(readability-identifier-naming)

    //! @brief Construct a new Awaitable object.
    //! @param handle - coroutine handle
    explicit Awaitable(const std::coroutine_handle<promise_type>& handle);
    //! @brief Destroy the Awaitable object.
    ~Awaitable();
    //! @brief Construct a new Awaitable object.
    Awaitable(const Awaitable&) = delete;
    //! @brief Construct a new Awaitable object.
    Awaitable(Awaitable&&) = delete;
    //! @brief The operator (=) overloading of Awaitable class.
    //! @return reference of the Awaitable object
    Awaitable& operator=(const Awaitable&) = delete;
    //! @brief The operator (=) overloading of Awaitable class.
    //! @return reference of the Awaitable object
    Awaitable& operator=(Awaitable&&) = delete;

    //! @brief Resume the execution of the coroutine if it is suspended.
    void resume() const;
    //! @brief Check whether the coroutine has been completed.
    //! @return be done or not
    [[nodiscard]] bool done() const;

private:
    //! @brief Coroutine handle.
    std::coroutine_handle<promise_type> handle;
    //! @brief Flag to ensure only one instance is active.
    static std::atomic_bool active;
};

//! @brief Alias for the type information.
//! @tparam UDT - type of user defined data
template <typename UDT>
using TypeInfo = utility::reflection::TypeInfo<UDT>;
//! @brief Get the name field directly for sub-cli related registrations.
//! @tparam MappedCLI - type of sub-cli or sub-cli's category
//! @return name field
template <typename MappedCLI>
consteval std::string_view name()
{
    return TypeInfo<MappedCLI>::name;
}
//! @brief Get the alias attribute directly for sub-cli related registrations.
//! @tparam Meth - type of sub-cli's category
//! @return alias attribute
template <typename Meth>
requires std::is_same_v<Meth, reg_algo::MatchMethod> || std::is_same_v<Meth, reg_algo::NotationMethod>
    || std::is_same_v<Meth, reg_algo::OptimalMethod> || std::is_same_v<Meth, reg_algo::SearchMethod>
    || std::is_same_v<Meth, reg_algo::SortMethod>
consteval std::string_view alias()
{
    return TypeInfo<reg_algo::ApplyAlgorithm>::fields.find(REFLECTION_STR(TypeInfo<Meth>::name))
        .attrs.find(REFLECTION_STR("alias"))
        .value;
}
//! @brief Get the alias attribute directly for sub-cli related registrations.
//! @tparam Inst - type of sub-cli's category
//! @return alias attribute
template <typename Inst>
requires std::is_same_v<Inst, reg_dp::BehavioralInstance> || std::is_same_v<Inst, reg_dp::CreationalInstance>
    || std::is_same_v<Inst, reg_dp::StructuralInstance>
consteval std::string_view alias()
{
    return TypeInfo<reg_dp::ApplyDesignPattern>::fields.find(REFLECTION_STR(TypeInfo<Inst>::name))
        .attrs.find(REFLECTION_STR("alias"))
        .value;
}
//! @brief Get the alias attribute directly for sub-cli related registrations.
//! @tparam Inst - type of sub-cli's category
//! @return alias attribute
template <typename Inst>
requires std::is_same_v<Inst, reg_ds::CacheInstance> || std::is_same_v<Inst, reg_ds::FilterInstance>
    || std::is_same_v<Inst, reg_ds::GraphInstance> || std::is_same_v<Inst, reg_ds::HeapInstance>
    || std::is_same_v<Inst, reg_ds::LinearInstance> || std::is_same_v<Inst, reg_ds::TreeInstance>
consteval std::string_view alias()
{
    return TypeInfo<reg_ds::ApplyDataStructure>::fields.find(REFLECTION_STR(TypeInfo<Inst>::name))
        .attrs.find(REFLECTION_STR("alias"))
        .value;
}
//! @brief Get the alias attribute directly for sub-cli related registrations.
//! @tparam Meth - type of sub-cli's category
//! @return alias attribute
template <typename Meth>
requires std::is_same_v<Meth, reg_num::ArithmeticMethod> || std::is_same_v<Meth, reg_num::DivisorMethod>
    || std::is_same_v<Meth, reg_num::IntegralMethod> || std::is_same_v<Meth, reg_num::PrimeMethod>
consteval std::string_view alias()
{
    return TypeInfo<reg_num::ApplyNumeric>::fields.find(REFLECTION_STR(TypeInfo<Meth>::name))
        .attrs.find(REFLECTION_STR("alias"))
        .value;
}
//! @brief Get the description attribute directly for sub-cli related registrations.
//! @tparam MappedCLI - type of sub-cli or sub-cli's category
//! @return description attribute
template <typename MappedCLI>
consteval std::string_view descr()
{
    return TypeInfo<MappedCLI>::attrs.find(REFLECTION_STR("descr")).value;
}

//! @brief The "Set Choice" message in the applied action.
//! @tparam Evt - type of applied action event
template <typename Evt>
struct SetChoice
{
    //! @brief Target choice.
    const std::string choice;
};
//! @brief The "Run Candidates" message in the applied action.
//! @tparam Evt - type of applied action event
template <typename Evt>
struct RunCandidates
{
    //! @brief Collection of candidates for choice.
    const std::vector<std::string> candidates;
};
//! @brief Indication type of setting in the applied action.
//! @tparam Msg - type of message
template <typename Msg>
struct SettingIndication;
//! @brief Indication type of running in the applied action.
//! @tparam Msg - type of message
template <typename Msg>
struct RunningIndication;
//! @brief Message type list.
//! @tparam Types - type of indication types
template <typename... Types>
struct MessageTypeList
{
    //! @brief Alias for the parameter pack.
    //! @tparam InnerTypes - type of inner types
    //! @tparam TemplatedType - type of templated type
    template <template <typename... InnerTypes> class TemplatedType>
    using AsParameterPackFor = TemplatedType<Types...>;
    //! @brief Alias for the providing interface.
    //! @tparam Intf - type of interface
    template <typename Intf>
    using WithInterface = MessageTypeList<Types..., Intf>;
};
//! @brief Alias for the message type list.
using MessageTypes = MessageTypeList<
    SettingIndication<SetChoice<reg_algo::MatchMethod>>,
    RunningIndication<RunCandidates<reg_algo::MatchMethod>>,
    SettingIndication<SetChoice<reg_algo::NotationMethod>>,
    RunningIndication<RunCandidates<reg_algo::NotationMethod>>,
    SettingIndication<SetChoice<reg_algo::OptimalMethod>>,
    RunningIndication<RunCandidates<reg_algo::OptimalMethod>>,
    SettingIndication<SetChoice<reg_algo::SearchMethod>>,
    RunningIndication<RunCandidates<reg_algo::SearchMethod>>,
    SettingIndication<SetChoice<reg_algo::SortMethod>>,
    RunningIndication<RunCandidates<reg_algo::SortMethod>>,
    SettingIndication<SetChoice<reg_dp::BehavioralInstance>>,
    RunningIndication<RunCandidates<reg_dp::BehavioralInstance>>,
    SettingIndication<SetChoice<reg_dp::CreationalInstance>>,
    RunningIndication<RunCandidates<reg_dp::CreationalInstance>>,
    SettingIndication<SetChoice<reg_dp::StructuralInstance>>,
    RunningIndication<RunCandidates<reg_dp::StructuralInstance>>,
    SettingIndication<SetChoice<reg_ds::CacheInstance>>,
    RunningIndication<RunCandidates<reg_ds::CacheInstance>>,
    SettingIndication<SetChoice<reg_ds::FilterInstance>>,
    RunningIndication<RunCandidates<reg_ds::FilterInstance>>,
    SettingIndication<SetChoice<reg_ds::GraphInstance>>,
    RunningIndication<RunCandidates<reg_ds::GraphInstance>>,
    SettingIndication<SetChoice<reg_ds::HeapInstance>>,
    RunningIndication<RunCandidates<reg_ds::HeapInstance>>,
    SettingIndication<SetChoice<reg_ds::LinearInstance>>,
    RunningIndication<RunCandidates<reg_ds::LinearInstance>>,
    SettingIndication<SetChoice<reg_ds::TreeInstance>>,
    RunningIndication<RunCandidates<reg_ds::TreeInstance>>,
    SettingIndication<SetChoice<reg_num::ArithmeticMethod>>,
    RunningIndication<RunCandidates<reg_num::ArithmeticMethod>>,
    SettingIndication<SetChoice<reg_num::DivisorMethod>>,
    RunningIndication<RunCandidates<reg_num::DivisorMethod>>,
    SettingIndication<SetChoice<reg_num::IntegralMethod>>,
    RunningIndication<RunCandidates<reg_num::IntegralMethod>>,
    SettingIndication<SetChoice<reg_num::PrimeMethod>>,
    RunningIndication<RunCandidates<reg_num::PrimeMethod>>>;

//! @brief Alias for the message handler.
//! @tparam Msg - type of message
template <typename Msg>
using Handler = std::function<void(const Msg&)>;
//! @brief Message dispatcher.
//! @tparam Is - type of indications
template <typename... Is>
class Dispatcher;
//! @brief Message dispatcher of the setting indication.
//! @tparam Msg - type of message
//! @tparam Is - type of indications
template <typename Msg, typename... Is>
class Dispatcher<SettingIndication<Msg>, Is...> : public Dispatcher<Is...>
{
public:
    using Dispatcher<Is...>::registerHandler;
    //! @brief Register the handler.
    //! @param handling - handling for message
    virtual void registerHandler(Handler<Msg> handling) = 0;
};
//! @brief Message dispatcher of the running indication.
//! @tparam Msg - type of messages
//! @tparam Is - type of indications
template <typename Msg, typename... Is>
class Dispatcher<RunningIndication<Msg>, Is...> : public Dispatcher<Is...>
{
public:
    using Dispatcher<Is...>::registerHandler;
    //! @brief Register the handler.
    //! @param handling - handling for message
    virtual void registerHandler(Handler<Msg> handling) = 0;
};
//! @brief Default message dispatcher.
template <>
class Dispatcher<>
{
public:
    //! @brief Construct a new Dispatcher object.
    Dispatcher() = default;
    //! @brief Destroy the Dispatcher object.
    virtual ~Dispatcher() = default;
    //! @brief Construct a new Dispatcher object.
    Dispatcher(const Dispatcher&) = default;
    //! @brief Construct a new Dispatcher object.
    Dispatcher(Dispatcher&&) noexcept = default;
    //! @brief The operator (=) overloading of Dispatcher class.
    //! @return reference of the Dispatcher object
    Dispatcher& operator=(const Dispatcher&) = default;
    //! @brief The operator (=) overloading of Dispatcher class.
    //! @return reference of the Dispatcher object
    Dispatcher& operator=(Dispatcher&&) noexcept = default;

    //! @brief Register the handler.
    void registerHandler();
};
//! @brief Message receiver.
//! @tparam Is - type of indications
template <typename... Is>
class Receiver;
//! @brief Message receiver of the setting indication.
//! @tparam Msg - type of message
//! @tparam Is - type of indications
template <typename Msg, typename... Is>
class Receiver<SettingIndication<Msg>, Is...> : public Receiver<Is...>
{
public:
    using Receiver<Is...>::onMessage;
    //! @brief Action on message.
    //! @param message - message body
    virtual void onMessage(const Msg& message) = 0;
};
//! @brief Message receiver of the running indication.
//! @tparam Msg - type of message
//! @tparam Is - type of indications
template <typename Msg, typename... Is>
class Receiver<RunningIndication<Msg>, Is...> : public Receiver<Is...>
{
public:
    using Receiver<Is...>::onMessage;
    //! @brief Action on message.
    //! @param message - message body
    virtual void onMessage(const Msg& message) = 0;
};
//! @brief Default message receiver.
template <>
class Receiver<>
{
public:
    //! @brief Construct a new Receiver object.
    Receiver() = default;
    //! @brief Destroy the Receiver object.
    virtual ~Receiver() = default;
    //! @brief Construct a new Receiver object.
    Receiver(const Receiver&) = default;
    //! @brief Construct a new Receiver object.
    Receiver(Receiver&&) noexcept = default;
    //! @brief The operator (=) overloading of Receiver class.
    //! @return reference of the Receiver object
    Receiver& operator=(const Receiver&) = default;
    //! @brief The operator (=) overloading of Receiver class.
    //! @return reference of the Receiver object
    Receiver& operator=(Receiver&&) noexcept = default;

    //! @brief Action on message.
    void onMessage();
};
//! @brief Forwarding basis for all message types.
struct ForwardBase : public MessageTypes::AsParameterPackFor<Dispatcher>,
                     public MessageTypes::AsParameterPackFor<Receiver>
{
};
//! @brief Forwarding action.
//! @tparam Is - type of indications
template <typename... Is>
class Forward;
//! @brief Forward message of the setting indication.
//! @tparam Msg - type of message
//! @tparam Is - type of indications
template <typename Msg, typename... Is>
class Forward<SettingIndication<Msg>, Is...> : public Forward<Is...>
{
public:
    //! @brief Alias for the base class.
    using Base = Forward<Is...>;
    using Base::registerHandler, Base::onMessage;
    //! @brief Register the handler.
    //! @param handling - handling for message
    void registerHandler(Handler<Msg> handling) override;
    //! @brief Action on message.
    //! @param message - message body
    void onMessage(const Msg& message) override;

private:
    //! @brief Message handler.
    Handler<Msg> handler{};
};
template <typename Msg, typename... Is>
void Forward<SettingIndication<Msg>, Is...>::registerHandler(Handler<Msg> handling)
{
    handler = std::move(handling);
}
template <typename Msg, typename... Is>
void Forward<SettingIndication<Msg>, Is...>::onMessage(const Msg& message)
{
    if (handler)
    {
        handler(message);
    }
}
//! @brief Forward message of the running indication.
//! @tparam Msg - type of message
//! @tparam Is - type of indications
template <typename Msg, typename... Is>
class Forward<RunningIndication<Msg>, Is...> : public Forward<Is...>
{
public:
    //! @brief Alias for the base class.
    using Base = Forward<Is...>;
    using Base::registerHandler, Base::onMessage;
    //! @brief Register the handler.
    //! @param handling - handling for message
    void registerHandler(Handler<Msg> handling) override;
    //! @brief Action on message.
    //! @param message - message body
    void onMessage(const Msg& message) override;

private:
    //! @brief Message handler.
    Handler<Msg> handler{};
};
template <typename Msg, typename... Is>
void Forward<RunningIndication<Msg>, Is...>::registerHandler(Handler<Msg> handling)
{
    handler = std::move(handling);
}
template <typename Msg, typename... Is>
void Forward<RunningIndication<Msg>, Is...>::onMessage(const Msg& message)
{
    if (handler)
    {
        handler(message);
    }
}
//! @brief Forwarding action interface.
//! @tparam Intf - type of interface
template <typename Intf>
class Forward<Intf> : public Intf
{
};

//! @brief Message forwarder.
struct MessageForwarder : public MessageTypes::WithInterface<ForwardBase>::AsParameterPackFor<Forward>
{
};

//! @brief Applying event type object's helper type for the visitor.
//! @tparam Ts - type of visitors
template <typename... Ts>
struct EventVisitor : public Ts...
{
    using Ts::operator()...;
};
//! @brief Explicit deduction guide for EventVisitor.
//! @tparam Ts - type of visitors
template <typename... Ts>
EventVisitor(Ts...) -> EventVisitor<Ts...>;
//! @brief Alias for the applied action event.
using Event = std::variant<
    reg_algo::MatchMethod,
    reg_algo::NotationMethod,
    reg_algo::OptimalMethod,
    reg_algo::SearchMethod,
    reg_algo::SortMethod,
    reg_dp::BehavioralInstance,
    reg_dp::CreationalInstance,
    reg_dp::StructuralInstance,
    reg_ds::CacheInstance,
    reg_ds::FilterInstance,
    reg_ds::GraphInstance,
    reg_ds::HeapInstance,
    reg_ds::LinearInstance,
    reg_ds::TreeInstance,
    reg_num::ArithmeticMethod,
    reg_num::DivisorMethod,
    reg_num::IntegralMethod,
    reg_num::PrimeMethod>;
} // namespace action
} // namespace application
