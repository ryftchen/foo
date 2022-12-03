#pragma once

#include "fsm.hpp"

namespace utility::fsm
{
template <class Derived, class State>
FSM<Derived, State>::FSM(State initState) : state(initState)
{
}

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
inline State FSM<Derived, State>::noTransition(const Event& /*unused*/)
{
    return state;
}
} // namespace utility::fsm
