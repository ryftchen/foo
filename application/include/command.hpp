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
    template <class T>
    struct Bottom;
    enum BasicTaskType
    {
        help,
        version,
        console
    };
    template <>
    struct Bottom<BasicTaskType>
    {
        static constexpr int value = 3;
    };

    enum AlgoTaskType
    {
        optimum,
        integral,
        sort,
        match
    };
    template <>
    struct Bottom<AlgoTaskType>
    {
        static constexpr int value = 4;
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

    Command();
    ~Command() = default;
    void runCommander(const int argc, const char* const argv[]);

private:
    static constexpr uint32_t titleWidthForPrintTask{40};
    static constexpr uint32_t maxLineNumForPrintLog{50};
    static constexpr int maxMethodOfAlgoTask{std::max<int>(
        {Bottom<OptimumMethod>::value,
         Bottom<IntegralMethod>::value,
         Bottom<SortMethod>::value,
         Bottom<MatchMethod>::value})};
    mutable std::mutex commandMutex;
    util_argument::Argument program{util_argument::Argument("foo")};

#pragma pack(8)
    struct TaskPlan
    {
        TaskPlan() = default;
        struct BasicTask
        {
            std::bitset<Bottom<BasicTaskType>::value> basicTaskBit;

            [[nodiscard]] bool empty() const { return basicTaskBit.none(); }
            void reset() { basicTaskBit.reset(); }
        } basicTask{};

        struct AlgoTask
        {
            std::bitset<Bottom<OptimumMethod>::value> optimumBit;
            std::bitset<Bottom<IntegralMethod>::value> integralBit;
            std::bitset<Bottom<SortMethod>::value> sortBit;
            std::bitset<Bottom<MatchMethod>::value> matchBit;

            [[nodiscard]] bool empty() const
            {
                return optimumBit.none() && integralBit.none() && sortBit.none() && matchBit.none();
            }
            void reset()
            {
                optimumBit.reset();
                integralBit.reset();
                sortBit.reset();
                matchBit.reset();
            }
        } algoTask{};

        [[nodiscard]] bool empty() const { return basicTask.empty() && algoTask.empty(); }
        void reset()
        {
            basicTask.reset();
            algoTask.reset();
        };
    } taskPlan{};
#pragma pack()

    static constexpr std::array<std::string_view, Bottom<BasicTaskType>::value> basicTaskNameTable{
        "help",
        "version",
        "console"};
    typedef void (Command::*PerformBasicTaskFunctor)() const;
    const std::array<PerformBasicTaskFunctor, Bottom<BasicTaskType>::value> performBasicTaskFunctor{
        &Command::printHelpMessage,
        &Command::printVersionInfo,
        &Command::printConsoleOutput};
    static constexpr std::array<std::string_view, Bottom<AlgoTaskType>::value> algoTaskNameTable{
        "optimum",
        "integral",
        "sort",
        "match"};
    static constexpr std::array<std::array<std::string_view, maxMethodOfAlgoTask>, Bottom<AlgoTaskType>::value>
        algoTaskMethodTable{
            {{"fib", "gra", "ann", "par", "gen"},
             {"tra", "sim", "rom", "gau", "mon"},
             {"bub", "sel", "ins", "she", "mer", "qui", "hea", "cou", "buc", "rad"},
             {"rab", "knu", "boy", "hor", "sun"}}};
    typedef void (Command::*PerformAlgoTaskFunctor)() const;
    const std::array<PerformAlgoTaskFunctor, Bottom<AlgoTaskType>::value> performAlgoTaskFunctor{
        &Command::runOptimum,
        &Command::runIntegral,
        &Command::runSort,
        &Command::runMatch};
    typedef void (Command::*SetAlgoTaskBitFunctor)(const std::string&);
    const std::array<SetAlgoTaskBitFunctor, Bottom<AlgoTaskType>::value> setAlgoTaskBitFunctor{
        &Command::setOptimumBit,
        &Command::setIntegralBit,
        &Command::setSortBit,
        &Command::setMatchBit};

    const std::unordered_multimap<
        algo_expression::ExprRange<double, double>,
        algo_expression::ExprTarget,
        algo_expression::ExprMapHash>
        optimumExprMap{
            {{algo_expression::Function1::range1,
              algo_expression::Function1::range2,
              algo_expression::Function1::optimumExpr},
             algo_expression::Function1()},
            {{algo_expression::Function2::range1,
              algo_expression::Function2::range2,
              algo_expression::Function2::optimumExpr},
             algo_expression::Function2()}};
    const std::unordered_multimap<
        algo_expression::ExprRange<double, double>,
        algo_expression::ExprTarget,
        algo_expression::ExprMapHash>
        integralExprMap{
            {{algo_expression::Function1::range1,
              algo_expression::Function1::range2,
              algo_expression::Function1::integralExpr},
             algo_expression::Function1()},
            {{algo_expression::Function2::range1,
              algo_expression::Function2::range2,
              algo_expression::Function2::integralExpr},
             algo_expression::Function2()}};

    void foregroundHandle(const int argc, const char* const argv[]);
    void backgroundHandle() const;
    void validateBasicTask();
    void validateAlgorithmTask();
    bool checkTask() const;
    void performTask() const;
    void runOptimum() const;
    void getOptimumResult(
        const algo_expression::Expression& express,
        const double leftEndpoint,
        const double rightEndpoint,
        const double epsilon) const;
    void setOptimumBit(const std::string& method);
    void runIntegral() const;
    void getIntegralResult(
        const algo_expression::Expression& express,
        const double lowerLimit,
        const double upperLimit,
        const double epsilon) const;
    void setIntegralBit(const std::string& method);
    void runSort() const;
    template <typename T>
    void getSortResult(const std::shared_ptr<algo_sort::Sort<T>>& sort) const;
    void setSortBit(const std::string& method);
    void runMatch() const;
    void getMatchResult(const std::shared_ptr<algo_match::Match>& match) const;
    void setMatchBit(const std::string& method);
    void printConsoleOutput() const;
    void printVersionInfo() const;
    void printHelpMessage() const;
    static std::string getIconBanner();

    void enterConsole() const;
    void registerOnConsole(util_console::Console& console) const;
    static void viewLogContent();
    [[noreturn]] void throwUnexpectedMethodException(const std::string& info);
    [[noreturn]] void throwExcessArgumentException();

protected:
    friend std::ostream& operator<<(std::ostream& os, const AlgoTaskType& taskType);
};
