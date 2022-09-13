#pragma once

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
    enum AlgoTaskType
    {
        optimum,
        integral,
        sort,
        algoTaskBottom
    };
    enum OptimumMethod
    {
        fibonacci,
        gradient,
        annealing,
        particle,
        genetic,
        optimumBottom
    };
    enum IntegralMethod
    {
        trapezoidal,
        simpson,
        romberg,
        gauss,
        monteCarlo,
        integralBottom
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
        radix,
        sortBottom
    };
    enum UtilTaskType
    {
        help,
        version,
        console,
        utilTaskBottom
    };

    Command();
    ~Command() = default;
    void runCommander(const int argc, const char* const argv[]);

private:
    static constexpr uint32_t maxMethodOfAlgoTask = 10;
    static constexpr uint32_t titleWidthForPrintTask = 40;
    static constexpr uint32_t maxLineNumForPrintLog = 50;
    mutable std::mutex commandMutex;
    util_argument::Argument program{util_argument::Argument("foo")};

    using AlgoTaskBitSet = std::bitset<maxMethodOfAlgoTask>;
#pragma pack(8)
    struct TaskPlan
    {
        struct UtilTask
        {
            std::bitset<UtilTaskType::utilTaskBottom> utilTaskBit;

            [[nodiscard]] bool empty() const { return !utilTaskBit.any(); }
            void reset() { utilTaskBit.reset(); }
        } utilTask{};

        struct AlgoTask
        {
            AlgoTaskBitSet optimumBit;
            AlgoTaskBitSet integralBit;
            AlgoTaskBitSet sortBit;

            [[nodiscard]] bool empty() const
            {
                return !optimumBit.any() && !integralBit.any() && !sortBit.any();
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

    static constexpr std::string_view algoTaskNameTable[AlgoTaskType::algoTaskBottom] = {
        "optimum", "integral", "sort"};
    AlgoTaskBitSet* const algoTaskBitPtr[AlgoTaskType::algoTaskBottom] = {
        &taskPlan.algoTask.optimumBit, &taskPlan.algoTask.integralBit, &taskPlan.algoTask.sortBit};
    static constexpr std::string_view
        algoTaskMethodTable[AlgoTaskType::algoTaskBottom][maxMethodOfAlgoTask] = {
            {"fib", "gra", "ann", "par", "gen"},
            {"tra", "sim", "rom", "gau", "mon"},
            {"bub", "sec", "ins", "she", "mer", "qui", "hea", "cou", "buc", "rad"}};
    typedef void (Command::*PerformAlgoTaskFunctor)() const;
    const PerformAlgoTaskFunctor performAlgoTaskFunctor[AlgoTaskType::algoTaskBottom] = {
        &Command::runOptimum, &Command::runIntegral, &Command::runSort};
    typedef void (Command::*SetAlgoTaskBitFunctor)(const char* const);
    const SetAlgoTaskBitFunctor setAlgoTaskBitFunctor[AlgoTaskType::algoTaskBottom] = {
        &Command::setOptimumBit, &Command::setIntegralBit, &Command::setSortBit};

    static constexpr std::string_view utilTaskNameTable[UtilTaskType::utilTaskBottom] = {
        "help", "version", "console"};
    typedef void (Command::*PerformUtilTaskFunctor)() const;
    const PerformUtilTaskFunctor performUtilTaskFunctor[UtilTaskType::utilTaskBottom] = {
        &Command::printHelpMessage, &Command::printVersionInfo, &Command::printConsoleOutput};

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
    void validateAlgorithmTask();
    void validateUtilityTask();
    bool checkTask() const;
    void performTask() const;
    void runOptimum() const;
    void getOptimumResult(
        const algo_expression::Expression& express, const double leftEndpoint,
        const double rightEndpoint, const double epsilon) const;
    void setOptimumBit(const char* const method);
    void runIntegral() const;
    void getIntegralResult(
        const algo_expression::Expression& express, const double lowerLimit,
        const double upperLimit, const double epsilon) const;
    void setIntegralBit(const char* const method);
    void runSort() const;
    template <typename T>
    void getSortResult(const std::shared_ptr<algo_sort::Sort<T>>& sort) const;
    void setSortBit(const char* const method);
    void printConsoleOutput() const;
    void printVersionInfo() const;
    void printHelpMessage() const;
    static std::string getIconBanner();

    void enterConsole() const;
    void registerOnConsole(util_console::Console& console) const;
    static void displayLogContext();
    [[noreturn]] void throwUnexpectedMethodException(const std::string& methodInfo);
    [[noreturn]] void throwExcessArgumentException();

protected:
    friend std::ostream& operator<<(std::ostream& os, const AlgoTaskType& taskType);
};
