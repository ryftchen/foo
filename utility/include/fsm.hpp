#pragma once

#include <atomic>
#include <iostream>

namespace util_fsm
{
template <class Func, class... Args>
using InvokeResult = std::invoke_result_t<Func, Args...>;

template <class Func, class... Args>
using IsInvocable = std::is_invocable<Func, Args...>;

template <class Func, class... Args>
InvokeResult<Func, Args...> invokeResult(Func&& func, Args&&... args)
{
    return func(args...);
}

template <class Mem, class T1, class T2, class... Args>
InvokeResult<Mem T1::*, T2, Args...> invokeResult(Mem T1::*func, T2&& obj, Args&&... args)
{
    return (obj.*func)(args...);
}

template <
    class Func,
    class Arg1,
    class Arg2,
    bool val1 = IsInvocable<Func>::value,
    bool val2 = IsInvocable<Func, Arg1>::value,
    bool val3 = IsInvocable<Func, Arg2>::value,
    bool val4 = IsInvocable<Func, Arg1, Arg2>::value>
struct BinaryFuncHelper;

template <class Func, class Arg1, class Arg2>
struct BinaryFuncHelper<Func, Arg1, Arg2, true, false, false, false>
{
    using ResultType = InvokeResult<Func>;
    static ResultType invoke(Func&& func, Arg1&& /*unused*/, Arg2&& /*unused*/) { return invokeResult(func); }
};

template <class Func, class Arg1, class Arg2>
struct BinaryFuncHelper<Func, Arg1, Arg2, false, true, false, false>
{
    using ResultType = InvokeResult<Func, Arg1>;
    static ResultType invoke(Func&& func, Arg1&& arg1, Arg2&& /*unused*/) { return invokeResult(func, arg1); }
};

template <class Func, class Arg1, class Arg2>
struct BinaryFuncHelper<Func, Arg1, Arg2, false, false, true, false>
{
    using ResultType = InvokeResult<Func, Arg2>;
    static ResultType invoke(Func&& func, Arg1&& /*unused*/, Arg2&& arg2) { return invokeResult(func, arg2); }
};

template <class Func, class Arg1, class Arg2>
struct BinaryFuncHelper<Func, Arg1, Arg2, false, false, false, true>
{
    using ResultType = InvokeResult<Func, Arg1, Arg2>;
    static ResultType invoke(Func&& func, Arg1&& arg1, Arg2&& arg2) { return invokeResult(func, arg1, arg2); }
};

template <class Func, class Arg1, class Arg2>
using InvokeAsBinaryFuncResult = typename BinaryFuncHelper<Func, Arg1, Arg2>::ResultType;

template <class Func, class Arg1, class Arg2>
InvokeAsBinaryFuncResult<Func, Arg1, Arg2> invokeAsBinaryFunc(Func&& func, Arg1&& arg1, Arg2&& arg2)
{
    return BinaryFuncHelper<Func, Arg1, Arg2>::invoke(
        std::forward<Func>(func), std::forward<Arg1>(arg1), std::forward<Arg2>(arg2));
}

template <class...>
struct List
{
};

template <class...>
struct Concat;

template <class T, class... Types>
struct Concat<T, List<Types...>>
{
    using Type = List<T, Types...>;
};

template <>
struct Concat<>
{
    using Type = List<>;
};

template <template <typename> class Predicate, class...>
struct filter;

template <template <typename> class Predicate, class T, class... Types>
struct filter<Predicate, T, Types...>
{
    using Type = typename std::conditional<
        Predicate<T>::value,
        typename Concat<T, typename filter<Predicate, Types...>::Type>::Type,
        typename filter<Predicate, Types...>::Type>::type;
};

template <template <typename> class Predicate>
struct filter<Predicate>
{
    using Type = List<>;
};

template <class Derived, class State = int>
class FSM
{
public:
    using StateType = State;
    explicit FSM(State initState = State()) : state(initState){};

    template <class Event>
    void processEvent(const Event& event);
    State currentState() const;

private:
    template <State Source, class Event, State Target>
    struct RowBase
    {
        using StateType = State;
        using EventType = Event;

        static constexpr StateType sourceValue() { return Source; }
        static constexpr StateType targetValue() { return Target; }

    protected:
        template <class Action>
        static void processEvent(Action&& action, Derived& self, const Event& event)
        {
            invokeAsBinaryFunc(action, self, event);
        }
        static constexpr void processEvent(std::nullptr_t, Derived& /*unused*/, const Event& /*unused*/) {}

        template <class Guard>
        static bool checkGuard(Guard&& guard, const Derived& self, const Event& event)
        {
            return invokeAsBinaryFunc(guard, self, event);
        }
        static constexpr bool checkGuard(std::nullptr_t, const Derived& /*unused*/, const Event& /*unused*/)
        {
            return true;
        }
    };

    template <class Event, class...>
    struct ByEventType;

    template <class Event, class... Types>
    struct ByEventType<Event, List<Types...>>
    {
        template <class T>
        using Predicate = std::is_same<typename T::EventType, Event>;
        using Type = typename filter<Predicate, Types...>::Type;
    };

    template <class Event>
    struct ByEventType<Event, List<>>
    {
        using Type = List<>;
    };

    template <class Event, class...>
    struct handleEvent;

    template <class Event, class T, class... Types>
    struct handleEvent<Event, List<T, Types...>>
    {
        static State execute(Derived& self, const Event& event, State state)
        {
            return (
                ((T::sourceValue() == state) && T::checkGuard(self, event))
                    ? (T::processEvent(self, event), T::targetValue())
                    : handleEvent<Event, List<Types...>>::execute(self, event, state));
        }
    };

    template <class Event>
    struct handleEvent<Event, List<>>
    {
        static State execute(Derived& self, const Event& event, State /*unused*/) { return self.noTransition(event); }
    };

    class ProcessingLock
    {
    public:
        explicit ProcessingLock(FSM& fsm);
        ~ProcessingLock();

    private:
        std::atomic<bool>& isProcessing;
    };

    State state;
    std::atomic<bool> isProcessing{false};

protected:
    template <class... Rows>
    using Map = List<Rows...>;

    template <class Event>
    State noTransition(const Event& /*unused*/);

    template <
        State Source,
        class Event,
        State Target,
        class Action = std::nullptr_t,
        Action action = nullptr,
        class Guard = std::nullptr_t,
        Guard guard = nullptr>
    struct BasicRow : public RowBase<Source, Event, Target>
    {
        static void processEvent(Derived& self, const Event& event)
        {
            RowBase<Source, Event, Target>::processEvent(action, self, event);
        }
        static bool checkGuard(const Derived& self, const Event& event)
        {
            return RowBase<Source, Event, Target>::checkGuard(guard, self, event);
        }
    };

    template <
        State Source,
        class Event,
        State Target,
        void (Derived::*action)(const Event&) = nullptr,
        bool (Derived::*guard)(const Event&) const = nullptr>
    struct MemFuncRow : public RowBase<Source, Event, Target>
    {
        static void processEvent(Derived& self, const Event& event)
        {
            if (nullptr != action)
            {
                RowBase<Source, Event, Target>::processEvent(action, self, event);
            }
        }
        static bool checkGuard(const Derived& self, const Event& event)
        {
            if (nullptr == guard)
            {
                return true;
            }
            RowBase<Source, Event, Target>::checkGuard(guard, self, event);
        }
    };

    template <State Source, class Event, State Target, auto action = nullptr, auto guard = nullptr>
    struct Row : public RowBase<Source, Event, Target>
    {
        static void processEvent(Derived& self, const Event& event)
        {
            RowBase<Source, Event, Target>::processEvent(action, self, event);
        }
        static bool checkGuard(const Derived& self, const Event& event)
        {
            return RowBase<Source, Event, Target>::checkGuard(guard, self, event);
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
State FSM<Derived, State>::currentState() const
{
    return state;
}

template <class Derived, class State>
template <class Event>
State FSM<Derived, State>::noTransition(const Event& /*unused*/)
{
    return state;
}

template <class Derived, class State>
FSM<Derived, State>::ProcessingLock::ProcessingLock(FSM& fsm) : isProcessing(fsm.isProcessing)
{
    try
    {
        if (isProcessing)
        {
            throw std::logic_error("fsm: Call processEvent recursively.");
        }
        isProcessing = true;
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << std::endl;
    }
}

template <class Derived, class State>
FSM<Derived, State>::ProcessingLock::~ProcessingLock()
{
    isProcessing = false;
}

extern void checkIfExceptedFSMState(const bool normalState);
} // namespace util_fsm
