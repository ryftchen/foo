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

//! @brief The application module.
namespace application // NOLINT (modernize-concat-nested-namespaces)
{
//! @brief Command-line-related functions in the application module.
namespace command
{
//! @brief Represent the maximum value of an enum.
//! @tparam T - type of specific enum
template <class T>
struct Bottom;

//! @brief Enumerate specific basic tasks.
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
    //! @brief Argument controller for running commander.
    //! @param argc - argument count
    //! @param argv - argument vector
    void runCommander(const int argc, const char* const argv[]);

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
    utility::argument::Argument mainCLI{"foo", std::string{note::version()}};
    //! @brief Parse argument helper to apply algorithm.
    utility::argument::Argument subCLIAppAlgo{"app-algo", std::string{note::version()}};
    //! @brief Parse argument helper to apply design pattern.
    utility::argument::Argument subCLIAppDp{"app-dp", std::string{note::version()}};
    //! @brief Parse argument helper to apply data structure.
    utility::argument::Argument subCLIAppDs{"app-ds", std::string{note::version()}};
    //! @brief Parse argument helper to apply numeric.
    utility::argument::Argument subCLIAppNum{"app-num", std::string{note::version()}};

    //! @brief Initialize the parse argument helpers.
    void initializeCLI();
    //! @brief Foreground handler for parsing command line arguments.
    //! @param argc - argument count
    //! @param argv - argument vector
    void foregroundHandler(const int argc, const char* const argv[]);
    //! @brief Background handler for performing the specific task.
    void backgroundHandler();
    //! @brief Pre-check the basic task.
    void validateBasicTask();
    //! @brief Pre-check the regular task.
    void validateRegularTask();
    //! @brief Check whether any tasks exist.
    //! @return any tasks exist or do not exist
    bool hasAnyTask() const;
    //! @brief Dispatch specific tasks.
    void dispatchTask();
    //! @brief Execute the command line of console mode.
    void executeConsoleCommand() const;
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
    //! @brief Alias for the category alias.
    using CategoryAlias = std::string;
    //! @brief Alias for the target task.
    using TargetTask = std::string;
    //! @brief Alias for the aggregation of TargetTask.
    using AggregationTasks = std::vector<TargetTask>;
    //! @brief Alias for the functor to run the tasks.
    typedef void (*RunTasksFunctor)(const AggregationTasks&);
    //! @brief Alias for the functor to update the task.
    typedef void (*UpdateTaskFunctor)(const TargetTask&);
    //! @brief Alias for the tuple of RunTasksFunctor and UpdateTaskFunctor.
    using TaskFunctorTuple = std::tuple<RunTasksFunctor, UpdateTaskFunctor>;
    //! @brief Alias for the extend attribute of the target category.
    struct CategoryExtAttr
    {
        //! @brief The aggregation for the candidate task.
        AggregationTasks candidates{};
        //! @brief The tuple containing the callback to be got.
        TaskFunctorTuple callbacks{};
    };
    //! @brief Alias for the map of CategoryName and CategoryExtAttr.
    using SubCLIMap = std::map<CategoryName, CategoryExtAttr>;
    //! @brief Alias for the map of SubCLIName and SubCLIMap.
    using RegularTaskMap = std::map<SubCLIName, SubCLIMap>;

    //! @brief Get a member of TaskFunctorTuple.
    //! @tparam T - type of member to be got
    //! @param tuple - a tuple containing the member types to be got
    //! @return member corresponding to the specific type
    template <typename T>
    static auto get(const TaskFunctorTuple& tuple) -> const T&;
    //! @brief Filter the alias under the sub-cli.
    //! @tparam T - type of regular task corresponding to sub-cli
    //! @param name - mapping for aliases
    template <typename T>
    std::map<CategoryName, CategoryAlias> filterAliasUnderSubCLI(const SubCLIName& name) const;

    // clang-format off
    //! @brief Mapping table of all basic tasks.
    const std::map<CategoryName, void (Command::*)() const> basicTaskDispatcher{
        // - Category -+------------ Run Task ------------
        // ------------+----------------------------------
        { "console"    , &Command::executeConsoleCommand },
        { "dump"       , &Command::dumpConfiguration     },
        { "help"       , &Command::showHelpMessage       },
        { "version"    , &Command::showVersionIcon       }
        // ------------+----------------------------------
    };
    //! @brief Mapping table of all regular tasks.
    const RegularTaskMap regularTaskDispatcher{
        // - Sub-CLI -+--- Category ---+----------------- Task -----------------+----------- Run Tasks -----------+----------- Update Task -----------
        // -----------+----------------+----------------------------------------+---------------------------------+-----------------------------------
        { "app-algo"  , {{ "match"      , {{ "rab", "knu", "boy", "hor", "sun" } , { &app_algo::runMatchTasks     , &app_algo::updateMatchTask     }}},
                         { "notation"   , {{ "pre", "pos"                      } , { &app_algo::runNotationTasks  , &app_algo::updateNotationTask  }}},
                         { "optimal"    , {{ "gra", "ann", "par", "gen"        } , { &app_algo::runOptimalTasks   , &app_algo::updateOptimalTask   }}},
                         { "search"     , {{ "bin", "int", "fib"               } , { &app_algo::runSearchTasks    , &app_algo::updateSearchTask    }}},
                         { "sort"       , {{ "bub", "sel", "ins", "she", "mer",
                                             "qui", "hea", "cou", "buc", "rad" } , { &app_algo::runSortTasks      , &app_algo::updateSortTask      }}}}},
        { "app-dp"    , {{ "behavioral" , {{ "cha", "com", "int", "ite", "med",
                                             "mem", "obs", "sta", "str", "tem",
                                             "vis"                             } , { &app_dp::runBehavioralTasks  , &app_dp::updateBehavioralTask  }}},
                         { "creational" , {{ "abs", "bui", "fac", "pro", "sin" } , { &app_dp::runCreationalTasks  , &app_dp::updateCreationalTask  }}},
                         { "structural" , {{ "ada", "bri", "com", "dec", "fac",
                                             "fly", "pro"                      } , { &app_dp::runStructuralTasks  , &app_dp::updateStructuralTask  }}}}},
        { "app-ds"    , {{ "linear"     , {{ "lin", "sta", "que"               } , { &app_ds::runLinearTasks      , &app_ds::updateLinearTask      }}},
                         { "tree"       , {{ "bin", "ade", "spl"               } , { &app_ds::runTreeTasks        , &app_ds::updateTreeTask        }}}}},
        { "app-num"   , {{ "arithmetic" , {{ "add", "sub", "mul", "div"        } , { &app_num::runArithmeticTasks , &app_num::updateArithmeticTask }}},
                         { "divisor"    , {{ "euc", "ste"                      } , { &app_num::runDivisorTasks    , &app_num::updateDivisorTask    }}},
                         { "integral"   , {{ "tra", "sim", "rom", "gau", "mon" } , { &app_num::runIntegralTasks   , &app_num::updateIntegralTask   }}},
                         { "prime"      , {{ "era", "eul"                      } , { &app_num::runPrimeTasks      , &app_num::updatePrimeTask      }}}}}
        // -----------+----------------+----------------------------------------+---------------------------------+-----------------------------------
    };
    // clang-format on

    //! @brief Manage basic tasks.
    class BasicTask
    {
    public:
        //! @brief Bit flags for managing basic tasks.
        std::bitset<Bottom<Category>::value> primaryBit{};

        //! @brief Check whether any basic tasks do not exist.
        //! @return any basic tasks do not exist or exist
        [[nodiscard]] inline bool empty() const { return primaryBit.none(); }
        //! @brief Reset bit flags that manage basic tasks.
        inline void reset() { primaryBit.reset(); }
    };

    //! @brief Manage regular tasks by sub-cli.
    class RegularTask
    {
    public:
        //! @brief Enumerate specific regular tasks.
        enum SubTask : std::uint8_t
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

        //! @brief Check whether any regular tasks do not exist.
        //! @return any regular tasks do not exist or exist
        [[nodiscard]] inline bool empty() const
        {
            return app_algo::getTask().empty() && app_ds::getTask().empty() && app_dp::getTask().empty()
                && app_num::getTask().empty() && !helpOnly;
        }
        //! @brief Reset bit flags that manage regular tasks.
        inline void reset()
        {
            app_algo::getTask().reset();
            app_ds::getTask().reset();
            app_dp::getTask().reset();
            app_num::getTask().reset();
            helpOnly = false;
        }
        //! @brief Get the existing sub-task.
        //! @return existing sub-task
        SubTask getExistingSubTask()
        {
            const std::uint8_t validation = !app_algo::getTask().empty() + !app_dp::getTask().empty()
                + !app_ds::getTask().empty() + !app_num::getTask().empty();
            if (1 == validation)
            {
                if (!app_algo::getTask().empty())
                {
                    return SubTask::algorithm;
                }
                else if (!app_dp::getTask().empty())
                {
                    return SubTask::designPattern;
                }
                else if (!app_ds::getTask().empty())
                {
                    return SubTask::dataStructure;
                }
                else if (!app_num::getTask().empty())
                {
                    return SubTask::numeric;
                }
            }

            reset();
            throw std::logic_error(
                "The current regular task does not meet the requirement of having only one sub-task.");
        }
    };

    //! @brief Manage all types of tasks.
    struct DispatchedTask
    {
        //! @brief Dispatch basic type tasks.
        BasicTask basicTask{};
        //! @brief Dispatch regular type tasks.
        RegularTask regularTask{};

        //! @brief Check whether any tasks do not exist.
        //! @return any tasks do not exist or exist
        [[nodiscard]] inline bool empty() const { return basicTask.empty() && regularTask.empty(); }
        //! @brief Reset bit flags that manage all tasks.
        inline void reset()
        {
            basicTask.reset();
            regularTask.reset();
        }
    } /** @brief Dispatch all types of tasks. */ dispatchedTask{};

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
//! @param tgt - target task
//! @return thread name
inline std::string presetTaskName(const std::string_view cli, const std::string_view cat, const std::string_view tgt)
{
    return '@' + std::string{cli} + '_' + std::string{cat} + '_' + std::string{tgt};
}

//! @brief Alias for memory pool when making multi-threading.
using PublicThreadPool = utility::memory::Memory<utility::thread::Thread>;
extern PublicThreadPool& getPublicThreadPool();
} // namespace command
} // namespace application

//! @brief Reflect the sub-cli name and alias to the field.
#define COMMAND_REFLECT_SUB_CLI_FIELD(category, alias)            \
    Field                                                         \
    {                                                             \
        REFLECTION_STR(#category), &Type::category##Bit, AttrList \
        {                                                         \
            Attr                                                  \
            {                                                     \
                REFLECTION_STR("alias"), #alias                   \
            }                                                     \
        }                                                         \
    }
//! @brief Reflect the sub-cli's option and task name to the field.
#define COMMAND_REFLECT_TASK_FIELD(method, task)        \
    Field                                               \
    {                                                   \
        REFLECTION_STR(#method), Type::method, AttrList \
        {                                               \
            Attr                                        \
            {                                           \
                REFLECTION_STR("task"), #task           \
            }                                           \
        }                                               \
    }
//! @brief Static reflection for AlgorithmTask. Mapping to regular tasks.
template <>
struct utility::reflection::TypeInfo<application::app_algo::AlgorithmTask>
    : TypeInfoBase<application::app_algo::AlgorithmTask>
{
    //! @brief Name.
    static constexpr std::string_view name{"app-algo"};
    //! @brief Field list.
    static constexpr FieldList fields{
        // clang-format off
        COMMAND_REFLECT_SUB_CLI_FIELD(match   , m),
        COMMAND_REFLECT_SUB_CLI_FIELD(notation, n),
        COMMAND_REFLECT_SUB_CLI_FIELD(optimal , o),
        COMMAND_REFLECT_SUB_CLI_FIELD(search  , s),
        COMMAND_REFLECT_SUB_CLI_FIELD(sort    , S),
        // clang-format on
    };
    //! @brief Attribute list.
    static constexpr AttrList attrs{};
};
//! @brief Static reflection for MatchMethod. Mapping to regular tasks.
template <>
struct utility::reflection::TypeInfo<application::app_algo::MatchMethod>
    : TypeInfoBase<application::app_algo::MatchMethod>
{
    //! @brief Name.
    static constexpr std::string_view name{"match"};
    //! @brief Field list.
    static constexpr FieldList fields{
        // clang-format off
        COMMAND_REFLECT_TASK_FIELD(rabinKarp       , rab),
        COMMAND_REFLECT_TASK_FIELD(knuthMorrisPratt, knu),
        COMMAND_REFLECT_TASK_FIELD(boyerMoore      , boy),
        COMMAND_REFLECT_TASK_FIELD(horspool        , hor),
        COMMAND_REFLECT_TASK_FIELD(sunday          , sun),
        // clang-format on
    };
    //! @brief Attribute list.
    static constexpr AttrList attrs{};
};
//! @brief Static reflection for NotationMethod. Mapping to regular tasks.
template <>
struct utility::reflection::TypeInfo<application::app_algo::NotationMethod>
    : TypeInfoBase<application::app_algo::NotationMethod>
{
    //! @brief Name.
    static constexpr std::string_view name{"notation"};
    //! @brief Field list.
    static constexpr FieldList fields{
        // clang-format off
        COMMAND_REFLECT_TASK_FIELD(prefix , pre),
        COMMAND_REFLECT_TASK_FIELD(postfix, pos),
        // clang-format on
    };
    //! @brief Attribute list.
    static constexpr AttrList attrs{};
};
//! @brief Static reflection for OptimalMethod. Mapping to regular tasks.
template <>
struct utility::reflection::TypeInfo<application::app_algo::OptimalMethod>
    : TypeInfoBase<application::app_algo::OptimalMethod>
{
    //! @brief Name.
    static constexpr std::string_view name{"optimal"};
    //! @brief Field list.
    static constexpr FieldList fields{
        // clang-format off
        COMMAND_REFLECT_TASK_FIELD(gradient , gra),
        COMMAND_REFLECT_TASK_FIELD(annealing, ann),
        COMMAND_REFLECT_TASK_FIELD(particle , par),
        COMMAND_REFLECT_TASK_FIELD(genetic  , gen),
        // clang-format on
    };
    //! @brief Attribute list.
    static constexpr AttrList attrs{};
};
//! @brief Static reflection for SearchMethod. Mapping to regular tasks.
template <>
struct utility::reflection::TypeInfo<application::app_algo::SearchMethod>
    : TypeInfoBase<application::app_algo::SearchMethod>
{
    //! @brief Name.
    static constexpr std::string_view name{"search"};
    //! @brief Field list.
    static constexpr FieldList fields{
        // clang-format off
        COMMAND_REFLECT_TASK_FIELD(binary       , bin),
        COMMAND_REFLECT_TASK_FIELD(interpolation, int),
        COMMAND_REFLECT_TASK_FIELD(fibonacci    , fib),
        // clang-format on
    };
    //! @brief Attribute list.
    static constexpr AttrList attrs{};
};
//! @brief Static reflection for SortMethod. Mapping to regular tasks.
template <>
struct utility::reflection::TypeInfo<application::app_algo::SortMethod>
    : TypeInfoBase<application::app_algo::SortMethod>
{
    //! @brief Name.
    static constexpr std::string_view name{"sort"};
    //! @brief Field list.
    static constexpr FieldList fields{
        // clang-format off
        COMMAND_REFLECT_TASK_FIELD(bubble   , bub),
        COMMAND_REFLECT_TASK_FIELD(selection, sel),
        COMMAND_REFLECT_TASK_FIELD(insertion, ins),
        COMMAND_REFLECT_TASK_FIELD(shell    , she),
        COMMAND_REFLECT_TASK_FIELD(merge    , mer),
        COMMAND_REFLECT_TASK_FIELD(quick    , qui),
        COMMAND_REFLECT_TASK_FIELD(heap     , hea),
        COMMAND_REFLECT_TASK_FIELD(counting , cou),
        COMMAND_REFLECT_TASK_FIELD(bucket   , buc),
        COMMAND_REFLECT_TASK_FIELD(radix    , rad),
        // clang-format on
    };
    //! @brief Attribute list.
    static constexpr AttrList attrs{};
};

//! @brief Static reflection for DesignPatternTask. Mapping to regular tasks.
template <>
struct utility::reflection::TypeInfo<application::app_dp::DesignPatternTask>
    : TypeInfoBase<application::app_dp::DesignPatternTask>
{
    //! @brief Name.
    static constexpr std::string_view name{"app-dp"};
    //! @brief Field list.
    static constexpr FieldList fields{
        // clang-format off
        COMMAND_REFLECT_SUB_CLI_FIELD(behavioral, b),
        COMMAND_REFLECT_SUB_CLI_FIELD(creational, c),
        COMMAND_REFLECT_SUB_CLI_FIELD(structural, s),
        // clang-format on
    };
    //! @brief Attribute list.
    static constexpr AttrList attrs{};
};
//! @brief Static reflection for BehavioralInstance. Mapping to regular tasks.
template <>
struct utility::reflection::TypeInfo<application::app_dp::BehavioralInstance>
    : TypeInfoBase<application::app_dp::BehavioralInstance>
{
    //! @brief Name.
    static constexpr std::string_view name{"behavioral"};
    //! @brief Field list.
    static constexpr FieldList fields{
        // clang-format off
        COMMAND_REFLECT_TASK_FIELD(chainOfResponsibility, cha),
        COMMAND_REFLECT_TASK_FIELD(command              , com),
        COMMAND_REFLECT_TASK_FIELD(interpreter          , int),
        COMMAND_REFLECT_TASK_FIELD(iterator             , ite),
        COMMAND_REFLECT_TASK_FIELD(mediator             , med),
        COMMAND_REFLECT_TASK_FIELD(memento              , mem),
        COMMAND_REFLECT_TASK_FIELD(observer             , obs),
        COMMAND_REFLECT_TASK_FIELD(state                , sta),
        COMMAND_REFLECT_TASK_FIELD(strategy             , str),
        COMMAND_REFLECT_TASK_FIELD(templateMethod       , tem),
        COMMAND_REFLECT_TASK_FIELD(visitor              , vis),
        // clang-format on
    };
    //! @brief Attribute list.
    static constexpr AttrList attrs{};
};
//! @brief Static reflection for CreationalInstance. Mapping to regular tasks.
template <>
struct utility::reflection::TypeInfo<application::app_dp::CreationalInstance>
    : TypeInfoBase<application::app_dp::CreationalInstance>
{
    //! @brief Name.
    static constexpr std::string_view name{"creational"};
    //! @brief Field list.
    static constexpr FieldList fields{
        // clang-format off
        COMMAND_REFLECT_TASK_FIELD(abstractFactory, abs),
        COMMAND_REFLECT_TASK_FIELD(builder        , bui),
        COMMAND_REFLECT_TASK_FIELD(factoryMethod  , fac),
        COMMAND_REFLECT_TASK_FIELD(prototype      , pro),
        COMMAND_REFLECT_TASK_FIELD(singleton      , sin),
        // clang-format on
    };
    //! @brief Attribute list.
    static constexpr AttrList attrs{};
};
//! @brief Static reflection for StructuralInstance. Mapping to regular tasks.
template <>
struct utility::reflection::TypeInfo<application::app_dp::StructuralInstance>
    : TypeInfoBase<application::app_dp::StructuralInstance>
{
    //! @brief Name.
    static constexpr std::string_view name{"structural"};
    //! @brief Field list.
    static constexpr FieldList fields{
        // clang-format off
        COMMAND_REFLECT_TASK_FIELD(adapter  , ada),
        COMMAND_REFLECT_TASK_FIELD(bridge   , bri),
        COMMAND_REFLECT_TASK_FIELD(composite, com),
        COMMAND_REFLECT_TASK_FIELD(decorator, dec),
        COMMAND_REFLECT_TASK_FIELD(facade   , fac),
        COMMAND_REFLECT_TASK_FIELD(flyweight, fly),
        COMMAND_REFLECT_TASK_FIELD(proxy    , pro),
        // clang-format on
    };
    //! @brief Attribute list.
    static constexpr AttrList attrs{};
};

//! @brief Static reflection for DataStructureTask. Mapping to regular tasks.
template <>
struct utility::reflection::TypeInfo<application::app_ds::DataStructureTask>
    : TypeInfoBase<application::app_ds::DataStructureTask>
{
    //! @brief Name.
    static constexpr std::string_view name{"app-ds"};
    //! @brief Field list.
    static constexpr FieldList fields{
        // clang-format off
        COMMAND_REFLECT_SUB_CLI_FIELD(linear, l),
        COMMAND_REFLECT_SUB_CLI_FIELD(tree  , t),
        // clang-format on
    };
    //! @brief Attribute list.
    static constexpr AttrList attrs{};
};
//! @brief Static reflection for LinearInstance. Mapping to regular tasks.
template <>
struct utility::reflection::TypeInfo<application::app_ds::LinearInstance>
    : TypeInfoBase<application::app_ds::LinearInstance>
{
    //! @brief Name.
    static constexpr std::string_view name{"linear"};
    //! @brief Field list.
    static constexpr FieldList fields{
        // clang-format off
        COMMAND_REFLECT_TASK_FIELD(linkedList, lin),
        COMMAND_REFLECT_TASK_FIELD(stack     , sta),
        COMMAND_REFLECT_TASK_FIELD(queue     , que),
        // clang-format on
    };
    //! @brief Attribute list.
    static constexpr AttrList attrs{};
};
//! @brief Static reflection for TreeInstance. Mapping to regular tasks.
template <>
struct utility::reflection::TypeInfo<application::app_ds::TreeInstance>
    : TypeInfoBase<application::app_ds::TreeInstance>
{
    //! @brief Name.
    static constexpr std::string_view name{"tree"};
    //! @brief Field list.
    static constexpr FieldList fields{
        // clang-format off
        COMMAND_REFLECT_TASK_FIELD(binarySearch       , bin),
        COMMAND_REFLECT_TASK_FIELD(adelsonVelskyLandis, ade),
        COMMAND_REFLECT_TASK_FIELD(splay              , spl),
        // clang-format on
    };
    //! @brief Attribute list.
    static constexpr AttrList attrs{};
};

//! @brief Static reflection for NumericTask. Mapping to regular tasks.
template <>
struct utility::reflection::TypeInfo<application::app_num::NumericTask>
    : TypeInfoBase<application::app_num::NumericTask>
{
    //! @brief Name.
    static constexpr std::string_view name{"app-num"};
    //! @brief Field list.
    static constexpr FieldList fields{
        // clang-format off
        COMMAND_REFLECT_SUB_CLI_FIELD(arithmetic, a),
        COMMAND_REFLECT_SUB_CLI_FIELD(divisor   , d),
        COMMAND_REFLECT_SUB_CLI_FIELD(integral  , i),
        COMMAND_REFLECT_SUB_CLI_FIELD(prime     , p),
        // clang-format on
    };
    //! @brief Attribute list.
    static constexpr AttrList attrs{};
};
//! @brief Static reflection for ArithmeticMethod. Mapping to regular tasks.
template <>
struct utility::reflection::TypeInfo<application::app_num::ArithmeticMethod>
    : TypeInfoBase<application::app_num::ArithmeticMethod>
{
    //! @brief Name.
    static constexpr std::string_view name{"arithmetic"};
    //! @brief Field list.
    static constexpr FieldList fields{
        // clang-format off
        COMMAND_REFLECT_TASK_FIELD(addition      , add),
        COMMAND_REFLECT_TASK_FIELD(subtraction   , sub),
        COMMAND_REFLECT_TASK_FIELD(multiplication, mul),
        COMMAND_REFLECT_TASK_FIELD(division      , div),
        // clang-format on
    };
    //! @brief Attribute list.
    static constexpr AttrList attrs{};
};
//! @brief Static reflection for DivisorMethod. Mapping to regular tasks.
template <>
struct utility::reflection::TypeInfo<application::app_num::DivisorMethod>
    : TypeInfoBase<application::app_num::DivisorMethod>
{
    //! @brief Name.
    static constexpr std::string_view name{"divisor"};
    //! @brief Field list.
    static constexpr FieldList fields{
        // clang-format off
        COMMAND_REFLECT_TASK_FIELD(euclidean, euc),
        COMMAND_REFLECT_TASK_FIELD(stein    , ste),
        // clang-format on
    };
    //! @brief Attribute list.
    static constexpr AttrList attrs{};
};
//! @brief Static reflection for IntegralMethod. Mapping to regular tasks.
template <>
struct utility::reflection::TypeInfo<application::app_num::IntegralMethod>
    : TypeInfoBase<application::app_num::IntegralMethod>
{
    //! @brief Name.
    static constexpr std::string_view name{"integral"};
    //! @brief Field list.
    static constexpr FieldList fields{
        // clang-format off
        COMMAND_REFLECT_TASK_FIELD(trapezoidal, tra),
        COMMAND_REFLECT_TASK_FIELD(simpson    , sim),
        COMMAND_REFLECT_TASK_FIELD(romberg    , rom),
        COMMAND_REFLECT_TASK_FIELD(gauss      , gau),
        COMMAND_REFLECT_TASK_FIELD(monteCarlo , mon),
        // clang-format on
    };
    //! @brief Attribute list.
    static constexpr AttrList attrs{};
};
//! @brief Static reflection for PrimeMethod. Mapping to regular tasks.
template <>
struct utility::reflection::TypeInfo<application::app_num::PrimeMethod>
    : TypeInfoBase<application::app_num::PrimeMethod>
{
    //! @brief Name.
    static constexpr std::string_view name{"prime"};
    //! @brief Field list.
    static constexpr FieldList fields{
        // clang-format off
        COMMAND_REFLECT_TASK_FIELD(eratosthenes, era),
        COMMAND_REFLECT_TASK_FIELD(euler       , eul),
        // clang-format on
    };
    //! @brief Attribute list.
    static constexpr AttrList attrs{};
};
#undef COMMAND_REFLECT_SUB_CLI_FIELD
#undef COMMAND_REFLECT_TASK_FIELD
