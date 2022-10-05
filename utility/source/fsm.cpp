#include "fsm.hpp"

namespace util_fsm
{
void checkIfExceptedFSMState(const bool normalState)
{
    if (!normalState)
    {
        throw std::runtime_error("fsm: State is abnormal.");
    }
}
} // namespace util_fsm
