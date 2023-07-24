//! @file command.hpp
//! @author ryftchen
//! @brief The declarations (command) in the application module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen.

#pragma once

#include "application/example/include/apply_algorithm.hpp"
#include "application/example/include/apply_data_structure.hpp"
#include "application/example/include/apply_design_pattern.hpp"
#include "application/example/include/apply_numeric.hpp"
#include "utility/include/argument.hpp"
#include "utility/include/console.hpp"
#include "utility/include/memory.hpp"
#include "utility/include/thread.hpp"

//! @brief Command-line-related functions in the application module.
namespace application::command
{
//! @brief Execute the command line.
class Command final
{
public:
    //! @brief Destroy the Command object.
    virtual ~Command();
    //! @brief Construct a new Command object.
    Command(const Command&) = delete;
    //! @brief The operator (=) overloading of Command class.
    //! @return reference of Command object
    Command& operator=(const Command&) = delete;

    //! @brief Get the Command instance.
    //! @return reference of Command object
    static Command& getInstance();
    //! @brief Interface for running commander.
    //! @param argc - argument count
    //! @param argv - argument vector
    void runCommander(const int argc, const char* const argv[]);

private:
    //! @brief Construct a new Command object.
    Command();

    //! @brief Mutex for controlling multi-threading.
    mutable std::mutex mtx;
    //! @brief The synchronization condition for foreground and background. Use with mtx.
    std::condition_variable cv;
    //! @brief Flag to indicate whether parsing of arguments is completed.
    std::atomic<bool> isParsed{false};
    //! @brief A Argument object for parsing arguments.
    utility::argument::Argument program{utility::argument::Argument("foo", "1.0")};
    //! @brief Copyright information.
    static constexpr std::string_view copyrightInfo{"Copyright (c) 2022-2023 ryftchen"};

    //! @brief Foreground handler for parsing command line arguments.
    //! @param argc - argument count
    //! @param argv - argument vector
    void foregroundHandler(const int argc, const char* const argv[]);
    //! @brief Background handler for performing the specific task.
    void backgroundHandler();
    //! @brief Pre-check the basic task.
    void validateBasicTask();
    //! @brief Pre-check the general task.
    void validateGeneralTask();
    //! @brief Check whether any tasks exist.
    //! @return any tasks exist or do not exist
    bool hasAnyTask() const;
    //! @brief Dispatch specific tasks.
    void dispatchTask() const;

    //! @brief Alias for the functor to perform the task.
    typedef void (*PerformTaskFunctor)(const std::vector<std::string>&);
    //! @brief Alias for the functor to update the task.
    typedef void (*UpdateTaskFunctor)(const std::string&);
    //! @brief Alias for the task category.
    using TaskCategory = std::string;
    //! @brief Alias for the task type.
    using TaskType = std::string;
    //! @brief Alias for the target task.
    using TargetTask = std::string;
    //! @brief Alias for the vector of TargetTask.
    using TargetTaskVector = std::vector<TargetTask>;
    //! @brief Alias for the tuple of PerformTaskFunctor and UpdateTaskFunctor.
    using TaskFunctorTuple = std::tuple<PerformTaskFunctor, UpdateTaskFunctor>;
    //! @brief Alias for the tuple of TargetTaskVector and TaskFunctorTuple.
    using TaskTypeTuple = std::tuple<TargetTaskVector, TaskFunctorTuple>;
    //! @brief Alias for the map of TaskType and TaskTypeTuple.
    using TaskCategoryMap = std::map<TaskType, TaskTypeTuple>;
    //! @brief Alias for the map of TaskCategory and TaskCategoryMap.
    using GeneralTaskMap = std::map<TaskCategory, TaskCategoryMap>;

    //! @brief Get category completion for verification.
    //! @param category - expected category
    //! @param input - input value to be verified
    //! @return category after completing
    TaskCategory getCategoryCompletionForVerification(const TaskCategory& category, const std::string& input) const;
    //! @memberof application::command::Command
    //! @brief Get a member of TaskTypeTuple.
    //! @tparam T - type of member to be got
    //! @param tuple - a tuple containing the member types to be got
    //! @return member corresponding to the specific type
    template <typename T>
    auto get(const TaskTypeTuple& tuple) const;
    //! @memberof application::command::Command
    //! @brief Get a member of TaskFunctorTuple.
    //! @tparam T - type of member to be got
    //! @param tuple - a tuple containing the member types to be got
    //! @return member corresponding to the specific type
    template <typename T>
    auto get(const TaskFunctorTuple& tuple) const;

    // clang-format off
    //! @brief Mapping table of all basic tasks.
    const std::map<std::string, void (Command::*)() const> basicTaskDispatcher{
        // - Category -+------------ Show ------------
        // ------------+------------------------------
        { "console"    , &Command::showConsoleOutput },
        { "help"       , &Command::showHelpMessage   },
        { "version"    , &Command::showVersionInfo   },
        // ------------+------------------------------
    };
    //! @brief Mapping table of all general tasks.
    const GeneralTaskMap generalTaskDispatcher{
        // --- Category ---+----- Type -----+----------------- Task -----------------+----------- Run Tasks -----------+----------- Update Task -----------
        // ----------------+----------------+----------------------------------------+---------------------------------+-----------------------------------
        { "algorithm"      , {{ "match"      , {{ "rab", "knu", "boy", "hor", "sun" } , { &app_algo::runMatchTasks     , &app_algo::updateMatchTask     }}},
                              { "notation"   , {{ "pre", "pos"                      } , { &app_algo::runNotationTasks  , &app_algo::updateNotationTask  }}},
                              { "optimal"    , {{ "gra", "ann", "par", "gen"        } , { &app_algo::runOptimalTasks   , &app_algo::updateOptimalTask   }}},
                              { "search"     , {{ "bin", "int", "fib"               } , { &app_algo::runSearchTasks    , &app_algo::updateSearchTask    }}},
                              { "sort"       , {{ "bub", "sel", "ins", "she", "mer",
                                                  "qui", "hea", "cou", "buc", "rad" } , { &app_algo::runSortTasks      , &app_algo::updateSortTask      }}}}},
        { "data-structure" , {{ "linear"     , {{ "lin", "sta", "que"               } , { &app_ds::runLinearTasks      , &app_ds::updateLinearTask      }}},
                              { "tree"       , {{ "bin", "ade", "spl"               } , { &app_ds::runTreeTasks        , &app_ds::updateTreeTask        }}}}},
        { "design-pattern" , {{ "behavioral" , {{ "cha", "com", "int", "ite", "med",
                                                  "mem", "obs", "sta", "str", "tem",
                                                  "vis"                             } , { &app_dp::runBehavioralTasks  , &app_dp::updateBehavioralTask  }}},
                              { "creational" , {{ "abs", "bui", "fac", "pro", "sin" } , { &app_dp::runCreationalTasks  , &app_dp::updateCreationalTask  }}},
                              { "structural" , {{ "ada", "bri", "com", "dec", "fac",
                                                  "fly", "pro"                      } , { &app_dp::runStructuralTasks  , &app_dp::updateStructuralTask  }}}}},
        { "numeric"        , {{ "arithmetic" , {{ "add", "sub", "mul", "div"        } , { &app_num::runArithmeticTasks , &app_num::updateArithmeticTask }}},
                              { "divisor"    , {{ "euc", "ste"                      } , { &app_num::runDivisorTasks    , &app_num::updateDivisorTask    }}},
                              { "integral"   , {{ "tra", "sim", "rom", "gau", "mon" } , { &app_num::runIntegralTasks   , &app_num::updateIntegralTask   }}},
                              { "prime"      , {{ "era", "eul"                      } , { &app_num::runPrimeTasks      , &app_num::updatePrimeTask      }}}}}
        // ----------------+----------------+----------------------------------------+---------------------------------+-----------------------------------
    };
    // clang-format on

    //! @brief Manage basic tasks.
    class BasicTask
    {
    public:
        //! @brief Represent the maximum value of an enum.
        //! @tparam T - type of specific enum
        template <class T>
        struct Bottom;

        //! @brief Enumerate specific basic tasks.
        enum Category : std::uint8_t
        {
            console,
            help,
            version
        };
        //! @brief Store the maximum value of the Category enum.
        template <>
        struct Bottom<Category>
        {
            //! @brief Maximum value of the Category enum.
            static constexpr std::uint8_t value{3};
        };

        //! @brief Bit flags for managing basic tasks.
        std::bitset<Bottom<Category>::value> primaryBit;

        //! @brief Check whether any basic tasks do not exist.
        //! @return any basic tasks do not exist or exist
        [[nodiscard]] inline bool empty() const { return primaryBit.none(); }
        //! @brief Reset bit flags that manage basic tasks.
        inline void reset() { primaryBit.reset(); }
    };

    //! @brief Manage general tasks.
    class GeneralTask
    {
    public:
        //! @brief Represent the maximum value of an enum.
        //! @tparam T - type of specific enum
        template <class T>
        struct Bottom;

        //! @brief Enumerate specific general tasks.
        enum Category : std::uint8_t
        {
            algorithm,
            dataStructure,
            designPattern,
            numeric
        };
        //! @brief Store the maximum value of the Category enum.
        template <>
        struct Bottom<Category>
        {
            //! @brief Maximum value of the Category enum.
            static constexpr std::uint8_t value{4};
        };

        //! @brief Check whether any general tasks do not exist.
        //! @return any general tasks do not exist or exist
        [[nodiscard]] static inline bool empty()
        {
            return (
                app_algo::getTask().empty() && app_ds::getTask().empty() && app_dp::getTask().empty()
                && app_num::getTask().empty());
        }
        //! @brief Reset bit flags that manage general tasks.
        static inline void reset()
        {
            app_algo::getTask().reset();
            app_ds::getTask().reset();
            app_dp::getTask().reset();
            app_num::getTask().reset();
        }
    };

    //! @brief Manage all types of tasks.
    struct DispatchedTask
    {
        //! @brief Construct a new DispatchedTask object.
        DispatchedTask() = default;

        //! @brief A BasicTask object for managing basic type tasks.
        BasicTask basicTask;
        //! @brief A GeneralTask object for managing general type tasks.
        GeneralTask generalTask;

        //! @brief Check whether any tasks do not exist.
        //! @return any tasks do not exist or exist
        [[nodiscard]] bool inline empty() const { return (basicTask.empty() && generalTask.empty()); }
        //! @brief Reset bit flags that manage all tasks.
        inline void reset()
        {
            basicTask.reset();
            generalTask.reset();
        };
    } /** @brief A DispatchedTask object for managing all types of tasks. */ dispatchedTask{};

    //! @brief Print output of the console mode command line.
    void showConsoleOutput() const;
    //! @brief Print help message.
    void showHelpMessage() const;
    //! @brief Print version information.
    void showVersionInfo() const;
    //! @brief Enter console mode.
    void enterConsoleMode() const;
    //! @brief Register the command line to console mode.
    //! @tparam T type of client
    //! @param console - console to be registered
    //! @param client - client used to send
    template <typename T>
    void registerOnConsole(utility::console::Console& console, T& client) const;
    //! @brief Get ASCII banner text.
    //! @return ASCII banner text content
    static std::string getIconBanner();
    //! @brief Throw an exception when excess arguments.
    [[noreturn]] inline void throwExcessArgumentException();

    //! @brief Latency for console.
    static constexpr std::uint16_t latency{10};
    //! @brief Maximum latency for console.
    static constexpr std::uint16_t maxLatency{200};
};

inline void Command::throwExcessArgumentException()
{
    dispatchedTask.reset();
    throw std::runtime_error("Excess argument.");
}

template <typename T>
auto Command::get(const TaskTypeTuple& tuple) const
{
    if constexpr (std::is_same_v<T, TargetTaskVector>)
    {
        return std::get<0>(tuple);
    }
    else if constexpr (std::is_same_v<T, TaskFunctorTuple>)
    {
        return std::get<1>(tuple);
    }
}

template <typename T>
auto Command::get(const TaskFunctorTuple& tuple) const
{
    if constexpr (std::is_same_v<T, PerformTaskFunctor>)
    {
        return std::get<0>(tuple);
    }
    else if constexpr (std::is_same_v<T, UpdateTaskFunctor>)
    {
        return std::get<1>(tuple);
    }
}

//! @brief Alias for memory pool when making multi-threading.
using PublicThreadPool = utility::memory::Memory<utility::thread::Thread>;

extern PublicThreadPool& getPublicThreadPool();
} // namespace application::command
