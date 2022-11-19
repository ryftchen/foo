#pragma once

#include <condition_variable>
#include "run_algorithm.hpp"
#include "run_data_structure.hpp"
#include "run_design_pattern.hpp"
#include "run_numeric.hpp"
#include "utility/include/argument.hpp"
#include "utility/include/console.hpp"

class Command
{
public:
    Command();
    virtual ~Command() = default;
    Command(const Command&) = delete;
    Command& operator=(const Command&) = delete;

    void runCommander(const int argc, const char* const argv[]);

private:
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

        [[nodiscard]] bool empty() const { return primaryBit.none(); }
        void reset() { primaryBit.reset(); }
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

        [[nodiscard]] static bool empty()
        {
            return (
                run_algo::getTask().empty() && run_ds::getTask().empty() && run_dp::getTask().empty()
                && run_num::getTask().empty());
        }
        static void reset()
        {
            run_algo::getTask().reset();
            run_ds::getTask().reset();
            run_dp::getTask().reset();
            run_num::getTask().reset();
        }
    };
#pragma pack()
    struct TaskPlan
    {
        TaskPlan() = default;

        BasicTask basicTask;
        GeneralTask generalTask;

        [[nodiscard]] bool empty() const { return (basicTask.empty() && generalTask.empty()); }
        void reset()
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
        { "algorithm"      , {{ "match"      , {{ "rab", "knu", "boy", "hor", "sun" } , { &run_algo::runMatch     , &run_algo::updateMatchTask     }}},
                              { "notation"   , {{ "pre", "pos"                      } , { &run_algo::runNotation  , &run_algo::updateNotationTask  }}},
                              { "optimal"    , {{ "gra", "ann", "par", "gen"        } , { &run_algo::runOptimal   , &run_algo::updateOptimalTask   }}},
                              { "search"     , {{ "bin", "int", "fib"               } , { &run_algo::runSearch    , &run_algo::updateSearchTask    }}},
                              { "sort"       , {{ "bub", "sel", "ins", "she", "mer",
                                                  "qui", "hea", "cou", "buc", "rad" } , { &run_algo::runSort      , &run_algo::updateSortTask      }}}}},
        { "data-structure" , {{ "linear"     , {{ "lin", "sta", "que"               } , { &run_ds::runLinear      , &run_ds::updateLinearTask      }}},
                              { "tree"       , {{ "bin", "ade", "spl"               } , { &run_ds::runTree        , &run_ds::updateTreeTask        }}}}},
        { "design-pattern" , {{ "behavioral" , {{ "cha", "com", "int", "ite", "med",
                                                  "mem", "obs", "sta", "str", "tem",
                                                  "vis"                             } , { &run_dp::runBehavioral  , &run_dp::updateBehavioralTask  }}},
                              { "creational" , {{ "abs", "bui", "fac", "pro", "sin" } , { &run_dp::runCreational  , &run_dp::updateCreationalTask  }}},
                              { "structural" , {{ "ada", "bri", "com", "dec", "fac",
                                                  "fly", "pro"                      } , { &run_dp::runStructural  , &run_dp::updateStructuralTask  }}}}},
        { "numeric"        , {{ "arithmetic" , {{ "add", "sub", "mul", "div"        } , { &run_num::runArithmetic , &run_num::updateArithmeticTask }}},
                              { "divisor"    , {{ "euc", "ste"                      } , { &run_num::runDivisor    , &run_num::updateDivisorTask    }}},
                              { "integral"   , {{ "tra", "sim", "rom", "gau", "mon" } , { &run_num::runIntegral   , &run_num::updateIntegralTask   }}},
                              { "prime"      , {{ "era", "eul"                      } , { &run_num::runPrime      , &run_num::updatePrimeTask      }}}}}
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
