#include "command.hpp"
#include <ext/stdio_filebuf.h>
#include <sys/file.h>
#include "exception.hpp"
#include "hash.hpp"
#include "integral.hpp"
#include "optimum.hpp"
#include "thread.hpp"

bool Command::parseArgv(const int argc, char* const argv[])
{
    if (argc < 1)
    {
        executeCommand(COMMAND_EXECUTE_OUTPUT_NAME);
        LOGGER_INF("No command line option.");
        return false;
    }

    std::bitset<TaskBit::taskButtom> taskBit(0);
    for (int i = 0; i < argc; ++i)
    {
        if (argv[i][0] == '-')
        {
            switch (bkdrHash(argv[i]))
            {
                case "-o"_bkdrHash:
                case "--optimum"_bkdrHash:
                    COMMAND_PERPARE_BITSET(task.optimumBit, TaskBit::taskOptimum);
                    break;
                case "-i"_bkdrHash:
                case "--integral"_bkdrHash:
                    COMMAND_PERPARE_BITSET(task.integralBit, TaskBit::taskIntegral);
                    break;
                case "-s"_bkdrHash:
                case "--sort"_bkdrHash:
                    COMMAND_PERPARE_BITSET(task.sortBit, TaskBit::taskSort);
                    break;
                case "--log"_bkdrHash:
                    printLogContext();
                    break;
                case "--help"_bkdrHash:
                    printInstruction();
                    break;
                default:
                    printUnkownParameter(argv + i);
                    break;
            }
        }
        else
        {
            setBitFromTaskPlan(argv + i, taskBit);
        }

        if (true == task.taskDone)
        {
            return false;
        }
    }

    return true;
}

void Command::doTask()
{
    for (int i = 0; i < TaskBit::taskButtom; ++i)
    {
        (this->*taskFunctor[TaskBit(i)])();
    }
}

void Command::setBitFromTaskPlan(
    char* const argv[], const std::bitset<TaskBit::taskButtom>& taskBit)
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
        printUnkownParameter(argv);
    }
}

// Optimum
void Command::runOptimum() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    if (task.optimumBit.any())
    {
        std::cout << OPTIMUM_RUN_BEGIN << std::endl;

        std::cout << EXPRESS_FUN_1_OPTIMUM << std::endl;
        const std::shared_ptr<Expression> fun1 = std::make_shared<Function1>();
        getOptimumResult(*fun1, EXPRESS_FUN_1_RANGE_1, EXPRESS_FUN_1_RANGE_2, OPTIMUM_EPSILON);

        std::cout << EXPRESS_FUN_2_OPTIMUM << std::endl;
        const std::shared_ptr<Expression> fun2 = std::make_shared<Function2>();
        getOptimumResult(*fun2, EXPRESS_FUN_2_RANGE_1, EXPRESS_FUN_2_RANGE_2, OPTIMUM_EPSILON);

        std::cout << OPTIMUM_RUN_END << std::endl;
    }
}
void Command::getOptimumResult(
    const Expression& express, const double leftEndpoint, const double rightEndpoint,
    const double epsilon) const
{
    assert((leftEndpoint > rightEndpoint) && (epsilon > 0.0));
    Thread threadPool(task.optimumBit.count());
    const auto optimumFunctor =
        [&](const std::string& threadName, const std::shared_ptr<Optimum>& classPtr)
    {
        threadPool.enqueue(
            threadName, &Optimum::operator(), classPtr, leftEndpoint, rightEndpoint, epsilon);
    };

    for (int i = 0; i < OptimumBit::optimumButtom; ++i)
    {
        if (task.optimumBit.test(OptimumBit(i)))
        {
            const std::string threadName = taskTable[TaskBit::taskOptimum][OptimumBit(i)];
            switch (bkdrHash(threadName.c_str()))
            {
                case "o_fib"_bkdrHash:
                {
                    std::shared_ptr<Optimum> fib = std::make_shared<Fibonacci>(express);
                    optimumFunctor(threadName, fib);
                    break;
                }
                case "o_gra"_bkdrHash:
                {
                    std::shared_ptr<Optimum> gra = std::make_shared<Gradient>(express);
                    optimumFunctor(threadName, gra);
                    break;
                }
                case "o_ann"_bkdrHash:
                {
                    std::shared_ptr<Optimum> ann = std::make_shared<Annealing>(express);
                    optimumFunctor(threadName, ann);
                    break;
                }
                case "o_par"_bkdrHash:
                {
                    std::shared_ptr<Optimum> par = std::make_shared<Particle>(express);
                    optimumFunctor(threadName, par);
                    break;
                }
                case "o_gen"_bkdrHash:
                {
                    std::shared_ptr<Optimum> gen = std::make_shared<Genetic>(express);
                    optimumFunctor(threadName, gen);
                    break;
                }
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
            task.optimumBit.set(OptimumBit::optimumFibonacci);
            break;
        case "gra"_bkdrHash:
            task.optimumBit.set(OptimumBit::optimumGradient);
            break;
        case "ann"_bkdrHash:
            task.optimumBit.set(OptimumBit::optimumAnnealing);
            break;
        case "par"_bkdrHash:
            task.optimumBit.set(OptimumBit::optimumParticle);
            break;
        case "gen"_bkdrHash:
            task.optimumBit.set(OptimumBit::optimumGenetic);
            break;
        default:
            printUnkownParameter(argv);
            break;
    }
}

// Integral
void Command::runIntegral() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    if (task.integralBit.any())
    {
        std::cout << INTEGRAL_RUN_BEGIN << std::endl;

        std::cout << EXPRESS_FUN_1_INTEGRAL << std::endl;
        const std::shared_ptr<Expression> fun1 = std::make_shared<Function1>();
        getIntegralResult(*fun1, EXPRESS_FUN_1_RANGE_1, EXPRESS_FUN_1_RANGE_2, INTEGRAL_EPSILON);

        std::cout << EXPRESS_FUN_2_INTEGRAL << std::endl;
        const std::shared_ptr<Expression> fun2 = std::make_shared<Function2>();
        getIntegralResult(*fun2, EXPRESS_FUN_2_RANGE_1, EXPRESS_FUN_2_RANGE_2, INTEGRAL_EPSILON);

        std::cout << INTEGRAL_RUN_END << std::endl;
    }
}
void Command::getIntegralResult(
    const Expression& express, const double lowerLimit, const double upperLimit,
    const double epsilon) const
{
    assert(epsilon > 0.0);
    Thread threadPool(task.integralBit.count());
    const auto integralFunctor =
        [&](const std::string& threadName, const std::shared_ptr<Integral>& classPtr)
    {
        threadPool.enqueue(
            threadName, &Integral::operator(), classPtr, lowerLimit, upperLimit, epsilon);
    };

    for (int i = 0; i < IntegralBit::integralButtom; ++i)
    {
        if (task.integralBit.test(IntegralBit(i)))
        {
            const std::string threadName = taskTable[TaskBit::taskIntegral][IntegralBit(i)];
            switch (bkdrHash(threadName.c_str()))
            {
                case "i_tra"_bkdrHash:
                {
                    std::shared_ptr<Integral> tra = std::make_shared<Trapezoidal>(express);
                    integralFunctor(threadName, tra);
                    break;
                }
                case "i_sim"_bkdrHash:
                {
                    std::shared_ptr<Integral> sim = std::make_shared<Simpson>(express);
                    integralFunctor(threadName, sim);
                    break;
                }
                case "i_rom"_bkdrHash:
                {
                    std::shared_ptr<Integral> rom = std::make_shared<Romberg>(express);
                    integralFunctor(threadName, rom);
                    break;
                }
                case "i_gau"_bkdrHash:
                {
                    std::shared_ptr<Integral> gau = std::make_shared<Gauss>(express);
                    integralFunctor(threadName, gau);
                    break;
                }
                case "i_mon"_bkdrHash:
                {
                    std::shared_ptr<Integral> mon = std::make_shared<MonteCarlo>(express);
                    integralFunctor(threadName, mon);
                    break;
                }
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
            task.integralBit.set(IntegralBit::integralTrapezoidal);
            break;
        case "sim"_bkdrHash:
            task.integralBit.set(IntegralBit::integralSimpson);
            break;
        case "rom"_bkdrHash:
            task.integralBit.set(IntegralBit::integralRomberg);
            break;
        case "gau"_bkdrHash:
            task.integralBit.set(IntegralBit::integralGauss);
            break;
        case "mon"_bkdrHash:
            task.integralBit.set(IntegralBit::integralMonteCarlo);
            break;
        default:
            printUnkownParameter(argv);
            break;
    }
}

// Sort
void Command::runSort() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    if (task.sortBit.any())
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
void Command::getSortResult(const std::shared_ptr<Sort<int>>& sort) const
{
    Thread threadPool(task.sortBit.count());
    const auto sortFunctor = [&](const std::string& threadName,
                                 void (Sort<int>::*methodPtr)(int* const, const uint32_t) const)
    {
        threadPool.enqueue(
            threadName, methodPtr, sort, sort->getRandomArray().get(), sort->getLength());
    };

    for (int i = 0; i < SortBit::sortButtom; ++i)
    {
        if (task.sortBit.test(SortBit(i)))
        {
            const std::string threadName = taskTable[TaskBit::taskSort][SortBit(i)];
            switch (bkdrHash(threadName.c_str()))
            {
                case "s_bub"_bkdrHash:
                    sortFunctor(threadName, &Sort<int>::bubbleSort);
                    break;
                case "s_sec"_bkdrHash:
                    sortFunctor(threadName, &Sort<int>::selectionSort);
                    break;
                case "s_ins"_bkdrHash:
                    sortFunctor(threadName, &Sort<int>::insertionSort);
                    break;
                case "s_she"_bkdrHash:
                    sortFunctor(threadName, &Sort<int>::shellSort);
                    break;
                case "s_mer"_bkdrHash:
                    sortFunctor(threadName, &Sort<int>::mergeSort);
                    break;
                case "s_qui"_bkdrHash:
                    sortFunctor(threadName, &Sort<int>::quickSort);
                    break;
                case "s_hea"_bkdrHash:
                    sortFunctor(threadName, &Sort<int>::heapSort);
                    break;
                case "s_cou"_bkdrHash:
                    sortFunctor(threadName, &Sort<int>::countingSort);
                    break;
                case "s_buc"_bkdrHash:
                    sortFunctor(threadName, &Sort<int>::bucketSort);
                    break;
                case "s_rad"_bkdrHash:
                    sortFunctor(threadName, &Sort<int>::radixSort);
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
            task.sortBit.set(SortBit::sortBubble);
            break;
        case "sel"_bkdrHash:
            task.sortBit.set(SortBit::sortSelection);
            break;
        case "ins"_bkdrHash:
            task.sortBit.set(SortBit::sortInsertion);
            break;
        case "she"_bkdrHash:
            task.sortBit.set(SortBit::sortShell);
            break;
        case "mer"_bkdrHash:
            task.sortBit.set(SortBit::sortMerge);
            break;
        case "qui"_bkdrHash:
            task.sortBit.set(SortBit::sortQuick);
            break;
        case "hea"_bkdrHash:
            task.sortBit.set(SortBit::sortHeap);
            break;
        case "cou"_bkdrHash:
            task.sortBit.set(SortBit::sortCounting);
            break;
        case "buc"_bkdrHash:
            task.sortBit.set(SortBit::sortBucket);
            break;
        case "rad"_bkdrHash:
            task.sortBit.set(SortBit::sortRadix);
            break;
        default:
            printUnkownParameter(argv);
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

    task.taskDone = true;
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

    task.taskDone = true;
}

void Command::printUnkownParameter(char* const argv[])
{
    const std::string str = "Unknown command line option: " + std::string(argv[0])
        + ". Try with --help to get information.";
    LOGGER_WRN(str.c_str());

    task.taskDone = true;
}

void executeCommand(const char* const command)
{
    try
    {
        const pid_t status = system(command);
        if (-1 == status)
        {
            throw std::runtime_error("System error.");
        }
        else if (WIFEXITED(status))
        {
            if (0 != WEXITSTATUS(status))
            {
                throw ExecuteCommandError(command);
            }
        }
    }
    catch (std::runtime_error const& error)
    {
        LOGGER_ERR(error.what());
    }
    catch (ExecuteCommandError const& error)
    {
        LOGGER_ERR(error.what());
    }
}