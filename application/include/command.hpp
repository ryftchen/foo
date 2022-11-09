#pragma once

#include <condition_variable>
#include "algorithm/test/algorithm_test.hpp"
#include "data_structure/test/data_structure_test.hpp"
#include "design_pattern/test/design_pattern_test.hpp"
#include "numeric/test/numeric_test.hpp"
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
                algo_tst::getTask().empty() && ds_tst::getTask().empty() && dp_tst::getTask().empty()
                && num_tst::getTask().empty());
        }
        static void reset()
        {
            algo_tst::getTask().reset();
            ds_tst::getTask().reset();
            dp_tst::getTask().reset();
            num_tst::getTask().reset();
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
        { "algorithm"      , {{ "match"      , {{ "rab", "knu", "boy", "hor", "sun" } , { &algo_tst::runMatch     , &algo_tst::updateMatchTask     }}},
                              { "notation"   , {{ "pre", "pos"                      } , { &algo_tst::runNotation  , &algo_tst::updateNotationTask  }}},
                              { "optimal"    , {{ "gra", "ann", "par", "gen"        } , { &algo_tst::runOptimal   , &algo_tst::updateOptimalTask    }}},
                              { "search"     , {{ "bin", "int", "fib"               } , { &algo_tst::runSearch    , &algo_tst::updateSearchTask    }}},
                              { "sort"       , {{ "bub", "sel", "ins", "she", "mer",
                                                  "qui", "hea", "cou", "buc", "rad" } , { &algo_tst::runSort      , &algo_tst::updateSortTask      }}}}},
        { "data-structure" , {{ "linear"     , {{ "lin", "sta", "que"               } , { &ds_tst::runLinear      , &ds_tst::updateLinearTask      }}},
                              { "tree"       , {{ "bin", "ade"                      } , { &ds_tst::runTree        , &ds_tst::updateTreeTask        }}}}},
        { "design-pattern" , {{ "behavioral" , {{ "cha", "com", "int", "ite", "med",
                                                  "mem", "obs", "sta", "str", "tem",
                                                  "vis"                             } , { &dp_tst::runBehavioral  , &dp_tst::updateBehavioralTask  }}},
                              { "creational" , {{ "abs", "bui", "fac", "pro", "sin" } , { &dp_tst::runCreational  , &dp_tst::updateCreationalTask  }}},
                              { "structural" , {{ "ada", "bri", "com", "dec", "fac",
                                                  "fly", "pro"                      } , { &dp_tst::runStructural  , &dp_tst::updateStructuralTask  }}}}},
        { "numeric"        , {{ "arithmetic" , {{ "add", "sub", "mul", "div"        } , { &num_tst::runArithmetic , &num_tst::updateArithmeticTask }}},
                              { "divisor"    , {{ "euc", "ste"                      } , { &num_tst::runDivisor    , &num_tst::updateDivisorTask    }}},
                              { "integral"   , {{ "tra", "sim", "rom", "gau", "mon" } , { &num_tst::runIntegral   , &num_tst::updateIntegralTask   }}},
                              { "prime"      , {{ "era", "eul"                      } , { &num_tst::runPrime      , &num_tst::updatePrimeTask      }}}}}
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
