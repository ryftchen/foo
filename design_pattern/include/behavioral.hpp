//! @file behavioral.hpp
//! @author ryftchen
//! @brief The declarations (behavioral) in the design pattern module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023
#pragma once

//! @brief Behavioral-related functions in the design pattern module.
namespace design_pattern::behavioral
{
//! @brief Pattern of behavioral.
class BehavioralPattern
{
public:
    //! @brief Construct a new BehavioralPattern object.
    BehavioralPattern();
    //! @brief Destroy the BehavioralPattern object.
    virtual ~BehavioralPattern() = default;

    //! @brief The chain of responsibility instance.
    void chainOfResponsibilityInstance() const;
    //! @brief The command instance.
    void commandInstance() const;
    //! @brief The interpreter instance.
    void interpreterInstance() const;
    //! @brief The iterator instance.
    void iteratorInstance() const;
    //! @brief The mediator instance.
    void mediatorInstance() const;
    //! @brief The memento instance.
    void mementoInstance() const;
    //! @brief The observer instance.
    void observerInstance() const;
    //! @brief The state instance.
    void stateInstance() const;
    //! @brief The strategy instance.
    void strategyInstance() const;
    //! @brief The template method instance.
    void templateMethodInstance() const;
    //! @brief The visitor instance.
    void visitorInstance() const;
};
} // namespace design_pattern::behavioral
