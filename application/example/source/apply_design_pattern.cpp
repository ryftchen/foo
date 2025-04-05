//! @file apply_design_pattern.cpp
//! @author ryftchen
//! @brief The definitions (apply_design_pattern) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "apply_design_pattern.hpp"
#include "register_design_pattern.hpp"

#ifndef __PRECOMPILED_HEADER
#include <cassert>
#include <ranges>
#include <syncstream>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

#include "application/core/include/log.hpp"
#include "utility/include/currying.hpp"

//! @brief Title of printing when design pattern tasks are beginning.
#define APP_DP_PRINT_TASK_BEGIN_TITLE(category)                                                               \
    std::osyncstream(std::cout) << "\nDESIGN PATTERN TASK: " << std::setiosflags(std::ios_base::left)         \
                                << std::setfill('.') << std::setw(50) << category << "BEGIN"                  \
                                << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl; \
    {
//! @brief Title of printing when design pattern tasks are ending.
#define APP_DP_PRINT_TASK_END_TITLE(category)                                                           \
    }                                                                                                   \
    std::osyncstream(std::cout) << "\nDESIGN PATTERN TASK: " << std::setiosflags(std::ios_base::left)   \
                                << std::setfill('.') << std::setw(50) << category << "END"              \
                                << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << '\n' \
                                << std::endl;

namespace application::app_dp
{
using namespace reg_dp; // NOLINT(google-build-using-namespace)

//! @brief Make the title of a particular instance in design pattern choices.
//! @tparam T - type of target instance
//! @param instance - target instance
//! @return initial capitalized title
template <typename T>
static std::string makeTitle(const T instance)
{
    std::string title(toString(instance));
    title.at(0) = std::toupper(title.at(0));

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
//! @tparam Cat - the specific value of Category enum
//! @return alias of the category name
template <Category Cat>
static consteval std::string_view categoryAlias()
{
    constexpr auto attr =
        TypeInfo<ApplyDesignPattern>::fields.find(REFLECTION_STR(toString<Cat>())).attrs.find(REFLECTION_STR("alias"));
    static_assert(attr.hasValue);
    return attr.value;
}

namespace behavioral
{
//! @brief Show the contents of the behavioral result.
//! @param instance - the specific value of BehavioralInstance enum
//! @param result - behavioral result
static void showResult(const BehavioralInstance instance, const std::string_view result)
{
    std::printf("\n==> %-21s Instance <==\n%s", makeTitle(instance).c_str(), result.data());
}

void BehavioralPattern::chainOfResponsibilityInstance()
try
{
    const auto output = Behavioral().chainOfResponsibility();
    showResult(BehavioralInstance::chainOfResponsibility, output.str());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void BehavioralPattern::commandInstance()
try
{
    const auto output = Behavioral().command();
    showResult(BehavioralInstance::command, output.str());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void BehavioralPattern::interpreterInstance()
try
{
    const auto output = Behavioral().interpreter();
    showResult(BehavioralInstance::interpreter, output.str());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void BehavioralPattern::iteratorInstance()
try
{
    const auto output = Behavioral().iterator();
    showResult(BehavioralInstance::iterator, output.str());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void BehavioralPattern::mediatorInstance()
try
{
    const auto output = Behavioral().mediator();
    showResult(BehavioralInstance::mediator, output.str());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void BehavioralPattern::mementoInstance()
try
{
    const auto output = Behavioral().memento();
    showResult(BehavioralInstance::memento, output.str());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void BehavioralPattern::observerInstance()
try
{
    const auto output = Behavioral().observer();
    showResult(BehavioralInstance::observer, output.str());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void BehavioralPattern::stateInstance()
try
{
    const auto output = Behavioral().state();
    showResult(BehavioralInstance::state, output.str());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void BehavioralPattern::strategyInstance()
try
{
    const auto output = Behavioral().strategy();
    showResult(BehavioralInstance::strategy, output.str());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void BehavioralPattern::templateMethodInstance()
try
{
    const auto output = Behavioral().templateMethod();
    showResult(BehavioralInstance::templateMethod, output.str());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void BehavioralPattern::visitorInstance()
try
{
    const auto output = Behavioral().visitor();
    showResult(BehavioralInstance::visitor, output.str());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}
} // namespace behavioral
//! @brief To apply behavioral-related instances.
//! @param candidates - container for the candidate target instances
void applyingBehavioral(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::behavioral;
    const auto& bits = categoryOpts<category>();
    if (bits.none())
    {
        return;
    }
    assert(bits.size() == candidates.size());

    APP_DP_PRINT_TASK_BEGIN_TITLE(category);
    auto& pooling = configure::task::resourcePool();
    auto* const threads = pooling.newElement(bits.count());
    const auto taskNamer = utility::currying::curry(curriedTaskName(), categoryAlias<category>());
    const auto addTask = [threads, &taskNamer](const std::string_view subTask, void (*targetInstance)())
    { threads->enqueue(taskNamer(subTask), targetInstance); };

    std::cout << "\nInstances of the " << toString<category>() << " pattern:" << std::endl;
    for (const auto index :
         std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); }))
    {
        const auto& target = candidates.at(index);
        switch (utility::common::bkdrHash(target.c_str()))
        {
            using behavioral::BehavioralPattern;
            static_assert(utility::common::isStatelessClass<BehavioralPattern>());
            case abbrValue(BehavioralInstance::chainOfResponsibility):
                addTask(target, &BehavioralPattern::chainOfResponsibilityInstance);
                break;
            case abbrValue(BehavioralInstance::command):
                addTask(target, &BehavioralPattern::commandInstance);
                break;
            case abbrValue(BehavioralInstance::interpreter):
                addTask(target, &BehavioralPattern::interpreterInstance);
                break;
            case abbrValue(BehavioralInstance::iterator):
                addTask(target, &BehavioralPattern::iteratorInstance);
                break;
            case abbrValue(BehavioralInstance::mediator):
                addTask(target, &BehavioralPattern::mediatorInstance);
                break;
            case abbrValue(BehavioralInstance::memento):
                addTask(target, &BehavioralPattern::mementoInstance);
                break;
            case abbrValue(BehavioralInstance::observer):
                addTask(target, &BehavioralPattern::observerInstance);
                break;
            case abbrValue(BehavioralInstance::state):
                addTask(target, &BehavioralPattern::stateInstance);
                break;
            case abbrValue(BehavioralInstance::strategy):
                addTask(target, &BehavioralPattern::strategyInstance);
                break;
            case abbrValue(BehavioralInstance::templateMethod):
                addTask(target, &BehavioralPattern::templateMethodInstance);
                break;
            case abbrValue(BehavioralInstance::visitor):
                addTask(target, &BehavioralPattern::visitorInstance);
                break;
            default:
                throw std::logic_error{"Unknown " + std::string{toString<category>()} + " instance: " + target + '.'};
        }
    }

    pooling.deleteElement(threads);
    APP_DP_PRINT_TASK_END_TITLE(category);
}

namespace creational
{
//! @brief Show the contents of the creational result.
//! @param instance - the specific value of CreationalInstance enum
//! @param result - creational result
static void showResult(const CreationalInstance instance, const std::string_view result)
{
    std::printf("\n==> %-15s Instance <==\n%s", makeTitle(instance).c_str(), result.data());
}

void CreationalPattern::abstractFactoryInstance()
try
{
    const auto output = Creational().abstractFactory();
    showResult(CreationalInstance::abstractFactory, output.str());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void CreationalPattern::builderInstance()
try
{
    const auto output = Creational().builder();
    showResult(CreationalInstance::builder, output.str());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void CreationalPattern::factoryMethodInstance()
try
{
    const auto output = Creational().factoryMethod();
    showResult(CreationalInstance::factoryMethod, output.str());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void CreationalPattern::prototypeInstance()
try
{
    const auto output = Creational().prototype();
    showResult(CreationalInstance::prototype, output.str());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void CreationalPattern::singletonInstance()
try
{
    const auto output = Creational().singleton();
    showResult(CreationalInstance::singleton, output.str());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}
} // namespace creational
//! @brief To apply creational-related instances.
//! @param candidates - container for the candidate target instances
void applyingCreational(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::creational;
    const auto& bits = categoryOpts<category>();
    if (bits.none())
    {
        return;
    }
    assert(bits.size() == candidates.size());

    APP_DP_PRINT_TASK_BEGIN_TITLE(category);
    auto& pooling = configure::task::resourcePool();
    auto* const threads = pooling.newElement(bits.count());
    const auto taskNamer = utility::currying::curry(curriedTaskName(), categoryAlias<category>());
    const auto addTask = [threads, &taskNamer](const std::string_view subTask, void (*targetInstance)())
    { threads->enqueue(taskNamer(subTask), targetInstance); };

    std::cout << "\nInstances of the " << toString<category>() << " pattern:" << std::endl;
    for (const auto index :
         std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); }))
    {
        const auto& target = candidates.at(index);
        switch (utility::common::bkdrHash(target.c_str()))
        {
            using creational::CreationalPattern;
            static_assert(utility::common::isStatelessClass<CreationalPattern>());
            case abbrValue(CreationalInstance::abstractFactory):
                addTask(target, &CreationalPattern::abstractFactoryInstance);
                break;
            case abbrValue(CreationalInstance::builder):
                addTask(target, &CreationalPattern::builderInstance);
                break;
            case abbrValue(CreationalInstance::factoryMethod):
                addTask(target, &CreationalPattern::factoryMethodInstance);
                break;
            case abbrValue(CreationalInstance::prototype):
                addTask(target, &CreationalPattern::prototypeInstance);
                break;
            case abbrValue(CreationalInstance::singleton):
                addTask(target, &CreationalPattern::singletonInstance);
                break;
            default:
                throw std::logic_error{"Unknown " + std::string{toString<category>()} + " instance: " + target + '.'};
        }
    }

    pooling.deleteElement(threads);
    APP_DP_PRINT_TASK_END_TITLE(category);
}

namespace structural
{
//! @brief Show the contents of the structural result.
//! @param instance - the specific value of StructuralInstance enum
//! @param result - structural result
static void showResult(const StructuralInstance instance, const std::string_view result)
{
    std::printf("\n==> %-9s Instance <==\n%s", makeTitle(instance).c_str(), result.data());
}

void StructuralPattern::adapterInstance()
try
{
    const auto output = Structural().adapter();
    showResult(StructuralInstance::adapter, output.str());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void StructuralPattern::bridgeInstance()
try
{
    const auto output = Structural().bridge();
    showResult(StructuralInstance::bridge, output.str());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void StructuralPattern::compositeInstance()
try
{
    const auto output = Structural().composite();
    showResult(StructuralInstance::composite, output.str());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void StructuralPattern::decoratorInstance()
try
{
    const auto output = Structural().decorator();
    showResult(StructuralInstance::decorator, output.str());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void StructuralPattern::facadeInstance()
try
{
    const auto output = Structural().facade();
    showResult(StructuralInstance::facade, output.str());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void StructuralPattern::flyweightInstance()
try
{
    const auto output = Structural().flyweight();
    showResult(StructuralInstance::flyweight, output.str());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void StructuralPattern::proxyInstance()
try
{
    const auto output = Structural().proxy();
    showResult(StructuralInstance::proxy, output.str());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}
} // namespace structural
//! @brief To apply structural-related instances.
//! @param candidates - container for the candidate target instances
void applyingStructural(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::structural;
    const auto& bits = categoryOpts<category>();
    if (bits.none())
    {
        return;
    }
    assert(bits.size() == candidates.size());

    APP_DP_PRINT_TASK_BEGIN_TITLE(category);
    auto& pooling = configure::task::resourcePool();
    auto* const threads = pooling.newElement(bits.count());
    const auto taskNamer = utility::currying::curry(curriedTaskName(), categoryAlias<category>());
    const auto addTask = [threads, &taskNamer](const std::string_view subTask, void (*targetInstance)())
    { threads->enqueue(taskNamer(subTask), targetInstance); };

    std::cout << "\nInstances of the " << toString<category>() << " pattern:" << std::endl;
    for (const auto index :
         std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); }))
    {
        const auto& target = candidates.at(index);
        switch (utility::common::bkdrHash(target.c_str()))
        {
            using structural::StructuralPattern;
            static_assert(utility::common::isStatelessClass<StructuralPattern>());
            case abbrValue(StructuralInstance::adapter):
                addTask(target, &StructuralPattern::adapterInstance);
                break;
            case abbrValue(StructuralInstance::bridge):
                addTask(target, &StructuralPattern::bridgeInstance);
                break;
            case abbrValue(StructuralInstance::composite):
                addTask(target, &StructuralPattern::compositeInstance);
                break;
            case abbrValue(StructuralInstance::decorator):
                addTask(target, &StructuralPattern::decoratorInstance);
                break;
            case abbrValue(StructuralInstance::facade):
                addTask(target, &StructuralPattern::facadeInstance);
                break;
            case abbrValue(StructuralInstance::flyweight):
                addTask(target, &StructuralPattern::flyweightInstance);
                break;
            case abbrValue(StructuralInstance::proxy):
                addTask(target, &StructuralPattern::proxyInstance);
                break;
            default:
                throw std::logic_error{"Unknown " + std::string{toString<category>()} + " instance: " + target + '.'};
        }
    }

    pooling.deleteElement(threads);
    APP_DP_PRINT_TASK_END_TITLE(category);
}
} // namespace application::app_dp
