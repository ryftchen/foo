#include "command.hpp"
#include "hash.hpp"
#include "integral.hpp"
#include "log.hpp"
#include "optimum.hpp"
#include "thread.hpp"

Command::Command()
{
    program.addArgument("--help").nArgs(0).defaultValue(false).implicitValue(true).help(
        "show help");

    program.addArgument("--version")
        .nArgs(0)
        .defaultValue(false)
        .implicitValue(true)
        .help("show version");

    program.addArgument("--console")
        .nArgs(util_argument::NArgsPattern::atLeastOne)
        .help("run commands on console");

    program.addArgument("-o", "--optimum")
        .nArgs(util_argument::NArgsPattern::any)
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
        .help("run optimum\r\n"
              "[ fib | gra | ann | par | gen ]    "
              "[ Fibonacci | Gradient | Annealing | Particle | Genetic ]");

    program.addArgument("-i", "--integral")
        .nArgs(util_argument::NArgsPattern::any)
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
        .help("run integral\r\n"
              "[ tra | sim | rom | gau | mon ]    "
              "[ Trapezoidal | Simpson | Romberg | Gauss | MonteCarlo ]");

    program.addArgument("-s", "--sort")
        .nArgs(util_argument::NArgsPattern::any)
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
        .help("run sort\r\n"
              "[ bub | sel | ins | she | mer ]    "
              "[ Bubble | Selection | Insertion | Shell | Merge ]\r\n"
              "[ qui | hea | cou | buc | rad ]    "
              "[ Quick | Heap | Counting | Bucket | Radix ]");
}

void Command::runCommander(const int argc, const char* const argv[])
{
    LOGGER_START;

    try
    {
        if (0 != argc - 1)
        {
            foregroundHandle(argc, argv);
            backgroundHandle();

            LOGGER_STOP;
            return;
        }

        enterConsole();
    }
    catch (const std::exception& error)
    {
        LOGGER_ERR(error.what());
    }

    LOGGER_STOP;
}

void Command::foregroundHandle(const int argc, const char* const argv[])
{
    program.parseArgs(argc, argv);

    validateAlgorithmTask();
    validateUtilityTask();
}

void Command::backgroundHandle() const
{
    if (checkTask())
    {
        performTask();
    }
}

void Command::validateAlgorithmTask()
{
    for (int i = 0; i < AlgoTaskType::algoTaskBottom; ++i)
    {
        if (program.isUsed("--" + algoTaskNameTable[AlgoTaskType(i)]))
        {
            const auto methods =
                program.get<std::vector<std::string>>("--" + algoTaskNameTable[AlgoTaskType(i)]);
            if (!methods.empty() && !checkTask())
            {
                for (const auto& method : methods)
                {
                    (this->*setAlgoTaskBitFunctor[AlgoTaskType(i)])(method.c_str());
                }
            }
            else
            {
                COMMAND_CHECK_EXIST_EXCESS_ARG;
                (this->algoTaskBitPtr[AlgoTaskType(i)])->set();
            }
        }
    }
}

void Command::validateUtilityTask()
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
    if (!taskPlan.algoTask.empty())
    {
        for (int i = 0; i < AlgoTaskType::algoTaskBottom; ++i)
        {
            (this->*performAlgoTaskFunctor[AlgoTaskType(i)])();
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
        if (taskPlan.algoTask.optimumBit.any())
        {
            const auto printFunctor = [](const algo_expression::TargetExpression& expression)
            {
                std::visit(
                    algo_expression::ExpressionOverloaded{
                        [](const algo_expression::Function1& /*unused*/)
                        {
                            std::cout << EXPRESSION_FUN_1_OPTIMUM << std::endl;
                        },
                        [](const algo_expression::Function2& /*unused*/)
                        {
                            std::cout << EXPRESSION_FUN_2_OPTIMUM << std::endl;
                        },
                    },
                    expression);
            };
            const auto resultFunctor =
                [this](
                    const algo_expression::Expression& expression,
                    const algo_expression::ExpressionRange<double, double>& range)
            {
                getOptimumResult(expression, range.range1, range.range2, OPTIMUM_EPSILON);
            };

            COMMAND_PRINT_ALGO_TASK_TITLE(AlgoTaskType::optimum, "BEGIN");
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
            COMMAND_PRINT_ALGO_TASK_TITLE(AlgoTaskType::optimum, "END");
        }
    }
    catch (const std::exception& error)
    {
        LOGGER_ERR(error.what());
    }
}

void Command::getOptimumResult(
    const algo_expression::Expression& express, const double leftEndpoint,
    const double rightEndpoint, const double epsilon) const
{
    assert((leftEndpoint < rightEndpoint) && (epsilon > 0.0));
    util_thread::Thread threadPool(std::min(
        static_cast<uint32_t>(taskPlan.algoTask.optimumBit.count()),
        static_cast<uint32_t>(OptimumMethod::optimumBottom)));
    const auto optimumFunctor =
        [&](const std::string& threadName, const std::shared_ptr<algo_optimum::Optimum>& classPtr)
    {
        threadPool.enqueue(
            threadName, &algo_optimum::Optimum::operator(), classPtr, leftEndpoint, rightEndpoint,
            epsilon);
    };

    for (int i = 0; i < OptimumMethod::optimumBottom; ++i)
    {
        if (taskPlan.algoTask.optimumBit.test(OptimumMethod(i)))
        {
            const std::string threadName =
                std::string{1, std::string_view{algoTaskNameTable[AlgoTaskType::optimum]}.at(0)}
                + "_" + algoTaskMethodTable[AlgoTaskType::optimum][OptimumMethod(i)];
            switch (util_hash::bkdrHash(
                algoTaskMethodTable[AlgoTaskType::optimum][OptimumMethod(i)].c_str()))
            {
                case HASH_BKDR("fib"):
                    optimumFunctor(threadName, std::make_shared<algo_optimum::Fibonacci>(express));
                    break;
                case HASH_BKDR("gra"):
                    optimumFunctor(threadName, std::make_shared<algo_optimum::Gradient>(express));
                    break;
                case HASH_BKDR("ann"):
                    optimumFunctor(threadName, std::make_shared<algo_optimum::Annealing>(express));
                    break;
                case HASH_BKDR("par"):
                    optimumFunctor(threadName, std::make_shared<algo_optimum::Particle>(express));
                    break;
                case HASH_BKDR("gen"):;
                    optimumFunctor(threadName, std::make_shared<algo_optimum::Genetic>(express));
                    break;
                default:
                    break;
            }
        }
    }
}

void Command::setOptimumBit(const char* const method)
{
    switch (util_hash::bkdrHash(method))
    {
        case HASH_BKDR("fib"):
            taskPlan.algoTask.optimumBit.set(OptimumMethod::fibonacci);
            break;
        case HASH_BKDR("gra"):
            taskPlan.algoTask.optimumBit.set(OptimumMethod::gradient);
            break;
        case HASH_BKDR("ann"):
            taskPlan.algoTask.optimumBit.set(OptimumMethod::annealing);
            break;
        case HASH_BKDR("par"):
            taskPlan.algoTask.optimumBit.set(OptimumMethod::particle);
            break;
        case HASH_BKDR("gen"):
            taskPlan.algoTask.optimumBit.set(OptimumMethod::genetic);
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
        if (taskPlan.algoTask.integralBit.any())
        {
            const auto printFunctor = [](const algo_expression::TargetExpression& expression)
            {
                std::visit(
                    algo_expression::ExpressionOverloaded{
                        [](const algo_expression::Function1& /*unused*/)
                        {
                            std::cout << EXPRESSION_FUN_1_INTEGRAL << std::endl;
                        },
                        [](const algo_expression::Function2& /*unused*/)
                        {
                            std::cout << EXPRESSION_FUN_2_INTEGRAL << std::endl;
                        },
                    },
                    expression);
            };
            const auto resultFunctor =
                [this](
                    const algo_expression::Expression& expression,
                    const algo_expression::ExpressionRange<double, double>& range)
            {
                getIntegralResult(expression, range.range1, range.range2, INTEGRAL_EPSILON);
            };

            COMMAND_PRINT_ALGO_TASK_TITLE(AlgoTaskType::integral, "BEGIN");
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
            COMMAND_PRINT_ALGO_TASK_TITLE(AlgoTaskType::integral, "END");
        }
    }
    catch (const std::exception& error)
    {
        LOGGER_ERR(error.what());
    }
}

void Command::getIntegralResult(
    const algo_expression::Expression& express, const double lowerLimit, const double upperLimit,
    const double epsilon) const
{
    assert(epsilon > 0.0);
    util_thread::Thread threadPool(std::min(
        static_cast<uint32_t>(taskPlan.algoTask.integralBit.count()),
        static_cast<uint32_t>(IntegralMethod::integralBottom)));
    const auto integralFunctor =
        [&](const std::string& threadName, const std::shared_ptr<algo_integral::Integral>& classPtr)
    {
        threadPool.enqueue(
            threadName, &algo_integral::Integral::operator(), classPtr, lowerLimit, upperLimit,
            epsilon);
    };

    for (int i = 0; i < IntegralMethod::integralBottom; ++i)
    {
        if (taskPlan.algoTask.integralBit.test(IntegralMethod(i)))
        {
            const std::string threadName =
                std::string{1, std::string_view{algoTaskNameTable[AlgoTaskType::integral]}.at(0)}
                + "_" + algoTaskMethodTable[AlgoTaskType::integral][IntegralMethod(i)];
            switch (util_hash::bkdrHash(
                algoTaskMethodTable[AlgoTaskType::integral][IntegralMethod(i)].c_str()))
            {
                case HASH_BKDR("tra"):
                    integralFunctor(
                        threadName, std::make_shared<algo_integral::Trapezoidal>(express));
                    break;
                case HASH_BKDR("sim"):
                    integralFunctor(threadName, std::make_shared<algo_integral::Simpson>(express));
                    break;
                case HASH_BKDR("rom"):
                    integralFunctor(threadName, std::make_shared<algo_integral::Romberg>(express));
                    break;
                case HASH_BKDR("gau"):
                    integralFunctor(threadName, std::make_shared<algo_integral::Gauss>(express));
                    break;
                case HASH_BKDR("mon"):
                    integralFunctor(
                        threadName, std::make_shared<algo_integral::MonteCarlo>(express));
                    break;
                default:
                    break;
            }
        }
    }
}

void Command::setIntegralBit(const char* const method)
{
    switch (util_hash::bkdrHash(method))
    {
        case HASH_BKDR("tra"):
            taskPlan.algoTask.integralBit.set(IntegralMethod::trapezoidal);
            break;
        case HASH_BKDR("sim"):
            taskPlan.algoTask.integralBit.set(IntegralMethod::simpson);
            break;
        case HASH_BKDR("rom"):
            taskPlan.algoTask.integralBit.set(IntegralMethod::romberg);
            break;
        case HASH_BKDR("gau"):
            taskPlan.algoTask.integralBit.set(IntegralMethod::gauss);
            break;
        case HASH_BKDR("mon"):
            taskPlan.algoTask.integralBit.set(IntegralMethod::monteCarlo);
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
        if (taskPlan.algoTask.sortBit.any())
        {
            const int leftEndpoint = SORT_ARRAY_RANGE_1;
            const int rightEndpoint = SORT_ARRAY_RANGE_2;
            const uint32_t length = SORT_ARRAY_LENGTH;
            static_assert((leftEndpoint < rightEndpoint) && (length > 0));

            COMMAND_PRINT_ALGO_TASK_TITLE(AlgoTaskType::sort, "BEGIN");
            const std::shared_ptr<algo_sort::Sort<int>> sort =
                std::make_shared<algo_sort::Sort<int>>(length, leftEndpoint, rightEndpoint);
            getSortResult(sort);
            COMMAND_PRINT_ALGO_TASK_TITLE(AlgoTaskType::sort, "END");
        }
    }
    catch (const std::exception& error)
    {
        LOGGER_ERR(error.what());
    }
}

template <typename T>
void Command::getSortResult(const std::shared_ptr<algo_sort::Sort<T>>& sort) const
{
    util_thread::Thread threadPool(std::min(
        static_cast<uint32_t>(taskPlan.algoTask.sortBit.count()),
        static_cast<uint32_t>(SortMethod::sortBottom)));
    const auto sortFunctor = [&](const std::string& threadName,
                                 void (algo_sort::Sort<T>::*methodPtr)(T* const, const uint32_t)
                                     const)
    {
        threadPool.enqueue(
            threadName, methodPtr, sort, sort->getRandomArray().get(), sort->getLength());
    };

    for (int i = 0; i < SortMethod::sortBottom; ++i)
    {
        if (taskPlan.algoTask.sortBit.test(SortMethod(i)))
        {
            const std::string threadName =
                std::string{1, std::string_view{algoTaskNameTable[AlgoTaskType::sort]}.at(0)} + "_"
                + algoTaskMethodTable[AlgoTaskType::sort][SortMethod(i)];
            switch (
                util_hash::bkdrHash(algoTaskMethodTable[AlgoTaskType::sort][SortMethod(i)].c_str()))
            {
                case HASH_BKDR("bub"):
                    sortFunctor(threadName, &algo_sort::Sort<T>::bubbleSort);
                    break;
                case HASH_BKDR("sec"):
                    sortFunctor(threadName, &algo_sort::Sort<T>::selectionSort);
                    break;
                case HASH_BKDR("ins"):
                    sortFunctor(threadName, &algo_sort::Sort<T>::insertionSort);
                    break;
                case HASH_BKDR("she"):
                    sortFunctor(threadName, &algo_sort::Sort<T>::shellSort);
                    break;
                case HASH_BKDR("mer"):
                    sortFunctor(threadName, &algo_sort::Sort<T>::mergeSort);
                    break;
                case HASH_BKDR("qui"):
                    sortFunctor(threadName, &algo_sort::Sort<T>::quickSort);
                    break;
                case HASH_BKDR("hea"):
                    sortFunctor(threadName, &algo_sort::Sort<T>::heapSort);
                    break;
                case HASH_BKDR("cou"):
                    sortFunctor(threadName, &algo_sort::Sort<T>::countingSort);
                    break;
                case HASH_BKDR("buc"):
                    sortFunctor(threadName, &algo_sort::Sort<T>::bucketSort);
                    break;
                case HASH_BKDR("rad"):
                    sortFunctor(threadName, &algo_sort::Sort<T>::radixSort);
                    break;
                default:
                    break;
            }
        }
    }
}

void Command::setSortBit(const char* const method)
{
    switch (util_hash::bkdrHash(method))
    {
        case HASH_BKDR("bub"):
            taskPlan.algoTask.sortBit.set(SortMethod::bubble);
            break;
        case HASH_BKDR("sel"):
            taskPlan.algoTask.sortBit.set(SortMethod::selection);
            break;
        case HASH_BKDR("ins"):
            taskPlan.algoTask.sortBit.set(SortMethod::insertion);
            break;
        case HASH_BKDR("she"):
            taskPlan.algoTask.sortBit.set(SortMethod::shell);
            break;
        case HASH_BKDR("mer"):
            taskPlan.algoTask.sortBit.set(SortMethod::merge);
            break;
        case HASH_BKDR("qui"):
            taskPlan.algoTask.sortBit.set(SortMethod::quick);
            break;
        case HASH_BKDR("hea"):
            taskPlan.algoTask.sortBit.set(SortMethod::heap);
            break;
        case HASH_BKDR("cou"):
            taskPlan.algoTask.sortBit.set(SortMethod::counting);
            break;
        case HASH_BKDR("buc"):
            taskPlan.algoTask.sortBit.set(SortMethod::bucket);
            break;
        case HASH_BKDR("rad"):
            taskPlan.algoTask.sortBit.set(SortMethod::radix);
            break;
        default:
            break;
    }
}

void Command::printConsoleOutput() const
{
    const auto commands =
        program.get<std::vector<std::string>>("--" + utilTaskNameTable[UtilTaskType::console]);
    if (!commands.empty())
    {
        util_console::Console console(" > ");
        registerOnConsole(console);
        for (const auto& command : commands)
        {
            console.commandExecutor(command);
        }
    }
}

void Command::printVersionInfo() const
{
    util_file::executeCommand(("echo " + getIconBanner()).c_str());
    std::cout << "Version: " << program.programVersion << std::endl;
}

void Command::printHelpMessage() const
{
    std::cout << program.help().str();
}

std::string Command::getIconBanner()
{
    std::string banner;
    banner += R"(")";
    banner += R"( ______   ______     ______    \n)";
    banner += R"(/\  ___\ /\  __ \   /\  __ \   \n)";
    banner += R"(\ \  __\ \ \ \/\ \  \ \ \/\ \  \n)";
    banner += R"( \ \_\    \ \_____\  \ \_____\ \n)";
    banner += R"(  \/_/     \/_____/   \/_____/ \n)";
    banner += R"(")";

    return banner;
}

void Command::enterConsole() const
{
    util_console::Console console("foo > ");
    registerOnConsole(console);

    util_file::executeCommand(("tput bel; echo " + getIconBanner()).c_str());
    int returnCode;
    do
    {
        returnCode = console.readLine();
        console.setGreeting("foo > ");
    }
    while (util_console::Console::ReturnCode::quit != returnCode);
}

void Command::registerOnConsole(util_console::Console& console) const
{
    console.registerCommand(
        "log",
        [this](const std::vector<std::string>& /*unused*/) -> decltype(auto)
        {
            displayLogContext();
            return util_console::Console::ReturnCode::success;
        },
        "display log");
}

void Command::displayLogContext()
{
    LOGGER_STOP;

    util_file::printFile(LOG_PATH, true, COMMAND_PRINT_MAX_LINE, &util_log::changeLogLevelStyle);
}

void Command::throwUnexpectedMethodException(const std::string& methodInfo)
{
    taskPlan.reset();
    throw std::runtime_error("Unexpected method of " + methodInfo);
}

void Command::throwExcessArgumentException()
{
    taskPlan.reset();
    throw std::runtime_error("Excess argument.");
}

std::ostream& operator<<(std::ostream& os, const Command::AlgoTaskType& taskType)
{
    switch (taskType)
    {
        case Command::AlgoTaskType::optimum:
            os << "OPTIMUM";
            break;
        case Command::AlgoTaskType::integral:
            os << "INTEGRAL";
            break;
        case Command::AlgoTaskType::sort:
            os << "SORT";
            break;
        default:
            os << "UNKNOWN: "
               << static_cast<std::underlying_type_t<Command::AlgoTaskType>>(taskType);
    }

    return os;
}
