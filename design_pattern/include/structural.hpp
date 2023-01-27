//! @file structural.hpp
//! @author ryftchen
//! @brief The declarations (structural) in the design pattern module.
//! @version 0.1
//! @copyright Copyright (c) 2022
#pragma once

//! @brief Structural-related functions in the design pattern module.
namespace design_pattern::structural
{
//! @brief Pattern of structural.
class StructuralPattern
{
public:
    //! @brief Construct a new StructuralPattern object.
    StructuralPattern();
    //! @brief Destroy the StructuralPattern object.
    virtual ~StructuralPattern() = default;

    //! @brief The adapter instance.
    void adapterInstance() const;
    //! @brief The bridge instance.
    void bridgeInstance() const;
    //! @brief The composite instance.
    void compositeInstance() const;
    //! @brief The decorator instance.
    void decoratorInstance() const;
    //! @brief The facade instance.
    void facadeInstance() const;
    //! @brief The flyweight instance.
    void flyweightInstance() const;
    //! @brief The proxy instance.
    void proxyInstance() const;
};
} // namespace design_pattern::structural
