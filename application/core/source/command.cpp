//! @file command.cpp
//! @author ryftchen
//! @brief The definitions (command) in the application module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#include "command.hpp"
#include "log.hpp"
#include "view.hpp"

#ifndef __PRECOMPILED_HEADER
#include <fstream>
#include <iterator>
#include <ranges>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

namespace application::command
{
Command::Command()
{
    mainCLI.addArgument("-h", "--help").argsNum(0).implicitVal(true).help("show help and exit");

    mainCLI.addArgument("-v", "--version").argsNum(0).implicitVal(true).help("show version and exit");

    mainCLI.addArgument("-d", "--dump").argsNum(0).implicitVal(true).help("dump default configuration and exit");

    mainCLI.addArgument("-c", "--console")
        .argsNum(utility::argument::ArgsNumPattern::any)
        .defaultVal<std::vector<std::string>>({"help"})
        .appending()
        .action(
            [](const std::string& value)
            {
                if (value.find_first_not_of(' ') != std::string::npos)
                {
                    return value;
                }
                throw std::logic_error("Invalid console command.");
            })
        .metavar("CMD")
        .help("run commands in console mode and exit\n"
              "separate with quotes");

    const auto& algoTbl = regularTaskDispatcher.at(subCLIAppAlgo.title);
    subCLIAppAlgo.addDescription("apply algorithm");
    subCLIAppAlgo.addArgument("-h", "--help").argsNum(0).implicitVal(true).help("show help and exit");
    subCLIAppAlgo.addArgument("-m", "--match")
        .argsNum(0, get<TargetTaskContainer>(algoTbl.at("match")).size())
        .defaultVal<std::vector<std::string>>(TargetTaskContainer{get<TargetTaskContainer>(algoTbl.at("match"))})
        .remaining()
        .metavar("OPT")
        .help("run match tasks\n"
              "- rab    Rabin-Karp\n"
              "- knu    Knuth-Morris-Pratt\n"
              "- boy    Boyer-Moore\n"
              "- hor    Horspool\n"
              "- sun    Sunday\n"
              "add the tasks listed above");
    subCLIAppAlgo.addArgument("-n", "--notation")
        .argsNum(0, get<TargetTaskContainer>(algoTbl.at("notation")).size())
        .defaultVal<std::vector<std::string>>(TargetTaskContainer{get<TargetTaskContainer>(algoTbl.at("notation"))})
        .remaining()
        .metavar("OPT")
        .help("run notation tasks\n"
              "- pre    Prefix\n"
              "- pos    Postfix\n"
              "add the tasks listed above");
    subCLIAppAlgo.addArgument("-o", "--optimal")
        .argsNum(0, get<TargetTaskContainer>(algoTbl.at("optimal")).size())
        .defaultVal<std::vector<std::string>>(TargetTaskContainer{get<TargetTaskContainer>(algoTbl.at("optimal"))})
        .remaining()
        .metavar("OPT")
        .help("run optimal tasks\n"
              "- gra    Gradient Descent\n"
              "- ann    Simulated Annealing\n"
              "- par    Particle Swarm\n"
              "- gen    Genetic\n"
              "add the tasks listed above");
    subCLIAppAlgo.addArgument("-s", "--search")
        .argsNum(0, get<TargetTaskContainer>(algoTbl.at("search")).size())
        .defaultVal<std::vector<std::string>>(TargetTaskContainer{get<TargetTaskContainer>(algoTbl.at("search"))})
        .remaining()
        .metavar("OPT")
        .help("run search tasks\n"
              "- bin    Binary\n"
              "- int    Interpolation\n"
              "- fib    Fibonacci\n"
              "add the tasks listed above");
    subCLIAppAlgo.addArgument("-S", "--sort")
        .argsNum(0, get<TargetTaskContainer>(algoTbl.at("sort")).size())
        .defaultVal<std::vector<std::string>>(TargetTaskContainer{get<TargetTaskContainer>(algoTbl.at("sort"))})
        .remaining()
        .metavar("OPT")
        .help("run sort tasks\n"
              "- bub    Bubble\n"
              "- sel    Selection\n"
              "- ins    Insertion\n"
              "- she    Shell\n"
              "- mer    Merge\n"
              "- qui    Quick\n"
              "- hea    Heap\n"
              "- cou    Counting\n"
              "- buc    Bucket\n"
              "- rad    Radix\n"
              "add the tasks listed above");
    mainCLI.addSubParser(subCLIAppAlgo);

    const auto& dpTbl = regularTaskDispatcher.at(subCLIAppDp.title);
    subCLIAppDp.addDescription("apply design pattern");
    subCLIAppDp.addArgument("-h", "--help").argsNum(0).implicitVal(true).help("show help and exit");
    subCLIAppDp.addArgument("-b", "--behavioral")
        .argsNum(0, get<TargetTaskContainer>(dpTbl.at("behavioral")).size())
        .defaultVal<std::vector<std::string>>(TargetTaskContainer{get<TargetTaskContainer>(dpTbl.at("behavioral"))})
        .remaining()
        .metavar("OPT")
        .help("run behavioral tasks\n"
              "- cha    Chain Of Responsibility\n"
              "- com    Command\n"
              "- int    Interpreter\n"
              "- ite    Iterator\n"
              "- med    Mediator\n"
              "- mem    Memento\n"
              "- obs    Observer\n"
              "- sta    State\n"
              "- str    Strategy\n"
              "- tem    Template Method\n"
              "- vis    Visitor\n"
              "add the tasks listed above");
    subCLIAppDp.addArgument("-c", "--creational")
        .argsNum(0, get<TargetTaskContainer>(dpTbl.at("creational")).size())
        .defaultVal<std::vector<std::string>>(TargetTaskContainer{get<TargetTaskContainer>(dpTbl.at("creational"))})
        .remaining()
        .metavar("OPT")
        .help("run creational tasks\n"
              "- abs    Abstract Factory\n"
              "- bui    Builder\n"
              "- fac    Factory Method\n"
              "- pro    Prototype\n"
              "- sin    Singleton\n"
              "add the tasks listed above");
    subCLIAppDp.addArgument("-s", "--structural")
        .argsNum(0, get<TargetTaskContainer>(dpTbl.at("structural")).size())
        .defaultVal<std::vector<std::string>>(TargetTaskContainer{get<TargetTaskContainer>(dpTbl.at("structural"))})
        .remaining()
        .metavar("OPT")
        .help("run creational tasks\n"
              "- ada    Adapter\n"
              "- bri    Bridge\n"
              "- com    Composite\n"
              "- dec    Decorator\n"
              "- fac    Facade\n"
              "- fly    Flyweight\n"
              "- pro    Proxy\n"
              "add the tasks listed above");
    mainCLI.addSubParser(subCLIAppDp);

    const auto& dsTbl = regularTaskDispatcher.at(subCLIAppDs.title);
    subCLIAppDs.addDescription("apply data structure");
    subCLIAppDs.addArgument("-h", "--help").argsNum(0).implicitVal(true).help("show help and exit");
    subCLIAppDs.addArgument("-l", "--linear")
        .argsNum(0, get<TargetTaskContainer>(dsTbl.at("linear")).size())
        .defaultVal<std::vector<std::string>>(TargetTaskContainer{get<TargetTaskContainer>(dsTbl.at("linear"))})
        .remaining()
        .metavar("OPT")
        .help("run linear tasks\n"
              "- lin    Linked List\n"
              "- sta    Stack\n"
              "- que    Queue\n"
              "add the tasks listed above");
    subCLIAppDs.addArgument("-t", "--tree")
        .argsNum(0, get<TargetTaskContainer>(dsTbl.at("tree")).size())
        .defaultVal<std::vector<std::string>>(TargetTaskContainer{get<TargetTaskContainer>(dsTbl.at("tree"))})
        .remaining()
        .metavar("OPT")
        .help("run tree tasks\n"
              "- bin    Binary Search\n"
              "- ade    Adelson-Velsky-Landis\n"
              "- spl    Splay\n"
              "add the tasks listed above");
    mainCLI.addSubParser(subCLIAppDs);

    const auto& numTbl = regularTaskDispatcher.at(subCLIAppNum.title);
    subCLIAppNum.addDescription("apply numeric");
    subCLIAppNum.addArgument("-h", "--help").argsNum(0).implicitVal(true).help("show help and exit");
    subCLIAppNum.addArgument("-a", "--arithmetic")
        .argsNum(0, get<TargetTaskContainer>(numTbl.at("arithmetic")).size())
        .defaultVal<std::vector<std::string>>(TargetTaskContainer{get<TargetTaskContainer>(numTbl.at("arithmetic"))})
        .remaining()
        .metavar("OPT")
        .help("run arithmetic tasks\n"
              "- add    Addition\n"
              "- sub    Subtraction\n"
              "- mul    Multiplication\n"
              "- div    Division\n"
              "add the tasks listed above");
    subCLIAppNum.addArgument("-d", "--divisor")
        .argsNum(0, get<TargetTaskContainer>(numTbl.at("divisor")).size())
        .defaultVal<std::vector<std::string>>(TargetTaskContainer{get<TargetTaskContainer>(numTbl.at("divisor"))})
        .remaining()
        .metavar("OPT")
        .help("run divisor tasks\n"
              "- euc    Euclidean\n"
              "- ste    Stein\n"
              "add the tasks listed above");
    subCLIAppNum.addArgument("-i", "--integral")
        .argsNum(0, get<TargetTaskContainer>(numTbl.at("integral")).size())
        .defaultVal<std::vector<std::string>>(TargetTaskContainer{get<TargetTaskContainer>(numTbl.at("integral"))})
        .remaining()
        .metavar("OPT")
        .help("run integral tasks\n"
              "- tra    Trapezoidal\n"
              "- sim    Adaptive Simpson's 1/3\n"
              "- rom    Romberg\n"
              "- gau    Gauss-Legendre's 5-Points\n"
              "- mon    Monte-Carlo\n"
              "add the tasks listed above");
    subCLIAppNum.addArgument("-p", "--prime")
        .argsNum(0, get<TargetTaskContainer>(numTbl.at("prime")).size())
        .defaultVal<std::vector<std::string>>(TargetTaskContainer{get<TargetTaskContainer>(numTbl.at("prime"))})
        .remaining()
        .metavar("OPT")
        .help("run prime tasks\n"
              "- era    Eratosthenes\n"
              "- eul    Euler\n"
              "add the tasks listed above");
    mainCLI.addSubParser(subCLIAppNum);
}

Command::~Command()
{
    dispatchedTask.reset();
}

Command& Command::getInstance()
{
    static Command commander{};
    return commander;
}

void Command::runCommander(const int argc, const char* const argv[])
try
{
    triggerHelper<log::Log>(HelperOperation::start);
    triggerHelper<view::View>(HelperOperation::start);

    if (1 == argc)
    {
        enterConsoleMode();
    }
    else
    {
        constexpr std::uint32_t childThdNum = 2;
        auto threads = std::make_shared<utility::thread::Thread>(childThdNum);
        threads->enqueue("commander-fg", &Command::foregroundHandler, this, argc, argv);
        threads->enqueue("commander-bg", &Command::backgroundHandler, this);
    }

    triggerHelper<view::View>(HelperOperation::stop);
    triggerHelper<log::Log>(HelperOperation::stop);
}
catch (const std::exception& error)
{
    LOG_ERR << error.what();
}

void Command::foregroundHandler(const int argc, const char* const argv[])
try
{
    std::unique_lock<std::mutex> lock(mtx);
    mainCLI.parseArgs(argc, argv);
    validateBasicTask();
    validateRegularTask();

    isParsed.store(true);
    lock.unlock();
    cv.notify_one();
}
catch (const std::exception& error)
{
    isParsed.store(true);
    cv.notify_one();
    LOG_WRN << error.what();
}

void Command::backgroundHandler()
try
{
    if (std::unique_lock<std::mutex> lock(mtx); true)
    {
        cv.wait(
            lock,
            [this]()
            {
                return isParsed.load();
            });
    }

    if (hasAnyTask())
    {
        dispatchTask();
    }
}
catch (const std::exception& error)
{
    LOG_WRN << error.what();
}

void Command::validateBasicTask()
{
    for (std::uint8_t i = 0; i < Bottom<Category>::value; ++i)
    {
        if (!mainCLI.isUsed(std::next(basicTaskDispatcher.cbegin(), Category(i))->first))
        {
            continue;
        }
        checkForExcessiveArguments();

        dispatchedTask.basicTask.primaryBit.set(Category(i));
    }
}

void Command::validateRegularTask()
{
    const auto isSubCLIUsed = [this](const auto& subCLIPair)
    {
        if (mainCLI.isSubCommandUsed(subCLIPair.first))
        {
            checkForExcessiveArguments();
            return true;
        }
        return false;
    };

    for ([[maybe_unused]] const auto& [subCLIName, subCLIMap] :
         regularTaskDispatcher | std::views::filter(isSubCLIUsed))
    {
        const auto& subCLI = mainCLI.at<utility::argument::Argument>(subCLIName);
        if (!subCLI)
        {
            dispatchedTask.regularTask.helpOnly = true;
            return;
        }

        if (subCLI.isUsed("help"))
        {
            dispatchedTask.regularTask.helpOnly = true;
        }
        const auto isCategoryUsed = [this, subCLI](const auto& categoryPair)
        {
            if (subCLI.isUsed(categoryPair.first))
            {
                checkForExcessiveArguments();
                return true;
            }
            return false;
        };

        for ([[maybe_unused]] const auto& [taskCategory, taskCategoryTuple] :
             subCLIMap | std::views::filter(isCategoryUsed))
        {
            const auto taskCntr = subCLI.get<std::vector<std::string>>(taskCategory);
            for (const auto& task : taskCntr)
            {
                (*get<UpdateTaskFunctor>(get<TaskFunctorTuple>(taskCategoryTuple)))(task);
            }
        }
    }
}

bool Command::hasAnyTask() const
{
    return !dispatchedTask.empty();
}

void Command::dispatchTask()
{
    if (!dispatchedTask.basicTask.empty())
    {
        for (std::uint8_t i = 0; i < Bottom<Category>::value; ++i)
        {
            if (dispatchedTask.basicTask.primaryBit.test(Category(i)))
            {
                (this->*std::next(basicTaskDispatcher.cbegin(), Category(i))->second)();
            }
        }
    }
    else if (!dispatchedTask.regularTask.empty())
    {
        if (dispatchedTask.regularTask.helpOnly)
        {
            const auto isSubCLIUsed = [this](const auto& subCLIPair)
            {
                return mainCLI.isSubCommandUsed(subCLIPair.first);
            };

            for (const auto& subCLIName : std::views::keys(regularTaskDispatcher | std::views::filter(isSubCLIUsed)))
            {
                const auto& subCLI = mainCLI.at<utility::argument::Argument>(subCLIName);
                std::cout << subCLI.help().str() << std::flush;
                return;
            }
        }

        using SubTask = RegularTask::SubTask;
        const auto performTask = [this](const SubTask subTask)
        {
            for (const auto& taskCategoryTuple :
                 std::views::values(std::next(regularTaskDispatcher.cbegin(), subTask)->second))
            {
                (*get<PerformTaskFunctor>(get<TaskFunctorTuple>(taskCategoryTuple)))(
                    get<TargetTaskContainer>(taskCategoryTuple));
            }
        };

        if (!app_algo::getTask().empty())
        {
            performTask(SubTask::algorithm);
        }
        else if (!app_dp::getTask().empty())
        {
            performTask(SubTask::designPattern);
        }
        else if (!app_ds::getTask().empty())
        {
            performTask(SubTask::dataStructure);
        }
        else if (!app_num::getTask().empty())
        {
            performTask(SubTask::numeric);
        }
    }
}

//! @brief Launch the TCP client for console mode.
//! @param client - TCP client to be launched
template <>
void Command::launchClient<utility::socket::TCPSocket>(std::shared_ptr<utility::socket::TCPSocket>& client)
{
    client->onRawMessageReceived = [&client](char* buffer, const int length)
    {
        try
        {
            const auto tcpResp = view::View::parseTLVPacket(buffer, length);
            if (tcpResp.stopTag)
            {
                client->setNonBlocking();
            }
        }
        catch (std::exception& error)
        {
            LOG_WRN << error.what();
        }
    };
    client->toConnect(VIEW_TCP_HOST, VIEW_TCP_PORT);
}

//! @brief Launch the UDP client for console mode.
//! @param client - UDP client to be launched
template <>
void Command::launchClient<utility::socket::UDPSocket>(std::shared_ptr<utility::socket::UDPSocket>& client)
{
    client->onRawMessageReceived =
        [&client](char* buffer, const int length, const std::string& /*ip*/, const std::uint16_t /*port*/)
    {
        try
        {
            const auto udpResp = view::View::parseTLVPacket(buffer, length);
            if (udpResp.stopTag)
            {
                client->setNonBlocking();
            }
        }
        catch (std::exception& error)
        {
            LOG_WRN << error.what();
        }
    };
    client->toReceive();
    client->toConnect(VIEW_UDP_HOST, VIEW_UDP_PORT);
}

void Command::executeConsoleCommand() const
{
    if (!CONFIG_ACTIVE_HELPER)
    {
        std::cout << "exit" << std::endl;
        return;
    }

    const auto cmdCntr =
        mainCLI.get<std::vector<std::string>>(std::next(basicTaskDispatcher.cbegin(), Category::console)->first);
    if (cmdCntr.empty())
    {
        return;
    }

    auto udpClient = std::make_shared<utility::socket::UDPSocket>();
    launchClient(udpClient);
    utility::console::Console console(" > ");
    registerOnConsole(console, udpClient);

    for (const auto& cmd : cmdCntr)
    {
        console.commandExecutor(cmd);
    }

    udpClient->toSend(utility::common::base64Encode("stop"));
    udpClient->waitIfAlive();
}

void Command::showHelpMessage() const
{
    std::cout << mainCLI.help().str() << std::flush;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void Command::dumpConfiguration() const
{
    namespace file = utility::file;

    std::ofstream ofs = file::openFile(CONFIG_FILE_PATH, true);
    file::fdLock(ofs, file::LockMode::write);
    ofs << config::getDefaultConfiguration();
    file::fdUnlock(ofs);
    file::closeFile(ofs);

    const auto configs = file::getFileContents(CONFIG_FILE_PATH);
    std::ostringstream os;
    std::copy(configs.cbegin(), configs.cend(), std::ostream_iterator<std::string>(os, "\n"));
    std::cout << os.str() << std::flush;
}

void Command::showVersionIcon() const
{
    constexpr std::string_view processor =
#ifdef __TARGET_PROCESSOR
                                   __TARGET_PROCESSOR,
#else
                                   "other processor",
#endif // __TARGET_PROCESSOR
                               buildDate = "" __DATE__ " " __TIME__ "";
    constexpr auto getCXXCompiler = []()
    {
        std::ostringstream os;
        os <<
#ifdef __clang__
            "clang " << __clang_major__ << '.' << __clang_minor__ << '.' << __clang_patchlevel__;
#elif __GNUC__
            "gcc " << __GNUC__ << '.' << __GNUC_MINOR__ << '.' << __GNUC_PATCHLEVEL__;
#else
            "other compiler";
#endif // __clang__
        return std::move(os).str();
    };
    const std::string additionalInfo = "echo '" + std::string{copyright} + "' ; echo 'Built with " + getCXXCompiler()
        + " for " + std::string{processor} + " on " + std::string{buildDate} + ".'";

    std::string fullIcon = "tput rev ; echo " + getIconBanner();
    fullIcon.pop_back();
    fullIcon +=
#ifndef NDEBUG
        "              DEBUG VERSION "
#else
        "            RELEASE VERSION "
#endif // NDEBUG
        + mainCLI.version + " ' ; tput sgr0 ; echo ; " + additionalInfo;

    std::cout << utility::common::executeCommand(fullIcon) << std::flush;
}

void Command::checkForExcessiveArguments()
{
    if (hasAnyTask())
    {
        dispatchedTask.reset();
        throw std::logic_error("Excessive arguments.");
    }
}

template <typename T>
const T& Command::get(const TaskCategoryTuple& tuple)
{
    if constexpr (std::is_same_v<T, TargetTaskContainer>)
    {
        return std::get<0>(tuple);
    }
    else if constexpr (std::is_same_v<T, TaskFunctorTuple>)
    {
        return std::get<1>(tuple);
    }
}

template <typename T>
const T& Command::get(const TaskFunctorTuple& tuple)
{
    if constexpr (std::is_same_v<T, PerformTaskFunctor>)
    {
        return std::get<0>(tuple);
    }
    else if constexpr (std::is_same_v<T, UpdateTaskFunctor>)
    {
        return std::get<1>(tuple);
    }
}

template <typename T>
void Command::triggerHelper(const HelperOperation operation)
{
    if (!CONFIG_ACTIVE_HELPER)
    {
        return;
    }

    constexpr auto getInstance = []() -> T&
    {
        if constexpr (std::is_same_v<T, log::Log>)
        {
            return log::Log::getInstance();
        }
        else if constexpr (std::is_same_v<T, view::View>)
        {
            return view::View::getInstance();
        }
    };

    auto& helper = getInstance();
    switch (operation)
    {
        case HelperOperation::start:
            helper.waitToStart();
            break;
        case HelperOperation::stop:
            helper.waitToStop();
            break;
        case HelperOperation::rollback:
            helper.requestToRollback();
            break;
        default:
            break;
    }
}

void Command::enterConsoleMode()
{
    if (!CONFIG_ACTIVE_HELPER)
    {
        std::cout << "exit" << std::endl;
        return;
    }

    try
    {
#ifndef NDEBUG
        LOG_DBG << "Enter console mode.";
#endif // NDEBUG
        using utility::console::Console;

        std::cout << utility::common::executeCommand("tput bel ; echo " + getIconBanner() + " ; sleep 0.1s")
                  << std::flush;
        auto tcpClient = std::make_shared<utility::socket::TCPSocket>();
        launchClient(tcpClient);
        std::string user = "USER";
        if (nullptr != std::getenv("USER"))
        {
            user = std::getenv("USER");
        }
        char hostName[HOST_NAME_MAX] = {'\0'};
        if (::gethostname(hostName, HOST_NAME_MAX))
        {
            std::strncpy(hostName, "HOSTNAME", HOST_NAME_MAX);
        }
        const std::string greeting = user + '@' + std::string{hostName} + " foo > ";
        Console console(greeting);
        registerOnConsole(console, tcpClient);

        int retVal = Console::RetCode::success;
        do
        {
            try
            {
                retVal = console.readCommandLine();
                console.setGreeting(greeting);
            }
            catch (const std::exception& error)
            {
                LOG_WRN << error.what();
                utility::time::millisecondLevelSleep(maxLatency);
            }
        }
        while (Console::RetCode::quit != retVal);

        tcpClient->toSend(utility::common::base64Encode("stop"));
        tcpClient->waitIfAlive();
#ifndef NDEBUG
        LOG_DBG << "Exit console mode.";
#endif // NDEBUG
    }
    catch (const std::exception& error)
    {
        LOG_ERR << error.what();
    }
}

template <typename T>
void Command::registerOnConsole(utility::console::Console& console, std::shared_ptr<T>& client)
{
    using utility::console::Console;

    console.registerCommand(
        "refresh",
        [](const Console::Args& /*input*/)
        {
            int retVal = Console::RetCode::success;
            try
            {
                triggerHelper<log::Log>(HelperOperation::rollback);
                triggerHelper<log::Log>(HelperOperation::start);

                LOG_INF << "Refreshed the outputs.";
                utility::time::millisecondLevelSleep(maxLatency);
            }
            catch (const std::exception& error)
            {
                retVal = Console::RetCode::error;
                LOG_WRN << error.what();
            }
            return retVal;
        },
        "refresh the outputs");

    console.registerCommand(
        "reconnect",
        [&client](const Console::Args& /*input*/)
        {
            int retVal = Console::RetCode::success;
            try
            {
                client->toSend(utility::common::base64Encode("stop"));
                client->waitIfAlive();
                client.reset();
                triggerHelper<view::View>(HelperOperation::rollback);
                triggerHelper<view::View>(HelperOperation::start);

                client = std::make_shared<T>();
                launchClient(client);
                LOG_INF << "Reconnected to the servers.";
                utility::time::millisecondLevelSleep(maxLatency);
            }
            catch (const std::exception& error)
            {
                retVal = Console::RetCode::error;
                LOG_WRN << error.what();
            }
            return retVal;
        },
        "reconnect to the servers");

    for (const auto& [option, optionTuple] : VIEW_OPTIONS)
    {
        console.registerCommand(
            option,
            [&client](const Console::Args& input)
            {
                int retVal = Console::RetCode::success;
                try
                {
                    std::string cmds;
                    for (const auto& arg : input)
                    {
                        cmds += arg + ' ';
                    }
                    if (!cmds.empty())
                    {
                        cmds.pop_back();
                    }

                    client->toSend(utility::common::base64Encode(cmds));
                    utility::time::millisecondLevelSleep(maxLatency);
                }
                catch (const std::exception& error)
                {
                    retVal = Console::RetCode::error;
                    LOG_WRN << error.what();
                }
                return retVal;
            },
            view::View::get<view::View::HelpMessage>(optionTuple));
    }
}

std::string Command::getIconBanner()
{
    std::string banner;
    banner += R"(')";
    banner += R"(  ______   ______     ______    \n)";
    banner += R"( /\  ___\ /\  __ \   /\  __ \   \n)";
    banner += R"( \ \  __\ \ \ \/\ \  \ \ \/\ \  \n)";
    banner += R"(  \ \_\    \ \_____\  \ \_____\ \n)";
    banner += R"(   \/_/     \/_____/   \/_____/ \n)";
    banner += R"(')";

    return banner;
}

//! @brief Get memory pool when making multi-threading.
//! @return reference of the PublicThreadPool object
PublicThreadPool& getPublicThreadPool()
{
    static PublicThreadPool pool{};
    return pool;
}
} // namespace application::command
