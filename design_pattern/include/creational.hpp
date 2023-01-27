//! @file creational.hpp
//! @author ryftchen
//! @brief The declarations (creational) in the design pattern module.
//! @version 0.1
//! @copyright Copyright (c) 2022
#pragma once

//! @brief Creational-related functions in the design pattern module.
namespace design_pattern::creational
{
//! @brief Pattern of creational.
class CreationalPattern
{
public:
    //! @brief Construct a new CreationalPattern object.
    CreationalPattern();
    //! @brief Destroy the CreationalPattern object.
    virtual ~CreationalPattern() = default;

    //! @brief The abstract factory instance.
    void abstractFactoryInstance() const;
    //! @brief The builder instance.
    void builderInstance() const;
    //! @brief The factory method instance.
    void factoryMethodInstance() const;
    //! @brief The prototype instance.
    void prototypeInstance() const;
    //! @brief The singleton instance.
    void singletonInstance() const;
};
} // namespace design_pattern::creational
