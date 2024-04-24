//! @file apply_design_pattern.cpp
//! @author ryftchen
//! @brief The definitions (apply_design_pattern) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#include "apply_design_pattern.hpp"

#ifndef __PRECOMPILED_HEADER
#include <iomanip>
#include <syncstream>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

#include "application/core/include/command.hpp"
#include "application/core/include/log.hpp"
#include "utility/include/currying.hpp"

//! @brief Title of printing when design pattern tasks are beginning.
#define APP_DP_PRINT_TASK_BEGIN_TITLE(category)                                                               \
    std::osyncstream(std::cout) << "\r\n"                                                                     \
                                << "DESIGN PATTERN TASK: " << std::setiosflags(std::ios_base::left)           \
                                << std::setfill('.') << std::setw(50) << category << "BEGIN"                  \
                                << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl; \
    {
//! @brief Title of printing when design pattern tasks are ending.
#define APP_DP_PRINT_TASK_END_TITLE(category)                                                           \
    }                                                                                                   \
    std::osyncstream(std::cout) << "\r\n"                                                               \
                                << "DESIGN PATTERN TASK: " << std::setiosflags(std::ios_base::left)     \
                                << std::setfill('.') << std::setw(50) << category << "END"              \
                                << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << '\n' \
                                << std::endl;
//! @brief Get the title of a particular instance in design pattern tasks.
#define APP_DP_GET_INSTANCE_TITLE(instance)                  \
    ({                                                       \
        std::string title = std::string{toString(instance)}; \
        title.at(0) = std::toupper(title.at(0));             \
        title;                                               \
    })

namespace application::app_dp
{
//! @brief Alias for Category.
using Category = DesignPatternTask::Category;

//! @brief Get the design pattern task.
//! @return reference of the DesignPatternTask object
DesignPatternTask& getTask()
{
    static DesignPatternTask task{};
    return task;
}

//! @brief Get the task name curried.
//! @return task name curried
static const auto& getTaskNameCurried()
{
    static const auto curried =
        utility::currying::curry(command::presetTaskName, utility::reflection::TypeInfo<DesignPatternTask>::name);
    return curried;
}

//! @brief Convert category enumeration to string.
//! @param cat - the specific value of Category enum
//! @return category name
constexpr std::string_view toString(const Category cat)
{
    switch (cat)
    {
        case Category::behavioral:
            return utility::reflection::TypeInfo<BehavioralInstance>::name;
        case Category::creational:
            return utility::reflection::TypeInfo<CreationalInstance>::name;
        case Category::structural:
            return utility::reflection::TypeInfo<StructuralInstance>::name;
        default:
            return "";
    }
}

//! @brief Get the bit flags of the category in design pattern tasks.
//! @tparam Cat - the specific value of Category enum
//! @return reference of the category bit flags
template <Category Cat>
constexpr auto& getCategoryBit()
{
    return std::invoke(
        utility::reflection::TypeInfo<DesignPatternTask>::fields.find(REFLECTION_STR(toString(Cat))).value, getTask());
}

//! @brief Get the alias of the category in design pattern tasks.
//! @tparam Cat - the specific value of Category enum
//! @return alias of the category name
template <Category Cat>
constexpr std::string_view getCategoryAlias()
{
    constexpr auto attr = utility::reflection::TypeInfo<DesignPatternTask>::fields.find(REFLECTION_STR(toString(Cat)))
                              .attrs.find(REFLECTION_STR("alias"));
    static_assert(attr.hasValue);
    return attr.value;
}

//! @brief Abbreviation value for the target instance.
//! @tparam T - type of target instance
//! @param instance - target instance
//! @return abbreviation value
template <class T>
consteval std::size_t abbrVal(const T instance)
{
    using TypeInfo = utility::reflection::TypeInfo<T>;
    static_assert(TypeInfo::fields.size == Bottom<T>::value);

    std::size_t value = 0;
    TypeInfo::fields.forEach(
        [instance, &value](auto field)
        {
            if (field.name == toString(instance))
            {
                static_assert(1 == field.attrs.size);
                auto attr = field.attrs.find(REFLECTION_STR("task"));
                static_assert(attr.hasValue);
                value = utility::common::operator""_bkdrHash(attr.value, 0);
            }
        });
    return value;
}

//! @brief Mapping table for enum and string about behavioral instances. X macro.
#define APP_DP_BEHAVIORAL_INSTANCE_TABLE                 \
    ELEM(chainOfResponsibility, "chainOfResponsibility") \
    ELEM(command, "command")                             \
    ELEM(interpreter, "interpreter")                     \
    ELEM(iterator, "iterator")                           \
    ELEM(mediator, "mediator")                           \
    ELEM(memento, "memento")                             \
    ELEM(observer, "observer")                           \
    ELEM(state, "state")                                 \
    ELEM(strategy, "strategy")                           \
    ELEM(templateMethod, "templateMethod")               \
    ELEM(visitor, "visitor")
//! @brief Convert instance enumeration to string.
//! @param instance - the specific value of BehavioralInstance enum
//! @return instance name
constexpr std::string_view toString(const BehavioralInstance instance)
{
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {APP_DP_BEHAVIORAL_INSTANCE_TABLE};
#undef ELEM
    return table[instance];
}
#undef APP_DP_BEHAVIORAL_INSTANCE_TABLE

//! @brief Mapping table for enum and string about creational instances. X macro.
#define APP_DP_CREATIONAL_INSTANCE_TABLE     \
    ELEM(abstractFactory, "abstractFactory") \
    ELEM(builder, "builder")                 \
    ELEM(factoryMethod, "factoryMethod")     \
    ELEM(prototype, "prototype")             \
    ELEM(singleton, "singleton")
//! @brief Convert instance enumeration to string.
//! @param instance - the specific value of CreationalInstance enum
//! @return instance name
constexpr std::string_view toString(const CreationalInstance instance)
{
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {APP_DP_CREATIONAL_INSTANCE_TABLE};
#undef ELEM
    return table[instance];
}
#undef APP_DP_CREATIONAL_INSTANCE_TABLE

//! @brief Mapping table for enum and string about structural instances. X macro.
#define APP_DP_STRUCTURAL_INSTANCE_TABLE \
    ELEM(adapter, "adapter")             \
    ELEM(bridge, "bridge")               \
    ELEM(composite, "composite")         \
    ELEM(decorator, "decorator")         \
    ELEM(facade, "facade")               \
    ELEM(flyweight, "flyweight")         \
    ELEM(proxy, "proxy")
//! @brief Convert instance enumeration to string.
//! @param instance - the specific value of StructuralInstance enum
//! @return instance name
constexpr std::string_view toString(const StructuralInstance instance)
{
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {APP_DP_STRUCTURAL_INSTANCE_TABLE};
#undef ELEM
    return table[instance];
}
#undef APP_DP_STRUCTURAL_INSTANCE_TABLE

namespace behavioral
{
//! @brief Display behavioral result.
#define BEHAVIORAL_RESULT "\r\n==> %-21s Instance <==\n%s"
//! @brief Print behavioral result content.
#define BEHAVIORAL_PRINT_RESULT_CONTENT(instance) \
    COMMON_PRINT(BEHAVIORAL_RESULT, APP_DP_GET_INSTANCE_TITLE(instance).data(), output.str().c_str())

void BehavioralPattern::chainOfResponsibilityInstance()
try
{
    const auto output = Behavioral().chainOfResponsibility();
    BEHAVIORAL_PRINT_RESULT_CONTENT(BehavioralInstance::chainOfResponsibility);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void BehavioralPattern::commandInstance()
try
{
    const auto output = Behavioral().command();
    BEHAVIORAL_PRINT_RESULT_CONTENT(BehavioralInstance::command);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void BehavioralPattern::interpreterInstance()
try
{
    const auto output = Behavioral().interpreter();
    BEHAVIORAL_PRINT_RESULT_CONTENT(BehavioralInstance::interpreter);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void BehavioralPattern::iteratorInstance()
try
{
    const auto output = Behavioral().iterator();
    BEHAVIORAL_PRINT_RESULT_CONTENT(BehavioralInstance::iterator);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void BehavioralPattern::mediatorInstance()
try
{
    const auto output = Behavioral().mediator();
    BEHAVIORAL_PRINT_RESULT_CONTENT(BehavioralInstance::mediator);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void BehavioralPattern::mementoInstance()
try
{
    const auto output = Behavioral().memento();
    BEHAVIORAL_PRINT_RESULT_CONTENT(BehavioralInstance::memento);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void BehavioralPattern::observerInstance()
try
{
    const auto output = Behavioral().observer();
    BEHAVIORAL_PRINT_RESULT_CONTENT(BehavioralInstance::observer);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void BehavioralPattern::stateInstance()
try
{
    const auto output = Behavioral().state();
    BEHAVIORAL_PRINT_RESULT_CONTENT(BehavioralInstance::state);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void BehavioralPattern::strategyInstance()
try
{
    const auto output = Behavioral().strategy();
    BEHAVIORAL_PRINT_RESULT_CONTENT(BehavioralInstance::strategy);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void BehavioralPattern::templateMethodInstance()
try
{
    const auto output = Behavioral().templateMethod();
    BEHAVIORAL_PRINT_RESULT_CONTENT(BehavioralInstance::templateMethod);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void BehavioralPattern::visitorInstance()
try
{
    const auto output = Behavioral().visitor();
    BEHAVIORAL_PRINT_RESULT_CONTENT(BehavioralInstance::visitor);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

#undef BEHAVIORAL_RESULT
#undef BEHAVIORAL_PRINT_RESULT_CONTENT
} // namespace behavioral

//! @brief Run behavioral tasks.
//! @param candidates - container for the candidate target instances
void runBehavioralTasks(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::behavioral;
    const auto& bitFlag = getCategoryBit<category>();
    if (bitFlag.none())
    {
        return;
    }

    APP_DP_PRINT_TASK_BEGIN_TITLE(category);
    using behavioral::BehavioralPattern;

    auto& pooling = command::getPublicThreadPool();
    auto* const threads = pooling.newElement(std::min(
        static_cast<std::uint32_t>(bitFlag.count()), static_cast<std::uint32_t>(Bottom<BehavioralInstance>::value)));
    const auto behavioralFunctor = [threads](const std::string& threadName, void (*instancePtr)())
    {
        threads->enqueue(threadName, instancePtr);
    };
    const auto name = utility::currying::curry(getTaskNameCurried(), getCategoryAlias<category>());

    std::cout << "\r\nInstances of the " << toString(category) << " pattern:" << std::endl;
    for (std::uint8_t i = 0; i < Bottom<BehavioralInstance>::value; ++i)
    {
        if (!bitFlag.test(BehavioralInstance(i)))
        {
            continue;
        }

        const std::string targetInstance = candidates.at(i);
        switch (utility::common::bkdrHash(targetInstance.data()))
        {
            case abbrVal(BehavioralInstance::chainOfResponsibility):
                behavioralFunctor(name(targetInstance), &BehavioralPattern::chainOfResponsibilityInstance);
                break;
            case abbrVal(BehavioralInstance::command):
                behavioralFunctor(name(targetInstance), &BehavioralPattern::commandInstance);
                break;
            case abbrVal(BehavioralInstance::interpreter):
                behavioralFunctor(name(targetInstance), &BehavioralPattern::interpreterInstance);
                break;
            case abbrVal(BehavioralInstance::iterator):
                behavioralFunctor(name(targetInstance), &BehavioralPattern::iteratorInstance);
                break;
            case abbrVal(BehavioralInstance::mediator):
                behavioralFunctor(name(targetInstance), &BehavioralPattern::mediatorInstance);
                break;
            case abbrVal(BehavioralInstance::memento):
                behavioralFunctor(name(targetInstance), &BehavioralPattern::mementoInstance);
                break;
            case abbrVal(BehavioralInstance::observer):
                behavioralFunctor(name(targetInstance), &BehavioralPattern::observerInstance);
                break;
            case abbrVal(BehavioralInstance::state):
                behavioralFunctor(name(targetInstance), &BehavioralPattern::stateInstance);
                break;
            case abbrVal(BehavioralInstance::strategy):
                behavioralFunctor(name(targetInstance), &BehavioralPattern::strategyInstance);
                break;
            case abbrVal(BehavioralInstance::templateMethod):
                behavioralFunctor(name(targetInstance), &BehavioralPattern::templateMethodInstance);
                break;
            case abbrVal(BehavioralInstance::visitor):
                behavioralFunctor(name(targetInstance), &BehavioralPattern::visitorInstance);
                break;
            default:
                LOG_INF << "Execute to apply an unknown " << toString(category) << " instance.";
                break;
        }
    }

    pooling.deleteElement(threads);
    APP_DP_PRINT_TASK_END_TITLE(category);
}

//! @brief Update behavioral instances in tasks.
//! @param target - target instance
void updateBehavioralTask(const std::string& target)
{
    constexpr auto category = Category::behavioral;
    auto& bitFlag = getCategoryBit<category>();

    switch (utility::common::bkdrHash(target.c_str()))
    {
        case abbrVal(BehavioralInstance::chainOfResponsibility):
            bitFlag.set(BehavioralInstance::chainOfResponsibility);
            break;
        case abbrVal(BehavioralInstance::command):
            bitFlag.set(BehavioralInstance::command);
            break;
        case abbrVal(BehavioralInstance::interpreter):
            bitFlag.set(BehavioralInstance::interpreter);
            break;
        case abbrVal(BehavioralInstance::iterator):
            bitFlag.set(BehavioralInstance::iterator);
            break;
        case abbrVal(BehavioralInstance::mediator):
            bitFlag.set(BehavioralInstance::mediator);
            break;
        case abbrVal(BehavioralInstance::memento):
            bitFlag.set(BehavioralInstance::memento);
            break;
        case abbrVal(BehavioralInstance::observer):
            bitFlag.set(BehavioralInstance::observer);
            break;
        case abbrVal(BehavioralInstance::state):
            bitFlag.set(BehavioralInstance::state);
            break;
        case abbrVal(BehavioralInstance::strategy):
            bitFlag.set(BehavioralInstance::strategy);
            break;
        case abbrVal(BehavioralInstance::templateMethod):
            bitFlag.set(BehavioralInstance::templateMethod);
            break;
        case abbrVal(BehavioralInstance::visitor):
            bitFlag.set(BehavioralInstance::visitor);
            break;
        default:
            bitFlag.reset();
            throw std::runtime_error("Unexpected " + std::string{toString(category)} + " instance: " + target + '.');
    }
}

namespace creational
{
//! @brief Display creational result.
#define CREATIONAL_RESULT "\r\n==> %-15s Instance <==\n%s"
//! @brief Print creational result content.
#define CREATIONAL_PRINT_RESULT_CONTENT(instance) \
    COMMON_PRINT(CREATIONAL_RESULT, APP_DP_GET_INSTANCE_TITLE(instance).data(), output.str().c_str())

void CreationalPattern::abstractFactoryInstance()
try
{
    const auto output = Creational().abstractFactory();
    CREATIONAL_PRINT_RESULT_CONTENT(CreationalInstance::abstractFactory);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void CreationalPattern::builderInstance()
try
{
    const auto output = Creational().builder();
    CREATIONAL_PRINT_RESULT_CONTENT(CreationalInstance::builder);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void CreationalPattern::factoryMethodInstance()
try
{
    const auto output = Creational().factoryMethod();
    CREATIONAL_PRINT_RESULT_CONTENT(CreationalInstance::factoryMethod);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void CreationalPattern::prototypeInstance()
try
{
    const auto output = Creational().prototype();
    CREATIONAL_PRINT_RESULT_CONTENT(CreationalInstance::prototype);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void CreationalPattern::singletonInstance()
try
{
    const auto output = Creational().singleton();
    CREATIONAL_PRINT_RESULT_CONTENT(CreationalInstance::singleton);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

#undef CREATIONAL_RESULT
#undef CREATIONAL_PRINT_RESULT_CONTENT
} // namespace creational

//! @brief Run creational tasks.
//! @param candidates - container for the candidate target instances
void runCreationalTasks(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::creational;
    const auto& bitFlag = getCategoryBit<category>();
    if (bitFlag.none())
    {
        return;
    }

    APP_DP_PRINT_TASK_BEGIN_TITLE(category);
    using creational::CreationalPattern;

    auto& pooling = command::getPublicThreadPool();
    auto* const threads = pooling.newElement(std::min(
        static_cast<std::uint32_t>(bitFlag.count()), static_cast<std::uint32_t>(Bottom<CreationalInstance>::value)));
    const auto creationalFunctor = [threads](const std::string& threadName, void (*instancePtr)())
    {
        threads->enqueue(threadName, instancePtr);
    };
    const auto name = utility::currying::curry(getTaskNameCurried(), getCategoryAlias<category>());

    std::cout << "\r\nInstances of the " << toString(category) << " pattern:" << std::endl;
    for (std::uint8_t i = 0; i < Bottom<CreationalInstance>::value; ++i)
    {
        if (!bitFlag.test(CreationalInstance(i)))
        {
            continue;
        }

        const std::string targetInstance = candidates.at(i);
        switch (utility::common::bkdrHash(targetInstance.data()))
        {
            case abbrVal(CreationalInstance::abstractFactory):
                creationalFunctor(name(targetInstance), &CreationalPattern::abstractFactoryInstance);
                break;
            case abbrVal(CreationalInstance::builder):
                creationalFunctor(name(targetInstance), &CreationalPattern::builderInstance);
                break;
            case abbrVal(CreationalInstance::factoryMethod):
                creationalFunctor(name(targetInstance), &CreationalPattern::factoryMethodInstance);
                break;
            case abbrVal(CreationalInstance::prototype):
                creationalFunctor(name(targetInstance), &CreationalPattern::prototypeInstance);
                break;
            case abbrVal(CreationalInstance::singleton):
                creationalFunctor(name(targetInstance), &CreationalPattern::singletonInstance);
                break;
            default:
                LOG_INF << "Execute to apply an unknown " << toString(category) << " instance.";
                break;
        }
    }

    pooling.deleteElement(threads);
    APP_DP_PRINT_TASK_END_TITLE(category);
}

//! @brief Update creational instances in tasks.
//! @param target - target instance
void updateCreationalTask(const std::string& target)
{
    constexpr auto category = Category::creational;
    auto& bitFlag = getCategoryBit<category>();

    switch (utility::common::bkdrHash(target.c_str()))
    {
        case abbrVal(CreationalInstance::abstractFactory):
            bitFlag.set(CreationalInstance::abstractFactory);
            break;
        case abbrVal(CreationalInstance::builder):
            bitFlag.set(CreationalInstance::builder);
            break;
        case abbrVal(CreationalInstance::factoryMethod):
            bitFlag.set(CreationalInstance::factoryMethod);
            break;
        case abbrVal(CreationalInstance::prototype):
            bitFlag.set(CreationalInstance::prototype);
            break;
        case abbrVal(CreationalInstance::singleton):
            bitFlag.set(CreationalInstance::singleton);
            break;
        default:
            bitFlag.reset();
            throw std::runtime_error("Unexpected " + std::string{toString(category)} + " instance: " + target + '.');
    }
}

namespace structural
{
//! @brief Display structural result.
#define STRUCTURAL_RESULT "\r\n==> %-9s Instance <==\n%s"
//! @brief Print structural result content.
#define STRUCTURAL_PRINT_RESULT_CONTENT(instance) \
    COMMON_PRINT(STRUCTURAL_RESULT, APP_DP_GET_INSTANCE_TITLE(instance).data(), output.str().c_str())

void StructuralPattern::adapterInstance()
try
{
    const auto output = Structural().adapter();
    STRUCTURAL_PRINT_RESULT_CONTENT(StructuralInstance::adapter);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void StructuralPattern::bridgeInstance()
try
{
    const auto output = Structural().bridge();
    STRUCTURAL_PRINT_RESULT_CONTENT(StructuralInstance::bridge);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void StructuralPattern::compositeInstance()
try
{
    const auto output = Structural().composite();
    STRUCTURAL_PRINT_RESULT_CONTENT(StructuralInstance::composite);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void StructuralPattern::decoratorInstance()
try
{
    const auto output = Structural().decorator();
    STRUCTURAL_PRINT_RESULT_CONTENT(StructuralInstance::decorator);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void StructuralPattern::facadeInstance()
try
{
    const auto output = Structural().facade();
    STRUCTURAL_PRINT_RESULT_CONTENT(StructuralInstance::facade);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void StructuralPattern::flyweightInstance()
try
{
    const auto output = Structural().flyweight();
    STRUCTURAL_PRINT_RESULT_CONTENT(StructuralInstance::flyweight);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void StructuralPattern::proxyInstance()
try
{
    const auto output = Structural().proxy();
    STRUCTURAL_PRINT_RESULT_CONTENT(StructuralInstance::proxy);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

#undef STRUCTURAL_RESULT
#undef STRUCTURAL_PRINT_RESULT_CONTENT
} // namespace structural

//! @brief Run structural tasks.
//! @param candidates - container for the candidate target instances
void runStructuralTasks(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::structural;
    const auto& bitFlag = getCategoryBit<category>();
    if (bitFlag.none())
    {
        return;
    }

    APP_DP_PRINT_TASK_BEGIN_TITLE(category);
    using structural::StructuralPattern;

    auto& pooling = command::getPublicThreadPool();
    auto* const threads = pooling.newElement(std::min(
        static_cast<std::uint32_t>(bitFlag.count()), static_cast<std::uint32_t>(Bottom<StructuralInstance>::value)));
    const auto structuralFunctor = [threads](const std::string& threadName, void (*instancePtr)())
    {
        threads->enqueue(threadName, instancePtr);
    };
    const auto name = utility::currying::curry(getTaskNameCurried(), getCategoryAlias<category>());

    std::cout << "\r\nInstances of the " << toString(category) << " pattern:" << std::endl;
    for (std::uint8_t i = 0; i < Bottom<StructuralInstance>::value; ++i)
    {
        if (!bitFlag.test(StructuralInstance(i)))
        {
            continue;
        }

        const std::string targetInstance = candidates.at(i);
        switch (utility::common::bkdrHash(targetInstance.data()))
        {
            case abbrVal(StructuralInstance::adapter):
                structuralFunctor(name(targetInstance), &StructuralPattern::adapterInstance);
                break;
            case abbrVal(StructuralInstance::bridge):
                structuralFunctor(name(targetInstance), &StructuralPattern::bridgeInstance);
                break;
            case abbrVal(StructuralInstance::composite):
                structuralFunctor(name(targetInstance), &StructuralPattern::compositeInstance);
                break;
            case abbrVal(StructuralInstance::decorator):
                structuralFunctor(name(targetInstance), &StructuralPattern::decoratorInstance);
                break;
            case abbrVal(StructuralInstance::facade):
                structuralFunctor(name(targetInstance), &StructuralPattern::facadeInstance);
                break;
            case abbrVal(StructuralInstance::flyweight):
                structuralFunctor(name(targetInstance), &StructuralPattern::flyweightInstance);
                break;
            case abbrVal(StructuralInstance::proxy):
                structuralFunctor(name(targetInstance), &StructuralPattern::proxyInstance);
                break;
            default:
                LOG_INF << "Execute to apply an unknown " << toString(category) << " instance.";
                break;
        }
    }

    pooling.deleteElement(threads);
    APP_DP_PRINT_TASK_END_TITLE(category);
}

//! @brief Update structural instances in tasks.
//! @param target - target instance
void updateStructuralTask(const std::string& target)
{
    constexpr auto category = Category::structural;
    auto& bitFlag = getCategoryBit<category>();

    switch (utility::common::bkdrHash(target.c_str()))
    {
        case abbrVal(StructuralInstance::adapter):
            bitFlag.set(StructuralInstance::adapter);
            break;
        case abbrVal(StructuralInstance::bridge):
            bitFlag.set(StructuralInstance::bridge);
            break;
        case abbrVal(StructuralInstance::composite):
            bitFlag.set(StructuralInstance::composite);
            break;
        case abbrVal(StructuralInstance::decorator):
            bitFlag.set(StructuralInstance::decorator);
            break;
        case abbrVal(StructuralInstance::facade):
            bitFlag.set(StructuralInstance::facade);
            break;
        case abbrVal(StructuralInstance::flyweight):
            bitFlag.set(StructuralInstance::flyweight);
            break;
        case abbrVal(StructuralInstance::proxy):
            bitFlag.set(StructuralInstance::proxy);
            break;
        default:
            bitFlag.reset();
            throw std::runtime_error("Unexpected " + std::string{toString(category)} + " instance: " + target + '.');
    }
}

#undef CATEGORY_TABLE
} // namespace application::app_dp
