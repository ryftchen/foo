//! @file apply_design_pattern.cpp
//! @author ryftchen
//! @brief The definitions (apply_design_pattern) in the application module.
//! @version 1.0
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
//! @brief Get the bit flags of the instance (category) in design pattern tasks.
#define APP_DP_GET_BIT(category)                                                                                 \
    std::invoke(                                                                                                 \
        utility::reflection::TypeInfo<DesignPatternTask>::fields.find(REFLECTION_STR(toString(category))).value, \
        getTask())
//! @brief Get the alias of the instance (category) in design pattern tasks.
#define APP_DP_GET_ALIAS(category)                                                                            \
    ({                                                                                                        \
        constexpr auto attr =                                                                                 \
            utility::reflection::TypeInfo<DesignPatternTask>::fields.find(REFLECTION_STR(toString(category))) \
                .attrs.find(REFLECTION_STR("alias"));                                                         \
        static_assert(attr.hasValue);                                                                         \
        attr.value;                                                                                           \
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

//! @brief Mapping table for enum and string about design pattern tasks. X macro.
#define CATEGORY_TABLE             \
    ELEM(behavioral, "behavioral") \
    ELEM(creational, "creational") \
    ELEM(structural, "structural")

//! @brief Convert category enumeration to string.
//! @param cat - the specific value of Category enum
//! @return category name
constexpr std::string_view toString(const Category cat)
{
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {CATEGORY_TABLE};
#undef ELEM
    return table[cat];
}

namespace behavioral
{
//! @brief Display behavioral result.
#define BEHAVIORAL_RESULT "\r\n==> %-21s Instance <==\n%s"
//! @brief Print behavioral result content.
#define BEHAVIORAL_PRINT_RESULT_CONTENT(instance) \
    COMMON_PRINT(BEHAVIORAL_RESULT, toString(instance).data(), output.str().c_str());
//! @brief Mapping table for enum and string about behavioral instances. X macro.
#define BEHAVIORAL_INSTANCE_TABLE                        \
    ELEM(chainOfResponsibility, "ChainOfResponsibility") \
    ELEM(command, "Command")                             \
    ELEM(interpreter, "Interpreter")                     \
    ELEM(iterator, "Iterator")                           \
    ELEM(mediator, "Mediator")                           \
    ELEM(memento, "Memento")                             \
    ELEM(observer, "Observer")                           \
    ELEM(state, "State")                                 \
    ELEM(strategy, "Strategy")                           \
    ELEM(templateMethod, "TemplateMethod")               \
    ELEM(visitor, "Visitor")

//! @brief Convert instance enumeration to string.
//! @param instance - the specific value of BehavioralInstance enum
//! @return instance name
constexpr std::string_view toString(const BehavioralInstance instance)
{
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {BEHAVIORAL_INSTANCE_TABLE};
#undef ELEM
    return table[instance];
}

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
#undef BEHAVIORAL_INSTANCE_TABLE
} // namespace behavioral

//! @brief Run behavioral tasks.
//! @param targets - container of target instances
void runBehavioralTasks(const std::vector<std::string>& targets)
{
    constexpr auto category = Category::behavioral;
    const auto& bit = APP_DP_GET_BIT(category);
    if (bit.none())
    {
        return;
    }

    APP_DP_PRINT_TASK_BEGIN_TITLE(category);
    using behavioral::BehavioralPattern;
    using utility::common::operator""_bkdrHash;

    auto& pooling = command::getPublicThreadPool();
    auto* const threads = pooling.newElement(std::min(
        static_cast<std::uint32_t>(bit.count()), static_cast<std::uint32_t>(Bottom<BehavioralInstance>::value)));
    const auto behavioralFunctor = [threads](const std::string& threadName, void (*instancePtr)())
    {
        threads->enqueue(threadName, instancePtr);
    };
    const auto name = utility::currying::curry(getTaskNameCurried(), APP_DP_GET_ALIAS(category));

    std::cout << "\r\nInstances of the " << toString(category) << " pattern:" << std::endl;
    for (std::uint8_t i = 0; i < Bottom<BehavioralInstance>::value; ++i)
    {
        if (!bit.test(BehavioralInstance(i)))
        {
            continue;
        }

        const std::string targetInstance = targets.at(i);
        switch (utility::common::bkdrHash(targetInstance.data()))
        {
            case "cha"_bkdrHash:
                behavioralFunctor(name(targetInstance), &BehavioralPattern::chainOfResponsibilityInstance);
                break;
            case "com"_bkdrHash:
                behavioralFunctor(name(targetInstance), &BehavioralPattern::commandInstance);
                break;
            case "int"_bkdrHash:
                behavioralFunctor(name(targetInstance), &BehavioralPattern::interpreterInstance);
                break;
            case "ite"_bkdrHash:
                behavioralFunctor(name(targetInstance), &BehavioralPattern::iteratorInstance);
                break;
            case "med"_bkdrHash:
                behavioralFunctor(name(targetInstance), &BehavioralPattern::mediatorInstance);
                break;
            case "mem"_bkdrHash:
                behavioralFunctor(name(targetInstance), &BehavioralPattern::mementoInstance);
                break;
            case "obs"_bkdrHash:
                behavioralFunctor(name(targetInstance), &BehavioralPattern::observerInstance);
                break;
            case "sta"_bkdrHash:
                behavioralFunctor(name(targetInstance), &BehavioralPattern::stateInstance);
                break;
            case "str"_bkdrHash:
                behavioralFunctor(name(targetInstance), &BehavioralPattern::strategyInstance);
                break;
            case "tem"_bkdrHash:
                behavioralFunctor(name(targetInstance), &BehavioralPattern::templateMethodInstance);
                break;
            case "vis"_bkdrHash:
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
    auto& bit = APP_DP_GET_BIT(category);

    using utility::common::operator""_bkdrHash;
    switch (utility::common::bkdrHash(target.c_str()))
    {
        case "cha"_bkdrHash:
            bit.set(BehavioralInstance::chainOfResponsibility);
            break;
        case "com"_bkdrHash:
            bit.set(BehavioralInstance::command);
            break;
        case "int"_bkdrHash:
            bit.set(BehavioralInstance::interpreter);
            break;
        case "ite"_bkdrHash:
            bit.set(BehavioralInstance::iterator);
            break;
        case "med"_bkdrHash:
            bit.set(BehavioralInstance::mediator);
            break;
        case "mem"_bkdrHash:
            bit.set(BehavioralInstance::memento);
            break;
        case "obs"_bkdrHash:
            bit.set(BehavioralInstance::observer);
            break;
        case "sta"_bkdrHash:
            bit.set(BehavioralInstance::state);
            break;
        case "str"_bkdrHash:
            bit.set(BehavioralInstance::strategy);
            break;
        case "tem"_bkdrHash:
            bit.set(BehavioralInstance::templateMethod);
            break;
        case "vis"_bkdrHash:
            bit.set(BehavioralInstance::visitor);
            break;
        default:
            bit.reset();
            throw std::runtime_error("Unexpected " + std::string{toString(category)} + " instance: " + target + '.');
    }
}

namespace creational
{
//! @brief Display creational result.
#define CREATIONAL_RESULT "\r\n==> %-15s Instance <==\n%s"
//! @brief Print creational result content.
#define CREATIONAL_PRINT_RESULT_CONTENT(instance) \
    COMMON_PRINT(CREATIONAL_RESULT, toString(instance).data(), output.str().c_str());
//! @brief Mapping table for enum and string about creational instances. X macro.
#define CREATIONAL_INSTANCE_TABLE            \
    ELEM(abstractFactory, "AbstractFactory") \
    ELEM(builder, "Builder")                 \
    ELEM(factoryMethod, "FactoryMethod")     \
    ELEM(prototype, "Prototype")             \
    ELEM(singleton, "Singleton")

//! @brief Convert instance enumeration to string.
//! @param instance - the specific value of CreationalInstance enum
//! @return instance name
constexpr std::string_view toString(const CreationalInstance instance)
{
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {CREATIONAL_INSTANCE_TABLE};
#undef ELEM
    return table[instance];
}

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
#undef CREATIONAL_INSTANCE_TABLE
} // namespace creational

//! @brief Run creational tasks.
//! @param targets - container of target instances
void runCreationalTasks(const std::vector<std::string>& targets)
{
    constexpr auto category = Category::creational;
    const auto& bit = APP_DP_GET_BIT(category);
    if (bit.none())
    {
        return;
    }

    APP_DP_PRINT_TASK_BEGIN_TITLE(category);
    using creational::CreationalPattern;
    using utility::common::operator""_bkdrHash;

    auto& pooling = command::getPublicThreadPool();
    auto* const threads = pooling.newElement(std::min(
        static_cast<std::uint32_t>(bit.count()), static_cast<std::uint32_t>(Bottom<CreationalInstance>::value)));
    const auto creationalFunctor = [threads](const std::string& threadName, void (*instancePtr)())
    {
        threads->enqueue(threadName, instancePtr);
    };
    const auto name = utility::currying::curry(getTaskNameCurried(), APP_DP_GET_ALIAS(category));

    std::cout << "\r\nInstances of the " << toString(category) << " pattern:" << std::endl;
    for (std::uint8_t i = 0; i < Bottom<CreationalInstance>::value; ++i)
    {
        if (!bit.test(CreationalInstance(i)))
        {
            continue;
        }

        const std::string targetInstance = targets.at(i);
        switch (utility::common::bkdrHash(targetInstance.data()))
        {
            case "abs"_bkdrHash:
                creationalFunctor(name(targetInstance), &CreationalPattern::abstractFactoryInstance);
                break;
            case "bui"_bkdrHash:
                creationalFunctor(name(targetInstance), &CreationalPattern::builderInstance);
                break;
            case "fac"_bkdrHash:
                creationalFunctor(name(targetInstance), &CreationalPattern::factoryMethodInstance);
                break;
            case "pro"_bkdrHash:
                creationalFunctor(name(targetInstance), &CreationalPattern::prototypeInstance);
                break;
            case "sin"_bkdrHash:
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
    auto& bit = APP_DP_GET_BIT(category);

    using utility::common::operator""_bkdrHash;
    switch (utility::common::bkdrHash(target.c_str()))
    {
        case "abs"_bkdrHash:
            bit.set(CreationalInstance::abstractFactory);
            break;
        case "bui"_bkdrHash:
            bit.set(CreationalInstance::builder);
            break;
        case "fac"_bkdrHash:
            bit.set(CreationalInstance::factoryMethod);
            break;
        case "pro"_bkdrHash:
            bit.set(CreationalInstance::prototype);
            break;
        case "sin"_bkdrHash:
            bit.set(CreationalInstance::singleton);
            break;
        default:
            bit.reset();
            throw std::runtime_error("Unexpected " + std::string{toString(category)} + " instance: " + target + '.');
    }
}

namespace structural
{
//! @brief Display structural result.
#define STRUCTURAL_RESULT "\r\n==> %-9s Instance <==\n%s"
//! @brief Print structural result content.
#define STRUCTURAL_PRINT_RESULT_CONTENT(instance) \
    COMMON_PRINT(STRUCTURAL_RESULT, toString(instance).data(), output.str().c_str());
//! @brief Mapping table for enum and string about structural instances. X macro.
#define STRUCTURAL_INSTANCE_TABLE \
    ELEM(adapter, "Adapter")      \
    ELEM(bridge, "Bridge")        \
    ELEM(composite, "Composite")  \
    ELEM(decorator, "Decorator")  \
    ELEM(facade, "Facade")        \
    ELEM(flyweight, "Flyweight")  \
    ELEM(proxy, "Proxy")

//! @brief Convert instance enumeration to string.
//! @param instance - the specific value of StructuralInstance enum
//! @return instance name
constexpr std::string_view toString(const StructuralInstance instance)
{
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {STRUCTURAL_INSTANCE_TABLE};
#undef ELEM
    return table[instance];
}

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
#undef STRUCTURAL_INSTANCE_TABLE
} // namespace structural

//! @brief Run structural tasks.
//! @param targets - container of target instances
void runStructuralTasks(const std::vector<std::string>& targets)
{
    constexpr auto category = Category::structural;
    const auto& bit = APP_DP_GET_BIT(category);
    if (bit.none())
    {
        return;
    }

    APP_DP_PRINT_TASK_BEGIN_TITLE(category);
    using structural::StructuralPattern;
    using utility::common::operator""_bkdrHash;

    auto& pooling = command::getPublicThreadPool();
    auto* const threads = pooling.newElement(std::min(
        static_cast<std::uint32_t>(bit.count()), static_cast<std::uint32_t>(Bottom<StructuralInstance>::value)));
    const auto structuralFunctor = [threads](const std::string& threadName, void (*instancePtr)())
    {
        threads->enqueue(threadName, instancePtr);
    };
    const auto name = utility::currying::curry(getTaskNameCurried(), APP_DP_GET_ALIAS(category));

    std::cout << "\r\nInstances of the " << toString(category) << " pattern:" << std::endl;
    for (std::uint8_t i = 0; i < Bottom<StructuralInstance>::value; ++i)
    {
        if (!bit.test(StructuralInstance(i)))
        {
            continue;
        }

        const std::string targetInstance = targets.at(i);
        switch (utility::common::bkdrHash(targetInstance.data()))
        {
            case "ada"_bkdrHash:
                structuralFunctor(name(targetInstance), &StructuralPattern::adapterInstance);
                break;
            case "bri"_bkdrHash:
                structuralFunctor(name(targetInstance), &StructuralPattern::bridgeInstance);
                break;
            case "com"_bkdrHash:
                structuralFunctor(name(targetInstance), &StructuralPattern::compositeInstance);
                break;
            case "dec"_bkdrHash:
                structuralFunctor(name(targetInstance), &StructuralPattern::decoratorInstance);
                break;
            case "fac"_bkdrHash:
                structuralFunctor(name(targetInstance), &StructuralPattern::facadeInstance);
                break;
            case "fly"_bkdrHash:
                structuralFunctor(name(targetInstance), &StructuralPattern::flyweightInstance);
                break;
            case "pro"_bkdrHash:
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
    auto& bit = APP_DP_GET_BIT(category);

    using utility::common::operator""_bkdrHash;
    switch (utility::common::bkdrHash(target.c_str()))
    {
        case "ada"_bkdrHash:
            bit.set(StructuralInstance::adapter);
            break;
        case "bri"_bkdrHash:
            bit.set(StructuralInstance::bridge);
            break;
        case "com"_bkdrHash:
            bit.set(StructuralInstance::composite);
            break;
        case "dec"_bkdrHash:
            bit.set(StructuralInstance::decorator);
            break;
        case "fac"_bkdrHash:
            bit.set(StructuralInstance::facade);
            break;
        case "fly"_bkdrHash:
            bit.set(StructuralInstance::flyweight);
            break;
        case "pro"_bkdrHash:
            bit.set(StructuralInstance::proxy);
            break;
        default:
            bit.reset();
            throw std::runtime_error("Unexpected " + std::string{toString(category)} + " instance: " + target + '.');
    }
}

#undef CATEGORY_TABLE
} // namespace application::app_dp
