#include "command.hpp"
#include "hash.hpp"
#include "integral.hpp"
#include "log.hpp"
#include "optimum.hpp"
#include "thread.hpp"

#define COMMAND_PRINT_TASK_TITLE(task, taskType, title)                                                            \
    std::cout << std::endl                                                                                         \
              << task << " TASK: " << std::setiosflags(std::ios_base::left) << std::setfill('.')                   \
              << std::setw(titleWidthForPrintTask) << taskType << title << std::resetiosflags(std::ios_base::left) \
              << std::setfill(' ') << std::endl

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
                if (std::any_of(
                        generalTaskMap.at("algorithm").cbegin(),
                        generalTaskMap.at("algorithm").cend(),
                        [&value](const auto& taskCategoryMap)
                        {
                            return (taskCategoryMap.first == value);
                        }))
                {
                    return value;
                }
                throw std::runtime_error("Unknown algorithm category: " + value);
            })
        .help("demo of algorithm, see \"tasks\" for detail of specific category");

    program.addArgument("-n", "--numeric")
        .nArgs(1)
        .action(
            [this](const std::string& value)
            {
                if (std::any_of(
                        generalTaskMap.at("numeric").cbegin(),
                        generalTaskMap.at("numeric").cend(),
                        [&value](const auto& taskCategoryMap)
                        {
                            return (taskCategoryMap.first == value);
                        }))
                {
                    return value;
                }
                throw std::runtime_error("Unknown numeric category: " + value);
            })
        .help("demo of numeric, see \"tasks\" for detail of specific category");

    program.addArgument("tasks").remaining().help("specify task\r\n"
                                                  "├── -a --algorithm\r\n"
                                                  "│   ├── match\r\n"
                                                  "│   │   └── rab, knu, boy, hor, sun\r\n"
                                                  "│   └── sort\r\n"
                                                  "│       ├── bub, sel, ins, she, mer\r\n"
                                                  "│       └── qui, hea, cou, buc, rad\r\n"
                                                  "└── -n --numeric\r\n"
                                                  "    ├── integral\r\n"
                                                  "    │   └── tra, sim, rom, gau, mon\r\n"
                                                  "    └── optimum\r\n"
                                                  "        └── gra, ann, par, gen\r\n");
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

        validateBasicTask();
        validateGeneralTask();
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

void Command::validateBasicTask()
{
    for (int i = 0; i < Bottom<BasicTaskCategory>::value; ++i)
    {
        if (!program.isUsed(std::next(basicTaskMap.cbegin(), BasicTaskCategory(i))->first))
        {
            continue;
        }

        if (checkTask())
        {
            throwExcessArgumentException();
        }

        taskPlan.basicTask.basicTaskBit.set(BasicTaskCategory(i));
    }
}

void Command::validateGeneralTask()
{
    for ([[maybe_unused]] const auto& [taskCategoryName, taskCategoryMap] : generalTaskMap)
    {
        if (!program.isUsed(taskCategoryName))
        {
            continue;
        }

        if (checkTask())
        {
            throwExcessArgumentException();
        }

        for ([[maybe_unused]] const auto& [taskTypeName, taskTypeTuple] : taskCategoryMap)
        {
            if (taskTypeName != program[taskCategoryName])
            {
                continue;
            }

            TaskMethodVector tasks;
            if (program.isUsed("tasks"))
            {
                tasks = program.get<std::vector<std::string>>("tasks");
            }
            else
            {
                const auto taskMethodVector = get<TaskMethodVector>(taskTypeTuple);
                tasks.assign(taskMethodVector.cbegin(), taskMethodVector.cend());
                tasks.erase(
                    std::remove_if(
                        tasks.begin(),
                        tasks.end(),
                        [](const std::string& task) -> bool
                        {
                            return (std::string::npos == task.find_first_not_of(" "));
                        }),
                    std::end(tasks));
            }

            for (const auto& task : tasks)
            {
                (this->*get<SetTaskBitFunctor>(get<TaskFunctorTuple>(taskTypeTuple)))(task);
            }
        }
    }
}

bool Command::checkTask() const
{
    return !taskPlan.empty();
}

void Command::performTask() const
{
    if (!taskPlan.basicTask.empty())
    {
        for (int i = 0; i < Bottom<BasicTaskCategory>::value; ++i)
        {
            if (taskPlan.basicTask.basicTaskBit.test(BasicTaskCategory(i)))
            {
                (this->*std::next(basicTaskMap.cbegin(), BasicTaskCategory(i))->second)();
            }
        }
    }

    if (!taskPlan.generalTask.algoTask.empty())
    {
        for ([[maybe_unused]] const auto& [taskTypeName, taskTypeTuple] :
             std::next(generalTaskMap.cbegin(), GeneralTaskCategory::algorithm)->second)
        {
            (this->*get<PerformTaskFunctor>(get<TaskFunctorTuple>(taskTypeTuple)))();
        }
    }

    if (!taskPlan.generalTask.numTask.empty())
    {
        for ([[maybe_unused]] const auto& [taskTypeName, taskTypeTuple] :
             std::next(generalTaskMap.cbegin(), GeneralTaskCategory::numeric)->second)
        {
            (this->*get<PerformTaskFunctor>(get<TaskFunctorTuple>(taskTypeTuple)))();
        }
    }
}

void Command::printConsoleOutput() const
{
    const auto commands =
        program.get<std::vector<std::string>>(std::next(basicTaskMap.cbegin(), BasicTaskCategory::console)->first);
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
            viewLogContent();
            return util_console::Console::ReturnCode::success;
        },
        "view log");
}

void Command::viewLogContent()
{
    LOG_TO_STOP(logger);

    util_file::printFile(
        std::string{util_log::logPath}.c_str(), true, maxLineNumForPrintLog, &util_log::changeLogLevelStyle);
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

void Command::runMatch() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    if (taskPlan.generalTask.algoTask.matchBit.none())
    {
        return;
    }

    const uint32_t length = algo_match::maxDigit;
    static_assert(length > algo_match::singlePattern.length());

    COMMAND_PRINT_TASK_TITLE("ALGORITHM", AlgorithmTaskType::match, "BEGIN");
    const std::shared_ptr<algo_match::Match> match = std::make_shared<algo_match::Match>(length);
    getMatchResult(match);
    COMMAND_PRINT_TASK_TITLE("ALGORITHM", AlgorithmTaskType::match, "END") << std::endl;
}

void Command::setMatchBit(const std::string& method)
{
    using util_hash::operator""_bkdrHash;
    switch (util_hash::bkdrHash(method.c_str()))
    {
        case "rab"_bkdrHash:
            taskPlan.generalTask.algoTask.matchBit.set(MatchMethod::rabinKarp);
            break;
        case "knu"_bkdrHash:
            taskPlan.generalTask.algoTask.matchBit.set(MatchMethod::knuthMorrisPratt);
            break;
        case "boy"_bkdrHash:
            taskPlan.generalTask.algoTask.matchBit.set(MatchMethod::boyerMoore);
            break;
        case "hor"_bkdrHash:
            taskPlan.generalTask.algoTask.matchBit.set(MatchMethod::horspool);
            break;
        case "sun"_bkdrHash:
            taskPlan.generalTask.algoTask.matchBit.set(MatchMethod::sunday);
            break;
        default:
            throwUnexpectedMethodException("match: " + method);
    }
}

void Command::getMatchResult(const std::shared_ptr<algo_match::Match>& match) const
{
    util_thread::Thread threadPool(std::min(
        static_cast<uint32_t>(taskPlan.generalTask.algoTask.matchBit.count()),
        static_cast<uint32_t>(Bottom<MatchMethod>::value)));
    const auto matchFunctor =
        [&](const std::string& threadName, int (*methodPtr)(const char*, const char*, const uint32_t, const uint32_t))
    {
        threadPool.enqueue(
            threadName,
            methodPtr,
            match->getSearchingText().get(),
            algo_match::singlePattern.data(),
            match->getLength(),
            algo_match::singlePattern.length());
    };

    for (int i = 0; i < Bottom<MatchMethod>::value; ++i)
    {
        if (!taskPlan.generalTask.algoTask.matchBit.test(MatchMethod(i)))
        {
            continue;
        }

        const auto taskCategoryMap = std::next(
            std::next(generalTaskMap.cbegin(), GeneralTaskCategory::algorithm)->second.cbegin(),
            AlgorithmTaskType::match);
        const auto targetMethod = get<TaskMethodVector>(taskCategoryMap->second).at(i);
        const std::string threadName = std::string{1, taskCategoryMap->first.at(0)} + "_" + targetMethod;
        using util_hash::operator""_bkdrHash;
        switch (util_hash::bkdrHash(targetMethod.data()))
        {
            case "rab"_bkdrHash:
                matchFunctor(threadName, &algo_match::Match::rkMethod);
                break;
            case "knu"_bkdrHash:
                matchFunctor(threadName, &algo_match::Match::kmpMethod);
                break;
            case "boy"_bkdrHash:
                matchFunctor(threadName, &algo_match::Match::bmMethod);
                break;
            case "hor"_bkdrHash:
                matchFunctor(threadName, &algo_match::Match::horspoolMethod);
                break;
            case "sun"_bkdrHash:
                matchFunctor(threadName, &algo_match::Match::sundayMethod);
                break;
            default:
                LOG_DBG(logger, "execute to run unknown match method.");
                break;
        }
    }
}

void Command::runSort() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    if (taskPlan.generalTask.algoTask.sortBit.none())
    {
        return;
    }

    const int leftEndpoint = algo_sort::arrayRange1;
    const int rightEndpoint = algo_sort::arrayRange2;
    const uint32_t length = algo_sort::arrayLength;
    static_assert((leftEndpoint < rightEndpoint) && (length > 0));

    COMMAND_PRINT_TASK_TITLE("ALGORITHM", AlgorithmTaskType::sort, "BEGIN");
    const std::shared_ptr<algo_sort::Sort<int>> sort =
        std::make_shared<algo_sort::Sort<int>>(length, leftEndpoint, rightEndpoint);
    getSortResult(sort);
    COMMAND_PRINT_TASK_TITLE("ALGORITHM", AlgorithmTaskType::sort, "END") << std::endl;
}

void Command::setSortBit(const std::string& method)
{
    using util_hash::operator""_bkdrHash;
    switch (util_hash::bkdrHash(method.c_str()))
    {
        case "bub"_bkdrHash:
            taskPlan.generalTask.algoTask.sortBit.set(SortMethod::bubble);
            break;
        case "sel"_bkdrHash:
            taskPlan.generalTask.algoTask.sortBit.set(SortMethod::selection);
            break;
        case "ins"_bkdrHash:
            taskPlan.generalTask.algoTask.sortBit.set(SortMethod::insertion);
            break;
        case "she"_bkdrHash:
            taskPlan.generalTask.algoTask.sortBit.set(SortMethod::shell);
            break;
        case "mer"_bkdrHash:
            taskPlan.generalTask.algoTask.sortBit.set(SortMethod::merge);
            break;
        case "qui"_bkdrHash:
            taskPlan.generalTask.algoTask.sortBit.set(SortMethod::quick);
            break;
        case "hea"_bkdrHash:
            taskPlan.generalTask.algoTask.sortBit.set(SortMethod::heap);
            break;
        case "cou"_bkdrHash:
            taskPlan.generalTask.algoTask.sortBit.set(SortMethod::counting);
            break;
        case "buc"_bkdrHash:
            taskPlan.generalTask.algoTask.sortBit.set(SortMethod::bucket);
            break;
        case "rad"_bkdrHash:
            taskPlan.generalTask.algoTask.sortBit.set(SortMethod::radix);
            break;
        default:
            throwUnexpectedMethodException("sort: " + method);
    }
}

template <typename T>
void Command::getSortResult(const std::shared_ptr<algo_sort::Sort<T>>& sort) const
{
    util_thread::Thread threadPool(std::min(
        static_cast<uint32_t>(taskPlan.generalTask.algoTask.sortBit.count()),
        static_cast<uint32_t>(Bottom<SortMethod>::value)));
    const auto sortFunctor =
        [&](const std::string& threadName, void (algo_sort::Sort<T>::*methodPtr)(T* const, const uint32_t) const)
    {
        threadPool.enqueue(threadName, methodPtr, sort, sort->getRandomArray().get(), sort->getLength());
    };

    for (int i = 0; i < Bottom<SortMethod>::value; ++i)
    {
        if (!taskPlan.generalTask.algoTask.sortBit.test(SortMethod(i)))
        {
            continue;
        }

        const auto taskCategoryMap = std::next(
            std::next(generalTaskMap.cbegin(), GeneralTaskCategory::algorithm)->second.cbegin(),
            AlgorithmTaskType::sort);
        const auto targetMethod = get<TaskMethodVector>(taskCategoryMap->second).at(i);
        const std::string threadName = std::string{1, taskCategoryMap->first.at(0)} + "_" + targetMethod;
        using util_hash::operator""_bkdrHash;
        switch (util_hash::bkdrHash(targetMethod.data()))
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
                LOG_DBG(logger, "execute to run unknown sort method.");
                break;
        }
    }
}

void Command::runIntegral() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    if (taskPlan.generalTask.numTask.integralBit.none())
    {
        return;
    }

    const auto printFunctor = [](const IntegralExprTarget& expression)
    {
        constexpr std::string_view prefix{"\r\nIntegral Expression: "};
        std::visit(
            num_expression::ExprOverloaded{
                [&prefix](const num_expression::Function1& /*unused*/)
                {
                    std::cout << prefix << num_expression::Function1::integralExpr << std::endl;
                },
                [&prefix](const num_expression::Function2& /*unused*/)
                {
                    std::cout << prefix << num_expression::Function2::integralExpr << std::endl;
                },
            },
            expression);
    };
    const auto resultFunctor =
        [this](const num_expression::Expression& expression, const num_expression::ExprRange<double, double>& range)
    {
        getIntegralResult(expression, range.range1, range.range2, num_integral::epsilon);
    };

    COMMAND_PRINT_TASK_TITLE("NUMERIC", NumericTaskType::integral, "BEGIN");
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
    COMMAND_PRINT_TASK_TITLE("NUMERIC", NumericTaskType::integral, "END") << std::endl;
}

void Command::setIntegralBit(const std::string& method)
{
    using util_hash::operator""_bkdrHash;
    switch (util_hash::bkdrHash(method.c_str()))
    {
        case "tra"_bkdrHash:
            taskPlan.generalTask.numTask.integralBit.set(IntegralMethod::trapezoidal);
            break;
        case "sim"_bkdrHash:
            taskPlan.generalTask.numTask.integralBit.set(IntegralMethod::simpson);
            break;
        case "rom"_bkdrHash:
            taskPlan.generalTask.numTask.integralBit.set(IntegralMethod::romberg);
            break;
        case "gau"_bkdrHash:
            taskPlan.generalTask.numTask.integralBit.set(IntegralMethod::gauss);
            break;
        case "mon"_bkdrHash:
            taskPlan.generalTask.numTask.integralBit.set(IntegralMethod::monteCarlo);
            break;
        default:
            throwUnexpectedMethodException("integral: " + method);
    }
}

void Command::getIntegralResult(
    const num_expression::Expression& express,
    const double lowerLimit,
    const double upperLimit,
    const double epsilon) const
{
    assert(epsilon > 0.0);
    util_thread::Thread threadPool(std::min(
        static_cast<uint32_t>(taskPlan.generalTask.numTask.integralBit.count()),
        static_cast<uint32_t>(Bottom<IntegralMethod>::value)));
    const auto integralFunctor =
        [&](const std::string& threadName, const std::shared_ptr<num_integral::Integral>& classPtr)
    {
        threadPool.enqueue(threadName, &num_integral::Integral::operator(), classPtr, lowerLimit, upperLimit, epsilon);
    };

    for (int i = 0; i < Bottom<IntegralMethod>::value; ++i)
    {
        if (!taskPlan.generalTask.numTask.integralBit.test(IntegralMethod(i)))
        {
            continue;
        }

        const auto taskCategoryMap = std::next(
            std::next(generalTaskMap.cbegin(), GeneralTaskCategory::numeric)->second.cbegin(),
            NumericTaskType::integral);
        const auto targetMethod = get<TaskMethodVector>(taskCategoryMap->second).at(i);
        const std::string threadName = std::string{1, taskCategoryMap->first.at(0)} + "_" + targetMethod;
        using util_hash::operator""_bkdrHash;
        switch (util_hash::bkdrHash(targetMethod.data()))
        {
            case "tra"_bkdrHash:
                integralFunctor(threadName, std::make_shared<num_integral::Trapezoidal>(express));
                break;
            case "sim"_bkdrHash:
                integralFunctor(threadName, std::make_shared<num_integral::Simpson>(express));
                break;
            case "rom"_bkdrHash:
                integralFunctor(threadName, std::make_shared<num_integral::Romberg>(express));
                break;
            case "gau"_bkdrHash:
                integralFunctor(threadName, std::make_shared<num_integral::Gauss>(express));
                break;
            case "mon"_bkdrHash:
                integralFunctor(threadName, std::make_shared<num_integral::MonteCarlo>(express));
                break;
            default:
                LOG_DBG(logger, "execute to run unknown integral method.");
                break;
        }
    }
}

void Command::runOptimum() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    if (taskPlan.generalTask.numTask.optimumBit.none())
    {
        return;
    }

    const auto printFunctor = [](const OptimumExprTarget& expression)
    {
        constexpr std::string_view prefix{"\r\nOptimum Expression: "};
        std::visit(
            num_expression::ExprOverloaded{
                [&prefix](const num_expression::Griewank& /*unused*/)
                {
                    std::cout << prefix << num_expression::Griewank::optimumExpr << std::endl;
                },
                [&prefix](const num_expression::Rastrigin& /*unused*/)
                {
                    std::cout << prefix << num_expression::Rastrigin::optimumExpr << std::endl;
                },
            },
            expression);
    };
    const auto resultFunctor =
        [this](const num_expression::Expression& expression, const num_expression::ExprRange<double, double>& range)
    {
        getOptimumResult(expression, range.range1, range.range2, num_optimum::epsilon);
    };

    COMMAND_PRINT_TASK_TITLE("NUMERIC", NumericTaskType::optimum, "BEGIN");
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
    COMMAND_PRINT_TASK_TITLE("NUMERIC", NumericTaskType::optimum, "END") << std::endl;
}

void Command::setOptimumBit(const std::string& method)
{
    using util_hash::operator""_bkdrHash;
    switch (util_hash::bkdrHash(method.c_str()))
    {
        case "gra"_bkdrHash:
            taskPlan.generalTask.numTask.optimumBit.set(OptimumMethod::gradient);
            break;
        case "ann"_bkdrHash:
            taskPlan.generalTask.numTask.optimumBit.set(OptimumMethod::annealing);
            break;
        case "par"_bkdrHash:
            taskPlan.generalTask.numTask.optimumBit.set(OptimumMethod::particle);
            break;
        case "gen"_bkdrHash:
            taskPlan.generalTask.numTask.optimumBit.set(OptimumMethod::genetic);
            break;
        default:
            throwUnexpectedMethodException("optimum: " + method);
    }
}

void Command::getOptimumResult(
    const num_expression::Expression& express,
    const double leftEndpoint,
    const double rightEndpoint,
    const double epsilon) const
{
    assert((leftEndpoint < rightEndpoint) && (epsilon > 0.0));
    util_thread::Thread threadPool(std::min(
        static_cast<uint32_t>(taskPlan.generalTask.numTask.optimumBit.count()),
        static_cast<uint32_t>(Bottom<OptimumMethod>::value)));
    const auto optimumFunctor =
        [&](const std::string& threadName, const std::shared_ptr<num_optimum::Optimum>& classPtr)
    {
        threadPool.enqueue(
            threadName, &num_optimum::Optimum::operator(), classPtr, leftEndpoint, rightEndpoint, epsilon);
    };

    for (int i = 0; i < Bottom<OptimumMethod>::value; ++i)
    {
        if (!taskPlan.generalTask.numTask.optimumBit.test(OptimumMethod(i)))
        {
            continue;
        }

        const auto taskCategoryMap = std::next(
            std::next(generalTaskMap.cbegin(), GeneralTaskCategory::numeric)->second.cbegin(),
            NumericTaskType::optimum);
        const auto targetMethod = get<TaskMethodVector>(taskCategoryMap->second).at(i);
        const std::string threadName = std::string{1, taskCategoryMap->first.at(0)} + "_" + targetMethod;
        using util_hash::operator""_bkdrHash;
        switch (util_hash::bkdrHash(targetMethod.data()))
        {
            case "gra"_bkdrHash:
                optimumFunctor(threadName, std::make_shared<num_optimum::Gradient>(express));
                break;
            case "ann"_bkdrHash:
                optimumFunctor(threadName, std::make_shared<num_optimum::Annealing>(express));
                break;
            case "par"_bkdrHash:
                optimumFunctor(threadName, std::make_shared<num_optimum::Particle>(express));
                break;
            case "gen"_bkdrHash:
                optimumFunctor(threadName, std::make_shared<num_optimum::Genetic>(express));
                break;
            default:
                LOG_DBG(logger, "Unable to execute unknown optimum method.");
                break;
        }
    }
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

std::ostream& operator<<(std::ostream& os, const Command::AlgorithmTaskType& taskType)
{
    switch (taskType)
    {
        case Command::AlgorithmTaskType::match:
            os << "MATCH";
            break;
        case Command::AlgorithmTaskType::sort:
            os << "SORT";
            break;
        default:
            os << "UNKNOWN: " << static_cast<std::underlying_type_t<Command::AlgorithmTaskType>>(taskType);
    }

    return os;
}

std::ostream& operator<<(std::ostream& os, const Command::NumericTaskType& taskType)
{
    switch (taskType)
    {
        case Command::NumericTaskType::integral:
            os << "INTEGRAL";
            break;
        case Command::NumericTaskType::optimum:
            os << "OPTIMUM";
            break;
        default:
            os << "UNKNOWN: " << static_cast<std::underlying_type_t<Command::NumericTaskType>>(taskType);
    }

    return os;
}
