//! @file command.hpp
//! @author ryftchen
//! @brief The declarations (command) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#pragma once

#include "note.hpp"

#include "application/example/include/apply_algorithm.hpp"
#include "application/example/include/apply_data_structure.hpp"
#include "application/example/include/apply_design_pattern.hpp"
#include "application/example/include/apply_numeric.hpp"
#include "utility/include/argument.hpp"
#include "utility/include/console.hpp"
#include "utility/include/memory.hpp"
#include "utility/include/reflection.hpp"
#include "utility/include/thread.hpp"

//! @brief Reflect the sub-cli's category and alias name to the field.
#define COMMAND_REFLECT_SUB_CLI_FIELD(category, alias)             \
    Field                                                          \
    {                                                              \
        REFLECTION_STR(#category), &Type::category##Opts, AttrList \
        {                                                          \
            Attr                                                   \
            {                                                      \
                REFLECTION_STR("alias"), #alias                    \
            }                                                      \
        }                                                          \
    }
//! @brief Reflect the category's enumeration and choice name to the field.
#define COMMAND_REFLECT_CATEGORY_FIELD(enumeration, choice)       \
    Field                                                         \
    {                                                             \
        REFLECTION_STR(#enumeration), Type::enumeration, AttrList \
        {                                                         \
            Attr                                                  \
            {                                                     \
                REFLECTION_STR("choice"), #choice                 \
            }                                                     \
        }                                                         \
    }

//! @brief Static reflection for AlgorithmChoice. Mapping to regular choices.
template <>
struct utility::reflection::TypeInfo<application::app_algo::AlgorithmChoice>
    : TypeInfoBase<application::app_algo::AlgorithmChoice>
{
    //! @brief Name.
    static constexpr std::string_view name{"app-algo"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        COMMAND_REFLECT_SUB_CLI_FIELD(match   , m),
        COMMAND_REFLECT_SUB_CLI_FIELD(notation, n),
        COMMAND_REFLECT_SUB_CLI_FIELD(optimal , o),
        COMMAND_REFLECT_SUB_CLI_FIELD(search  , s),
        COMMAND_REFLECT_SUB_CLI_FIELD(sort    , S),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{REFLECTION_STR("descr"), "apply algorithm"}};
};
//! @brief Static reflection for MatchMethod. Mapping to regular choices.
template <>
struct utility::reflection::TypeInfo<application::app_algo::MatchMethod>
    : TypeInfoBase<application::app_algo::MatchMethod>
{
    //! @brief Name.
    static constexpr std::string_view name{"match"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        COMMAND_REFLECT_CATEGORY_FIELD(rabinKarp       , rab),
        COMMAND_REFLECT_CATEGORY_FIELD(knuthMorrisPratt, knu),
        COMMAND_REFLECT_CATEGORY_FIELD(boyerMoore      , boy),
        COMMAND_REFLECT_CATEGORY_FIELD(horspool        , hor),
        COMMAND_REFLECT_CATEGORY_FIELD(sunday          , sun),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{
        REFLECTION_STR("descr"),
        "match-related choices\n"
        "- rab    Rabin-Karp\n"
        "- knu    Knuth-Morris-Pratt\n"
        "- boy    Boyer-Moore\n"
        "- hor    Horspool\n"
        "- sun    Sunday\n"
        "add the choices listed above"}};
};
//! @brief Static reflection for NotationMethod. Mapping to regular choices.
template <>
struct utility::reflection::TypeInfo<application::app_algo::NotationMethod>
    : TypeInfoBase<application::app_algo::NotationMethod>
{
    //! @brief Name.
    static constexpr std::string_view name{"notation"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        COMMAND_REFLECT_CATEGORY_FIELD(prefix , pre),
        COMMAND_REFLECT_CATEGORY_FIELD(postfix, pos),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{
        REFLECTION_STR("descr"),
        "notation-related choices\n"
        "- pre    Prefix\n"
        "- pos    Postfix\n"
        "add the choices listed above"}};
};
//! @brief Static reflection for OptimalMethod. Mapping to regular choices.
template <>
struct utility::reflection::TypeInfo<application::app_algo::OptimalMethod>
    : TypeInfoBase<application::app_algo::OptimalMethod>
{
    //! @brief Name.
    static constexpr std::string_view name{"optimal"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        COMMAND_REFLECT_CATEGORY_FIELD(gradient , gra),
        COMMAND_REFLECT_CATEGORY_FIELD(annealing, ann),
        COMMAND_REFLECT_CATEGORY_FIELD(particle , par),
        COMMAND_REFLECT_CATEGORY_FIELD(genetic  , gen),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{
        REFLECTION_STR("descr"),
        "optimal-related choices\n"
        "- gra    Gradient Descent\n"
        "- ann    Simulated Annealing\n"
        "- par    Particle Swarm\n"
        "- gen    Genetic\n"
        "add the choices listed above"}};
};
//! @brief Static reflection for SearchMethod. Mapping to regular choices.
template <>
struct utility::reflection::TypeInfo<application::app_algo::SearchMethod>
    : TypeInfoBase<application::app_algo::SearchMethod>
{
    //! @brief Name.
    static constexpr std::string_view name{"search"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        COMMAND_REFLECT_CATEGORY_FIELD(binary       , bin),
        COMMAND_REFLECT_CATEGORY_FIELD(interpolation, int),
        COMMAND_REFLECT_CATEGORY_FIELD(fibonacci    , fib),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{
        REFLECTION_STR("descr"),
        "search-related choices\n"
        "- bin    Binary\n"
        "- int    Interpolation\n"
        "- fib    Fibonacci\n"
        "add the choices listed above"}};
};
//! @brief Static reflection for SortMethod. Mapping to regular choices.
template <>
struct utility::reflection::TypeInfo<application::app_algo::SortMethod>
    : TypeInfoBase<application::app_algo::SortMethod>
{
    //! @brief Name.
    static constexpr std::string_view name{"sort"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        COMMAND_REFLECT_CATEGORY_FIELD(bubble   , bub),
        COMMAND_REFLECT_CATEGORY_FIELD(selection, sel),
        COMMAND_REFLECT_CATEGORY_FIELD(insertion, ins),
        COMMAND_REFLECT_CATEGORY_FIELD(shell    , she),
        COMMAND_REFLECT_CATEGORY_FIELD(merge    , mer),
        COMMAND_REFLECT_CATEGORY_FIELD(quick    , qui),
        COMMAND_REFLECT_CATEGORY_FIELD(heap     , hea),
        COMMAND_REFLECT_CATEGORY_FIELD(counting , cou),
        COMMAND_REFLECT_CATEGORY_FIELD(bucket   , buc),
        COMMAND_REFLECT_CATEGORY_FIELD(radix    , rad),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{
        REFLECTION_STR("descr"),
        "sort-related choices\n"
        "- bub    Bubble\n"
        "- sel    Selection\n"
        "- ins    Insertion\n"
        "- she    Shell\n"
        "- mer    Merge\n"
        "- qui    Quick\n"
        "- hea    Heap\n"
        "- cou    Counting\n"
        "- buc    Bucket\n"
        "- rad    Radix\n"
        "add the choices listed above"}};
};

//! @brief Static reflection for DesignPatternChoice. Mapping to regular choices.
template <>
struct utility::reflection::TypeInfo<application::app_dp::DesignPatternChoice>
    : TypeInfoBase<application::app_dp::DesignPatternChoice>
{
    //! @brief Name.
    static constexpr std::string_view name{"app-dp"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        COMMAND_REFLECT_SUB_CLI_FIELD(behavioral, b),
        COMMAND_REFLECT_SUB_CLI_FIELD(creational, c),
        COMMAND_REFLECT_SUB_CLI_FIELD(structural, s),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{REFLECTION_STR("descr"), "apply design pattern"}};
};
//! @brief Static reflection for BehavioralInstance. Mapping to regular choices.
template <>
struct utility::reflection::TypeInfo<application::app_dp::BehavioralInstance>
    : TypeInfoBase<application::app_dp::BehavioralInstance>
{
    //! @brief Name.
    static constexpr std::string_view name{"behavioral"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        COMMAND_REFLECT_CATEGORY_FIELD(chainOfResponsibility, cha),
        COMMAND_REFLECT_CATEGORY_FIELD(command              , com),
        COMMAND_REFLECT_CATEGORY_FIELD(interpreter          , int),
        COMMAND_REFLECT_CATEGORY_FIELD(iterator             , ite),
        COMMAND_REFLECT_CATEGORY_FIELD(mediator             , med),
        COMMAND_REFLECT_CATEGORY_FIELD(memento              , mem),
        COMMAND_REFLECT_CATEGORY_FIELD(observer             , obs),
        COMMAND_REFLECT_CATEGORY_FIELD(state                , sta),
        COMMAND_REFLECT_CATEGORY_FIELD(strategy             , str),
        COMMAND_REFLECT_CATEGORY_FIELD(templateMethod       , tem),
        COMMAND_REFLECT_CATEGORY_FIELD(visitor              , vis),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{
        REFLECTION_STR("descr"),
        "behavioral-related choices\n"
        "- cha    Chain Of Responsibility\n"
        "- com    Command\n"
        "- int    Interpreter\n"
        "- ite    Iterator\n"
        "- med    Mediator\n"
        "- mem    Memento\n"
        "- obs    Observer\n"
        "- sta    State\n"
        "- str    Strategy\n"
        "- tem    Template Method\n"
        "- vis    Visitor\n"
        "add the choices listed above"}};
};
//! @brief Static reflection for CreationalInstance. Mapping to regular choices.
template <>
struct utility::reflection::TypeInfo<application::app_dp::CreationalInstance>
    : TypeInfoBase<application::app_dp::CreationalInstance>
{
    //! @brief Name.
    static constexpr std::string_view name{"creational"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        COMMAND_REFLECT_CATEGORY_FIELD(abstractFactory, abs),
        COMMAND_REFLECT_CATEGORY_FIELD(builder        , bui),
        COMMAND_REFLECT_CATEGORY_FIELD(factoryMethod  , fac),
        COMMAND_REFLECT_CATEGORY_FIELD(prototype      , pro),
        COMMAND_REFLECT_CATEGORY_FIELD(singleton      , sin),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{
        REFLECTION_STR("descr"),
        "creational-related choices\n"
        "- abs    Abstract Factory\n"
        "- bui    Builder\n"
        "- fac    Factory Method\n"
        "- pro    Prototype\n"
        "- sin    Singleton\n"
        "add the choices listed above"}};
};
//! @brief Static reflection for StructuralInstance. Mapping to regular choices.
template <>
struct utility::reflection::TypeInfo<application::app_dp::StructuralInstance>
    : TypeInfoBase<application::app_dp::StructuralInstance>
{
    //! @brief Name.
    static constexpr std::string_view name{"structural"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        COMMAND_REFLECT_CATEGORY_FIELD(adapter  , ada),
        COMMAND_REFLECT_CATEGORY_FIELD(bridge   , bri),
        COMMAND_REFLECT_CATEGORY_FIELD(composite, com),
        COMMAND_REFLECT_CATEGORY_FIELD(decorator, dec),
        COMMAND_REFLECT_CATEGORY_FIELD(facade   , fac),
        COMMAND_REFLECT_CATEGORY_FIELD(flyweight, fly),
        COMMAND_REFLECT_CATEGORY_FIELD(proxy    , pro),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{
        REFLECTION_STR("descr"),
        "structural-related choices\n"
        "- ada    Adapter\n"
        "- bri    Bridge\n"
        "- com    Composite\n"
        "- dec    Decorator\n"
        "- fac    Facade\n"
        "- fly    Flyweight\n"
        "- pro    Proxy\n"
        "add the choices listed above"}};
};

//! @brief Static reflection for DataStructureChoice. Mapping to regular choices.
template <>
struct utility::reflection::TypeInfo<application::app_ds::DataStructureChoice>
    : TypeInfoBase<application::app_ds::DataStructureChoice>
{
    //! @brief Name.
    static constexpr std::string_view name{"app-ds"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        COMMAND_REFLECT_SUB_CLI_FIELD(linear, l),
        COMMAND_REFLECT_SUB_CLI_FIELD(tree  , t),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{REFLECTION_STR("descr"), "apply data structure"}};
};
//! @brief Static reflection for LinearInstance. Mapping to regular choices.
template <>
struct utility::reflection::TypeInfo<application::app_ds::LinearInstance>
    : TypeInfoBase<application::app_ds::LinearInstance>
{
    //! @brief Name.
    static constexpr std::string_view name{"linear"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        COMMAND_REFLECT_CATEGORY_FIELD(linkedList, lin),
        COMMAND_REFLECT_CATEGORY_FIELD(stack     , sta),
        COMMAND_REFLECT_CATEGORY_FIELD(queue     , que),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{
        REFLECTION_STR("descr"),
        "linear-related choices\n"
        "- lin    Linked List\n"
        "- sta    Stack\n"
        "- que    Queue\n"
        "add the choices listed above"}};
};
//! @brief Static reflection for TreeInstance. Mapping to regular choices.
template <>
struct utility::reflection::TypeInfo<application::app_ds::TreeInstance>
    : TypeInfoBase<application::app_ds::TreeInstance>
{
    //! @brief Name.
    static constexpr std::string_view name{"tree"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        COMMAND_REFLECT_CATEGORY_FIELD(binarySearch       , bin),
        COMMAND_REFLECT_CATEGORY_FIELD(adelsonVelskyLandis, ade),
        COMMAND_REFLECT_CATEGORY_FIELD(splay              , spl),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{
        REFLECTION_STR("descr"),
        "tree-related choices\n"
        "- bin    Binary Search\n"
        "- ade    Adelson-Velsky-Landis\n"
        "- spl    Splay\n"
        "add the choices listed above"}};
};

//! @brief Static reflection for NumericChoice. Mapping to regular choices.
template <>
struct utility::reflection::TypeInfo<application::app_num::NumericChoice>
    : TypeInfoBase<application::app_num::NumericChoice>
{
    //! @brief Name.
    static constexpr std::string_view name{"app-num"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        COMMAND_REFLECT_SUB_CLI_FIELD(arithmetic, a),
        COMMAND_REFLECT_SUB_CLI_FIELD(divisor   , d),
        COMMAND_REFLECT_SUB_CLI_FIELD(integral  , i),
        COMMAND_REFLECT_SUB_CLI_FIELD(prime     , p),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{REFLECTION_STR("descr"), "apply numeric"}};
};
//! @brief Static reflection for ArithmeticMethod. Mapping to regular choices.
template <>
struct utility::reflection::TypeInfo<application::app_num::ArithmeticMethod>
    : TypeInfoBase<application::app_num::ArithmeticMethod>
{
    //! @brief Name.
    static constexpr std::string_view name{"arithmetic"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        COMMAND_REFLECT_CATEGORY_FIELD(addition      , add),
        COMMAND_REFLECT_CATEGORY_FIELD(subtraction   , sub),
        COMMAND_REFLECT_CATEGORY_FIELD(multiplication, mul),
        COMMAND_REFLECT_CATEGORY_FIELD(division      , div),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{
        REFLECTION_STR("descr"),
        "arithmetic-related choices\n"
        "- add    Addition\n"
        "- sub    Subtraction\n"
        "- mul    Multiplication\n"
        "- div    Division\n"
        "add the choices listed above"}};
};
//! @brief Static reflection for DivisorMethod. Mapping to regular choices.
template <>
struct utility::reflection::TypeInfo<application::app_num::DivisorMethod>
    : TypeInfoBase<application::app_num::DivisorMethod>
{
    //! @brief Name.
    static constexpr std::string_view name{"divisor"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        COMMAND_REFLECT_CATEGORY_FIELD(euclidean, euc),
        COMMAND_REFLECT_CATEGORY_FIELD(stein    , ste),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{
        REFLECTION_STR("descr"),
        "divisor-related choices\n"
        "- euc    Euclidean\n"
        "- ste    Stein\n"
        "add the choices listed above"}};
};
//! @brief Static reflection for IntegralMethod. Mapping to regular choices.
template <>
struct utility::reflection::TypeInfo<application::app_num::IntegralMethod>
    : TypeInfoBase<application::app_num::IntegralMethod>
{
    //! @brief Name.
    static constexpr std::string_view name{"integral"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        COMMAND_REFLECT_CATEGORY_FIELD(trapezoidal, tra),
        COMMAND_REFLECT_CATEGORY_FIELD(simpson    , sim),
        COMMAND_REFLECT_CATEGORY_FIELD(romberg    , rom),
        COMMAND_REFLECT_CATEGORY_FIELD(gauss      , gau),
        COMMAND_REFLECT_CATEGORY_FIELD(monteCarlo , mon),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{
        REFLECTION_STR("descr"),
        "integral-related choices\n"
        "- tra    Trapezoidal\n"
        "- sim    Adaptive Simpson's 1/3\n"
        "- rom    Romberg\n"
        "- gau    Gauss-Legendre's 5-Points\n"
        "- mon    Monte-Carlo\n"
        "add the choices listed above"}};
};
//! @brief Static reflection for PrimeMethod. Mapping to regular choices.
template <>
struct utility::reflection::TypeInfo<application::app_num::PrimeMethod>
    : TypeInfoBase<application::app_num::PrimeMethod>
{
    //! @brief Name.
    static constexpr std::string_view name{"prime"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        COMMAND_REFLECT_CATEGORY_FIELD(eratosthenes, era),
        COMMAND_REFLECT_CATEGORY_FIELD(euler       , eul),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{
        REFLECTION_STR("descr"),
        "prime-related choices\n"
        "- era    Eratosthenes\n"
        "- eul    Euler\n"
        "add the choices listed above"}};
};

#undef COMMAND_REFLECT_SUB_CLI_FIELD
#undef COMMAND_REFLECT_CATEGORY_FIELD

//! @brief The application module.
namespace application // NOLINT (modernize-concat-nested-namespaces)
{
//! @brief Command-line-related functions in the application module.
namespace command
{
//! @brief Message of "update choice" when applying.
//! @tparam Evt - type of apply event
template <typename Evt>
struct AppUpdMsg
{
    //! @brief Target choice.
    const std::string cho;
};
//! @brief Message of "run choices" when applying.
//! @tparam Evt - type of apply event
template <typename Evt>
struct AppRunMsg
{
    //! @brief Choice candidates.
    const std::vector<std::string> cntr;
};
//! @brief Notification type of "update choice" when applying.
//! @tparam Msg - type of message when applying
template <typename Msg>
struct AppUpdNtyType
{
};
//! @brief Notification type of "run choices" when applying.
//! @tparam Msg - type of message when applying
template <typename Msg>
struct AppRunNtyType
{
};
//! @brief Message type list for applying.
//! @tparam Types - type of notification types
template <typename... Types>
struct ApplyMessageTypeList
{
    //! @brief Alias for the parameter pack.
    //! @tparam InnerTypes - type of inner types
    //! @tparam TemplatedType - type of templated type
    template <template <typename... InnerTypes> typename TemplatedType>
    using AsParameterPackFor = TemplatedType<Types...>;
    //! @brief Alias for the providing interface.
    //! @tparam Interface - type of interface
    template <typename Interface>
    using WithInterface = ApplyMessageTypeList<Types..., Interface>;
};
//! @brief Alias for the message type list for applying.
using ApplyMessageTypes = ApplyMessageTypeList<
    AppUpdNtyType<AppUpdMsg<app_algo::MatchMethod>>,
    AppRunNtyType<AppRunMsg<app_algo::MatchMethod>>,
    AppUpdNtyType<AppUpdMsg<app_algo::NotationMethod>>,
    AppRunNtyType<AppRunMsg<app_algo::NotationMethod>>,
    AppUpdNtyType<AppUpdMsg<app_algo::OptimalMethod>>,
    AppRunNtyType<AppRunMsg<app_algo::OptimalMethod>>,
    AppUpdNtyType<AppUpdMsg<app_algo::SearchMethod>>,
    AppRunNtyType<AppRunMsg<app_algo::SearchMethod>>,
    AppUpdNtyType<AppUpdMsg<app_algo::SortMethod>>,
    AppRunNtyType<AppRunMsg<app_algo::SortMethod>>,
    AppUpdNtyType<AppUpdMsg<app_dp::BehavioralInstance>>,
    AppRunNtyType<AppRunMsg<app_dp::BehavioralInstance>>,
    AppUpdNtyType<AppUpdMsg<app_dp::CreationalInstance>>,
    AppRunNtyType<AppRunMsg<app_dp::CreationalInstance>>,
    AppUpdNtyType<AppUpdMsg<app_dp::StructuralInstance>>,
    AppRunNtyType<AppRunMsg<app_dp::StructuralInstance>>,
    AppUpdNtyType<AppUpdMsg<app_ds::LinearInstance>>,
    AppRunNtyType<AppRunMsg<app_ds::LinearInstance>>,
    AppUpdNtyType<AppUpdMsg<app_ds::TreeInstance>>,
    AppRunNtyType<AppRunMsg<app_ds::TreeInstance>>,
    AppUpdNtyType<AppUpdMsg<app_num::ArithmeticMethod>>,
    AppRunNtyType<AppRunMsg<app_num::ArithmeticMethod>>,
    AppUpdNtyType<AppUpdMsg<app_num::DivisorMethod>>,
    AppRunNtyType<AppRunMsg<app_num::DivisorMethod>>,
    AppUpdNtyType<AppUpdMsg<app_num::IntegralMethod>>,
    AppRunNtyType<AppRunMsg<app_num::IntegralMethod>>,
    AppUpdNtyType<AppUpdMsg<app_num::PrimeMethod>>,
    AppRunNtyType<AppRunMsg<app_num::PrimeMethod>>>;

//! @brief Alias for the message handler for applying.
//! @tparam Msg - type of message when applying
template <typename Msg>
using Handler = std::function<void(const Msg&)>;
//! @brief Message dispatcher for applying.
//! @tparam Ns - type of notifications when applying
template <typename... Ns>
class Dispatcher;
//! @brief Message dispatcher of "update choice" for applying.
//! @tparam Msg - type of message when applying
//! @tparam Ns - type of notifications when applying
template <typename Msg, typename... Ns>
class Dispatcher<AppUpdNtyType<Msg>, Ns...> : public Dispatcher<Ns...>
{
public:
    using Dispatcher<Ns...>::registerHandler;

    //! @brief Register the handler.
    //! @param handler - message handler
    virtual void registerHandler(Handler<Msg> handler) = 0;
};
//! @brief Message dispatcher of "run choices" for applying.
//! @tparam Msg - type of messages when applying
//! @tparam Ns - type of notifications when applying
template <typename Msg, typename... Ns>
class Dispatcher<AppRunNtyType<Msg>, Ns...> : public Dispatcher<Ns...>
{
public:
    using Dispatcher<Ns...>::registerHandler;

    //! @brief Register the handler.
    //! @param handler - message handler
    virtual void registerHandler(Handler<Msg> handler) = 0;
};
//! @brief Default message dispatcher for applying.
template <>
class Dispatcher<>
{
public:
    //! @brief Register the handler.
    void registerHandler(){};
};
//! @brief Message receiver for applying.
//! @tparam Ns - type of notifications when applying
template <typename... Ns>
class Receiver;
//! @brief Message receiver of "update choice" for applying.
//! @tparam Msg - type of message when applying
//! @tparam Ns - type of notifications when applying
template <typename Msg, typename... Ns>
class Receiver<AppUpdNtyType<Msg>, Ns...> : public Receiver<Ns...>
{
public:
    using Receiver<Ns...>::onMessage;

    //! @brief Action on message.
    //! @param message - message body
    virtual void onMessage(const Msg& message) = 0;
};
//! @brief Message receiver of "run choices" for applying.
//! @tparam Msg - type of message when applying
//! @tparam Ns - type of notifications when applying
template <typename Msg, typename... Ns>
class Receiver<AppRunNtyType<Msg>, Ns...> : public Receiver<Ns...>
{
public:
    using Receiver<Ns...>::onMessage;

    //! @brief Action on message.
    //! @param message - message body
    virtual void onMessage(const Msg& message) = 0;
};
//! @brief Default message receiver for applying.
template <>
class Receiver<>
{
public:
    //! @brief Action on message.
    void onMessage(){};
};
//! @brief Forwarding basis for all message types when applying.
struct ForwardBase : public ApplyMessageTypes::AsParameterPackFor<Dispatcher>,
                     public ApplyMessageTypes::AsParameterPackFor<Receiver>
{
};
//! @brief Forwarding action for applying.
//! @tparam Ns - type of notifications when applying
template <typename... Ns>
class Forward;
//! @brief Forward message of "update choice" for applying.
//! @tparam Msg - type of message when applying
//! @tparam Ns - type of notifications when applying
template <typename Msg, typename... Ns>
class Forward<AppUpdNtyType<Msg>, Ns...> : public Forward<Ns...>
{
public:
    //! @brief Alias for the base class.
    using Base = Forward<Ns...>;
    using Base::registerHandler, Base::onMessage;

    //! @brief Register the handler.
    //! @param handler - message handler
    void registerHandler(Handler<Msg> handler) override { this->handler = std::move(handler); }
    //! @brief Action on message.
    //! @param message - message body
    void onMessage(const Msg& message) override
    {
        if (handler)
        {
            handler(message);
        }
    }

private:
    //! @brief Message handler.
    Handler<Msg> handler{};
};
//! @brief Forward message of "run choices" for applying.
//! @tparam Msg - type of message when applying
//! @tparam Ns - type of notifications when applying
template <typename Msg, typename... Ns>
class Forward<AppRunNtyType<Msg>, Ns...> : public Forward<Ns...>
{
public:
    //! @brief Alias for the base class.
    using Base = Forward<Ns...>;
    using Base::registerHandler, Base::onMessage;

    //! @brief Register the handler.
    //! @param handler - message handler
    void registerHandler(Handler<Msg> handler) override { this->handler = std::move(handler); }
    //! @brief Action on message.
    //! @param message - message body
    void onMessage(const Msg& message) override
    {
        if (handler)
        {
            handler(message);
        }
    }

private:
    //! @brief Message handler.
    Handler<Msg> handler{};
};
//! @brief Forwarding action interface for applying.
//! @tparam Interface - type of interface
template <typename Interface>
class Forward<Interface> : public Interface
{
};

//! @brief Message forwarder for applying.
class ApplyMessageForwarder : public ApplyMessageTypes::WithInterface<ForwardBase>::AsParameterPackFor<Forward>
{
};

//! @brief Apply event object's helper type for the visitor.
//! @tparam Ts - type of visitors
template <class... Ts>
struct AppEvtOverloaded : Ts...
{
    using Ts::operator()...;
};
//! @brief Explicit deduction guide for AppEvtOverloaded.
//! @tparam Ts - type of visitors
template <class... Ts>
AppEvtOverloaded(Ts...) -> AppEvtOverloaded<Ts...>;
//! @brief Alias for the applying event.
using ApplyEvent = std::variant<
    app_algo::MatchMethod,
    app_algo::NotationMethod,
    app_algo::OptimalMethod,
    app_algo::SearchMethod,
    app_algo::SortMethod,
    app_dp::BehavioralInstance,
    app_dp::CreationalInstance,
    app_dp::StructuralInstance,
    app_ds::LinearInstance,
    app_ds::TreeInstance,
    app_num::ArithmeticMethod,
    app_num::DivisorMethod,
    app_num::IntegralMethod,
    app_num::PrimeMethod>;

//! @brief Represent the maximum value of an enum.
//! @tparam T - type of specific enum
template <class T>
struct Bottom;

//! @brief Enumerate specific basic categories.
enum Category : std::uint8_t
{
    //! @brief Console.
    console,
    //! @brief Dump.
    dump,
    //! @brief Help.
    help,
    //! @brief Version.
    version
};
//! @brief Store the maximum value of the Category enum.
template <>
struct Bottom<Category>
{
    //! @brief Maximum value of the Category enum.
    static constexpr std::uint8_t value{4};
};

//! @brief Execute the command line.
class Command final
{
public:
    //! @brief Destroy the Command object.
    virtual ~Command();
    //! @brief Construct a new Command object.
    Command(const Command&) = delete;
    //! @brief Construct a new Command object.
    Command(Command&&) = delete;
    //! @brief The operator (=) overloading of Command class.
    //! @return reference of the Command object
    Command& operator=(const Command&) = delete;
    //! @brief The operator (=) overloading of Command class.
    //! @return reference of the Command object
    Command& operator=(Command&&) = delete;

    //! @brief Get the Command instance.
    //! @return reference of the Command object
    static Command& getInstance();
    //! @brief Execution manager for running commander.
    //! @param argc - argument count
    //! @param argv - argument vector
    void execManager(const int argc, const char* const argv[]);

private:
    //! @brief Construct a new Command object.
    Command();

    //! @brief Mutex for controlling multi-threading.
    mutable std::mutex mtx{};
    //! @brief The synchronization condition for foreground and background. Use with mtx.
    std::condition_variable cv{};
    //! @brief Flag to indicate whether parsing of arguments is completed.
    std::atomic<bool> isParsed{false};
    //! @brief Parse argument helper for commander.
    utility::argument::Argument mainCLI{"foo", note::version()};
    //! @brief Parse argument helper to apply algorithm.
    utility::argument::Argument subCLIAppAlgo{
        utility::reflection::TypeInfo<app_algo::AlgorithmChoice>::name, note::version()};
    //! @brief Parse argument helper to apply design pattern.
    utility::argument::Argument subCLIAppDp{
        utility::reflection::TypeInfo<app_dp::DesignPatternChoice>::name, note::version()};
    //! @brief Parse argument helper to apply data structure.
    utility::argument::Argument subCLIAppDs{
        utility::reflection::TypeInfo<app_ds::DataStructureChoice>::name, note::version()};
    //! @brief Parse argument helper to apply numeric.
    utility::argument::Argument subCLIAppNum{
        utility::reflection::TypeInfo<app_num::NumericChoice>::name, note::version()};

    //! @brief Initialize the parse argument helpers.
    void initializeCLI();
    //! @brief Foreground handler for parsing command line arguments.
    //! @param argc - argument count
    //! @param argv - argument vector
    void foregroundHandler(const int argc, const char* const argv[]);
    //! @brief Background handler for performing the specific tasks.
    void backgroundHandler();
    //! @brief Pre-check the basic type or regular type task.
    void validate();
    //! @brief Check whether any type tasks exist.
    //! @return any type tasks exist or do not exist
    bool anySelected() const;
    //! @brief Dispatch specific tasks.
    void dispatch();
    //! @brief Execute the command line of console mode.
    void executeInConsole() const;
    //! @brief Print help message.
    void showHelpMessage() const;
    //! @brief Dump configuration.
    void dumpConfiguration() const;
    //! @brief Print version icon.
    void showVersionIcon() const;
    //! @brief Check for excessive arguments.
    void checkForExcessiveArguments();

    //! @brief Alias for the sub-cli name.
    using SubCLIName = std::string;
    //! @brief Alias for the category name.
    using CategoryName = std::string;
    //! @brief Alias for the target choice.
    using ChoiceName = std::string;
    //! @brief Alias for the container of ChoiceName.
    using ChoiceContainer = std::vector<ChoiceName>;
    //! @brief Alias for the extend attribute of the target category.
    struct CategoryExtAttr
    {
        //! @brief The candidates for the choice.
        ChoiceContainer choices{};
        //! @brief The internal event for applying.
        ApplyEvent event{};
    };
    //! @brief Alias for the map of CategoryName and CategoryExtAttr.
    using SubCLIMap = std::map<CategoryName, CategoryExtAttr>;
    //! @brief Alias for the map of SubCLIName and SubCLIMap.
    using RegularChoiceMap = std::map<SubCLIName, SubCLIMap>;

    //! @brief Get the description.
    //! @tparam T - type of sub-cli or category
    //! @return description
    template <class T>
    static inline constexpr std::string_view getDescr()
    {
        using TypeInfo = utility::reflection::TypeInfo<T>;

        return TypeInfo::attrs.find(REFLECTION_STR("descr")).value;
    }
    //! @brief Get the alias name.
    //! @tparam SubCLI - type of sub-cli
    //! @tparam Cat - type of category
    //! @return alias name
    template <class SubCLI, class Cat>
    static inline constexpr std::string_view getAlias()
    {
        using SubCLITypeInfo = utility::reflection::TypeInfo<SubCLI>;
        using CatTypeInfo = utility::reflection::TypeInfo<Cat>;

        return SubCLITypeInfo::fields.find(REFLECTION_STR(CatTypeInfo::name)).attrs.find(REFLECTION_STR("alias")).value;
    }
    //! @brief Extract all choices in the category.
    //! @tparam Cat - type of category
    //! @return all choices
    template <class Cat>
    static ChoiceContainer extractChoices();

    // clang-format off
    //! @brief Mapping table of all basic categories.
    const std::map<CategoryName, void (Command::*)() const> basicCategories
    {
        // - Category -+----------- Functor -----------
        // ------------+-------------------------------
        { "console"    , &Command::executeInConsole  },
        { "dump"       , &Command::dumpConfiguration },
        { "help"       , &Command::showHelpMessage   },
        { "version"    , &Command::showVersionIcon   }
        // ------------+--------------------------------
    };
    // clang-format on
    //! @brief Mapping table of all regular choices. Fill as needed.
    RegularChoiceMap regularChoices{};

    //! @brief Manage basic categories.
    class BasicManager
    {
    public:
        //! @brief Bit flags for managing basic categories.
        std::bitset<Bottom<Category>::value> categories{};

        //! @brief Check whether any basic categories do not exist.
        //! @return any basic categories do not exist or exist
        [[nodiscard]] inline bool empty() const { return categories.none(); }
        //! @brief Reset bit flags that manage basic categories.
        inline void reset() { categories.reset(); }
    };

    //! @brief Manage regular choices of sub-cli.
    class RegularManager
    {
    public:
        //! @brief Enumerate specific regular choices.
        enum Order : std::uint8_t
        {
            //! @brief Algorithm.
            algorithm,
            //! @brief Design pattern.
            designPattern,
            //! @brief Data structure.
            dataStructure,
            //! @brief Numeric.
            numeric
        };
        //! @brief Flag for help only.
        bool helpOnly{false};

        //! @brief Check whether any regular choices do not exist.
        //! @return any regular choices do not exist or exist
        [[nodiscard]] inline bool empty() const
        {
            return app_algo::manager().empty() && app_ds::manager().empty() && app_dp::manager().empty()
                && app_num::manager().empty() && !helpOnly;
        }
        //! @brief Reset bit flags that manage regular choices.
        inline void reset()
        {
            app_algo::manager().reset();
            app_ds::manager().reset();
            app_dp::manager().reset();
            app_num::manager().reset();
            helpOnly = false;
        }
        //! @brief Get the existing order.
        //! @return existing order
        Order getExistingOrder()
        {
            const std::uint8_t validation = !app_algo::manager().empty() + !app_dp::manager().empty()
                + !app_ds::manager().empty() + !app_num::manager().empty();
            if (1 == validation)
            {
                if (!app_algo::manager().empty())
                {
                    return Order::algorithm;
                }
                else if (!app_dp::manager().empty())
                {
                    return Order::designPattern;
                }
                else if (!app_ds::manager().empty())
                {
                    return Order::dataStructure;
                }
                else if (!app_num::manager().empty())
                {
                    return Order::numeric;
                }
            }

            reset();
            throw std::logic_error("The current regular choice does not meet the requirement of having only one order");
        }
    };

    //! @brief Manage all types of tasks.
    struct DispatchManager
    {
        //! @brief Dispatch basic type tasks.
        BasicManager basicManager{};
        //! @brief Dispatch regular type tasks.
        RegularManager regularManager{};

        //! @brief Check whether any tasks do not exist.
        //! @return any tasks do not exist or exist
        [[nodiscard]] inline bool empty() const { return basicManager.empty() && regularManager.empty(); }
        //! @brief Reset bit flags that manage all tasks.
        inline void reset()
        {
            basicManager.reset();
            regularManager.reset();
        }
    } /** @brief Dispatch all types of tasks. */ dispatchManager{};
    //! @brief Forward messages for applying.
    ApplyMessageForwarder appMsgForwarder{};

    //! @brief Enter console mode.
    static void enterConsoleMode();
    //! @brief Register the command line to console mode.
    //! @tparam T - type of client
    //! @param console - console to be registered
    //! @param client - client used to send
    template <typename T>
    static void registerOnConsole(utility::console::Console& console, std::shared_ptr<T>& client);
    //! @brief Launch the client for console mode.
    //! @tparam T - type of client
    //! @param client - client to be launched
    template <typename T>
    static void launchClient(std::shared_ptr<T>& client);
    //! @brief Validate dependencies version.
    void validateDependenciesVersion() const;
    //! @brief Get ASCII banner text.
    //! @return ASCII banner text content
    static std::string getIconBanner();

    //! @brief Latency (ms) for console.
    static constexpr std::uint16_t latency{10};
};

//! @brief Preset thread name.
//! @param cli - sub-cli
//! @param cat - category
//! @param cho - choice
//! @return thread name
inline std::string presetTaskName(const std::string_view cli, const std::string_view cat, const std::string_view cho)
{
    return '@' + std::string{cli} + '_' + std::string{cat} + '_' + std::string{cho};
}

//! @brief Alias for memory pool when making multi-threading.
using PublicThreadPool = utility::memory::Memory<utility::thread::Thread>;
extern PublicThreadPool& getPublicThreadPool();
} // namespace command
} // namespace application
