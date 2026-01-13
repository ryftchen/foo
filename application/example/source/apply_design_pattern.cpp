//! @file apply_design_pattern.cpp
//! @author ryftchen
//! @brief The definitions (apply_design_pattern) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2026 ryftchen. All rights reserved.

#include "apply_design_pattern.hpp"
#include "register_design_pattern.hpp"

#ifndef _PRECOMPILED_HEADER
#include <iomanip>
#include <ranges>
#else
#include "application/pch/precompiled_header.hpp"
#endif

#include "application/core/include/log.hpp"
#include "utility/include/currying.hpp"

//! @brief Title of printing when design pattern tasks are beginning.
#define APP_DP_PRINT_TASK_TITLE_SCOPE_BEGIN(title)                                                            \
    std::osyncstream(std::cout) << "\nAPPLY DESIGN PATTERN: " << std::setiosflags(std::ios_base::left)        \
                                << std::setfill('.') << std::setw(50) << (title) << "BEGIN"                   \
                                << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl; \
    {
//! @brief Title of printing when design pattern tasks are ending.
#define APP_DP_PRINT_TASK_TITLE_SCOPE_END(title)                                                        \
    }                                                                                                   \
    std::osyncstream(std::cout) << "\nAPPLY DESIGN PATTERN: " << std::setiosflags(std::ios_base::left)  \
                                << std::setfill('.') << std::setw(50) << (title) << "END"               \
                                << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << '\n' \
                                << std::endl;

namespace application::app_dp
{
using namespace reg_dp; // NOLINT(google-build-using-namespace)

//! @brief Make the title of a particular instance in design pattern choices.
//! @tparam Inst - type of target instance
//! @param instance - target instance
//! @return initial capitalized title
template <typename Inst>
static std::string customTitle(const Inst instance)
{
    std::string title(TypeInfo<Inst>::fields.nameOfValue(instance));
    title.at(0) = static_cast<char>(std::toupper(title.at(0)));
    return title;
}

//! @brief Get the curried task name.
//! @return curried task name
static const auto& curriedTaskName()
{
    static const auto curried =
        utility::currying::curry(configure::task::presetName, TypeInfo<ApplyDesignPattern>::name);
    return curried;
}

//! @brief Get the alias of the category in design pattern choices.
//! @tparam Cat - target category
//! @return alias of the category name
template <Category Cat>
static consteval std::string_view categoryAlias()
{
    constexpr auto attr =
        TypeInfo<ApplyDesignPattern>::fields.find(REFLECTION_STR(toString(Cat))).attrs.find(REFLECTION_STR("alias"));
    static_assert(attr.hasValue);
    return attr.value;
}

namespace behavioral
{
//! @brief Show the contents of the behavioral result.
//! @param instance - used behavioral instance
//! @param result - behavioral result
static void display(const BehavioralInstance instance, const std::string& result)
{
    std::printf("\n==> %-21s Instance <==\n%s", customTitle(instance).c_str(), result.c_str());
}

//! @brief Pattern of behavioral.
//! @param instance - used behavioral instance
static void pattern(const BehavioralInstance instance)
try
{
    std::ostringstream result{};
    switch (instance)
    {
        static_assert(utility::common::isStatelessClass<Showcase>());
        case BehavioralInstance::chainOfResponsibility:
            result = Showcase::chainOfResponsibility();
            break;
        case BehavioralInstance::command:
            result = Showcase::command();
            break;
        case BehavioralInstance::interpreter:
            result = Showcase::interpreter();
            break;
        case BehavioralInstance::iterator:
            result = Showcase::iterator();
            break;
        case BehavioralInstance::mediator:
            result = Showcase::mediator();
            break;
        case BehavioralInstance::memento:
            result = Showcase::memento();
            break;
        case BehavioralInstance::observer:
            result = Showcase::observer();
            break;
        case BehavioralInstance::state:
            result = Showcase::state();
            break;
        case BehavioralInstance::strategy:
            result = Showcase::strategy();
            break;
        case BehavioralInstance::templateMethod:
            result = Showcase::templateMethod();
            break;
        case BehavioralInstance::visitor:
            result = Showcase::visitor();
            break;
        default:
            return;
    }
    display(instance, result.str());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in %s (%s): %s", __func__, customTitle(instance).c_str(), err.what());
}
} // namespace behavioral
//! @brief To apply behavioral-related instances that are mapped to choices.
//! @param candidates - container for the candidate target choices
void applyingBehavioral(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::behavioral;
    const auto& spec = categoryOpts<category>();
    if (MACRO_IMPLIES(spec.any(), spec.size() != candidates.size()))
    {
        return;
    }

    const std::string_view title = design_pattern::behavioral::description();
    APP_DP_PRINT_TASK_TITLE_SCOPE_BEGIN(title);

    auto& pooling = configure::task::resourcePool();
    auto* const allocatedJob = pooling.newEntry(spec.count());
    const auto taskNamer = utility::currying::curry(curriedTaskName(), categoryAlias<category>());
    const auto addTask = [allocatedJob, &taskNamer](const std::string_view subTask, const BehavioralInstance instance)
    { allocatedJob->enqueue(taskNamer(subTask), behavioral::pattern, instance); };
    MACRO_DEFER(utility::common::wrapClosure([&]() { pooling.deleteEntry(allocatedJob); }));

    std::cout << "\nInstances of the " << toString(category) << " pattern:" << std::endl;
    for (const auto index :
         std::views::iota(0U, spec.size()) | std::views::filter([&spec](const auto i) { return spec.test(i); }))
    {
        const auto& choice = candidates.at(index);
        switch (utility::common::bkdrHash(choice.c_str()))
        {
            case abbrLitHash(BehavioralInstance::chainOfResponsibility):
                addTask(choice, BehavioralInstance::chainOfResponsibility);
                break;
            case abbrLitHash(BehavioralInstance::command):
                addTask(choice, BehavioralInstance::command);
                break;
            case abbrLitHash(BehavioralInstance::interpreter):
                addTask(choice, BehavioralInstance::interpreter);
                break;
            case abbrLitHash(BehavioralInstance::iterator):
                addTask(choice, BehavioralInstance::iterator);
                break;
            case abbrLitHash(BehavioralInstance::mediator):
                addTask(choice, BehavioralInstance::mediator);
                break;
            case abbrLitHash(BehavioralInstance::memento):
                addTask(choice, BehavioralInstance::memento);
                break;
            case abbrLitHash(BehavioralInstance::observer):
                addTask(choice, BehavioralInstance::observer);
                break;
            case abbrLitHash(BehavioralInstance::state):
                addTask(choice, BehavioralInstance::state);
                break;
            case abbrLitHash(BehavioralInstance::strategy):
                addTask(choice, BehavioralInstance::strategy);
                break;
            case abbrLitHash(BehavioralInstance::templateMethod):
                addTask(choice, BehavioralInstance::templateMethod);
                break;
            case abbrLitHash(BehavioralInstance::visitor):
                addTask(choice, BehavioralInstance::visitor);
                break;
            default:
                throw std::runtime_error{"Unknown " + std::string{toString(category)} + " choice: " + choice + '.'};
        }
    }

    APP_DP_PRINT_TASK_TITLE_SCOPE_END(title);
}

namespace creational
{
//! @brief Show the contents of the creational result.
//! @param instance - used creational instance
//! @param result - creational result
static void display(const CreationalInstance instance, const std::string& result)
{
    std::printf("\n==> %-15s Instance <==\n%s", customTitle(instance).c_str(), result.c_str());
}

//! @brief Pattern of creational.
//! @param instance - used creational instance
static void pattern(const CreationalInstance instance)
try
{
    std::ostringstream result{};
    switch (instance)
    {
        static_assert(utility::common::isStatelessClass<Showcase>());
        case CreationalInstance::abstractFactory:
            result = Showcase::abstractFactory();
            break;
        case CreationalInstance::builder:
            result = Showcase::builder();
            break;
        case CreationalInstance::factoryMethod:
            result = Showcase::factoryMethod();
            break;
        case CreationalInstance::prototype:
            result = Showcase::prototype();
            break;
        case CreationalInstance::singleton:
            result = Showcase::singleton();
            break;
        default:
            return;
    }
    display(instance, result.str());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in %s (%s): %s", __func__, customTitle(instance).c_str(), err.what());
}
} // namespace creational
//! @brief To apply creational-related instances that are mapped to choices.
//! @param candidates - container for the candidate target choices
void applyingCreational(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::creational;
    const auto& spec = categoryOpts<category>();
    if (MACRO_IMPLIES(spec.any(), spec.size() != candidates.size()))
    {
        return;
    }

    const std::string_view title = design_pattern::creational::description();
    APP_DP_PRINT_TASK_TITLE_SCOPE_BEGIN(title);

    auto& pooling = configure::task::resourcePool();
    auto* const allocatedJob = pooling.newEntry(spec.count());
    const auto taskNamer = utility::currying::curry(curriedTaskName(), categoryAlias<category>());
    const auto addTask = [allocatedJob, &taskNamer](const std::string_view subTask, const CreationalInstance instance)
    { allocatedJob->enqueue(taskNamer(subTask), creational::pattern, instance); };
    MACRO_DEFER(utility::common::wrapClosure([&]() { pooling.deleteEntry(allocatedJob); }));

    std::cout << "\nInstances of the " << toString(category) << " pattern:" << std::endl;
    for (const auto index :
         std::views::iota(0U, spec.size()) | std::views::filter([&spec](const auto i) { return spec.test(i); }))
    {
        const auto& choice = candidates.at(index);
        switch (utility::common::bkdrHash(choice.c_str()))
        {
            case abbrLitHash(CreationalInstance::abstractFactory):
                addTask(choice, CreationalInstance::abstractFactory);
                break;
            case abbrLitHash(CreationalInstance::builder):
                addTask(choice, CreationalInstance::builder);
                break;
            case abbrLitHash(CreationalInstance::factoryMethod):
                addTask(choice, CreationalInstance::factoryMethod);
                break;
            case abbrLitHash(CreationalInstance::prototype):
                addTask(choice, CreationalInstance::prototype);
                break;
            case abbrLitHash(CreationalInstance::singleton):
                addTask(choice, CreationalInstance::singleton);
                break;
            default:
                throw std::runtime_error{"Unknown " + std::string{toString(category)} + " choice: " + choice + '.'};
        }
    }

    APP_DP_PRINT_TASK_TITLE_SCOPE_END(title);
}

namespace structural
{
//! @brief Show the contents of the structural result.
//! @param instance - used structural instance
//! @param result - structural result
static void display(const StructuralInstance instance, const std::string& result)
{
    std::printf("\n==> %-9s Instance <==\n%s", customTitle(instance).c_str(), result.c_str());
}

//! @brief Pattern of structural.
//! @param instance - used structural instance
static void pattern(const StructuralInstance instance)
try
{
    std::ostringstream result{};
    switch (instance)
    {
        static_assert(utility::common::isStatelessClass<Showcase>());
        case StructuralInstance::adapter:
            result = Showcase::adapter();
            break;
        case StructuralInstance::bridge:
            result = Showcase::bridge();
            break;
        case StructuralInstance::composite:
            result = Showcase::composite();
            break;
        case StructuralInstance::decorator:
            result = Showcase::decorator();
            break;
        case StructuralInstance::facade:
            result = Showcase::facade();
            break;
        case StructuralInstance::flyweight:
            result = Showcase::flyweight();
            break;
        case StructuralInstance::proxy:
            result = Showcase::proxy();
            break;
        default:
            return;
    }
    display(instance, result.str());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in %s (%s): %s", __func__, customTitle(instance).c_str(), err.what());
}
} // namespace structural
//! @brief To apply structural-related instances that are mapped to choices.
//! @param candidates - container for the candidate target choices
void applyingStructural(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::structural;
    const auto& spec = categoryOpts<category>();
    if (MACRO_IMPLIES(spec.any(), spec.size() != candidates.size()))
    {
        return;
    }

    const std::string_view title = design_pattern::structural::description();
    APP_DP_PRINT_TASK_TITLE_SCOPE_BEGIN(title);

    auto& pooling = configure::task::resourcePool();
    auto* const allocatedJob = pooling.newEntry(spec.count());
    const auto taskNamer = utility::currying::curry(curriedTaskName(), categoryAlias<category>());
    const auto addTask = [allocatedJob, &taskNamer](const std::string_view subTask, const StructuralInstance instance)
    { allocatedJob->enqueue(taskNamer(subTask), structural::pattern, instance); };
    MACRO_DEFER(utility::common::wrapClosure([&]() { pooling.deleteEntry(allocatedJob); }));

    std::cout << "\nInstances of the " << toString(category) << " pattern:" << std::endl;
    for (const auto index :
         std::views::iota(0U, spec.size()) | std::views::filter([&spec](const auto i) { return spec.test(i); }))
    {
        const auto& choice = candidates.at(index);
        switch (utility::common::bkdrHash(choice.c_str()))
        {
            case abbrLitHash(StructuralInstance::adapter):
                addTask(choice, StructuralInstance::adapter);
                break;
            case abbrLitHash(StructuralInstance::bridge):
                addTask(choice, StructuralInstance::bridge);
                break;
            case abbrLitHash(StructuralInstance::composite):
                addTask(choice, StructuralInstance::composite);
                break;
            case abbrLitHash(StructuralInstance::decorator):
                addTask(choice, StructuralInstance::decorator);
                break;
            case abbrLitHash(StructuralInstance::facade):
                addTask(choice, StructuralInstance::facade);
                break;
            case abbrLitHash(StructuralInstance::flyweight):
                addTask(choice, StructuralInstance::flyweight);
                break;
            case abbrLitHash(StructuralInstance::proxy):
                addTask(choice, StructuralInstance::proxy);
                break;
            default:
                throw std::runtime_error{"Unknown " + std::string{toString(category)} + " choice: " + choice + '.'};
        }
    }

    APP_DP_PRINT_TASK_TITLE_SCOPE_END(title);
}
} // namespace application::app_dp

#undef APP_DP_PRINT_TASK_TITLE_SCOPE_BEGIN
#undef APP_DP_PRINT_TASK_TITLE_SCOPE_END
