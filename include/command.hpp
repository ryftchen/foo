#pragma once
#include <bitset>
#include <mutex>
#include "expression.hpp"
#include "sort.hpp"

#define COMMAND_PER_TASK_MAX_METHOD 10
#define COMMAND_PRINT_TITLE_WIDTH 40
#define COMMAND_PRINT_MAX_LINE 50
#define COMMAND_PREPARE_TASK_CHECK(isAllowSubParam)                 \
    do                                                              \
    {                                                               \
        if (!validatePluralOptions(argc, argv, i, isAllowSubParam)) \
        {                                                           \
            return;                                                 \
        }                                                           \
    }                                                               \
    while (0)
#define COMMAND_PREPARE_BIT_SET(taskType, taskTypeBit)                          \
    do                                                                          \
    {                                                                           \
        COMMAND_PREPARE_TASK_CHECK(true);                                       \
        taskBitForPreview.set(taskType);                                        \
        if (((i + 1) == argc) || ((argc > (i + 1)) && ('-' == argv[i + 1][0]))) \
        {                                                                       \
            taskTypeBit.set();                                                  \
        }                                                                       \
    }                                                                           \
    while (0)
#define COMMAND_PRINT_TASK_TITLE(taskType, title)                                      \
    std::cout << std::endl                                                             \
              << "TASK " << std::setiosflags(std::ios_base::left) << std::setfill('.') \
              << std::setw(COMMAND_PRINT_TITLE_WIDTH) << taskType << title             \
              << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl;
#define COMMAND_EXECUTE_OUTPUT_NAME "tput bel; banner foo"

class Command
{
public:
    virtual ~Command() = default;
    void parseArgv(const int argc, char* const argv[]);
    void performTask() const;
    bool checkTask() const;
    enum TaskType
    {
        taskOptimum,
        taskIntegral,
        taskSort,
        taskBottom
    };
    enum OptimumMethod
    {
        optimumFibonacci,
        optimumGradient,
        optimumAnnealing,
        optimumParticle,
        optimumGenetic,
        optimumBottom
    };
    enum IntegralMethod
    {
        integralTrapezoidal,
        integralSimpson,
        integralRomberg,
        integralGauss,
        integralMonteCarlo,
        integralBottom
    };
    enum SortMethod
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

private:
    mutable std::mutex commandMutex;
#pragma pack(8)
    struct TaskPlan
    {
        std::bitset<OptimumMethod::optimumBottom> optimumBit;
        std::bitset<IntegralMethod::integralBottom> integralBit;
        std::bitset<SortMethod::sortBottom> sortBit;

        TaskPlan() = default;
        void reset()
        {
            optimumBit.reset();
            integralBit.reset();
            sortBit.reset();
        };
    } taskPlan{};
#pragma pack()
    typedef void (Command::*TaskFunctor)() const;
    const TaskFunctor taskFunctor[TaskType::taskBottom] = {
        &Command::runOptimum, &Command::runIntegral, &Command::runSort};
    const std::string taskTable[TaskType::taskBottom][COMMAND_PER_TASK_MAX_METHOD] = {
        {"o_fib", "o_gra", "o_ann", "o_par", "o_gen"},
        {"i_tra", "i_sim", "i_rom", "i_gau", "i_mon"},
        {"s_bub", "s_sec", "s_ins", "s_she", "s_mer", "s_qui", "s_hea", "s_cou", "s_buc", "s_rad"}};
    const std::unordered_multimap<
        ExpressionRange<double, double>, TargetExpression, ExpressionMapHash>
        expressionMap{
            {{EXPRESSION_FUN_1_RANGE_1, EXPRESSION_FUN_1_RANGE_2}, Function1()},
            {{EXPRESSION_FUN_2_RANGE_1, EXPRESSION_FUN_2_RANGE_2}, Function2()}};
    bool validatePluralOptions(
        const int argc, char* const argv[], const int index, const bool isAllowSubParam);
    void updateTaskPlan(
        char* const argv[], const std::bitset<TaskType::taskBottom>& taskBitForPreview);
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
    template <typename T>
    void getSortResult(const std::shared_ptr<Sort<T>>& sort) const;
    void setSortBit(char* const argv[]);

protected:
    static void printLogContext();
    static void printInstruction();
    void printUnexpectedOption(char* const argv[], const bool isUnknownOption);
    friend std::ostream& operator<<(std::ostream& os, const TaskType& taskType);
};

std::ostream& operator<<(std::ostream& os, const Command::TaskType& taskType);
