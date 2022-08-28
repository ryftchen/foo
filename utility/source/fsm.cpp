#include "fsm.hpp"

void checkIfExceptedFSMState(const bool normalState)
{
    if (!normalState)
    {
        throw std::runtime_error("FSM's state is abnormal.");
    }
}
