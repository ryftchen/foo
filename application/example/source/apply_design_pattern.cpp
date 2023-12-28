//! @file apply_design_pattern.cpp
//! @author ryftchen
//! @brief The definitions (apply_design_pattern) in the application module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen. All rights reserved.

#include "apply_design_pattern.hpp"

#ifndef __PRECOMPILED_HEADER
#include <iomanip>
#include <syncstream>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

#include "application/core/include/command.hpp"
#include "application/core/include/log.hpp"

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
{
    try
    {
        const auto output = Behavioral().chainOfResponsibility();
        BEHAVIORAL_PRINT_RESULT_CONTENT(BehavioralInstance::chainOfResponsibility);
    }
    catch (const std::exception& error)
    {
        LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
    }
}

void BehavioralPattern::commandInstance()
{
    try
    {
        const auto output = Behavioral().command();
        BEHAVIORAL_PRINT_RESULT_CONTENT(BehavioralInstance::command);
    }
    catch (const std::exception& error)
    {
        LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
    }
}

void BehavioralPattern::interpreterInstance()
{
    try
    {
        const auto output = Behavioral().interpreter();
        BEHAVIORAL_PRINT_RESULT_CONTENT(BehavioralInstance::interpreter);
    }
    catch (const std::exception& error)
    {
        LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
    }
}

void BehavioralPattern::iteratorInstance()
{
    try
    {
        const auto output = Behavioral().iterator();
        BEHAVIORAL_PRINT_RESULT_CONTENT(BehavioralInstance::iterator);
    }
    catch (const std::exception& error)
    {
        LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
    }
}

void BehavioralPattern::mediatorInstance()
{
    try
    {
        const auto output = Behavioral().mediator();
        BEHAVIORAL_PRINT_RESULT_CONTENT(BehavioralInstance::mediator);
    }
    catch (const std::exception& error)
    {
        LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
    }
}

void BehavioralPattern::mementoInstance()
{
    try
    {
        const auto output = Behavioral().memento();
        BEHAVIORAL_PRINT_RESULT_CONTENT(BehavioralInstance::memento);
    }
    catch (const std::exception& error)
    {
        LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
    }
}

void BehavioralPattern::observerInstance()
{
    try
    {
        const auto output = Behavioral().observer();
        BEHAVIORAL_PRINT_RESULT_CONTENT(BehavioralInstance::observer);
    }
    catch (const std::exception& error)
    {
        LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
    }
}

void BehavioralPattern::stateInstance()
{
    try
    {
        const auto output = Behavioral().state();
        BEHAVIORAL_PRINT_RESULT_CONTENT(BehavioralInstance::state);
    }
    catch (const std::exception& error)
    {
        LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
    }
}

void BehavioralPattern::strategyInstance()
{
    try
    {
        const auto output = Behavioral().strategy();
        BEHAVIORAL_PRINT_RESULT_CONTENT(BehavioralInstance::strategy);
    }
    catch (const std::exception& error)
    {
        LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
    }
}

void BehavioralPattern::templateMethodInstance()
{
    try
    {
        const auto output = Behavioral().templateMethod();
        BEHAVIORAL_PRINT_RESULT_CONTENT(BehavioralInstance::templateMethod);
    }
    catch (const std::exception& error)
    {
        LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
    }
}

void BehavioralPattern::visitorInstance()
{
    try
    {
        const auto output = Behavioral().visitor();
        BEHAVIORAL_PRINT_RESULT_CONTENT(BehavioralInstance::visitor);
    }
    catch (const std::exception& error)
    {
        LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
    }
}

#undef BEHAVIORAL_RESULT
#undef BEHAVIORAL_PRINT_RESULT_CONTENT
#undef BEHAVIORAL_INSTANCE_TABLE
} // namespace behavioral

//! @brief Run behavioral tasks.
//! @param targets - container of target instances
void runBehavioralTasks(const std::vector<std::string>& targets)
{
    if (getBit<BehavioralInstance>().none())
    {
        return;
    }

    using behavioral::BehavioralPattern;
    using utility::common::operator""_bkdrHash;

    APP_DP_PRINT_TASK_BEGIN_TITLE(Category::behavioral);
    auto* const threads = command::getPublicThreadPool().newElement(std::min(
        static_cast<std::uint32_t>(getBit<BehavioralInstance>().count()),
        static_cast<std::uint32_t>(Bottom<BehavioralInstance>::value)));

    const auto behavioralFunctor = [threads](const std::string& threadName, void (*instancePtr)())
    {
        threads->enqueue(threadName, instancePtr);
    };

    std::cout << "\r\nInstances of the behavioral pattern:" << std::endl;
    for (std::uint8_t i = 0; i < Bottom<BehavioralInstance>::value; ++i)
    {
        if (!getBit<BehavioralInstance>().test(BehavioralInstance(i)))
        {
            continue;
        }

        const std::string targetInstance = targets.at(i), threadName = "task-dp_b_" + targetInstance;
        switch (utility::common::bkdrHash(targetInstance.data()))
        {
            case "cha"_bkdrHash:
                behavioralFunctor(threadName, &BehavioralPattern::chainOfResponsibilityInstance);
                break;
            case "com"_bkdrHash:
                behavioralFunctor(threadName, &BehavioralPattern::commandInstance);
                break;
            case "int"_bkdrHash:
                behavioralFunctor(threadName, &BehavioralPattern::interpreterInstance);
                break;
            case "ite"_bkdrHash:
                behavioralFunctor(threadName, &BehavioralPattern::iteratorInstance);
                break;
            case "med"_bkdrHash:
                behavioralFunctor(threadName, &BehavioralPattern::mediatorInstance);
                break;
            case "mem"_bkdrHash:
                behavioralFunctor(threadName, &BehavioralPattern::mementoInstance);
                break;
            case "obs"_bkdrHash:
                behavioralFunctor(threadName, &BehavioralPattern::observerInstance);
                break;
            case "sta"_bkdrHash:
                behavioralFunctor(threadName, &BehavioralPattern::stateInstance);
                break;
            case "str"_bkdrHash:
                behavioralFunctor(threadName, &BehavioralPattern::strategyInstance);
                break;
            case "tem"_bkdrHash:
                behavioralFunctor(threadName, &BehavioralPattern::templateMethodInstance);
                break;
            case "vis"_bkdrHash:
                behavioralFunctor(threadName, &BehavioralPattern::visitorInstance);
                break;
            default:
                LOG_INF << "Execute to apply an unknown behavioral instance.";
                break;
        }
    }

    command::getPublicThreadPool().deleteElement(threads);
    APP_DP_PRINT_TASK_END_TITLE(Category::behavioral);
}

//! @brief Update behavioral instances in tasks.
//! @param target - target instance
void updateBehavioralTask(const std::string& target)
{
    using utility::common::operator""_bkdrHash;
    switch (utility::common::bkdrHash(target.c_str()))
    {
        case "cha"_bkdrHash:
            setBit<BehavioralInstance>(BehavioralInstance::chainOfResponsibility);
            break;
        case "com"_bkdrHash:
            setBit<BehavioralInstance>(BehavioralInstance::command);
            break;
        case "int"_bkdrHash:
            setBit<BehavioralInstance>(BehavioralInstance::interpreter);
            break;
        case "ite"_bkdrHash:
            setBit<BehavioralInstance>(BehavioralInstance::iterator);
            break;
        case "med"_bkdrHash:
            setBit<BehavioralInstance>(BehavioralInstance::mediator);
            break;
        case "mem"_bkdrHash:
            setBit<BehavioralInstance>(BehavioralInstance::memento);
            break;
        case "obs"_bkdrHash:
            setBit<BehavioralInstance>(BehavioralInstance::observer);
            break;
        case "sta"_bkdrHash:
            setBit<BehavioralInstance>(BehavioralInstance::state);
            break;
        case "str"_bkdrHash:
            setBit<BehavioralInstance>(BehavioralInstance::strategy);
            break;
        case "tem"_bkdrHash:
            setBit<BehavioralInstance>(BehavioralInstance::templateMethod);
            break;
        case "vis"_bkdrHash:
            setBit<BehavioralInstance>(BehavioralInstance::visitor);
            break;
        default:
            getBit<BehavioralInstance>().reset();
            throw std::runtime_error("Unexpected behavioral instance: " + target + '.');
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
{
    try
    {
        const auto output = Creational().abstractFactory();
        CREATIONAL_PRINT_RESULT_CONTENT(CreationalInstance::abstractFactory);
    }
    catch (const std::exception& error)
    {
        LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
    }
}

void CreationalPattern::builderInstance()
{
    try
    {
        const auto output = Creational().builder();
        CREATIONAL_PRINT_RESULT_CONTENT(CreationalInstance::builder);
    }
    catch (const std::exception& error)
    {
        LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
    }
}

void CreationalPattern::factoryMethodInstance()
{
    try
    {
        const auto output = Creational().factoryMethod();
        CREATIONAL_PRINT_RESULT_CONTENT(CreationalInstance::factoryMethod);
    }
    catch (const std::exception& error)
    {
        LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
    }
}

void CreationalPattern::prototypeInstance()
{
    try
    {
        const auto output = Creational().prototype();
        CREATIONAL_PRINT_RESULT_CONTENT(CreationalInstance::prototype);
    }
    catch (const std::exception& error)
    {
        LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
    }
}

void CreationalPattern::singletonInstance()
{
    try
    {
        const auto output = Creational().singleton();
        CREATIONAL_PRINT_RESULT_CONTENT(CreationalInstance::singleton);
    }
    catch (const std::exception& error)
    {
        LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
    }
}

#undef CREATIONAL_RESULT
#undef CREATIONAL_PRINT_RESULT_CONTENT
#undef CREATIONAL_INSTANCE_TABLE
} // namespace creational

//! @brief Run creational tasks.
//! @param targets - container of target instances
void runCreationalTasks(const std::vector<std::string>& targets)
{
    if (getBit<CreationalInstance>().none())
    {
        return;
    }

    using creational::CreationalPattern;
    using utility::common::operator""_bkdrHash;

    APP_DP_PRINT_TASK_BEGIN_TITLE(Category::creational);
    auto* const threads = command::getPublicThreadPool().newElement(std::min(
        static_cast<std::uint32_t>(getBit<CreationalInstance>().count()),
        static_cast<std::uint32_t>(Bottom<CreationalInstance>::value)));

    const auto creationalFunctor = [threads](const std::string& threadName, void (*instancePtr)())
    {
        threads->enqueue(threadName, instancePtr);
    };

    std::cout << "\r\nInstances of the creational pattern:" << std::endl;
    for (std::uint8_t i = 0; i < Bottom<CreationalInstance>::value; ++i)
    {
        if (!getBit<CreationalInstance>().test(CreationalInstance(i)))
        {
            continue;
        }

        const std::string targetInstance = targets.at(i), threadName = "task-dp_c_" + targetInstance;
        switch (utility::common::bkdrHash(targetInstance.data()))
        {
            case "abs"_bkdrHash:
                creationalFunctor(threadName, &CreationalPattern::abstractFactoryInstance);
                break;
            case "bui"_bkdrHash:
                creationalFunctor(threadName, &CreationalPattern::builderInstance);
                break;
            case "fac"_bkdrHash:
                creationalFunctor(threadName, &CreationalPattern::factoryMethodInstance);
                break;
            case "pro"_bkdrHash:
                creationalFunctor(threadName, &CreationalPattern::prototypeInstance);
                break;
            case "sin"_bkdrHash:
                creationalFunctor(threadName, &CreationalPattern::singletonInstance);
                break;
            default:
                LOG_INF << "Execute to apply an unknown creational instance.";
                break;
        }
    }

    command::getPublicThreadPool().deleteElement(threads);
    APP_DP_PRINT_TASK_END_TITLE(Category::creational);
}

//! @brief Update creational instances in tasks.
//! @param target - target instance
void updateCreationalTask(const std::string& target)
{
    using utility::common::operator""_bkdrHash;
    switch (utility::common::bkdrHash(target.c_str()))
    {
        case "abs"_bkdrHash:
            setBit<CreationalInstance>(CreationalInstance::abstractFactory);
            break;
        case "bui"_bkdrHash:
            setBit<CreationalInstance>(CreationalInstance::builder);
            break;
        case "fac"_bkdrHash:
            setBit<CreationalInstance>(CreationalInstance::factoryMethod);
            break;
        case "pro"_bkdrHash:
            setBit<CreationalInstance>(CreationalInstance::prototype);
            break;
        case "sin"_bkdrHash:
            setBit<CreationalInstance>(CreationalInstance::singleton);
            break;
        default:
            getBit<CreationalInstance>().reset();
            throw std::runtime_error("Unexpected creational instance: " + target + '.');
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
{
    try
    {
        const auto output = Structural().adapter();
        STRUCTURAL_PRINT_RESULT_CONTENT(StructuralInstance::adapter);
    }
    catch (const std::exception& error)
    {
        LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
    }
}

void StructuralPattern::bridgeInstance()
{
    try
    {
        const auto output = Structural().bridge();
        STRUCTURAL_PRINT_RESULT_CONTENT(StructuralInstance::bridge);
    }
    catch (const std::exception& error)
    {
        LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
    }
}

void StructuralPattern::compositeInstance()
{
    try
    {
        const auto output = Structural().composite();
        STRUCTURAL_PRINT_RESULT_CONTENT(StructuralInstance::composite);
    }
    catch (const std::exception& error)
    {
        LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
    }
}

void StructuralPattern::decoratorInstance()
{
    try
    {
        const auto output = Structural().decorator();
        STRUCTURAL_PRINT_RESULT_CONTENT(StructuralInstance::decorator);
    }
    catch (const std::exception& error)
    {
        LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
    }
}

void StructuralPattern::facadeInstance()
{
    try
    {
        const auto output = Structural().facade();
        STRUCTURAL_PRINT_RESULT_CONTENT(StructuralInstance::facade);
    }
    catch (const std::exception& error)
    {
        LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
    }
}

void StructuralPattern::flyweightInstance()
{
    try
    {
        const auto output = Structural().flyweight();
        STRUCTURAL_PRINT_RESULT_CONTENT(StructuralInstance::flyweight);
    }
    catch (const std::exception& error)
    {
        LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
    }
}

void StructuralPattern::proxyInstance()
{
    try
    {
        const auto output = Structural().proxy();
        STRUCTURAL_PRINT_RESULT_CONTENT(StructuralInstance::proxy);
    }
    catch (const std::exception& error)
    {
        LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
    }
}

#undef STRUCTURAL_RESULT
#undef STRUCTURAL_PRINT_RESULT_CONTENT
#undef STRUCTURAL_INSTANCE_TABLE
} // namespace structural

//! @brief Run structural tasks.
//! @param targets - container of target instances
void runStructuralTasks(const std::vector<std::string>& targets)
{
    if (getBit<StructuralInstance>().none())
    {
        return;
    }

    using structural::StructuralPattern;
    using utility::common::operator""_bkdrHash;

    APP_DP_PRINT_TASK_BEGIN_TITLE(Category::structural);
    auto* const threads = command::getPublicThreadPool().newElement(std::min(
        static_cast<std::uint32_t>(getBit<StructuralInstance>().count()),
        static_cast<std::uint32_t>(Bottom<StructuralInstance>::value)));

    const auto structuralFunctor = [threads](const std::string& threadName, void (*instancePtr)())
    {
        threads->enqueue(threadName, instancePtr);
    };

    std::cout << "\r\nInstances of the structural pattern:" << std::endl;
    for (std::uint8_t i = 0; i < Bottom<StructuralInstance>::value; ++i)
    {
        if (!getBit<StructuralInstance>().test(StructuralInstance(i)))
        {
            continue;
        }

        const std::string targetInstance = targets.at(i), threadName = "task-dp_s_" + targetInstance;
        switch (utility::common::bkdrHash(targetInstance.data()))
        {
            case "ada"_bkdrHash:
                structuralFunctor(threadName, &StructuralPattern::adapterInstance);
                break;
            case "bri"_bkdrHash:
                structuralFunctor(threadName, &StructuralPattern::bridgeInstance);
                break;
            case "com"_bkdrHash:
                structuralFunctor(threadName, &StructuralPattern::compositeInstance);
                break;
            case "dec"_bkdrHash:
                structuralFunctor(threadName, &StructuralPattern::decoratorInstance);
                break;
            case "fac"_bkdrHash:
                structuralFunctor(threadName, &StructuralPattern::facadeInstance);
                break;
            case "fly"_bkdrHash:
                structuralFunctor(threadName, &StructuralPattern::flyweightInstance);
                break;
            case "pro"_bkdrHash:
                structuralFunctor(threadName, &StructuralPattern::proxyInstance);
                break;
            default:
                LOG_INF << "Execute to apply an unknown structural instance.";
                break;
        }
    }

    command::getPublicThreadPool().deleteElement(threads);
    APP_DP_PRINT_TASK_END_TITLE(Category::structural);
}

//! @brief Update structural instances in tasks.
//! @param target - target instance
void updateStructuralTask(const std::string& target)
{
    using utility::common::operator""_bkdrHash;
    switch (utility::common::bkdrHash(target.c_str()))
    {
        case "ada"_bkdrHash:
            setBit<StructuralInstance>(StructuralInstance::adapter);
            break;
        case "bri"_bkdrHash:
            setBit<StructuralInstance>(StructuralInstance::bridge);
            break;
        case "com"_bkdrHash:
            setBit<StructuralInstance>(StructuralInstance::composite);
            break;
        case "dec"_bkdrHash:
            setBit<StructuralInstance>(StructuralInstance::decorator);
            break;
        case "fac"_bkdrHash:
            setBit<StructuralInstance>(StructuralInstance::facade);
            break;
        case "fly"_bkdrHash:
            setBit<StructuralInstance>(StructuralInstance::flyweight);
            break;
        case "pro"_bkdrHash:
            setBit<StructuralInstance>(StructuralInstance::proxy);
            break;
        default:
            getBit<StructuralInstance>().reset();
            throw std::runtime_error("Unexpected structural instance: " + target + '.');
    }
}
} // namespace application::app_dp
