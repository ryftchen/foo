//! @file register_design_pattern.cpp
//! @author ryftchen
//! @brief The definitions (register_design_pattern) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2026 ryftchen. All rights reserved.

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

namespace manage
{
//! @brief Get the design pattern choice applier.
//! @return reference of the ApplyDesignPattern object
ApplyDesignPattern& choiceApplier()
{
    static utility::common::NoDestructor<ApplyDesignPattern> applier{};
    return *applier.get();
}

//! @brief Check whether any design pattern choices exist.
//! @return any design pattern choices exist or not
bool present()
{
    bool isExist = false;
    TypeInfo<ApplyDesignPattern>::forEachVarOf(
        choiceApplier(), [&isExist](const auto /*field*/, auto&& var) { isExist |= !var.none(); });
    return isExist;
}
//! @brief Reset bit flags that manage design pattern choices.
void clear()
{
    TypeInfo<ApplyDesignPattern>::forEachVarOf(choiceApplier(), [](const auto /*field*/, auto&& var) { var.reset(); });
}
} // namespace manage

//! @brief Find the position of bit flags to set a particular instance.
//! @tparam Inst - type of target instance
//! @param stringify - instance name
//! @return position of bit flags
template <typename Inst>
static consteval std::size_t mappedPos(const std::string_view stringify)
{
    return static_cast<std::underlying_type_t<Inst>>(TypeInfo<Inst>::fields.template valueOfName<Inst>(stringify));
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
//! @brief Set behavioral-related choice.
//! @param choice - target choice
template <>
void setChoice<BehavioralInstance>(const std::string& choice)
{
    constexpr auto category = Category::behavioral;
    auto& spec = categoryOpts<category>();

    switch (utility::common::bkdrHash(choice.c_str()))
    {
        case abbrLitHash(BehavioralInstance::chainOfResponsibility):
            spec.set(mappedPos<BehavioralInstance>(MACRO_STRINGIFY(chainOfResponsibility)));
            break;
        case abbrLitHash(BehavioralInstance::command):
            spec.set(mappedPos<BehavioralInstance>(MACRO_STRINGIFY(command)));
            break;
        case abbrLitHash(BehavioralInstance::interpreter):
            spec.set(mappedPos<BehavioralInstance>(MACRO_STRINGIFY(interpreter)));
            break;
        case abbrLitHash(BehavioralInstance::iterator):
            spec.set(mappedPos<BehavioralInstance>(MACRO_STRINGIFY(iterator)));
            break;
        case abbrLitHash(BehavioralInstance::mediator):
            spec.set(mappedPos<BehavioralInstance>(MACRO_STRINGIFY(mediator)));
            break;
        case abbrLitHash(BehavioralInstance::memento):
            spec.set(mappedPos<BehavioralInstance>(MACRO_STRINGIFY(memento)));
            break;
        case abbrLitHash(BehavioralInstance::observer):
            spec.set(mappedPos<BehavioralInstance>(MACRO_STRINGIFY(observer)));
            break;
        case abbrLitHash(BehavioralInstance::state):
            spec.set(mappedPos<BehavioralInstance>(MACRO_STRINGIFY(state)));
            break;
        case abbrLitHash(BehavioralInstance::strategy):
            spec.set(mappedPos<BehavioralInstance>(MACRO_STRINGIFY(strategy)));
            break;
        case abbrLitHash(BehavioralInstance::templateMethod):
            spec.set(mappedPos<BehavioralInstance>(MACRO_STRINGIFY(templateMethod)));
            break;
        case abbrLitHash(BehavioralInstance::visitor):
            spec.set(mappedPos<BehavioralInstance>(MACRO_STRINGIFY(visitor)));
            break;
        default:
            spec.reset();
            throw std::runtime_error{"Unexpected " + std::string{toString(category)} + " choice: " + choice + '.'};
    }
}
//! @brief Run behavioral-related candidates.
//! @param candidates - container for the candidate target choices
template <>
void runCandidates<BehavioralInstance>(const std::vector<std::string>& candidates)
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
//! @brief Set creational-related choice.
//! @param choice - target choice
template <>
void setChoice<CreationalInstance>(const std::string& choice)
{
    constexpr auto category = Category::creational;
    auto& spec = categoryOpts<category>();

    switch (utility::common::bkdrHash(choice.c_str()))
    {
        case abbrLitHash(CreationalInstance::abstractFactory):
            spec.set(mappedPos<CreationalInstance>(MACRO_STRINGIFY(abstractFactory)));
            break;
        case abbrLitHash(CreationalInstance::builder):
            spec.set(mappedPos<CreationalInstance>(MACRO_STRINGIFY(builder)));
            break;
        case abbrLitHash(CreationalInstance::factoryMethod):
            spec.set(mappedPos<CreationalInstance>(MACRO_STRINGIFY(factoryMethod)));
            break;
        case abbrLitHash(CreationalInstance::prototype):
            spec.set(mappedPos<CreationalInstance>(MACRO_STRINGIFY(prototype)));
            break;
        case abbrLitHash(CreationalInstance::singleton):
            spec.set(mappedPos<CreationalInstance>(MACRO_STRINGIFY(singleton)));
            break;
        default:
            spec.reset();
            throw std::runtime_error{"Unexpected " + std::string{toString(category)} + " choice: " + choice + '.'};
    }
}
//! @brief Run creational-related candidates.
//! @param candidates - container for the candidate target choices
template <>
void runCandidates<CreationalInstance>(const std::vector<std::string>& candidates)
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
//! @brief Set structural-related choice.
//! @param choice - target choice
template <>
void setChoice<StructuralInstance>(const std::string& choice)
{
    constexpr auto category = Category::structural;
    auto& spec = categoryOpts<category>();

    switch (utility::common::bkdrHash(choice.c_str()))
    {
        case abbrLitHash(StructuralInstance::adapter):
            spec.set(mappedPos<StructuralInstance>(MACRO_STRINGIFY(adapter)));
            break;
        case abbrLitHash(StructuralInstance::bridge):
            spec.set(mappedPos<StructuralInstance>(MACRO_STRINGIFY(bridge)));
            break;
        case abbrLitHash(StructuralInstance::composite):
            spec.set(mappedPos<StructuralInstance>(MACRO_STRINGIFY(composite)));
            break;
        case abbrLitHash(StructuralInstance::decorator):
            spec.set(mappedPos<StructuralInstance>(MACRO_STRINGIFY(decorator)));
            break;
        case abbrLitHash(StructuralInstance::facade):
            spec.set(mappedPos<StructuralInstance>(MACRO_STRINGIFY(facade)));
            break;
        case abbrLitHash(StructuralInstance::flyweight):
            spec.set(mappedPos<StructuralInstance>(MACRO_STRINGIFY(flyweight)));
            break;
        case abbrLitHash(StructuralInstance::proxy):
            spec.set(mappedPos<StructuralInstance>(MACRO_STRINGIFY(proxy)));
            break;
        default:
            spec.reset();
            throw std::runtime_error{"Unexpected " + std::string{toString(category)} + " choice: " + choice + '.'};
    }
}
//! @brief Run structural-related candidates.
//! @param candidates - container for the candidate target choices
template <>
void runCandidates<StructuralInstance>(const std::vector<std::string>& candidates)
{
    app_dp::applyingStructural(candidates);
}
} // namespace application::reg_dp
