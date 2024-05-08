//! @file fsm.hpp
//! @author ryftchen
//! @brief The declarations (fsm) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#pragma once

#include <atomic>

//! @brief The utility module.
namespace utility // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Finite-state-machine-related functions in the utility module.
namespace fsm
{
extern const char* version();

//! @brief Alias for invoke result.
//! @tparam Func - type of callable function
//! @tparam Args - type of function arguments
template <class Func, class... Args>
using InvokeResult = std::invoke_result_t<Func, Args...>;

//! @brief Alias for checking whether to be invocable.
//! @tparam Func - type of callable function
//! @tparam Args - type of function arguments
template <class Func, class... Args>
using IsInvocable = std::is_invocable<Func, Args...>;

//! @brief Invoke result.
//! @tparam Func - type of callable function
//! @tparam Args - type of function arguments
//! @param func - callable function
//! @param args - function arguments
//! @return wrapping of calls
template <class Func, class... Args>
InvokeResult<Func, Args...> invokeResult(Func&& func, Args&&... args)
{
    return func(args...);
}

//! @brief Invoke result. Multiple objects.
//! @tparam Ret - type of return value
//! @tparam T1 - type of class to which the function belongs
//! @tparam T2 - type of object to which the function belongs
//! @tparam Args - type of function arguments
//! @param func - callable function
//! @param obj - object to which the function belongs
//! @param args - function arguments
//! @return wrapping of calls
template <class Ret, class T1, class T2, class... Args>
InvokeResult<Ret T1::*, T2, Args...> invokeResult(Ret T1::*func, T2&& obj, Args&&... args)
{
    return (obj.*func)(args...);
}

//! @brief Binary function helper.
//! @tparam Func - type of callable function
//! @tparam Arg1 - type of function arguments
//! @tparam Arg2 - type of function arguments
//! @tparam val1 - flag to indicate the value when exclude both Arg1 and Arg2
//! @tparam val2 - flag to indicate the value when include only Arg1
//! @tparam val3 - flag to indicate the value when include only Arg2
//! @tparam val4 - flag to indicate the value when include both Arg1 and Arg2
template <
    class Func,
    class Arg1,
    class Arg2,
    bool val1 = IsInvocable<Func>::value,
    bool val2 = IsInvocable<Func, Arg1>::value,
    bool val3 = IsInvocable<Func, Arg2>::value,
    bool val4 = IsInvocable<Func, Arg1, Arg2>::value>
struct BinaryFuncHelper;

//! @brief Binary function helper. Exclude both Arg1 and Arg2.
//! @tparam Func - type of callable function
//! @tparam Arg1 - type of function arguments
//! @tparam Arg2 - type of function arguments
template <class Func, class Arg1, class Arg2>
struct BinaryFuncHelper<Func, Arg1, Arg2, true, false, false, false>
{
    //! @brief Alias for invoke result.
    using ResultType = InvokeResult<Func>;
    //! @brief Invoke operation.
    //! @param func - callable function
    //! @return invoke result
    static ResultType invoke(Func&& func, Arg1&& /*arg1*/, Arg2&& /*arg2*/) { return invokeResult(func); }
};

//! @brief Binary function helper. Include only Arg1.
//! @tparam Func - type of callable function
//! @tparam Arg1 - type of function arguments
//! @tparam Arg2 - type of function arguments
template <class Func, class Arg1, class Arg2>
struct BinaryFuncHelper<Func, Arg1, Arg2, false, true, false, false>
{
    //! @brief Alias for invoke result.
    using ResultType = InvokeResult<Func, Arg1>;
    //! @brief Invoke operation.
    //! @param func - callable function
    //! @param arg1 - function argument
    //! @return invoke result
    static ResultType invoke(Func&& func, Arg1&& arg1, Arg2&& /*arg2*/) { return invokeResult(func, arg1); }
};

//! @brief Binary function helper. Include only Arg2.
//! @tparam Func - type of callable function
//! @tparam Arg1 - type of function arguments
//! @tparam Arg2 - type of function arguments
template <class Func, class Arg1, class Arg2>
struct BinaryFuncHelper<Func, Arg1, Arg2, false, false, true, false>
{
    //! @brief Alias for invoke result.
    using ResultType = InvokeResult<Func, Arg2>;
    //! @brief Invoke operation.
    //! @param func - callable function
    //! @param arg2 - function arguments
    //! @return invoke result
    static ResultType invoke(Func&& func, Arg1&& /*arg1*/, Arg2&& arg2) { return invokeResult(func, arg2); }
};

//! @brief Binary function helper. Include both Arg1 and Arg2.
//! @tparam Func - type of callable function
//! @tparam Arg1 - type of function arguments
//! @tparam Arg2 - type of function arguments
template <class Func, class Arg1, class Arg2>
struct BinaryFuncHelper<Func, Arg1, Arg2, false, false, false, true>
{
    //! @brief Alias for invoke result.
    using ResultType = InvokeResult<Func, Arg1, Arg2>;
    //! @brief Invoke operation.
    //! @param func - callable function
    //! @param arg1 - function arguments
    //! @param arg2 - function arguments
    //! @return invoke result
    static ResultType invoke(Func&& func, Arg1&& arg1, Arg2&& arg2) { return invokeResult(func, arg1, arg2); }
};

//! @brief Alias for invoke result of binary function.
//! @tparam Func - type of callable function
//! @tparam Arg1 - type of function arguments
//! @tparam Arg2 - type of function arguments
template <class Func, class Arg1, class Arg2>
using InvokeAsBinaryFuncResult = typename BinaryFuncHelper<Func, Arg1, Arg2>::ResultType;

//! @brief Invoke as binary function.
//! @tparam Func - type of callable function
//! @tparam Arg1 - type of function arguments
//! @tparam Arg2 - type of function arguments
//! @param func - callable function
//! @param arg1 - function arguments
//! @param arg2 - function arguments
//! @return invoke result of binary function
template <class Func, class Arg1, class Arg2>
InvokeAsBinaryFuncResult<Func, Arg1, Arg2> invokeAsBinaryFunc(Func&& func, Arg1&& arg1, Arg2&& arg2)
{
    return BinaryFuncHelper<Func, Arg1, Arg2>::invoke(
        std::forward<Func>(func), std::forward<Arg1>(arg1), std::forward<Arg2>(arg2));
}

//! @brief The list of behaviors.
//! @tparam Types - type of behaviors
template <class... Types>
struct List
{
};

//! @brief Associate behaviors.
//! @tparam Types - type of behaviors
template <class... Types>
struct Concat;

//! @brief Associate events with behaviors.
//! @tparam T - type of triggered event
//! @tparam Types - type of behaviors
template <class T, class... Types>
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
template <template <typename> class Predicate, class... Types>
struct Filter;

//! @brief The filter of events and behaviors. Based on conditions.
//! @tparam Predicate - type of predicate
//! @tparam T - specific type for predicate
//! @tparam Types - type of behaviors
template <template <typename> class Predicate, class T, class... Types>
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
template <class Derived, class State = int>
class FSM
{
public:
    //! @brief Alias for state.
    using StateType = State;
    //! @brief Construct a new FSM object.
    //! @param initState - initialization value of state
    explicit FSM(State initState = State()) : state(initState) {}

    //! @brief Process the specific event.
    //! @tparam Event - type of triggered event
    //! @param event - event to be processed
    template <class Event>
    void processEvent(const Event& event);
    //! @brief Get current state.
    //! @return State current state
    inline State currentState() const;

private:
    //! @brief Row-based.
    //! @tparam Source - source state
    //! @tparam Event - type of triggered event
    //! @tparam Target - target state
    template <State Source, class Event, State Target>
    struct RowBased
    {
        //! @brief Alias for state.
        using StateType = State;
        //! @brief Alias for event.
        using EventType = Event;

        //! @brief Get source state.
        //! @return source state
        static constexpr StateType sourceValue() { return Source; }
        //! @brief Get target state.
        //! @return target state
        static constexpr StateType targetValue() { return Target; }

    protected:
        //! @brief Process the specific event.
        //! @tparam Action - type of action function
        //! @param action - action function
        //! @param self - derived object
        //! @param event - event to be processed
        template <class Action>
        static void processEvent(Action&& action, Derived& self, const Event& event)
        {
            invokeAsBinaryFunc(action, self, event);
        }
        //! @brief Process the specific event by default.
        static constexpr void processEvent(std::nullptr_t /*null*/, Derived& /*self*/, const Event& /*event*/) {}

        //! @brief Check guard condition.
        //! @tparam Guard - type of guard condition
        //! @param guard - guard condition
        //! @param self - derived object
        //! @param event - event to be processed
        //! @return pass or not pass
        template <class Guard>
        static bool checkGuard(Guard&& guard, const Derived& self, const Event& event)
        {
            return invokeAsBinaryFunc(guard, self, event);
        }
        //! @brief Check guard condition by default.
        //! @return pass or not pass
        static constexpr bool checkGuard(std::nullptr_t /*null*/, const Derived& /*self*/, const Event& /*event*/)
        {
            return true;
        }
    };

    //! @brief Classification by event type.
    //! @tparam Event - type of triggered event
    //! @tparam Types - type of behaviors
    template <class Event, class... Types>
    struct ByEventType;

    //! @brief Classification by event type. Include both event and behaviors.
    //! @tparam Event - type of triggered event
    //! @tparam Types - type of behaviors
    template <class Event, class... Types>
    struct ByEventType<Event, List<Types...>>
    {
        //! @brief Alias for predicate.
        //! @tparam T - type of class to which the struct belongs
        template <class T>
        using Predicate = std::is_same<typename T::EventType, Event>;
        //! @brief Alias for filter type.
        using Type = typename Filter<Predicate, Types...>::Type;
    };

    //! @brief Classification by event type. Include only event.
    //! @tparam Event - type of triggered event
    template <class Event>
    struct ByEventType<Event, List<>>
    {
        //! @brief Alias for list.
        using Type = List<>;
    };

    //! @brief Handle the specific event.
    //! @tparam Event - type of triggered event
    //! @tparam Types - type of behaviors
    template <class Event, class... Types>
    struct handleEvent;

    //! @brief Handle the specific event. Include both event and behaviors.
    //! @tparam Event - type of triggered event
    //! @tparam T - type of derived class
    //! @tparam Types - type of behaviors
    template <class Event, class T, class... Types>
    struct handleEvent<Event, List<T, Types...>>
    {
        //! @brief Execute handling.
        //! @param self - derived object
        //! @param event - event to be processed
        //! @param state - source state
        //! @return state after execute
        static State execute(Derived& self, const Event& event, State state)
        {
            return (
                ((T::sourceValue() == state) && T::checkGuard(self, event))
                    ? (T::processEvent(self, event), T::targetValue())
                    : handleEvent<Event, List<Types...>>::execute(self, event, state));
        }
    };

    //! @brief Handle the specific event. Include only event.
    //! @tparam Event - type of triggered event
    template <class Event>
    struct handleEvent<Event, List<>>
    {
        //! @brief Execute handling. No transition.
        //! @param self - derived object
        //! @param event - event to be processed
        //! @return state after execute
        static State execute(Derived& self, const Event& event, State /*state*/) { return self.noTransition(event); }
    };

    //! @brief Lock of FSM processing.
    class ProcessingLock
    {
    public:
        //! @brief Construct a new ProcessingLock object.
        //! @param fsm - FSM object
        explicit ProcessingLock(FSM& fsm) : isProcessing(fsm.isProcessing)
        {
            if (isProcessing.load())
            {
                throw std::logic_error("Call process event recursively.");
            }
            isProcessing.store(true);
        }
        //! @brief Destroy the ProcessingLock object.
        ~ProcessingLock() { isProcessing.store(false); }

    private:
        //! @brief Flag to indicate whether the FSM is processing.
        std::atomic<bool>& isProcessing;
    };

    //! @brief FSM state.
    State state;
    //! @brief Flag to indicate whether the FSM is processing.
    std::atomic<bool> isProcessing{false};

protected:
    //! @brief Alias for transition map.
    //! @tparam Rows - type of row-based
    template <class... Rows>
    using Map = List<Rows...>;

    //! @brief No transition can be found for the given event in its current state.
    //! @tparam Event - type of triggered event
    //! @param event - event to be processed
    //! @return current state
    template <class Event>
    inline State noTransition(const Event& event);

    //! @brief The basic row of the transition map.
    //! @tparam Source - source state
    //! @tparam Event - type of triggered event
    //! @tparam Target - target state
    //! @tparam Action - type of action function
    //! @tparam action - action function
    //! @tparam Guard - type of guard condition
    //! @tparam guard - guard condition
    template <
        State Source,
        class Event,
        State Target,
        class Action = std::nullptr_t,
        Action action = nullptr,
        class Guard = std::nullptr_t,
        Guard guard = nullptr>
    struct BasicRow : public RowBased<Source, Event, Target>
    {
        //! @brief Process the specific event.
        //! @param self - derived object
        //! @param event - event to be processed
        static void processEvent(Derived& self, const Event& event)
        {
            RowBased<Source, Event, Target>::processEvent(action, self, event);
        }
        //! @brief Check guard condition.
        //! @param self - derived object
        //! @param event - event to be processed
        //! @return pass or not pass
        static bool checkGuard(const Derived& self, const Event& event)
        {
            return RowBased<Source, Event, Target>::checkGuard(guard, self, event);
        }
    };

    //! @brief Member function row of the transition map.
    //! @tparam Source - source state
    //! @tparam Event - type of triggered event
    //! @tparam Target - target state
    //! @tparam action - action function
    //! @tparam guard - guard condition
    template <
        State Source,
        class Event,
        State Target,
        void (Derived::*action)(const Event&) = nullptr,
        bool (Derived::*guard)(const Event&) const = nullptr>
    struct MemFuncRow : public RowBased<Source, Event, Target>
    {
        //! @brief Process the specific event.
        //! @param self - derived object
        //! @param event - event to be processed
        static void processEvent(Derived& self, const Event& event)
        {
            if (nullptr != action)
            {
                RowBased<Source, Event, Target>::processEvent(action, self, event);
            }
        }
        //! @brief Check guard condition.
        //! @param self - derived object
        //! @param event - event to be processed
        //! @return pass or not pass
        static bool checkGuard(const Derived& self, const Event& event)
        {
            if (nullptr == guard)
            {
                return true;
            }
            RowBased<Source, Event, Target>::checkGuard(guard, self, event);
        }
    };

    //! @brief The generic row of the transition map.
    //! @tparam Source - source state
    //! @tparam Event - type of triggered event
    //! @tparam Target - target state
    //! @tparam action - action function
    //! @tparam guard - guard condition
    template <State Source, class Event, State Target, auto action = nullptr, auto guard = nullptr>
    struct Row : public RowBased<Source, Event, Target>
    {
        //! @brief Process the specific event.
        //! @param self - derived object
        //! @param event - event to be processed
        static void processEvent(Derived& self, const Event& event)
        {
            RowBased<Source, Event, Target>::processEvent(action, self, event);
        }
        //! @brief Check guard condition.
        //! @param self - derived object
        //! @param event - event to be processed
        //! @return pass or not pass
        static bool checkGuard(const Derived& self, const Event& event)
        {
            return RowBased<Source, Event, Target>::checkGuard(guard, self, event);
        }
    };
};

template <class Derived, class State>
template <class Event>
void FSM<Derived, State>::processEvent(const Event& event)
{
    using Rows = typename ByEventType<Event, typename Derived::TransitionMap>::Type;
    ProcessingLock procLock(*this);
    static_assert(std::is_base_of<FSM, Derived>::value);
    Derived& self = static_cast<Derived&>(*this);
    state = handleEvent<Event, Rows>::execute(self, event, state);
}

template <class Derived, class State>
inline State FSM<Derived, State>::currentState() const
{
    return state;
}

template <class Derived, class State>
template <class Event>
inline State FSM<Derived, State>::noTransition(const Event& /*event*/)
{
    return state;
}
} // namespace fsm
} // namespace utility
