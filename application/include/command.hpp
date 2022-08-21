#pragma once
#include <bitset>
#include <mutex>
#include "argument.hpp"
#include "expression.hpp"
#include "sort.hpp"

#define COMMAND_ALG_TASK_MAX_METHOD 10
#define COMMAND_PRINT_TITLE_WIDTH 40
#define COMMAND_PRINT_MAX_LINE 50
#define COMMAND_PRINT_ALG_TASK_TITLE(taskType, title)                                  \
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
    enum AlgTaskType
    {
        optimum,
        integral,
        sort,
        algTaskBottom
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
        log,
        version,
        help,
        utilTaskBottom
    };

    Command();
    virtual ~Command() = default;
    void runCommander(const int argc, const char* const argv[]);

private:
    mutable std::mutex commandMutex;
    Argument program{Argument("foo")};

    using AlgTaskBitSet = std::bitset<COMMAND_ALG_TASK_MAX_METHOD>;
#pragma pack(8)
    struct TaskPlan
    {
        struct AlgTask
        {
            AlgTaskBitSet optimumBit;
            AlgTaskBitSet integralBit;
            AlgTaskBitSet sortBit;

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
        } algTask{};

        struct UtilTask
        {
            std::bitset<UtilTaskType::utilTaskBottom> utilTaskBit;
            struct LogConfig
            {
                uint32_t maxLine{COMMAND_PRINT_MAX_LINE};
                bool isReverse{true};
            } logConfig{};

            [[nodiscard]] bool empty() const { return !utilTaskBit.any(); }
            void reset()
            {
                utilTaskBit.reset();
                logConfig = {COMMAND_PRINT_MAX_LINE, true};
            }
        } utilTask{};

        TaskPlan() = default;
        [[nodiscard]] bool empty() const { return algTask.empty() && utilTask.empty(); }
        void reset()
        {
            algTask.reset();
            utilTask.reset();
        };
    } taskPlan{};
#pragma pack()

    const std::string algTaskNameTable[AlgTaskType::algTaskBottom] = {
        "optimum", "integral", "sort"};
    AlgTaskBitSet* const algTaskBitPtr[AlgTaskType::algTaskBottom] = {
        &taskPlan.algTask.optimumBit, &taskPlan.algTask.integralBit, &taskPlan.algTask.sortBit};
    const std::string algTaskMethodTable[AlgTaskType::algTaskBottom][COMMAND_ALG_TASK_MAX_METHOD] =
        {{"fib", "gra", "ann", "par", "gen"},
         {"tra", "sim", "rom", "gau", "mon"},
         {"bub", "sec", "ins", "she", "mer", "qui", "hea", "cou", "buc", "rad"}};
    typedef void (Command::*PerformAlgTaskFunctor)() const;
    const PerformAlgTaskFunctor performAlgTaskFunctor[AlgTaskType::algTaskBottom] = {
        &Command::runOptimum, &Command::runIntegral, &Command::runSort};
    typedef void (Command::*SetAlgTaskBitFunctor)(const char* const);
    const SetAlgTaskBitFunctor setAlgTaskBitFunctor[AlgTaskType::algTaskBottom] = {
        &Command::setOptimumBit, &Command::setIntegralBit, &Command::setSortBit};

    const std::string utilTaskNameTable[UtilTaskType::utilTaskBottom] = {"log", "version", "help"};
    typedef void (Command::*PerformUtilTaskFunctor)() const;
    const PerformUtilTaskFunctor performUtilTaskFunctor[UtilTaskType::utilTaskBottom] = {
        &Command::printLogContext, &Command::printVersionInfo, &Command::printHelpMessage};

    const std::unordered_multimap<
        ExpressionRange<double, double>, TargetExpression, ExpressionMapHash>
        expressionMap{
            {{EXPRESSION_FUN_1_RANGE_1, EXPRESSION_FUN_1_RANGE_2}, Function1()},
            {{EXPRESSION_FUN_2_RANGE_1, EXPRESSION_FUN_2_RANGE_2}, Function2()}};

    void foregroundHandle(const int argc, const char* const argv[]);
    void backgroundHandle() const;
    void precheckAlgorithmTask();
    void precheckUtilityTask();
    bool checkTask() const;
    void performTask() const;
    void runOptimum() const;
    void getOptimumResult(
        const Expression& express, const double leftEndpoint, const double rightEndpoint,
        const double epsilon) const;
    void setOptimumBit(const char* const method);
    void runIntegral() const;
    void getIntegralResult(
        const Expression& express, const double lowerLimit, const double upperLimit,
        const double epsilon) const;
    void setIntegralBit(const char* const method);
    void runSort() const;
    template <typename T>
    void getSortResult(const std::shared_ptr<Sort<T>>& sort) const;
    void setSortBit(const char* const method);
    void printLogContext() const;
    void printVersionInfo() const;
    void printHelpMessage() const;
    [[noreturn]] void throwUnexpectedMethodException(const std::string methodInfo);
    [[noreturn]] void throwExcessArgumentException();

protected:
    friend std::ostream& operator<<(std::ostream& os, const AlgTaskType& taskType);
};

std::ostream& operator<<(std::ostream& os, const Command::AlgTaskType& taskType);
