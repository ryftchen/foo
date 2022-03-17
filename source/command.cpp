#include "command.hpp"
#include <sys/file.h>
#include <ext/stdio_filebuf.h>
#include "exception.hpp"
#include "hash.hpp"
#include "integral.hpp"
#include "log.hpp"
#include "optimum.hpp"

std::atomic<bool> Command::parseArgv(const int argc, char *const argv[])
{
    if (argc < 1)
    {
        LOGGER(Log::Level::levelInfo, "No command line option.");
        printLicense();
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
                    COMMAND_PERPARE_BITSET(run.optimumBit, TaskBit::taskOptimum);
                    break;
                case "-i"_bkdrHash:
                case "--integral"_bkdrHash:
                    COMMAND_PERPARE_BITSET(run.integralBit, TaskBit::taskIntegral);
                    break;
                case "-s"_bkdrHash:
                case "--sort"_bkdrHash:
                    COMMAND_PERPARE_BITSET(run.sortBit, TaskBit::taskSort);
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

        if (true == run.taskDone)
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
    char *const argv[],
    const std::bitset<TaskBit::taskButtom> &taskBit)
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
    if (run.optimumBit.any())
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
    const Expression &express,
    const double leftEndpoint,
    const double rightEndpoint,
    const double epsilon) const
{
    assert((leftEndpoint > rightEndpoint) && (epsilon > 0.0));
    std::vector<std::shared_ptr<std::thread>> optimumThread;
    const auto optimumFunctor = [&](const std::shared_ptr<Optimum> classPoint,
                                    const char *const threadName) {
        const std::shared_ptr<std::thread> methodThread = std::make_shared<std::thread>(
            &Optimum::operator(), classPoint, leftEndpoint, rightEndpoint, epsilon);
        pthread_setname_np(methodThread->native_handle(), threadName);
        optimumThread.emplace_back(methodThread);
    };

    std::shared_ptr<Optimum> fib, gra, ann, par, gen;
    char threadName[COMMAND_THREAD_NAME_LENGTH] = {'\0'};
    for (int i = 0; i < OptimumBit::optimumButtom; ++i)
    {
        if (run.optimumBit.test(OptimumBit(i)))
        {
            memcpy(
                threadName,
                taskTable[TaskBit::taskOptimum][OptimumBit(i)],
                COMMAND_THREAD_NAME_LENGTH);
            switch (bkdrHash(threadName))
            {
                case "o_fib"_bkdrHash:
                    fib = std::make_shared<Fibonacci>(express);
                    optimumFunctor(fib, threadName);
                    break;
                case "o_gra"_bkdrHash:
                    gra = std::make_shared<Gradient>(express);
                    optimumFunctor(gra, threadName);
                    break;
                case "o_ann"_bkdrHash:
                    ann = std::make_shared<Annealing>(express);
                    optimumFunctor(ann, threadName);
                    break;
                case "o_par"_bkdrHash:
                    par = std::make_shared<Particle>(express);
                    optimumFunctor(par, threadName);
                    break;
                case "o_gen"_bkdrHash:
                    gen = std::make_shared<Genetic>(express);
                    optimumFunctor(gen, threadName);
                    break;
                default:
                    break;
            }
        }
    }

    for (const auto thread : optimumThread)
    {
        thread->join();
    }
}
void Command::setOptimumBit(char *const argv[])
{
    switch (bkdrHash(argv[0]))
    {
        case "fib"_bkdrHash:
            run.optimumBit.set(OptimumBit::optimumFibonacci);
            break;
        case "gra"_bkdrHash:
            run.optimumBit.set(OptimumBit::optimumGradient);
            break;
        case "ann"_bkdrHash:
            run.optimumBit.set(OptimumBit::optimumAnnealing);
            break;
        case "par"_bkdrHash:
            run.optimumBit.set(OptimumBit::optimumParticle);
            break;
        case "gen"_bkdrHash:
            run.optimumBit.set(OptimumBit::optimumGenetic);
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
    if (run.integralBit.any())
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
    const Expression &express,
    const double lowerLimit,
    const double upperLimit,
    const double epsilon) const
{
    assert(epsilon > 0.0);
    std::vector<std::shared_ptr<std::thread>> integralThread;
    const auto integralFunctor = [&](const std::shared_ptr<Integral> classPoint,
                                     const char *const threadName) {
        const std::shared_ptr<std::thread> methodThread = std::make_shared<std::thread>(
            &Integral::operator(), classPoint, lowerLimit, upperLimit, epsilon);
        pthread_setname_np(methodThread->native_handle(), threadName);
        integralThread.emplace_back(methodThread);
    };

    std::shared_ptr<Integral> tra, sim, rom, gau, mon;
    char threadName[COMMAND_THREAD_NAME_LENGTH] = {'\0'};
    for (int i = 0; i < IntegralBit::integralButtom; ++i)
    {
        if (run.integralBit.test(IntegralBit(i)))
        {
            memcpy(
                threadName,
                taskTable[TaskBit::taskIntegral][IntegralBit(i)],
                COMMAND_THREAD_NAME_LENGTH);
            switch (bkdrHash(threadName))
            {
                case "i_tra"_bkdrHash:
                    tra = std::make_shared<Trapezoidal>(express);
                    integralFunctor(tra, threadName);
                    break;
                case "i_sim"_bkdrHash:
                    sim = std::make_shared<Simpson>(express);
                    integralFunctor(sim, threadName);
                    break;
                case "i_rom"_bkdrHash:
                    rom = std::make_shared<Romberg>(express);
                    integralFunctor(rom, threadName);
                    break;
                case "i_gau"_bkdrHash:
                    gau = std::make_shared<Gauss>(express);
                    integralFunctor(gau, threadName);
                    break;
                case "i_mon"_bkdrHash:
                    mon = std::make_shared<MonteCarlo>(express);
                    integralFunctor(mon, threadName);
                    break;
                default:
                    break;
            }
        }
    }

    for (const auto thread : integralThread)
    {
        thread->join();
    }
}
void Command::setIntegralBit(char *const argv[])
{
    switch (bkdrHash(argv[0]))
    {
        case "tra"_bkdrHash:
            run.integralBit.set(IntegralBit::integralTrapezoidal);
            break;
        case "sim"_bkdrHash:
            run.integralBit.set(IntegralBit::integralSimpson);
            break;
        case "rom"_bkdrHash:
            run.integralBit.set(IntegralBit::integralRomberg);
            break;
        case "gau"_bkdrHash:
            run.integralBit.set(IntegralBit::integralGauss);
            break;
        case "mon"_bkdrHash:
            run.integralBit.set(IntegralBit::integralMonteCarlo);
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
    if (run.sortBit.any())
    {
        constexpr const int leftEndpoint = SORT_ARRAY_RANGE_1;
        constexpr const int rightEndpoint = SORT_ARRAY_RANGE_2;
        constexpr const uint32_t length = SORT_ARRAY_LENGTH;
        static_assert((leftEndpoint < rightEndpoint) && (length > 0));

        const std::shared_ptr<Sort<int>> sort =
            std::make_shared<Sort<int>>(length, leftEndpoint, rightEndpoint);
        getSortResult(sort);
    }
}
void Command::getSortResult(const std::shared_ptr<Sort<int>> &sort) const
{
    std::vector<std::shared_ptr<std::thread>> sortThread;
    const auto sortFunctor = [&](void (Sort<int>::*methodPoint)(int *const, const uint32_t) const,
                                 const char *const threadName) {
        const std::shared_ptr<std::thread> methodThread = std::make_shared<std::thread>(
            methodPoint, sort, sort->getRandomArray().get(), sort->getLength());
        pthread_setname_np(methodThread->native_handle(), threadName);
        sortThread.emplace_back(methodThread);
    };

    char threadName[COMMAND_THREAD_NAME_LENGTH] = {'\0'};
    for (int i = 0; i < SortBit::sortButtom; ++i)
    {
        if (run.sortBit.test(SortBit(i)))
        {
            memcpy(
                threadName, taskTable[TaskBit::taskSort][SortBit(i)], COMMAND_THREAD_NAME_LENGTH);
            switch (bkdrHash(threadName))
            {
                case "s_bub"_bkdrHash:
                    sortFunctor(&Sort<int>::bubbleSort, threadName);
                    break;
                case "s_sec"_bkdrHash:
                    sortFunctor(&Sort<int>::selectionSort, threadName);
                    break;
                case "s_ins"_bkdrHash:
                    sortFunctor(&Sort<int>::insertionSort, threadName);
                    break;
                case "s_she"_bkdrHash:
                    sortFunctor(&Sort<int>::shellSort, threadName);
                    break;
                case "s_mer"_bkdrHash:
                    sortFunctor(&Sort<int>::mergeSort, threadName);
                    break;
                case "s_qui"_bkdrHash:
                    sortFunctor(&Sort<int>::quickSort, threadName);
                    break;
                case "s_hea"_bkdrHash:
                    sortFunctor(&Sort<int>::heapSort, threadName);
                    break;
                case "s_cou"_bkdrHash:
                    sortFunctor(&Sort<int>::countingSort, threadName);
                    break;
                case "s_buc"_bkdrHash:
                    sortFunctor(&Sort<int>::bucketSort, threadName);
                    break;
                case "s_rad"_bkdrHash:
                    sortFunctor(&Sort<int>::radixSort, threadName);
                    break;
                default:
                    break;
            }
        }
    }

    for (const auto thread : sortThread)
    {
        thread->join();
    }
}
void Command::setSortBit(char *const argv[])
{
    switch (bkdrHash(argv[0]))
    {
        case "bub"_bkdrHash:
            run.sortBit.set(SortBit::sortBubble);
            break;
        case "sel"_bkdrHash:
            run.sortBit.set(SortBit::sortSelection);
            break;
        case "ins"_bkdrHash:
            run.sortBit.set(SortBit::sortInsertion);
            break;
        case "she"_bkdrHash:
            run.sortBit.set(SortBit::sortShell);
            break;
        case "mer"_bkdrHash:
            run.sortBit.set(SortBit::sortMerge);
            break;
        case "qui"_bkdrHash:
            run.sortBit.set(SortBit::sortQuick);
            break;
        case "hea"_bkdrHash:
            run.sortBit.set(SortBit::sortHeap);
            break;
        case "cou"_bkdrHash:
            run.sortBit.set(SortBit::sortCounting);
            break;
        case "buc"_bkdrHash:
            run.sortBit.set(SortBit::sortBucket);
            break;
        case "rad"_bkdrHash:
            run.sortBit.set(SortBit::sortRadix);
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
            static_cast<__gnu_cxx::stdio_filebuf<char> *const>(logger.getOfs().rdbuf())->fd();
        if (flock(fd, LOCK_UN))
        {
            throw UnlockWriterLockError(basename(LOG_PATH));
        }

        printFile(LOG_PATH, true, COMMAND_PRINT_MAX_LINE);
    }
    catch (UnlockWriterLockError const &error)
    {
        LOGGER(Log::Level::levelError, error.what());
    }

    run.taskDone = true;
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

    run.taskDone = true;
}

void Command::printLicense()
{
    try
    {
        const pid_t status = system(COMMAND_LICENSE_CMD);
        if (-1 == status)
        {
            throw std::runtime_error("System error.");
        }
        else if (WIFEXITED(status))
        {
            if (0 != WEXITSTATUS(status))
            {
                throw RunCommandError(COMMAND_LICENSE_CMD);
            }
        }
    }
    catch (std::runtime_error const &error)
    {
        LOGGER(Log::Level::levelError, error.what());
    }
    catch (RunCommandError const &error)
    {
        LOGGER(Log::Level::levelError, error.what());
    }

    run.taskDone = true;
}

void Command::printUnkownParameter(char *const argv[])
{
    const std::string str = "Unknown command line option: " + std::string(argv[0]) +
        ". Try with --help to get information.";
    LOGGER(Log::Level::levelWarn, str.c_str());

    run.taskDone = true;
}
