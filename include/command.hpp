#pragma once
#include <bitset>
#include <mutex>
#include <thread>
#include "./expression.hpp"
#include "./sort.hpp"

// #define BIT_SET(a, b) (a |= b)
// #define BIT_TEST(a, b) (a == (a | b))
// #define BIT_RESET(a, b) (a &= (a ^ b))
#define README_PATH "./README.md"
#define BACKUP_CMD "./script/build.sh --backup"
#define COMMAND_THREAD_NAME_LENGTH 6
#define COMMAND_MAX_METHOD 10
#define COMMAND_PRINT_MAX_LINE 50
#define COMMAND_PRINT_PRECISION 5
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
    bool parseArgv(const int argc, char *const argv[]);
    void doTask();

private:
    std::mutex commandMutex;
    typedef void (Command::*TaskFunctor)();
    const TaskFunctor taskFunctor[TaskBit::taskButtom] = {
        &Command::runOptimum,
        &Command::runIntegral,
        &Command::runSort};
    const char taskTable[TaskBit::taskButtom][COMMAND_MAX_METHOD][COMMAND_THREAD_NAME_LENGTH] = {
        {"o_fib", "o_gra", "o_ann", "o_par", "o_gen"},
        {"i_tra", "i_sim", "i_rom", "i_gau", "i_mon"},
        {"s_bub", "s_sec", "s_ins", "s_she", "s_mer", "s_qui", "s_hea", "s_cou", "s_buc", "s_rad"}};
    // #pragma pack(8)
    struct TaskPlan
    {
        std::bitset<OptimumBit::optimumButtom> optimumBit;
        std::bitset<IntegralBit::integralButtom> integralBit;
        std::bitset<SortBit::sortButtom> sortBit;
        bool taskDone = false;

        TaskPlan() = default;
    } run;
    // #pragma pack()
    void setBitFromTaskPlan(char *const argv[], const std::bitset<TaskBit::taskButtom> &taskBit);
    void runOptimum();
    void getOptimumResult(
        const Expression &express,
        const double leftEndpoint,
        const double rightEndpoint,
        const double epsilon) const;
    void setOptimumBit(char *const argv[]);
    void runIntegral();
    void getIntegralResult(
        const Expression &express,
        const double lowerLimit,
        const double upperLimit,
        const double epsilon) const;
    void setIntegralBit(char *const argv[]);
    void runSort();
    void getSortResult(const std::shared_ptr<Sort<int>> sort) const;
    void setSortBit(char *const argv[]);
    friend void printFile(const char *const pathname, const bool reverse, const uint32_t maxLine);

protected:
    void backupProject();
    void printLogContext();
    void printInstruction();
    void printUnkownParameter(char *const argv[]);
};
