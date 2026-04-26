//! @file schedule.hpp
//! @author ryftchen
//! @brief The declarations (schedule) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2026 ryftchen. All rights reserved.

#pragma once

#ifndef _PRECOMPILED_HEADER
#include <coroutine>
#include <map>
#else
#include "application/pch/precompiled_header.hpp"
#endif

#include "application/example/include/register_algorithm.hpp"
#include "application/example/include/register_data_structure.hpp"
#include "application/example/include/register_design_pattern.hpp"
#include "application/example/include/register_numeric.hpp"

//! @brief The application module.
namespace application // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Applied-schedule-related functions in the application module.
namespace schedule
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

extern void enterNextPhase(Awaitable& awaitable);

//! @brief Native schedule.
namespace native
{
//! @brief Represent the maximum value of an enum.
//! @tparam Enum - type of specific enum
template <typename Enum>
struct Bottom;

//! @brief Enumerate specific native categories.
enum class Category : std::uint8_t
{
    //! @brief Console.
    console,
    //! @brief Dump.
    dump,
    //! @brief Help.
    help,
    //! @brief Version.
    version
};
//! @brief Store the maximum value of the Category enum.
template <>
struct Bottom<Category>
{
    //! @brief Maximum value of the Category enum.
    static constexpr std::uint8_t value{4};
};

//! @brief Gather and notify handlers.
//! @tparam Ctx - type of context
//! @tparam Key - type of key
template <typename Ctx, typename Key = Category>
class Notifier
{
public:
    //! @brief The base procedure when notified.
    class Procedure
    {
    public:
        //! @brief Destroy the Procedure object.
        virtual ~Procedure() = default;

        //! @brief Perform the specific operation.
        virtual void execute() const = 0;
    };
    //! @brief The procedure when notified.
    //! @tparam CRTP - type of derived class
    template <typename CRTP>
    class ProcedureImpl : public Procedure
    {
    public:
        //! @brief Perform the specific operation.
        void execute() const override { static_cast<const CRTP&>(*this).execute(); }
    };
    //! @brief The handler used to trigger a procedure when notified.
    //! @tparam key - specific key
    template <Key key>
    class Handler : public ProcedureImpl<Handler<key>>
    {
    public:
        //! @brief Construct a new Handler object.
        //! @param ctx - involved context
        explicit Handler(const Ctx& ctx) : ctx{ctx} {}

        //! @brief Perform the specific operation.
        void execute() const override;

    private:
        //! @brief The involved context.
        const Ctx& ctx{};
    };

    //! @brief Attach a handler with a specific key to the notifier.
    //! @param key - specific key
    //! @param handler - handler to be attached
    void attach(const Key key, std::shared_ptr<Procedure> handler);
    //! @brief Notify the handler associated with the given key.
    //! @param key - specific key
    void notify(const Key key) const;

private:
    //! @brief Map of handlers identified by a key.
    std::map<Key, std::shared_ptr<Procedure>> handlers{};
};

template <typename Ctx, typename Key>
void Notifier<Ctx, Key>::attach(const Key key, std::shared_ptr<Procedure> handler)
{
    handlers[key] = std::move(handler);
}

template <typename Ctx, typename Key>
void Notifier<Ctx, Key>::notify(const Key key) const
{
    if (handlers.contains(key))
    {
        handlers.at(key)->execute();
    }
}
} // namespace native

//! @brief Extra schedule.
namespace extra
{
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
    //! @brief Destroy the Dispatcher object.
    virtual ~Dispatcher() = default;

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
    //! @brief Destroy the Receiver object.
    virtual ~Receiver() = default;

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

//! @brief Alias for the applied action event type.
using EventType = std::variant<
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
} // namespace extra

//! @brief Manage tasks.
class TaskManager
{
public:
    //! @brief Destroy the TaskManager object.
    virtual ~TaskManager() = default;

    //! @brief Check whether any tasks do not exist.
    //! @return any tasks do not exist or exist
    [[nodiscard]] virtual bool empty() const = 0;
    //! @brief Reset bit flags that manage all tasks.
    virtual void reset() = 0;
};

//! @brief Manage native categories.
class NativeManager : virtual public TaskManager
{
public:
    //! @brief Bit flags for managing native categories.
    std::bitset<native::Bottom<native::Category>::value> nativeCategories;

    //! @brief Check whether any native categories do not exist.
    //! @return any native categories do not exist or exist
    [[nodiscard]] bool empty() const override;
    //! @brief Reset bit flags that manage native categories.
    void reset() override;
};

//! @brief Manage extra choices of sub-cli.
class ExtraManager : virtual public TaskManager
{
public:
    //! @brief Alias for the attribute of the registered sub-cli's category.
    struct Attr
    {
        //! @brief The candidates for the choice.
        const std::vector<std::string> choices;
        //! @brief The internal event for applying.
        const extra::EventType event;
    };
    //! @brief Alias for the map of sub-cli's category name and Attr.
    using CategoryMap = std::map<std::string, Attr>;
    //! @brief Mapping table of all extra choices. Fill as needed.
    std::map<std::string, CategoryMap> extraChoiceRegistry;

    //! @brief Wrap interfaces to check for existing and reset extra choices.
    struct Intf
    {
        //! @brief Construct a new Intf object.
        //! @param presentCb - callback of checking
        //! @param clearCb - callback of resetting
        Intf(std::function<bool()> presentCb, std::function<void()> clearCb) :
            present{std::move(presentCb)}, clear{std::move(clearCb)}
        {
            if (!present || !clear)
            {
                throw std::runtime_error{"Invalid sub-command interfaces are being used."};
            }
        }

        //! @brief Check the existence status of the extra choice.
        const std::function<bool()> present;
        //! @brief Reset control of the extra choice.
        const std::function<void()> clear;
    };
    //! @brief Existence status and reset control of the sub-cli to which the extra choices belong.
    std::map<std::string, Intf> extraChecklist;
    //! @brief Flag for help only.
    bool extraHelping{false};

    //! @brief Check whether any extra choices do not exist.
    //! @return any extra choices do not exist or exist
    [[nodiscard]] bool empty() const override;
    //! @brief Reset bit flags that manage extra choices.
    void reset() override;
};

//! @brief Schedule all managed tasks.
class TaskScheduler : public NativeManager, public ExtraManager
{
public:
    //! @brief Check whether any tasks do not exist.
    //! @return any tasks do not exist or exist
    [[nodiscard]] bool empty() const final;
    //! @brief Reset bit flags that manage all tasks.
    void reset() final;
};

//! @brief Registration metadata.
namespace meta
{
//! @brief Alias for the type information.
//! @tparam UDT - type of user defined data
template <typename UDT>
using TypeInfo = utility::reflection::TypeInfo<UDT>;

//! @brief Get the name field directly for sub-cli related registrations.
//! @tparam Mapped - type of sub-cli or sub-cli's category
//! @return name field
template <typename Mapped>
consteval std::string_view name()
{
    return TypeInfo<Mapped>::name;
}

//! @brief Get the alias attribute directly for sub-cli related registrations.
//! @tparam Meth - type of sub-cli's category
//! @return alias attribute
template <typename Meth>
requires reg_algo::Registrant<Meth>
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
requires reg_dp::Registrant<Inst>
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
requires reg_ds::Registrant<Inst>
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
requires reg_num::Registrant<Meth>
consteval std::string_view alias()
{
    return TypeInfo<reg_num::ApplyNumeric>::fields.find(REFLECTION_STR(TypeInfo<Meth>::name))
        .attrs.find(REFLECTION_STR("alias"))
        .value;
}

//! @brief Get the description attribute directly for sub-cli related registrations.
//! @tparam Mapped - type of sub-cli or sub-cli's category
//! @return description attribute
template <typename Mapped>
consteval std::string_view descr()
{
    return TypeInfo<Mapped>::attrs.find(REFLECTION_STR("descr")).value;
}

//! @brief Extract all choices in the sub-cli's category.
//! @tparam Cat - type of sub-cli's category
//! @return all choices
template <typename Cat>
constexpr std::vector<std::string> choice()
{
    constexpr auto refl = REFLECTION_STR("choice");
    std::vector<std::string> choices{};
    choices.reserve(TypeInfo<Cat>::fields.size);
    TypeInfo<Cat>::fields.forEach(
        [refl, &choices](const auto field)
        {
            static_assert(field.attrs.contains(refl) && (field.attrs.size == 1));
            const auto attr = field.attrs.find(refl);
            static_assert(attr.hasValue);
            choices.emplace_back(attr.value);
        });
    return choices;
}
} // namespace meta
} // namespace schedule
} // namespace application
