//! @file fsm.hpp
//! @author ryftchen
//! @brief The declarations (fsm) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include <atomic>
#include <stdexcept>

//! @brief The utility module.
namespace utility // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Finite-state-machine-related functions in the utility module.
namespace fsm
{
extern const char* version() noexcept;

//! @brief Alias for invoke result type.
//! @tparam Func - type of callable function
//! @tparam Args - type of function arguments
template <typename Func, typename... Args>
using InvokeResultType = std::invoke_result_t<Func, Args...>;
//! @brief Invoke callable.
//! @tparam Func - type of callable function
//! @tparam Args - type of function arguments
//! @param func - callable function
//! @param args - function arguments
//! @return result from calls
template <typename Func, typename... Args>
inline InvokeResultType<Func, Args...> invokeCallable(Func&& func, Args&&... args)
{
    return func(args...);
}
//! @brief Invoke callable. Multiple objects.
//! @tparam Ret - type of return value
//! @tparam T1 - type of class to which the function belongs
//! @tparam T2 - type of object to which the function belongs
//! @tparam Args - type of function arguments
//! @param func - callable function
//! @param obj - object to which the function belongs
//! @param args - function arguments
//! @return result from calls
template <typename Ret, typename T1, typename T2, typename... Args>
inline InvokeResultType<Ret T1::*, T2, Args...> invokeCallable(Ret T1::*func, T2&& obj, Args&&... args)
{
    return (obj.*func)(args...);
}

//! @brief Flexible invoke helper.
//! @tparam Func - type of callable function
//! @tparam Arg1 - type of function arguments
//! @tparam Arg2 - type of function arguments
//! @tparam isInvocable0 - flag to indicate the value when exclude both Arg1 and Arg2
//! @tparam isInvocable1 - flag to indicate the value when include only Arg1
//! @tparam isInvocable2 - flag to indicate the value when include only Arg2
//! @tparam isInvocable12 - flag to indicate the value when include both Arg1 and Arg2
template <
    typename Func,
    typename Arg1,
    typename Arg2,
    bool isInvocable0 = std::is_invocable_v<Func>,
    bool isInvocable1 = std::is_invocable_v<Func, Arg1>,
    bool isInvocable2 = std::is_invocable_v<Func, Arg2>,
    bool isInvocable12 = std::is_invocable_v<Func, Arg1, Arg2>>
struct FlexInvokeHelper;
//! @brief Flexible invoke helper. Exclude both Arg1 and Arg2.
//! @tparam Func - type of callable function
//! @tparam Arg1 - type of function arguments
//! @tparam Arg2 - type of function arguments
template <typename Func, typename Arg1, typename Arg2>
struct FlexInvokeHelper<Func, Arg1, Arg2, true, false, false, false>
{
    //! @brief Alias for return type.
    using ReturnType = InvokeResultType<Func>;
    //! @brief Invoke operation.
    //! @param func - callable function
    //! @return invoke result
    static inline ReturnType invoke(Func&& func, Arg1&& /*arg1*/, Arg2&& /*arg2*/) { return invokeCallable(func); }
};
//! @brief Flexible invoke helper. Include only Arg1.
//! @tparam Func - type of callable function
//! @tparam Arg1 - type of function arguments
//! @tparam Arg2 - type of function arguments
template <typename Func, typename Arg1, typename Arg2>
struct FlexInvokeHelper<Func, Arg1, Arg2, false, true, false, false>
{
    //! @brief Alias for return type.
    using ReturnType = InvokeResultType<Func, Arg1>;
    //! @brief Invoke operation.
    //! @param func - callable function
    //! @param arg1 - function argument
    //! @return invoke result
    static inline ReturnType invoke(Func&& func, Arg1&& arg1, Arg2&& /*arg2*/) { return invokeCallable(func, arg1); }
};
//! @brief Flexible invoke helper. Include only Arg2.
//! @tparam Func - type of callable function
//! @tparam Arg1 - type of function arguments
//! @tparam Arg2 - type of function arguments
template <typename Func, typename Arg1, typename Arg2>
struct FlexInvokeHelper<Func, Arg1, Arg2, false, false, true, false>
{
    //! @brief Alias for return type.
    using ReturnType = InvokeResultType<Func, Arg2>;
    //! @brief Invoke operation.
    //! @param func - callable function
    //! @param arg2 - function arguments
    //! @return invoke result
    static inline ReturnType invoke(Func&& func, Arg1&& /*arg1*/, Arg2&& arg2) { return invokeCallable(func, arg2); }
};
//! @brief Flexible invoke helper. Include both Arg1 and Arg2.
//! @tparam Func - type of callable function
//! @tparam Arg1 - type of function arguments
//! @tparam Arg2 - type of function arguments
template <typename Func, typename Arg1, typename Arg2>
struct FlexInvokeHelper<Func, Arg1, Arg2, false, false, false, true>
{
    //! @brief Alias for return type.
    using ReturnType = InvokeResultType<Func, Arg1, Arg2>;
    //! @brief Invoke operation.
    //! @param func - callable function
    //! @param arg1 - function arguments
    //! @param arg2 - function arguments
    //! @return invoke result
    static inline ReturnType invoke(Func&& func, Arg1&& arg1, Arg2&& arg2) { return invokeCallable(func, arg1, arg2); }
};

//! @brief Alias for adaptive invoke result type.
//! @tparam Func - type of callable function
//! @tparam Arg1 - type of function arguments
//! @tparam Arg2 - type of function arguments
template <typename Func, typename Arg1, typename Arg2>
using AdaptInvokeResultType = typename FlexInvokeHelper<Func, Arg1, Arg2>::ReturnType;
//! @brief Adaptive invoke.
//! @tparam Func - type of callable function
//! @tparam Arg1 - type of function arguments
//! @tparam Arg2 - type of function arguments
//! @param func - callable function
//! @param arg1 - function arguments
//! @param arg2 - function arguments
//! @return result from calls
template <typename Func, typename Arg1, typename Arg2>
inline AdaptInvokeResultType<Func, Arg1, Arg2> adaptiveInvoke(Func&& func, Arg1&& arg1, Arg2&& arg2)
{
    return FlexInvokeHelper<Func, Arg1, Arg2>::invoke(
        std::forward<Func>(func), std::forward<Arg1>(arg1), std::forward<Arg2>(arg2));
}

//! @brief The list of behaviors.
//! @tparam Types - type of behaviors
template <typename... Types>
struct List
{
};

//! @brief Associate behaviors.
//! @tparam Types - type of behaviors
template <typename... Types>
struct Concat;
//! @brief Associate events with behaviors.
//! @tparam T - type of triggered event
//! @tparam Types - type of behaviors
template <typename T, typename... Types>
struct Concat<T, List<Types...>>
{
    //! @brief Alias for list.
    using Type = List<T, Types...>;
};
//! @brief Associate.
template <>
struct Concat<>
{
    //! @brief Alias for list.
    using Type = List<>;
};

//! @brief The filter of events and behaviors.
//! @tparam Predicate - type of predicate
//! @tparam Types - type of behaviors
template <template <typename> class Predicate, typename... Types>
struct Filter;
//! @brief The filter of events and behaviors. Based on conditions.
//! @tparam Predicate - type of predicate
//! @tparam T - specific type for predicate
//! @tparam Types - type of behaviors
template <template <typename> class Predicate, typename T, typename... Types>
struct Filter<Predicate, T, Types...>
{
    //! @brief Alias for concat or filter.
    using Type = typename std::conditional<
        Predicate<T>::value,
        typename Concat<T, typename Filter<Predicate, Types...>::Type>::Type,
        typename Filter<Predicate, Types...>::Type>::type;
};
//! @brief The filter of behaviors.
//! @tparam Predicate - type of predicate
template <template <typename> class Predicate>
struct Filter<Predicate>
{
    //! @brief Alias for list.
    using Type = List<>;
};

//! @brief Finite state machine.
//! @tparam Derived - type of derived class
//! @tparam State - type of state
template <typename Derived, typename State = int>
class FSM
{
public:
    //! @brief Alias for state.
    using StateType = State;
    //! @brief Construct a new FSM object.
    //! @param initState - initialization value of state
    explicit FSM(const State initState = {}) : state{initState} {}

    //! @brief Process the specific event.
    //! @tparam Event - type of triggered event
    //! @param event - event to be processed
    template <typename Event>
    inline void processEvent(const Event& event);
    //! @brief Get current state.
    //! @return State current state
    inline State currentState() const;

private:
    //! @brief The base row of the transition table.
    //! @tparam Source - source state
    //! @tparam Event - type of triggered event
    //! @tparam Target - target state
    template <State Source, typename Event, State Target>
    struct RowBase
    {
    public:
        //! @brief Alias for state.
        using StateType = State;
        //! @brief Alias for event.
        using EventType = Event;
        //! @brief Get source state.
        //! @return source state
        static inline constexpr StateType sourceValue() { return Source; }
        //! @brief Get target state.
        //! @return target state
        static inline constexpr StateType targetValue() { return Target; }

    protected:
        //! @brief Process the specific event.
        //! @tparam Action - type of action function
        //! @param action - action function
        //! @param self - derived object
        //! @param event - event to be processed
        template <typename Action>
        static inline void processEvent(Action&& action, Derived& self, const Event& event)
        {
            adaptiveInvoke(action, self, event);
        }
        //! @brief Process the specific event by default.
        static inline constexpr void processEvent(
            const std::nullptr_t /*null*/, Derived& /*self*/, const Event& /*event*/)
        {
        }

        //! @brief Check guard condition.
        //! @tparam Guard - type of guard condition
        //! @param guard - guard condition
        //! @param self - derived object
        //! @param event - event to be processed
        //! @return pass or not pass
        template <typename Guard>
        static inline bool checkGuard(Guard&& guard, const Derived& self, const Event& event)
        {
            return adaptiveInvoke(guard, self, event);
        }
        //! @brief Check guard condition by default.
        //! @return pass or not pass
        static inline constexpr bool checkGuard(
            const std::nullptr_t /*null*/, const Derived& /*self*/, const Event& /*event*/)
        {
            return true;
        }
    };

    //! @brief Classification by event type.
    //! @tparam Event - type of triggered event
    //! @tparam Types - type of behaviors
    template <typename Event, typename... Types>
    struct ByEventType;
    //! @brief Classification by event type. Include both event and behaviors.
    //! @tparam Event - type of triggered event
    //! @tparam Types - type of behaviors
    template <typename Event, typename... Types>
    struct ByEventType<Event, List<Types...>>
    {
        //! @brief Alias for predicate.
        //! @tparam T - type of class to which the struct belongs
        template <typename T>
        using Predicate = std::is_same<typename T::EventType, Event>;
        //! @brief Alias for filter type.
        using Type = typename Filter<Predicate, Types...>::Type;
    };
    //! @brief Classification by event type. Include only event.
    //! @tparam Event - type of triggered event
    template <typename Event>
    struct ByEventType<Event, List<>>
    {
        //! @brief Alias for list.
        using Type = List<>;
    };

    //! @brief Handle the specific event.
    //! @tparam Event - type of triggered event
    //! @tparam Types - type of behaviors
    template <typename Event, typename... Types>
    struct handleEvent;
    //! @brief Handle the specific event. Include both event and behaviors.
    //! @tparam Event - type of triggered event
    //! @tparam T - type of derived class
    //! @tparam Types - type of behaviors
    template <typename Event, typename T, typename... Types>
    struct handleEvent<Event, List<T, Types...>>
    {
        //! @brief Execute handling.
        //! @param self - derived object
        //! @param event - event to be processed
        //! @param state - source state
        //! @return state after execute
        static inline State execute(Derived& self, const Event& event, const State state)
        {
            return ((T::sourceValue() == state) && T::checkGuard(self, event))
                ? (T::processEvent(self, event), T::targetValue())
                : handleEvent<Event, List<Types...>>::execute(self, event, state);
        }
    };
    //! @brief Handle the specific event. Include only event.
    //! @tparam Event - type of triggered event
    template <typename Event>
    struct handleEvent<Event, List<>>
    {
        //! @brief Execute handling. No transition.
        //! @param self - derived object
        //! @param event - event to be processed
        //! @return state after execute
        static inline State execute(Derived& self, const Event& event, const State /*state*/)
        {
            return self.noTransition(event);
        }
    };

    //! @brief Lock of FSM procedure.
    class ProcedureLock
    {
    public:
        //! @brief Construct a new ProcedureLock object.
        //! @param fsm - FSM object
        explicit ProcedureLock(FSM& fsm) : isProcessing{fsm.isProcessing}
        {
            if (isProcessing.load())
            {
                throw std::logic_error{"Call process event recursively."};
            }
            isProcessing.store(true);
        }
        //! @brief Construct a new ProcedureLock object.
        ProcedureLock() = delete;
        //! @brief Destroy the ProcedureLock object.
        virtual ~ProcedureLock() { isProcessing.store(false); }

    private:
        //! @brief Flag to indicate whether the FSM is processing.
        std::atomic<bool>& isProcessing;
    };

    //! @brief FSM state.
    State state{};
    //! @brief Flag to indicate whether the FSM is processing.
    std::atomic<bool> isProcessing{false};

protected:
    //! @brief Alias for transition table.
    //! @tparam Rows - type of row-based
    template <typename... Rows>
    using Table = List<Rows...>;
    //! @brief No transition can be found for the given event in its current state.
    //! @tparam Event - type of triggered event
    //! @param event - event to be processed
    //! @return current state
    template <typename Event>
    inline State noTransition(const Event& event);

    //! @brief Default row of the transition table.
    //! @tparam Source - source state
    //! @tparam Event - type of triggered event
    //! @tparam Target - target state
    //! @tparam Action - type of action function
    //! @tparam action - action function
    //! @tparam Guard - type of guard condition
    //! @tparam guard - guard condition
    template <
        State Source,
        typename Event,
        State Target,
        typename Action = std::nullptr_t,
        Action action = nullptr,
        typename Guard = std::nullptr_t,
        Guard guard = nullptr>
    struct DefRow : public RowBase<Source, Event, Target>
    {
        //! @brief Process the specific event.
        //! @param self - derived object
        //! @param event - event to be processed
        static inline void processEvent(Derived& self, const Event& event)
        {
            RowBase<Source, Event, Target>::processEvent(action, self, event);
        }
        //! @brief Check guard condition.
        //! @param self - derived object
        //! @param event - event to be processed
        //! @return pass or not pass
        static inline bool checkGuard(const Derived& self, const Event& event)
        {
            return RowBase<Source, Event, Target>::checkGuard(guard, self, event);
        }
    };

    //! @brief Member function row of the transition table.
    //! @tparam Source - source state
    //! @tparam Event - type of triggered event
    //! @tparam Target - target state
    //! @tparam action - action function
    //! @tparam guard - guard condition
    template <
        State Source,
        typename Event,
        State Target,
        void (Derived::*action)(const Event&) = nullptr,
        bool (Derived::*guard)(const Event&) const = nullptr>
    struct MemFuncRow : public RowBase<Source, Event, Target>
    {
        //! @brief Process the specific event.
        //! @param self - derived object
        //! @param event - event to be processed
        static inline void processEvent(Derived& self, const Event& event)
        {
            if (nullptr != action)
            {
                RowBase<Source, Event, Target>::processEvent(action, self, event);
            }
        }
        //! @brief Check guard condition.
        //! @param self - derived object
        //! @param event - event to be processed
        //! @return pass or not pass
        static inline bool checkGuard(const Derived& self, const Event& event)
        {
            if (nullptr == guard)
            {
                return true;
            }
            RowBase<Source, Event, Target>::checkGuard(guard, self, event);
        }
    };

    //! @brief The generic row of the transition table.
    //! @tparam Source - source state
    //! @tparam Event - type of triggered event
    //! @tparam Target - target state
    //! @tparam action - action function
    //! @tparam guard - guard condition
    template <State Source, typename Event, State Target, auto action = nullptr, auto guard = nullptr>
    struct Row : public RowBase<Source, Event, Target>
    {
        //! @brief Process the specific event.
        //! @param self - derived object
        //! @param event - event to be processed
        static inline void processEvent(Derived& self, const Event& event)
        {
            RowBase<Source, Event, Target>::processEvent(action, self, event);
        }
        //! @brief Check guard condition.
        //! @param self - derived object
        //! @param event - event to be processed
        //! @return pass or not pass
        static inline bool checkGuard(const Derived& self, const Event& event)
        {
            return RowBase<Source, Event, Target>::checkGuard(guard, self, event);
        }
    };
};

template <typename Derived, typename State>
template <typename Event>
inline void FSM<Derived, State>::processEvent(const Event& event)
{
    using Rows = typename ByEventType<Event, typename Derived::TransitionTable>::Type;
    ProcedureLock lock(*this);
    static_assert(std::is_base_of_v<FSM, Derived>);
    Derived& self = static_cast<Derived&>(*this);
    state = handleEvent<Event, Rows>::execute(self, event, state);
}

template <typename Derived, typename State>
inline State FSM<Derived, State>::currentState() const
{
    return state;
}

template <typename Derived, typename State>
template <typename Event>
inline State FSM<Derived, State>::noTransition(const Event& /*event*/)
{
    return state;
}
} // namespace fsm
} // namespace utility
