//! @file action.hpp
//! @author ryftchen
//! @brief The declarations (action) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#ifndef __PRECOMPILED_HEADER
#include <coroutine>
#include <functional>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

#include "application/example/include/apply_algorithm.hpp"
#include "application/example/include/apply_data_structure.hpp"
#include "application/example/include/apply_design_pattern.hpp"
#include "application/example/include/apply_numeric.hpp"

//! @brief The application module.
namespace application // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Applied-action-related functions in the application module.
namespace action
{
//! @brief Awaitable coroutine.
class Awaitable
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
    explicit Awaitable(const std::coroutine_handle<promise_type>& handle) : handle{handle} {}
    //! @brief Destroy the Awaitable object.
    virtual ~Awaitable();
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
    std::coroutine_handle<promise_type> handle{};
};

//! @brief The "Update Choice" message in the applied action.
//! @tparam Evt - type of applied action event
template <typename Evt>
struct UpdateChoice
{
    //! @brief Target choice.
    const std::string cho{};
};
//! @brief The "Run Choices" message in the applied action.
//! @tparam Evt - type of applied action event
template <typename Evt>
struct RunChoices
{
    //! @brief Collection of candidates for choice.
    const std::vector<std::string> coll{};
};
//! @brief Indication type of updating in the applied action.
//! @tparam Msg - type of message
template <typename Msg>
struct UpdatingIndication
{
};
//! @brief Indication type of running in the applied action.
//! @tparam Msg - type of message
template <typename Msg>
struct RunningIndication
{
};
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
    UpdatingIndication<UpdateChoice<app_algo::MatchMethod>>,
    RunningIndication<RunChoices<app_algo::MatchMethod>>,
    UpdatingIndication<UpdateChoice<app_algo::NotationMethod>>,
    RunningIndication<RunChoices<app_algo::NotationMethod>>,
    UpdatingIndication<UpdateChoice<app_algo::OptimalMethod>>,
    RunningIndication<RunChoices<app_algo::OptimalMethod>>,
    UpdatingIndication<UpdateChoice<app_algo::SearchMethod>>,
    RunningIndication<RunChoices<app_algo::SearchMethod>>,
    UpdatingIndication<UpdateChoice<app_algo::SortMethod>>,
    RunningIndication<RunChoices<app_algo::SortMethod>>,
    UpdatingIndication<UpdateChoice<app_dp::BehavioralInstance>>,
    RunningIndication<RunChoices<app_dp::BehavioralInstance>>,
    UpdatingIndication<UpdateChoice<app_dp::CreationalInstance>>,
    RunningIndication<RunChoices<app_dp::CreationalInstance>>,
    UpdatingIndication<UpdateChoice<app_dp::StructuralInstance>>,
    RunningIndication<RunChoices<app_dp::StructuralInstance>>,
    UpdatingIndication<UpdateChoice<app_ds::LinearInstance>>,
    RunningIndication<RunChoices<app_ds::LinearInstance>>,
    UpdatingIndication<UpdateChoice<app_ds::TreeInstance>>,
    RunningIndication<RunChoices<app_ds::TreeInstance>>,
    UpdatingIndication<UpdateChoice<app_num::ArithmeticMethod>>,
    RunningIndication<RunChoices<app_num::ArithmeticMethod>>,
    UpdatingIndication<UpdateChoice<app_num::DivisorMethod>>,
    RunningIndication<RunChoices<app_num::DivisorMethod>>,
    UpdatingIndication<UpdateChoice<app_num::IntegralMethod>>,
    RunningIndication<RunChoices<app_num::IntegralMethod>>,
    UpdatingIndication<UpdateChoice<app_num::PrimeMethod>>,
    RunningIndication<RunChoices<app_num::PrimeMethod>>>;

//! @brief Alias for the message handler.
//! @tparam Msg - type of message
template <typename Msg>
using Handler = std::function<void(const Msg&)>;
//! @brief Message dispatcher.
//! @tparam Is - type of indications
template <typename... Is>
class Dispatcher;
//! @brief Message dispatcher of the updating indication.
//! @tparam Msg - type of message
//! @tparam Is - type of indications
template <typename Msg, typename... Is>
class Dispatcher<UpdatingIndication<Msg>, Is...> : public Dispatcher<Is...>
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
    //! @brief Register the handler.
    void registerHandler() {}
};
//! @brief Message receiver.
//! @tparam Is - type of indications
template <typename... Is>
class Receiver;
//! @brief Message receiver of the updating indication.
//! @tparam Msg - type of message
//! @tparam Is - type of indications
template <typename Msg, typename... Is>
class Receiver<UpdatingIndication<Msg>, Is...> : public Receiver<Is...>
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
    //! @brief Action on message.
    void onMessage() {}
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
//! @brief Forward message of the updating indication.
//! @tparam Msg - type of message
//! @tparam Is - type of indications
template <typename Msg, typename... Is>
class Forward<UpdatingIndication<Msg>, Is...> : public Forward<Is...>
{
public:
    //! @brief Alias for the base class.
    using Base = Forward<Is...>;
    using Base::registerHandler, Base::onMessage;
    //! @brief Register the handler.
    //! @param handling - handling for message
    void registerHandler(Handler<Msg> handling) override { handler = std::move(handling); }
    //! @brief Action on message.
    //! @param message - message body
    void onMessage(const Msg& message) override
    {
        if (handler)
        {
            handler(message);
        }
    }

private:
    //! @brief Message handler.
    Handler<Msg> handler{};
};
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
    void registerHandler(Handler<Msg> handling) override { handler = std::move(handling); }
    //! @brief Action on message.
    //! @param message - message body
    void onMessage(const Msg& message) override
    {
        if (handler)
        {
            handler(message);
        }
    }

private:
    //! @brief Message handler.
    Handler<Msg> handler{};
};
//! @brief Forwarding action interface.
//! @tparam Intf - type of interface
template <typename Intf>
class Forward<Intf> : public Intf
{
};

//! @brief Message forwarder.
class MessageForwarder : public MessageTypes::WithInterface<ForwardBase>::AsParameterPackFor<Forward>
{
};

//! @brief Applying event type object's helper type for the visitor.
//! @tparam Ts - type of visitors
template <typename... Ts>
struct EvtVisitor : Ts...
{
    using Ts::operator()...;
};
//! @brief Explicit deduction guide for EvtVisitor.
//! @tparam Ts - type of visitors
template <typename... Ts>
EvtVisitor(Ts...) -> EvtVisitor<Ts...>;
//! @brief Alias for the applied action event type.
using EventType = std::variant<
    app_algo::MatchMethod,
    app_algo::NotationMethod,
    app_algo::OptimalMethod,
    app_algo::SearchMethod,
    app_algo::SortMethod,
    app_dp::BehavioralInstance,
    app_dp::CreationalInstance,
    app_dp::StructuralInstance,
    app_ds::LinearInstance,
    app_ds::TreeInstance,
    app_num::ArithmeticMethod,
    app_num::DivisorMethod,
    app_num::IntegralMethod,
    app_num::PrimeMethod>;
} // namespace action
} // namespace application
