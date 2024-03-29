//! @file command.hpp
//! @author ryftchen
//! @brief The declarations (command) in the application module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#pragma once

#include "application/example/include/apply_algorithm.hpp"
#include "application/example/include/apply_data_structure.hpp"
#include "application/example/include/apply_design_pattern.hpp"
#include "application/example/include/apply_numeric.hpp"
#include "utility/include/argument.hpp"
#include "utility/include/console.hpp"
#include "utility/include/memory.hpp"
#include "utility/include/thread.hpp"

//! @brief The application module.
namespace application // NOLINT(modernize-concat-nested-namespaces)
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
    //! @brief The operator (=) overloading of Command class.
    //! @return reference of the Command object
    Command& operator=(const Command&) = delete;

    //! @brief Get the Command instance.
    //! @return reference of the Command object
    static Command& getInstance();
    //! @brief Interface for running commander.
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
    //! @brief Parse arguments helper for commander.
    utility::argument::Argument mainCLI{"foo", "1.0"};
    //! @brief Parse arguments helper to apply algorithm.
    utility::argument::Argument subCLIAppAlgo{"app-algo"};
    //! @brief Parse arguments helper to apply design pattern.
    utility::argument::Argument subCLIAppDp{"app-dp"};
    //! @brief Parse arguments helper to apply data structure.
    utility::argument::Argument subCLIAppDs{"app-ds"};
    //! @brief Parse arguments helper to apply numeric.
    utility::argument::Argument subCLIAppNum{"app-num"};
    //! @brief Copyright information.
    static constexpr std::string_view copyright{"Copyright (c) 2022-2024 ryftchen. All rights reserved."};

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

    //! @brief Alias for the functor to perform the task.
    typedef void (*PerformTaskFunctor)(const std::vector<std::string>&);
    //! @brief Alias for the functor to update the task.
    typedef void (*UpdateTaskFunctor)(const std::string&);
    //! @brief Alias for the sub-cli.
    using SubCLI = std::string;
    //! @brief Alias for the task category.
    using TaskCategory = std::string;
    //! @brief Alias for the target task.
    using TargetTask = std::string;
    //! @brief Alias for the container of TargetTask.
    using TargetTaskContainer = std::vector<TargetTask>;
    //! @brief Alias for the tuple of PerformTaskFunctor and UpdateTaskFunctor.
    using TaskFunctorTuple = std::tuple<PerformTaskFunctor, UpdateTaskFunctor>;
    //! @brief Alias for the tuple of TargetTaskContainer and TaskFunctorTuple.
    using TaskCategoryTuple = std::tuple<TargetTaskContainer, TaskFunctorTuple>;
    //! @brief Alias for the map of TaskCategory and TaskCategoryTuple.
    using SubCLIMap = std::map<TaskCategory, TaskCategoryTuple>;
    //! @brief Alias for the map of SubCLI and SubCLIMap.
    using RegularTaskMap = std::map<SubCLI, SubCLIMap>;

    //! @brief Get a member of TaskCategoryTuple.
    //! @tparam T - type of member to be got
    //! @param tuple - a tuple containing the member types to be got
    //! @return member corresponding to the specific type
    template <typename T>
    static const T& get(const TaskCategoryTuple& tuple);
    //! @brief Get a member of TaskFunctorTuple.
    //! @tparam T - type of member to be got
    //! @param tuple - a tuple containing the member types to be got
    //! @return member corresponding to the specific type
    template <typename T>
    static const T& get(const TaskFunctorTuple& tuple);

    // clang-format off
    //! @brief Mapping table of all basic tasks.
    const std::map<std::string, void (Command::*)() const> basicTaskDispatcher{
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
        std::bitset<Bottom<Category>::value> primaryBit;

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
            return (
                app_algo::getTask().empty() && app_ds::getTask().empty() && app_dp::getTask().empty()
                && app_num::getTask().empty() && !helpOnly);
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
    };

    //! @brief Manage all types of tasks.
    struct DispatchedTask
    {
        //! @brief Dispatch basic type tasks.
        BasicTask basicTask;
        //! @brief Dispatch regular type tasks.
        RegularTask regularTask;

        //! @brief Check whether any tasks do not exist.
        //! @return any tasks do not exist or exist
        [[nodiscard]] inline bool empty() const { return (basicTask.empty() && regularTask.empty()); }
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
    //! @brief Get ASCII banner text.
    //! @return ASCII banner text content
    static std::string getIconBanner();

    //! @brief Maximum latency (ms) for console.
    static constexpr std::uint16_t maxLatency{200};
};

//! @brief Preset thread name.
//! @param cli - sub-cli
//! @param cat - category
//! @param tgt - target task
//! @return thread name
inline std::string presetTaskName(const std::string_view cli, const std::string_view cat, const std::string_view tgt)
{
    return "task-" + std::string{cli} + '_' + std::string{cat} + '_' + std::string{tgt};
}

//! @brief Alias for memory pool when making multi-threading.
using PublicThreadPool = utility::memory::Memory<utility::thread::Thread>;
extern PublicThreadPool& getPublicThreadPool();
} // namespace command
} // namespace application
