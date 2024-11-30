//! @file action.hpp
//! @author ryftchen
//! @brief The declarations (action) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#pragma once

#include "application/example/include/apply_algorithm.hpp"
#include "application/example/include/apply_data_structure.hpp"
#include "application/example/include/apply_design_pattern.hpp"
#include "application/example/include/apply_numeric.hpp"
#include "utility/include/memory.hpp"
#include "utility/include/reflection.hpp"
#include "utility/include/thread.hpp"

//! @brief Reflect the sub-cli category and alias name to the field in the command line argument mapping.
#define ACTION_REFLECT_COMMAND_SUB_CLI_FIELD(category, alias)      \
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
//! @brief Reflect the category enumeration and choice name to the field in the command line argument mapping.
#define ACTION_REFLECT_COMMAND_CATEGORY_FIELD(enumeration, choice) \
    Field                                                          \
    {                                                              \
        REFLECTION_STR(#enumeration), Type::enumeration, AttrList  \
        {                                                          \
            Attr                                                   \
            {                                                      \
                REFLECTION_STR("choice"), #choice                  \
            }                                                      \
        }                                                          \
    }

//! @brief Static reflection for ApplyAlgorithm. Used to map command line arguments.
template <>
struct utility::reflection::TypeInfo<application::app_algo::ApplyAlgorithm>
    : TypeInfoBase<application::app_algo::ApplyAlgorithm>
{
    //! @brief Name.
    static constexpr std::string_view name{"app-algo"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        ACTION_REFLECT_COMMAND_SUB_CLI_FIELD(match   , m),
        ACTION_REFLECT_COMMAND_SUB_CLI_FIELD(notation, n),
        ACTION_REFLECT_COMMAND_SUB_CLI_FIELD(optimal , o),
        ACTION_REFLECT_COMMAND_SUB_CLI_FIELD(search  , s),
        ACTION_REFLECT_COMMAND_SUB_CLI_FIELD(sort    , S),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{REFLECTION_STR("descr"), "apply algorithm"}};
};
//! @brief Static reflection for MatchMethod. Used to map command line arguments.
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
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(rabinKarp       , rab),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(knuthMorrisPratt, knu),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(boyerMoore      , boy),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(horspool        , hor),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(sunday          , sun),
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
//! @brief Static reflection for NotationMethod. Used to map command line arguments.
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
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(prefix , pre),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(postfix, pos),
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
//! @brief Static reflection for OptimalMethod. Used to map command line arguments.
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
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(gradient , gra),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(annealing, ann),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(particle , par),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(genetic  , gen),
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
//! @brief Static reflection for SearchMethod. Used to map command line arguments.
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
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(binary       , bin),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(interpolation, int),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(fibonacci    , fib),
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
//! @brief Static reflection for SortMethod. Used to map command line arguments.
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
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(bubble   , bub),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(selection, sel),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(insertion, ins),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(shell    , she),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(merge    , mer),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(quick    , qui),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(heap     , hea),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(counting , cou),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(bucket   , buc),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(radix    , rad),
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

//! @brief Static reflection for ApplyDesignPattern. Used to map command line arguments.
template <>
struct utility::reflection::TypeInfo<application::app_dp::ApplyDesignPattern>
    : TypeInfoBase<application::app_dp::ApplyDesignPattern>
{
    //! @brief Name.
    static constexpr std::string_view name{"app-dp"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        ACTION_REFLECT_COMMAND_SUB_CLI_FIELD(behavioral, b),
        ACTION_REFLECT_COMMAND_SUB_CLI_FIELD(creational, c),
        ACTION_REFLECT_COMMAND_SUB_CLI_FIELD(structural, s),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{REFLECTION_STR("descr"), "apply design pattern"}};
};
//! @brief Static reflection for BehavioralInstance. Used to map command line arguments.
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
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(chainOfResponsibility, cha),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(command              , com),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(interpreter          , int),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(iterator             , ite),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(mediator             , med),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(memento              , mem),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(observer             , obs),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(state                , sta),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(strategy             , str),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(templateMethod       , tem),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(visitor              , vis),
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
//! @brief Static reflection for CreationalInstance. Used to map command line arguments.
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
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(abstractFactory, abs),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(builder        , bui),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(factoryMethod  , fac),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(prototype      , pro),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(singleton      , sin),
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
//! @brief Static reflection for StructuralInstance. Used to map command line arguments.
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
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(adapter  , ada),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(bridge   , bri),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(composite, com),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(decorator, dec),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(facade   , fac),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(flyweight, fly),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(proxy    , pro),
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

//! @brief Static reflection for ApplyDataStructure. Used to map command line arguments.
template <>
struct utility::reflection::TypeInfo<application::app_ds::ApplyDataStructure>
    : TypeInfoBase<application::app_ds::ApplyDataStructure>
{
    //! @brief Name.
    static constexpr std::string_view name{"app-ds"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        ACTION_REFLECT_COMMAND_SUB_CLI_FIELD(linear, l),
        ACTION_REFLECT_COMMAND_SUB_CLI_FIELD(tree  , t),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{REFLECTION_STR("descr"), "apply data structure"}};
};
//! @brief Static reflection for LinearInstance. Used to map command line arguments.
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
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(linkedList, lin),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(stack     , sta),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(queue     , que),
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
//! @brief Static reflection for TreeInstance. Used to map command line arguments.
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
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(binarySearch       , bin),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(adelsonVelskyLandis, ade),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(splay              , spl),
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

//! @brief Static reflection for ApplyNumeric. Used to map command line arguments.
template <>
struct utility::reflection::TypeInfo<application::app_num::ApplyNumeric>
    : TypeInfoBase<application::app_num::ApplyNumeric>
{
    //! @brief Name.
    static constexpr std::string_view name{"app-num"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        ACTION_REFLECT_COMMAND_SUB_CLI_FIELD(arithmetic, a),
        ACTION_REFLECT_COMMAND_SUB_CLI_FIELD(divisor   , d),
        ACTION_REFLECT_COMMAND_SUB_CLI_FIELD(integral  , i),
        ACTION_REFLECT_COMMAND_SUB_CLI_FIELD(prime     , p),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{REFLECTION_STR("descr"), "apply numeric"}};
};
//! @brief Static reflection for ArithmeticMethod. Used to map command line arguments.
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
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(addition      , add),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(subtraction   , sub),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(multiplication, mul),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(division      , div),
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
//! @brief Static reflection for DivisorMethod. Used to map command line arguments.
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
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(euclidean, euc),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(stein    , ste),
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
//! @brief Static reflection for IntegralMethod. Used to map command line arguments.
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
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(trapezoidal, tra),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(simpson    , sim),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(romberg    , rom),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(gauss      , gau),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(monteCarlo , mon),
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
//! @brief Static reflection for PrimeMethod. Used to map command line arguments.
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
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(eratosthenes, era),
        ACTION_REFLECT_COMMAND_CATEGORY_FIELD(euler       , eul),
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

#undef ACTION_REFLECT_COMMAND_SUB_CLI_FIELD
#undef ACTION_REFLECT_COMMAND_CATEGORY_FIELD

//! @brief The application module.
namespace application // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Applied-action-related functions in the application module.
namespace action
{
//! @brief The "Update Choice" message in the applied action.
//! @tparam Evt - type of applied action event
template <typename Evt>
struct UpdateChoice
{
    //! @brief Target choice.
    const std::string cho{};
};
//! @brief The "Run Choices" message in the applied action.
//! @tparam Evt - type of applied action event
template <typename Evt>
struct RunChoices
{
    //! @brief Collection of candidates for choice.
    const std::vector<std::string> coll{};
};
//! @brief Indication type of updating in the applied action.
//! @tparam Msg - type of message
template <typename Msg>
struct UpdatingIndication
{
};
//! @brief Indication type of running in the applied action.
//! @tparam Msg - type of message
template <typename Msg>
struct RunningIndication
{
};
//! @brief Message type list.
//! @tparam Types - type of indication types
template <typename... Types>
struct MessageTypeList
{
    //! @brief Alias for the parameter pack.
    //! @tparam InnerTypes - type of inner types
    //! @tparam TemplatedType - type of templated type
    template <template <typename... InnerTypes> typename TemplatedType>
    using AsParameterPackFor = TemplatedType<Types...>;
    //! @brief Alias for the providing interface.
    //! @tparam Intf - type of interface
    template <typename Intf>
    using WithInterface = MessageTypeList<Types..., Intf>;
};
//! @brief Alias for the message type list.
using MessageTypes = MessageTypeList<
    UpdatingIndication<UpdateChoice<app_algo::MatchMethod>>,
    RunningIndication<RunChoices<app_algo::MatchMethod>>,
    UpdatingIndication<UpdateChoice<app_algo::NotationMethod>>,
    RunningIndication<RunChoices<app_algo::NotationMethod>>,
    UpdatingIndication<UpdateChoice<app_algo::OptimalMethod>>,
    RunningIndication<RunChoices<app_algo::OptimalMethod>>,
    UpdatingIndication<UpdateChoice<app_algo::SearchMethod>>,
    RunningIndication<RunChoices<app_algo::SearchMethod>>,
    UpdatingIndication<UpdateChoice<app_algo::SortMethod>>,
    RunningIndication<RunChoices<app_algo::SortMethod>>,
    UpdatingIndication<UpdateChoice<app_dp::BehavioralInstance>>,
    RunningIndication<RunChoices<app_dp::BehavioralInstance>>,
    UpdatingIndication<UpdateChoice<app_dp::CreationalInstance>>,
    RunningIndication<RunChoices<app_dp::CreationalInstance>>,
    UpdatingIndication<UpdateChoice<app_dp::StructuralInstance>>,
    RunningIndication<RunChoices<app_dp::StructuralInstance>>,
    UpdatingIndication<UpdateChoice<app_ds::LinearInstance>>,
    RunningIndication<RunChoices<app_ds::LinearInstance>>,
    UpdatingIndication<UpdateChoice<app_ds::TreeInstance>>,
    RunningIndication<RunChoices<app_ds::TreeInstance>>,
    UpdatingIndication<UpdateChoice<app_num::ArithmeticMethod>>,
    RunningIndication<RunChoices<app_num::ArithmeticMethod>>,
    UpdatingIndication<UpdateChoice<app_num::DivisorMethod>>,
    RunningIndication<RunChoices<app_num::DivisorMethod>>,
    UpdatingIndication<UpdateChoice<app_num::IntegralMethod>>,
    RunningIndication<RunChoices<app_num::IntegralMethod>>,
    UpdatingIndication<UpdateChoice<app_num::PrimeMethod>>,
    RunningIndication<RunChoices<app_num::PrimeMethod>>>;

//! @brief Alias for the message handler.
//! @tparam Msg - type of message
template <typename Msg>
using Handler = std::function<void(const Msg&)>;
//! @brief Message dispatcher.
//! @tparam Is - type of indications
template <typename... Is>
class Dispatcher;
//! @brief Message dispatcher of the updating indication.
//! @tparam Msg - type of message
//! @tparam Is - type of indications
template <typename Msg, typename... Is>
class Dispatcher<UpdatingIndication<Msg>, Is...> : public Dispatcher<Is...>
{
public:
    using Dispatcher<Is...>::registerHandler;

    //! @brief Register the handler.
    //! @param handler - message handler
    virtual void registerHandler(Handler<Msg> handler) = 0;
};
//! @brief Message dispatcher of the running indication.
//! @tparam Msg - type of messages
//! @tparam Is - type of indications
template <typename Msg, typename... Is>
class Dispatcher<RunningIndication<Msg>, Is...> : public Dispatcher<Is...>
{
public:
    using Dispatcher<Is...>::registerHandler;

    //! @brief Register the handler.
    //! @param handler - message handler
    virtual void registerHandler(Handler<Msg> handler) = 0;
};
//! @brief Default message dispatcher.
template <>
class Dispatcher<>
{
public:
    //! @brief Register the handler.
    void registerHandler() {}
};
//! @brief Message receiver.
//! @tparam Is - type of indications
template <typename... Is>
class Receiver;
//! @brief Message receiver of the updating indication.
//! @tparam Msg - type of message
//! @tparam Is - type of indications
template <typename Msg, typename... Is>
class Receiver<UpdatingIndication<Msg>, Is...> : public Receiver<Is...>
{
public:
    using Receiver<Is...>::onMessage;

    //! @brief Action on message.
    //! @param message - message body
    virtual void onMessage(const Msg& message) = 0;
};
//! @brief Message receiver of the running indication.
//! @tparam Msg - type of message
//! @tparam Is - type of indications
template <typename Msg, typename... Is>
class Receiver<RunningIndication<Msg>, Is...> : public Receiver<Is...>
{
public:
    using Receiver<Is...>::onMessage;

    //! @brief Action on message.
    //! @param message - message body
    virtual void onMessage(const Msg& message) = 0;
};
//! @brief Default message receiver.
template <>
class Receiver<>
{
public:
    //! @brief Action on message.
    void onMessage() {}
};
//! @brief Forwarding basis for all message types.
struct ForwardBase : public MessageTypes::AsParameterPackFor<Dispatcher>,
                     public MessageTypes::AsParameterPackFor<Receiver>
{
};
//! @brief Forwarding action.
//! @tparam Is - type of indications
template <typename... Is>
class Forward;
//! @brief Forward message of the updating indication.
//! @tparam Msg - type of message
//! @tparam Is - type of indications
template <typename Msg, typename... Is>
class Forward<UpdatingIndication<Msg>, Is...> : public Forward<Is...>
{
public:
    //! @brief Alias for the base class.
    using Base = Forward<Is...>;
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
//! @brief Forward message of the running indication.
//! @tparam Msg - type of message
//! @tparam Is - type of indications
template <typename Msg, typename... Is>
class Forward<RunningIndication<Msg>, Is...> : public Forward<Is...>
{
public:
    //! @brief Alias for the base class.
    using Base = Forward<Is...>;
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
//! @brief Forwarding action interface.
//! @tparam Intf - type of interface
template <typename Intf>
class Forward<Intf> : public Intf
{
};

//! @brief Message forwarder.
class MessageForwarder : public MessageTypes::WithInterface<ForwardBase>::AsParameterPackFor<Forward>
{
};

//! @brief Applying event type object's helper type for the visitor.
//! @tparam Ts - type of visitors
template <class... Ts>
struct EvtTypeOverloaded : Ts...
{
    using Ts::operator()...;
};
//! @brief Explicit deduction guide for EvtTypeOverloaded.
//! @tparam Ts - type of visitors
template <class... Ts>
EvtTypeOverloaded(Ts...) -> EvtTypeOverloaded<Ts...>;
//! @brief Alias for the applied action event type.
using EventType = std::variant<
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

//! @brief Preset full name for the task.
//! @param cli - sub-cli name
//! @param cat - category name
//! @param cho - choice name
//! @return full name
inline std::string presetTaskName(const std::string_view cli, const std::string_view cat, const std::string_view cho)
{
    return '@' + std::string{cli} + '_' + std::string{cat} + '_' + std::string{cho};
}

//! @brief Alias for memory pool when making multi-threading.
using ResourcePool = utility::memory::Memory<utility::thread::Thread>;
extern ResourcePool& resourcePool();
} // namespace action
} // namespace application
