//! @file command.cpp
//! @author ryftchen
//! @brief The definitions (command) in the application module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#include "command.hpp"
#ifndef __PRECOMPILED_HEADER
#include <unistd.h>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER
#include "log.hpp"
#include "view.hpp"

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
                            generalTaskDispatcher.at("algorithm").cbegin(),
                            generalTaskDispatcher.at("algorithm").cend(),
                            [&value](const auto& taskCategoryMap)
                            {
                                return (taskCategoryMap.first == value);
                            }))
                    {
                        return value;
                    }
                    throw std::runtime_error("<COMMAND> Unknown algorithm category: " + value);
                })
            .help("run algorithm tasks with a category:\n"
                  "- match       Match Method\n"
                  "- notation    Notation Method\n"
                  "- optimal     Optimal Method\n"
                  "- search      Search Method\n"
                  "- sort        Sort Method");

        program.addArgument("-ds", "--data-structure")
            .nArgs(1)
            .action(
                [this](const std::string& value)
                {
                    if (std::any_of(
                            generalTaskDispatcher.at("data-structure").cbegin(),
                            generalTaskDispatcher.at("data-structure").cend(),
                            [&value](const auto& taskCategoryMap)
                            {
                                return (taskCategoryMap.first == value);
                            }))
                    {
                        return value;
                    }
                    throw std::runtime_error("<COMMAND> Unknown data structure category: " + value);
                })
            .help("run data structure tasks with a category:\n"
                  "- linear    Linear Structure\n"
                  "- tree      Tree Structure");

        program.addArgument("-dp", "--design-pattern")
            .nArgs(1)
            .action(
                [this](const std::string& value)
                {
                    if (std::any_of(
                            generalTaskDispatcher.at("design-pattern").cbegin(),
                            generalTaskDispatcher.at("design-pattern").cend(),
                            [&value](const auto& taskCategoryMap)
                            {
                                return (taskCategoryMap.first == value);
                            }))
                    {
                        return value;
                    }
                    throw std::runtime_error("<COMMAND> Unknown design pattern category: " + value);
                })
            .help("run design pattern tasks with a category:\n"
                  "- behavioral    Behavioral Pattern\n"
                  "- creational    Creational Pattern\n"
                  "- structural    Structural Pattern");

        program.addArgument("-n", "--numeric")
            .nArgs(1)
            .action(
                [this](const std::string& value)
                {
                    if (std::any_of(
                            generalTaskDispatcher.at("numeric").cbegin(),
                            generalTaskDispatcher.at("numeric").cend(),
                            [&value](const auto& taskCategoryMap)
                            {
                                return (taskCategoryMap.first == value);
                            }))
                    {
                        return value;
                    }
                    throw std::runtime_error("<COMMAND> Unknown numeric category: " + value);
                })
            .help("run numeric tasks with a category:\n"
                  "- arithmetic    Arithmetic Method\n"
                  "- divisor       Divisor Method\n"
                  "- integral      Integral Method\n"
                  "- prime         Prime Method");

        program.addArgument("tasks").remaining().help("add a category with --help for task details");
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << std::endl;
    }
}

Command::~Command()
{
    try
    {
        dispatchedTask.reset();
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << std::endl;
    }
}

Command& Command::getInstance()
{
    static Command commander;
    return commander;
}

void Command::runCommander(const int argc, const char* const argv[])
{
    try
    {
        LOG_TO_START;
        VIEW_TO_START;

        if (0 != argc - 1)
        {
            std::shared_ptr<utility::thread::Thread> thread = std::make_shared<utility::thread::Thread>(2);
            thread->enqueue("commander_fore", &Command::foregroundHandler, this, argc, argv);
            thread->enqueue("commander_back", &Command::backgroundHandler, this);
        }
        else
        {
            LOG_INF("<COMMAND> Enter console mode.");
            enterConsoleMode();
            LOG_INF("<COMMAND> Exit console mode.");
        }

        LOG_TO_STOP;
        VIEW_TO_STOP;
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << std::endl;
    }
}

void Command::foregroundHandler(const int argc, const char* const argv[])
{
    try
    {
        std::unique_lock<std::mutex> lock(mtx);
        program.parseArgs(argc, argv);
        validateBasicTask();
        validateGeneralTask();

        isParsed.store(true);
        lock.unlock();
        cv.notify_one();
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
        if (std::unique_lock<std::mutex> lock(mtx); true)
        {
            cv.wait(
                lock,
                [this]() -> decltype(auto)
                {
                    return isParsed.load();
                });
        }

        if (checkTask())
        {
            dispatchTask();
        }
    }
    catch (const std::exception& error)
    {
        LOG_WRN(error.what());
    }
}

void Command::validateBasicTask()
{
    for (std::uint8_t i = 0; i < BasicTask::Bottom<BasicTask::Category>::value; ++i)
    {
        if (!program.isUsed(std::next(basicTaskDispatcher.cbegin(), BasicTask::Category(i))->first))
        {
            continue;
        }

        if (checkTask())
        {
            throwExcessArgumentException();
        }

        dispatchedTask.basicTask.primaryBit.set(BasicTask::Category(i));
    }
}

void Command::validateGeneralTask()
{
    bool isToBeExcess = false;
    for ([[maybe_unused]] const auto& [taskCategoryName, taskCategoryMap] : generalTaskDispatcher)
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
                            return (std::string::npos == task.find_first_not_of(' '));
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
    return !dispatchedTask.empty();
}

void Command::dispatchTask() const
{
    if (!dispatchedTask.basicTask.empty())
    {
        for (std::uint8_t i = 0; i < BasicTask::Bottom<BasicTask::Category>::value; ++i)
        {
            if (dispatchedTask.basicTask.primaryBit.test(BasicTask::Category(i)))
            {
                (this->*std::next(basicTaskDispatcher.cbegin(), BasicTask::Category(i))->second)();
            }
        }
    }
    else if (
        !dispatchedTask.generalTask.empty() && !dispatchedTask.basicTask.primaryBit.test(BasicTask::Category::help))
    {
        for (std::uint8_t i = 0; i < GeneralTask::Bottom<GeneralTask::Category>::value; ++i)
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
                 std::next(generalTaskDispatcher.cbegin(), GeneralTask::Category(i))->second)
            {
                (*get<PerformTaskFunctor>(get<TaskFunctorTuple>(taskTypeTuple)))(get<TargetTaskVector>(taskTypeTuple));
            }
        }
    }
}

void Command::printConsoleOutput() const
{
    const auto commands = program.get<std::vector<std::string>>(
        std::next(basicTaskDispatcher.cbegin(), BasicTask::Category::console)->first);
    if (commands.empty())
    {
        return;
    }

    using utility::console::Console;
    using utility::socket::UDPSocket;
    using view::View;

    UDPSocket udpClient;
    udpClient.onRawMessageReceived =
        [&](char* buffer, const int length, const std::string& /*ip*/, const std::uint16_t /*port*/)
    {
        try
        {
            if (View::parseTLVPacket(buffer, length).stopFlag)
            {
                udpClient.setNonBlocking();
            }
        }
        catch (std::exception& error)
        {
            LOG_WRN(error.what());
        }
    };
    udpClient.toReceive();
    udpClient.toConnect(std::string{View::udpHost}, View::udpPort);
    utility::time::millisecondLevelSleep(latency);

    Console console(" > ");
    registerOnConsole<UDPSocket>(console, udpClient);
    for (const auto& command : commands)
    {
        console.commandExecutor(command);
    }

    udpClient.toSend("stop");
    udpClient.waitIfAlive();
    udpClient.toClose();
}

void Command::printHelpMessage() const
{
    if (dispatchedTask.generalTask.empty())
    {
        std::cout << program.help().str();
        return;
    }

    if (!app_algo::getTask().empty())
    {
        using app_algo::AlgorithmTask;
        using app_algo::getBit;
        std::printf("Usage: foo -a, --algorithm ");
        if (!getBit<AlgorithmTask::MatchMethod>().none())
        {
            std::puts("match [tasks...]\n"
                      "\r\nNon-optional:\n"
                      "rab    Rabin-Karp\n"
                      "knu    Knuth-Morris-Pratt\n"
                      "boy    Boyer-Moore\n"
                      "hor    Horspool\n"
                      "sun    Sunday");
        }
        else if (!getBit<AlgorithmTask::NotationMethod>().none())
        {
            std::puts("notation [tasks...]\n"
                      "\r\nNon-optional:\n"
                      "pre    Prefix\n"
                      "pos    Postfix");
        }
        else if (!getBit<AlgorithmTask::OptimalMethod>().none())
        {
            std::puts("optimal [tasks...]\n"
                      "\r\nNon-optional:\n"
                      "gra    Gradient Descent\n"
                      "ann    Simulated Annealing\n"
                      "par    Particle Swarm\n"
                      "gen    Genetic");
        }
        else if (!getBit<AlgorithmTask::SearchMethod>().none())
        {
            std::puts("search [tasks...]\n"
                      "\r\nNon-optional:\n"
                      "bin    Binary\n"
                      "int    Interpolation\n"
                      "fib    Fibonacci");
        }
        else if (!getBit<AlgorithmTask::SortMethod>().none())
        {
            std::puts("sort [tasks...]\n"
                      "\r\nNon-optional:\n"
                      "bub    Bubble\n"
                      "sel    Selection\n"
                      "ins    Insertion\n"
                      "she    Shell\n"
                      "mer    Merge\n"
                      "qui    Quick\n"
                      "hea    Heap\n"
                      "cou    Counting\n"
                      "buc    Bucket\n"
                      "rad    Radix");
        }
    }
    else if (!app_ds::getTask().empty())
    {
        using app_ds::DataStructureTask;
        using app_ds::getBit;
        std::printf("Usage: foo -ds, --data-structure ");
        if (!getBit<DataStructureTask::LinearInstance>().none())
        {
            std::puts("linear [tasks...]\n"
                      "\r\nNon-optional:\n"
                      "lin    Linked List\n"
                      "sta    Stack\n"
                      "que    Queue");
        }
        else if (!getBit<DataStructureTask::TreeInstance>().none())
        {
            std::puts("tree [tasks...]\n"
                      "\r\nNon-optional:\n"
                      "bin    Binary Search\n"
                      "ade    Adelson-Velsky-Landis\n"
                      "spl    Splay");
        }
    }
    else if (!app_dp::getTask().empty())
    {
        using app_dp::DesignPatternTask;
        using app_dp::getBit;
        std::printf("Usage: foo -dp, --design-pattern ");
        if (!getBit<DesignPatternTask::BehavioralInstance>().none())
        {
            std::puts("behavioral [tasks...]\n"
                      "\r\nNon-optional:\n"
                      "cha    Chain Of Responsibility\n"
                      "com    Command\n"
                      "int    Interpreter\n"
                      "ite    Iterator\n"
                      "med    Mediator\n"
                      "mem    Memento\n"
                      "obs    Observer\n"
                      "sta    State\n"
                      "str    Strategy\n"
                      "tem    Template Method\n"
                      "vis    Visitor");
        }
        else if (!getBit<DesignPatternTask::CreationalInstance>().none())
        {
            std::puts("creational [tasks...]\n"
                      "\r\nNon-optional:\n"
                      "abs    Abstract Factory\n"
                      "bui    Builder\n"
                      "fac    Factory Method\n"
                      "pro    Prototype\n"
                      "sin    Singleton");
        }
        else if (!getBit<DesignPatternTask::StructuralInstance>().none())
        {
            std::puts("structural [tasks...]\n"
                      "\r\nNon-optional:\n"
                      "ada    Adapter\n"
                      "bri    Bridge\n"
                      "com    Composite\n"
                      "dec    Decorator\n"
                      "fac    Facade\n"
                      "fly    Flyweight\n"
                      "pro    Proxy");
        }
    }
    else if (!app_num::getTask().empty())
    {
        using app_num::getBit;
        using app_num::NumericTask;
        std::printf("Usage: foo -n, --numeric ");
        if (!getBit<NumericTask::ArithmeticMethod>().none())
        {
            std::puts("arithmetic [tasks...]\n"
                      "\r\nNon-optional:\n"
                      "add    Addition\n"
                      "sub    Subtraction\n"
                      "mul    Multiplication\n"
                      "div    Division");
        }
        else if (!getBit<NumericTask::DivisorMethod>().none())
        {
            std::puts("divisor [tasks...]\n"
                      "\r\nNon-optional:\n"
                      "euc    Euclidean\n"
                      "ste    Stein");
        }
        else if (!getBit<NumericTask::IntegralMethod>().none())
        {
            std::puts("integral [tasks...]\n"
                      "\r\nNon-optional:\n"
                      "tra    Trapezoidal\n"
                      "sim    Adaptive Simpson's 1/3\n"
                      "rom    Romberg\n"
                      "gau    Gauss-Legendre's 5-Points\n"
                      "mon    Monte-Carlo");
        }
        else if (!getBit<NumericTask::PrimeMethod>().none())
        {
            std::puts("prime [tasks...]\n"
                      "\r\nNon-optional:\n"
                      "era    Eratosthenes\n"
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
    versionStr += " \"; tput sgr0; echo ";
    versionStr += '\"' + std::string{copyrightInfo} + '\"';

    COMMON_PRINT("%s", utility::common::executeCommand(versionStr).c_str());
}

void Command::enterConsoleMode() const
{
    try
    {
        COMMON_PRINT("%s", utility::common::executeCommand(("tput bel; echo " + getIconBanner())).c_str());

        using utility::console::Console;
        using utility::socket::TCPSocket;
        using view::View;

        TCPSocket tcpClient;
        tcpClient.onRawMessageReceived = [&](char* buffer, const int length)
        {
            try
            {
                if (View::parseTLVPacket(buffer, length).stopFlag)
                {
                    tcpClient.setNonBlocking();
                }
            }
            catch (std::exception& error)
            {
                LOG_WRN(error.what());
            }
        };
        tcpClient.toConnect(std::string{View::tcpHost}, View::tcpPort);
        utility::time::millisecondLevelSleep(latency);

        char hostName[HOST_NAME_MAX + 1];
        if (gethostname(hostName, HOST_NAME_MAX + 1))
        {
            throw std::runtime_error("<COMMAND> Host name could not be obtained.");
        }
        const std::string greeting = std::string{(nullptr != std::getenv("USER")) ? std::getenv("USER") : "root"} + '@'
            + std::string{hostName} + " foo > ";
        Console console(greeting);
        registerOnConsole<TCPSocket>(console, tcpClient);

        int retVal = Console::ReturnCode::success;
        do
        {
            retVal = console.readCommandLine();
            console.setGreeting(greeting);
        }
        while (Console::ReturnCode::quit != retVal);

        tcpClient.toSend("stop");
        tcpClient.waitIfAlive();
        tcpClient.toClose();
    }
    catch (const std::exception& error)
    {
        LOG_WRN(error.what());
    }
}

template <typename T>
void Command::registerOnConsole(utility::console::Console& console, T& client) const
{
    using utility::console::Console;
    using view::View;

    for (const auto& [option, optionTuple] : VIEW_OPTIONS)
    {
        const auto& cmd = option;
        const auto& helpInfo = View::get<View::HelpInfo>(optionTuple);
        console.registerCommand(
            cmd,
            [cmd, &client](const std::vector<std::string>& /*unused*/) -> decltype(auto)
            {
                int retVal = Console::ReturnCode::success;
                try
                {
                    client.toSend(cmd);
                    utility::time::millisecondLevelSleep(maxLatency);
                }
                catch (const std::exception& error)
                {
                    retVal = Console::ReturnCode::error;
                    LOG_WRN(error.what());
                }
                return retVal;
            },
            helpInfo);
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

//! @brief Get memory pool when making multi-threading.
//! @return reference of the PublicThreadPool object
PublicThreadPool& getPublicThreadPool()
{
    static PublicThreadPool pool;
    return pool;
}
} // namespace application::command
