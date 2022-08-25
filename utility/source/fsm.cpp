#include "fsm.hpp"

void checkIfExceptedFSMState(const bool normalStatus)
{
    if (!normalStatus)
    {
        throw std::runtime_error("FSM's state is abnormal.");
    }
}