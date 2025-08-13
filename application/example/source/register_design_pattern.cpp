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

//! @brief Find the position of bit flags to set a particular instance.
//! @tparam T - type of target instance
//! @param stringify - instance name
//! @return position of bit flags
template <typename T>
static consteval std::size_t findPosition(const std::string_view stringify)
{
    return static_cast<std::size_t>(TypeInfo<T>::fields.template valueOfName<T>(stringify));
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
            bits.set(findPosition<BehavioralInstance>(MACRO_STRINGIFY(chainOfResponsibility)));
            break;
        case abbrValue(BehavioralInstance::command):
            bits.set(findPosition<BehavioralInstance>(MACRO_STRINGIFY(command)));
            break;
        case abbrValue(BehavioralInstance::interpreter):
            bits.set(findPosition<BehavioralInstance>(MACRO_STRINGIFY(interpreter)));
            break;
        case abbrValue(BehavioralInstance::iterator):
            bits.set(findPosition<BehavioralInstance>(MACRO_STRINGIFY(iterator)));
            break;
        case abbrValue(BehavioralInstance::mediator):
            bits.set(findPosition<BehavioralInstance>(MACRO_STRINGIFY(mediator)));
            break;
        case abbrValue(BehavioralInstance::memento):
            bits.set(findPosition<BehavioralInstance>(MACRO_STRINGIFY(memento)));
            break;
        case abbrValue(BehavioralInstance::observer):
            bits.set(findPosition<BehavioralInstance>(MACRO_STRINGIFY(observer)));
            break;
        case abbrValue(BehavioralInstance::state):
            bits.set(findPosition<BehavioralInstance>(MACRO_STRINGIFY(state)));
            break;
        case abbrValue(BehavioralInstance::strategy):
            bits.set(findPosition<BehavioralInstance>(MACRO_STRINGIFY(strategy)));
            break;
        case abbrValue(BehavioralInstance::templateMethod):
            bits.set(findPosition<BehavioralInstance>(MACRO_STRINGIFY(templateMethod)));
            break;
        case abbrValue(BehavioralInstance::visitor):
            bits.set(findPosition<BehavioralInstance>(MACRO_STRINGIFY(visitor)));
            break;
        default:
            bits.reset();
            throw std::logic_error{"Unexpected " + std::string{toString(category)} + " instance: " + target + '.'};
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
            bits.set(findPosition<CreationalInstance>(MACRO_STRINGIFY(abstractFactory)));
            break;
        case abbrValue(CreationalInstance::builder):
            bits.set(findPosition<CreationalInstance>(MACRO_STRINGIFY(builder)));
            break;
        case abbrValue(CreationalInstance::factoryMethod):
            bits.set(findPosition<CreationalInstance>(MACRO_STRINGIFY(factoryMethod)));
            break;
        case abbrValue(CreationalInstance::prototype):
            bits.set(findPosition<CreationalInstance>(MACRO_STRINGIFY(prototype)));
            break;
        case abbrValue(CreationalInstance::singleton):
            bits.set(findPosition<CreationalInstance>(MACRO_STRINGIFY(singleton)));
            break;
        default:
            bits.reset();
            throw std::logic_error{"Unexpected " + std::string{toString(category)} + " instance: " + target + '.'};
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
            bits.set(findPosition<StructuralInstance>(MACRO_STRINGIFY(adapter)));
            break;
        case abbrValue(StructuralInstance::bridge):
            bits.set(findPosition<StructuralInstance>(MACRO_STRINGIFY(bridge)));
            break;
        case abbrValue(StructuralInstance::composite):
            bits.set(findPosition<StructuralInstance>(MACRO_STRINGIFY(composite)));
            break;
        case abbrValue(StructuralInstance::decorator):
            bits.set(findPosition<StructuralInstance>(MACRO_STRINGIFY(decorator)));
            break;
        case abbrValue(StructuralInstance::facade):
            bits.set(findPosition<StructuralInstance>(MACRO_STRINGIFY(facade)));
            break;
        case abbrValue(StructuralInstance::flyweight):
            bits.set(findPosition<StructuralInstance>(MACRO_STRINGIFY(flyweight)));
            break;
        case abbrValue(StructuralInstance::proxy):
            bits.set(findPosition<StructuralInstance>(MACRO_STRINGIFY(proxy)));
            break;
        default:
            bits.reset();
            throw std::logic_error{"Unexpected " + std::string{toString(category)} + " instance: " + target + '.'};
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
