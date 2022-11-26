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

namespace app_command
{
class Command
{
public:
    virtual ~Command() = default;
    Command(const Command&) = delete;
    Command& operator=(const Command&) = delete;

    static inline Command& getInstance();
    void runCommander(const int argc, const char* const argv[]);

private:
    Command();

    mutable std::mutex commandMutex;
    std::condition_variable commandCondition;
    std::atomic<bool> isParsed{false};
    util_argument::Argument program{util_argument::Argument("foo")};
    void foregroundHandle(const int argc, const char* const argv[]);
    void backgroundHandle();
    void validateBasicTask();
    void validateGeneralTask();
    bool checkTask() const;
    void performTask() const;

#pragma pack(8)
    class BasicTask
    {
    public:
        template <class T>
        struct Bottom;

        enum Category
        {
            console,
            help,
            version
        };
        template <>
        struct Bottom<Category>
        {
            static constexpr int value = 3;
        };

        std::bitset<Bottom<Category>::value> primaryBit;

        [[nodiscard]] inline bool empty() const { return primaryBit.none(); }
        inline void reset() { primaryBit.reset(); }
    };

    class GeneralTask
    {
    public:
        template <class T>
        struct Bottom;

        enum Category
        {
            algorithm,
            dataStructure,
            designPattern,
            numeric
        };
        template <>
        struct Bottom<Category>
        {
            static constexpr int value = 4;
        };

        [[nodiscard]] static inline bool empty()
        {
            return (
                app_algo::getTask().empty() && app_ds::getTask().empty() && app_dp::getTask().empty()
                && app_num::getTask().empty());
        }
        static inline void reset()
        {
            app_algo::getTask().reset();
            app_ds::getTask().reset();
            app_dp::getTask().reset();
            app_num::getTask().reset();
        }
    };
#pragma pack()
    struct TaskPlan
    {
        TaskPlan() = default;

        BasicTask basicTask;
        GeneralTask generalTask;

        [[nodiscard]] bool inline empty() const { return (basicTask.empty() && generalTask.empty()); }
        inline void reset()
        {
            basicTask.reset();
            generalTask.reset();
        };
    } taskPlan{};

    typedef void (*PerformTaskFunctor)(const std::vector<std::string>&);
    typedef void (*UpdateTaskFunctor)(const std::string&);
    using TaskCategoryName = std::string;
    using TaskTypeName = std::string;
    using TargetTaskName = std::string;
    using TargetTaskVector = std::vector<TargetTaskName>;
    using TaskFunctorTuple = std::tuple<PerformTaskFunctor, UpdateTaskFunctor>;
    using TaskTypeTuple = std::tuple<TargetTaskVector, TaskFunctorTuple>;
    using TaskCategoryMap = std::map<TaskTypeName, TaskTypeTuple>;
    using GeneralTaskMap = std::map<TaskCategoryName, TaskCategoryMap>;
    template <typename T>
    auto get(const TaskTypeTuple& tuple) const;
    template <typename T>
    auto get(const TaskFunctorTuple& tuple) const;

    // clang-format off
    const std::map<std::string, void (Command::*)() const> basicTaskMap{
        // - Category -+----------- Run -----------
        { "console" , &Command::printConsoleOutput },
        { "help"    , &Command::printHelpMessage   },
        { "version" , &Command::printVersionInfo   },
    };
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
    static constexpr uint32_t maxLineNumForPrintLog{50};
    void printConsoleOutput() const;
    void printHelpMessage() const;
    void printVersionInfo() const;

    void enterConsoleMode() const;
    void registerOnConsole(util_console::Console& console) const;
    static void viewLogContent();
    static std::string getIconBanner();
    [[noreturn]] inline void throwExcessArgumentException();
};

inline Command& Command::getInstance()
{
    static Command commander;
    return commander;
}

inline void Command::throwExcessArgumentException()
{
    taskPlan.reset();
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

extern util_memory::Memory<util_thread::Thread>& getMemoryForMultithreading();
} // namespace app_command
