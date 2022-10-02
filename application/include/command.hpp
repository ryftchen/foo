#pragma once

#include <algorithm>
#include <bitset>
#include <mutex>
#include "argument.hpp"
#include "console.hpp"
#include "expression.hpp"
#include "match.hpp"
#include "sort.hpp"

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
        sort
    };
    template <>
    struct Bottom<AlgorithmTaskType>
    {
        static constexpr int value = 2;
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

    enum NumericTaskType
    {
        integral,
        optimum
    };
    template <>
    struct Bottom<NumericTaskType>
    {
        static constexpr int value = 2;
    };

    enum OptimumMethod
    {
        fibonacci,
        gradient,
        annealing,
        particle,
        genetic
    };
    template <>
    struct Bottom<OptimumMethod>
    {
        static constexpr int value = 5;
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
                std::bitset<Bottom<SortMethod>::value> sortBit;

                [[nodiscard]] bool empty() const { return matchBit.none() && sortBit.none(); }
                void reset()
                {
                    matchBit.reset();
                    sortBit.reset();
                }
            } algoTask{};

            struct NumTask
            {
                std::bitset<Bottom<IntegralMethod>::value> integralBit;
                std::bitset<Bottom<OptimumMethod>::value> optimumBit;

                [[nodiscard]] bool empty() const { return integralBit.none() && optimumBit.none(); }
                void reset()
                {
                    integralBit.reset();
                    optimumBit.reset();
                }
            } numTask{};

            [[nodiscard]] bool empty() const { return algoTask.empty() && numTask.empty(); }
            void reset()
            {
                algoTask.reset();
                numTask.reset();
            }
        } generalTask{};

        [[nodiscard]] bool empty() const { return basicTask.empty() && generalTask.empty(); }
        void reset()
        {
            basicTask.reset();
            generalTask.reset();
        };
    } taskPlan{};
#pragma pack()

    typedef void (Command::*PerformTaskFunctor)() const;
    typedef void (Command::*SetTaskBitFunctor)(const std::string&);
    using TaskCategoryName = std::string;
    using TaskTypeName = std::string;
    using TaskMethodName = std::string;
    using TaskMethodVector = std::vector<TaskMethodName>;
    using TaskFunctorTuple = std::tuple<PerformTaskFunctor, SetTaskBitFunctor>;
    using TaskTypeTuple = std::tuple<TaskMethodVector, TaskFunctorTuple>;
    using TaskCategoryMap = std::map<TaskTypeName, TaskTypeTuple>;
    using BasicTaskMap = std::map<TaskCategoryName, PerformTaskFunctor>;
    using GeneralTaskMap = std::map<TaskCategoryName, TaskCategoryMap>;
    template <typename T>
    auto get(const TaskTypeTuple& tuple) const;
    template <typename T>
    auto get(const TaskFunctorTuple& tuple) const;

    const BasicTaskMap basicTaskMap{
        {"console", &Command::printConsoleOutput},
        {"help", &Command::printHelpMessage},
        {"version", &Command::printVersionInfo},
    };
    void printConsoleOutput() const;
    void printHelpMessage() const;
    void printVersionInfo() const;
    void enterConsole() const;
    void registerOnConsole(util_console::Console& console) const;
    static void viewLogContent();
    static std::string getIconBanner();

    // clang-format off
    const GeneralTaskMap generalTaskMap{
        {"algorithm",
         {{"match",
          {{"rab", "knu", "boy", "hor", "sun"},
           {&Command::runMatch, &Command::setMatchBit}}},
          {"sort",
          {{"bub", "sel", "ins", "she", "mer", "qui", "hea", "cou", "buc", "rad"},
           {&Command::runSort, &Command::setSortBit}}}}},
        {"numeric",
         {{"integral",
          {{"tra", "sim", "rom", "gau", "mon"},
           {&Command::runIntegral, &Command::setIntegralBit}}},
          {"optimum",
          {{"fib", "gra", "ann", "par", "gen"},
           {&Command::runOptimum, &Command::setOptimumBit}}}}}};
    // clang-format on
    void runMatch() const;
    void setMatchBit(const std::string& method);
    void getMatchResult(const std::shared_ptr<algo_match::Match>& match) const;
    void runSort() const;
    void setSortBit(const std::string& method);
    template <typename T>
    void getSortResult(const std::shared_ptr<algo_sort::Sort<T>>& sort) const;
    void runIntegral() const;
    void setIntegralBit(const std::string& method);
    void getIntegralResult(
        const num_expression::Expression& express,
        const double lowerLimit,
        const double upperLimit,
        const double epsilon) const;
    const std::unordered_multimap<
        num_expression::ExprRange<double, double>,
        num_expression::ExprTarget,
        num_expression::ExprMapHash>
        integralExprMap{
            {{num_expression::Function1::range1,
              num_expression::Function1::range2,
              num_expression::Function1::integralExpr},
             num_expression::Function1()},
            {{num_expression::Function2::range1,
              num_expression::Function2::range2,
              num_expression::Function2::integralExpr},
             num_expression::Function2()}};
    void runOptimum() const;
    void setOptimumBit(const std::string& method);
    void getOptimumResult(
        const num_expression::Expression& express,
        const double leftEndpoint,
        const double rightEndpoint,
        const double epsilon) const;
    const std::unordered_multimap<
        num_expression::ExprRange<double, double>,
        num_expression::ExprTarget,
        num_expression::ExprMapHash>
        optimumExprMap{
            {{num_expression::Function1::range1,
              num_expression::Function1::range2,
              num_expression::Function1::optimumExpr},
             num_expression::Function1()},
            {{num_expression::Function2::range1,
              num_expression::Function2::range2,
              num_expression::Function2::optimumExpr},
             num_expression::Function2()}};

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
    else if constexpr (std::is_same_v<T, SetTaskBitFunctor>)
    {
        return std::get<1>(tuple);
    }
}
