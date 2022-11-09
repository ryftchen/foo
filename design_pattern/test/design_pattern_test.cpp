#include "design_pattern_test.hpp"
#include "behavioral.hpp"
#include "creational.hpp"
#include "structural.hpp"
#include "utility/include/hash.hpp"
#include "utility/include/log.hpp"
#include "utility/include/thread.hpp"

#define DESIGN_PATTERN_PRINT_TASK_BEGIN_TITLE(taskType)                                                               \
    std::cout << "\r\n"                                                                                               \
              << "DESIGN PATTERN TASK: " << std::setiosflags(std::ios_base::left) << std::setfill('.')                \
              << std::setw(50) << taskType << "BEGIN" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') \
              << std::endl;                                                                                           \
    {
#define DESIGN_PATTERN_PRINT_TASK_END_TITLE(taskType)                                                               \
    }                                                                                                               \
    std::cout << "\r\n"                                                                                             \
              << "DESIGN PATTERN TASK: " << std::setiosflags(std::ios_base::left) << std::setfill('.')              \
              << std::setw(50) << taskType << "END" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') \
              << "\r\n"                                                                                             \
              << std::endl;

namespace dp_tst
{
using util_hash::operator""_bkdrHash;
using Type = DesignPatternTask::Type;
template <class T>
using Bottom = DesignPatternTask::Bottom<T>;
using BehavioralInstance = DesignPatternTask::BehavioralInstance;
using CreationalInstance = DesignPatternTask::CreationalInstance;
using StructuralInstance = DesignPatternTask::StructuralInstance;

DesignPatternTask& getTask()
{
    static DesignPatternTask task;
    return task;
}

void runBehavioral(const std::vector<std::string>& targets)
{
    if (getBit<BehavioralInstance>().none())
    {
        return;
    }

    DESIGN_PATTERN_PRINT_TASK_BEGIN_TITLE(Type::behavioral);

    using dp_behavioral::BehavioralPattern;
    const std::shared_ptr<BehavioralPattern> behavioral = std::make_shared<BehavioralPattern>();
    std::shared_ptr<util_thread::Thread> threads = std::make_shared<util_thread::Thread>(std::min(
        static_cast<uint32_t>(getBit<BehavioralInstance>().count()),
        static_cast<uint32_t>(Bottom<BehavioralInstance>::value)));
    const auto behavioralFunctor = [&](const std::string& threadName, void (BehavioralPattern::*instancePtr)() const)
    {
        threads->enqueue(threadName, instancePtr, behavioral);
    };

    for (int i = 0; i < Bottom<BehavioralInstance>::value; ++i)
    {
        if (!getBit<BehavioralInstance>().test(BehavioralInstance(i)))
        {
            continue;
        }

        const std::string targetInstance = targets.at(i), threadName = "b_" + targetInstance;
        switch (util_hash::bkdrHash(targetInstance.data()))
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
                LOG_DBG("execute to run unknown behavioral instance.");
                break;
        }
    }

    DESIGN_PATTERN_PRINT_TASK_END_TITLE(Type::behavioral);
}

void updateBehavioralTask(const std::string& target)
{
    switch (util_hash::bkdrHash(target.c_str()))
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
            throw std::runtime_error("Unexpected task of behavioral: " + target);
    }
}

void runCreational(const std::vector<std::string>& targets)
{
    if (getBit<CreationalInstance>().none())
    {
        return;
    }

    DESIGN_PATTERN_PRINT_TASK_BEGIN_TITLE(Type::creational);

    using dp_creational::CreationalPattern;
    const std::shared_ptr<CreationalPattern> creational = std::make_shared<CreationalPattern>();
    std::shared_ptr<util_thread::Thread> threads = std::make_shared<util_thread::Thread>(std::min(
        static_cast<uint32_t>(getBit<CreationalInstance>().count()),
        static_cast<uint32_t>(Bottom<CreationalInstance>::value)));
    const auto creationalFunctor = [&](const std::string& threadName, void (CreationalPattern::*instancePtr)() const)
    {
        threads->enqueue(threadName, instancePtr, creational);
    };

    for (int i = 0; i < Bottom<CreationalInstance>::value; ++i)
    {
        if (!getBit<CreationalInstance>().test(CreationalInstance(i)))
        {
            continue;
        }

        const std::string targetInstance = targets.at(i), threadName = "c_" + targetInstance;
        switch (util_hash::bkdrHash(targetInstance.data()))
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
                LOG_DBG("execute to run unknown creational instance.");
                break;
        }
    }

    DESIGN_PATTERN_PRINT_TASK_END_TITLE(Type::creational);
}

void updateCreationalTask(const std::string& target)
{
    switch (util_hash::bkdrHash(target.c_str()))
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
            throw std::runtime_error("Unexpected task of creational: " + target);
    }
}

void runStructural(const std::vector<std::string>& targets)
{
    if (getBit<StructuralInstance>().none())
    {
        return;
    }

    DESIGN_PATTERN_PRINT_TASK_BEGIN_TITLE(Type::structural);

    using dp_structural::StructuralPattern;
    const std::shared_ptr<StructuralPattern> structural = std::make_shared<StructuralPattern>();
    std::shared_ptr<util_thread::Thread> threads = std::make_shared<util_thread::Thread>(std::min(
        static_cast<uint32_t>(getBit<StructuralInstance>().count()),
        static_cast<uint32_t>(Bottom<StructuralInstance>::value)));
    const auto structuralFunctor = [&](const std::string& threadName, void (StructuralPattern::*instancePtr)() const)
    {
        threads->enqueue(threadName, instancePtr, structural);
    };

    for (int i = 0; i < Bottom<StructuralInstance>::value; ++i)
    {
        if (!getBit<StructuralInstance>().test(StructuralInstance(i)))
        {
            continue;
        }

        const std::string targetInstance = targets.at(i), threadName = "s_" + targetInstance;
        switch (util_hash::bkdrHash(targetInstance.data()))
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
                LOG_DBG("execute to run unknown structural instance.");
                break;
        }
    }

    DESIGN_PATTERN_PRINT_TASK_END_TITLE(Type::structural);
}

void updateStructuralTask(const std::string& target)
{
    switch (util_hash::bkdrHash(target.c_str()))
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
            throw std::runtime_error("Unexpected task of structural: " + target);
    }
}
} // namespace dp_tst
