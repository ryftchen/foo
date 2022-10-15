#include "command.hpp"
#include "algorithm/include/match.hpp"
#include "algorithm/include/notation.hpp"
#include "algorithm/include/search.hpp"
#include "algorithm/include/sort.hpp"
#include "numeric/include/arithmetic.hpp"
#include "numeric/include/divisor.hpp"
#include "numeric/include/integral.hpp"
#include "numeric/include/optimum.hpp"
#include "numeric/include/sieve.hpp"
#include "utility/include/hash.hpp"
#include "utility/include/log.hpp"
#include "utility/include/thread.hpp"

#define COMMAND_PRINT_TASK_TITLE(taskCategory, taskType, title)                                                    \
    std::cout << "\r\n"                                                                                            \
              << taskCategory << " TASK: " << std::setiosflags(std::ios_base::left) << std::setfill('.')           \
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

    program.addArgument("tasks").remaining().help("specify task\r\n" + std::string{optionTreeOfHelpMsg});
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
                (this->*get<UpdateTaskFunctor>(get<TaskFunctorTuple>(taskTypeTuple)))(task);
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

    for (int i = 0; i < Bottom<GeneralTaskCategory>::value; ++i)
    {
        switch (GeneralTaskCategory(i))
        {
            case GeneralTaskCategory::algorithm:
                if (taskPlan.generalTask.algoTask.empty())
                {
                    continue;
                }
                break;
            case GeneralTaskCategory::numeric:
                if (taskPlan.generalTask.numTask.empty())
                {
                    continue;
                }
                break;
            default:
                break;
        }

        for ([[maybe_unused]] const auto& [taskTypeName, taskTypeTuple] :
             std::next(generalTaskMap.cbegin(), GeneralTaskCategory(i))->second)
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

    static_assert(algo_match::maxDigit > algo_match::singlePattern.length());
    COMMAND_PRINT_TASK_TITLE("ALGORITHM", AlgorithmTaskType::match, "BEGIN");
    [&]
    {
        const std::shared_ptr<algo_match::Match> match = std::make_shared<algo_match::Match>(algo_match::maxDigit);
        util_thread::Thread threadPool(std::min(
            static_cast<uint32_t>(taskPlan.generalTask.algoTask.matchBit.count()),
            static_cast<uint32_t>(Bottom<MatchMethod>::value)));
        const auto matchFunctor = [&](const std::string& threadName,
                                      int (*methodPtr)(const char*, const char*, const uint32_t, const uint32_t))
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
    }();
    COMMAND_PRINT_TASK_TITLE("ALGORITHM", AlgorithmTaskType::match, "END") << std::endl;
}

void Command::updateMatchTask(const std::string& method)
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

void Command::runNotation() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    if (taskPlan.generalTask.algoTask.notationBit.none())
    {
        return;
    }

    COMMAND_PRINT_TASK_TITLE("ALGORITHM", AlgorithmTaskType::notation, "BEGIN");
    [&]
    {
        const std::shared_ptr<algo_notation::Notation> notation = std::make_shared<algo_notation::Notation>();
        util_thread::Thread threadPool(std::min(
            static_cast<uint32_t>(taskPlan.generalTask.algoTask.notationBit.count()),
            static_cast<uint32_t>(Bottom<NotationMethod>::value)));
        const auto notationFunctor = [&](const std::string& threadName, std::string (*methodPtr)(const std::string&))
        {
            threadPool.enqueue(threadName, methodPtr, std::string{algo_notation::infixNotation});
        };

        for (int i = 0; i < Bottom<NotationMethod>::value; ++i)
        {
            if (!taskPlan.generalTask.algoTask.notationBit.test(NotationMethod(i)))
            {
                continue;
            }

            const auto taskCategoryMap = std::next(
                std::next(generalTaskMap.cbegin(), GeneralTaskCategory::algorithm)->second.cbegin(),
                AlgorithmTaskType::notation);
            const auto targetMethod = get<TaskMethodVector>(taskCategoryMap->second).at(i);
            const std::string threadName = std::string{1, taskCategoryMap->first.at(0)} + "_" + targetMethod;
            using util_hash::operator""_bkdrHash;
            switch (util_hash::bkdrHash(targetMethod.data()))
            {
                case "pre"_bkdrHash:
                    notationFunctor(threadName, &algo_notation::Notation::prefixMethod);
                    break;
                case "pos"_bkdrHash:
                    notationFunctor(threadName, &algo_notation::Notation::postfixMethod);
                    break;
                default:
                    LOG_DBG(logger, "execute to run unknown notation method.");
                    break;
            }
        }
    }();
    COMMAND_PRINT_TASK_TITLE("ALGORITHM", AlgorithmTaskType::notation, "END") << std::endl;
}

void Command::updateNotationTask(const std::string& method)
{
    using util_hash::operator""_bkdrHash;
    switch (util_hash::bkdrHash(method.c_str()))
    {
        case "pre"_bkdrHash:
            taskPlan.generalTask.algoTask.notationBit.set(NotationMethod::prefix);
            break;
        case "pos"_bkdrHash:
            taskPlan.generalTask.algoTask.notationBit.set(NotationMethod::postfix);
            break;
        default:
            throwUnexpectedMethodException("notation: " + method);
    }
}

void Command::runSearch() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    if (taskPlan.generalTask.algoTask.searchBit.none())
    {
        return;
    }

    static_assert((algo_search::arrayRange1 < algo_search::arrayRange2) && (algo_search::arrayLength > 0));
    COMMAND_PRINT_TASK_TITLE("ALGORITHM", AlgorithmTaskType::search, "BEGIN");
    [&]
    {
        const std::shared_ptr<algo_search::Search<double>> search = std::make_shared<algo_search::Search<double>>(
            algo_search::arrayLength, algo_search::arrayRange1, algo_search::arrayRange2);
        util_thread::Thread threadPool(std::min(
            static_cast<uint32_t>(taskPlan.generalTask.algoTask.searchBit.count()),
            static_cast<uint32_t>(Bottom<SearchMethod>::value)));
        const auto searchFunctor =
            [&](const std::string& threadName,
                int (algo_search::Search<double>::*methodPtr)(const double* const, const uint32_t, const double) const)
        {
            threadPool.enqueue(
                threadName,
                methodPtr,
                search,
                search->getOrderedArray().get(),
                search->getLength(),
                search->getSearchedKey());
        };

        for (int i = 0; i < Bottom<SearchMethod>::value; ++i)
        {
            if (!taskPlan.generalTask.algoTask.searchBit.test(SearchMethod(i)))
            {
                continue;
            }

            const auto taskCategoryMap = std::next(
                std::next(generalTaskMap.cbegin(), GeneralTaskCategory::algorithm)->second.cbegin(),
                AlgorithmTaskType::search);
            const auto targetMethod = get<TaskMethodVector>(taskCategoryMap->second).at(i);
            const std::string threadName = std::string{1, taskCategoryMap->first.at(0)} + "_" + targetMethod;
            using util_hash::operator""_bkdrHash;
            switch (util_hash::bkdrHash(targetMethod.data()))
            {
                case "bin"_bkdrHash:
                    searchFunctor(threadName, &algo_search::Search<double>::binarySearch);
                    break;
                case "int"_bkdrHash:
                    searchFunctor(threadName, &algo_search::Search<double>::interpolationSearch);
                    break;
                case "fib"_bkdrHash:
                    searchFunctor(threadName, &algo_search::Search<double>::fibonacciSearch);
                    break;
                default:
                    LOG_DBG(logger, "execute to run unknown search method.");
                    break;
            }
        }
    }();
    COMMAND_PRINT_TASK_TITLE("ALGORITHM", AlgorithmTaskType::search, "END") << std::endl;
}

void Command::updateSearchTask(const std::string& method)
{
    using util_hash::operator""_bkdrHash;
    switch (util_hash::bkdrHash(method.c_str()))
    {
        case "bin"_bkdrHash:
            taskPlan.generalTask.algoTask.searchBit.set(SearchMethod::binary);
            break;
        case "int"_bkdrHash:
            taskPlan.generalTask.algoTask.searchBit.set(SearchMethod::interpolation);
            break;
        case "fib"_bkdrHash:
            taskPlan.generalTask.algoTask.searchBit.set(SearchMethod::fibonacci);
            break;
        default:
            throwUnexpectedMethodException("search: " + method);
    }
}

void Command::runSort() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    if (taskPlan.generalTask.algoTask.sortBit.none())
    {
        return;
    }

    static_assert((algo_sort::arrayRange1 < algo_sort::arrayRange2) && (algo_sort::arrayLength > 0));
    COMMAND_PRINT_TASK_TITLE("ALGORITHM", AlgorithmTaskType::sort, "BEGIN");
    [&]
    {
        const std::shared_ptr<algo_sort::Sort<int>> sort = std::make_shared<algo_sort::Sort<int>>(
            algo_sort::arrayLength, algo_sort::arrayRange1, algo_sort::arrayRange2);
        util_thread::Thread threadPool(std::min(
            static_cast<uint32_t>(taskPlan.generalTask.algoTask.sortBit.count()),
            static_cast<uint32_t>(Bottom<SortMethod>::value)));
        const auto sortFunctor = [&](const std::string& threadName,
                                     void (algo_sort::Sort<int>::*methodPtr)(int* const, const uint32_t) const)
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
                    sortFunctor(threadName, &algo_sort::Sort<int>::bubbleSort);
                    break;
                case "sel"_bkdrHash:
                    sortFunctor(threadName, &algo_sort::Sort<int>::selectionSort);
                    break;
                case "ins"_bkdrHash:
                    sortFunctor(threadName, &algo_sort::Sort<int>::insertionSort);
                    break;
                case "she"_bkdrHash:
                    sortFunctor(threadName, &algo_sort::Sort<int>::shellSort);
                    break;
                case "mer"_bkdrHash:
                    sortFunctor(threadName, &algo_sort::Sort<int>::mergeSort);
                    break;
                case "qui"_bkdrHash:
                    sortFunctor(threadName, &algo_sort::Sort<int>::quickSort);
                    break;
                case "hea"_bkdrHash:
                    sortFunctor(threadName, &algo_sort::Sort<int>::heapSort);
                    break;
                case "cou"_bkdrHash:
                    sortFunctor(threadName, &algo_sort::Sort<int>::countingSort);
                    break;
                case "buc"_bkdrHash:
                    sortFunctor(threadName, &algo_sort::Sort<int>::bucketSort);
                    break;
                case "rad"_bkdrHash:
                    sortFunctor(threadName, &algo_sort::Sort<int>::radixSort);
                    break;
                default:
                    LOG_DBG(logger, "execute to run unknown sort method.");
                    break;
            }
        }
    }();
    COMMAND_PRINT_TASK_TITLE("ALGORITHM", AlgorithmTaskType::sort, "END") << std::endl;
}

void Command::updateSortTask(const std::string& method)
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

void Command::runArithmetic() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    if (taskPlan.generalTask.numTask.arithmeticBit.none())
    {
        return;
    }

    COMMAND_PRINT_TASK_TITLE("NUMERIC", NumericTaskType::arithmetic, "BEGIN");
    [&]
    {
        const std::shared_ptr<num_arithmetic::Arithmetic> arithmetic = std::make_shared<num_arithmetic::Arithmetic>();
        util_thread::Thread threadPool(std::min(
            static_cast<uint32_t>(taskPlan.generalTask.numTask.arithmeticBit.count()),
            static_cast<uint32_t>(Bottom<ArithmeticMethod>::value)));
        const auto arithmeticFunctor = [&](const std::string& threadName, int (*methodPtr)(const int, const int))
        {
            threadPool.enqueue(threadName, methodPtr, num_arithmetic::integer1, num_arithmetic::integer2);
        };

        for (int i = 0; i < Bottom<ArithmeticMethod>::value; ++i)
        {
            if (!taskPlan.generalTask.numTask.arithmeticBit.test(ArithmeticMethod(i)))
            {
                continue;
            }

            const auto taskCategoryMap = std::next(
                std::next(generalTaskMap.cbegin(), GeneralTaskCategory::numeric)->second.cbegin(),
                NumericTaskType::arithmetic);
            const auto targetMethod = get<TaskMethodVector>(taskCategoryMap->second).at(i);
            const std::string threadName = std::string{1, taskCategoryMap->first.at(0)} + "_" + targetMethod;
            using util_hash::operator""_bkdrHash;
            switch (util_hash::bkdrHash(targetMethod.data()))
            {
                case "add"_bkdrHash:
                    arithmeticFunctor(threadName, &num_arithmetic::Arithmetic::additionMethod);
                    break;
                case "sub"_bkdrHash:
                    arithmeticFunctor(threadName, &num_arithmetic::Arithmetic::subtractionMethod);
                    break;
                case "mul"_bkdrHash:
                    arithmeticFunctor(threadName, &num_arithmetic::Arithmetic::multiplicationMethod);
                    break;
                case "div"_bkdrHash:
                    arithmeticFunctor(threadName, &num_arithmetic::Arithmetic::divisionMethod);
                    break;
                default:
                    LOG_DBG(logger, "execute to run unknown arithmetic method.");
                    break;
            }
        }
    }();
    COMMAND_PRINT_TASK_TITLE("NUMERIC", NumericTaskType::arithmetic, "END") << std::endl;
}

void Command::updateArithmeticTask(const std::string& method)
{
    using util_hash::operator""_bkdrHash;
    switch (util_hash::bkdrHash(method.c_str()))
    {
        case "add"_bkdrHash:
            taskPlan.generalTask.numTask.arithmeticBit.set(ArithmeticMethod::addition);
            break;
        case "sub"_bkdrHash:
            taskPlan.generalTask.numTask.arithmeticBit.set(ArithmeticMethod::subtraction);
            break;
        case "mul"_bkdrHash:
            taskPlan.generalTask.numTask.arithmeticBit.set(ArithmeticMethod::multiplication);
            break;
        case "div"_bkdrHash:
            taskPlan.generalTask.numTask.arithmeticBit.set(ArithmeticMethod::division);
            break;
        default:
            throwUnexpectedMethodException("arithmetic: " + method);
    }
}

void Command::runDivisor() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    if (taskPlan.generalTask.numTask.divisorBit.none())
    {
        return;
    }

    COMMAND_PRINT_TASK_TITLE("NUMERIC", NumericTaskType::divisor, "BEGIN");
    [&]
    {
        const std::shared_ptr<num_divisor::Divisor> divisor = std::make_shared<num_divisor::Divisor>();
        util_thread::Thread threadPool(std::min(
            static_cast<uint32_t>(taskPlan.generalTask.numTask.divisorBit.count()),
            static_cast<uint32_t>(Bottom<DivisorMethod>::value)));
        const auto divisorFunctor =
            [&](const std::string& threadName, std::vector<int> (num_divisor::Divisor::*methodPtr)(int, int) const)
        {
            threadPool.enqueue(threadName, methodPtr, divisor, num_divisor::integer1, num_divisor::integer2);
        };

        for (int i = 0; i < Bottom<DivisorMethod>::value; ++i)
        {
            if (!taskPlan.generalTask.numTask.divisorBit.test(DivisorMethod(i)))
            {
                continue;
            }

            const auto taskCategoryMap = std::next(
                std::next(generalTaskMap.cbegin(), GeneralTaskCategory::numeric)->second.cbegin(),
                NumericTaskType::divisor);
            const auto targetMethod = get<TaskMethodVector>(taskCategoryMap->second).at(i);
            const std::string threadName = std::string{1, taskCategoryMap->first.at(0)} + "_" + targetMethod;
            using util_hash::operator""_bkdrHash;
            switch (util_hash::bkdrHash(targetMethod.data()))
            {
                case "euc"_bkdrHash:
                    divisorFunctor(threadName, &num_divisor::Divisor::euclidMethod);
                    break;
                case "ste"_bkdrHash:
                    divisorFunctor(threadName, &num_divisor::Divisor::steinMethod);
                    break;
                default:
                    LOG_DBG(logger, "execute to run unknown divisor method.");
                    break;
            }
        }
    }();
    COMMAND_PRINT_TASK_TITLE("NUMERIC", NumericTaskType::divisor, "END") << std::endl;
}

void Command::updateDivisorTask(const std::string& method)
{
    using util_hash::operator""_bkdrHash;
    switch (util_hash::bkdrHash(method.c_str()))
    {
        case "euc"_bkdrHash:
            taskPlan.generalTask.numTask.divisorBit.set(DivisorMethod::euclid);
            break;
        case "ste"_bkdrHash:
            taskPlan.generalTask.numTask.divisorBit.set(DivisorMethod::stein);
            break;
        default:
            throwUnexpectedMethodException("divisor: " + method);
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
        constexpr std::string_view prefix{"\r\nIntegral expression: "};
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
        static_assert(num_integral::epsilon > 0.0);
        util_thread::Thread threadPool(std::min(
            static_cast<uint32_t>(taskPlan.generalTask.numTask.integralBit.count()),
            static_cast<uint32_t>(Bottom<IntegralMethod>::value)));
        const auto integralFunctor =
            [&](const std::string& threadName, const std::shared_ptr<num_integral::Integral>& classPtr)
        {
            threadPool.enqueue(
                threadName,
                &num_integral::Integral::operator(),
                classPtr,
                range.range1,
                range.range2,
                num_integral::epsilon);
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
                    integralFunctor(threadName, std::make_shared<num_integral::Trapezoidal>(expression));
                    break;
                case "sim"_bkdrHash:
                    integralFunctor(threadName, std::make_shared<num_integral::Simpson>(expression));
                    break;
                case "rom"_bkdrHash:
                    integralFunctor(threadName, std::make_shared<num_integral::Romberg>(expression));
                    break;
                case "gau"_bkdrHash:
                    integralFunctor(threadName, std::make_shared<num_integral::Gauss>(expression));
                    break;
                case "mon"_bkdrHash:
                    integralFunctor(threadName, std::make_shared<num_integral::MonteCarlo>(expression));
                    break;
                default:
                    LOG_DBG(logger, "execute to run unknown integral method.");
                    break;
            }
        }
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

void Command::updateIntegralTask(const std::string& method)
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

void Command::runOptimum() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    if (taskPlan.generalTask.numTask.optimumBit.none())
    {
        return;
    }

    const auto printFunctor = [](const OptimumExprTarget& expression)
    {
        constexpr std::string_view prefix{"\r\nOptimum expression: "};
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
        assert((range.range1 < range.range2) && (num_optimum::epsilon > 0.0));
        util_thread::Thread threadPool(std::min(
            static_cast<uint32_t>(taskPlan.generalTask.numTask.optimumBit.count()),
            static_cast<uint32_t>(Bottom<OptimumMethod>::value)));
        const auto optimumFunctor =
            [&](const std::string& threadName, const std::shared_ptr<num_optimum::Optimum>& classPtr)
        {
            threadPool.enqueue(
                threadName,
                &num_optimum::Optimum::operator(),
                classPtr,
                range.range1,
                range.range2,
                num_optimum::epsilon);
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
                    optimumFunctor(threadName, std::make_shared<num_optimum::Gradient>(expression));
                    break;
                case "ann"_bkdrHash:
                    optimumFunctor(threadName, std::make_shared<num_optimum::Annealing>(expression));
                    break;
                case "par"_bkdrHash:
                    optimumFunctor(threadName, std::make_shared<num_optimum::Particle>(expression));
                    break;
                case "gen"_bkdrHash:
                    optimumFunctor(threadName, std::make_shared<num_optimum::Genetic>(expression));
                    break;
                default:
                    LOG_DBG(logger, "Unable to execute unknown optimum method.");
                    break;
            }
        }
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

void Command::updateOptimumTask(const std::string& method)
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

void Command::runSieve() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    if (taskPlan.generalTask.numTask.sieveBit.none())
    {
        return;
    }

    COMMAND_PRINT_TASK_TITLE("NUMERIC", NumericTaskType::sieve, "BEGIN");
    [&]
    {
        const std::shared_ptr<num_sieve::Sieve> sieve = std::make_shared<num_sieve::Sieve>();
        util_thread::Thread threadPool(std::min(
            static_cast<uint32_t>(taskPlan.generalTask.numTask.sieveBit.count()),
            static_cast<uint32_t>(Bottom<SieveMethod>::value)));
        const auto sieveFunctor = [&](const std::string& threadName,
                                      std::vector<uint32_t> (num_sieve::Sieve::*methodPtr)(const uint32_t) const)
        {
            threadPool.enqueue(threadName, methodPtr, sieve, num_sieve::maxPositiveInteger);
        };

        for (int i = 0; i < Bottom<SieveMethod>::value; ++i)
        {
            if (!taskPlan.generalTask.numTask.sieveBit.test(SieveMethod(i)))
            {
                continue;
            }

            const auto taskCategoryMap = std::next(
                std::next(generalTaskMap.cbegin(), GeneralTaskCategory::numeric)->second.cbegin(),
                NumericTaskType::sieve);
            const auto targetMethod = get<TaskMethodVector>(taskCategoryMap->second).at(i);
            const std::string threadName = std::string{1, taskCategoryMap->first.at(0)} + "_" + targetMethod;
            using util_hash::operator""_bkdrHash;
            switch (util_hash::bkdrHash(targetMethod.data()))
            {
                case "era"_bkdrHash:
                    sieveFunctor(threadName, &num_sieve::Sieve::eratosthenesMethod);
                    break;
                case "eul"_bkdrHash:
                    sieveFunctor(threadName, &num_sieve::Sieve::eulerMethod);
                    break;
                default:
                    LOG_DBG(logger, "execute to run unknown sieve method.");
                    break;
            }
        }
    }();
    COMMAND_PRINT_TASK_TITLE("NUMERIC", NumericTaskType::sieve, "END") << std::endl;
}

void Command::updateSieveTask(const std::string& method)
{
    using util_hash::operator""_bkdrHash;
    switch (util_hash::bkdrHash(method.c_str()))
    {
        case "era"_bkdrHash:
            taskPlan.generalTask.numTask.sieveBit.set(SieveMethod::eratosthenes);
            break;
        case "eul"_bkdrHash:
            taskPlan.generalTask.numTask.sieveBit.set(SieveMethod::euler);
            break;
        default:
            throwUnexpectedMethodException("sieve: " + method);
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
        case Command::AlgorithmTaskType::notation:
            os << "NOTATION";
            break;
        case Command::AlgorithmTaskType::search:
            os << "SEARCH";
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
        case Command::NumericTaskType::arithmetic:
            os << "ARITHMETIC";
            break;
        case Command::NumericTaskType::divisor:
            os << "DIVISOR";
            break;
        case Command::NumericTaskType::integral:
            os << "INTEGRAL";
            break;
        case Command::NumericTaskType::optimum:
            os << "OPTIMUM";
            break;
        case Command::NumericTaskType::sieve:
            os << "SIEVE";
            break;
        default:
            os << "UNKNOWN: " << static_cast<std::underlying_type_t<Command::NumericTaskType>>(taskType);
    }

    return os;
}
