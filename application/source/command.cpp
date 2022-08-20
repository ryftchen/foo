#include "command.hpp"
#include "hash.hpp"
#include "integral.hpp"
#include "log.hpp"
#include "optimum.hpp"
#include "thread.hpp"

Command::Command()
{
    program.addArgument("-o", "--optimum")
        .nArgs(NArgsPattern::any)
        .action(
            [this](const std::string& value)
            {
                static const std::vector<std::string> choices = {"fib", "gra", "ann", "par", "gen"};
                if (std::find(choices.begin(), choices.end(), value) != choices.end())
                {
                    return value;
                }
                throwUnexpectedMethodException("optimum: " + value);
            })
        .help("run optimum"
              "\r\n    [ fib | gra | ann | par | gen ]"
              "    Fibonacci | Gradient | Annealing | Particle | Genetic");

    program.addArgument("-i", "--integral")
        .nArgs(NArgsPattern::any)
        .action(
            [this](const std::string& value)
            {
                static const std::vector<std::string> choices = {"tra", "sim", "rom", "gau", "mon"};
                if (std::find(choices.begin(), choices.end(), value) != choices.end())
                {
                    return value;
                }
                throwUnexpectedMethodException("integral: " + value);
            })
        .help("run integral"
              "\r\n    [ tra | sim | rom | gau | mon ]"
              "    Trapezoidal | Simpson | Romberg | Gauss | MonteCarlo");

    program.addArgument("-s", "--sort")
        .nArgs(NArgsPattern::any)
        .action(
            [this](const std::string& value)
            {
                static const std::vector<std::string> choices = {"bub", "sel", "ins", "she", "mer",
                                                                 "qui", "hea", "cou", "buc", "rad"};
                if (std::find(choices.begin(), choices.end(), value) != choices.end())
                {
                    return value;
                }
                throwUnexpectedMethodException("sort: " + value);
            })
        .help("run sort"
              "\r\n    [ bub | sel | ins | she | mer ]"
              "    Bubble | Selection | Insertion | Shell | Merge"
              "\r\n    [ qui | hea | cou | buc | rad ]"
              "    Quick | Heap | Counting | Bucket | Radix");

    program.addArgument("--log").nArgs(0).help("display log and exit program");

    program.addArgument("--version")
        .nArgs(0)
        .defaultValue(false)
        .implicitValue(true)
        .help("show version");

    program.addArgument("--help").nArgs(0).defaultValue(false).implicitValue(true).help(
        "show help");
}

void Command::runCommand(const int argc, const char* const argv[])
{
    try
    {
        if (0 == argc - 1)
        {
            LOGGER_INF("No command line option.");
            printVersionInfo();
            printHelpMessage();

            TIME_WAIT_MILLISECOND_50;
            LOGGER_EXIT;
            return;
        }

        foregroundHandle(argc, argv);
        backgroundHandle();
    }
    catch (const std::exception& error)
    {
        LOGGER_ERR(error.what());
    }

    TIME_WAIT_MILLISECOND_50;
    LOGGER_EXIT;
}

void Command::foregroundHandle(const int argc, const char* const argv[])
{
    program.parseArgs(argc, argv);

    precheckAlgorithmTask();
    precheckUtilityTask();
}

void Command::backgroundHandle() const
{
    if (checkTask())
    {
        performTask();
    }
}

void Command::precheckAlgorithmTask()
{
    for (int i = 0; i < AlgTaskType::algTaskBottom; ++i)
    {
        if (program.isUsed("--" + algTaskNameTable[AlgTaskType(i)]))
        {
            auto methods =
                program.get<std::vector<std::string>>("--" + algTaskNameTable[AlgTaskType(i)]);
            if (!methods.empty() && !checkTask())
            {
                for (auto method : methods)
                {
                    (this->*setAlgTaskBitFunctor[AlgTaskType(i)])(method.c_str());
                }
            }
            else
            {
                COMMAND_CHECK_EXIST_EXCESS_ARG;
                (this->algTaskBitPtr[AlgTaskType(i)])->set();
            }
        }
    }
}

void Command::precheckUtilityTask()
{
    for (int i = 0; i < UtilTaskType::utilTaskBottom; ++i)
    {
        if (program.isUsed("--" + utilTaskNameTable[UtilTaskType(i)]))
        {
            COMMAND_CHECK_EXIST_EXCESS_ARG;
            taskPlan.utilTask.utilTaskBit.set(UtilTaskType(i));
        }
    }
}

bool Command::checkTask() const
{
    return !taskPlan.empty();
}

void Command::performTask() const
{
    if (!taskPlan.algTask.empty())
    {
        for (int i = 0; i < AlgTaskType::algTaskBottom; ++i)
        {
            (this->*performAlgTaskFunctor[AlgTaskType(i)])();
        }
    }

    if (!taskPlan.utilTask.empty())
    {
        for (int j = 0; j < UtilTaskType::utilTaskBottom; ++j)
        {
            if (taskPlan.utilTask.utilTaskBit.test(UtilTaskType(j)))
            {
                (this->*performUtilTaskFunctor[UtilTaskType(j)])();
            }
        }
    }
}

// Optimum
void Command::runOptimum() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    try
    {
        if (taskPlan.algTask.optimumBit.any())
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

            COMMAND_PRINT_ALG_TASK_TITLE(AlgTaskType::optimum, "BEGIN");
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
            COMMAND_PRINT_ALG_TASK_TITLE(AlgTaskType::optimum, "END");
        }
    }
    catch (const std::exception& error)
    {
        LOGGER_ERR(error.what());
    }
}

void Command::getOptimumResult(
    const Expression& express, const double leftEndpoint, const double rightEndpoint,
    const double epsilon) const
{
    assert((leftEndpoint > rightEndpoint) && (epsilon > 0.0));
    Thread threadPool(std::min(
        static_cast<uint32_t>(taskPlan.algTask.optimumBit.count()),
        static_cast<uint32_t>(OptimumMethod::optimumBottom)));
    const auto optimumFunctor =
        [&](const std::string& threadName, const std::shared_ptr<Optimum>& classPtr)
    {
        threadPool.enqueue(
            threadName, &Optimum::operator(), classPtr, leftEndpoint, rightEndpoint, epsilon);
    };

    for (int i = 0; i < OptimumMethod::optimumBottom; ++i)
    {
        if (taskPlan.algTask.optimumBit.test(OptimumMethod(i)))
        {
            const std::string threadName =
                std::string{1, algTaskNameTable[AlgTaskType::optimum].at(0)} + "_"
                + algTaskMethodTable[AlgTaskType::optimum][OptimumMethod(i)];
            switch (bkdrHash(algTaskMethodTable[AlgTaskType::optimum][OptimumMethod(i)].c_str()))
            {
                case "fib"_bkdrHash:
                    optimumFunctor(threadName, std::make_shared<Fibonacci>(express));
                    break;
                case "gra"_bkdrHash:
                    optimumFunctor(threadName, std::make_shared<Gradient>(express));
                    break;
                case "ann"_bkdrHash:
                    optimumFunctor(threadName, std::make_shared<Annealing>(express));
                    break;
                case "par"_bkdrHash:
                    optimumFunctor(threadName, std::make_shared<Particle>(express));
                    break;
                case "gen"_bkdrHash:;
                    optimumFunctor(threadName, std::make_shared<Genetic>(express));
                    break;
                default:
                    break;
            }
        }
    }
}

void Command::setOptimumBit(const char* const method)
{
    switch (bkdrHash(method))
    {
        case "fib"_bkdrHash:
            taskPlan.algTask.optimumBit.set(OptimumMethod::fibonacci);
            break;
        case "gra"_bkdrHash:
            taskPlan.algTask.optimumBit.set(OptimumMethod::gradient);
            break;
        case "ann"_bkdrHash:
            taskPlan.algTask.optimumBit.set(OptimumMethod::annealing);
            break;
        case "par"_bkdrHash:
            taskPlan.algTask.optimumBit.set(OptimumMethod::particle);
            break;
        case "gen"_bkdrHash:
            taskPlan.algTask.optimumBit.set(OptimumMethod::genetic);
            break;
        default:
            break;
    }
}

// Integral
void Command::runIntegral() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    try
    {
        if (taskPlan.algTask.integralBit.any())
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

            COMMAND_PRINT_ALG_TASK_TITLE(AlgTaskType::integral, "BEGIN");
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
            COMMAND_PRINT_ALG_TASK_TITLE(AlgTaskType::integral, "END");
        }
    }
    catch (const std::exception& error)
    {
        LOGGER_ERR(error.what());
    }
}

void Command::getIntegralResult(
    const Expression& express, const double lowerLimit, const double upperLimit,
    const double epsilon) const
{
    assert(epsilon > 0.0);
    Thread threadPool(std::min(
        static_cast<uint32_t>(taskPlan.algTask.integralBit.count()),
        static_cast<uint32_t>(IntegralMethod::integralBottom)));
    const auto integralFunctor =
        [&](const std::string& threadName, const std::shared_ptr<Integral>& classPtr)
    {
        threadPool.enqueue(
            threadName, &Integral::operator(), classPtr, lowerLimit, upperLimit, epsilon);
    };

    for (int i = 0; i < IntegralMethod::integralBottom; ++i)
    {
        if (taskPlan.algTask.integralBit.test(IntegralMethod(i)))
        {
            const std::string threadName =
                std::string{1, algTaskNameTable[AlgTaskType::integral].at(0)} + "_"
                + algTaskMethodTable[AlgTaskType::integral][IntegralMethod(i)];
            switch (bkdrHash(algTaskMethodTable[AlgTaskType::integral][IntegralMethod(i)].c_str()))
            {
                case "tra"_bkdrHash:
                    integralFunctor(threadName, std::make_shared<Trapezoidal>(express));
                    break;
                case "sim"_bkdrHash:
                    integralFunctor(threadName, std::make_shared<Simpson>(express));
                    break;
                case "rom"_bkdrHash:
                    integralFunctor(threadName, std::make_shared<Romberg>(express));
                    break;
                case "gau"_bkdrHash:
                    integralFunctor(threadName, std::make_shared<Gauss>(express));
                    break;
                case "mon"_bkdrHash:
                    integralFunctor(threadName, std::make_shared<MonteCarlo>(express));
                    break;
                default:
                    break;
            }
        }
    }
}

void Command::setIntegralBit(const char* const method)
{
    switch (bkdrHash(method))
    {
        case "tra"_bkdrHash:
            taskPlan.algTask.integralBit.set(IntegralMethod::trapezoidal);
            break;
        case "sim"_bkdrHash:
            taskPlan.algTask.integralBit.set(IntegralMethod::simpson);
            break;
        case "rom"_bkdrHash:
            taskPlan.algTask.integralBit.set(IntegralMethod::romberg);
            break;
        case "gau"_bkdrHash:
            taskPlan.algTask.integralBit.set(IntegralMethod::gauss);
            break;
        case "mon"_bkdrHash:
            taskPlan.algTask.integralBit.set(IntegralMethod::monteCarlo);
            break;
        default:
            break;
    }
}

// Sort
void Command::runSort() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    try
    {
        if (taskPlan.algTask.sortBit.any())
        {
            const int leftEndpoint = SORT_ARRAY_RANGE_1;
            const int rightEndpoint = SORT_ARRAY_RANGE_2;
            const uint32_t length = SORT_ARRAY_LENGTH;
            static_assert((leftEndpoint < rightEndpoint) && (length > 0));

            COMMAND_PRINT_ALG_TASK_TITLE(AlgTaskType::sort, "BEGIN");
            const std::shared_ptr<Sort<int>> sort =
                std::make_shared<Sort<int>>(length, leftEndpoint, rightEndpoint);
            getSortResult(sort);
            COMMAND_PRINT_ALG_TASK_TITLE(AlgTaskType::sort, "END");
        }
    }
    catch (const std::exception& error)
    {
        LOGGER_ERR(error.what());
    }
}

template <typename T>
void Command::getSortResult(const std::shared_ptr<Sort<T>>& sort) const
{
    Thread threadPool(std::min(
        static_cast<uint32_t>(taskPlan.algTask.sortBit.count()),
        static_cast<uint32_t>(SortMethod::sortBottom)));
    const auto sortFunctor = [&](const std::string& threadName,
                                 void (Sort<T>::*methodPtr)(T* const, const uint32_t) const)
    {
        threadPool.enqueue(
            threadName, methodPtr, sort, sort->getRandomArray().get(), sort->getLength());
    };

    for (int i = 0; i < SortMethod::sortBottom; ++i)
    {
        if (taskPlan.algTask.sortBit.test(SortMethod(i)))
        {
            const std::string threadName = std::string{1, algTaskNameTable[AlgTaskType::sort].at(0)}
                + "_" + algTaskMethodTable[AlgTaskType::sort][SortMethod(i)];
            switch (bkdrHash(algTaskMethodTable[AlgTaskType::sort][SortMethod(i)].c_str()))
            {
                case "bub"_bkdrHash:
                    sortFunctor(threadName, &Sort<T>::bubbleSort);
                    break;
                case "sec"_bkdrHash:
                    sortFunctor(threadName, &Sort<T>::selectionSort);
                    break;
                case "ins"_bkdrHash:
                    sortFunctor(threadName, &Sort<T>::insertionSort);
                    break;
                case "she"_bkdrHash:
                    sortFunctor(threadName, &Sort<T>::shellSort);
                    break;
                case "mer"_bkdrHash:
                    sortFunctor(threadName, &Sort<T>::mergeSort);
                    break;
                case "qui"_bkdrHash:
                    sortFunctor(threadName, &Sort<T>::quickSort);
                    break;
                case "hea"_bkdrHash:
                    sortFunctor(threadName, &Sort<T>::heapSort);
                    break;
                case "cou"_bkdrHash:
                    sortFunctor(threadName, &Sort<T>::countingSort);
                    break;
                case "buc"_bkdrHash:
                    sortFunctor(threadName, &Sort<T>::bucketSort);
                    break;
                case "rad"_bkdrHash:
                    sortFunctor(threadName, &Sort<T>::radixSort);
                    break;
                default:
                    break;
            }
        }
    }
}

void Command::setSortBit(const char* const method)
{
    switch (bkdrHash(method))
    {
        case "bub"_bkdrHash:
            taskPlan.algTask.sortBit.set(SortMethod::bubble);
            break;
        case "sel"_bkdrHash:
            taskPlan.algTask.sortBit.set(SortMethod::selection);
            break;
        case "ins"_bkdrHash:
            taskPlan.algTask.sortBit.set(SortMethod::insertion);
            break;
        case "she"_bkdrHash:
            taskPlan.algTask.sortBit.set(SortMethod::shell);
            break;
        case "mer"_bkdrHash:
            taskPlan.algTask.sortBit.set(SortMethod::merge);
            break;
        case "qui"_bkdrHash:
            taskPlan.algTask.sortBit.set(SortMethod::quick);
            break;
        case "hea"_bkdrHash:
            taskPlan.algTask.sortBit.set(SortMethod::heap);
            break;
        case "cou"_bkdrHash:
            taskPlan.algTask.sortBit.set(SortMethod::counting);
            break;
        case "buc"_bkdrHash:
            taskPlan.algTask.sortBit.set(SortMethod::bucket);
            break;
        case "rad"_bkdrHash:
            taskPlan.algTask.sortBit.set(SortMethod::radix);
            break;
        default:
            break;
    }
}

void Command::printLogContext() const
{
    LOGGER_EXIT;
    TIME_WAIT_MILLISECOND_50;
    printFile(
        LOG_PATH, taskPlan.utilTask.logConfig.isReverse, taskPlan.utilTask.logConfig.maxLine,
        &changeLogLevelStyle);
}

void Command::printVersionInfo() const
{
    std::string banner;
    banner += R"(")";
    banner += R"( ______   ______     ______    \n)";
    banner += R"(/\  ___\ /\  __ \   /\  __ \   \n)";
    banner += R"(\ \  __\ \ \ \/\ \  \ \ \/\ \  \n)";
    banner += R"( \ \_\    \ \_____\  \ \_____\ \n)";
    banner += R"(  \/_/     \/_____/   \/_____/ \n)";
    banner += R"(")";

    std::cout << "Version: " << program.programVersion << std::endl;
    executeCommand(("tput bel; echo " + banner).c_str());
}

void Command::printHelpMessage() const
{
    std::cout << program.help().str();
}

void Command::throwUnexpectedMethodException(const std::string methodInfo)
{
    taskPlan.reset();
    throw std::runtime_error("Unexpected method of " + methodInfo);
}

void Command::throwExcessArgumentException()
{
    taskPlan.reset();
    throw std::runtime_error("Excess argument.");
}

std::ostream& operator<<(std::ostream& os, const Command::AlgTaskType& taskType)
{
    switch (taskType)
    {
        case Command::AlgTaskType::optimum:
            os << "OPTIMUM";
            break;
        case Command::AlgTaskType::integral:
            os << "INTEGRAL";
            break;
        case Command::AlgTaskType::sort:
            os << "SORT";
            break;
        default:
            os << "UNKNOWN: "
               << static_cast<std::underlying_type_t<Command::AlgTaskType>>(taskType);
    }

    return os;
}
