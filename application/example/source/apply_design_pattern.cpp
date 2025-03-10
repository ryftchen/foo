//! @file apply_design_pattern.cpp
//! @author ryftchen
//! @brief The definitions (apply_design_pattern) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "apply_design_pattern.hpp"

#ifndef __PRECOMPILED_HEADER
#include <cassert>
#include <iomanip>
#include <ranges>
#include <syncstream>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

#include "application/core/include/action.hpp"
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
//! @brief Alias for the type information.
//! @tparam T - type of target object
template <typename T>
using TypeInfo = utility::reflection::TypeInfo<T>;
//! @brief Alias for Category.
using Category = ApplyDesignPattern::Category;

//! @brief Get the design pattern choice manager.
//! @return reference of the ApplyDesignPattern object
ApplyDesignPattern& manager()
{
    static ApplyDesignPattern manager{};
    return manager;
}

//! @brief Get the task name curried.
//! @return task name curried
static const auto& taskNameCurried()
{
    static const auto curried = utility::currying::curry(action::presetTaskName, TypeInfo<ApplyDesignPattern>::name);
    return curried;
}

//! @brief Convert category enumeration to string.
//! @tparam Cat - the specific value of Category enum
//! @return category name
template <Category Cat>
consteval std::string_view toString()
{
    switch (Cat)
    {
        case Category::behavioral:
            return TypeInfo<BehavioralInstance>::name;
        case Category::creational:
            return TypeInfo<CreationalInstance>::name;
        case Category::structural:
            return TypeInfo<StructuralInstance>::name;
        default:
            break;
    }

    return {};
}

//! @brief Get the bit flags of the category in design pattern choices.
//! @tparam Cat - the specific value of Category enum
//! @return reference of the category bit flags
template <Category Cat>
constexpr auto& getCategoryOpts()
{
    return std::invoke(TypeInfo<ApplyDesignPattern>::fields.find(REFLECTION_STR(toString<Cat>())).value, manager());
}

//! @brief Get the alias of the category in design pattern choices.
//! @tparam Cat - the specific value of Category enum
//! @return alias of the category name
template <Category Cat>
consteval std::string_view getCategoryAlias()
{
    constexpr auto attr =
        TypeInfo<ApplyDesignPattern>::fields.find(REFLECTION_STR(toString<Cat>())).attrs.find(REFLECTION_STR("alias"));
    static_assert(attr.hasValue);
    return attr.value;
}

//! @brief Abbreviation value for the target instance.
//! @tparam T - type of target instance
//! @param instance - target instance
//! @return abbreviation value
template <typename T>
consteval std::size_t abbrVal(const T instance)
{
    static_assert(Bottom<T>::value == TypeInfo<T>::fields.size);
    std::size_t value = 0;
    TypeInfo<T>::fields.forEach(
        [instance, &value](const auto field)
        {
            if (field.name == toString(instance))
            {
                static_assert(1 == field.attrs.size);
                const auto attr = field.attrs.find(REFLECTION_STR("choice"));
                static_assert(attr.hasValue);
                value = utility::common::operator""_bkdrHash(attr.value);
            }
        });

    return value;
}

//! @brief Get the title of a particular instance in design pattern choices.
//! @tparam T - type of target instance
//! @param instance - target instance
//! @return initial capitalized title
template <typename T>
std::string getTitle(const T instance)
{
    std::string title(toString(instance));
    title.at(0) = std::toupper(title.at(0));

    return title;
}

// clang-format off
//! @brief Mapping table for enum and string about behavioral instances. X macro.
#define APP_DP_BEHAVIORAL_INSTANCE_TABLE                 \
    ELEM(chainOfResponsibility, "chainOfResponsibility") \
    ELEM(command              , "command"              ) \
    ELEM(interpreter          , "interpreter"          ) \
    ELEM(iterator             , "iterator"             ) \
    ELEM(mediator             , "mediator"             ) \
    ELEM(memento              , "memento"              ) \
    ELEM(observer             , "observer"             ) \
    ELEM(state                , "state"                ) \
    ELEM(strategy             , "strategy"             ) \
    ELEM(templateMethod       , "templateMethod"       ) \
    ELEM(visitor              , "visitor"              )
// clang-format on
//! @brief Convert instance enumeration to string.
//! @param instance - the specific value of BehavioralInstance enum
//! @return instance name
constexpr std::string_view toString(const BehavioralInstance instance)
{
//! @cond
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {APP_DP_BEHAVIORAL_INSTANCE_TABLE};
    static_assert((sizeof(table) / sizeof(table[0])) == Bottom<BehavioralInstance>::value);
    return table[instance];
//! @endcond
#undef ELEM
}
#undef APP_DP_BEHAVIORAL_INSTANCE_TABLE

// clang-format off
//! @brief Mapping table for enum and string about creational instances. X macro.
#define APP_DP_CREATIONAL_INSTANCE_TABLE     \
    ELEM(abstractFactory, "abstractFactory") \
    ELEM(builder        , "builder"        ) \
    ELEM(factoryMethod  , "factoryMethod"  ) \
    ELEM(prototype      , "prototype"      ) \
    ELEM(singleton      , "singleton"      )
// clang-format on
//! @brief Convert instance enumeration to string.
//! @param instance - the specific value of CreationalInstance enum
//! @return instance name
constexpr std::string_view toString(const CreationalInstance instance)
{
//! @cond
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {APP_DP_CREATIONAL_INSTANCE_TABLE};
    static_assert((sizeof(table) / sizeof(table[0])) == Bottom<CreationalInstance>::value);
    return table[instance];
//! @endcond
#undef ELEM
}
#undef APP_DP_CREATIONAL_INSTANCE_TABLE

// clang-format off
//! @brief Mapping table for enum and string about structural instances. X macro.
#define APP_DP_STRUCTURAL_INSTANCE_TABLE \
    ELEM(adapter  , "adapter"  )         \
    ELEM(bridge   , "bridge"   )         \
    ELEM(composite, "composite")         \
    ELEM(decorator, "decorator")         \
    ELEM(facade   , "facade"   )         \
    ELEM(flyweight, "flyweight")         \
    ELEM(proxy    , "proxy"    )
// clang-format on
//! @brief Convert instance enumeration to string.
//! @param instance - the specific value of StructuralInstance enum
//! @return instance name
constexpr std::string_view toString(const StructuralInstance instance)
{
//! @cond
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {APP_DP_STRUCTURAL_INSTANCE_TABLE};
    static_assert((sizeof(table) / sizeof(table[0])) == Bottom<StructuralInstance>::value);
    return table[instance];
//! @endcond
#undef ELEM
}
#undef APP_DP_STRUCTURAL_INSTANCE_TABLE

namespace behavioral
{
//! @brief Show the contents of the behavioral result.
//! @param instance - the specific value of BehavioralInstance enum
//! @param result - behavioral result
static void showResult(const BehavioralInstance instance, const std::string_view result)
{
    std::printf("\n==> %-21s Instance <==\n%s", getTitle(instance).c_str(), result.data());
}

void BehavioralPattern::chainOfResponsibilityInstance()
try
{
    const auto output = Behavioral().chainOfResponsibility();
    showResult(BehavioralInstance::chainOfResponsibility, output.str());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void BehavioralPattern::commandInstance()
try
{
    const auto output = Behavioral().command();
    showResult(BehavioralInstance::command, output.str());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void BehavioralPattern::interpreterInstance()
try
{
    const auto output = Behavioral().interpreter();
    showResult(BehavioralInstance::interpreter, output.str());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void BehavioralPattern::iteratorInstance()
try
{
    const auto output = Behavioral().iterator();
    showResult(BehavioralInstance::iterator, output.str());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void BehavioralPattern::mediatorInstance()
try
{
    const auto output = Behavioral().mediator();
    showResult(BehavioralInstance::mediator, output.str());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void BehavioralPattern::mementoInstance()
try
{
    const auto output = Behavioral().memento();
    showResult(BehavioralInstance::memento, output.str());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void BehavioralPattern::observerInstance()
try
{
    const auto output = Behavioral().observer();
    showResult(BehavioralInstance::observer, output.str());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void BehavioralPattern::stateInstance()
try
{
    const auto output = Behavioral().state();
    showResult(BehavioralInstance::state, output.str());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void BehavioralPattern::strategyInstance()
try
{
    const auto output = Behavioral().strategy();
    showResult(BehavioralInstance::strategy, output.str());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void BehavioralPattern::templateMethodInstance()
try
{
    const auto output = Behavioral().templateMethod();
    showResult(BehavioralInstance::templateMethod, output.str());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void BehavioralPattern::visitorInstance()
try
{
    const auto output = Behavioral().visitor();
    showResult(BehavioralInstance::visitor, output.str());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}
} // namespace behavioral

//! @brief Update behavioral-related choice.
//! @param target - target instance
template <>
void updateChoice<BehavioralInstance>(const std::string_view target)
{
    constexpr auto category = Category::behavioral;
    auto& bits = getCategoryOpts<category>();

    switch (utility::common::bkdrHash(target.data()))
    {
        case abbrVal(BehavioralInstance::chainOfResponsibility):
            bits.set(BehavioralInstance::chainOfResponsibility);
            break;
        case abbrVal(BehavioralInstance::command):
            bits.set(BehavioralInstance::command);
            break;
        case abbrVal(BehavioralInstance::interpreter):
            bits.set(BehavioralInstance::interpreter);
            break;
        case abbrVal(BehavioralInstance::iterator):
            bits.set(BehavioralInstance::iterator);
            break;
        case abbrVal(BehavioralInstance::mediator):
            bits.set(BehavioralInstance::mediator);
            break;
        case abbrVal(BehavioralInstance::memento):
            bits.set(BehavioralInstance::memento);
            break;
        case abbrVal(BehavioralInstance::observer):
            bits.set(BehavioralInstance::observer);
            break;
        case abbrVal(BehavioralInstance::state):
            bits.set(BehavioralInstance::state);
            break;
        case abbrVal(BehavioralInstance::strategy):
            bits.set(BehavioralInstance::strategy);
            break;
        case abbrVal(BehavioralInstance::templateMethod):
            bits.set(BehavioralInstance::templateMethod);
            break;
        case abbrVal(BehavioralInstance::visitor):
            bits.set(BehavioralInstance::visitor);
            break;
        default:
            bits.reset();
            throw std::logic_error{
                "Unexpected " + std::string{toString<category>()} + " instance: " + target.data() + '.'};
    }
}

//! @brief Run behavioral-related choices.
//! @param candidates - container for the candidate target instances
template <>
void runChoices<BehavioralInstance>(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::behavioral;
    const auto& bits = getCategoryOpts<category>();
    if (bits.none())
    {
        return;
    }
    assert(bits.size() == candidates.size());

    APP_DP_PRINT_TASK_BEGIN_TITLE(category);
    auto& pooling = action::resourcePool();
    auto* const threads = pooling.newElement(bits.count());
    const auto taskNamer = utility::currying::curry(taskNameCurried(), getCategoryAlias<category>());
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
            case abbrVal(BehavioralInstance::chainOfResponsibility):
                addTask(target, &BehavioralPattern::chainOfResponsibilityInstance);
                break;
            case abbrVal(BehavioralInstance::command):
                addTask(target, &BehavioralPattern::commandInstance);
                break;
            case abbrVal(BehavioralInstance::interpreter):
                addTask(target, &BehavioralPattern::interpreterInstance);
                break;
            case abbrVal(BehavioralInstance::iterator):
                addTask(target, &BehavioralPattern::iteratorInstance);
                break;
            case abbrVal(BehavioralInstance::mediator):
                addTask(target, &BehavioralPattern::mediatorInstance);
                break;
            case abbrVal(BehavioralInstance::memento):
                addTask(target, &BehavioralPattern::mementoInstance);
                break;
            case abbrVal(BehavioralInstance::observer):
                addTask(target, &BehavioralPattern::observerInstance);
                break;
            case abbrVal(BehavioralInstance::state):
                addTask(target, &BehavioralPattern::stateInstance);
                break;
            case abbrVal(BehavioralInstance::strategy):
                addTask(target, &BehavioralPattern::strategyInstance);
                break;
            case abbrVal(BehavioralInstance::templateMethod):
                addTask(target, &BehavioralPattern::templateMethodInstance);
                break;
            case abbrVal(BehavioralInstance::visitor):
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
    std::printf("\n==> %-15s Instance <==\n%s", getTitle(instance).c_str(), result.data());
}

void CreationalPattern::abstractFactoryInstance()
try
{
    const auto output = Creational().abstractFactory();
    showResult(CreationalInstance::abstractFactory, output.str());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void CreationalPattern::builderInstance()
try
{
    const auto output = Creational().builder();
    showResult(CreationalInstance::builder, output.str());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void CreationalPattern::factoryMethodInstance()
try
{
    const auto output = Creational().factoryMethod();
    showResult(CreationalInstance::factoryMethod, output.str());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void CreationalPattern::prototypeInstance()
try
{
    const auto output = Creational().prototype();
    showResult(CreationalInstance::prototype, output.str());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void CreationalPattern::singletonInstance()
try
{
    const auto output = Creational().singleton();
    showResult(CreationalInstance::singleton, output.str());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}
} // namespace creational

//! @brief Update creational-related choice.
//! @param target - target instance
template <>
void updateChoice<CreationalInstance>(const std::string_view target)
{
    constexpr auto category = Category::creational;
    auto& bits = getCategoryOpts<category>();

    switch (utility::common::bkdrHash(target.data()))
    {
        case abbrVal(CreationalInstance::abstractFactory):
            bits.set(CreationalInstance::abstractFactory);
            break;
        case abbrVal(CreationalInstance::builder):
            bits.set(CreationalInstance::builder);
            break;
        case abbrVal(CreationalInstance::factoryMethod):
            bits.set(CreationalInstance::factoryMethod);
            break;
        case abbrVal(CreationalInstance::prototype):
            bits.set(CreationalInstance::prototype);
            break;
        case abbrVal(CreationalInstance::singleton):
            bits.set(CreationalInstance::singleton);
            break;
        default:
            bits.reset();
            throw std::logic_error{
                "Unexpected " + std::string{toString<category>()} + " instance: " + target.data() + '.'};
    }
}

//! @brief Run creational-related choices.
//! @param candidates - container for the candidate target instances
template <>
void runChoices<CreationalInstance>(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::creational;
    const auto& bits = getCategoryOpts<category>();
    if (bits.none())
    {
        return;
    }
    assert(bits.size() == candidates.size());

    APP_DP_PRINT_TASK_BEGIN_TITLE(category);
    auto& pooling = action::resourcePool();
    auto* const threads = pooling.newElement(bits.count());
    const auto taskNamer = utility::currying::curry(taskNameCurried(), getCategoryAlias<category>());
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
            case abbrVal(CreationalInstance::abstractFactory):
                addTask(target, &CreationalPattern::abstractFactoryInstance);
                break;
            case abbrVal(CreationalInstance::builder):
                addTask(target, &CreationalPattern::builderInstance);
                break;
            case abbrVal(CreationalInstance::factoryMethod):
                addTask(target, &CreationalPattern::factoryMethodInstance);
                break;
            case abbrVal(CreationalInstance::prototype):
                addTask(target, &CreationalPattern::prototypeInstance);
                break;
            case abbrVal(CreationalInstance::singleton):
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
    std::printf("\n==> %-9s Instance <==\n%s", getTitle(instance).c_str(), result.data());
}

void StructuralPattern::adapterInstance()
try
{
    const auto output = Structural().adapter();
    showResult(StructuralInstance::adapter, output.str());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void StructuralPattern::bridgeInstance()
try
{
    const auto output = Structural().bridge();
    showResult(StructuralInstance::bridge, output.str());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void StructuralPattern::compositeInstance()
try
{
    const auto output = Structural().composite();
    showResult(StructuralInstance::composite, output.str());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void StructuralPattern::decoratorInstance()
try
{
    const auto output = Structural().decorator();
    showResult(StructuralInstance::decorator, output.str());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void StructuralPattern::facadeInstance()
try
{
    const auto output = Structural().facade();
    showResult(StructuralInstance::facade, output.str());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void StructuralPattern::flyweightInstance()
try
{
    const auto output = Structural().flyweight();
    showResult(StructuralInstance::flyweight, output.str());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void StructuralPattern::proxyInstance()
try
{
    const auto output = Structural().proxy();
    showResult(StructuralInstance::proxy, output.str());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}
} // namespace structural

//! @brief Update structural-related choice.
//! @param target - target instance
template <>
void updateChoice<StructuralInstance>(const std::string_view target)
{
    constexpr auto category = Category::structural;
    auto& bits = getCategoryOpts<category>();

    switch (utility::common::bkdrHash(target.data()))
    {
        case abbrVal(StructuralInstance::adapter):
            bits.set(StructuralInstance::adapter);
            break;
        case abbrVal(StructuralInstance::bridge):
            bits.set(StructuralInstance::bridge);
            break;
        case abbrVal(StructuralInstance::composite):
            bits.set(StructuralInstance::composite);
            break;
        case abbrVal(StructuralInstance::decorator):
            bits.set(StructuralInstance::decorator);
            break;
        case abbrVal(StructuralInstance::facade):
            bits.set(StructuralInstance::facade);
            break;
        case abbrVal(StructuralInstance::flyweight):
            bits.set(StructuralInstance::flyweight);
            break;
        case abbrVal(StructuralInstance::proxy):
            bits.set(StructuralInstance::proxy);
            break;
        default:
            bits.reset();
            throw std::logic_error{
                "Unexpected " + std::string{toString<category>()} + " instance: " + target.data() + '.'};
    }
}

//! @brief Run structural-related choices.
//! @param candidates - container for the candidate target instances
template <>
void runChoices<StructuralInstance>(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::structural;
    const auto& bits = getCategoryOpts<category>();
    if (bits.none())
    {
        return;
    }
    assert(bits.size() == candidates.size());

    APP_DP_PRINT_TASK_BEGIN_TITLE(category);
    auto& pooling = action::resourcePool();
    auto* const threads = pooling.newElement(bits.count());
    const auto taskNamer = utility::currying::curry(taskNameCurried(), getCategoryAlias<category>());
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
            case abbrVal(StructuralInstance::adapter):
                addTask(target, &StructuralPattern::adapterInstance);
                break;
            case abbrVal(StructuralInstance::bridge):
                addTask(target, &StructuralPattern::bridgeInstance);
                break;
            case abbrVal(StructuralInstance::composite):
                addTask(target, &StructuralPattern::compositeInstance);
                break;
            case abbrVal(StructuralInstance::decorator):
                addTask(target, &StructuralPattern::decoratorInstance);
                break;
            case abbrVal(StructuralInstance::facade):
                addTask(target, &StructuralPattern::facadeInstance);
                break;
            case abbrVal(StructuralInstance::flyweight):
                addTask(target, &StructuralPattern::flyweightInstance);
                break;
            case abbrVal(StructuralInstance::proxy):
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
