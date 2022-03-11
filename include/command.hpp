#pragma once
#include <atomic>
#include <bitset>
#include <mutex>
#include <thread>
#include "./expression.hpp"
#include "./sort.hpp"

#define COMMAND_THREAD_NAME_LENGTH 6
#define COMMAND_MAX_METHOD 10
#define COMMAND_PRINT_MAX_LINE 50
#define COMMAND_PERPARE_BITSET(xBit, taskX)                                                        \
    do                                                                                             \
    {                                                                                              \
        taskBit.reset();                                                                           \
        if ((argc > i + 1) && (argv[i + 1][0] != '-'))                                             \
        {                                                                                          \
            taskBit.set(taskX);                                                                    \
        }                                                                                          \
        else                                                                                       \
        {                                                                                          \
            xBit.set();                                                                            \
        }                                                                                          \
    } while (0)
#define COMMAND_LICENSE_CMD "cat ./LICENSE"

class Command
{
public:
    enum TaskBit
    {
        taskOptimum,
        taskIntegral,
        taskSort,
        taskButtom
    };
    enum OptimumBit
    {
        optimumFibonacci,
        optimumGradient,
        optimumAnnealing,
        optimumParticle,
        optimumGenetic,
        optimumButtom
    };
    enum IntegralBit
    {
        integralTrapezoidal,
        integralSimpson,
        integralRomberg,
        integralGauss,
        integralMonteCarlo,
        integralButtom
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
        sortButtom
    };
    virtual ~Command(){};
    std::atomic<bool> parseArgv(const int argc, char *const argv[]);
    void doTask();

private:
#pragma pack(8)
    struct TaskPlan
    {
        std::bitset<OptimumBit::optimumButtom> optimumBit;
        std::bitset<IntegralBit::integralButtom> integralBit;
        std::bitset<SortBit::sortButtom> sortBit;
        std::atomic<bool> taskDone = false;

        TaskPlan() = default;
    } run;
#pragma pack()
    mutable std::mutex commandMutex;
    typedef void (Command::*TaskFunctor)() const;
    const TaskFunctor taskFunctor[TaskBit::taskButtom] = {
        &Command::runOptimum,
        &Command::runIntegral,
        &Command::runSort};
    const char taskTable[TaskBit::taskButtom][COMMAND_MAX_METHOD][COMMAND_THREAD_NAME_LENGTH] = {
        {"o_fib", "o_gra", "o_ann", "o_par", "o_gen"},
        {"i_tra", "i_sim", "i_rom", "i_gau", "i_mon"},
        {"s_bub", "s_sec", "s_ins", "s_she", "s_mer", "s_qui", "s_hea", "s_cou", "s_buc", "s_rad"}};
    void setBitFromTaskPlan(char *const argv[], const std::bitset<TaskBit::taskButtom> &taskBit);
    void runOptimum() const;
    void getOptimumResult(
        const Expression &express,
        const double leftEndpoint,
        const double rightEndpoint,
        const double epsilon) const;
    void setOptimumBit(char *const argv[]);
    void runIntegral() const;
    void getIntegralResult(
        const Expression &express,
        const double lowerLimit,
        const double upperLimit,
        const double epsilon) const;
    void setIntegralBit(char *const argv[]);
    void runSort() const;
    void getSortResult(const std::shared_ptr<Sort<int>> &sort) const;
    void setSortBit(char *const argv[]);
    friend void printFile(const char *const pathname, const bool reverse, const uint32_t maxLine);

protected:
    void printLogContext();
    void printInstruction();
    void printLicense();
    void printUnkownParameter(char *const argv[]);
};
