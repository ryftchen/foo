#include "command.hpp"
#include "hash.hpp"
#include "integral.hpp"
#include "log.hpp"
#include "optimum.hpp"
#include "thread.hpp"

void Command::parseArgv(const int argc, char* const argv[])
{
    if (argc < 1)
    {
        executeCommand(COMMAND_EXECUTE_OUTPUT_NAME);
        LOGGER_INF("No command line option.");
        return;
    }

    std::bitset<TaskType::taskBottom> taskBitForPreview(0);
    for (int i = 0; i < argc; ++i)
    {
        if ('-' == argv[i][0])
        {
            switch (bkdrHash(argv[i]))
            {
                case "-o"_bkdrHash:
                    [[fallthrough]];
                case "--optimum"_bkdrHash:
                    COMMAND_PREPARE_BIT_SET(TaskType::taskOptimum, taskPlan.optimumBit);
                    break;
                case "-i"_bkdrHash:
                    [[fallthrough]];
                case "--integral"_bkdrHash:
                    COMMAND_PREPARE_BIT_SET(TaskType::taskIntegral, taskPlan.integralBit);
                    break;
                case "-s"_bkdrHash:
                    [[fallthrough]];
                case "--sort"_bkdrHash:
                    COMMAND_PREPARE_BIT_SET(TaskType::taskSort, taskPlan.sortBit);
                    break;
                case "--log"_bkdrHash:
                    COMMAND_PREPARE_TASK_CHECK(false);
                    printLogContext();
                    break;
                case "--help"_bkdrHash:
                    COMMAND_PREPARE_TASK_CHECK(false);
                    printInstruction();
                    break;
                default:
                    printUnexpectedOption(argv + i, true);
                    break;
            }
            if (taskBitForPreview.none())
            {
                return;
            }
        }
        else
        {
            updateTaskPlan(argv + i, taskBitForPreview);
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

void Command::performTask() const
{
    for (int i = 0; i < TaskType::taskBottom; ++i)
    {
        (this->*taskFunctor[TaskType(i)])();
    }
}

bool Command::validatePluralOptions(
    const int argc, char* const argv[], const int index, const bool isAllowSubParam = true)
{
    bool isValidationPass = true;
    if (checkTask())
    {
        printUnexpectedOption(argv + index, false);
        isValidationPass = false;
    }
    else if (argc > (index + 1) && !isAllowSubParam)
    {
        printUnexpectedOption(argv + (index + 1), false);
        isValidationPass = false;
    }

    return isValidationPass;
}

void Command::updateTaskPlan(
    char* const argv[], const std::bitset<TaskType::taskBottom>& taskBitForPreview)
{
    if (taskBitForPreview.test(TaskType::taskOptimum))
    {
        setOptimumBit(argv);
    }
    else if (taskBitForPreview.test(TaskType::taskIntegral))
    {
        setIntegralBit(argv);
    }
    else if (taskBitForPreview.test(TaskType::taskSort))
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
    try
    {
        if (taskPlan.optimumBit.any())
        {
            const auto printFunctor = [](const TargetExpression& expression)
            {
                std::visit(
                    ExpressionOverloaded{
                        [](const Function1& /*unused*/)
                        {
                            std::cout << EXPRESSION_FUN_1_OPTIMUM << std::endl;
                        },
                        [](const Function2& /*unused*/)
                        {
                            std::cout << EXPRESSION_FUN_2_OPTIMUM << std::endl;
                        },
                    },
                    expression);
            };
            const auto resultFunctor =
                [this](const Expression& expression, const ExpressionRange<double, double>& range)
            {
                getOptimumResult(expression, range.range1, range.range2, OPTIMUM_EPSILON);
            };

            COMMAND_PRINT_TASK_TITLE(TaskType::taskOptimum, "BEGIN");
            for ([[maybe_unused]] const auto& [range, expression] : expressionMap)
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
                        [[unlikely]] default : break;
                }
            }
            COMMAND_PRINT_TASK_TITLE(TaskType::taskOptimum, "END");
        }
    }
    catch (CallFunctionError const& error)
    {
        LOGGER_ERR(error.what());
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

    for (int i = 0; i < OptimumMethod::optimumBottom; ++i)
    {
        if (taskPlan.optimumBit.test(OptimumMethod(i)))
        {
            const std::string threadName = taskTable[TaskType::taskOptimum][OptimumMethod(i)];
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
            taskPlan.optimumBit.set(OptimumMethod::optimumFibonacci);
            break;
        case "gra"_bkdrHash:
            taskPlan.optimumBit.set(OptimumMethod::optimumGradient);
            break;
        case "ann"_bkdrHash:
            taskPlan.optimumBit.set(OptimumMethod::optimumAnnealing);
            break;
        case "par"_bkdrHash:
            taskPlan.optimumBit.set(OptimumMethod::optimumParticle);
            break;
        case "gen"_bkdrHash:
            taskPlan.optimumBit.set(OptimumMethod::optimumGenetic);
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
    try
    {
        if (taskPlan.integralBit.any())
        {
            const auto printFunctor = [](const TargetExpression& expression)
            {
                std::visit(
                    ExpressionOverloaded{
                        [](const Function1& /*unused*/)
                        {
                            std::cout << EXPRESSION_FUN_1_INTEGRAL << std::endl;
                        },
                        [](const Function2& /*unused*/)
                        {
                            std::cout << EXPRESSION_FUN_2_INTEGRAL << std::endl;
                        },
                    },
                    expression);
            };
            const auto resultFunctor =
                [this](const Expression& expression, const ExpressionRange<double, double>& range)
            {
                getIntegralResult(expression, range.range1, range.range2, INTEGRAL_EPSILON);
            };

            COMMAND_PRINT_TASK_TITLE(TaskType::taskIntegral, "BEGIN");
            for ([[maybe_unused]] const auto& [range, expression] : expressionMap)
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
                        [[unlikely]] default : break;
                }
            }
            COMMAND_PRINT_TASK_TITLE(TaskType::taskIntegral, "END");
        }
    }
    catch (CallFunctionError const& error)
    {
        LOGGER_ERR(error.what());
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

    for (int i = 0; i < IntegralMethod::integralBottom; ++i)
    {
        if (taskPlan.integralBit.test(IntegralMethod(i)))
        {
            const std::string threadName = taskTable[TaskType::taskIntegral][IntegralMethod(i)];
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
            taskPlan.integralBit.set(IntegralMethod::integralTrapezoidal);
            break;
        case "sim"_bkdrHash:
            taskPlan.integralBit.set(IntegralMethod::integralSimpson);
            break;
        case "rom"_bkdrHash:
            taskPlan.integralBit.set(IntegralMethod::integralRomberg);
            break;
        case "gau"_bkdrHash:
            taskPlan.integralBit.set(IntegralMethod::integralGauss);
            break;
        case "mon"_bkdrHash:
            taskPlan.integralBit.set(IntegralMethod::integralMonteCarlo);
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
    try
    {
        if (taskPlan.sortBit.any())
        {
            const int leftEndpoint = SORT_ARRAY_RANGE_1;
            const int rightEndpoint = SORT_ARRAY_RANGE_2;
            const uint32_t length = SORT_ARRAY_LENGTH;
            static_assert((leftEndpoint < rightEndpoint) && (length > 0));

            COMMAND_PRINT_TASK_TITLE(TaskType::taskSort, "BEGIN");
            const std::shared_ptr<Sort<int>> sort =
                std::make_shared<Sort<int>>(length, leftEndpoint, rightEndpoint);
            getSortResult(sort);
            COMMAND_PRINT_TASK_TITLE(TaskType::taskSort, "END");
        }
    }
    catch (CallFunctionError const& error)
    {
        LOGGER_ERR(error.what());
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

    for (int i = 0; i < SortMethod::sortBottom; ++i)
    {
        if (taskPlan.sortBit.test(SortMethod(i)))
        {
            const std::string threadName = taskTable[TaskType::taskSort][SortMethod(i)];
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
            taskPlan.sortBit.set(SortMethod::sortBubble);
            break;
        case "sel"_bkdrHash:
            taskPlan.sortBit.set(SortMethod::sortSelection);
            break;
        case "ins"_bkdrHash:
            taskPlan.sortBit.set(SortMethod::sortInsertion);
            break;
        case "she"_bkdrHash:
            taskPlan.sortBit.set(SortMethod::sortShell);
            break;
        case "mer"_bkdrHash:
            taskPlan.sortBit.set(SortMethod::sortMerge);
            break;
        case "qui"_bkdrHash:
            taskPlan.sortBit.set(SortMethod::sortQuick);
            break;
        case "hea"_bkdrHash:
            taskPlan.sortBit.set(SortMethod::sortHeap);
            break;
        case "cou"_bkdrHash:
            taskPlan.sortBit.set(SortMethod::sortCounting);
            break;
        case "buc"_bkdrHash:
            taskPlan.sortBit.set(SortMethod::sortBucket);
            break;
        case "rad"_bkdrHash:
            taskPlan.sortBit.set(SortMethod::sortRadix);
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
        tryToOperateFileLock(logger.getOfs(), LOG_PATH, false, false);
        printFile(LOG_PATH, true, COMMAND_PRINT_MAX_LINE, &changeLogLevelStyle);
        tryToOperateFileLock(logger.getOfs(), LOG_PATH, true, false);
    }
    catch (LockFileError const& error)
    {
        LOGGER_ERR(error.what());
    }
}

void Command::printInstruction()
{
    std::puts(
        "Usage    : foo [Options...]\n\n"
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

void Command::printUnexpectedOption(char* const argv[], const bool isUnknownOption)
{
    const std::string str = isUnknownOption
        ? ("Unknown command line option: " + std::string(argv[0])
           + ". Try with --help to get information.")
        : ("Excess command line option: " + std::string(argv[0]) + ".");
    LOGGER_WRN(str.c_str());

    taskPlan.reset();
}

std::ostream& operator<<(std::ostream& os, const Command::TaskType& taskType)
{
    switch (taskType)
    {
        case Command::TaskType::taskOptimum:
            os << "OPTIMUM";
            break;
        case Command::TaskType::taskIntegral:
            os << "INGTERGAL";
            break;
        case Command::TaskType::taskSort:
            os << "SORT";
            break;
        default:
            os << "UNKNOWN: " << static_cast<std::underlying_type_t<Command::TaskType>>(taskType);
    }

    return os;
}
