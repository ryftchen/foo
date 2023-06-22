//! @file creational.cpp
//! @author ryftchen
//! @brief The definitions (creational) in the data structure module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023

#include "creational.hpp"

namespace design_pattern::creational
{
//! @brief The abstract factory pattern.
namespace abstract_factory
{
//! @brief Output stream for the abstract factory pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}
} // namespace abstract_factory

//! @brief The builder pattern.
namespace builder
{
//! @brief Output stream for the builder pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}
} // namespace builder

//! @brief The factory method pattern.
namespace factory_method
{
//! @brief Output stream for the factory method pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}
} // namespace factory_method

//! @brief The prototype pattern.
namespace prototype
{
//! @brief Output stream for the prototype pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

std::unique_ptr<Prototype> Client::types[2];
int Client::nTypes = 2;
} // namespace prototype

//! @brief The singleton factory pattern.
namespace singleton
{
//! @brief Output stream for the singleton pattern. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

std::shared_ptr<Singleton> Singleton::instance = nullptr;
} // namespace singleton
} // namespace design_pattern::creational
