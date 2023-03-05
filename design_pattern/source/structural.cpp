//! @file structural.cpp
//! @author ryftchen
//! @brief The definitions (structural) in the data structure module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#include "structural.hpp"

namespace design_pattern::structural
{
//! @brief The adapter pattern.
namespace adapter
{
//! @brief Output stream for the adapter pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}
} // namespace adapter

//! @brief The bridge pattern.
namespace bridge
{
//! @brief Output stream for the bridge pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}
} // namespace bridge

//! @brief The composite pattern.
namespace composite
{
//! @brief Output stream for the composite pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}
} // namespace composite

//! @brief The decorator pattern.
namespace decorator
{
//! @brief Output stream for the decorator pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}
} // namespace decorator

//! @brief The facade pattern.
namespace facade
{
//! @brief Output stream for the facade pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}
} // namespace facade

//! @brief The flyweight pattern.
namespace flyweight
{
//! @brief Output stream for the flyweight pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}
} // namespace flyweight

//! @brief The proxy pattern.
namespace proxy
{
//! @brief Output stream for the proxy pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}
} // namespace proxy
} // namespace design_pattern::structural