#pragma once

#include <bitset>
#include <mutex>
#include "argument.hpp"
#include "console.hpp"
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
#define COMMAND_GET_ICON_BANNER                               \
    (                                                         \
        {                                                     \
            std::string banner;                               \
            banner += R"(")";                                 \
            banner += R"( ______   ______     ______    \n)"; \
            banner += R"(/\  ___\ /\  __ \   /\  __ \   \n)"; \
            banner += R"(\ \  __\ \ \ \/\ \  \ \ \/\ \  \n)"; \
            banner += R"( \ \_\    \ \_____\  \ \_____\ \n)"; \
            banner += R"(  \/_/     \/_____/   \/_____/ \n)"; \
            banner += R"(")";                                 \
        })

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

            [[nodiscard]] bool empty() const { return !utilTaskBit.any(); }
            void reset() { utilTaskBit.reset(); }
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

    const std::string utilTaskNameTable[UtilTaskType::utilTaskBottom] = {
        "console", "version", "help"};
    typedef void (Command::*PerformUtilTaskFunctor)() const;
    const PerformUtilTaskFunctor performUtilTaskFunctor[UtilTaskType::utilTaskBottom] = {
        &Command::printConsoleOutput, &Command::printVersionInfo, &Command::printHelpMessage};

    const std::unordered_multimap<
        alg_expression::ExpressionRange<double, double>, alg_expression::TargetExpression,
        alg_expression::ExpressionMapHash>
        expressionMap{
            {{EXPRESSION_FUN_1_RANGE_1, EXPRESSION_FUN_1_RANGE_2}, alg_expression::Function1()},
            {{EXPRESSION_FUN_2_RANGE_1, EXPRESSION_FUN_2_RANGE_2}, alg_expression::Function2()}};

    void foregroundHandle(const int argc, const char* const argv[]);
    void backgroundHandle() const;
    void precheckAlgorithmTask();
    void precheckUtilityTask();
    bool checkTask() const;
    void performTask() const;
    void runOptimum() const;
    void getOptimumResult(
        const alg_expression::Expression& express, const double leftEndpoint,
        const double rightEndpoint, const double epsilon) const;
    void setOptimumBit(const char* const method);
    void runIntegral() const;
    void getIntegralResult(
        const alg_expression::Expression& express, const double lowerLimit, const double upperLimit,
        const double epsilon) const;
    void setIntegralBit(const char* const method);
    void runSort() const;
    template <typename T>
    void getSortResult(const std::shared_ptr<alg_sort::Sort<T>>& sort) const;
    void setSortBit(const char* const method);
    void printConsoleOutput() const;
    void printVersionInfo() const;
    void printHelpMessage() const;

    void enterConsole() const;
    void registerOnConsole(util_console::Console& console) const;
    static void displayLogContext();
    [[noreturn]] void throwUnexpectedMethodException(const std::string methodInfo);
    [[noreturn]] void throwExcessArgumentException();

protected:
    friend std::ostream& operator<<(std::ostream& os, const AlgTaskType& taskType);
};
