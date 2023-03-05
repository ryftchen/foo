//! @file apply_design_pattern.cpp
//! @author ryftchen
//! @brief The definitions (apply_design_pattern) in the application module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#include "apply_design_pattern.hpp"
#include <iomanip>
#include "application/core/include/command.hpp"
#include "application/core/include/log.hpp"
#include "utility/include/hash.hpp"

//! @brief Title of printing when design pattern tasks are beginning.
#define APP_DP_PRINT_TASK_BEGIN_TITLE(taskType)                                                                       \
    std::cout << "\r\n"                                                                                               \
              << "DESIGN PATTERN TASK: " << std::setiosflags(std::ios_base::left) << std::setfill('.')                \
              << std::setw(50) << taskType << "BEGIN" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') \
              << std::endl;                                                                                           \
    {
//! @brief Title of printing when design pattern tasks are ending.
#define APP_DP_PRINT_TASK_END_TITLE(taskType)                                                                       \
    }                                                                                                               \
    std::cout << "\r\n"                                                                                             \
              << "DESIGN PATTERN TASK: " << std::setiosflags(std::ios_base::left) << std::setfill('.')              \
              << std::setw(50) << taskType << "END" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') \
              << "\r\n"                                                                                             \
              << std::endl;

namespace application::app_dp
{
//! @brief Alias for Type.
using Type = DesignPatternTask::Type;
//! @brief Alias for Bottom.
//! @tparam T - type of specific enum
template <class T>
using Bottom = DesignPatternTask::Bottom<T>;
//! @brief Alias for BehavioralInstance.
using BehavioralInstance = DesignPatternTask::BehavioralInstance;
//! @brief Alias for CreationalInstance.
using CreationalInstance = DesignPatternTask::CreationalInstance;
//! @brief Alias for StructuralInstance.
using StructuralInstance = DesignPatternTask::StructuralInstance;

//! @brief Get the design pattern task.
//! @return reference of DesignPatternTask object
DesignPatternTask& getTask()
{
    static DesignPatternTask task;
    return task;
}

namespace behavioral
{
//! @brief Display behavioral result.
#define BEHAVIORAL_RESULT "\r\n*%-21s instance:\r\n%s"
//! @brief Print behavioral result content.
#define BEHAVIORAL_PRINT_RESULT_CONTENT(method) COMMON_PRINT(BEHAVIORAL_RESULT, method, output.str().c_str());

BehavioralPattern::BehavioralPattern()
{
    std::cout << "\r\nInstances of the behavioral pattern:" << std::endl;
}

void BehavioralPattern::chainOfResponsibilityInstance()
{
    try
    {
        const auto output = Behavioral::chainOfResponsibility();
        BEHAVIORAL_PRINT_RESULT_CONTENT("ChainOfResponsibility");
    }
    catch (const std::exception& error)
    {
        LOG_ERR(error.what());
    }
}

void BehavioralPattern::commandInstance()
{
    try
    {
        const auto output = Behavioral::command();
        BEHAVIORAL_PRINT_RESULT_CONTENT("Command");
    }
    catch (const std::exception& error)
    {
        LOG_ERR(error.what());
    }
}

void BehavioralPattern::interpreterInstance()
{
    try
    {
        const auto output = Behavioral::interpreter();
        BEHAVIORAL_PRINT_RESULT_CONTENT("Interpreter");
    }
    catch (const std::exception& error)
    {
        LOG_ERR(error.what());
    }
}

void BehavioralPattern::iteratorInstance()
{
    try
    {
        const auto output = Behavioral::iterator();
        BEHAVIORAL_PRINT_RESULT_CONTENT("Iterator");
    }
    catch (const std::exception& error)
    {
        LOG_ERR(error.what());
    }
}

void BehavioralPattern::mediatorInstance()
{
    try
    {
        const auto output = Behavioral::mediator();
        BEHAVIORAL_PRINT_RESULT_CONTENT("Mediator");
    }
    catch (const std::exception& error)
    {
        LOG_ERR(error.what());
    }
}

void BehavioralPattern::mementoInstance()
{
    try
    {
        const auto output = Behavioral::memento();
        BEHAVIORAL_PRINT_RESULT_CONTENT("Memento");
    }
    catch (const std::exception& error)
    {
        LOG_ERR(error.what());
    }
}

void BehavioralPattern::observerInstance()
{
    try
    {
        const auto output = Behavioral::observer();
        BEHAVIORAL_PRINT_RESULT_CONTENT("Observer");
    }
    catch (const std::exception& error)
    {
        LOG_ERR(error.what());
    }
}

void BehavioralPattern::stateInstance()
{
    try
    {
        const auto output = Behavioral::state();
        BEHAVIORAL_PRINT_RESULT_CONTENT("State");
    }
    catch (const std::exception& error)
    {
        LOG_ERR(error.what());
    }
}

void BehavioralPattern::strategyInstance()
{
    try
    {
        const auto output = Behavioral::strategy();
        BEHAVIORAL_PRINT_RESULT_CONTENT("Strategy");
    }
    catch (const std::exception& error)
    {
        LOG_ERR(error.what());
    }
}

void BehavioralPattern::templateMethodInstance()
{
    try
    {
        const auto output = Behavioral::templateMethod();
        BEHAVIORAL_PRINT_RESULT_CONTENT("TemplateMethod");
    }
    catch (const std::exception& error)
    {
        LOG_ERR(error.what());
    }
}

void BehavioralPattern::visitorInstance()
{
    try
    {
        const auto output = Behavioral::visitor();
        BEHAVIORAL_PRINT_RESULT_CONTENT("Visitor");
    }
    catch (const std::exception& error)
    {
        LOG_ERR(error.what());
    }
}
} // namespace behavioral

//! @brief Run behavioral tasks.
//! @param targets - vector of target instances
void runBehavioral(const std::vector<std::string>& targets)
{
    if (getBit<BehavioralInstance>().none())
    {
        return;
    }

    using behavioral::BehavioralPattern;
    using utility::hash::operator""_bkdrHash;

    APP_DP_PRINT_TASK_BEGIN_TITLE(Type::behavioral);
    auto* threads = command::getPoolForMultithreading().newElement(std::min(
        static_cast<uint32_t>(getBit<BehavioralInstance>().count()),
        static_cast<uint32_t>(Bottom<BehavioralInstance>::value)));

    const auto behavioralFunctor = [&](const std::string& threadName, void (*instancePtr)())
    {
        threads->enqueue(threadName, instancePtr);
    };

    for (uint8_t i = 0; i < Bottom<BehavioralInstance>::value; ++i)
    {
        if (!getBit<BehavioralInstance>().test(BehavioralInstance(i)))
        {
            continue;
        }

        const std::string targetInstance = targets.at(i), threadName = "b_" + targetInstance;
        switch (utility::hash::bkdrHash(targetInstance.data()))
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
                LOG_DBG("Execute to apply an unknown behavioral instance.");
                break;
        }
    }

    command::getPoolForMultithreading().deleteElement(threads);
    APP_DP_PRINT_TASK_END_TITLE(Type::behavioral);
}

//! @brief Update behavioral instances in tasks.
//! @param target - target instance
void updateBehavioralTask(const std::string& target)
{
    using utility::hash::operator""_bkdrHash;
    switch (utility::hash::bkdrHash(target.c_str()))
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
            throw std::runtime_error("Unexpected behavioral instance: " + target + ".");
    }
}

namespace creational
{
//! @brief Display creational result.
#define CREATIONAL_RESULT "\r\n*%-15s instance:\r\n%s"
//! @brief Print creational result content.
#define CREATIONAL_PRINT_RESULT_CONTENT(method) COMMON_PRINT(CREATIONAL_RESULT, method, output.str().c_str());

CreationalPattern::CreationalPattern()
{
    std::cout << "\r\nInstances of the creational pattern:" << std::endl;
}

void CreationalPattern::abstractFactoryInstance()
{
    try
    {
        const auto output = Creational::abstractFactory();
        CREATIONAL_PRINT_RESULT_CONTENT("AbstractFactory");
    }
    catch (const std::exception& error)
    {
        LOG_ERR(error.what());
    }
}

void CreationalPattern::builderInstance()
{
    try
    {
        const auto output = Creational::builder();
        CREATIONAL_PRINT_RESULT_CONTENT("Builder");
    }
    catch (const std::exception& error)
    {
        LOG_ERR(error.what());
    }
}

void CreationalPattern::factoryMethodInstance()
{
    try
    {
        const auto output = Creational::factoryMethod();
        CREATIONAL_PRINT_RESULT_CONTENT("FactoryMethod");
    }
    catch (const std::exception& error)
    {
        LOG_ERR(error.what());
    }
}

void CreationalPattern::prototypeInstance()
{
    try
    {
        const auto output = Creational::prototype();
        CREATIONAL_PRINT_RESULT_CONTENT("Prototype");
    }
    catch (const std::exception& error)
    {
        LOG_ERR(error.what());
    }
}

void CreationalPattern::singletonInstance()
{
    try
    {
        const auto output = Creational::singleton();
        CREATIONAL_PRINT_RESULT_CONTENT("Singleton");
    }
    catch (const std::exception& error)
    {
        LOG_ERR(error.what());
    }
}
} // namespace creational

//! @brief Run creational tasks.
//! @param targets - vector of target instances
void runCreational(const std::vector<std::string>& targets)
{
    if (getBit<CreationalInstance>().none())
    {
        return;
    }

    using creational::CreationalPattern;
    using utility::hash::operator""_bkdrHash;

    APP_DP_PRINT_TASK_BEGIN_TITLE(Type::creational);
    auto* threads = command::getPoolForMultithreading().newElement(std::min(
        static_cast<uint32_t>(getBit<CreationalInstance>().count()),
        static_cast<uint32_t>(Bottom<CreationalInstance>::value)));

    const auto creationalFunctor = [&](const std::string& threadName, void (*instancePtr)())
    {
        threads->enqueue(threadName, instancePtr);
    };

    for (uint8_t i = 0; i < Bottom<CreationalInstance>::value; ++i)
    {
        if (!getBit<CreationalInstance>().test(CreationalInstance(i)))
        {
            continue;
        }

        const std::string targetInstance = targets.at(i), threadName = "c_" + targetInstance;
        switch (utility::hash::bkdrHash(targetInstance.data()))
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
                LOG_DBG("Execute to apply an unknown creational instance.");
                break;
        }
    }

    command::getPoolForMultithreading().deleteElement(threads);
    APP_DP_PRINT_TASK_END_TITLE(Type::creational);
}

//! @brief Update creational instances in tasks.
//! @param target - target instance
void updateCreationalTask(const std::string& target)
{
    using utility::hash::operator""_bkdrHash;
    switch (utility::hash::bkdrHash(target.c_str()))
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
            throw std::runtime_error("Unexpected creational instance: " + target + ".");
    }
}

namespace structural
{
//! @brief Display structural result.
#define STRUCTURAL_RESULT "\r\n*%-9s instance:\r\n%s"
//! @brief Print structural result content.
#define STRUCTURAL_PRINT_RESULT_CONTENT(method) COMMON_PRINT(STRUCTURAL_RESULT, method, output.str().c_str());

StructuralPattern::StructuralPattern()
{
    std::cout << "\r\nInstances of the structural pattern:" << std::endl;
}

void StructuralPattern::adapterInstance()
{
    try
    {
        const auto output = Structural::adapter();
        STRUCTURAL_PRINT_RESULT_CONTENT("Adapter");
    }
    catch (const std::exception& error)
    {
        LOG_ERR(error.what());
    }
}

void StructuralPattern::bridgeInstance()
{
    try
    {
        const auto output = Structural::bridge();
        STRUCTURAL_PRINT_RESULT_CONTENT("Bridge");
    }
    catch (const std::exception& error)
    {
        LOG_ERR(error.what());
    }
}

void StructuralPattern::compositeInstance()
{
    try
    {
        const auto output = Structural::composite();
        STRUCTURAL_PRINT_RESULT_CONTENT("Composite");
    }
    catch (const std::exception& error)
    {
        LOG_ERR(error.what());
    }
}

void StructuralPattern::decoratorInstance()
{
    try
    {
        const auto output = Structural::decorator();
        STRUCTURAL_PRINT_RESULT_CONTENT("Decorator");
    }
    catch (const std::exception& error)
    {
        LOG_ERR(error.what());
    }
}

void StructuralPattern::facadeInstance()
{
    try
    {
        const auto output = Structural::facade();
        STRUCTURAL_PRINT_RESULT_CONTENT("Facade");
    }
    catch (const std::exception& error)
    {
        LOG_ERR(error.what());
    }
}

void StructuralPattern::flyweightInstance()
{
    try
    {
        const auto output = Structural::flyweight();
        STRUCTURAL_PRINT_RESULT_CONTENT("Flyweight");
    }
    catch (const std::exception& error)
    {
        LOG_ERR(error.what());
    }
}

void StructuralPattern::proxyInstance()
{
    try
    {
        const auto output = Structural::proxy();
        STRUCTURAL_PRINT_RESULT_CONTENT("Proxy");
    }
    catch (const std::exception& error)
    {
        LOG_ERR(error.what());
    }
}
} // namespace structural

//! @brief Run structural tasks.
//! @param targets - vector of target instances
void runStructural(const std::vector<std::string>& targets)
{
    if (getBit<StructuralInstance>().none())
    {
        return;
    }

    using structural::StructuralPattern;
    using utility::hash::operator""_bkdrHash;

    APP_DP_PRINT_TASK_BEGIN_TITLE(Type::structural);
    auto* threads = command::getPoolForMultithreading().newElement(std::min(
        static_cast<uint32_t>(getBit<StructuralInstance>().count()),
        static_cast<uint32_t>(Bottom<StructuralInstance>::value)));

    const auto structuralFunctor = [&](const std::string& threadName, void (*instancePtr)())
    {
        threads->enqueue(threadName, instancePtr);
    };

    for (uint8_t i = 0; i < Bottom<StructuralInstance>::value; ++i)
    {
        if (!getBit<StructuralInstance>().test(StructuralInstance(i)))
        {
            continue;
        }

        const std::string targetInstance = targets.at(i), threadName = "s_" + targetInstance;
        switch (utility::hash::bkdrHash(targetInstance.data()))
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
                LOG_DBG("Execute to apply an unknown structural instance.");
                break;
        }
    }

    command::getPoolForMultithreading().deleteElement(threads);
    APP_DP_PRINT_TASK_END_TITLE(Type::structural);
}

//! @brief Update structural instances in tasks.
//! @param target - target instance
void updateStructuralTask(const std::string& target)
{
    using utility::hash::operator""_bkdrHash;
    switch (utility::hash::bkdrHash(target.c_str()))
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
            throw std::runtime_error("Unexpected structural instance: " + target + ".");
    }
}
} // namespace application::app_dp
