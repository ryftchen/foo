#pragma once

#include <bitset>
#include <mutex>
#include <variant>
#include "argument.hpp"
#include "console.hpp"
#include "expression.hpp"

class Command
{
public:
    Command();
    ~Command() = default;
    void runCommander(const int argc, const char* const argv[]);

private:
    mutable std::mutex commandMutex;
    util_argument::Argument program{util_argument::Argument("foo")};
    static constexpr uint32_t titleWidthForPrintTask{40};
    static constexpr uint32_t maxLineNumForPrintLog{50};
    void foregroundHandle(const int argc, const char* const argv[]);
    void backgroundHandle() const;
    void validateBasicTask();
    void validateGeneralTask();
    bool checkTask() const;
    void performTask() const;

    template <class T>
    struct Bottom;
    enum BasicTaskCategory
    {
        console,
        help,
        version
    };
    template <>
    struct Bottom<BasicTaskCategory>
    {
        static constexpr int value = 3;
    };

    enum GeneralTaskCategory
    {
        algorithm,
        numeric
    };
    template <>
    struct Bottom<GeneralTaskCategory>
    {
        static constexpr int value = 2;
    };

    enum AlgorithmTaskType
    {
        match,
        notation,
        search,
        sort
    };
    template <>
    struct Bottom<AlgorithmTaskType>
    {
        static constexpr int value = 4;
    };

    enum MatchMethod
    {
        rabinKarp,
        knuthMorrisPratt,
        boyerMoore,
        horspool,
        sunday
    };
    template <>
    struct Bottom<MatchMethod>
    {
        static constexpr int value = 5;
    };

    enum NotationMethod
    {
        prefix,
        postfix
    };
    template <>
    struct Bottom<NotationMethod>
    {
        static constexpr int value = 2;
    };

    enum SearchMethod
    {
        binary,
        interpolation,
        fibonacci
    };
    template <>
    struct Bottom<SearchMethod>
    {
        static constexpr int value = 3;
    };

    enum SortMethod
    {
        bubble,
        selection,
        insertion,
        shell,
        merge,
        quick,
        heap,
        counting,
        bucket,
        radix
    };
    template <>
    struct Bottom<SortMethod>
    {
        static constexpr int value = 10;
    };

    enum NumericTaskType
    {
        arithmetic,
        divisor,
        integral,
        optimum,
        sieve
    };
    template <>
    struct Bottom<NumericTaskType>
    {
        static constexpr int value = 5;
    };

    enum ArithmeticMethod
    {
        addition,
        subtraction,
        multiplication,
        division
    };
    template <>
    struct Bottom<ArithmeticMethod>
    {
        static constexpr int value = 4;
    };

    enum DivisorMethod
    {
        euclid,
        stein
    };
    template <>
    struct Bottom<DivisorMethod>
    {
        static constexpr int value = 2;
    };

    enum IntegralMethod
    {
        trapezoidal,
        simpson,
        romberg,
        gauss,
        monteCarlo
    };
    template <>
    struct Bottom<IntegralMethod>
    {
        static constexpr int value = 5;
    };

    enum OptimumMethod
    {
        gradient,
        annealing,
        particle,
        genetic
    };
    template <>
    struct Bottom<OptimumMethod>
    {
        static constexpr int value = 4;
    };

    enum SieveMethod
    {
        eratosthenes,
        euler
    };
    template <>
    struct Bottom<SieveMethod>
    {
        static constexpr int value = 2;
    };

#pragma pack(8)
    struct TaskPlan
    {
        TaskPlan() = default;
        struct BasicTask
        {
            std::bitset<Bottom<BasicTaskCategory>::value> basicTaskBit;

            [[nodiscard]] bool empty() const { return basicTaskBit.none(); }
            void reset() { basicTaskBit.reset(); }
        } basicTask{};

        struct GeneralTask
        {
            struct AlgoTask
            {
                std::bitset<Bottom<MatchMethod>::value> matchBit;
                std::bitset<Bottom<NotationMethod>::value> notationBit;
                std::bitset<Bottom<SearchMethod>::value> searchBit;
                std::bitset<Bottom<SortMethod>::value> sortBit;

                [[nodiscard]] bool empty() const
                {
                    return (matchBit.none() && notationBit.none() && searchBit.none() && sortBit.none());
                }
                void reset()
                {
                    matchBit.reset();
                    notationBit.reset();
                    searchBit.reset();
                    sortBit.reset();
                }
            } algoTask{};

            struct NumTask
            {
                std::bitset<Bottom<ArithmeticMethod>::value> arithmeticBit;
                std::bitset<Bottom<DivisorMethod>::value> divisorBit;
                std::bitset<Bottom<IntegralMethod>::value> integralBit;
                std::bitset<Bottom<OptimumMethod>::value> optimumBit;
                std::bitset<Bottom<SieveMethod>::value> sieveBit;

                [[nodiscard]] bool empty() const
                {
                    return (
                        arithmeticBit.none() && divisorBit.none() && integralBit.none() && optimumBit.none()
                        && sieveBit.none());
                }
                void reset()
                {
                    arithmeticBit.reset();
                    divisorBit.reset();
                    integralBit.reset();
                    optimumBit.reset();
                    sieveBit.reset();
                }
            } numTask{};

            [[nodiscard]] bool empty() const { return (algoTask.empty() && numTask.empty()); }
            void reset()
            {
                algoTask.reset();
                numTask.reset();
            }
        } generalTask{};

        [[nodiscard]] bool empty() const { return (basicTask.empty() && generalTask.empty()); }
        void reset()
        {
            basicTask.reset();
            generalTask.reset();
        };
    } taskPlan{};
#pragma pack()

    typedef void (Command::*PerformTaskFunctor)() const;
    typedef void (Command::*UpdateTaskFunctor)(const std::string&);
    using TaskCategoryName = std::string;
    using TaskTypeName = std::string;
    using TaskMethodName = std::string;
    using TaskMethodVector = std::vector<TaskMethodName>;
    using TaskFunctorTuple = std::tuple<PerformTaskFunctor, UpdateTaskFunctor>;
    using TaskTypeTuple = std::tuple<TaskMethodVector, TaskFunctorTuple>;
    using TaskCategoryMap = std::map<TaskTypeName, TaskTypeTuple>;
    using BasicTaskMap = std::map<TaskCategoryName, PerformTaskFunctor>;
    using GeneralTaskMap = std::map<TaskCategoryName, TaskCategoryMap>;
    template <typename T>
    auto get(const TaskTypeTuple& tuple) const;
    template <typename T>
    auto get(const TaskFunctorTuple& tuple) const;

    // clang-format off
    static constexpr std::string_view optionTreeOfHelpMsg{
        "├── -a --algorithm\r\n"
        "│   ├── match\r\n"
        "│   │   └── rab, knu, boy, hor, sun\r\n"
        "│   ├── notation\r\n"
        "│   │   └── pre, pos\r\n"
        "│   ├── search\r\n"
        "│   │   └── bin, int, fib\r\n"
        "│   └── sort\r\n"
        "│       ├── bub, sel, ins, she, mer\r\n"
        "│       └── qui, hea, cou, buc, rad\r\n"
        "└── -n --numeric\r\n"
        "    ├── arithmetic\r\n"
        "    │   └── add, sub, mul, div\r\n"
        "    ├── divisor\r\n"
        "    │   └── euc, ste\r\n"
        "    ├── integral\r\n"
        "    │   └── tra, sim, rom, gau, mon\r\n"
        "    ├── optimum\r\n"
        "    │   └── gra, ann, par, gen\r\n"
        "    └── sieve\r\n"
        "        └── era, eul"
    };
    const BasicTaskMap basicTaskMap{
        // - Category -+----------- Run -----------
        { "console" , &Command::printConsoleOutput },
        { "help"    , &Command::printHelpMessage   },
        { "version" , &Command::printVersionInfo   },
    };
    const GeneralTaskMap generalTaskMap{
        // - Category -+----- Type -----+---------------- Method ----------------+----------- Run -----------+---------- UpdateTask ----------
        // ------------+----------------+----------------------------------------+---------------------------+--------------------------------
        { "algorithm" , {{ "match"      , {{ "rab", "knu", "boy", "hor", "sun" } , { &Command::runMatch      , &Command::updateMatchTask      }}},
                         { "notation"   , {{ "pre", "pos"                      } , { &Command::runNotation   , &Command::updateNotationTask   }}},
                         { "search"     , {{ "bin", "int", "fib"               } , { &Command::runSearch     , &Command::updateSearchTask     }}},
                         { "sort"       , {{ "bub", "sel", "ins", "she", "mer",
                                             "qui", "hea", "cou", "buc", "rad" } , { &Command::runSort       , &Command::updateSortTask       }}}}},
        { "numeric"   , {{ "arithmetic" , {{ "add", "sub", "mul", "div"        } , { &Command::runArithmetic , &Command::updateArithmeticTask }}},
                         { "divisor"    , {{ "euc", "ste"                      } , { &Command::runDivisor    , &Command::updateDivisorTask    }}},
                         { "integral"   , {{ "tra", "sim", "rom", "gau", "mon" } , { &Command::runIntegral   , &Command::updateIntegralTask   }}},
                         { "optimum"    , {{ "gra", "ann", "par", "gen"        } , { &Command::runOptimum    , &Command::updateOptimumTask    }}},
                         { "sieve"      , {{ "era", "eul"                      } , { &Command::runSieve      , &Command::updateSieveTask      }}}}}
        // ------------+----------------+----------------------------------------+---------------------------+--------------------------------
    };
    // clang-format on
    void printConsoleOutput() const;
    void printHelpMessage() const;
    void printVersionInfo() const;
    void runMatch() const;
    void updateMatchTask(const std::string& method);
    void runNotation() const;
    void updateNotationTask(const std::string& method);
    void runSearch() const;
    void updateSearchTask(const std::string& method);
    void runSort() const;
    void updateSortTask(const std::string& method);
    void runArithmetic() const;
    void updateArithmeticTask(const std::string& method);
    void runDivisor() const;
    void updateDivisorTask(const std::string& method);
    void runIntegral() const;
    void updateIntegralTask(const std::string& method);
    void runOptimum() const;
    void updateOptimumTask(const std::string& method);
    void runSieve() const;
    void updateSieveTask(const std::string& method);

    typedef std::variant<num_expression::Function1, num_expression::Function2> IntegralExprTarget;
    const std::
        unordered_multimap<num_expression::ExprRange<double, double>, IntegralExprTarget, num_expression::ExprMapHash>
            integralExprMap{
                {{num_expression::Function1::range1,
                  num_expression::Function1::range2,
                  num_expression::Function1::integralExpr},
                 num_expression::Function1()},
                {{num_expression::Function2::range1,
                  num_expression::Function2::range2,
                  num_expression::Function2::integralExpr},
                 num_expression::Function2()}};
    typedef std::variant<num_expression::Griewank, num_expression::Rastrigin> OptimumExprTarget;
    const std::
        unordered_multimap<num_expression::ExprRange<double, double>, OptimumExprTarget, num_expression::ExprMapHash>
            optimumExprMap{
                {{num_expression::Griewank::range1,
                  num_expression::Griewank::range2,
                  num_expression::Griewank::optimumExpr},
                 num_expression::Griewank()},
                {{num_expression::Rastrigin::range1,
                  num_expression::Rastrigin::range2,
                  num_expression::Rastrigin::optimumExpr},
                 num_expression::Rastrigin()}};

    void enterConsole() const;
    void registerOnConsole(util_console::Console& console) const;
    static void viewLogContent();
    static std::string getIconBanner();
    [[noreturn]] void throwExcessArgumentException();
    [[noreturn]] void throwUnexpectedMethodException(const std::string& info);

protected:
    friend std::ostream& operator<<(std::ostream& os, const AlgorithmTaskType& taskType);
    friend std::ostream& operator<<(std::ostream& os, const NumericTaskType& taskType);
};

template <typename T>
auto Command::get(const TaskTypeTuple& tuple) const
{
    if constexpr (std::is_same_v<T, TaskMethodVector>)
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
