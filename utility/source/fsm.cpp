#include "fsm.hpp"
#include <sstream>

namespace util_fsm
{
void checkIfExceptedFSMState(const int currentState, const int exceptedState)
{
    if (currentState != exceptedState)
    {
        std::ostringstream stream;
        stream << "fsm: Abnormal status, current: " << currentState << ", expected: " << exceptedState << ".";
        throw std::runtime_error(stream.str());
    }
}
} // namespace util_fsm
