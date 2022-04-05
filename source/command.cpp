#include "command.hpp"
#include <ext/stdio_filebuf.h>
#include <sys/file.h>
#include "exception.hpp"
#include "hash.hpp"
#include "integral.hpp"
#include "optimum.hpp"
#include "thread.hpp"

Command::Command(int argc, char* argv[])
{
    parseArgv(argc - 1, argv + 1);
}

void Command::parseArgv(const int argc, char* const argv[])
{
    if (argc < 1)
    {
        executeCommand(COMMAND_EXECUTE_OUTPUT_NAME);
        LOGGER_INF("No command line option.");
        return;
    }

    std::bitset<TaskBit::taskBottom> taskBit(0);
    for (int i = 0; i < argc; ++i)
    {
        if (argv[i][0] == '-')
        {
            switch (bkdrHash(argv[i]))
            {
                case "-o"_bkdrHash:
                case "--optimum"_bkdrHash:
                    COMMAND_PREPARE_BIT_SET(taskPlan.optimumBit, TaskBit::taskOptimum);
                    break;
                case "-i"_bkdrHash:
                case "--integral"_bkdrHash:
                    COMMAND_PREPARE_BIT_SET(taskPlan.integralBit, TaskBit::taskIntegral);
                    break;
                case "-s"_bkdrHash:
                case "--sort"_bkdrHash:
                    COMMAND_PREPARE_BIT_SET(taskPlan.sortBit, TaskBit::taskSort);
                    break;
                case "--log"_bkdrHash:
                    COMMAND_PREPARE_TASK_CHECK;
                    printLogContext();
                    break;
                case "--help"_bkdrHash:
                    COMMAND_PREPARE_TASK_CHECK;
                    printInstruction();
                    break;
                default:
                    printUnexpectedOption(argv + i, true);
                    break;
            }
            if (taskBit.none())
            {
                return;
            }
        }
        else
        {
            setTaskPlanFromTaskBit(argv + i, taskBit);
            if (!checkTask())
            {
                return;
            }
        }
    }

    return;
}

bool Command::checkTask() const
{
    return taskPlan.optimumBit.any() || taskPlan.integralBit.any() || taskPlan.sortBit.any();
}

void Command::doTask()
{
    for (int i = 0; i < TaskBit::taskBottom; ++i)
    {
        (this->*taskFunctor[TaskBit(i)])();
    }
}

void Command::setTaskPlanFromTaskBit(
    char* const argv[], const std::bitset<TaskBit::taskBottom>& taskBit)
{
    if (taskBit.test(TaskBit::taskOptimum))
    {
        setOptimumBit(argv);
    }
    else if (taskBit.test(TaskBit::taskIntegral))
    {
        setIntegralBit(argv);
    }
    else if (taskBit.test(TaskBit::taskSort))
    {
        setSortBit(argv);
    }
    else
    {
        printUnexpectedOption(argv, true);
    }
}

// Optimum
void Command::runOptimum() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    if (taskPlan.optimumBit.any())
    {
        const auto printFunctor = [](const TargetExpression& expression)
        {
            std::visit(
                ExpressionOverloaded{
                    [](const Function1& /*unused*/)
                    {
                        std::cout << EXPRESS_FUN_1_OPTIMUM << std::endl;
                    },
                    [](const Function2& /*unused*/)
                    {
                        std::cout << EXPRESS_FUN_2_OPTIMUM << std::endl;
                    },
                },
                expression);
        };
        const auto resultFunctor =
            [this](const Expression& expression, const ExpressionRange<double, double>& range)
        {
            getOptimumResult(expression, range.range1, range.range2, OPTIMUM_EPSILON);
        };

        std::cout << OPTIMUM_RUN_BEGIN << std::endl;
        for (const auto& [range, expression] : expressionMap)
        {
            printFunctor(expression);
            switch (expression.index())
            {
                case 0:
                    resultFunctor(std::get<0>(expression), range);
                    break;
                case 1:
                    resultFunctor(std::get<1>(expression), range);
                    break;
                default:
                    break;
            }
        }
        std::cout << OPTIMUM_RUN_END << std::endl;
    }
}
void Command::getOptimumResult(
    const Expression& express, const double leftEndpoint, const double rightEndpoint,
    const double epsilon) const
{
    assert((leftEndpoint > rightEndpoint) && (epsilon > 0.0));
    Thread threadPool(taskPlan.optimumBit.count());
    const auto optimumFunctor =
        [&](const std::string& threadName, const std::shared_ptr<Optimum>& classPtr)
    {
        threadPool.enqueue(
            threadName, &Optimum::operator(), classPtr, leftEndpoint, rightEndpoint, epsilon);
    };

    for (int i = 0; i < OptimumBit::optimumBottom; ++i)
    {
        if (taskPlan.optimumBit.test(OptimumBit(i)))
        {
            const std::string threadName = taskTable[TaskBit::taskOptimum][OptimumBit(i)];
            switch (bkdrHash(threadName.c_str()))
            {
                case "o_fib"_bkdrHash:
                    optimumFunctor(threadName, std::make_shared<Fibonacci>(express));
                    break;
                case "o_gra"_bkdrHash:
                    optimumFunctor(threadName, std::make_shared<Gradient>(express));
                    break;
                case "o_ann"_bkdrHash:
                    optimumFunctor(threadName, std::make_shared<Annealing>(express));
                    break;
                case "o_par"_bkdrHash:
                    optimumFunctor(threadName, std::make_shared<Particle>(express));
                    break;
                case "o_gen"_bkdrHash:;
                    optimumFunctor(threadName, std::make_shared<Genetic>(express));
                    break;
                default:
                    break;
            }
        }
    }
}
void Command::setOptimumBit(char* const argv[])
{
    switch (bkdrHash(argv[0]))
    {
        case "fib"_bkdrHash:
            taskPlan.optimumBit.set(OptimumBit::optimumFibonacci);
            break;
        case "gra"_bkdrHash:
            taskPlan.optimumBit.set(OptimumBit::optimumGradient);
            break;
        case "ann"_bkdrHash:
            taskPlan.optimumBit.set(OptimumBit::optimumAnnealing);
            break;
        case "par"_bkdrHash:
            taskPlan.optimumBit.set(OptimumBit::optimumParticle);
            break;
        case "gen"_bkdrHash:
            taskPlan.optimumBit.set(OptimumBit::optimumGenetic);
            break;
        default:
            printUnexpectedOption(argv, true);
            break;
    }
}

// Integral
void Command::runIntegral() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    if (taskPlan.integralBit.any())
    {
        const auto printFunctor = [](const TargetExpression& expression)
        {
            std::visit(
                ExpressionOverloaded{
                    [](const Function1& /*unused*/)
                    {
                        std::cout << EXPRESS_FUN_1_INTEGRAL << std::endl;
                    },
                    [](const Function2& /*unused*/)
                    {
                        std::cout << EXPRESS_FUN_2_INTEGRAL << std::endl;
                    },
                },
                expression);
        };
        const auto resultFunctor =
            [this](const Expression& expression, const ExpressionRange<double, double>& range)
        {
            getIntegralResult(expression, range.range1, range.range2, INTEGRAL_EPSILON);
        };

        std::cout << INTEGRAL_RUN_BEGIN << std::endl;
        for (const auto& [range, expression] : expressionMap)
        {
            printFunctor(expression);
            switch (expression.index())
            {
                case 0:
                    resultFunctor(std::get<0>(expression), range);
                    break;
                case 1:
                    resultFunctor(std::get<1>(expression), range);
                    break;
                default:
                    break;
            }
        }
        std::cout << INTEGRAL_RUN_END << std::endl;
    }
}
void Command::getIntegralResult(
    const Expression& express, const double lowerLimit, const double upperLimit,
    const double epsilon) const
{
    assert(epsilon > 0.0);
    Thread threadPool(taskPlan.integralBit.count());
    const auto integralFunctor =
        [&](const std::string& threadName, const std::shared_ptr<Integral>& classPtr)
    {
        threadPool.enqueue(
            threadName, &Integral::operator(), classPtr, lowerLimit, upperLimit, epsilon);
    };

    for (int i = 0; i < IntegralBit::integralBottom; ++i)
    {
        if (taskPlan.integralBit.test(IntegralBit(i)))
        {
            const std::string threadName = taskTable[TaskBit::taskIntegral][IntegralBit(i)];
            switch (bkdrHash(threadName.c_str()))
            {
                case "i_tra"_bkdrHash:
                    integralFunctor(threadName, std::make_shared<Trapezoidal>(express));
                    break;
                case "i_sim"_bkdrHash:
                    integralFunctor(threadName, std::make_shared<Simpson>(express));
                    break;
                case "i_rom"_bkdrHash:
                    integralFunctor(threadName, std::make_shared<Romberg>(express));
                    break;
                case "i_gau"_bkdrHash:
                    integralFunctor(threadName, std::make_shared<Gauss>(express));
                    break;
                case "i_mon"_bkdrHash:
                    integralFunctor(threadName, std::make_shared<MonteCarlo>(express));
                    break;
                default:
                    break;
            }
        }
    }
}
void Command::setIntegralBit(char* const argv[])
{
    switch (bkdrHash(argv[0]))
    {
        case "tra"_bkdrHash:
            taskPlan.integralBit.set(IntegralBit::integralTrapezoidal);
            break;
        case "sim"_bkdrHash:
            taskPlan.integralBit.set(IntegralBit::integralSimpson);
            break;
        case "rom"_bkdrHash:
            taskPlan.integralBit.set(IntegralBit::integralRomberg);
            break;
        case "gau"_bkdrHash:
            taskPlan.integralBit.set(IntegralBit::integralGauss);
            break;
        case "mon"_bkdrHash:
            taskPlan.integralBit.set(IntegralBit::integralMonteCarlo);
            break;
        default:
            printUnexpectedOption(argv, true);
            break;
    }
}

// Sort
void Command::runSort() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    if (taskPlan.sortBit.any())
    {
        constexpr int leftEndpoint = SORT_ARRAY_RANGE_1;
        constexpr int rightEndpoint = SORT_ARRAY_RANGE_2;
        constexpr uint32_t length = SORT_ARRAY_LENGTH;
        static_assert((leftEndpoint < rightEndpoint) && (length > 0));

        const std::shared_ptr<Sort<int>> sort =
            std::make_shared<Sort<int>>(length, leftEndpoint, rightEndpoint);
        getSortResult(sort);
    }
}
template <typename T>
void Command::getSortResult(const std::shared_ptr<Sort<T>>& sort) const
{
    Thread threadPool(taskPlan.sortBit.count());
    const auto sortFunctor = [&](const std::string& threadName,
                                 void (Sort<T>::*methodPtr)(T* const, const uint32_t) const)
    {
        threadPool.enqueue(
            threadName, methodPtr, sort, sort->getRandomArray().get(), sort->getLength());
    };

    for (int i = 0; i < SortBit::sortBottom; ++i)
    {
        if (taskPlan.sortBit.test(SortBit(i)))
        {
            const std::string threadName = taskTable[TaskBit::taskSort][SortBit(i)];
            switch (bkdrHash(threadName.c_str()))
            {
                case "s_bub"_bkdrHash:
                    sortFunctor(threadName, &Sort<T>::bubbleSort);
                    break;
                case "s_sec"_bkdrHash:
                    sortFunctor(threadName, &Sort<T>::selectionSort);
                    break;
                case "s_ins"_bkdrHash:
                    sortFunctor(threadName, &Sort<T>::insertionSort);
                    break;
                case "s_she"_bkdrHash:
                    sortFunctor(threadName, &Sort<T>::shellSort);
                    break;
                case "s_mer"_bkdrHash:
                    sortFunctor(threadName, &Sort<T>::mergeSort);
                    break;
                case "s_qui"_bkdrHash:
                    sortFunctor(threadName, &Sort<T>::quickSort);
                    break;
                case "s_hea"_bkdrHash:
                    sortFunctor(threadName, &Sort<T>::heapSort);
                    break;
                case "s_cou"_bkdrHash:
                    sortFunctor(threadName, &Sort<T>::countingSort);
                    break;
                case "s_buc"_bkdrHash:
                    sortFunctor(threadName, &Sort<T>::bucketSort);
                    break;
                case "s_rad"_bkdrHash:
                    sortFunctor(threadName, &Sort<T>::radixSort);
                    break;
                default:
                    break;
            }
        }
    }
}
void Command::setSortBit(char* const argv[])
{
    switch (bkdrHash(argv[0]))
    {
        case "bub"_bkdrHash:
            taskPlan.sortBit.set(SortBit::sortBubble);
            break;
        case "sel"_bkdrHash:
            taskPlan.sortBit.set(SortBit::sortSelection);
            break;
        case "ins"_bkdrHash:
            taskPlan.sortBit.set(SortBit::sortInsertion);
            break;
        case "she"_bkdrHash:
            taskPlan.sortBit.set(SortBit::sortShell);
            break;
        case "mer"_bkdrHash:
            taskPlan.sortBit.set(SortBit::sortMerge);
            break;
        case "qui"_bkdrHash:
            taskPlan.sortBit.set(SortBit::sortQuick);
            break;
        case "hea"_bkdrHash:
            taskPlan.sortBit.set(SortBit::sortHeap);
            break;
        case "cou"_bkdrHash:
            taskPlan.sortBit.set(SortBit::sortCounting);
            break;
        case "buc"_bkdrHash:
            taskPlan.sortBit.set(SortBit::sortBucket);
            break;
        case "rad"_bkdrHash:
            taskPlan.sortBit.set(SortBit::sortRadix);
            break;
        default:
            printUnexpectedOption(argv, true);
            break;
    }
}

void Command::printLogContext()
{
    try
    {
        const int fd =
            static_cast<__gnu_cxx::stdio_filebuf<char>* const>(logger.getOfs().rdbuf())->fd();
        if (flock(fd, LOCK_UN))
        {
            throw UnlockWriterLockError(basename(LOG_PATH));
        }

        printFile(LOG_PATH, true, COMMAND_PRINT_MAX_LINE, &changeLogLevelStyle);
    }
    catch (UnlockWriterLockError const& error)
    {
        LOGGER_ERR(error.what());
    }
}

void Command::printInstruction()
{
    puts("Usage    : foo [Options...]\n\n"
         "[Options]:\n\n"
         "    -o, --optimum                      Optimum\n"
         "    [ fib | gra | ann | par | gen ]    Fibonacci|Gradient|Annealing|Particle|Genetic\n\n"
         "    -i, --integral                     Integral\n"
         "    [ tra | sim | rom | gau | mon ]    Trapezoidal|Simpson|Romberg|Gauss|MonteCarlo\n\n"
         "    -s, --sort                         Sort\n"
         "    [ bub | sel | ins | she | mer ]    Bubble|Selection|Insertion|Shell|Merge\n"
         "    [ qui | hea | cou | buc | rad ]    Quick|Heap|Counting|Bucket|Radix\n\n"
         "    --log                              Log\n\n"
         "    --help                             Help");
}

void Command::printUnexpectedOption(char* const argv[], const bool isUnknown)
{
    std::string str = "";
    if (isUnknown)
    {
        str = "Unknown command line option: " + std::string(argv[0])
            + ". Try with --help to get information.";
    }
    else
    {
        str = "Excess command line option: " + std::string(argv[0]) + ".";
    }
    LOGGER_WRN(str.c_str());

    taskPlan = TaskPlan();
}

void executeCommand(const char* const command)
{
    try
    {
        FILE* file = popen(command, "r");
        if (nullptr == file)
        {
            throw CallFunctionError("popen()");
        }

        char resultBuffer[BUFFER_SIZE_MAX] = {'\0'};
        while (nullptr != fgets(resultBuffer, sizeof(resultBuffer), file))
        {
            if ('\n' == resultBuffer[strlen(resultBuffer) - 1])
            {
                resultBuffer[strlen(resultBuffer) - 1] = '\0';
            }
            std::cout << resultBuffer << std::endl;
        }

        const int status = pclose(file);
        if (-1 == status)
        {
            throw CallFunctionError("pclose()");
        }
        else if (WIFEXITED(status))
        {
            if (0 != WEXITSTATUS(status))
            {
                throw ExecuteCommandError(command);
            }
        }
    }
    catch (CallFunctionError const& error)
    {
        LOGGER_ERR(error.what());
    }
    catch (ExecuteCommandError const& error)
    {
        LOGGER_ERR(error.what());
    }
}