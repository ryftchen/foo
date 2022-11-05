#include "command.hpp"
#include "algorithm/include/match.hpp"
#include "algorithm/include/notation.hpp"
#include "algorithm/include/search.hpp"
#include "algorithm/include/sort.hpp"
#include "data_structure/include/linear.hpp"
#include "design_pattern/include/behavioral.hpp"
#include "design_pattern/include/creational.hpp"
#include "design_pattern/include/structural.hpp"
#include "numeric/include/arithmetic.hpp"
#include "numeric/include/divisor.hpp"
#include "numeric/include/integral.hpp"
#include "numeric/include/optimal.hpp"
#include "numeric/include/prime.hpp"
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
        program.addArgument("-h", "--help").nArgs(0).implicitValue(true).help("show help");

        program.addArgument("-v", "--version").nArgs(0).implicitValue(true).help("show version");

        program.addArgument("-c", "--console")
            .nArgs(util_argument::NArgsPattern::any)
            .defaultValue<std::vector<std::string>>({"help"})
            .appending()
            .help("run commands on console mode");

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
            .help("match, notation, search, sort [add category with \"--help\" for task details]");

        program.addArgument("-ds", "--data-structure")
            .nArgs(1)
            .action(
                [this](const std::string& value)
                {
                    if (std::any_of(
                            generalTaskMap.at("data-structure").cbegin(),
                            generalTaskMap.at("data-structure").cend(),
                            [&value](const auto& taskCategoryMap)
                            {
                                return (taskCategoryMap.first == value);
                            }))
                    {
                        return value;
                    }
                    throw std::runtime_error("Unknown data structure category: " + value);
                })
            .help("linear [add category with \"--help\" for task details]");

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
            .help("behavioral, creational, structural [add category with \"--help\" for task details]");

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
            .help("arithmetic, divisor, integral, optimal, prime [add category with \"--help\" for task details]");

        program.addArgument("tasks").remaining().help("specify tasks");
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

        isParsed.store(true);
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
                    return isParsed.load();
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

        taskPlan.basicTask.primaryBit.set(BasicTask::Category(i));
    }
}

void Command::validateGeneralTask()
{
    bool isToBeExcess = false;
    for ([[maybe_unused]] const auto& [taskCategoryName, taskCategoryMap] : generalTaskMap)
    {
        if (!program.isUsed(taskCategoryName))
        {
            continue;
        }

        if (isToBeExcess || (checkTask() && !program.isUsed("help")))
        {
            throwExcessArgumentException();
        }

        for ([[maybe_unused]] const auto& [taskTypeName, taskTypeTuple] : taskCategoryMap)
        {
            if (taskTypeName != program[taskCategoryName])
            {
                continue;
            }

            TargetTaskVector tasks;
            if (program.isUsed("tasks"))
            {
                tasks = program.get<std::vector<std::string>>("tasks");
                if (program.isUsed("help"))
                {
                    throwExcessArgumentException();
                }
            }
            else
            {
                const auto taskMethodVector = get<TargetTaskVector>(taskTypeTuple);
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
            isToBeExcess = true;
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
            if (taskPlan.basicTask.primaryBit.test(BasicTask::Category(i)))
            {
                (this->*std::next(basicTaskMap.cbegin(), BasicTask::Category(i))->second)();
            }
        }
    }
    else if (!taskPlan.generalTask.empty() && !taskPlan.basicTask.primaryBit.test(BasicTask::Category::help))
    {
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
                case GeneralTask::Category::dataStructure:
                    if (taskPlan.generalTask.dataStructureTask.empty())
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
    if (taskPlan.generalTask.empty())
    {
        std::cout << program.help().str();
        return;
    }

    if (!taskPlan.generalTask.algorithmTask.empty())
    {
        std::cout << "Usage: foo -a, --algorithm ";
        if (!taskPlan.generalTask.getBit<MatchMethod>().none())
        {
            std::puts("match [tasks...]\r\n\r\nNon-optional:\r\n"
                      "rab    Rabin-Karp\r\n"
                      "knu    Knuth-Morris-Pratt\r\n"
                      "boy    Boyer-Moore\r\n"
                      "hor    Horspool\r\n"
                      "sun    Sunday");
        }
        else if (!taskPlan.generalTask.getBit<NotationMethod>().none())
        {
            std::puts("notation [tasks...]\r\n\r\nNon-optional:\r\n"
                      "pre    Prefix\r\n"
                      "pos    Postfix");
        }
        else if (!taskPlan.generalTask.getBit<SearchMethod>().none())
        {
            std::puts("search [tasks...]\r\n\r\nNon-optional:\r\n"
                      "bin    Binary\r\n"
                      "int    Interpolation\r\n"
                      "fib    Fibonacci");
        }
        else if (!taskPlan.generalTask.getBit<SortMethod>().none())
        {
            std::puts("search [tasks...]\r\n\r\nNon-optional:\r\n"
                      "bub    Bubble\r\n"
                      "sel    Selection\r\n"
                      "ins    Insertion\r\n"
                      "she    Shell\r\n"
                      "mer    Merge\r\n"
                      "qui    Quick\r\n"
                      "hea    Heap\r\n"
                      "cou    Counting\r\n"
                      "buc    Bucket\r\n"
                      "rad    Radix");
        }
    }
    else if (!taskPlan.generalTask.dataStructureTask.empty())
    {
        std::cout << "Usage: foo -ds, --data-structure ";
        if (!taskPlan.generalTask.getBit<LinearMethod>().none())
        {
            std::puts("linear [tasks...]\r\n\r\nNon-optional:\r\n"
                      "lin    Linked List\r\n"
                      "sta    Stack\r\n"
                      "que    Queue");
        }
    }
    else if (!taskPlan.generalTask.designPatternTask.empty())
    {
        std::cout << "Usage: foo -dp, --design-pattern ";
        if (!taskPlan.generalTask.getBit<BehavioralMethod>().none())
        {
            std::puts("behavioral [tasks...]\r\n\r\nNon-optional:\r\n"
                      "cha    Chain Of Responsibility\r\n"
                      "com    Command\r\n"
                      "int    Interpreter\r\n"
                      "ite    Iterator\r\n"
                      "med    Mediator\r\n"
                      "mem    Memento\r\n"
                      "obs    Observer\r\n"
                      "sta    State\r\n"
                      "str    Strategy\r\n"
                      "tem    Template Method\r\n"
                      "vis    Visitor");
        }
        else if (!taskPlan.generalTask.getBit<CreationalMethod>().none())
        {
            std::puts("creational [tasks...]\r\n\r\nNon-optional:\r\n"
                      "abs    Abstract Factory\r\n"
                      "bui    Builder\r\n"
                      "fac    Factory Method\r\n"
                      "pro    Prototype\r\n"
                      "sin    Singleton");
        }
        else if (!taskPlan.generalTask.getBit<StructuralMethod>().none())
        {
            std::puts("structural [tasks...]\r\n\r\nNon-optional:\r\n"
                      "ada    Adapter\r\n"
                      "bri    Bridge\r\n"
                      "com    Composite\r\n"
                      "dec    Decorator\r\n"
                      "fac    Facade\r\n"
                      "fly    Flyweight\r\n"
                      "pro    Proxy");
        }
    }
    else if (!taskPlan.generalTask.numericTask.empty())
    {
        std::cout << "Usage: foo -n, --numeric ";
        if (!taskPlan.generalTask.getBit<ArithmeticMethod>().none())
        {
            std::puts("arithmetic [tasks...]\r\n\r\nNon-optional:\r\n"
                      "add    Addition\r\n"
                      "sub    Subtraction\r\n"
                      "mul    Multiplication\r\n"
                      "div    Division");
        }
        else if (!taskPlan.generalTask.getBit<DivisorMethod>().none())
        {
            std::puts("divisor [tasks...]\r\n\r\nNon-optional:\r\n"
                      "euc    Euclidean\r\n"
                      "ste    Stein");
        }
        else if (!taskPlan.generalTask.getBit<IntegralMethod>().none())
        {
            std::puts("integral [tasks...]\r\n\r\nNon-optional:\r\n"
                      "tra    Trapezoidal\r\n"
                      "sim    Adaptive Simpson's 1/3\r\n"
                      "rom    Romberg\r\n"
                      "gau    Gauss-Legendre's 5-Points\r\n"
                      "mon    Monte-Carlo");
        }
        else if (!taskPlan.generalTask.getBit<OptimalMethod>().none())
        {
            std::puts("optimal [tasks...]\r\n\r\nNon-optional:\r\n"
                      "gra    Gradient Descent\r\n"
                      "ann    Simulated Annealing\r\n"
                      "par    Particle Swarm\r\n"
                      "gen    Genetic");
        }
        else if (!taskPlan.generalTask.getBit<PrimeMethod>().none())
        {
            std::puts("prime [tasks...]\r\n\r\nNon-optional:\r\n"
                      "era    Eratosthenes\r\n"
                      "eul    Euler");
        }
    }
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
    const auto [taskCategory, taskType] = getTargetTaskAttribute<MatchMethod>();
    COMMAND_PRINT_TASK_BEGIN_TITLE;

    using algo_match::MatchSolution;
    const std::shared_ptr<MatchSolution> match = std::make_shared<MatchSolution>(algo_match::maxDigit);
    std::shared_ptr<util_thread::Thread> threads = std::make_shared<util_thread::Thread>(std::min(
        static_cast<uint32_t>(taskPlan.generalTask.getBit<MatchMethod>().count()),
        static_cast<uint32_t>(AlgorithmTask::Bottom<MatchMethod>::value)));
    const auto matchFunctor =
        [&](const std::string& threadName,
            int (MatchSolution::*methodPtr)(const char*, const char*, const uint32_t, const uint32_t) const)
    {
        threads->enqueue(
            threadName,
            methodPtr,
            match,
            match->getMatchingText().get(),
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

        const auto [targetMethod, threadName] = getTargetTaskDetail(taskCategory, taskType, i);
        using util_hash::operator""_bkdrHash;
        switch (util_hash::bkdrHash(targetMethod.data()))
        {
            case "rab"_bkdrHash:
                matchFunctor(threadName, &MatchSolution::rkMethod);
                break;
            case "knu"_bkdrHash:
                matchFunctor(threadName, &MatchSolution::kmpMethod);
                break;
            case "boy"_bkdrHash:
                matchFunctor(threadName, &MatchSolution::bmMethod);
                break;
            case "hor"_bkdrHash:
                matchFunctor(threadName, &MatchSolution::horspoolMethod);
                break;
            case "sun"_bkdrHash:
                matchFunctor(threadName, &MatchSolution::sundayMethod);
                break;
            default:
                LOG_DBG("execute to run unknown match method.");
                break;
        }
    }

    COMMAND_PRINT_TASK_END_TITLE;
}

void Command::updateMatchTask(const std::string& target)
{
    using util_hash::operator""_bkdrHash;
    switch (util_hash::bkdrHash(target.c_str()))
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
            throwUnexpectedTaskException("match: " + target);
    }
}

void Command::runNotation() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    if (taskPlan.generalTask.getBit<NotationMethod>().none())
    {
        return;
    }

    const auto [taskCategory, taskType] = getTargetTaskAttribute<NotationMethod>();
    COMMAND_PRINT_TASK_BEGIN_TITLE;

    using algo_notation::NotationSolution;
    const std::shared_ptr<NotationSolution> notation = std::make_shared<NotationSolution>();
    std::shared_ptr<util_thread::Thread> threads = std::make_shared<util_thread::Thread>(std::min(
        static_cast<uint32_t>(taskPlan.generalTask.getBit<NotationMethod>().count()),
        static_cast<uint32_t>(AlgorithmTask::Bottom<NotationMethod>::value)));
    const auto notationFunctor =
        [&](const std::string& threadName, std::string (NotationSolution::*methodPtr)(const std::string&) const)
    {
        threads->enqueue(threadName, methodPtr, notation, std::string{algo_notation::infixNotation});
    };

    for (int i = 0; i < AlgorithmTask::Bottom<NotationMethod>::value; ++i)
    {
        if (!taskPlan.generalTask.getBit<NotationMethod>().test(NotationMethod(i)))
        {
            continue;
        }

        const auto [targetMethod, threadName] = getTargetTaskDetail(taskCategory, taskType, i);
        using util_hash::operator""_bkdrHash;
        switch (util_hash::bkdrHash(targetMethod.data()))
        {
            case "pre"_bkdrHash:
                notationFunctor(threadName, &NotationSolution::prefixMethod);
                break;
            case "pos"_bkdrHash:
                notationFunctor(threadName, &NotationSolution::postfixMethod);
                break;
            default:
                LOG_DBG("execute to run unknown notation method.");
                break;
        }
    }

    COMMAND_PRINT_TASK_END_TITLE;
}

void Command::updateNotationTask(const std::string& target)
{
    using util_hash::operator""_bkdrHash;
    switch (util_hash::bkdrHash(target.c_str()))
    {
        case "pre"_bkdrHash:
            taskPlan.generalTask.setBit<NotationMethod>(NotationMethod::prefix);
            break;
        case "pos"_bkdrHash:
            taskPlan.generalTask.setBit<NotationMethod>(NotationMethod::postfix);
            break;
        default:
            throwUnexpectedTaskException("notation: " + target);
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
    const auto [taskCategory, taskType] = getTargetTaskAttribute<SearchMethod>();
    COMMAND_PRINT_TASK_BEGIN_TITLE;

    using algo_search::SearchSolution;
    const std::shared_ptr<SearchSolution<double>> search = std::make_shared<SearchSolution<double>>(
        algo_search::arrayLength, algo_search::arrayRange1, algo_search::arrayRange2);
    std::shared_ptr<util_thread::Thread> threads = std::make_shared<util_thread::Thread>(std::min(
        static_cast<uint32_t>(taskPlan.generalTask.getBit<SearchMethod>().count()),
        static_cast<uint32_t>(AlgorithmTask::Bottom<SearchMethod>::value)));
    const auto searchFunctor =
        [&](const std::string& threadName,
            int (SearchSolution<double>::*methodPtr)(const double* const, const uint32_t, const double) const)
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

        const auto [targetMethod, threadName] = getTargetTaskDetail(taskCategory, taskType, i);
        using util_hash::operator""_bkdrHash;
        switch (util_hash::bkdrHash(targetMethod.data()))
        {
            case "bin"_bkdrHash:
                searchFunctor(threadName, &SearchSolution<double>::binaryMethod);
                break;
            case "int"_bkdrHash:
                searchFunctor(threadName, &SearchSolution<double>::interpolationMethod);
                break;
            case "fib"_bkdrHash:
                searchFunctor(threadName, &SearchSolution<double>::fibonacciMethod);
                break;
            default:
                LOG_DBG("execute to run unknown search method.");
                break;
        }
    }

    COMMAND_PRINT_TASK_END_TITLE;
}

void Command::updateSearchTask(const std::string& target)
{
    using util_hash::operator""_bkdrHash;
    switch (util_hash::bkdrHash(target.c_str()))
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
            throwUnexpectedTaskException("search: " + target);
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
    const auto [taskCategory, taskType] = getTargetTaskAttribute<SortMethod>();
    COMMAND_PRINT_TASK_BEGIN_TITLE;

    using algo_sort::SortSolution;
    const std::shared_ptr<SortSolution<int>> sort =
        std::make_shared<SortSolution<int>>(algo_sort::arrayLength, algo_sort::arrayRange1, algo_sort::arrayRange2);
    std::shared_ptr<util_thread::Thread> threads = std::make_shared<util_thread::Thread>(std::min(
        static_cast<uint32_t>(taskPlan.generalTask.getBit<SortMethod>().count()),
        static_cast<uint32_t>(AlgorithmTask::Bottom<SortMethod>::value)));
    const auto sortFunctor = [&](const std::string& threadName,
                                 std::vector<int> (SortSolution<int>::*methodPtr)(int* const, const uint32_t) const)
    {
        threads->enqueue(threadName, methodPtr, sort, sort->getRandomArray().get(), sort->getLength());
    };

    for (int i = 0; i < AlgorithmTask::Bottom<SortMethod>::value; ++i)
    {
        if (!taskPlan.generalTask.getBit<SortMethod>().test(SortMethod(i)))
        {
            continue;
        }

        const auto [targetMethod, threadName] = getTargetTaskDetail(taskCategory, taskType, i);
        using util_hash::operator""_bkdrHash;
        switch (util_hash::bkdrHash(targetMethod.data()))
        {
            case "bub"_bkdrHash:
                sortFunctor(threadName, &SortSolution<int>::bubbleMethod);
                break;
            case "sel"_bkdrHash:
                sortFunctor(threadName, &SortSolution<int>::selectionMethod);
                break;
            case "ins"_bkdrHash:
                sortFunctor(threadName, &SortSolution<int>::insertionMethod);
                break;
            case "she"_bkdrHash:
                sortFunctor(threadName, &SortSolution<int>::shellMethod);
                break;
            case "mer"_bkdrHash:
                sortFunctor(threadName, &SortSolution<int>::mergeMethod);
                break;
            case "qui"_bkdrHash:
                sortFunctor(threadName, &SortSolution<int>::quickMethod);
                break;
            case "hea"_bkdrHash:
                sortFunctor(threadName, &SortSolution<int>::heapMethod);
                break;
            case "cou"_bkdrHash:
                sortFunctor(threadName, &SortSolution<int>::countingMethod);
                break;
            case "buc"_bkdrHash:
                sortFunctor(threadName, &SortSolution<int>::bucketMethod);
                break;
            case "rad"_bkdrHash:
                sortFunctor(threadName, &SortSolution<int>::radixMethod);
                break;
            default:
                LOG_DBG("execute to run unknown sort method.");
                break;
        }
    }

    COMMAND_PRINT_TASK_END_TITLE;
}

void Command::updateSortTask(const std::string& target)
{
    using util_hash::operator""_bkdrHash;
    switch (util_hash::bkdrHash(target.c_str()))
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
            throwUnexpectedTaskException("sort: " + target);
    }
}

void Command::runBehavioral() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    if (taskPlan.generalTask.getBit<BehavioralMethod>().none())
    {
        return;
    }

    const auto [taskCategory, taskType] = getTargetTaskAttribute<BehavioralMethod>();
    COMMAND_PRINT_TASK_BEGIN_TITLE;

    using dp_behavioral::BehavioralPattern;
    const std::shared_ptr<BehavioralPattern> behavioral = std::make_shared<BehavioralPattern>();
    std::shared_ptr<util_thread::Thread> threads = std::make_shared<util_thread::Thread>(std::min(
        static_cast<uint32_t>(taskPlan.generalTask.getBit<BehavioralMethod>().count()),
        static_cast<uint32_t>(DesignPatternTask::Bottom<BehavioralMethod>::value)));
    const auto behavioralFunctor = [&](const std::string& threadName, void (BehavioralPattern::*instancePtr)() const)
    {
        threads->enqueue(threadName, instancePtr, behavioral);
    };

    for (int i = 0; i < DesignPatternTask::Bottom<BehavioralMethod>::value; ++i)
    {
        if (!taskPlan.generalTask.getBit<BehavioralMethod>().test(BehavioralMethod(i)))
        {
            continue;
        }

        const auto [targetInstance, threadName] = getTargetTaskDetail(taskCategory, taskType, i);
        using util_hash::operator""_bkdrHash;
        switch (util_hash::bkdrHash(targetInstance.data()))
        {
            case "cha"_bkdrHash:
                behavioralFunctor(threadName, &BehavioralPattern::chainOfResponsibilityInstance);
                break;
            case "com"_bkdrHash:
                behavioralFunctor(threadName, &BehavioralPattern::commandInstance);
                break;
            case "int"_bkdrHash:
                behavioralFunctor(threadName, &BehavioralPattern::interpreterInstance);
                break;
            case "ite"_bkdrHash:
                behavioralFunctor(threadName, &BehavioralPattern::iteratorInstance);
                break;
            case "med"_bkdrHash:
                behavioralFunctor(threadName, &BehavioralPattern::mediatorInstance);
                break;
            case "mem"_bkdrHash:
                behavioralFunctor(threadName, &BehavioralPattern::mementoInstance);
                break;
            case "obs"_bkdrHash:
                behavioralFunctor(threadName, &BehavioralPattern::observerInstance);
                break;
            case "sta"_bkdrHash:
                behavioralFunctor(threadName, &BehavioralPattern::stateInstance);
                break;
            case "str"_bkdrHash:
                behavioralFunctor(threadName, &BehavioralPattern::strategyInstance);
                break;
            case "tem"_bkdrHash:
                behavioralFunctor(threadName, &BehavioralPattern::templateMethodInstance);
                break;
            case "vis"_bkdrHash:
                behavioralFunctor(threadName, &BehavioralPattern::visitorInstance);
                break;
            default:
                LOG_DBG("execute to run unknown behavioral instance.");
                break;
        }
    }

    COMMAND_PRINT_TASK_END_TITLE;
}

void Command::runLinear() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    if (taskPlan.generalTask.getBit<LinearMethod>().none())
    {
        return;
    }

    const auto [taskCategory, taskType] = getTargetTaskAttribute<LinearMethod>();
    COMMAND_PRINT_TASK_BEGIN_TITLE;

    using ds_linear::LinearStructure;
    const std::shared_ptr<LinearStructure> linear = std::make_shared<LinearStructure>();
    std::shared_ptr<util_thread::Thread> threads = std::make_shared<util_thread::Thread>(std::min(
        static_cast<uint32_t>(taskPlan.generalTask.getBit<LinearMethod>().count()),
        static_cast<uint32_t>(DataStructureTask::Bottom<LinearMethod>::value)));
    const auto linearFunctor = [&](const std::string& threadName, void (LinearStructure::*instancePtr)() const)
    {
        threads->enqueue(threadName, instancePtr, linear);
    };

    for (int i = 0; i < DataStructureTask::Bottom<LinearMethod>::value; ++i)
    {
        if (!taskPlan.generalTask.getBit<LinearMethod>().test(LinearMethod(i)))
        {
            continue;
        }

        const auto [targetInstance, threadName] = getTargetTaskDetail(taskCategory, taskType, i);
        using util_hash::operator""_bkdrHash;
        switch (util_hash::bkdrHash(targetInstance.data()))
        {
            case "lin"_bkdrHash:
                linearFunctor(threadName, &LinearStructure::linkedListInstance);
                break;
            case "sta"_bkdrHash:
                linearFunctor(threadName, &LinearStructure::stackInstance);
                break;
            case "que"_bkdrHash:
                linearFunctor(threadName, &LinearStructure::queueInstance);
                break;
            default:
                LOG_DBG("execute to run unknown linear instance.");
                break;
        }
    }

    COMMAND_PRINT_TASK_END_TITLE;
}

void Command::updateLinearTask(const std::string& target)
{
    using util_hash::operator""_bkdrHash;
    switch (util_hash::bkdrHash(target.c_str()))
    {
        case "lin"_bkdrHash:
            taskPlan.generalTask.setBit<LinearMethod>(LinearMethod::linkedList);
            break;
        case "sta"_bkdrHash:
            taskPlan.generalTask.setBit<LinearMethod>(LinearMethod::stack);
            break;
        case "que"_bkdrHash:
            taskPlan.generalTask.setBit<LinearMethod>(LinearMethod::queue);
            break;
        default:
            throwUnexpectedTaskException("linear: " + target);
    }
}

void Command::updateBehavioralTask(const std::string& target)
{
    using util_hash::operator""_bkdrHash;
    switch (util_hash::bkdrHash(target.c_str()))
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
            throwUnexpectedTaskException("behavioral: " + target);
    }
}

void Command::runCreational() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    if (taskPlan.generalTask.getBit<CreationalMethod>().none())
    {
        return;
    }

    const auto [taskCategory, taskType] = getTargetTaskAttribute<CreationalMethod>();
    COMMAND_PRINT_TASK_BEGIN_TITLE;

    using dp_creational::CreationalPattern;
    const std::shared_ptr<CreationalPattern> creational = std::make_shared<CreationalPattern>();
    std::shared_ptr<util_thread::Thread> threads = std::make_shared<util_thread::Thread>(std::min(
        static_cast<uint32_t>(taskPlan.generalTask.getBit<CreationalMethod>().count()),
        static_cast<uint32_t>(DesignPatternTask::Bottom<CreationalMethod>::value)));
    const auto creationalFunctor = [&](const std::string& threadName, void (CreationalPattern::*instancePtr)() const)
    {
        threads->enqueue(threadName, instancePtr, creational);
    };

    for (int i = 0; i < DesignPatternTask::Bottom<CreationalMethod>::value; ++i)
    {
        if (!taskPlan.generalTask.getBit<CreationalMethod>().test(CreationalMethod(i)))
        {
            continue;
        }

        const auto [targetInstance, threadName] = getTargetTaskDetail(taskCategory, taskType, i);
        using util_hash::operator""_bkdrHash;
        switch (util_hash::bkdrHash(targetInstance.data()))
        {
            case "abs"_bkdrHash:
                creationalFunctor(threadName, &CreationalPattern::abstractFactoryInstance);
                break;
            case "bui"_bkdrHash:
                creationalFunctor(threadName, &CreationalPattern::builderInstance);
                break;
            case "fac"_bkdrHash:
                creationalFunctor(threadName, &CreationalPattern::factoryMethodInstance);
                break;
            case "pro"_bkdrHash:
                creationalFunctor(threadName, &CreationalPattern::prototypeInstance);
                break;
            case "sin"_bkdrHash:
                creationalFunctor(threadName, &CreationalPattern::singletonInstance);
                break;
            default:
                LOG_DBG("execute to run unknown creational instance.");
                break;
        }
    }

    COMMAND_PRINT_TASK_END_TITLE;
}

void Command::updateCreationalTask(const std::string& target)
{
    using util_hash::operator""_bkdrHash;
    switch (util_hash::bkdrHash(target.c_str()))
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
            throwUnexpectedTaskException("creational: " + target);
    }
}

void Command::runStructural() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    if (taskPlan.generalTask.getBit<StructuralMethod>().none())
    {
        return;
    }

    const auto [taskCategory, taskType] = getTargetTaskAttribute<StructuralMethod>();
    COMMAND_PRINT_TASK_BEGIN_TITLE;

    using dp_structural::StructuralPattern;
    const std::shared_ptr<StructuralPattern> structural = std::make_shared<StructuralPattern>();
    std::shared_ptr<util_thread::Thread> threads = std::make_shared<util_thread::Thread>(std::min(
        static_cast<uint32_t>(taskPlan.generalTask.getBit<StructuralMethod>().count()),
        static_cast<uint32_t>(DesignPatternTask::Bottom<StructuralMethod>::value)));
    const auto structuralFunctor = [&](const std::string& threadName, void (StructuralPattern::*instancePtr)() const)
    {
        threads->enqueue(threadName, instancePtr, structural);
    };

    for (int i = 0; i < DesignPatternTask::Bottom<StructuralMethod>::value; ++i)
    {
        if (!taskPlan.generalTask.getBit<StructuralMethod>().test(StructuralMethod(i)))
        {
            continue;
        }

        const auto [targetInstance, threadName] = getTargetTaskDetail(taskCategory, taskType, i);
        using util_hash::operator""_bkdrHash;
        switch (util_hash::bkdrHash(targetInstance.data()))
        {
            case "ada"_bkdrHash:
                structuralFunctor(threadName, &StructuralPattern::adapterInstance);
                break;
            case "bri"_bkdrHash:
                structuralFunctor(threadName, &StructuralPattern::bridgeInstance);
                break;
            case "com"_bkdrHash:
                structuralFunctor(threadName, &StructuralPattern::compositeInstance);
                break;
            case "dec"_bkdrHash:
                structuralFunctor(threadName, &StructuralPattern::decoratorInstance);
                break;
            case "fac"_bkdrHash:
                structuralFunctor(threadName, &StructuralPattern::facadeInstance);
                break;
            case "fly"_bkdrHash:
                structuralFunctor(threadName, &StructuralPattern::flyweightInstance);
                break;
            case "pro"_bkdrHash:
                structuralFunctor(threadName, &StructuralPattern::proxyInstance);
                break;
            default:
                LOG_DBG("execute to run unknown structural instance.");
                break;
        }
    }

    COMMAND_PRINT_TASK_END_TITLE;
}

void Command::updateStructuralTask(const std::string& target)
{
    using util_hash::operator""_bkdrHash;
    switch (util_hash::bkdrHash(target.c_str()))
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
            throwUnexpectedTaskException("structural: " + target);
    }
}

void Command::runArithmetic() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    if (taskPlan.generalTask.getBit<ArithmeticMethod>().none())
    {
        return;
    }

    const auto [taskCategory, taskType] = getTargetTaskAttribute<ArithmeticMethod>();
    COMMAND_PRINT_TASK_BEGIN_TITLE;

    using num_arithmetic::ArithmeticSolution;
    const std::shared_ptr<ArithmeticSolution> arithmetic = std::make_shared<ArithmeticSolution>();
    std::shared_ptr<util_thread::Thread> threads = std::make_shared<util_thread::Thread>(std::min(
        static_cast<uint32_t>(taskPlan.generalTask.getBit<ArithmeticMethod>().count()),
        static_cast<uint32_t>(NumericTask::Bottom<ArithmeticMethod>::value)));
    const auto arithmeticFunctor =
        [&](const std::string& threadName, int (ArithmeticSolution::*methodPtr)(const int, const int) const)
    {
        threads->enqueue(threadName, methodPtr, arithmetic, num_arithmetic::integer1, num_arithmetic::integer2);
    };

    for (int i = 0; i < NumericTask::Bottom<ArithmeticMethod>::value; ++i)
    {
        if (!taskPlan.generalTask.getBit<ArithmeticMethod>().test(ArithmeticMethod(i)))
        {
            continue;
        }

        const auto [targetMethod, threadName] = getTargetTaskDetail(taskCategory, taskType, i);
        using util_hash::operator""_bkdrHash;
        switch (util_hash::bkdrHash(targetMethod.data()))
        {
            case "add"_bkdrHash:
                arithmeticFunctor(threadName, &ArithmeticSolution::additionMethod);
                break;
            case "sub"_bkdrHash:
                arithmeticFunctor(threadName, &ArithmeticSolution::subtractionMethod);
                break;
            case "mul"_bkdrHash:
                arithmeticFunctor(threadName, &ArithmeticSolution::multiplicationMethod);
                break;
            case "div"_bkdrHash:
                arithmeticFunctor(threadName, &ArithmeticSolution::divisionMethod);
                break;
            default:
                LOG_DBG("execute to run unknown arithmetic method.");
                break;
        }
    }

    COMMAND_PRINT_TASK_END_TITLE;
}

void Command::updateArithmeticTask(const std::string& target)
{
    using util_hash::operator""_bkdrHash;
    switch (util_hash::bkdrHash(target.c_str()))
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
            throwUnexpectedTaskException("arithmetic: " + target);
    }
}

void Command::runDivisor() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    if (taskPlan.generalTask.getBit<DivisorMethod>().none())
    {
        return;
    }

    const auto [taskCategory, taskType] = getTargetTaskAttribute<DivisorMethod>();
    COMMAND_PRINT_TASK_BEGIN_TITLE;

    using num_divisor::DivisorSolution;
    const std::shared_ptr<DivisorSolution> divisor = std::make_shared<DivisorSolution>();
    std::shared_ptr<util_thread::Thread> threads = std::make_shared<util_thread::Thread>(std::min(
        static_cast<uint32_t>(taskPlan.generalTask.getBit<DivisorMethod>().count()),
        static_cast<uint32_t>(NumericTask::Bottom<DivisorMethod>::value)));
    const auto divisorFunctor =
        [&](const std::string& threadName, std::vector<int> (DivisorSolution::*methodPtr)(int, int) const)
    {
        threads->enqueue(threadName, methodPtr, divisor, num_divisor::integer1, num_divisor::integer2);
    };

    for (int i = 0; i < NumericTask::Bottom<DivisorMethod>::value; ++i)
    {
        if (!taskPlan.generalTask.getBit<DivisorMethod>().test(DivisorMethod(i)))
        {
            continue;
        }

        const auto [targetMethod, threadName] = getTargetTaskDetail(taskCategory, taskType, i);
        using util_hash::operator""_bkdrHash;
        switch (util_hash::bkdrHash(targetMethod.data()))
        {
            case "euc"_bkdrHash:
                divisorFunctor(threadName, &DivisorSolution::euclideanMethod);
                break;
            case "ste"_bkdrHash:
                divisorFunctor(threadName, &DivisorSolution::steinMethod);
                break;
            default:
                LOG_DBG("execute to run unknown divisor method.");
                break;
        }
    }

    COMMAND_PRINT_TASK_END_TITLE;
}

void Command::updateDivisorTask(const std::string& target)
{
    using util_hash::operator""_bkdrHash;
    switch (util_hash::bkdrHash(target.c_str()))
    {
        case "euc"_bkdrHash:
            taskPlan.generalTask.setBit<DivisorMethod>(DivisorMethod::euclidean);
            break;
        case "ste"_bkdrHash:
            taskPlan.generalTask.setBit<DivisorMethod>(DivisorMethod::stein);
            break;
        default:
            throwUnexpectedTaskException("divisor: " + target);
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
            [&](const std::string& threadName, const std::shared_ptr<num_integral::IntegralSolution>& classPtr)
        {
            threads->enqueue(
                threadName,
                &num_integral::IntegralSolution::operator(),
                classPtr,
                range.range1,
                range.range2,
                num_integral::epsilon);
        };

        const auto [taskCategory, taskType] = getTargetTaskAttribute<IntegralMethod>();
        for (int i = 0; i < NumericTask::Bottom<IntegralMethod>::value; ++i)
        {
            if (!taskPlan.generalTask.getBit<IntegralMethod>().test(IntegralMethod(i)))
            {
                continue;
            }

            const auto [targetMethod, threadName] = getTargetTaskDetail(taskCategory, taskType, i);
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

    const auto [taskCategory, taskType] = getTargetTaskAttribute<IntegralMethod>();
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

void Command::updateIntegralTask(const std::string& target)
{
    using util_hash::operator""_bkdrHash;
    switch (util_hash::bkdrHash(target.c_str()))
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
            throwUnexpectedTaskException("integral: " + target);
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
            [&](const std::string& threadName, const std::shared_ptr<num_optimal::OptimalSolution>& classPtr)
        {
            threads->enqueue(
                threadName,
                &num_optimal::OptimalSolution::operator(),
                classPtr,
                range.range1,
                range.range2,
                num_optimal::epsilon);
        };

        const auto [taskCategory, taskType] = getTargetTaskAttribute<OptimalMethod>();
        for (int i = 0; i < NumericTask::Bottom<OptimalMethod>::value; ++i)
        {
            if (!taskPlan.generalTask.getBit<OptimalMethod>().test(OptimalMethod(i)))
            {
                continue;
            }

            const auto [targetMethod, threadName] = getTargetTaskDetail(taskCategory, taskType, i);
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

    const auto [taskCategory, taskType] = getTargetTaskAttribute<OptimalMethod>();
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

void Command::updateOptimalTask(const std::string& target)
{
    using util_hash::operator""_bkdrHash;
    switch (util_hash::bkdrHash(target.c_str()))
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
            throwUnexpectedTaskException("optimal: " + target);
    }
}

void Command::runPrime() const
{
    std::unique_lock<std::mutex> lock(commandMutex);
    if (taskPlan.generalTask.getBit<PrimeMethod>().none())
    {
        return;
    }

    const auto [taskCategory, taskType] = getTargetTaskAttribute<PrimeMethod>();
    COMMAND_PRINT_TASK_BEGIN_TITLE;

    using num_prime::PrimeSolution;
    const std::shared_ptr<PrimeSolution> prime = std::make_shared<PrimeSolution>();
    std::shared_ptr<util_thread::Thread> threads = std::make_shared<util_thread::Thread>(std::min(
        static_cast<uint32_t>(taskPlan.generalTask.getBit<PrimeMethod>().count()),
        static_cast<uint32_t>(NumericTask::Bottom<PrimeMethod>::value)));
    const auto primeFunctor =
        [&](const std::string& threadName, std::vector<uint32_t> (PrimeSolution::*methodPtr)(const uint32_t) const)
    {
        threads->enqueue(threadName, methodPtr, prime, num_prime::maxPositiveInteger);
    };

    for (int i = 0; i < NumericTask::Bottom<PrimeMethod>::value; ++i)
    {
        if (!taskPlan.generalTask.getBit<PrimeMethod>().test(PrimeMethod(i)))
        {
            continue;
        }

        const auto [targetMethod, threadName] = getTargetTaskDetail(taskCategory, taskType, i);
        using util_hash::operator""_bkdrHash;
        switch (util_hash::bkdrHash(targetMethod.data()))
        {
            case "era"_bkdrHash:
                primeFunctor(threadName, &PrimeSolution::eratosthenesMethod);
                break;
            case "eul"_bkdrHash:
                primeFunctor(threadName, &PrimeSolution::eulerMethod);
                break;
            default:
                LOG_DBG("execute to run unknown prime method.");
                break;
        }
    }

    COMMAND_PRINT_TASK_END_TITLE;
}

void Command::updatePrimeTask(const std::string& target)
{
    using util_hash::operator""_bkdrHash;
    switch (util_hash::bkdrHash(target.c_str()))
    {
        case "era"_bkdrHash:
            taskPlan.generalTask.setBit<PrimeMethod>(PrimeMethod::eratosthenes);
            break;
        case "eul"_bkdrHash:
            taskPlan.generalTask.setBit<PrimeMethod>(PrimeMethod::euler);
            break;
        default:
            throwUnexpectedTaskException("prime: " + target);
    }
}
