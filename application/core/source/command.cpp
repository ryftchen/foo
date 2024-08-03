//! @file command.cpp
//! @author ryftchen
//! @brief The definitions (command) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#include "command.hpp"
#include "log.hpp"
#include "view.hpp"

#ifndef __PRECOMPILED_HEADER
#include <ranges>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

#include "utility/include/currying.hpp"

namespace application::command
{
//! @brief Anonymous namespace.
inline namespace
{
//! @brief Alias for the type information.
//! @tparam T - type of target object
template <class T>
using TypeInfo = utility::reflection::TypeInfo<T>;

//! @brief Constraint for external helpers.
//! @tparam T - type of helper
template <typename T>
concept HelperType = std::derived_from<T, utility::fsm::FSM<T>> &&
    requires (T /*helper*/)
{
    {
        T::getInstance()
    } -> std::same_as<T&>;
}
&&!std::is_copy_constructible_v<T> && !std::is_copy_assignable_v<T> && !std::is_move_constructible_v<T>
    && !std::is_move_assignable_v<T>;

//! @brief Enumerate specific events to control external helpers.
enum ExtEvent : std::uint8_t
{
    //! @brief Start.
    start,
    //! @brief Stop.
    stop,
    //! @brief Reset.
    reset
};
} // namespace

//! @brief Trigger the external helper with event.
//! @tparam Helper - target helper
//! @param event - target event
template <HelperType Helper>
static void triggerHelper(const ExtEvent event)
{
    if (!CONFIG_ACTIVATE_HELPER)
    {
        return;
    }

    switch (event)
    {
        case ExtEvent::start:
            Helper::getInstance().waitForStart();
            break;
        case ExtEvent::stop:
            Helper::getInstance().waitForStop();
            break;
        case ExtEvent::reset:
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
    dispatchManager.reset();
}

Command& Command::getInstance()
{
    static Command commander{};
    return commander;
}

void Command::execManager(const int argc, const char* const argv[])
try
{
    triggerHelper<log::Log>(ExtEvent::start);
    triggerHelper<view::View>(ExtEvent::start);

    if (1 == argc)
    {
        enterConsoleMode();
    }
    else
    {
        constexpr std::uint8_t groundNum = 2;
        auto threads = std::make_shared<utility::thread::Thread>(groundNum);
        threads->enqueue("commander-fg", &Command::foregroundHandler, this, argc, argv);
        threads->enqueue("commander-bg", &Command::backgroundHandler, this);
    }

    triggerHelper<view::View>(ExtEvent::stop);
    triggerHelper<log::Log>(ExtEvent::stop);
}
catch (const std::exception& err)
{
    LOG_ERR << err.what();
}

// NOLINTNEXTLINE (readability-function-size)
void Command::initializeCLI()
{
    mainCLI.addArgument("-h", "--help").argsNum(0).implicitVal(true).help("show help and exit");

    mainCLI.addArgument("-v", "--version").argsNum(0).implicitVal(true).help("show version and exit");

    mainCLI.addArgument("-d", "--dump").argsNum(0).implicitVal(true).help("dump default configuration and exit");

    mainCLI.addArgument("-c", "--console")
        .argsNum(utility::argument::ArgsNumPattern::any)
        .defaultVal<std::vector<std::string>>({"usage"})
        .appending()
        .action(
            [](const std::string& value)
            {
                if (std::none_of(
                        value.cbegin(),
                        value.cend(),
                        [](const auto c)
                        {
                            return ' ' != c;
                        }))
                {
                    throw std::invalid_argument("Invalid console command.");
                }
                return value;
            })
        .metavar("CMD")
        .help("run commands in console mode and exit\n"
              "separate with quotes");

    auto& algoTbl = regularChoices[subCLIAppAlgo.title()];
    subCLIAppAlgo.addDescription(getDescr<app_algo::AlgorithmChoice>());
    subCLIAppAlgo.addArgument("-h", "--help").argsNum(0).implicitVal(true).help("show help and exit");
    auto algoCat = std::string{TypeInfo<app_algo::MatchMethod>::name};
    auto algoCho = extractChoices<app_algo::MatchMethod>();
    algoTbl[algoCat] = CategoryExtAttr{algoCho, app_algo::MatchMethod{}};
    subCLIAppAlgo
        .addArgument("-" + std::string{getAlias<app_algo::AlgorithmChoice, app_algo::MatchMethod>()}, "--" + algoCat)
        .argsNum(0, algoCho.size())
        .defaultVal<std::vector<std::string>>(std::move(algoCho))
        .remaining()
        .metavar("OPT")
        .help(getDescr<app_algo::MatchMethod>());
    applyMessage.registerHandler(
        [](const apply::UpdateChoice<app_algo::MatchMethod>& msg)
        {
            app_algo::updateChoice<app_algo::MatchMethod>(msg.cho);
        });
    applyMessage.registerHandler(
        [](const apply::RunChoices<app_algo::MatchMethod>& msg)
        {
            app_algo::runChoices<app_algo::MatchMethod>(msg.coll);
        });
    algoCat = std::string{TypeInfo<app_algo::NotationMethod>::name};
    algoCho = extractChoices<app_algo::NotationMethod>();
    algoTbl[algoCat] = CategoryExtAttr{algoCho, app_algo::NotationMethod{}};
    subCLIAppAlgo
        .addArgument("-" + std::string{getAlias<app_algo::AlgorithmChoice, app_algo::NotationMethod>()}, "--" + algoCat)
        .argsNum(0, algoCho.size())
        .defaultVal<std::vector<std::string>>(std::move(algoCho))
        .remaining()
        .metavar("OPT")
        .help(getDescr<app_algo::NotationMethod>());
    applyMessage.registerHandler(
        [](const apply::UpdateChoice<app_algo::NotationMethod>& msg)
        {
            app_algo::updateChoice<app_algo::NotationMethod>(msg.cho);
        });
    applyMessage.registerHandler(
        [](const apply::RunChoices<app_algo::NotationMethod>& msg)
        {
            app_algo::runChoices<app_algo::NotationMethod>(msg.coll);
        });
    algoCat = std::string{TypeInfo<app_algo::OptimalMethod>::name};
    algoCho = extractChoices<app_algo::OptimalMethod>();
    algoTbl[algoCat] = CategoryExtAttr{algoCho, app_algo::OptimalMethod{}};
    subCLIAppAlgo
        .addArgument("-" + std::string{getAlias<app_algo::AlgorithmChoice, app_algo::OptimalMethod>()}, "--" + algoCat)
        .argsNum(0, algoCho.size())
        .defaultVal<std::vector<std::string>>(std::move(algoCho))
        .remaining()
        .metavar("OPT")
        .help(getDescr<app_algo::OptimalMethod>());
    applyMessage.registerHandler(
        [](const apply::UpdateChoice<app_algo::OptimalMethod>& msg)
        {
            app_algo::updateChoice<app_algo::OptimalMethod>(msg.cho);
        });
    applyMessage.registerHandler(
        [](const apply::RunChoices<app_algo::OptimalMethod>& msg)
        {
            app_algo::runChoices<app_algo::OptimalMethod>(msg.coll);
        });
    algoCat = std::string{TypeInfo<app_algo::SearchMethod>::name};
    algoCho = extractChoices<app_algo::SearchMethod>();
    algoTbl[algoCat] = CategoryExtAttr{algoCho, app_algo::SearchMethod{}};
    subCLIAppAlgo
        .addArgument("-" + std::string{getAlias<app_algo::AlgorithmChoice, app_algo::SearchMethod>()}, "--" + algoCat)
        .argsNum(0, algoCho.size())
        .defaultVal<std::vector<std::string>>(std::move(algoCho))
        .remaining()
        .metavar("OPT")
        .help(getDescr<app_algo::SearchMethod>());
    applyMessage.registerHandler(
        [](const apply::UpdateChoice<app_algo::SearchMethod>& msg)
        {
            app_algo::updateChoice<app_algo::SearchMethod>(msg.cho);
        });
    applyMessage.registerHandler(
        [](const apply::RunChoices<app_algo::SearchMethod>& msg)
        {
            app_algo::runChoices<app_algo::SearchMethod>(msg.coll);
        });
    algoCat = std::string{TypeInfo<app_algo::SortMethod>::name};
    algoCho = extractChoices<app_algo::SortMethod>();
    algoTbl[algoCat] = CategoryExtAttr{algoCho, app_algo::SortMethod{}};
    subCLIAppAlgo
        .addArgument("-" + std::string{getAlias<app_algo::AlgorithmChoice, app_algo::SortMethod>()}, "--" + algoCat)
        .argsNum(0, algoCho.size())
        .defaultVal<std::vector<std::string>>(std::move(algoCho))
        .remaining()
        .metavar("OPT")
        .help(getDescr<app_algo::SortMethod>());
    applyMessage.registerHandler(
        [](const apply::UpdateChoice<app_algo::SortMethod>& msg)
        {
            app_algo::updateChoice<app_algo::SortMethod>(msg.cho);
        });
    applyMessage.registerHandler(
        [](const apply::RunChoices<app_algo::SortMethod>& msg)
        {
            app_algo::runChoices<app_algo::SortMethod>(msg.coll);
        });
    mainCLI.addSubParser(subCLIAppAlgo);

    auto& dpTbl = regularChoices[subCLIAppDp.title()];
    subCLIAppDp.addDescription(getDescr<app_dp::DesignPatternChoice>());
    subCLIAppDp.addArgument("-h", "--help").argsNum(0).implicitVal(true).help("show help and exit");
    auto dpCat = std::string{TypeInfo<app_dp::BehavioralInstance>::name};
    auto dpCho = extractChoices<app_dp::BehavioralInstance>();
    dpTbl[dpCat] = CategoryExtAttr{dpCho, app_dp::BehavioralInstance{}};
    subCLIAppDp
        .addArgument(
            "-" + std::string{getAlias<app_dp::DesignPatternChoice, app_dp::BehavioralInstance>()}, "--" + dpCat)
        .argsNum(0, dpCho.size())
        .defaultVal<std::vector<std::string>>(std::move(dpCho))
        .remaining()
        .metavar("OPT")
        .help(getDescr<app_dp::BehavioralInstance>());
    applyMessage.registerHandler(
        [](const apply::UpdateChoice<app_dp::BehavioralInstance>& msg)
        {
            app_dp::updateChoice<app_dp::BehavioralInstance>(msg.cho);
        });
    applyMessage.registerHandler(
        [](const apply::RunChoices<app_dp::BehavioralInstance>& msg)
        {
            app_dp::runChoices<app_dp::BehavioralInstance>(msg.coll);
        });
    dpCat = std::string{TypeInfo<app_dp::CreationalInstance>::name};
    dpCho = extractChoices<app_dp::CreationalInstance>();
    dpTbl[dpCat] = CategoryExtAttr{dpCho, app_dp::CreationalInstance{}};
    subCLIAppDp
        .addArgument(
            "-" + std::string{getAlias<app_dp::DesignPatternChoice, app_dp::CreationalInstance>()}, "--" + dpCat)
        .argsNum(0, dpCho.size())
        .defaultVal<std::vector<std::string>>(std::move(dpCho))
        .remaining()
        .metavar("OPT")
        .help(getDescr<app_dp::CreationalInstance>());
    applyMessage.registerHandler(
        [](const apply::UpdateChoice<app_dp::CreationalInstance>& msg)
        {
            app_dp::updateChoice<app_dp::CreationalInstance>(msg.cho);
        });
    applyMessage.registerHandler(
        [](const apply::RunChoices<app_dp::CreationalInstance>& msg)
        {
            app_dp::runChoices<app_dp::CreationalInstance>(msg.coll);
        });
    dpCat = std::string{TypeInfo<app_dp::StructuralInstance>::name};
    dpCho = extractChoices<app_dp::StructuralInstance>();
    dpTbl[dpCat] = CategoryExtAttr{dpCho, app_dp::StructuralInstance{}};
    subCLIAppDp
        .addArgument(
            "-" + std::string{getAlias<app_dp::DesignPatternChoice, app_dp::StructuralInstance>()}, "--" + dpCat)
        .argsNum(0, dpCho.size())
        .defaultVal<std::vector<std::string>>(std::move(dpCho))
        .remaining()
        .metavar("OPT")
        .help(getDescr<app_dp::StructuralInstance>());
    applyMessage.registerHandler(
        [](const apply::UpdateChoice<app_dp::StructuralInstance>& msg)
        {
            app_dp::updateChoice<app_dp::StructuralInstance>(msg.cho);
        });
    applyMessage.registerHandler(
        [](const apply::RunChoices<app_dp::StructuralInstance>& msg)
        {
            app_dp::runChoices<app_dp::StructuralInstance>(msg.coll);
        });
    mainCLI.addSubParser(subCLIAppDp);

    auto& dsTbl = regularChoices[subCLIAppDs.title()];
    subCLIAppDs.addDescription(getDescr<app_ds::DataStructureChoice>());
    subCLIAppDs.addArgument("-h", "--help").argsNum(0).implicitVal(true).help("show help and exit");
    auto dsCat = std::string{TypeInfo<app_ds::LinearInstance>::name};
    auto dsCho = extractChoices<app_ds::LinearInstance>();
    dsTbl[dsCat] = CategoryExtAttr{dsCho, app_ds::LinearInstance{}};
    subCLIAppDs
        .addArgument("-" + std::string{getAlias<app_ds::DataStructureChoice, app_ds::LinearInstance>()}, "--" + dsCat)
        .argsNum(0, dsCho.size())
        .defaultVal<std::vector<std::string>>(std::move(dsCho))
        .remaining()
        .metavar("OPT")
        .help(getDescr<app_ds::LinearInstance>());
    applyMessage.registerHandler(
        [](const apply::UpdateChoice<app_ds::LinearInstance>& msg)
        {
            app_ds::updateChoice<app_ds::LinearInstance>(msg.cho);
        });
    applyMessage.registerHandler(
        [](const apply::RunChoices<app_ds::LinearInstance>& msg)
        {
            app_ds::runChoices<app_ds::LinearInstance>(msg.coll);
        });
    dsCat = std::string{TypeInfo<app_ds::TreeInstance>::name};
    dsCho = extractChoices<app_ds::TreeInstance>();
    dsTbl[dsCat] = CategoryExtAttr{dsCho, app_ds::TreeInstance{}};
    subCLIAppDs
        .addArgument("-" + std::string{getAlias<app_ds::DataStructureChoice, app_ds::TreeInstance>()}, "--" + dsCat)
        .argsNum(0, dsCho.size())
        .defaultVal<std::vector<std::string>>(std::move(dsCho))
        .remaining()
        .metavar("OPT")
        .help(getDescr<app_ds::TreeInstance>());
    applyMessage.registerHandler(
        [](const apply::UpdateChoice<app_ds::TreeInstance>& msg)
        {
            app_ds::updateChoice<app_ds::TreeInstance>(msg.cho);
        });
    applyMessage.registerHandler(
        [](const apply::RunChoices<app_ds::TreeInstance>& msg)
        {
            app_ds::runChoices<app_ds::TreeInstance>(msg.coll);
        });
    mainCLI.addSubParser(subCLIAppDs);

    auto& numTbl = regularChoices[subCLIAppNum.title()];
    subCLIAppNum.addDescription(getDescr<app_num::NumericChoice>());
    subCLIAppNum.addArgument("-h", "--help").argsNum(0).implicitVal(true).help("show help and exit");
    auto numCat = std::string{TypeInfo<app_num::ArithmeticMethod>::name};
    auto numCho = extractChoices<app_num::ArithmeticMethod>();
    numTbl[numCat] = CategoryExtAttr{numCho, app_num::ArithmeticMethod{}};
    subCLIAppNum
        .addArgument("-" + std::string{getAlias<app_num::NumericChoice, app_num::ArithmeticMethod>()}, "--" + numCat)
        .argsNum(0, numCho.size())
        .defaultVal<std::vector<std::string>>(std::move(numCho))
        .remaining()
        .metavar("OPT")
        .help(getDescr<app_num::ArithmeticMethod>());
    applyMessage.registerHandler(
        [](const apply::UpdateChoice<app_num::ArithmeticMethod>& msg)
        {
            app_num::updateChoice<app_num::ArithmeticMethod>(msg.cho);
        });
    applyMessage.registerHandler(
        [](const apply::RunChoices<app_num::ArithmeticMethod>& msg)
        {
            app_num::runChoices<app_num::ArithmeticMethod>(msg.coll);
        });
    numCat = std::string{TypeInfo<app_num::DivisorMethod>::name};
    numCho = extractChoices<app_num::DivisorMethod>();
    numTbl[numCat] = CategoryExtAttr{numCho, app_num::DivisorMethod{}};
    subCLIAppNum
        .addArgument("-" + std::string{getAlias<app_num::NumericChoice, app_num::DivisorMethod>()}, "--" + numCat)
        .argsNum(0, numCho.size())
        .defaultVal<std::vector<std::string>>(std::move(numCho))
        .remaining()
        .metavar("OPT")
        .help(getDescr<app_num::DivisorMethod>());
    applyMessage.registerHandler(
        [](const apply::UpdateChoice<app_num::DivisorMethod>& msg)
        {
            app_num::updateChoice<app_num::DivisorMethod>(msg.cho);
        });
    applyMessage.registerHandler(
        [](const apply::RunChoices<app_num::DivisorMethod>& msg)
        {
            app_num::runChoices<app_num::DivisorMethod>(msg.coll);
        });
    numCat = std::string{TypeInfo<app_num::IntegralMethod>::name};
    numCho = extractChoices<app_num::IntegralMethod>();
    numTbl[numCat] = CategoryExtAttr{numCho, app_num::IntegralMethod{}};
    subCLIAppNum
        .addArgument("-" + std::string{getAlias<app_num::NumericChoice, app_num::IntegralMethod>()}, "--" + numCat)
        .argsNum(0, numCho.size())
        .defaultVal<std::vector<std::string>>(std::move(numCho))
        .remaining()
        .metavar("OPT")
        .help(getDescr<app_num::IntegralMethod>());
    applyMessage.registerHandler(
        [](const apply::UpdateChoice<app_num::IntegralMethod>& msg)
        {
            app_num::updateChoice<app_num::IntegralMethod>(msg.cho);
        });
    applyMessage.registerHandler(
        [](const apply::RunChoices<app_num::IntegralMethod>& msg)
        {
            app_num::runChoices<app_num::IntegralMethod>(msg.coll);
        });
    numCat = std::string{TypeInfo<app_num::PrimeMethod>::name};
    numCho = extractChoices<app_num::PrimeMethod>();
    numTbl[numCat] = CategoryExtAttr{numCho, app_num::PrimeMethod{}};
    subCLIAppNum.addArgument("-" + std::string{getAlias<app_num::NumericChoice, app_num::PrimeMethod>()}, "--" + numCat)
        .argsNum(0, numCho.size())
        .defaultVal<std::vector<std::string>>(std::move(numCho))
        .remaining()
        .metavar("OPT")
        .help(getDescr<app_num::PrimeMethod>());
    applyMessage.registerHandler(
        [](const apply::UpdateChoice<app_num::PrimeMethod>& msg)
        {
            app_num::updateChoice<app_num::PrimeMethod>(msg.cho);
        });
    applyMessage.registerHandler(
        [](const apply::RunChoices<app_num::PrimeMethod>& msg)
        {
            app_num::runChoices<app_num::PrimeMethod>(msg.coll);
        });
    mainCLI.addSubParser(subCLIAppNum);
}

void Command::foregroundHandler(const int argc, const char* const argv[])
try
{
    std::unique_lock<std::mutex> lock(mtx);
    mainCLI.parseArgs(argc, argv);
    validate();

    isParsed.store(true);
    lock.unlock();
    cv.notify_one();
}
catch (const std::exception& err)
{
    isParsed.store(true);
    cv.notify_one();
    LOG_WRN << err.what();
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

    if (anySelected())
    {
        dispatch();
    }
}
catch (const std::exception& err)
{
    LOG_WRN << err.what();
}

void Command::validate()
{
    for (std::uint8_t i = 0; i < Bottom<Category>::value; ++i)
    {
        if (!mainCLI.isUsed(std::next(basicCategories.cbegin(), Category(i))->first))
        {
            continue;
        }
        checkForExcessiveArguments();

        dispatchManager.basicManager.categories.set(Category(i));
    }

    const auto isSubCLIUsed = [this](const auto& subCLIPair)
    {
        if (mainCLI.isSubCommandUsed(subCLIPair.first))
        {
            checkForExcessiveArguments();
            return true;
        }
        return false;
    };
    for ([[maybe_unused]] const auto& [subCLIName, subCLIMap] : regularChoices | std::views::filter(isSubCLIUsed))
    {
        const auto& subCLI = mainCLI.at<utility::argument::Argument>(subCLIName);
        if (!subCLI)
        {
            dispatchManager.regularManager.helpOnly = true;
            return;
        }
        if (subCLI.isUsed("help"))
        {
            dispatchManager.regularManager.helpOnly = true;
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
                std::visit(
                    apply::EvtTypeOverloaded{[this, target](auto&& event)
                                             {
                                                 using EventType = std::decay_t<decltype(event)>;
                                                 applyMessage.onMessage(apply::UpdateChoice<EventType>{target});
                                             }},
                    categoryAttr.event);
            }
        }
    }
}

bool Command::anySelected() const
{
    return !dispatchManager.empty();
}

void Command::dispatch()
{
    if (!dispatchManager.basicManager.empty())
    {
        for (std::uint8_t i = 0; i < Bottom<Category>::value; ++i)
        {
            if (dispatchManager.basicManager.categories.test(Category(i)))
            {
                (this->*std::next(basicCategories.cbegin(), Category(i))->second)();
            }
        }
    }
    else if (!dispatchManager.regularManager.empty())
    {
        if (dispatchManager.regularManager.helpOnly)
        {
            const auto isSubCLIUsed = [this](const auto& subCLIPair)
            {
                return mainCLI.isSubCommandUsed(subCLIPair.first);
            };

            for (const auto& subCLIName : std::views::keys(regularChoices | std::views::filter(isSubCLIUsed)))
            {
                const auto& subCLI = mainCLI.at<utility::argument::Argument>(subCLIName);
                std::cout << subCLI.help().str() << std::flush;
                return;
            }
            return;
        }

        for (const auto& categoryAttr : std::views::values(
                 std::next(regularChoices.cbegin(), dispatchManager.regularManager.getExistingOrder())->second))
        {
            const auto& candidates = categoryAttr.choices;
            std::visit(
                apply::EvtTypeOverloaded{[this, candidates](auto&& event)
                                         {
                                             using EventType = std::decay_t<decltype(event)>;
                                             applyMessage.onMessage(apply::RunChoices<EventType>{candidates});
                                         }},
                categoryAttr.event);
        }
    }
}

template <typename T>
Command::ChoiceContainer Command::extractChoices()
{
    using TypeInfo = utility::reflection::TypeInfo<T>;

    ChoiceContainer choices;
    TypeInfo::fields.forEach(
        [&choices](auto field)
        {
            choices.emplace_back(field.attrs.find(REFLECTION_STR("choice")).value);
        });

    return choices;
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
            if ((0 != length) && VIEW_TLV_PACKET(buffer, length).stopTag)
            {
                client->setNonBlocking();
            }
        }
        catch (const std::exception& err)
        {
            LOG_WRN << err.what();
        }
        VIEW_AWAKEN;
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
            if ((0 != length) && VIEW_TLV_PACKET(buffer, length).stopTag)
            {
                client->setNonBlocking();
            }
        }
        catch (const std::exception& err)
        {
            LOG_WRN << err.what();
        }
        VIEW_AWAKEN;
    };
    client->toReceive();
    client->toConnect(VIEW_UDP_HOST, VIEW_UDP_PORT);
}

void Command::executeInConsole() const
{
    if (!CONFIG_ACTIVATE_HELPER)
    {
        std::cout << "exit" << std::endl;
        return;
    }

    const auto& pendingInputs =
        mainCLI.get<std::vector<std::string>>(std::next(basicCategories.cbegin(), Category::console)->first);
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
    std::cout << config::getDefaultConfiguration() << std::endl;
}

void Command::showVersionIcon() const
{
    validateDependenciesVersion();

    const std::string fullIcon = std::format(
        "tput rev ; echo '{}{}{}' ; tput sgr0 ; echo ; echo '{}' ; echo 'Built with {} for {} on {}.'",
        getIconBanner(),
#ifndef NDEBUG
        "            DEBUG VERSION ",
#else
        "          RELEASE VERSION ",
#endif // NDEBUG
        mainCLI.version() + ' ',
        note::copyright(),
        note::compiler(),
        note::processor(),
        note::buildDate());

    std::cout << utility::io::executeCommand(fullIcon) << std::flush;
}

void Command::checkForExcessiveArguments()
{
    if (anySelected())
    {
        dispatchManager.reset();
        throw std::invalid_argument("Excessive arguments.");
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

    std::cout << utility::io::executeCommand("tput bel ; echo '" + getIconBanner() + "' ; sleep 0.1s") << std::flush;
    auto tcpClient = std::make_shared<utility::socket::TCPSocket>();
    launchClient(tcpClient);
    const char* const userEnv = std::getenv("USER");
    const std::string user = (nullptr != userEnv) ? userEnv : "USER";
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
        }
        catch (const std::exception& err)
        {
            LOG_WRN << err.what();
        }
        console.setGreeting(greeting);
        utility::time::millisecondLevelSleep(latency);
    }
    while (quit != retVal);

    tcpClient->toSend(utility::common::base64Encode("stop"));
    tcpClient->waitIfAlive();
#ifndef NDEBUG
    LOG_DBG << "Exit console mode.";
#endif // NDEBUG
}
catch (const std::exception& err)
{
    LOG_ERR << err.what();
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
                triggerHelper<log::Log>(ExtEvent::reset);
                triggerHelper<log::Log>(ExtEvent::start);

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
                triggerHelper<view::View>(ExtEvent::reset);
                triggerHelper<view::View>(ExtEvent::start);

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
            mainCLI.version().data(),
            utility::argument::version(),
            utility::common::version(),
            utility::console::version(),
            utility::currying::version(),
            utility::fsm::version(),
            utility::io::version(),
            utility::json::version(),
            utility::memory::version(),
            utility::reflection::version(),
            utility::socket::version(),
            utility::thread::version(),
            utility::time::version())
        || !allStrEqual(
            subCLIAppAlgo.version().data(),
            app_algo::match::version,
            app_algo::notation::version,
            app_algo::optimal::version,
            app_algo::search::version,
            app_algo::sort::version)
        || !allStrEqual(
            subCLIAppDp.version().data(),
            app_dp::behavioral::version,
            app_dp::creational::version,
            app_dp::structural::version)
        || !allStrEqual(subCLIAppDs.version().data(), app_ds::linear::version, app_ds::tree::version)
        || !allStrEqual(
            subCLIAppNum.version().data(),
            app_num::arithmetic::version,
            app_num::divisor::version,
            app_num::integral::version,
            app_num::prime::version))
    {
        throw std::runtime_error(std::format(
            "Dependencies version number mismatch. Expected main version: {} ({})"
            ", sub-version: {} ({}), {} ({}), {} ({}), {} ({}).",
            mainCLI.title(),
            mainCLI.version(),
            subCLIAppAlgo.title(),
            subCLIAppAlgo.version(),
            subCLIAppDp.title(),
            subCLIAppDp.version(),
            subCLIAppDs.title(),
            subCLIAppDs.version(),
            subCLIAppNum.title(),
            subCLIAppNum.version()));
    }
}

std::string Command::getIconBanner()
{
    std::string banner;
    banner += R"(  ______   ______     ______    \n)";
    banner += R"( /\  ___\ /\  __ \   /\  __ \   \n)";
    banner += R"( \ \  __\ \ \ \/\ \  \ \ \/\ \  \n)";
    banner += R"(  \ \_\    \ \_____\  \ \_____\ \n)";
    banner += R"(   \/_/     \/_____/   \/_____/ \n)";

    return banner;
}
} // namespace application::command
