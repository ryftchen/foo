//! @file command.hpp
//! @author ryftchen
//! @brief The declarations (command) in the application module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#pragma once

#include <condition_variable>
#include "apply_algorithm.hpp"
#include "apply_data_structure.hpp"
#include "apply_design_pattern.hpp"
#include "apply_numeric.hpp"
#include "utility/include/argument.hpp"
#include "utility/include/console.hpp"
#include "utility/include/memory.hpp"
#include "utility/include/thread.hpp"

//! @brief Command-line-related functions in the application module.
namespace application::command
{
//! @brief Execute the command line.
class Command
{
public:
    //! @brief Destroy the Command object.
    virtual ~Command() = default;
    //! @brief Construct a new Command object.
    Command(const Command&) = delete;
    //! @brief The operator (=) overloading of Command class.
    //! @return reference of Command object
    Command& operator=(const Command&) = delete;

    //! @brief Get the Command instance.
    //! @return reference of Command object
    static inline Command& getInstance();
    //! @brief Interface for running commander.
    //! @param argc - argument count
    //! @param argv - argument vector
    void runCommander(const int argc, const char* const argv[]);

private:
    //! @brief Construct a new Command object.
    Command();

    //! @brief Mutex for controlling multi-threading.
    mutable std::mutex commandMutex;
    //! @brief The synchronization condition for foreground and background. Use with commandMutex.
    std::condition_variable commandCondition;
    //! @brief Flag to indicate whether parsing of arguments is completed.
    std::atomic<bool> isParsed{false};
    //! @brief A Argument object for parsing arguments.
    utility::argument::Argument program{utility::argument::Argument("foo", "0.1")};
    static constexpr std::string_view copyrightInfo{"Copyright (c) 2022-2023 ryftchen."};

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
    bool checkTask() const;
    //! @brief Perform specific tasks.
    void performTask() const;

#pragma pack(8)
    //! @brief Manage basic tasks.
    class BasicTask
    {
    public:
        //! @brief Represent the maximum value of an enum.
        //! @tparam T - type of specific enum
        template <class T>
        struct Bottom;

        //! @brief Enumerate specific basic tasks.
        enum Category : uint8_t
        {
            console,
            help,
            version
        };
        //! @brief Store the maximum value of the Category enum.
        template <>
        struct Bottom<Category>
        {
            static constexpr uint8_t value = 3;
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
        enum Category : uint8_t
        {
            algorithm,
            dataStructure,
            designPattern,
            numeric
        };
        //! @brief Store the maximum value of the category type enum.
        template <>
        struct Bottom<Category>
        {
            static constexpr uint8_t value = 4;
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
#pragma pack()
    //! @brief Manage all types of tasks.
    struct AssignedTask
    {
        //! @brief Construct a new AssignedTask object.
        AssignedTask() = default;

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
    } /** @brief A AssignedTask object for managing all types of tasks. */ assignedTask{};

    //! @brief Alias for the functor to perform the task.
    typedef void (*PerformTaskFunctor)(const std::vector<std::string>&);
    //! @brief Alias for the functor to update the task.
    typedef void (*UpdateTaskFunctor)(const std::string&);
    //! @brief Alias for the task category name.
    using TaskCategoryName = std::string;
    //! @brief Alias for the task type name.
    using TaskTypeName = std::string;
    //! @brief Alias for the target task name.
    using TargetTaskName = std::string;
    //! @brief Alias for the vector of TargetTaskName.
    using TargetTaskVector = std::vector<TargetTaskName>;
    //! @brief Alias for the tuple of PerformTaskFunctor and UpdateTaskFunctor.
    using TaskFunctorTuple = std::tuple<PerformTaskFunctor, UpdateTaskFunctor>;
    //! @brief Alias for the tuple of TargetTaskVector and TaskFunctorTuple.
    using TaskTypeTuple = std::tuple<TargetTaskVector, TaskFunctorTuple>;
    //! @brief Alias for the map of TaskTypeName and TaskTypeTuple.
    using TaskCategoryMap = std::map<TaskTypeName, TaskTypeTuple>;
    //! @brief Alias for the map of TaskCategoryName and TaskCategoryMap.
    using GeneralTaskMap = std::map<TaskCategoryName, TaskCategoryMap>;

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
    const std::map<std::string, void (Command::*)() const> basicTaskMap{
        // - Category -+----------- Run -----------
        { "console" , &Command::printConsoleOutput },
        { "help"    , &Command::printHelpMessage   },
        { "version" , &Command::printVersionInfo   },
    };
    //! @brief Mapping table of all general tasks.
    const GeneralTaskMap generalTaskMap{
        // --- Category ---+----- Type -----+---------------- Target ----------------+----------- Run -----------+---------- UpdateTask ----------
        // ----------------+----------------+----------------------------------------+---------------------------+--------------------------------
        { "algorithm"      , {{ "match"      , {{ "rab", "knu", "boy", "hor", "sun" } , { &app_algo::runMatch     , &app_algo::updateMatchTask     }}},
                              { "notation"   , {{ "pre", "pos"                      } , { &app_algo::runNotation  , &app_algo::updateNotationTask  }}},
                              { "optimal"    , {{ "gra", "ann", "par", "gen"        } , { &app_algo::runOptimal   , &app_algo::updateOptimalTask   }}},
                              { "search"     , {{ "bin", "int", "fib"               } , { &app_algo::runSearch    , &app_algo::updateSearchTask    }}},
                              { "sort"       , {{ "bub", "sel", "ins", "she", "mer",
                                                  "qui", "hea", "cou", "buc", "rad" } , { &app_algo::runSort      , &app_algo::updateSortTask      }}}}},
        { "data-structure" , {{ "linear"     , {{ "lin", "sta", "que"               } , { &app_ds::runLinear      , &app_ds::updateLinearTask      }}},
                              { "tree"       , {{ "bin", "ade", "spl"               } , { &app_ds::runTree        , &app_ds::updateTreeTask        }}}}},
        { "design-pattern" , {{ "behavioral" , {{ "cha", "com", "int", "ite", "med",
                                                  "mem", "obs", "sta", "str", "tem",
                                                  "vis"                             } , { &app_dp::runBehavioral  , &app_dp::updateBehavioralTask  }}},
                              { "creational" , {{ "abs", "bui", "fac", "pro", "sin" } , { &app_dp::runCreational  , &app_dp::updateCreationalTask  }}},
                              { "structural" , {{ "ada", "bri", "com", "dec", "fac",
                                                  "fly", "pro"                      } , { &app_dp::runStructural  , &app_dp::updateStructuralTask  }}}}},
        { "numeric"        , {{ "arithmetic" , {{ "add", "sub", "mul", "div"        } , { &app_num::runArithmetic , &app_num::updateArithmeticTask }}},
                              { "divisor"    , {{ "euc", "ste"                      } , { &app_num::runDivisor    , &app_num::updateDivisorTask    }}},
                              { "integral"   , {{ "tra", "sim", "rom", "gau", "mon" } , { &app_num::runIntegral   , &app_num::updateIntegralTask   }}},
                              { "prime"      , {{ "era", "eul"                      } , { &app_num::runPrime      , &app_num::updatePrimeTask      }}}}}
        // ----------------+----------------+----------------------------------------+---------------------------+--------------------------------
    };
    // clang-format on
    //! @brief Max line numbers for printing log.
    static constexpr uint32_t maxLineNumForPrintLog{50};

    //! @brief Print output of the console mode command line.
    void printConsoleOutput() const;
    //! @brief Print help message.
    void printHelpMessage() const;
    //! @brief Print version information.
    void printVersionInfo() const;
    //! @brief Enter console mode.
    void enterConsoleMode() const;
    //! @brief Register the command line to console mode.
    //! @param console - console to be registered
    void registerOnConsole(utility::console::Console& console) const;
    //! @brief View log content.
    static void viewLogContent();
    //! @brief Get ASCII banner text.
    //! @return ASCII banner text content
    static std::string getIconBanner();
    //! @brief Throw an exception when excess arguments.
    [[noreturn]] inline void throwExcessArgumentException();
};

inline Command& Command::getInstance()
{
    static Command commander;
    return commander;
}

inline void Command::throwExcessArgumentException()
{
    assignedTask.reset();
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

extern utility::memory::Memory<utility::thread::Thread>& getMemoryForMultithreading();
} // namespace application::command
