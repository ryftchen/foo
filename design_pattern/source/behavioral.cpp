//! @file behavioral.cpp
//! @author ryftchen
//! @brief The definitions (behavioral) in the data structure module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023

#include "behavioral.hpp"

namespace design_pattern::behavioral
{
//! @brief The chain of responsibility pattern.
namespace chain_of_responsibility
{
//! @brief Output stream for the chain of responsibility pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}
} // namespace chain_of_responsibility

//! @brief The command pattern.
namespace command
{
//! @brief Output stream for the command pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}
} // namespace command

//! @brief The interpreter pattern.
namespace interpreter
{
//! @brief Output stream for the interpreter pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}
} // namespace interpreter

//! @brief The iterator pattern.
namespace iterator
{
//! @brief Output stream for the iterator pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}
} // namespace iterator

//! @brief The mediator pattern.
namespace mediator
{
//! @brief Output stream for the mediator pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}
} // namespace mediator

//! @brief The memento pattern.
namespace memento
{
//! @brief Output stream for the memento pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}
} // namespace memento

//! @brief The observer pattern.
namespace observer
{
//! @brief Output stream for the observer pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}
} // namespace observer

//! @brief The state pattern.
namespace state
{
//! @brief Output stream for the state pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}
} // namespace state

//! @brief The strategy pattern.
namespace strategy
{
//! @brief Output stream for the strategy pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}
} // namespace strategy

//! @brief The template method pattern.
namespace template_method
{
//! @brief Output stream for the template method pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}
} // namespace template_method

//! @brief The visitor pattern.
namespace visitor
{
//! @brief Output stream for the visitor pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}
} // namespace visitor
} // namespace design_pattern::behavioral
