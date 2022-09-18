#include "command.hpp"
#include "hash.hpp"
#include "integral.hpp"
#include "log.hpp"
#include "optimum.hpp"
#include "thread.hpp"

#define COMMAND_PRINT_ALGO_TASK_TITLE(taskType, title)                                 \
    std::cout << std::endl                                                             \
              << "TASK " << std::setiosflags(std::ios_base::left) << std::setfill('.') \
              << std::setw(titleWidthForPrintTask) << taskType << title                \
              << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl

Command::Command()
{
    program.addArgument("--help").nArgs(0).implicitValue(true).help("show help");

    program.addArgument("--version").nArgs(0).implicitValue(true).help("show version");

    program.addArgument("--console")
        .nArgs(util_argument::NArgsPattern::any)
        .defaultValue<std::vector<std::string>>({"help"})
        .appending()
        .help("run commands on console");

    program.addArgument("-a", "--algorithm")
        .nArgs(1)
        .action(
            [this](const std::string& value)
            {
                if (std::find(algoTaskNameTable.begin(), algoTaskNameTable.end(), value)
                    != algoTaskNameTable.end())
                {
                    return value;
                }
                throw std::runtime_error("Unknown algorithm category: " + value);
            })
        .help("demo of algorithm, see \"methods\" for detail of specific category");

    program.addArgument("methods").remaining().help("specify method\r\n"
                                                    "└── -a --algorithm\r\n"
                                                    "    ├── optimum\r\n"
                                                    "    │   └── fib, gra, ann, par, gen\r\n"
                                                    "    ├── integral\r\n"
                                                    "    │   └── tra, sim, rom, gau, mon\r\n"
                                                    "    └── sort\r\n"
                                                    "        ├── bub, sel, ins, she, mer\r\n"
                                                    "        └── qui, hea, cou, buc, rad");
}

void Command::runCommander(const int argc, const char* const argv[])
{
    LOG_TO_START(logger);

    if (0 != argc - 1)
    {
        foregroundHandle(argc, argv);
        backgroundHandle();
    }
    else
    {
        LOG_INF(logger, "Enter console mode.");
        enterConsole();
    }

    LOG_TO_STOP(logger);
}

void Command::foregroundHandle(const int argc, const char* const argv[])
{
    try
    {
        program.parseArgs(argc, argv);

        validateUtilityTask();
        validateAlgorithmTask();
    }
    catch (const std::exception& error)
    {
        LOG_WRN(logger, error.what());
    }
}

void Command::backgroundHandle() const
{
    try
    {
        if (checkTask())
        {
            performTask();
        }
    }
    catch (const std::exception& error)
    {
        LOG_ERR(logger, error.what());
    }
}

void Command::validateUtilityTask()
{
    for (int i = 0; i < Bottom<UtilTaskType>::value; ++i)
    {
        if (!program.isUsed(utilTaskNameTable.at(i)))
        {
            continue;
        }

        if (checkTask())
        {
            throwExcessArgumentException();
        }
        taskPlan.utilTask.utilTaskBit.set(UtilTaskType(i));
    }
}

void Command::validateAlgorithmTask()
{
    constexpr std::string_view algoOption{"algorithm"};
    if (!program.isUsed(algoOption))
    {
        return;
    }

    if (checkTask())
    {
        throwExcessArgumentException();
    }
    for (int i = 0; i < Bottom<AlgoTaskType>::value; ++i)
    {
        if (std::string{algoTaskNameTable.at(i)} != program[algoOption])
        {
            continue;
        }

        constexpr std::string_view methOption{"methods"};
        std::vector<std::string> methods;
        if (program.isUsed(methOption))
        {
            methods = program.get<std::vector<std::string>>(methOption);
        }
        else
        {
            methods.assign(algoTaskMethodTable.at(i).cbegin(), algoTaskMethodTable.at(i).cend());
            methods.erase(
                std::remove_if(
                    methods.begin(), methods.end(),
                    [](const std::string& method) -> bool
                    {
                        return (std::string::npos == method.find_first_not_of(" "));
                    }),
                std::end(methods));
        }

        for (const auto& method : methods)
        {
            (this->*setAlgoTaskBitFunctor.at(i))(method);
        }
    }
}

bool Command::checkTask() const
{
    return !taskPlan.empty();
}

void Command::performTask() const
{
    if (!taskPlan.utilTask.empty())
    {
        for (int i = 0; i < Bottom<UtilTaskType>::value; ++i)
        {
            if (taskPlan.utilTask.utilTaskBit.test(UtilTaskType(i)))
            {
                (this->*performUtilTaskFunctor.at(i))();
            }
        }
    }

    if (!taskPlan.algoTask.empty())
    {
        for (int j = 0; j < Bottom<AlgoTaskType>::value; ++j)
        {
            (this->*performAlgoTaskFunctor.at(j))();
        }
    }
}

// Optimum
void Command::runOptimum() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    if (taskPlan.algoTask.optimumBit.none())
    {
        return;
    }

    const auto printFunctor = [](const algo_expression::ExprTarget& expression)
    {
        constexpr std::string_view prefix{"\r\nOptimum Expression: "};
        std::visit(
            algo_expression::ExprOverloaded{
                [&prefix](const algo_expression::Function1& /*unused*/)
                {
                    std::cout << prefix << algo_expression::Function1::optimumExpr << std::endl;
                },
                [&prefix](const algo_expression::Function2& /*unused*/)
                {
                    std::cout << prefix << algo_expression::Function2::optimumExpr << std::endl;
                },
            },
            expression);
    };
    const auto resultFunctor = [this](
                                   const algo_expression::Expression& expression,
                                   const algo_expression::ExprRange<double, double>& range)
    {
        getOptimumResult(expression, range.range1, range.range2, algo_optimum::epsilon);
    };

    COMMAND_PRINT_ALGO_TASK_TITLE(AlgoTaskType::optimum, "BEGIN");
    for ([[maybe_unused]] const auto& [range, expression] : optimumExprMap)
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
    COMMAND_PRINT_ALGO_TASK_TITLE(AlgoTaskType::optimum, "END") << std::endl;
}

void Command::getOptimumResult(
    const algo_expression::Expression& express, const double leftEndpoint,
    const double rightEndpoint, const double epsilon) const
{
    assert((leftEndpoint < rightEndpoint) && (epsilon > 0.0));
    util_thread::Thread threadPool(std::min(
        static_cast<uint32_t>(taskPlan.algoTask.optimumBit.count()),
        static_cast<uint32_t>(Bottom<OptimumMethod>::value)));
    const auto optimumFunctor =
        [&](const std::string& threadName, const std::shared_ptr<algo_optimum::Optimum>& classPtr)
    {
        threadPool.enqueue(
            threadName, &algo_optimum::Optimum::operator(), classPtr, leftEndpoint, rightEndpoint,
            epsilon);
    };

    for (int i = 0; i < Bottom<OptimumMethod>::value; ++i)
    {
        if (!taskPlan.algoTask.optimumBit.test(OptimumMethod(i)))
        {
            continue;
        }

        const std::string threadName =
            std::string{1, algoTaskNameTable.at(AlgoTaskType::optimum).at(0)} + "_"
            + std::string{algoTaskMethodTable.at(AlgoTaskType::optimum).at(i)};
        using util_hash::operator""_bkdrHash;
        switch (util_hash::bkdrHash(algoTaskMethodTable.at(AlgoTaskType::optimum).at(i).data()))
        {
            case "fib"_bkdrHash:
                optimumFunctor(threadName, std::make_shared<algo_optimum::Fibonacci>(express));
                break;
            case "gra"_bkdrHash:
                optimumFunctor(threadName, std::make_shared<algo_optimum::Gradient>(express));
                break;
            case "ann"_bkdrHash:
                optimumFunctor(threadName, std::make_shared<algo_optimum::Annealing>(express));
                break;
            case "par"_bkdrHash:
                optimumFunctor(threadName, std::make_shared<algo_optimum::Particle>(express));
                break;
            case "gen"_bkdrHash:
                optimumFunctor(threadName, std::make_shared<algo_optimum::Genetic>(express));
                break;
            default:
                break;
        }
    }
}

void Command::setOptimumBit(const std::string& method)
{
    using util_hash::operator""_bkdrHash;
    switch (util_hash::bkdrHash(method.c_str()))
    {
        case "fib"_bkdrHash:
            taskPlan.algoTask.optimumBit.set(OptimumMethod::fibonacci);
            break;
        case "gra"_bkdrHash:
            taskPlan.algoTask.optimumBit.set(OptimumMethod::gradient);
            break;
        case "ann"_bkdrHash:
            taskPlan.algoTask.optimumBit.set(OptimumMethod::annealing);
            break;
        case "par"_bkdrHash:
            taskPlan.algoTask.optimumBit.set(OptimumMethod::particle);
            break;
        case "gen"_bkdrHash:
            taskPlan.algoTask.optimumBit.set(OptimumMethod::genetic);
            break;
        default:
            throwUnexpectedMethodException("optimum: " + method);
    }
}

// Integral
void Command::runIntegral() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    if (taskPlan.algoTask.integralBit.none())
    {
        return;
    }

    const auto printFunctor = [](const algo_expression::ExprTarget& expression)
    {
        constexpr std::string_view prefix{"\r\nIntegral Expression: "};
        std::visit(
            algo_expression::ExprOverloaded{
                [&prefix](const algo_expression::Function1& /*unused*/)
                {
                    std::cout << prefix << algo_expression::Function1::integralExpr << std::endl;
                },
                [&prefix](const algo_expression::Function2& /*unused*/)
                {
                    std::cout << prefix << algo_expression::Function2::integralExpr << std::endl;
                },
            },
            expression);
    };
    const auto resultFunctor = [this](
                                   const algo_expression::Expression& expression,
                                   const algo_expression::ExprRange<double, double>& range)
    {
        getIntegralResult(expression, range.range1, range.range2, algo_integral::epsilon);
    };

    COMMAND_PRINT_ALGO_TASK_TITLE(AlgoTaskType::integral, "BEGIN");
    for ([[maybe_unused]] const auto& [range, expression] : integralExprMap)
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
    COMMAND_PRINT_ALGO_TASK_TITLE(AlgoTaskType::integral, "END") << std::endl;
}

void Command::getIntegralResult(
    const algo_expression::Expression& express, const double lowerLimit, const double upperLimit,
    const double epsilon) const
{
    assert(epsilon > 0.0);
    util_thread::Thread threadPool(std::min(
        static_cast<uint32_t>(taskPlan.algoTask.integralBit.count()),
        static_cast<uint32_t>(Bottom<IntegralMethod>::value)));
    const auto integralFunctor =
        [&](const std::string& threadName, const std::shared_ptr<algo_integral::Integral>& classPtr)
    {
        threadPool.enqueue(
            threadName, &algo_integral::Integral::operator(), classPtr, lowerLimit, upperLimit,
            epsilon);
    };

    for (int i = 0; i < Bottom<IntegralMethod>::value; ++i)
    {
        if (!taskPlan.algoTask.integralBit.test(IntegralMethod(i)))
        {
            continue;
        }

        const std::string threadName =
            std::string{1, algoTaskNameTable.at(AlgoTaskType::integral).at(0)} + "_"
            + std::string{algoTaskMethodTable.at(AlgoTaskType::integral).at(i)};
        using util_hash::operator""_bkdrHash;
        switch (util_hash::bkdrHash(algoTaskMethodTable.at(AlgoTaskType::integral).at(i).data()))
        {
            case "tra"_bkdrHash:
                integralFunctor(threadName, std::make_shared<algo_integral::Trapezoidal>(express));
                break;
            case "sim"_bkdrHash:
                integralFunctor(threadName, std::make_shared<algo_integral::Simpson>(express));
                break;
            case "rom"_bkdrHash:
                integralFunctor(threadName, std::make_shared<algo_integral::Romberg>(express));
                break;
            case "gau"_bkdrHash:
                integralFunctor(threadName, std::make_shared<algo_integral::Gauss>(express));
                break;
            case "mon"_bkdrHash:
                integralFunctor(threadName, std::make_shared<algo_integral::MonteCarlo>(express));
                break;
            default:
                break;
        }
    }
}

void Command::setIntegralBit(const std::string& method)
{
    using util_hash::operator""_bkdrHash;
    switch (util_hash::bkdrHash(method.c_str()))
    {
        case "tra"_bkdrHash:
            taskPlan.algoTask.integralBit.set(IntegralMethod::trapezoidal);
            break;
        case "sim"_bkdrHash:
            taskPlan.algoTask.integralBit.set(IntegralMethod::simpson);
            break;
        case "rom"_bkdrHash:
            taskPlan.algoTask.integralBit.set(IntegralMethod::romberg);
            break;
        case "gau"_bkdrHash:
            taskPlan.algoTask.integralBit.set(IntegralMethod::gauss);
            break;
        case "mon"_bkdrHash:
            taskPlan.algoTask.integralBit.set(IntegralMethod::monteCarlo);
            break;
        default:
            throwUnexpectedMethodException("integral: " + method);
    }
}

// Sort
void Command::runSort() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    if (taskPlan.algoTask.sortBit.none())
    {
        return;
    }

    const int leftEndpoint = algo_sort::arrayRange1;
    const int rightEndpoint = algo_sort::arrayRange2;
    const uint32_t length = algo_sort::arrayLength;
    static_assert((leftEndpoint < rightEndpoint) && (length > 0));

    COMMAND_PRINT_ALGO_TASK_TITLE(AlgoTaskType::sort, "BEGIN");
    const std::shared_ptr<algo_sort::Sort<int>> sort =
        std::make_shared<algo_sort::Sort<int>>(length, leftEndpoint, rightEndpoint);
    getSortResult(sort);
    COMMAND_PRINT_ALGO_TASK_TITLE(AlgoTaskType::sort, "END") << std::endl;
}

template <typename T>
void Command::getSortResult(const std::shared_ptr<algo_sort::Sort<T>>& sort) const
{
    util_thread::Thread threadPool(std::min(
        static_cast<uint32_t>(taskPlan.algoTask.sortBit.count()),
        static_cast<uint32_t>(Bottom<SortMethod>::value)));
    const auto sortFunctor = [&](const std::string& threadName,
                                 void (algo_sort::Sort<T>::*methodPtr)(T* const, const uint32_t)
                                     const)
    {
        threadPool.enqueue(
            threadName, methodPtr, sort, sort->getRandomArray().get(), sort->getLength());
    };

    for (int i = 0; i < Bottom<SortMethod>::value; ++i)
    {
        if (!taskPlan.algoTask.sortBit.test(SortMethod(i)))
        {
            continue;
        }

        const std::string threadName =
            std::string{1, algoTaskNameTable.at(AlgoTaskType::sort).at(0)} + "_"
            + std::string{algoTaskMethodTable.at(AlgoTaskType::sort).at(i)};
        using util_hash::operator""_bkdrHash;
        switch (util_hash::bkdrHash(algoTaskMethodTable.at(AlgoTaskType::sort).at(i).data()))
        {
            case "bub"_bkdrHash:
                sortFunctor(threadName, &algo_sort::Sort<T>::bubbleSort);
                break;
            case "sel"_bkdrHash:
                sortFunctor(threadName, &algo_sort::Sort<T>::selectionSort);
                break;
            case "ins"_bkdrHash:
                sortFunctor(threadName, &algo_sort::Sort<T>::insertionSort);
                break;
            case "she"_bkdrHash:
                sortFunctor(threadName, &algo_sort::Sort<T>::shellSort);
                break;
            case "mer"_bkdrHash:
                sortFunctor(threadName, &algo_sort::Sort<T>::mergeSort);
                break;
            case "qui"_bkdrHash:
                sortFunctor(threadName, &algo_sort::Sort<T>::quickSort);
                break;
            case "hea"_bkdrHash:
                sortFunctor(threadName, &algo_sort::Sort<T>::heapSort);
                break;
            case "cou"_bkdrHash:
                sortFunctor(threadName, &algo_sort::Sort<T>::countingSort);
                break;
            case "buc"_bkdrHash:
                sortFunctor(threadName, &algo_sort::Sort<T>::bucketSort);
                break;
            case "rad"_bkdrHash:
                sortFunctor(threadName, &algo_sort::Sort<T>::radixSort);
                break;
            default:
                break;
        }
    }
}

void Command::setSortBit(const std::string& method)
{
    using util_hash::operator""_bkdrHash;
    switch (util_hash::bkdrHash(method.c_str()))
    {
        case "bub"_bkdrHash:
            taskPlan.algoTask.sortBit.set(SortMethod::bubble);
            break;
        case "sel"_bkdrHash:
            taskPlan.algoTask.sortBit.set(SortMethod::selection);
            break;
        case "ins"_bkdrHash:
            taskPlan.algoTask.sortBit.set(SortMethod::insertion);
            break;
        case "she"_bkdrHash:
            taskPlan.algoTask.sortBit.set(SortMethod::shell);
            break;
        case "mer"_bkdrHash:
            taskPlan.algoTask.sortBit.set(SortMethod::merge);
            break;
        case "qui"_bkdrHash:
            taskPlan.algoTask.sortBit.set(SortMethod::quick);
            break;
        case "hea"_bkdrHash:
            taskPlan.algoTask.sortBit.set(SortMethod::heap);
            break;
        case "cou"_bkdrHash:
            taskPlan.algoTask.sortBit.set(SortMethod::counting);
            break;
        case "buc"_bkdrHash:
            taskPlan.algoTask.sortBit.set(SortMethod::bucket);
            break;
        case "rad"_bkdrHash:
            taskPlan.algoTask.sortBit.set(SortMethod::radix);
            break;
        default:
            throwUnexpectedMethodException("sort: " + method);
    }
}

void Command::printHelpMessage() const
{
    std::cout << program.help().str();
}

void Command::printVersionInfo() const
{
    std::string versionStr = "tput rev; echo ";
    versionStr += getIconBanner();
    versionStr.pop_back();
    versionStr += "                    VERSION " + program.version;
    versionStr += " \"; tput sgr0";

    util_file::executeCommand(versionStr.c_str());
}

void Command::printConsoleOutput() const
{
    const auto commands =
        program.get<std::vector<std::string>>(utilTaskNameTable.at(UtilTaskType::console));
    if (commands.empty())
    {
        return;
    }

    util_console::Console console(" > ");
    registerOnConsole(console);
    for (const auto& command : commands)
    {
        console.commandExecutor(command);
    }
}

std::string Command::getIconBanner()
{
    std::string banner;
    banner += R"(")";
    banner += R"(  ______   ______     ______    \n)";
    banner += R"( /\  ___\ /\  __ \   /\  __ \   \n)";
    banner += R"( \ \  __\ \ \ \/\ \  \ \ \/\ \  \n)";
    banner += R"(  \ \_\    \ \_____\  \ \_____\ \n)";
    banner += R"(   \/_/     \/_____/   \/_____/ \n)";
    banner += R"(")";

    return banner;
}

void Command::enterConsole() const
{
    util_file::executeCommand(("tput bel; echo " + getIconBanner()).c_str());

    util_console::Console console("foo > ");
    registerOnConsole(console);
    int returnCode = 0;
    do
    {
        returnCode = console.readCommandLine();
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
    LOG_TO_STOP(logger);

    util_file::printFile(
        std::string{util_log::logPath}.c_str(), true, maxLineNumForPrintLog,
        &util_log::changeLogLevelStyle);
}

void Command::throwUnexpectedMethodException(const std::string& info)
{
    taskPlan.reset();
    throw std::runtime_error("Unexpected method of " + info);
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
