//! @file command.cpp
//! @author ryftchen
//! @brief The definitions (command) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#include "command.hpp"
#include "log.hpp"
#include "note.hpp"
#include "view.hpp"

#ifndef __PRECOMPILED_HEADER
#include <ranges>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

#include "utility/include/currying.hpp"

namespace application::command
{
//! @brief Alias for the type information.
//! @tparam T - type of target object
template <class T>
using TypeInfo = utility::reflection::TypeInfo<T>;

//! @brief Constraint for external helpers.
//! @tparam T - type of helper
template <typename T>
concept HelperType = requires (T /*helper*/) {
    {
        T::getInstance()
    } -> std::same_as<T&>;
} && !std::is_copy_constructible_v<T> && !std::is_copy_assignable_v<T> && std::derived_from<T, utility::fsm::FSM<T>>;

//! @brief Enumerate specific operations to control external helpers.
enum HelperControl : std::uint8_t
{
    //! @brief Start.
    start,
    //! @brief Stop.
    stop,
    //! @brief Reset.
    reset
};

//! @brief Trigger the external helper with operation.
//! @tparam Helper - target helper
//! @param operation - target operation
template <HelperType Helper>
static void triggerHelper(const HelperControl operation)
{
    if (!CONFIG_ACTIVATE_HELPER)
    {
        return;
    }

    switch (operation)
    {
        case HelperControl::start:
            Helper::getInstance().waitForStart();
            break;
        case HelperControl::stop:
            Helper::getInstance().waitForStop();
            break;
        case HelperControl::reset:
            Helper::getInstance().requestToReset();
            break;
        default:
            break;
    }
}

Command::Command()
{
    initializeCLI();
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
    triggerHelper<log::Log>(HelperControl::start);
    triggerHelper<view::View>(HelperControl::start);

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

    triggerHelper<view::View>(HelperControl::stop);
    triggerHelper<log::Log>(HelperControl::stop);
}
catch (const std::exception& error)
{
    LOG_ERR << error.what();
}

// NOLINTNEXTLINE (readability-function-size)
void Command::initializeCLI()
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

    const auto& algoTable = regularTaskDispatcher.at(subCLIAppAlgo.title);
    const auto& algoAlias = filterAliasUnderSubCLI<app_algo::AlgorithmTask>(subCLIAppAlgo.title);
    subCLIAppAlgo.addDescription("apply algorithm");
    subCLIAppAlgo.addArgument("-h", "--help").argsNum(0).implicitVal(true).help("show help and exit");
    auto algoCategory = std::string{TypeInfo<app_algo::MatchMethod>::name};
    subCLIAppAlgo.addArgument("-" + algoAlias.at(algoCategory), "--" + algoCategory)
        .argsNum(0, algoTable.at(algoCategory).candidates.size())
        .defaultVal<std::vector<std::string>>(AggregationTasks{algoTable.at(algoCategory).candidates})
        .remaining()
        .metavar("OPT")
        .help("run match tasks\n"
              "- rab    Rabin-Karp\n"
              "- knu    Knuth-Morris-Pratt\n"
              "- boy    Boyer-Moore\n"
              "- hor    Horspool\n"
              "- sun    Sunday\n"
              "add the tasks listed above");
    algoCategory = std::string{TypeInfo<app_algo::NotationMethod>::name};
    subCLIAppAlgo.addArgument("-" + algoAlias.at(algoCategory), "--" + algoCategory)
        .argsNum(0, algoTable.at(algoCategory).candidates.size())
        .defaultVal<std::vector<std::string>>(AggregationTasks{algoTable.at(algoCategory).candidates})
        .remaining()
        .metavar("OPT")
        .help("run notation tasks\n"
              "- pre    Prefix\n"
              "- pos    Postfix\n"
              "add the tasks listed above");
    algoCategory = std::string{TypeInfo<app_algo::OptimalMethod>::name};
    subCLIAppAlgo.addArgument("-" + algoAlias.at(algoCategory), "--" + algoCategory)
        .argsNum(0, algoTable.at(algoCategory).candidates.size())
        .defaultVal<std::vector<std::string>>(AggregationTasks{algoTable.at(algoCategory).candidates})
        .remaining()
        .metavar("OPT")
        .help("run optimal tasks\n"
              "- gra    Gradient Descent\n"
              "- ann    Simulated Annealing\n"
              "- par    Particle Swarm\n"
              "- gen    Genetic\n"
              "add the tasks listed above");
    algoCategory = std::string{TypeInfo<app_algo::SearchMethod>::name};
    subCLIAppAlgo.addArgument("-" + algoAlias.at(algoCategory), "--" + algoCategory)
        .argsNum(0, algoTable.at(algoCategory).candidates.size())
        .defaultVal<std::vector<std::string>>(AggregationTasks{algoTable.at(algoCategory).candidates})
        .remaining()
        .metavar("OPT")
        .help("run search tasks\n"
              "- bin    Binary\n"
              "- int    Interpolation\n"
              "- fib    Fibonacci\n"
              "add the tasks listed above");
    algoCategory = std::string{TypeInfo<app_algo::SortMethod>::name};
    subCLIAppAlgo.addArgument("-" + algoAlias.at(algoCategory), "--" + algoCategory)
        .argsNum(0, algoTable.at(algoCategory).candidates.size())
        .defaultVal<std::vector<std::string>>(AggregationTasks{algoTable.at(algoCategory).candidates})
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

    const auto& dpTable = regularTaskDispatcher.at(subCLIAppDp.title);
    const auto& dpAlias = filterAliasUnderSubCLI<app_dp::DesignPatternTask>(subCLIAppDp.title);
    subCLIAppDp.addDescription("apply design pattern");
    subCLIAppDp.addArgument("-h", "--help").argsNum(0).implicitVal(true).help("show help and exit");
    auto dpCategory = std::string{TypeInfo<app_dp::BehavioralInstance>::name};
    subCLIAppDp.addArgument("-" + dpAlias.at(dpCategory), "--" + dpCategory)
        .argsNum(0, dpTable.at(dpCategory).candidates.size())
        .defaultVal<std::vector<std::string>>(AggregationTasks{dpTable.at(dpCategory).candidates})
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
    dpCategory = std::string{TypeInfo<app_dp::CreationalInstance>::name};
    subCLIAppDp.addArgument("-" + dpAlias.at(dpCategory), "--" + dpCategory)
        .argsNum(0, dpTable.at(dpCategory).candidates.size())
        .defaultVal<std::vector<std::string>>(AggregationTasks{dpTable.at(dpCategory).candidates})
        .remaining()
        .metavar("OPT")
        .help("run creational tasks\n"
              "- abs    Abstract Factory\n"
              "- bui    Builder\n"
              "- fac    Factory Method\n"
              "- pro    Prototype\n"
              "- sin    Singleton\n"
              "add the tasks listed above");
    dpCategory = std::string{TypeInfo<app_dp::StructuralInstance>::name};
    subCLIAppDp.addArgument("-" + dpAlias.at(dpCategory), "--" + dpCategory)
        .argsNum(0, dpTable.at(dpCategory).candidates.size())
        .defaultVal<std::vector<std::string>>(AggregationTasks{dpTable.at(dpCategory).candidates})
        .remaining()
        .metavar("OPT")
        .help("run structural tasks\n"
              "- ada    Adapter\n"
              "- bri    Bridge\n"
              "- com    Composite\n"
              "- dec    Decorator\n"
              "- fac    Facade\n"
              "- fly    Flyweight\n"
              "- pro    Proxy\n"
              "add the tasks listed above");
    mainCLI.addSubParser(subCLIAppDp);

    const auto& dsTable = regularTaskDispatcher.at(subCLIAppDs.title);
    const auto& dsAlias = filterAliasUnderSubCLI<app_ds::DataStructureTask>(subCLIAppDs.title);
    subCLIAppDs.addDescription("apply data structure");
    subCLIAppDs.addArgument("-h", "--help").argsNum(0).implicitVal(true).help("show help and exit");
    auto dsCategory = std::string{TypeInfo<app_ds::LinearInstance>::name};
    subCLIAppDs.addArgument("-" + dsAlias.at(dsCategory), "--" + dsCategory)
        .argsNum(0, dsTable.at(dsCategory).candidates.size())
        .defaultVal<std::vector<std::string>>(AggregationTasks{dsTable.at(dsCategory).candidates})
        .remaining()
        .metavar("OPT")
        .help("run linear tasks\n"
              "- lin    Linked List\n"
              "- sta    Stack\n"
              "- que    Queue\n"
              "add the tasks listed above");
    dsCategory = std::string{TypeInfo<app_ds::TreeInstance>::name};
    subCLIAppDs.addArgument("-" + dsAlias.at(dsCategory), "--" + dsCategory)
        .argsNum(0, dsTable.at(dsCategory).candidates.size())
        .defaultVal<std::vector<std::string>>(AggregationTasks{dsTable.at(dsCategory).candidates})
        .remaining()
        .metavar("OPT")
        .help("run tree tasks\n"
              "- bin    Binary Search\n"
              "- ade    Adelson-Velsky-Landis\n"
              "- spl    Splay\n"
              "add the tasks listed above");
    mainCLI.addSubParser(subCLIAppDs);

    const auto& numTable = regularTaskDispatcher.at(subCLIAppNum.title);
    const auto& numAlias = filterAliasUnderSubCLI<app_num::NumericTask>(subCLIAppNum.title);
    subCLIAppNum.addDescription("apply numeric");
    subCLIAppNum.addArgument("-h", "--help").argsNum(0).implicitVal(true).help("show help and exit");
    auto numCategory = std::string{TypeInfo<app_num::ArithmeticMethod>::name};
    subCLIAppNum.addArgument("-" + numAlias.at(numCategory), "--" + numCategory)
        .argsNum(0, numTable.at(numCategory).candidates.size())
        .defaultVal<std::vector<std::string>>(AggregationTasks{numTable.at(numCategory).candidates})
        .remaining()
        .metavar("OPT")
        .help("run arithmetic tasks\n"
              "- add    Addition\n"
              "- sub    Subtraction\n"
              "- mul    Multiplication\n"
              "- div    Division\n"
              "add the tasks listed above");
    numCategory = std::string{TypeInfo<app_num::DivisorMethod>::name};
    subCLIAppNum.addArgument("-" + numAlias.at(numCategory), "--" + numCategory)
        .argsNum(0, numTable.at(numCategory).candidates.size())
        .defaultVal<std::vector<std::string>>(AggregationTasks{numTable.at(numCategory).candidates})
        .remaining()
        .metavar("OPT")
        .help("run divisor tasks\n"
              "- euc    Euclidean\n"
              "- ste    Stein\n"
              "add the tasks listed above");
    numCategory = std::string{TypeInfo<app_num::IntegralMethod>::name};
    subCLIAppNum.addArgument("-" + numAlias.at(numCategory), "--" + numCategory)
        .argsNum(0, numTable.at(numCategory).candidates.size())
        .defaultVal<std::vector<std::string>>(AggregationTasks{numTable.at(numCategory).candidates})
        .remaining()
        .metavar("OPT")
        .help("run integral tasks\n"
              "- tra    Trapezoidal\n"
              "- sim    Adaptive Simpson's 1/3\n"
              "- rom    Romberg\n"
              "- gau    Gauss-Legendre's 5-Points\n"
              "- mon    Monte-Carlo\n"
              "add the tasks listed above");
    numCategory = std::string{TypeInfo<app_num::PrimeMethod>::name};
    subCLIAppNum.addArgument("-" + numAlias.at(numCategory), "--" + numCategory)
        .argsNum(0, numTable.at(numCategory).candidates.size())
        .defaultVal<std::vector<std::string>>(AggregationTasks{numTable.at(numCategory).candidates})
        .remaining()
        .metavar("OPT")
        .help("run prime tasks\n"
              "- era    Eratosthenes\n"
              "- eul    Euler\n"
              "add the tasks listed above");
    mainCLI.addSubParser(subCLIAppNum);
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

//! @brief Get the callback for running tasks.
//! @param tuple - a tuple containing the callback to be got
//! @return callback for running tasks
template <>
auto Command::get<Command::RunTasksFunctor>(const TaskFunctorTuple& tuple) -> const RunTasksFunctor&
{
    return std::get<0>(tuple);
}

//! @brief Get the callback for updating task.
//! @param tuple - a tuple containing the callback to be got
//! @return callback for updating task
template <>
auto Command::get<Command::UpdateTaskFunctor>(const TaskFunctorTuple& tuple) -> const UpdateTaskFunctor&
{
    return std::get<1>(tuple);
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

        for ([[maybe_unused]] const auto& [categoryName, categoryAttr] : subCLIMap | std::views::filter(isCategoryUsed))
        {
            const auto& pendingTasks = subCLI.get<std::vector<std::string>>(categoryName);
            for (const auto& target : pendingTasks)
            {
                const auto& updateTask = get<UpdateTaskFunctor>(categoryAttr.callbacks);
                updateTask(target);
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
            return;
        }

        for (const auto& categoryAttr : std::views::values(
                 std::next(regularTaskDispatcher.cbegin(), dispatchedTask.regularTask.getExistingSubTask())->second))
        {
            const auto& runTasks = get<RunTasksFunctor>(categoryAttr.callbacks);
            runTasks(categoryAttr.candidates);
        }
    }
}

template <typename T>
std::map<Command::CategoryName, Command::CategoryAlias> Command::filterAliasUnderSubCLI(const SubCLIName& name) const
{
    using TypeInfo = utility::reflection::TypeInfo<T>;

    const auto& table = regularTaskDispatcher.at(name);
    if ((name != TypeInfo::name) || (table.size() != TypeInfo::fields.size))
    {
        throw std::logic_error("The " + name + " sub-command is invalid.");
    }

    std::map<CategoryName, CategoryAlias> aliases;
    TypeInfo::fields.forEach(
        [name, table, &aliases](auto field)
        {
            if (!field.hasValue || (table.cend() == table.find(std::string{field.name})))
            {
                throw std::logic_error(
                    "The --" + std::string{field.name} + " option has not been added to the " + name + " sub-command.");
            }

            static_assert(1 == field.attrs.size);
            auto attr = field.attrs.find(REFLECTION_STR("alias"));
            static_assert(attr.hasValue);
            aliases.emplace(field.name, attr.value);
        });

    return aliases;
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
            if (VIEW_TLV_PACKET(buffer, length).stopTag)
            {
                client->setNonBlocking();
            }
            VIEW_AWAKEN;
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
            if (VIEW_TLV_PACKET(buffer, length).stopTag)
            {
                client->setNonBlocking();
            }
            VIEW_AWAKEN;
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
    if (!CONFIG_ACTIVATE_HELPER)
    {
        std::cout << "exit" << std::endl;
        return;
    }

    const auto& pendingInputs =
        mainCLI.get<std::vector<std::string>>(std::next(basicTaskDispatcher.cbegin(), Category::console)->first);
    if (pendingInputs.empty())
    {
        return;
    }

    auto udpClient = std::make_shared<utility::socket::UDPSocket>();
    launchClient(udpClient);
    utility::console::Console console(" > ");
    registerOnConsole(console, udpClient);

    for (const auto& cmd : pendingInputs)
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

// NOLINTNEXTLINE (readability-convert-member-functions-to-static)
void Command::dumpConfiguration() const
{
    config::forcedConfigurationUpdateByDefault(CONFIG_FILE_PATH);

    const auto& configs = utility::file::getFileContents(CONFIG_FILE_PATH);
    std::ostringstream os;
    std::copy(configs.cbegin(), configs.cend(), std::ostream_iterator<std::string>(os, "\n"));
    std::cout << os.str() << std::flush;
}

void Command::showVersionIcon() const
{
    validateDependenciesVersion();

    const std::string additionalInfo = "echo '" + std::string{note::copyright()} + "' ; echo 'Built with "
        + note::compiler() + " for " + std::string{note::processor()} + " on " + std::string{note::buildDate()} + ".'";
    std::string fullIcon = "tput rev ; echo " + getIconBanner();
    fullIcon.pop_back();
    fullIcon +=
#ifndef NDEBUG
        "            DEBUG VERSION "
#else
        "          RELEASE VERSION "
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

void Command::enterConsoleMode()
try
{
    if (!CONFIG_ACTIVATE_HELPER)
    {
        std::cout << "exit" << std::endl;
        return;
    }

#ifndef NDEBUG
    LOG_DBG << "Enter console mode.";
#endif // NDEBUG
    using utility::console::Console;
    using enum Console::RetCode;

    std::cout << utility::common::executeCommand("tput bel ; echo " + getIconBanner() + " ; sleep 0.1s") << std::flush;
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
        std::strncpy(hostName, "HOSTNAME", HOST_NAME_MAX - 1);
        hostName[HOST_NAME_MAX - 1] = '\0';
    }
    const std::string greeting = user + '@' + std::string{hostName} + " foo > ";
    Console console(greeting);
    registerOnConsole(console, tcpClient);

    int retVal = success;
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
            utility::time::millisecondLevelSleep(latency);
        }
    }
    while (quit != retVal);

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

template <typename T>
void Command::registerOnConsole(utility::console::Console& console, std::shared_ptr<T>& client)
{
    using utility::console::Console;
    using enum Console::RetCode;

    console.registerCommand(
        "refresh",
        [](const Console::Args& /*input*/)
        {
            auto retVal = success;
            try
            {
                triggerHelper<log::Log>(HelperControl::reset);
                triggerHelper<log::Log>(HelperControl::start);

                LOG_INF << "Refreshed the outputs.";
            }
            catch (const std::exception& err)
            {
                retVal = error;
                LOG_WRN << err.what();
            }
            utility::time::millisecondLevelSleep(latency);
            return retVal;
        },
        "refresh the outputs");

    console.registerCommand(
        "reconnect",
        [&client](const Console::Args& /*input*/)
        {
            auto retVal = success;
            try
            {
                client->toSend(utility::common::base64Encode("stop"));
                client->waitIfAlive();
                client.reset();
                triggerHelper<view::View>(HelperControl::reset);
                triggerHelper<view::View>(HelperControl::start);

                client = std::make_shared<T>();
                launchClient(client);
                LOG_INF << "Reconnected to the servers.";
            }
            catch (const std::exception& err)
            {
                retVal = error;
                LOG_WRN << err.what();
            }
            utility::time::millisecondLevelSleep(latency);
            return retVal;
        },
        "reconnect to the servers");

    for (const auto& [optionName, optionAttr] : VIEW_OPTIONS)
    {
        console.registerCommand(
            optionName,
            [&client](const Console::Args& input)
            {
                auto retVal = success;
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
                    VIEW_AWAIT;
                }
                catch (const std::exception& err)
                {
                    retVal = error;
                    LOG_WRN << err.what();
                    utility::time::millisecondLevelSleep(latency);
                }
                return retVal;
            },
            optionAttr.prompt);
    }
}

void Command::validateDependenciesVersion() const
{
    using utility::common::allStrEqual;

    if (!allStrEqual(
            mainCLI.version.data(),
            utility::argument::version(),
            utility::common::version(),
            utility::console::version(),
            utility::currying::version(),
            utility::file::version(),
            utility::fsm::version(),
            utility::json::version(),
            utility::memory::version(),
            utility::reflection::version(),
            utility::socket::version(),
            utility::thread::version(),
            utility::time::version())
        || !allStrEqual(
            subCLIAppAlgo.version.data(),
            algorithm::match::version(),
            algorithm::notation::version(),
            algorithm::optimal::version(),
            algorithm::search::version(),
            algorithm::sort::version())
        || !allStrEqual(
            subCLIAppDp.version.data(),
            design_pattern::behavioral::version(),
            design_pattern::creational::version(),
            design_pattern::structural::version())
        || !allStrEqual(subCLIAppDs.version.data(), date_structure::linear::version(), date_structure::tree::version())
        || !allStrEqual(
            subCLIAppNum.version.data(),
            numeric::arithmetic::version(),
            numeric::divisor::version(),
            numeric::integral::version(),
            numeric::prime::version()))
    {
        throw std::runtime_error(
            "Dependencies version number mismatch. Expected main version: " + mainCLI.title + " (" + mainCLI.version
            + "), sub-version: " + subCLIAppAlgo.title + " (" + subCLIAppAlgo.version + "), " + subCLIAppDp.title + " ("
            + subCLIAppDp.version + "), " + subCLIAppDs.title + " (" + subCLIAppDs.version + "), " + subCLIAppNum.title
            + " (" + subCLIAppNum.version + ").");
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
    static PublicThreadPool pooling{};
    return pooling;
}
} // namespace application::command
