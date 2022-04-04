#pragma once
#include <atomic>
#include <bitset>
#include <mutex>
#include "expression.hpp"
#include "log.hpp"
#include "sort.hpp"

void executeCommand(const char* const command);

#define COMMAND_MAX_METHOD 10
#define COMMAND_PRINT_MAX_LINE 50
#define COMMAND_PREPARE_TASK_CHECK                  \
    do                                              \
    {                                               \
        if (checkTask())                            \
        {                                           \
            printUnexpectedOption(argv + i, false); \
            return;                                 \
        }                                           \
    }                                               \
    while (0)
#define COMMAND_PREPARE_BIT_SET(xBit, taskX)            \
    do                                                  \
    {                                                   \
        COMMAND_PREPARE_TASK_CHECK;                     \
        taskBit.set(taskX);                             \
        if ((argc <= i + 1) || (argv[i + 1][0] == '-')) \
        {                                               \
            xBit.set();                                 \
        }                                               \
    }                                                   \
    while (0)
#define COMMAND_EXECUTE_OUTPUT_NAME "tput bel; banner foo"

class Command
{
public:
    enum TaskBit
    {
        taskOptimum,
        taskIntegral,
        taskSort,
        taskBottom
    };
    enum OptimumBit
    {
        optimumFibonacci,
        optimumGradient,
        optimumAnnealing,
        optimumParticle,
        optimumGenetic,
        optimumBottom
    };
    enum IntegralBit
    {
        integralTrapezoidal,
        integralSimpson,
        integralRomberg,
        integralGauss,
        integralMonteCarlo,
        integralBottom
    };
    enum SortBit
    {
        sortBubble,
        sortSelection,
        sortInsertion,
        sortShell,
        sortMerge,
        sortQuick,
        sortHeap,
        sortCounting,
        sortBucket,
        sortRadix,
        sortBottom
    };
    Command(int argc, char* argv[]);
    virtual ~Command(){};
    void parseArgv(const int argc, char* const argv[]);
    bool checkTask();
    void doTask();

private:
    mutable std::mutex commandMutex;
#pragma pack(8)
    struct TaskPlan
    {
        std::bitset<OptimumBit::optimumBottom> optimumBit;
        std::bitset<IntegralBit::integralBottom> integralBit;
        std::bitset<SortBit::sortBottom> sortBit;

        TaskPlan() = default;
    } taskPlan;
#pragma pack()
    typedef void (Command::*TaskFunctor)() const;
    const TaskFunctor taskFunctor[TaskBit::taskBottom] = {
        &Command::runOptimum, &Command::runIntegral, &Command::runSort};
    const std::string taskTable[TaskBit::taskBottom][COMMAND_MAX_METHOD] = {
        {"o_fib", "o_gra", "o_ann", "o_par", "o_gen"},
        {"i_tra", "i_sim", "i_rom", "i_gau", "i_mon"},
        {"s_bub", "s_sec", "s_ins", "s_she", "s_mer", "s_qui", "s_hea", "s_cou", "s_buc", "s_rad"}};
    const std::unordered_multimap<
        ExpressionRange<double, double>, TargetExpression, ExpressionMapHash>
        expressionMap{
            {{EXPRESS_FUN_1_RANGE_1, EXPRESS_FUN_1_RANGE_2}, Function1()},
            {{EXPRESS_FUN_2_RANGE_1, EXPRESS_FUN_2_RANGE_2}, Function2()}};
    void setTaskPlanFromTaskBit(
        char* const argv[], const std::bitset<TaskBit::taskBottom>& taskBit);
    void runOptimum() const;
    void getOptimumResult(
        const Expression& express, const double leftEndpoint, const double rightEndpoint,
        const double epsilon) const;
    void setOptimumBit(char* const argv[]);
    void runIntegral() const;
    void getIntegralResult(
        const Expression& express, const double lowerLimit, const double upperLimit,
        const double epsilon) const;
    void setIntegralBit(char* const argv[]);
    void runSort() const;
    void getSortResult(const std::shared_ptr<Sort<int>>& sort) const;
    void setSortBit(char* const argv[]);

protected:
    static void printLogContext();
    static void printInstruction();
    void printUnexpectedOption(char* const argv[], const bool isUnknown);
};
