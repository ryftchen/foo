#include "command.hpp"
#include "algorithm/include/match.hpp"
#include "algorithm/include/notation.hpp"
#include "algorithm/include/search.hpp"
#include "algorithm/include/sort.hpp"
#include "design_pattern/include/behavioral.hpp"
#include "design_pattern/include/creational.hpp"
#include "design_pattern/include/structural.hpp"
#include "numeric/include/arithmetic.hpp"
#include "numeric/include/divisor.hpp"
#include "numeric/include/integral.hpp"
#include "numeric/include/optimal.hpp"
#include "numeric/include/sieve.hpp"
#include "utility/include/hash.hpp"
#include "utility/include/log.hpp"
#include "utility/include/thread.hpp"

#define COMMAND_PRINT_TASK_BEGIN_TITLE                                                                               \
    std::cout << "\r\n"                                                                                              \
              << taskCategory << " TASK: " << std::setiosflags(std::ios_base::left) << std::setfill('.')             \
              << std::setw(titleWidthForPrintTask) << taskType << "BEGIN" << std::resetiosflags(std::ios_base::left) \
              << std::setfill(' ') << std::endl;                                                                     \
    {
#define COMMAND_PRINT_TASK_END_TITLE                                                                               \
    }                                                                                                              \
    std::cout << "\r\n"                                                                                            \
              << taskCategory << " TASK: " << std::setiosflags(std::ios_base::left) << std::setfill('.')           \
              << std::setw(titleWidthForPrintTask) << taskType << "END" << std::resetiosflags(std::ios_base::left) \
              << std::setfill(' ') << "\r\n"                                                                       \
              << std::endl;

Command::Command()
{
    try
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

        program.addArgument("-dp", "--design-pattern")
            .nArgs(1)
            .action(
                [this](const std::string& value)
                {
                    if (std::any_of(
                            generalTaskMap.at("design-pattern").cbegin(),
                            generalTaskMap.at("design-pattern").cend(),
                            [&value](const auto& taskCategoryMap)
                            {
                                return (taskCategoryMap.first == value);
                            }))
                    {
                        return value;
                    }
                    throw std::runtime_error("Unknown design pattern category: " + value);
                })
            .help("demo of design pattern, see \"tasks\" for detail of specific category");

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
    catch (const std::exception& error)
    {
        std::cerr << error.what() << std::endl;
    }
}

void Command::runCommander(const int argc, const char* const argv[])
{
    LOG_TO_START;

    if (0 != argc - 1)
    {
        std::shared_ptr<util_thread::Thread> threads = std::make_shared<util_thread::Thread>(2);
        threads->enqueue("commander_fore", &Command::foregroundHandle, this, argc, argv);
        threads->enqueue("commander_back", &Command::backgroundHandle, this);
    }
    else
    {
        LOG_INF("Enter console mode.");
        enterConsoleMode();
    }

    LOG_TO_STOP;
}

void Command::foregroundHandle(const int argc, const char* const argv[])
{
    try
    {
        std::unique_lock<std::mutex> lock(commandMutex);
        program.parseArgs(argc, argv);
        validateBasicTask();
        validateGeneralTask();

        lock.unlock();
        commandCondition.notify_one();
        util_time::millisecondLevelSleep(1);
        lock.lock();
    }
    catch (const std::exception& error)
    {
        LOG_WRN(error.what());
    }
}

void Command::backgroundHandle()
{
    try
    {
        if (std::unique_lock<std::mutex> lock(commandMutex); true)
        {
            commandCondition.wait(
                lock,
                [this]() -> decltype(auto)
                {
                    return true;
                });
        }

        if (checkTask())
        {
            performTask();
        }
    }
    catch (const std::exception& error)
    {
        LOG_ERR(error.what());
    }
}

void Command::validateBasicTask()
{
    for (int i = 0; i < BasicTask::Bottom<BasicTask::Category>::value; ++i)
    {
        if (!program.isUsed(std::next(basicTaskMap.cbegin(), BasicTask::Category(i))->first))
        {
            continue;
        }

        if (checkTask())
        {
            throwExcessArgumentException();
        }

        taskPlan.basicTask.basicTaskBit.set(BasicTask::Category(i));
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
        for (int i = 0; i < BasicTask::Bottom<BasicTask::Category>::value; ++i)
        {
            if (taskPlan.basicTask.basicTaskBit.test(BasicTask::Category(i)))
            {
                (this->*std::next(basicTaskMap.cbegin(), BasicTask::Category(i))->second)();
            }
        }
    }

    for (int i = 0; i < GeneralTask::Bottom<GeneralTask::Category>::value; ++i)
    {
        switch (GeneralTask::Category(i))
        {
            case GeneralTask::Category::algorithm:
                if (taskPlan.generalTask.algorithmTask.empty())
                {
                    continue;
                }
                break;
            case GeneralTask::Category::designPattern:
                if (taskPlan.generalTask.designPatternTask.empty())
                {
                    continue;
                }
                break;
            case GeneralTask::Category::numeric:
                if (taskPlan.generalTask.numericTask.empty())
                {
                    continue;
                }
                break;
            default:
                break;
        }

        for ([[maybe_unused]] const auto& [taskTypeName, taskTypeTuple] :
             std::next(generalTaskMap.cbegin(), GeneralTask::Category(i))->second)
        {
            (this->*get<PerformTaskFunctor>(get<TaskFunctorTuple>(taskTypeTuple)))();
        }
    }
}

void Command::printConsoleOutput() const
{
    const auto commands =
        program.get<std::vector<std::string>>(std::next(basicTaskMap.cbegin(), BasicTask::Category::console)->first);
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

    util_common::executeCommand(versionStr.c_str());
}

void Command::enterConsoleMode() const
{
    util_common::executeCommand(("tput bel; echo " + getIconBanner()).c_str());

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
    LOG_TO_STOP;

    util_common::printFile(util_log::logPath.data(), true, maxLineNumForPrintLog, &util_log::changeLogLevelStyle);
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
    if (taskPlan.generalTask.getBit<MatchMethod>().none())
    {
        return;
    }

    static_assert(algo_match::maxDigit > algo_match::singlePattern.length());
    const auto [taskCategory, taskType] = getMethodAttribute<MatchMethod>();
    COMMAND_PRINT_TASK_BEGIN_TITLE;

    const std::shared_ptr<algo_match::Match> match = std::make_shared<algo_match::Match>(algo_match::maxDigit);
    std::shared_ptr<util_thread::Thread> threads = std::make_shared<util_thread::Thread>(std::min(
        static_cast<uint32_t>(taskPlan.generalTask.getBit<MatchMethod>().count()),
        static_cast<uint32_t>(AlgorithmTask::Bottom<MatchMethod>::value)));
    const auto matchFunctor =
        [&](const std::string& threadName, int (*methodPtr)(const char*, const char*, const uint32_t, const uint32_t))
    {
        threads->enqueue(
            threadName,
            methodPtr,
            match->getSearchingText().get(),
            algo_match::singlePattern.data(),
            match->getLength(),
            algo_match::singlePattern.length());
    };

    for (int i = 0; i < AlgorithmTask::Bottom<MatchMethod>::value; ++i)
    {
        if (!taskPlan.generalTask.getBit<MatchMethod>().test(MatchMethod(i)))
        {
            continue;
        }

        const auto [targetMethod, threadName] = getMethodDetail(taskCategory, taskType, i);
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
                LOG_DBG("execute to run unknown match method.");
                break;
        }
    }

    COMMAND_PRINT_TASK_END_TITLE;
}

void Command::updateMatchTask(const std::string& method)
{
    using util_hash::operator""_bkdrHash;
    switch (util_hash::bkdrHash(method.c_str()))
    {
        case "rab"_bkdrHash:
            taskPlan.generalTask.setBit<MatchMethod>(MatchMethod::rabinKarp);
            break;
        case "knu"_bkdrHash:
            taskPlan.generalTask.setBit<MatchMethod>(MatchMethod::knuthMorrisPratt);
            break;
        case "boy"_bkdrHash:
            taskPlan.generalTask.setBit<MatchMethod>(MatchMethod::boyerMoore);
            break;
        case "hor"_bkdrHash:
            taskPlan.generalTask.setBit<MatchMethod>(MatchMethod::horspool);
            break;
        case "sun"_bkdrHash:
            taskPlan.generalTask.setBit<MatchMethod>(MatchMethod::sunday);
            break;
        default:
            throwUnexpectedMethodException("match: " + method);
    }
}

void Command::runNotation() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    if (taskPlan.generalTask.getBit<NotationMethod>().none())
    {
        return;
    }

    const auto [taskCategory, taskType] = getMethodAttribute<NotationMethod>();
    COMMAND_PRINT_TASK_BEGIN_TITLE;

    const std::shared_ptr<algo_notation::Notation> notation = std::make_shared<algo_notation::Notation>();
    std::shared_ptr<util_thread::Thread> threads = std::make_shared<util_thread::Thread>(std::min(
        static_cast<uint32_t>(taskPlan.generalTask.getBit<NotationMethod>().count()),
        static_cast<uint32_t>(AlgorithmTask::Bottom<NotationMethod>::value)));
    const auto notationFunctor = [&](const std::string& threadName, std::string (*methodPtr)(const std::string&))
    {
        threads->enqueue(threadName, methodPtr, std::string{algo_notation::infixNotation});
    };

    for (int i = 0; i < AlgorithmTask::Bottom<NotationMethod>::value; ++i)
    {
        if (!taskPlan.generalTask.getBit<NotationMethod>().test(NotationMethod(i)))
        {
            continue;
        }

        const auto [targetMethod, threadName] = getMethodDetail(taskCategory, taskType, i);
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
                LOG_DBG("execute to run unknown notation method.");
                break;
        }
    }

    COMMAND_PRINT_TASK_END_TITLE;
}

void Command::updateNotationTask(const std::string& method)
{
    using util_hash::operator""_bkdrHash;
    switch (util_hash::bkdrHash(method.c_str()))
    {
        case "pre"_bkdrHash:
            taskPlan.generalTask.setBit<NotationMethod>(NotationMethod::prefix);
            break;
        case "pos"_bkdrHash:
            taskPlan.generalTask.setBit<NotationMethod>(NotationMethod::postfix);
            break;
        default:
            throwUnexpectedMethodException("notation: " + method);
    }
}

void Command::runSearch() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    if (taskPlan.generalTask.getBit<SearchMethod>().none())
    {
        return;
    }

    static_assert((algo_search::arrayRange1 < algo_search::arrayRange2) && (algo_search::arrayLength > 0));
    const auto [taskCategory, taskType] = getMethodAttribute<SearchMethod>();
    COMMAND_PRINT_TASK_BEGIN_TITLE;

    const std::shared_ptr<algo_search::Search<double>> search = std::make_shared<algo_search::Search<double>>(
        algo_search::arrayLength, algo_search::arrayRange1, algo_search::arrayRange2);
    std::shared_ptr<util_thread::Thread> threads = std::make_shared<util_thread::Thread>(std::min(
        static_cast<uint32_t>(taskPlan.generalTask.getBit<SearchMethod>().count()),
        static_cast<uint32_t>(AlgorithmTask::Bottom<SearchMethod>::value)));
    const auto searchFunctor =
        [&](const std::string& threadName,
            int (algo_search::Search<double>::*methodPtr)(const double* const, const uint32_t, const double) const)
    {
        threads->enqueue(
            threadName,
            methodPtr,
            search,
            search->getOrderedArray().get(),
            search->getLength(),
            search->getSearchedKey());
    };

    for (int i = 0; i < AlgorithmTask::Bottom<SearchMethod>::value; ++i)
    {
        if (!taskPlan.generalTask.getBit<SearchMethod>().test(SearchMethod(i)))
        {
            continue;
        }

        const auto [targetMethod, threadName] = getMethodDetail(taskCategory, taskType, i);
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
                LOG_DBG("execute to run unknown search method.");
                break;
        }
    }

    COMMAND_PRINT_TASK_END_TITLE;
}

void Command::updateSearchTask(const std::string& method)
{
    using util_hash::operator""_bkdrHash;
    switch (util_hash::bkdrHash(method.c_str()))
    {
        case "bin"_bkdrHash:
            taskPlan.generalTask.setBit<SearchMethod>(SearchMethod::binary);
            break;
        case "int"_bkdrHash:
            taskPlan.generalTask.setBit<SearchMethod>(SearchMethod::interpolation);
            break;
        case "fib"_bkdrHash:
            taskPlan.generalTask.setBit<SearchMethod>(SearchMethod::fibonacci);
            break;
        default:
            throwUnexpectedMethodException("search: " + method);
    }
}

void Command::runSort() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    if (taskPlan.generalTask.getBit<SortMethod>().none())
    {
        return;
    }

    static_assert((algo_sort::arrayRange1 < algo_sort::arrayRange2) && (algo_sort::arrayLength > 0));
    const auto [taskCategory, taskType] = getMethodAttribute<SortMethod>();
    COMMAND_PRINT_TASK_BEGIN_TITLE;

    const std::shared_ptr<algo_sort::Sort<int>> sort =
        std::make_shared<algo_sort::Sort<int>>(algo_sort::arrayLength, algo_sort::arrayRange1, algo_sort::arrayRange2);
    std::shared_ptr<util_thread::Thread> threads = std::make_shared<util_thread::Thread>(std::min(
        static_cast<uint32_t>(taskPlan.generalTask.getBit<SortMethod>().count()),
        static_cast<uint32_t>(AlgorithmTask::Bottom<SortMethod>::value)));
    const auto sortFunctor = [&](const std::string& threadName,
                                 std::vector<int> (algo_sort::Sort<int>::*methodPtr)(int* const, const uint32_t) const)
    {
        threads->enqueue(threadName, methodPtr, sort, sort->getRandomArray().get(), sort->getLength());
    };

    for (int i = 0; i < AlgorithmTask::Bottom<SortMethod>::value; ++i)
    {
        if (!taskPlan.generalTask.getBit<SortMethod>().test(SortMethod(i)))
        {
            continue;
        }

        const auto [targetMethod, threadName] = getMethodDetail(taskCategory, taskType, i);
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
                LOG_DBG("execute to run unknown sort method.");
                break;
        }
    }

    COMMAND_PRINT_TASK_END_TITLE;
}

void Command::updateSortTask(const std::string& method)
{
    using util_hash::operator""_bkdrHash;
    switch (util_hash::bkdrHash(method.c_str()))
    {
        case "bub"_bkdrHash:
            taskPlan.generalTask.setBit<SortMethod>(SortMethod::bubble);
            break;
        case "sel"_bkdrHash:
            taskPlan.generalTask.setBit<SortMethod>(SortMethod::selection);
            break;
        case "ins"_bkdrHash:
            taskPlan.generalTask.setBit<SortMethod>(SortMethod::insertion);
            break;
        case "she"_bkdrHash:
            taskPlan.generalTask.setBit<SortMethod>(SortMethod::shell);
            break;
        case "mer"_bkdrHash:
            taskPlan.generalTask.setBit<SortMethod>(SortMethod::merge);
            break;
        case "qui"_bkdrHash:
            taskPlan.generalTask.setBit<SortMethod>(SortMethod::quick);
            break;
        case "hea"_bkdrHash:
            taskPlan.generalTask.setBit<SortMethod>(SortMethod::heap);
            break;
        case "cou"_bkdrHash:
            taskPlan.generalTask.setBit<SortMethod>(SortMethod::counting);
            break;
        case "buc"_bkdrHash:
            taskPlan.generalTask.setBit<SortMethod>(SortMethod::bucket);
            break;
        case "rad"_bkdrHash:
            taskPlan.generalTask.setBit<SortMethod>(SortMethod::radix);
            break;
        default:
            throwUnexpectedMethodException("sort: " + method);
    }
}

void Command::runBehavioral() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    if (taskPlan.generalTask.getBit<BehavioralMethod>().none())
    {
        return;
    }

    const auto [taskCategory, taskType] = getMethodAttribute<BehavioralMethod>();
    COMMAND_PRINT_TASK_BEGIN_TITLE;

    const std::shared_ptr<dp_behavioral::Behavioral> behavioral = std::make_shared<dp_behavioral::Behavioral>();
    std::shared_ptr<util_thread::Thread> threads = std::make_shared<util_thread::Thread>(std::min(
        static_cast<uint32_t>(taskPlan.generalTask.getBit<BehavioralMethod>().count()),
        static_cast<uint32_t>(DesignPatternTask::Bottom<BehavioralMethod>::value)));
    const auto behavioralFunctor = [&](const std::string& threadName, void (*methodPtr)())
    {
        threads->enqueue(threadName, methodPtr);
    };

    for (int i = 0; i < DesignPatternTask::Bottom<BehavioralMethod>::value; ++i)
    {
        if (!taskPlan.generalTask.getBit<BehavioralMethod>().test(BehavioralMethod(i)))
        {
            continue;
        }

        const auto [targetMethod, threadName] = getMethodDetail(taskCategory, taskType, i);
        using util_hash::operator""_bkdrHash;
        switch (util_hash::bkdrHash(targetMethod.data()))
        {
            case "cha"_bkdrHash:
                behavioralFunctor(threadName, &dp_behavioral::Behavioral::chainOfResponsibilityInstance);
                break;
            case "com"_bkdrHash:
                behavioralFunctor(threadName, &dp_behavioral::Behavioral::commandInstance);
                break;
            case "int"_bkdrHash:
                behavioralFunctor(threadName, &dp_behavioral::Behavioral::interpreterInstance);
                break;
            case "ite"_bkdrHash:
                behavioralFunctor(threadName, &dp_behavioral::Behavioral::iteratorInstance);
                break;
            case "med"_bkdrHash:
                behavioralFunctor(threadName, &dp_behavioral::Behavioral::mediatorInstance);
                break;
            case "mem"_bkdrHash:
                behavioralFunctor(threadName, &dp_behavioral::Behavioral::mementoInstance);
                break;
            case "obs"_bkdrHash:
                behavioralFunctor(threadName, &dp_behavioral::Behavioral::observerInstance);
                break;
            case "sta"_bkdrHash:
                behavioralFunctor(threadName, &dp_behavioral::Behavioral::stateInstance);
                break;
            case "str"_bkdrHash:
                behavioralFunctor(threadName, &dp_behavioral::Behavioral::strategyInstance);
                break;
            case "tem"_bkdrHash:
                behavioralFunctor(threadName, &dp_behavioral::Behavioral::templateMethodInstance);
                break;
            case "vis"_bkdrHash:
                behavioralFunctor(threadName, &dp_behavioral::Behavioral::visitorInstance);
                break;
            default:
                LOG_DBG("execute to run unknown behavioral method.");
                break;
        }
    }

    COMMAND_PRINT_TASK_END_TITLE;
}

void Command::updateBehavioralTask(const std::string& method)
{
    using util_hash::operator""_bkdrHash;
    switch (util_hash::bkdrHash(method.c_str()))
    {
        case "cha"_bkdrHash:
            taskPlan.generalTask.setBit<BehavioralMethod>(BehavioralMethod::chainOfResponsibility);
            break;
        case "com"_bkdrHash:
            taskPlan.generalTask.setBit<BehavioralMethod>(BehavioralMethod::command);
            break;
        case "int"_bkdrHash:
            taskPlan.generalTask.setBit<BehavioralMethod>(BehavioralMethod::interpreter);
            break;
        case "ite"_bkdrHash:
            taskPlan.generalTask.setBit<BehavioralMethod>(BehavioralMethod::iterator);
            break;
        case "med"_bkdrHash:
            taskPlan.generalTask.setBit<BehavioralMethod>(BehavioralMethod::mediator);
            break;
        case "mem"_bkdrHash:
            taskPlan.generalTask.setBit<BehavioralMethod>(BehavioralMethod::memento);
            break;
        case "obs"_bkdrHash:
            taskPlan.generalTask.setBit<BehavioralMethod>(BehavioralMethod::observer);
            break;
        case "sta"_bkdrHash:
            taskPlan.generalTask.setBit<BehavioralMethod>(BehavioralMethod::state);
            break;
        case "str"_bkdrHash:
            taskPlan.generalTask.setBit<BehavioralMethod>(BehavioralMethod::strategy);
            break;
        case "tem"_bkdrHash:
            taskPlan.generalTask.setBit<BehavioralMethod>(BehavioralMethod::templateMethod);
            break;
        case "vis"_bkdrHash:
            taskPlan.generalTask.setBit<BehavioralMethod>(BehavioralMethod::visitor);
            break;
        default:
            throwUnexpectedMethodException("behavioral: " + method);
    }
}

void Command::runCreational() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    if (taskPlan.generalTask.getBit<CreationalMethod>().none())
    {
        return;
    }

    const auto [taskCategory, taskType] = getMethodAttribute<CreationalMethod>();
    COMMAND_PRINT_TASK_BEGIN_TITLE;

    const std::shared_ptr<dp_creational::Creational> creational = std::make_shared<dp_creational::Creational>();
    std::shared_ptr<util_thread::Thread> threads = std::make_shared<util_thread::Thread>(std::min(
        static_cast<uint32_t>(taskPlan.generalTask.getBit<CreationalMethod>().count()),
        static_cast<uint32_t>(DesignPatternTask::Bottom<CreationalMethod>::value)));
    const auto creationalFunctor = [&](const std::string& threadName, void (*methodPtr)())
    {
        threads->enqueue(threadName, methodPtr);
    };

    for (int i = 0; i < DesignPatternTask::Bottom<CreationalMethod>::value; ++i)
    {
        if (!taskPlan.generalTask.getBit<CreationalMethod>().test(CreationalMethod(i)))
        {
            continue;
        }

        const auto [targetMethod, threadName] = getMethodDetail(taskCategory, taskType, i);
        using util_hash::operator""_bkdrHash;
        switch (util_hash::bkdrHash(targetMethod.data()))
        {
            case "abs"_bkdrHash:
                creationalFunctor(threadName, &dp_creational::Creational::abstractFactoryInstance);
                break;
            case "bui"_bkdrHash:
                creationalFunctor(threadName, &dp_creational::Creational::builderInstance);
                break;
            case "fac"_bkdrHash:
                creationalFunctor(threadName, &dp_creational::Creational::factoryMethodInstance);
                break;
            case "pro"_bkdrHash:
                creationalFunctor(threadName, &dp_creational::Creational::prototypeInstance);
                break;
            case "sin"_bkdrHash:
                creationalFunctor(threadName, &dp_creational::Creational::singletonInstance);
                break;
            default:
                LOG_DBG("execute to run unknown design pattern method.");
                break;
        }
    }

    COMMAND_PRINT_TASK_END_TITLE;
}

void Command::updateCreationalTask(const std::string& method)
{
    using util_hash::operator""_bkdrHash;
    switch (util_hash::bkdrHash(method.c_str()))
    {
        case "abs"_bkdrHash:
            taskPlan.generalTask.setBit<CreationalMethod>(CreationalMethod::abstractFactory);
            break;
        case "bui"_bkdrHash:
            taskPlan.generalTask.setBit<CreationalMethod>(CreationalMethod::builder);
            break;
        case "fac"_bkdrHash:
            taskPlan.generalTask.setBit<CreationalMethod>(CreationalMethod::factoryMethod);
            break;
        case "pro"_bkdrHash:
            taskPlan.generalTask.setBit<CreationalMethod>(CreationalMethod::prototype);
            break;
        case "sin"_bkdrHash:
            taskPlan.generalTask.setBit<CreationalMethod>(CreationalMethod::singleton);
            break;
        default:
            throwUnexpectedMethodException("creational: " + method);
    }
}

void Command::runStructural() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    if (taskPlan.generalTask.getBit<StructuralMethod>().none())
    {
        return;
    }

    const auto [taskCategory, taskType] = getMethodAttribute<StructuralMethod>();
    COMMAND_PRINT_TASK_BEGIN_TITLE;

    const std::shared_ptr<dp_structural::Structural> structural = std::make_shared<dp_structural::Structural>();
    std::shared_ptr<util_thread::Thread> threads = std::make_shared<util_thread::Thread>(std::min(
        static_cast<uint32_t>(taskPlan.generalTask.getBit<StructuralMethod>().count()),
        static_cast<uint32_t>(DesignPatternTask::Bottom<StructuralMethod>::value)));
    const auto structuralFunctor = [&](const std::string& threadName, void (*methodPtr)())
    {
        threads->enqueue(threadName, methodPtr);
    };

    for (int i = 0; i < DesignPatternTask::Bottom<StructuralMethod>::value; ++i)
    {
        if (!taskPlan.generalTask.getBit<StructuralMethod>().test(StructuralMethod(i)))
        {
            continue;
        }

        const auto [targetMethod, threadName] = getMethodDetail(taskCategory, taskType, i);
        using util_hash::operator""_bkdrHash;
        switch (util_hash::bkdrHash(targetMethod.data()))
        {
            case "ada"_bkdrHash:
                structuralFunctor(threadName, &dp_structural::Structural::adapterInstance);
                break;
            case "bri"_bkdrHash:
                structuralFunctor(threadName, &dp_structural::Structural::bridgeInstance);
                break;
            case "com"_bkdrHash:
                structuralFunctor(threadName, &dp_structural::Structural::compositeInstance);
                break;
            case "dec"_bkdrHash:
                structuralFunctor(threadName, &dp_structural::Structural::decoratorInstance);
                break;
            case "fac"_bkdrHash:
                structuralFunctor(threadName, &dp_structural::Structural::facadeInstance);
                break;
            case "fly"_bkdrHash:
                structuralFunctor(threadName, &dp_structural::Structural::flyweightInstance);
                break;
            case "pro"_bkdrHash:
                structuralFunctor(threadName, &dp_structural::Structural::proxyInstance);
                break;
            default:
                LOG_DBG("execute to run unknown structural method.");
                break;
        }
    }

    COMMAND_PRINT_TASK_END_TITLE;
}

void Command::updateStructuralTask(const std::string& method)
{
    using util_hash::operator""_bkdrHash;
    switch (util_hash::bkdrHash(method.c_str()))
    {
        case "ada"_bkdrHash:
            taskPlan.generalTask.setBit<StructuralMethod>(StructuralMethod::adapter);
            break;
        case "bri"_bkdrHash:
            taskPlan.generalTask.setBit<StructuralMethod>(StructuralMethod::bridge);
            break;
        case "com"_bkdrHash:
            taskPlan.generalTask.setBit<StructuralMethod>(StructuralMethod::composite);
            break;
        case "dec"_bkdrHash:
            taskPlan.generalTask.setBit<StructuralMethod>(StructuralMethod::decorator);
            break;
        case "fac"_bkdrHash:
            taskPlan.generalTask.setBit<StructuralMethod>(StructuralMethod::facade);
            break;
        case "fly"_bkdrHash:
            taskPlan.generalTask.setBit<StructuralMethod>(StructuralMethod::flyweight);
            break;
        case "pro"_bkdrHash:
            taskPlan.generalTask.setBit<StructuralMethod>(StructuralMethod::proxy);
            break;
        default:
            throwUnexpectedMethodException("structural: " + method);
    }
}

void Command::runArithmetic() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    if (taskPlan.generalTask.getBit<ArithmeticMethod>().none())
    {
        return;
    }

    const auto [taskCategory, taskType] = getMethodAttribute<ArithmeticMethod>();
    COMMAND_PRINT_TASK_BEGIN_TITLE;

    const std::shared_ptr<num_arithmetic::Arithmetic> arithmetic = std::make_shared<num_arithmetic::Arithmetic>();
    std::shared_ptr<util_thread::Thread> threads = std::make_shared<util_thread::Thread>(std::min(
        static_cast<uint32_t>(taskPlan.generalTask.getBit<ArithmeticMethod>().count()),
        static_cast<uint32_t>(NumericTask::Bottom<ArithmeticMethod>::value)));
    const auto arithmeticFunctor = [&](const std::string& threadName, int (*methodPtr)(const int, const int))
    {
        threads->enqueue(threadName, methodPtr, num_arithmetic::integer1, num_arithmetic::integer2);
    };

    for (int i = 0; i < NumericTask::Bottom<ArithmeticMethod>::value; ++i)
    {
        if (!taskPlan.generalTask.getBit<ArithmeticMethod>().test(ArithmeticMethod(i)))
        {
            continue;
        }

        const auto [targetMethod, threadName] = getMethodDetail(taskCategory, taskType, i);
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
                LOG_DBG("execute to run unknown arithmetic method.");
                break;
        }
    }

    COMMAND_PRINT_TASK_END_TITLE;
}

void Command::updateArithmeticTask(const std::string& method)
{
    using util_hash::operator""_bkdrHash;
    switch (util_hash::bkdrHash(method.c_str()))
    {
        case "add"_bkdrHash:
            taskPlan.generalTask.setBit<ArithmeticMethod>(ArithmeticMethod::addition);
            break;
        case "sub"_bkdrHash:
            taskPlan.generalTask.setBit<ArithmeticMethod>(ArithmeticMethod::subtraction);
            break;
        case "mul"_bkdrHash:
            taskPlan.generalTask.setBit<ArithmeticMethod>(ArithmeticMethod::multiplication);
            break;
        case "div"_bkdrHash:
            taskPlan.generalTask.setBit<ArithmeticMethod>(ArithmeticMethod::division);
            break;
        default:
            throwUnexpectedMethodException("arithmetic: " + method);
    }
}

void Command::runDivisor() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    if (taskPlan.generalTask.getBit<DivisorMethod>().none())
    {
        return;
    }

    const auto [taskCategory, taskType] = getMethodAttribute<DivisorMethod>();
    COMMAND_PRINT_TASK_BEGIN_TITLE;

    const std::shared_ptr<num_divisor::Divisor> divisor = std::make_shared<num_divisor::Divisor>();
    std::shared_ptr<util_thread::Thread> threads = std::make_shared<util_thread::Thread>(std::min(
        static_cast<uint32_t>(taskPlan.generalTask.getBit<DivisorMethod>().count()),
        static_cast<uint32_t>(NumericTask::Bottom<DivisorMethod>::value)));
    const auto divisorFunctor =
        [&](const std::string& threadName, std::vector<int> (num_divisor::Divisor::*methodPtr)(int, int) const)
    {
        threads->enqueue(threadName, methodPtr, divisor, num_divisor::integer1, num_divisor::integer2);
    };

    for (int i = 0; i < NumericTask::Bottom<DivisorMethod>::value; ++i)
    {
        if (!taskPlan.generalTask.getBit<DivisorMethod>().test(DivisorMethod(i)))
        {
            continue;
        }

        const auto [targetMethod, threadName] = getMethodDetail(taskCategory, taskType, i);
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
                LOG_DBG("execute to run unknown divisor method.");
                break;
        }
    }

    COMMAND_PRINT_TASK_END_TITLE;
}

void Command::updateDivisorTask(const std::string& method)
{
    using util_hash::operator""_bkdrHash;
    switch (util_hash::bkdrHash(method.c_str()))
    {
        case "euc"_bkdrHash:
            taskPlan.generalTask.setBit<DivisorMethod>(DivisorMethod::euclid);
            break;
        case "ste"_bkdrHash:
            taskPlan.generalTask.setBit<DivisorMethod>(DivisorMethod::stein);
            break;
        default:
            throwUnexpectedMethodException("divisor: " + method);
    }
}

void Command::runIntegral() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    if (taskPlan.generalTask.getBit<IntegralMethod>().none())
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
        std::shared_ptr<util_thread::Thread> threads = std::make_shared<util_thread::Thread>(std::min(
            static_cast<uint32_t>(taskPlan.generalTask.getBit<IntegralMethod>().count()),
            static_cast<uint32_t>(NumericTask::Bottom<IntegralMethod>::value)));
        const auto integralFunctor =
            [&](const std::string& threadName, const std::shared_ptr<num_integral::Integral>& classPtr)
        {
            threads->enqueue(
                threadName,
                &num_integral::Integral::operator(),
                classPtr,
                range.range1,
                range.range2,
                num_integral::epsilon);
        };

        const auto [taskCategory, taskType] = getMethodAttribute<IntegralMethod>();
        for (int i = 0; i < NumericTask::Bottom<IntegralMethod>::value; ++i)
        {
            if (!taskPlan.generalTask.getBit<IntegralMethod>().test(IntegralMethod(i)))
            {
                continue;
            }

            const auto [targetMethod, threadName] = getMethodDetail(taskCategory, taskType, i);
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
                    LOG_DBG("execute to run unknown integral method.");
                    break;
            }
        }
    };

    const auto [taskCategory, taskType] = getMethodAttribute<IntegralMethod>();
    COMMAND_PRINT_TASK_BEGIN_TITLE;

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

    COMMAND_PRINT_TASK_END_TITLE;
}

void Command::updateIntegralTask(const std::string& method)
{
    using util_hash::operator""_bkdrHash;
    switch (util_hash::bkdrHash(method.c_str()))
    {
        case "tra"_bkdrHash:
            taskPlan.generalTask.setBit<IntegralMethod>(IntegralMethod::trapezoidal);
            break;
        case "sim"_bkdrHash:
            taskPlan.generalTask.setBit<IntegralMethod>(IntegralMethod::simpson);
            break;
        case "rom"_bkdrHash:
            taskPlan.generalTask.setBit<IntegralMethod>(IntegralMethod::romberg);
            break;
        case "gau"_bkdrHash:
            taskPlan.generalTask.setBit<IntegralMethod>(IntegralMethod::gauss);
            break;
        case "mon"_bkdrHash:
            taskPlan.generalTask.setBit<IntegralMethod>(IntegralMethod::monteCarlo);
            break;
        default:
            throwUnexpectedMethodException("integral: " + method);
    }
}

void Command::runOptimal() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    if (taskPlan.generalTask.getBit<OptimalMethod>().none())
    {
        return;
    }

    const auto printFunctor = [](const OptimalExprTarget& expression)
    {
        constexpr std::string_view prefix{"\r\nOptimal expression: "};
        std::visit(
            num_expression::ExprOverloaded{
                [&prefix](const num_expression::Griewank& /*unused*/)
                {
                    std::cout << prefix << num_expression::Griewank::optimalExpr << std::endl;
                },
                [&prefix](const num_expression::Rastrigin& /*unused*/)
                {
                    std::cout << prefix << num_expression::Rastrigin::optimalExpr << std::endl;
                },
            },
            expression);
    };
    const auto resultFunctor =
        [this](const num_expression::Expression& expression, const num_expression::ExprRange<double, double>& range)
    {
        assert((range.range1 < range.range2) && (num_optimal::epsilon > 0.0));
        std::shared_ptr<util_thread::Thread> threads = std::make_shared<util_thread::Thread>(std::min(
            static_cast<uint32_t>(taskPlan.generalTask.getBit<OptimalMethod>().count()),
            static_cast<uint32_t>(NumericTask::Bottom<OptimalMethod>::value)));
        const auto optimalFunctor =
            [&](const std::string& threadName, const std::shared_ptr<num_optimal::Optimal>& classPtr)
        {
            threads->enqueue(
                threadName,
                &num_optimal::Optimal::operator(),
                classPtr,
                range.range1,
                range.range2,
                num_optimal::epsilon);
        };

        const auto [taskCategory, taskType] = getMethodAttribute<OptimalMethod>();
        for (int i = 0; i < NumericTask::Bottom<OptimalMethod>::value; ++i)
        {
            if (!taskPlan.generalTask.getBit<OptimalMethod>().test(OptimalMethod(i)))
            {
                continue;
            }

            const auto [targetMethod, threadName] = getMethodDetail(taskCategory, taskType, i);
            using util_hash::operator""_bkdrHash;
            switch (util_hash::bkdrHash(targetMethod.data()))
            {
                case "gra"_bkdrHash:
                    optimalFunctor(threadName, std::make_shared<num_optimal::Gradient>(expression));
                    break;
                case "ann"_bkdrHash:
                    optimalFunctor(threadName, std::make_shared<num_optimal::Annealing>(expression));
                    break;
                case "par"_bkdrHash:
                    optimalFunctor(threadName, std::make_shared<num_optimal::Particle>(expression));
                    break;
                case "gen"_bkdrHash:
                    optimalFunctor(threadName, std::make_shared<num_optimal::Genetic>(expression));
                    break;
                default:
                    LOG_DBG("Unable to execute unknown optimal method.");
                    break;
            }
        }
    };

    const auto [taskCategory, taskType] = getMethodAttribute<OptimalMethod>();
    COMMAND_PRINT_TASK_BEGIN_TITLE;

    for ([[maybe_unused]] const auto& [range, expression] : optimalExprMap)
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

    COMMAND_PRINT_TASK_END_TITLE;
}

void Command::updateOptimalTask(const std::string& method)
{
    using util_hash::operator""_bkdrHash;
    switch (util_hash::bkdrHash(method.c_str()))
    {
        case "gra"_bkdrHash:
            taskPlan.generalTask.setBit<OptimalMethod>(OptimalMethod::gradient);
            break;
        case "ann"_bkdrHash:
            taskPlan.generalTask.setBit<OptimalMethod>(OptimalMethod::annealing);
            break;
        case "par"_bkdrHash:
            taskPlan.generalTask.setBit<OptimalMethod>(OptimalMethod::particle);
            break;
        case "gen"_bkdrHash:
            taskPlan.generalTask.setBit<OptimalMethod>(OptimalMethod::genetic);
            break;
        default:
            throwUnexpectedMethodException("optimal: " + method);
    }
}

void Command::runSieve() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    if (taskPlan.generalTask.getBit<SieveMethod>().none())
    {
        return;
    }

    const auto [taskCategory, taskType] = getMethodAttribute<SieveMethod>();
    COMMAND_PRINT_TASK_BEGIN_TITLE;

    const std::shared_ptr<num_sieve::Sieve> sieve = std::make_shared<num_sieve::Sieve>();
    std::shared_ptr<util_thread::Thread> threads = std::make_shared<util_thread::Thread>(std::min(
        static_cast<uint32_t>(taskPlan.generalTask.getBit<SieveMethod>().count()),
        static_cast<uint32_t>(NumericTask::Bottom<SieveMethod>::value)));
    const auto sieveFunctor =
        [&](const std::string& threadName, std::vector<uint32_t> (num_sieve::Sieve::*methodPtr)(const uint32_t) const)
    {
        threads->enqueue(threadName, methodPtr, sieve, num_sieve::maxPositiveInteger);
    };

    for (int i = 0; i < NumericTask::Bottom<SieveMethod>::value; ++i)
    {
        if (!taskPlan.generalTask.getBit<SieveMethod>().test(SieveMethod(i)))
        {
            continue;
        }

        const auto [targetMethod, threadName] = getMethodDetail(taskCategory, taskType, i);
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
                LOG_DBG("execute to run unknown sieve method.");
                break;
        }
    }

    COMMAND_PRINT_TASK_END_TITLE;
}

void Command::updateSieveTask(const std::string& method)
{
    using util_hash::operator""_bkdrHash;
    switch (util_hash::bkdrHash(method.c_str()))
    {
        case "era"_bkdrHash:
            taskPlan.generalTask.setBit<SieveMethod>(SieveMethod::eratosthenes);
            break;
        case "eul"_bkdrHash:
            taskPlan.generalTask.setBit<SieveMethod>(SieveMethod::euler);
            break;
        default:
            throwUnexpectedMethodException("sieve: " + method);
    }
}
