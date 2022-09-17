#pragma once

#include <algorithm>
#include <bitset>
#include <mutex>
#include "argument.hpp"
#include "console.hpp"
#include "expression.hpp"
#include "sort.hpp"

#define COMMAND_PRINT_ALGO_TASK_TITLE(taskType, title)                                 \
    std::cout << std::endl                                                             \
              << "TASK " << std::setiosflags(std::ios_base::left) << std::setfill('.') \
              << std::setw(titleWidthForPrintTask) << taskType << title                \
              << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl;
#define COMMAND_CHECK_FOR_EXCESS_ARG        \
    do                                      \
    {                                       \
        if (checkTask())                    \
        {                                   \
            throwExcessArgumentException(); \
        }                                   \
    }                                       \
    while (0)

class Command
{
public:
    template <class T>
    struct Bottom;
    enum UtilTaskType
    {
        help,
        version,
        console
    };
    template <>
    struct Bottom<UtilTaskType>
    {
        static constexpr int value = 3;
    };

    enum AlgoTaskType
    {
        optimum,
        integral,
        sort
    };
    template <>
    struct Bottom<AlgoTaskType>
    {
        static constexpr int value = 3;
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

    Command();
    ~Command() = default;
    void runCommander(const int argc, const char* const argv[]);

private:
    static constexpr uint32_t titleWidthForPrintTask = 40;
    static constexpr uint32_t maxLineNumForPrintLog = 50;
    static constexpr int maxMethodOfAlgoTask = std::max<int>(
        {Bottom<OptimumMethod>::value, Bottom<IntegralMethod>::value, Bottom<SortMethod>::value});
    mutable std::mutex commandMutex;
    util_argument::Argument program{util_argument::Argument("foo")};

#pragma pack(8)
    struct TaskPlan
    {
        struct UtilTask
        {
            std::bitset<Bottom<UtilTaskType>::value> utilTaskBit;

            [[nodiscard]] bool empty() const { return utilTaskBit.none(); }
            void reset() { utilTaskBit.reset(); }
        } utilTask{};

        struct AlgoTask
        {
            std::bitset<Bottom<OptimumMethod>::value> optimumBit;
            std::bitset<Bottom<IntegralMethod>::value> integralBit;
            std::bitset<Bottom<SortMethod>::value> sortBit;

            [[nodiscard]] bool empty() const
            {
                return optimumBit.none() && integralBit.none() && sortBit.none();
            }
            void reset()
            {
                optimumBit.reset();
                integralBit.reset();
                sortBit.reset();
            }
        } algoTask{};

        TaskPlan() = default;
        [[nodiscard]] bool empty() const { return utilTask.empty() && algoTask.empty(); }
        void reset()
        {
            utilTask.reset();
            algoTask.reset();
        };
    } taskPlan{};
#pragma pack()

    static constexpr std::array<std::string_view, Bottom<UtilTaskType>::value> utilTaskNameTable = {
        "help", "version", "console"};
    typedef void (Command::*PerformUtilTaskFunctor)() const;
    const std::array<PerformUtilTaskFunctor, Bottom<UtilTaskType>::value> performUtilTaskFunctor = {
        &Command::printHelpMessage, &Command::printVersionInfo, &Command::printConsoleOutput};
    static constexpr std::array<std::string_view, Bottom<AlgoTaskType>::value> algoTaskNameTable = {
        "optimum", "integral", "sort"};
    static constexpr std::array<
        std::array<std::string_view, maxMethodOfAlgoTask>, Bottom<AlgoTaskType>::value>
        algoTaskMethodTable = {
            {{"fib", "gra", "ann", "par", "gen"},
             {"tra", "sim", "rom", "gau", "mon"},
             {"bub", "sel", "ins", "she", "mer", "qui", "hea", "cou", "buc", "rad"}}};
    typedef void (Command::*PerformAlgoTaskFunctor)() const;
    const std::array<PerformAlgoTaskFunctor, Bottom<AlgoTaskType>::value> performAlgoTaskFunctor = {
        &Command::runOptimum, &Command::runIntegral, &Command::runSort};
    typedef void (Command::*SetAlgoTaskBitFunctor)(const std::string&);
    const std::array<SetAlgoTaskBitFunctor, Bottom<AlgoTaskType>::value> setAlgoTaskBitFunctor = {
        &Command::setOptimumBit, &Command::setIntegralBit, &Command::setSortBit};
    const std::unordered_multimap<
        algo_expression::ExpressionRange<double, double>, algo_expression::TargetExpression,
        algo_expression::ExpressionMapHash>
        expressionMap{
            {{algo_expression::Function1::range1, algo_expression::Function1::range2},
             algo_expression::Function1()},
            {{algo_expression::Function2::range1, algo_expression::Function2::range2},
             algo_expression::Function2()}};

    void foregroundHandle(const int argc, const char* const argv[]);
    void backgroundHandle() const;
    void validateUtilityTask();
    void validateAlgorithmTask();
    bool checkTask() const;
    void performTask() const;
    void runOptimum() const;
    void getOptimumResult(
        const algo_expression::Expression& express, const double leftEndpoint,
        const double rightEndpoint, const double epsilon) const;
    void setOptimumBit(const std::string& method);
    void runIntegral() const;
    void getIntegralResult(
        const algo_expression::Expression& express, const double lowerLimit,
        const double upperLimit, const double epsilon) const;
    void setIntegralBit(const std::string& method);
    void runSort() const;
    template <typename T>
    void getSortResult(const std::shared_ptr<algo_sort::Sort<T>>& sort) const;
    void setSortBit(const std::string& method);
    void printConsoleOutput() const;
    void printVersionInfo() const;
    void printHelpMessage() const;
    static std::string getIconBanner();

    void enterConsole() const;
    void registerOnConsole(util_console::Console& console) const;
    static void displayLogContext();
    [[noreturn]] void throwUnexpectedMethodException(const std::string& info);
    [[noreturn]] void throwExcessArgumentException();

protected:
    friend std::ostream& operator<<(std::ostream& os, const AlgoTaskType& taskType);
};
