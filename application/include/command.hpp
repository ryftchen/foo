#pragma once

#include <bitset>
#include <mutex>
#include "argument.hpp"
#include "console.hpp"
#include "expression.hpp"
#include "sort.hpp"

#define COMMAND_ALGO_TASK_MAX_METHOD 10
#define COMMAND_PRINT_TITLE_WIDTH 40
#define COMMAND_PRINT_MAX_LINE 50
#define COMMAND_PRINT_ALGO_TASK_TITLE(taskType, title)                                 \
    std::cout << std::endl                                                             \
              << "TASK " << std::setiosflags(std::ios_base::left) << std::setfill('.') \
              << std::setw(COMMAND_PRINT_TITLE_WIDTH) << taskType << title             \
              << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl;
#define COMMAND_CHECK_EXIST_EXCESS_ARG      \
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
        console,
        version,
        help,
        utilTaskBottom
    };

    Command();
    ~Command() = default;
    void runCommander(const int argc, const char* const argv[]);

private:
    mutable std::mutex commandMutex;
    util_argument::Argument program{util_argument::Argument("foo")};

    using AlgoTaskBitSet = std::bitset<COMMAND_ALGO_TASK_MAX_METHOD>;
#pragma pack(8)
    struct TaskPlan
    {
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

        struct UtilTask
        {
            std::bitset<UtilTaskType::utilTaskBottom> utilTaskBit;

            [[nodiscard]] bool empty() const { return !utilTaskBit.any(); }
            void reset() { utilTaskBit.reset(); }
        } utilTask{};

        TaskPlan() = default;
        [[nodiscard]] bool empty() const { return algoTask.empty() && utilTask.empty(); }
        void reset()
        {
            algoTask.reset();
            utilTask.reset();
        };
    } taskPlan{};
#pragma pack()

    const std::string algoTaskNameTable[AlgoTaskType::algoTaskBottom] = {
        "optimum", "integral", "sort"};
    AlgoTaskBitSet* const algoTaskBitPtr[AlgoTaskType::algoTaskBottom] = {
        &taskPlan.algoTask.optimumBit, &taskPlan.algoTask.integralBit, &taskPlan.algoTask.sortBit};
    const std::string
        algoTaskMethodTable[AlgoTaskType::algoTaskBottom][COMMAND_ALGO_TASK_MAX_METHOD] = {
            {"fib", "gra", "ann", "par", "gen"},
            {"tra", "sim", "rom", "gau", "mon"},
            {"bub", "sec", "ins", "she", "mer", "qui", "hea", "cou", "buc", "rad"}};
    typedef void (Command::*PerformAlgoTaskFunctor)() const;
    const PerformAlgoTaskFunctor performAlgoTaskFunctor[AlgoTaskType::algoTaskBottom] = {
        &Command::runOptimum, &Command::runIntegral, &Command::runSort};
    typedef void (Command::*SetAlgoTaskBitFunctor)(const char* const);
    const SetAlgoTaskBitFunctor setAlgoTaskBitFunctor[AlgoTaskType::algoTaskBottom] = {
        &Command::setOptimumBit, &Command::setIntegralBit, &Command::setSortBit};

    const std::string utilTaskNameTable[UtilTaskType::utilTaskBottom] = {
        "console", "version", "help"};
    typedef void (Command::*PerformUtilTaskFunctor)() const;
    const PerformUtilTaskFunctor performUtilTaskFunctor[UtilTaskType::utilTaskBottom] = {
        &Command::printConsoleOutput, &Command::printVersionInfo, &Command::printHelpMessage};

    const std::unordered_multimap<
        algo_expression::ExpressionRange<double, double>, algo_expression::TargetExpression,
        algo_expression::ExpressionMapHash>
        expressionMap{
            {{EXPRESSION_FUN_1_RANGE_1, EXPRESSION_FUN_1_RANGE_2}, algo_expression::Function1()},
            {{EXPRESSION_FUN_2_RANGE_1, EXPRESSION_FUN_2_RANGE_2}, algo_expression::Function2()}};

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
