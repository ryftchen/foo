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
ApplyDesignPattern& manager() noexcept
{
    static ApplyDesignPattern manager{};
    return manager;
}

//! @brief Check whether any design pattern choices exist.
//! @return any design pattern choices exist or not
bool present()
{
    bool isExist = false;
    TypeInfo<ApplyDesignPattern>::forEachVarOf(
        manager(), [&isExist](const auto /*field*/, auto&& var) { isExist |= !var.none(); });

    return isExist;
}

//! @brief Reset bit flags that manage design pattern choices.
void clear()
{
    TypeInfo<ApplyDesignPattern>::forEachVarOf(manager(), [](const auto /*field*/, auto&& var) { var.reset(); });
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
void updateChoice<BehavioralInstance>(const std::string& target)
{
    constexpr auto category = Category::behavioral;
    auto& bits = categoryOpts<category>();

    switch (utility::common::bkdrHash(target.c_str()))
    {
        case abbrValue(BehavioralInstance::chainOfResponsibility):
            bits.set(BehavioralInstance::chainOfResponsibility);
            break;
        case abbrValue(BehavioralInstance::command):
            bits.set(BehavioralInstance::command);
            break;
        case abbrValue(BehavioralInstance::interpreter):
            bits.set(BehavioralInstance::interpreter);
            break;
        case abbrValue(BehavioralInstance::iterator):
            bits.set(BehavioralInstance::iterator);
            break;
        case abbrValue(BehavioralInstance::mediator):
            bits.set(BehavioralInstance::mediator);
            break;
        case abbrValue(BehavioralInstance::memento):
            bits.set(BehavioralInstance::memento);
            break;
        case abbrValue(BehavioralInstance::observer):
            bits.set(BehavioralInstance::observer);
            break;
        case abbrValue(BehavioralInstance::state):
            bits.set(BehavioralInstance::state);
            break;
        case abbrValue(BehavioralInstance::strategy):
            bits.set(BehavioralInstance::strategy);
            break;
        case abbrValue(BehavioralInstance::templateMethod):
            bits.set(BehavioralInstance::templateMethod);
            break;
        case abbrValue(BehavioralInstance::visitor):
            bits.set(BehavioralInstance::visitor);
            break;
        default:
            bits.reset();
            throw std::logic_error{"Unexpected " + std::string{toString<category>()} + " instance: " + target + '.'};
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
void updateChoice<CreationalInstance>(const std::string& target)
{
    constexpr auto category = Category::creational;
    auto& bits = categoryOpts<category>();

    switch (utility::common::bkdrHash(target.c_str()))
    {
        case abbrValue(CreationalInstance::abstractFactory):
            bits.set(CreationalInstance::abstractFactory);
            break;
        case abbrValue(CreationalInstance::builder):
            bits.set(CreationalInstance::builder);
            break;
        case abbrValue(CreationalInstance::factoryMethod):
            bits.set(CreationalInstance::factoryMethod);
            break;
        case abbrValue(CreationalInstance::prototype):
            bits.set(CreationalInstance::prototype);
            break;
        case abbrValue(CreationalInstance::singleton):
            bits.set(CreationalInstance::singleton);
            break;
        default:
            bits.reset();
            throw std::logic_error{"Unexpected " + std::string{toString<category>()} + " instance: " + target + '.'};
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
void updateChoice<StructuralInstance>(const std::string& target)
{
    constexpr auto category = Category::structural;
    auto& bits = categoryOpts<category>();

    switch (utility::common::bkdrHash(target.c_str()))
    {
        case abbrValue(StructuralInstance::adapter):
            bits.set(StructuralInstance::adapter);
            break;
        case abbrValue(StructuralInstance::bridge):
            bits.set(StructuralInstance::bridge);
            break;
        case abbrValue(StructuralInstance::composite):
            bits.set(StructuralInstance::composite);
            break;
        case abbrValue(StructuralInstance::decorator):
            bits.set(StructuralInstance::decorator);
            break;
        case abbrValue(StructuralInstance::facade):
            bits.set(StructuralInstance::facade);
            break;
        case abbrValue(StructuralInstance::flyweight):
            bits.set(StructuralInstance::flyweight);
            break;
        case abbrValue(StructuralInstance::proxy):
            bits.set(StructuralInstance::proxy);
            break;
        default:
            bits.reset();
            throw std::logic_error{"Unexpected " + std::string{toString<category>()} + " instance: " + target + '.'};
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
