//! @file command.cpp
//! @author ryftchen
//! @brief The definitions (command) in the application module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023
#include "command.hpp"
#include <unistd.h>
#include <climits>
#include "utility/include/log.hpp"

namespace application::command
{
Command::Command()
{
    try
    {
        program.addArgument("-h", "--help").nArgs(0).implicitValue(true).help("show help and exit");

        program.addArgument("-v", "--version").nArgs(0).implicitValue(true).help("show version and exit");

        program.addArgument("-c", "--console")
            .nArgs(utility::argument::NArgsPattern::any)
            .defaultValue<std::vector<std::string>>({"help"})
            .appending()
            .help("run commands (with quotes) in console mode and exit");

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
            .help("select: match, notation, optimal, search, sort [add a category with --help for task details]");

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
            .help("select: linear, tree [add a category with --help for task details]");

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
            .help("select: behavioral, creational, structural [add a category with --help for task details]");

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
            .help("select: arithmetic, divisor, integral, prime [add a category with --help for task details]");

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
        std::shared_ptr<utility::thread::Thread> thread = std::make_shared<utility::thread::Thread>(2);
        thread->enqueue("commander_fore", &Command::foregroundHandler, this, argc, argv);
        thread->enqueue("commander_back", &Command::backgroundHandler, this);
    }
    else
    {
        LOG_INF("Enter console mode.");
        enterConsoleMode();
    }

    LOG_TO_STOP;
}

void Command::foregroundHandler(const int argc, const char* const argv[])
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
        utility::time::millisecondLevelSleep(1);
        lock.lock();
    }
    catch (const std::exception& error)
    {
        isParsed.store(true);
        LOG_WRN(error.what());
    }
}

void Command::backgroundHandler()
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

        assignedTask.basicTask.primaryBit.set(BasicTask::Category(i));
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
                (*get<UpdateTaskFunctor>(get<TaskFunctorTuple>(taskTypeTuple)))(task);
            }
            isToBeExcess = true;
        }
    }
}

bool Command::checkTask() const
{
    return !assignedTask.empty();
}

void Command::performTask() const
{
    if (!assignedTask.basicTask.empty())
    {
        for (int i = 0; i < BasicTask::Bottom<BasicTask::Category>::value; ++i)
        {
            if (assignedTask.basicTask.primaryBit.test(BasicTask::Category(i)))
            {
                (this->*std::next(basicTaskMap.cbegin(), BasicTask::Category(i))->second)();
            }
        }
    }
    else if (!assignedTask.generalTask.empty() && !assignedTask.basicTask.primaryBit.test(BasicTask::Category::help))
    {
        for (int i = 0; i < GeneralTask::Bottom<GeneralTask::Category>::value; ++i)
        {
            switch (GeneralTask::Category(i))
            {
                case GeneralTask::Category::algorithm:
                    if (app_algo::getTask().empty())
                    {
                        continue;
                    }
                    break;
                case GeneralTask::Category::dataStructure:
                    if (app_ds::getTask().empty())
                    {
                        continue;
                    }
                    break;
                case GeneralTask::Category::designPattern:
                    if (app_dp::getTask().empty())
                    {
                        continue;
                    }
                    break;
                case GeneralTask::Category::numeric:
                    if (app_num::getTask().empty())
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
                (*get<PerformTaskFunctor>(get<TaskFunctorTuple>(taskTypeTuple)))(get<TargetTaskVector>(taskTypeTuple));
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

    utility::console::Console console(" > ");
    registerOnConsole(console);
    for (const auto& command : commands)
    {
        console.commandExecutor(command);
    }
}

void Command::printHelpMessage() const
{
    if (assignedTask.generalTask.empty())
    {
        std::cout << program.help().str();
        return;
    }

    if (!app_algo::getTask().empty())
    {
        using app_algo::AlgorithmTask;
        using app_algo::getBit;
        std::cout << "Usage: foo -a, --algorithm ";
        if (!getBit<AlgorithmTask::MatchMethod>().none())
        {
            std::puts("match [tasks...]\r\n\r\nNon-optional:\r\n"
                      "rab    Rabin-Karp\r\n"
                      "knu    Knuth-Morris-Pratt\r\n"
                      "boy    Boyer-Moore\r\n"
                      "hor    Horspool\r\n"
                      "sun    Sunday");
        }
        else if (!getBit<AlgorithmTask::NotationMethod>().none())
        {
            std::puts("notation [tasks...]\r\n\r\nNon-optional:\r\n"
                      "pre    Prefix\r\n"
                      "pos    Postfix");
        }
        else if (!getBit<AlgorithmTask::OptimalMethod>().none())
        {
            std::puts("optimal [tasks...]\r\n\r\nNon-optional:\r\n"
                      "gra    Gradient Descent\r\n"
                      "ann    Simulated Annealing\r\n"
                      "par    Particle Swarm\r\n"
                      "gen    Genetic");
        }
        else if (!getBit<AlgorithmTask::SearchMethod>().none())
        {
            std::puts("search [tasks...]\r\n\r\nNon-optional:\r\n"
                      "bin    Binary\r\n"
                      "int    Interpolation\r\n"
                      "fib    Fibonacci");
        }
        else if (!getBit<AlgorithmTask::SortMethod>().none())
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
    else if (!app_ds::getTask().empty())
    {
        using app_ds::DataStructureTask;
        using app_ds::getBit;
        std::cout << "Usage: foo -ds, --data-structure ";
        if (!getBit<DataStructureTask::LinearInstance>().none())
        {
            std::puts("linear [tasks...]\r\n\r\nNon-optional:\r\n"
                      "lin    Linked List\r\n"
                      "sta    Stack\r\n"
                      "que    Queue");
        }
        else if (!getBit<DataStructureTask::TreeInstance>().none())
        {
            std::puts("tree [tasks...]\r\n\r\nNon-optional:\r\n"
                      "bin    Binary Search\r\n"
                      "ade    Adelson-Velsky-Landis\r\n"
                      "spl    Splay");
        }
    }
    else if (!app_dp::getTask().empty())
    {
        using app_dp::DesignPatternTask;
        using app_dp::getBit;
        std::cout << "Usage: foo -dp, --design-pattern ";
        if (!getBit<DesignPatternTask::BehavioralInstance>().none())
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
        else if (!getBit<DesignPatternTask::CreationalInstance>().none())
        {
            std::puts("creational [tasks...]\r\n\r\nNon-optional:\r\n"
                      "abs    Abstract Factory\r\n"
                      "bui    Builder\r\n"
                      "fac    Factory Method\r\n"
                      "pro    Prototype\r\n"
                      "sin    Singleton");
        }
        else if (!getBit<DesignPatternTask::StructuralInstance>().none())
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
    else if (!app_num::getTask().empty())
    {
        using app_num::getBit;
        using app_num::NumericTask;
        std::cout << "Usage: foo -n, --numeric ";
        if (!getBit<NumericTask::ArithmeticMethod>().none())
        {
            std::puts("arithmetic [tasks...]\r\n\r\nNon-optional:\r\n"
                      "add    Addition\r\n"
                      "sub    Subtraction\r\n"
                      "mul    Multiplication\r\n"
                      "div    Division");
        }
        else if (!getBit<NumericTask::DivisorMethod>().none())
        {
            std::puts("divisor [tasks...]\r\n\r\nNon-optional:\r\n"
                      "euc    Euclidean\r\n"
                      "ste    Stein");
        }
        else if (!getBit<NumericTask::IntegralMethod>().none())
        {
            std::puts("integral [tasks...]\r\n\r\nNon-optional:\r\n"
                      "tra    Trapezoidal\r\n"
                      "sim    Adaptive Simpson's 1/3\r\n"
                      "rom    Romberg\r\n"
                      "gau    Gauss-Legendre's 5-Points\r\n"
                      "mon    Monte-Carlo");
        }
        else if (!getBit<NumericTask::PrimeMethod>().none())
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

    utility::common::executeCommand(versionStr.c_str());
}

void Command::enterConsoleMode() const
{
    char hostName[HOST_NAME_MAX + 1];
    if (gethostname(hostName, HOST_NAME_MAX + 1))
    {
        throw std::runtime_error("Host name could not be obtained.");
    }
    const std::string greeting = std::string{(nullptr != std::getenv("USER")) ? std::getenv("USER") : "root"} + "@"
        + std::string{hostName} + " foo > ";

    utility::common::executeCommand(("tput bel; echo " + getIconBanner()).c_str());
    utility::console::Console console(greeting);
    registerOnConsole(console);
    int returnCode = 0;
    do
    {
        returnCode = console.readCommandLine();
        console.setGreeting(greeting);
    }
    while (utility::console::Console::ReturnCode::quit != returnCode);
}

void Command::registerOnConsole(utility::console::Console& console) const
{
    console.registerCommand(
        "log",
        [this](const std::vector<std::string>& /*unused*/) -> decltype(auto)
        {
            viewLogContent();
            return utility::console::Console::ReturnCode::success;
        },
        "view the log with highlights");
}

void Command::viewLogContent()
{
    LOG_TO_STOP;

    utility::common::printFile(
        utility::log::logPath.data(), true, maxLineNumForPrintLog, &utility::log::changeToLogStyle);
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

//! @brief Get memory pool when making multi-threading.
//! @return reference of the Memory object
utility::memory::Memory<utility::thread::Thread>& getMemoryForMultithreading()
{
    static utility::memory::Memory<utility::thread::Thread> memory;
    return memory;
}
} // namespace application::command
