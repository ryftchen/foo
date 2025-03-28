//! @file register_design_pattern.cpp
//! @author ryftchen
//! @brief The definitions (register_design_pattern) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "register_design_pattern.hpp"
#include "apply_design_pattern.hpp"

namespace application::reg_dp
{
//! @brief Register version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    static const char* const ver = "0.1.0";
    return ver;
}

//! @brief Get the design pattern choice manager.
//! @return reference of the ApplyDesignPattern object
ApplyDesignPattern& manager()
{
    static ApplyDesignPattern manager{};
    return manager;
}

namespace behavioral
{
//! @brief Register version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    return app_dp::behavioral::version;
}
} // namespace behavioral
//! @brief Update behavioral-related choice.
//! @param target - target instance
template <>
void updateChoice<BehavioralInstance>(const std::string_view target)
{
    constexpr auto category = Category::behavioral;
    auto& bits = getCategoryOpts<category>();

    switch (utility::common::bkdrHash(target.data()))
    {
        case abbrVal(BehavioralInstance::chainOfResponsibility):
            bits.set(BehavioralInstance::chainOfResponsibility);
            break;
        case abbrVal(BehavioralInstance::command):
            bits.set(BehavioralInstance::command);
            break;
        case abbrVal(BehavioralInstance::interpreter):
            bits.set(BehavioralInstance::interpreter);
            break;
        case abbrVal(BehavioralInstance::iterator):
            bits.set(BehavioralInstance::iterator);
            break;
        case abbrVal(BehavioralInstance::mediator):
            bits.set(BehavioralInstance::mediator);
            break;
        case abbrVal(BehavioralInstance::memento):
            bits.set(BehavioralInstance::memento);
            break;
        case abbrVal(BehavioralInstance::observer):
            bits.set(BehavioralInstance::observer);
            break;
        case abbrVal(BehavioralInstance::state):
            bits.set(BehavioralInstance::state);
            break;
        case abbrVal(BehavioralInstance::strategy):
            bits.set(BehavioralInstance::strategy);
            break;
        case abbrVal(BehavioralInstance::templateMethod):
            bits.set(BehavioralInstance::templateMethod);
            break;
        case abbrVal(BehavioralInstance::visitor):
            bits.set(BehavioralInstance::visitor);
            break;
        default:
            bits.reset();
            throw std::logic_error{
                "Unexpected " + std::string{toString<category>()} + " instance: " + target.data() + '.'};
    }
}
//! @brief Run behavioral-related choices.
//! @param candidates - container for the candidate target instances
template <>
void runChoices<BehavioralInstance>(const std::vector<std::string>& candidates)
{
    app_dp::applyingBehavioral(candidates);
}

namespace creational
{
//! @brief Register version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    return app_dp::creational::version;
}
} // namespace creational
//! @brief Update creational-related choice.
//! @param target - target instance
template <>
void updateChoice<CreationalInstance>(const std::string_view target)
{
    constexpr auto category = Category::creational;
    auto& bits = getCategoryOpts<category>();

    switch (utility::common::bkdrHash(target.data()))
    {
        case abbrVal(CreationalInstance::abstractFactory):
            bits.set(CreationalInstance::abstractFactory);
            break;
        case abbrVal(CreationalInstance::builder):
            bits.set(CreationalInstance::builder);
            break;
        case abbrVal(CreationalInstance::factoryMethod):
            bits.set(CreationalInstance::factoryMethod);
            break;
        case abbrVal(CreationalInstance::prototype):
            bits.set(CreationalInstance::prototype);
            break;
        case abbrVal(CreationalInstance::singleton):
            bits.set(CreationalInstance::singleton);
            break;
        default:
            bits.reset();
            throw std::logic_error{
                "Unexpected " + std::string{toString<category>()} + " instance: " + target.data() + '.'};
    }
}
//! @brief Run creational-related choices.
//! @param candidates - container for the candidate target instances
template <>
void runChoices<CreationalInstance>(const std::vector<std::string>& candidates)
{
    app_dp::applyingCreational(candidates);
}

namespace structural
{
//! @brief Register version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    return app_dp::structural::version;
}
} // namespace structural
//! @brief Update structural-related choice.
//! @param target - target instance
template <>
void updateChoice<StructuralInstance>(const std::string_view target)
{
    constexpr auto category = Category::structural;
    auto& bits = getCategoryOpts<category>();

    switch (utility::common::bkdrHash(target.data()))
    {
        case abbrVal(StructuralInstance::adapter):
            bits.set(StructuralInstance::adapter);
            break;
        case abbrVal(StructuralInstance::bridge):
            bits.set(StructuralInstance::bridge);
            break;
        case abbrVal(StructuralInstance::composite):
            bits.set(StructuralInstance::composite);
            break;
        case abbrVal(StructuralInstance::decorator):
            bits.set(StructuralInstance::decorator);
            break;
        case abbrVal(StructuralInstance::facade):
            bits.set(StructuralInstance::facade);
            break;
        case abbrVal(StructuralInstance::flyweight):
            bits.set(StructuralInstance::flyweight);
            break;
        case abbrVal(StructuralInstance::proxy):
            bits.set(StructuralInstance::proxy);
            break;
        default:
            bits.reset();
            throw std::logic_error{
                "Unexpected " + std::string{toString<category>()} + " instance: " + target.data() + '.'};
    }
}
//! @brief Run structural-related choices.
//! @param candidates - container for the candidate target instances
template <>
void runChoices<StructuralInstance>(const std::vector<std::string>& candidates)
{
    app_dp::applyingStructural(candidates);
}
} // namespace application::reg_dp
