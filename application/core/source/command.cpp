//! @file command.cpp
//! @author ryftchen
//! @brief The definitions (command) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2026 ryftchen. All rights reserved.

#include "command.hpp"
#include "log.hpp"
#include "view.hpp"

#ifndef _PRECOMPILED_HEADER
#include <barrier>
#include <latch>
#include <numeric>
#include <ranges>
#else
#include "application/pch/precompiled_header.hpp"
#endif

#include "utility/include/benchmark.hpp"
#include "utility/include/currying.hpp"
#include "utility/include/time.hpp"

namespace application
{
namespace command
{
//! @brief Alias for the native categories.
using Category = schedule::native::Category;
//! @brief Alias for the representing of the maximum value of an enum.
//! @tparam Enum - type of specific enum
template <typename Enum>
using Bottom = schedule::native::Bottom<Enum>;
//! @brief Alias for the local notifier.
using LocalNotifier = schedule::native::Notifier<Command>;

//! @brief Anonymous namespace.
inline namespace
{
//! @brief The semaphore that controls the maximum access limit.
std::counting_semaphore<1> cliSem(1); // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
} // namespace

//! @brief Manage external helpers.
namespace help
{
//! @brief Constraint for external helpers.
//! @tparam Type - type of helper
template <typename Type>
concept ExtHelper =
    !std::is_constructible_v<Type> && !std::is_copy_constructible_v<Type> && !std::is_copy_assignable_v<Type>
    && !std::is_move_constructible_v<Type> && !std::is_move_assignable_v<Type> && requires (const Type& /*helper*/) {
           { Type::getInstance() } -> std::same_as<std::shared_ptr<Type>>;
       };

//! @brief Enumerate specific events to control external helpers.
enum class ExtEvent : std::uint8_t
{
    //! @brief Startup.
    startup,
    //! @brief Shutdown.
    shutdown,
    //! @brief Reload.
    reload
};

//! @brief Trigger the external helper with event.
//! @tparam Helper - type of helper
//! @param event - target event
template <ExtHelper Helper>
requires std::derived_from<Helper, utility::fsm::FSM<Helper>>
static void triggerEvent(const ExtEvent event)
{
    if (!configure::detail::activateHelper())
    {
        return;
    }

    switch (event)
    {
        case ExtEvent::startup:
            typename Helper::Controller().startup();
            return;
        case ExtEvent::shutdown:
            typename Helper::Controller().shutdown();
            return;
        case ExtEvent::reload:
            typename Helper::Controller().reload();
            return;
        default:
            break;
    }
}

//! @brief Helper daemon service.
//! @tparam Helpers - type of arguments of helper
template <ExtHelper... Helpers>
requires (std::derived_from<Helpers, utility::fsm::FSM<Helpers>> && ...)
static void daemonService()
{
    utility::thread::Thread extendedJob(sizeof...(Helpers));
    (extendedJob.enqueue(Helpers::name, &Helpers::service, Helpers::getInstance()), ...);
}

// NOLINTBEGIN(readability-static-accessed-through-instance)
//! @brief Coroutine for managing the lifecycle of helper components.
//! @tparam Hs - type of helpers
//! @return awaitable instance
template <typename... Hs>
static schedule::Awaitable launchLifecycle()
{
    if (!configure::detail::activateHelper())
    {
        co_return;
    }

    std::latch waitPoint(1);
    const std::jthread daemon(
        [&waitPoint]()
        {
            daemonService<Hs...>();
            waitPoint.count_down();
        });
    std::barrier syncPoint(sizeof...(Hs) + 1);
    static constexpr auto publish = [](std::barrier<>& phase, const ExtEvent event) constexpr
    {
        std::vector<std::jthread> senders{};
        senders.reserve(sizeof...(Hs));
        (senders.emplace_back(std::jthread{[&phase, event]()
                                           {
                                               triggerEvent<Hs>(event);
                                               phase.arrive_and_wait();
                                           }}),
         ...);
        phase.arrive_and_wait();
    };

    co_await std::suspend_always{};
    publish(syncPoint, ExtEvent::startup);
    co_await std::suspend_always{};
    publish(syncPoint, ExtEvent::shutdown);

    waitPoint.wait();
}
// NOLINTEND(readability-static-accessed-through-instance)
} // namespace help

// clang-format off
//! @brief Mapping table for enum and attribute about command categories. X macro.
#define COMMAND_CATEGORY_X_MACRO_MAPPING                                \
    X(Category::console, "c", "run options in console mode and exit\n"  \
                              "separate with quotes"                  ) \
    X(Category::dump   , "d", "dump default configuration and exit"   ) \
    X(Category::help   , "h", "show this help message and exit"       ) \
    X(Category::version, "v", "show version information and exit"     )
// clang-format on

//! @brief Map the alias name.
//! @param cat - native category
//! @return alias name
static consteval std::string_view mappedAlias(const Category cat)
{
//! @cond
#define X(enum, descr, alias) {descr, alias},
    constexpr std::string_view table[][2] = {COMMAND_CATEGORY_X_MACRO_MAPPING};
    static_assert((sizeof(table) / sizeof(table[0])) == Bottom<Category>::value);
    return table[static_cast<std::underlying_type_t<Category>>(cat)][0];
//! @endcond
#undef X
}

//! @brief Map the description.
//! @param cat - native category
//! @return description
static consteval std::string_view mappedDescr(const Category cat)
{
//! @cond
#define X(enum, descr, alias) {descr, alias},
    constexpr std::string_view table[][2] = {COMMAND_CATEGORY_X_MACRO_MAPPING};
    static_assert((sizeof(table) / sizeof(table[0])) == Bottom<Category>::value);
    return table[static_cast<std::underlying_type_t<Category>>(cat)][1];
//! @endcond
#undef X
}

#undef COMMAND_CATEGORY_X_MACRO_MAPPING

//! @brief Convert category enumeration to string.
//! @param cat - native category
//! @return category name
static constexpr std::string_view toString(const Category cat)
{
    constexpr std::array<std::string_view, Bottom<Category>::value> stringify = {
        MACRO_STRINGIFY(console), MACRO_STRINGIFY(dump), MACRO_STRINGIFY(help), MACRO_STRINGIFY(version)};
    return stringify.at(static_cast<std::underlying_type_t<Category>>(cat));
}

//! @brief Get the Command instance.
//! @return reference of the Command object
Command& getInstance()
{
    static Command commander{};
    return commander;
}

Command::Command()
{
    initializeNativeCLI();
    initializeExtraCLI();
}

Command::~Command()
{
    clearSelected();
}

bool Command::execute(const int argc, const char* const argv[])
try
{
    isFaulty.store(false);
    isParsed.store(false);
    auto helpCtrl = help::launchLifecycle<log::Log, view::View>();
    schedule::enterNextPhase(helpCtrl);

    if (argc > 1)
    {
        constexpr std::uint8_t endNum = 2;
        utility::thread::Thread scheduledJob(endNum);
        scheduledJob.enqueue(title + "-front", &Command::frontEndHandler, this, argc, argv);
        scheduledJob.enqueue(title + "-back", &Command::backEndHandler, this);
    }
    else
    {
        enterConsoleMode();
    }

    schedule::enterNextPhase(helpCtrl);
    return !isFaulty.load();
}
catch (const std::exception& err)
{
    isFaulty.store(true);
    LOG_ERR << err.what();
    return !isFaulty.load();
}

void Command::mainCLISetup()
{
    using ArgsNumPattern = utility::argument::ArgsNumPattern;
    mainCLI
        .addArgument(
            shortPrefix + std::string{mappedAlias(Category::help)}, longPrefix + std::string{toString(Category::help)})
        .argsNum(0)
        .implicitValue(true)
        .help(mappedDescr(Category::help));
    builtInNotifier.attach(Category::help, std::make_shared<LocalNotifier::Handler<Category::help>>(*this));
    mainCLI
        .addArgument(
            shortPrefix + std::string{mappedAlias(Category::version)},
            longPrefix + std::string{toString(Category::version)})
        .argsNum(0)
        .implicitValue(true)
        .help(mappedDescr(Category::version));
    builtInNotifier.attach(Category::version, std::make_shared<LocalNotifier::Handler<Category::version>>(*this));
    mainCLI
        .addArgument(
            shortPrefix + std::string{mappedAlias(Category::dump)}, longPrefix + std::string{toString(Category::dump)})
        .argsNum(0)
        .implicitValue(true)
        .help(mappedDescr(Category::dump));
    builtInNotifier.attach(Category::dump, std::make_shared<LocalNotifier::Handler<Category::dump>>(*this));
    mainCLI
        .addArgument(
            shortPrefix + std::string{mappedAlias(Category::console)},
            longPrefix + std::string{toString(Category::console)})
        .argsNum(ArgsNumPattern::any)
        .defaultValue<std::vector<std::string>>({"usage"})
        .appending()
        .action(
            [](const std::string& input)
            {
                if (std::ranges::all_of(input, [l = std::locale{}](const auto c) { return std::isspace(c, l); }))
                {
                    throw std::runtime_error{"Invalid " + std::string{toString(Category::console)} + " command."};
                }
                return input;
            })
        .metaVariable("CMD")
        .help(mappedDescr(Category::console));
    builtInNotifier.attach(Category::console, std::make_shared<LocalNotifier::Handler<Category::console>>(*this));
}

template <typename Mapped>
utility::argument::Argument& Command::resolveSubCLI()
{
    if constexpr (std::is_same_v<Mapped, reg_algo::ApplyAlgorithm> || reg_algo::Registrant<Mapped>)
    {
        return subCLIAppAlgo;
    }
    else if constexpr (std::is_same_v<Mapped, reg_dp::ApplyDesignPattern> || reg_dp::Registrant<Mapped>)
    {
        return subCLIAppDp;
    }
    else if constexpr (std::is_same_v<Mapped, reg_ds::ApplyDataStructure> || reg_ds::Registrant<Mapped>)
    {
        return subCLIAppDs;
    }
    else if constexpr (std::is_same_v<Mapped, reg_num::ApplyNumeric> || reg_num::Registrant<Mapped>)
    {
        return subCLIAppNum;
    }
    throw std::runtime_error{"Invalid sub-command registration."};
}

template <typename SubCLI, typename Intf>
void Command::injectNewSubCLI(Intf&& intf)
{
    using schedule::meta::descr;
    auto& subCLI = resolveSubCLI<SubCLI>();
    auto& checklist = scheduleDispatcher.extraChecklist;
    checklist.emplace(subCLI.title(), std::forward<Intf>(intf));
    subCLI.addDescription(descr<SubCLI>());
    subCLI
        .addArgument(
            shortPrefix + std::string{mappedAlias(Category::help)}, longPrefix + std::string{toString(Category::help)})
        .argsNum(0)
        .implicitValue(true)
        .help(mappedDescr(Category::help));
    mainCLI.addSubParser(subCLI);
}

// NOLINTBEGIN(google-build-using-namespace)
template <typename Cat>
void Command::addNewCategoryToSubCLI(const std::string_view version)
{
    using namespace reg_algo;
    using namespace reg_dp;
    using namespace reg_ds;
    using namespace reg_num;
    using Attr = schedule::ExtraManager::Attr;
    using schedule::extra::SetChoice, schedule::extra::RunCandidates, schedule::meta::name, schedule::meta::alias,
        schedule::meta::descr, schedule::meta::choice;
    auto& subCLI = resolveSubCLI<Cat>();
    auto& registry = scheduleDispatcher.extraChoiceRegistry[subCLI.title()];
    auto candidates = choice<Cat>();
    registry.emplace(name<Cat>(), Attr{candidates, Cat{}});
    subCLI.addArgument(shortPrefix + std::string{alias<Cat>()}, longPrefix + std::string{name<Cat>()})
        .argsNum(0, candidates.size())
        .template defaultValue<decltype(candidates)>(std::move(candidates))
        .remaining()
        .metaVariable("OPT")
        .help(descr<Cat>());
    applyingForwarder.registerHandler([](const SetChoice<Cat>& msg) { setChoice<Cat>(msg.choice); });
    applyingForwarder.registerHandler([](const RunCandidates<Cat>& msg) { runCandidates<Cat>(msg.candidates); });
    versionLinks.emplace(VerLinkKey{subCLI.title(), version}, name<Cat>());
}

//! @brief Set up the sub-command line interface (algorithm module).
template <>
void Command::subCLISetup<reg_algo::ApplyAlgorithm>()
{
    using namespace reg_algo;
    using Intf = schedule::ExtraManager::Intf;
    injectNewSubCLI<ApplyAlgorithm>(Intf{manage::present, manage::clear});
    addNewCategoryToSubCLI<MatchMethod>(match::version());
    addNewCategoryToSubCLI<NotationMethod>(notation::version());
    addNewCategoryToSubCLI<OptimalMethod>(optimal::version());
    addNewCategoryToSubCLI<SearchMethod>(search::version());
    addNewCategoryToSubCLI<SortMethod>(sort::version());
}

//! @brief Set up the sub-command line interface (design pattern module).
template <>
void Command::subCLISetup<reg_dp::ApplyDesignPattern>()
{
    using namespace reg_dp;
    using Intf = schedule::ExtraManager::Intf;
    injectNewSubCLI<ApplyDesignPattern>(Intf{manage::present, manage::clear});
    addNewCategoryToSubCLI<BehavioralInstance>(behavioral::version());
    addNewCategoryToSubCLI<CreationalInstance>(creational::version());
    addNewCategoryToSubCLI<StructuralInstance>(structural::version());
}

//! @brief Set up the sub-command line interface (data structure module).
template <>
void Command::subCLISetup<reg_ds::ApplyDataStructure>()
{
    using namespace reg_ds;
    using Intf = schedule::ExtraManager::Intf;
    injectNewSubCLI<ApplyDataStructure>(Intf{manage::present, manage::clear});
    addNewCategoryToSubCLI<CacheInstance>(cache::version());
    addNewCategoryToSubCLI<FilterInstance>(filter::version());
    addNewCategoryToSubCLI<GraphInstance>(graph::version());
    addNewCategoryToSubCLI<HeapInstance>(heap::version());
    addNewCategoryToSubCLI<LinearInstance>(linear::version());
    addNewCategoryToSubCLI<TreeInstance>(tree::version());
}

//! @brief Set up the sub-command line interface (numeric module).
template <>
void Command::subCLISetup<reg_num::ApplyNumeric>()
{
    using namespace reg_num;
    using Intf = schedule::ExtraManager::Intf;
    injectNewSubCLI<ApplyNumeric>(Intf{manage::present, manage::clear});
    addNewCategoryToSubCLI<ArithmeticMethod>(arithmetic::version());
    addNewCategoryToSubCLI<DivisorMethod>(divisor::version());
    addNewCategoryToSubCLI<IntegralMethod>(integral::version());
    addNewCategoryToSubCLI<PrimeMethod>(prime::version());
}
// NOLINTEND(google-build-using-namespace)

void Command::initializeNativeCLI()
{
    mainCLISetup();
}

void Command::initializeExtraCLI()
{
    subCLISetup<reg_algo::ApplyAlgorithm>();
    subCLISetup<reg_dp::ApplyDesignPattern>();
    subCLISetup<reg_ds::ApplyDataStructure>();
    subCLISetup<reg_num::ApplyNumeric>();
}

void Command::frontEndHandler(const int argc, const char* const argv[])
try
{
    std::unique_lock<std::mutex> parserLock(parserMtx);
    mainCLI.clearUsed();
    mainCLI.parseArgs(argc, argv);
    precheck();

    isParsed.store(true);
    parserLock.unlock();
    parserCond.notify_one();
}
catch (const std::exception& err)
{
    isParsed.store(true);
    parserCond.notify_one();
    isFaulty.store(true);
    LOG_WRN << err.what();
}

void Command::backEndHandler()
try
{
    if (std::unique_lock<std::mutex> parserLock(parserMtx); true)
    {
        parserCond.wait(parserLock, [this]() { return isParsed.load(); });
    }

    if (anySelected())
    {
        dispatchAll();
        clearSelected();
    }
}
catch (const std::exception& err)
{
    clearSelected();
    isFaulty.store(true);
    LOG_WRN << err.what();
}

void Command::precheck()
{
    for (auto& spec = scheduleDispatcher.nativeCategories;
         const auto index : std::views::iota(0U, spec.size())
             | std::views::filter([this](const auto i) { return mainCLI.isUsed(toString(static_cast<Category>(i))); }))
    {
        checkExcessArgs();
        spec.set(index);
    }

    for (constexpr auto helpArgName = toString(Category::help);
         [[maybe_unused]] const auto& [subCLIName, categoryMap] : scheduleDispatcher.extraChoiceRegistry
             | std::views::filter([this](const auto& subCLIPair)
                                  { return mainCLI.isSubcommandUsed(subCLIPair.first) && (checkExcessArgs(), true); }))
    {
        const auto& subCLI = mainCLI.at<utility::argument::Argument>(subCLIName);
        const bool notAssigned = !subCLI;
        scheduleDispatcher.extraHelping = notAssigned || subCLI.isUsed(helpArgName);
        if (notAssigned)
        {
            return;
        }

        for ([[maybe_unused]] const auto& [categoryName, categoryAttr] : categoryMap
                 | std::views::filter([this, &subCLI](const auto& categoryPair)
                                      { return subCLI.isUsed(categoryPair.first) && (checkExcessArgs(), true); }))
        {
            for (const auto& choice : subCLI.get<std::vector<std::string>>(categoryName))
            {
                using schedule::extra::SetChoice;
                utility::common::patternMatch(
                    categoryAttr.event,
                    [this, &choice](auto&& event)
                    { applyingForwarder.onMessage(SetChoice<std::decay_t<decltype(event)>>{choice}); });
            }
        }
    }
}

bool Command::anySelected() const
{
    return !scheduleDispatcher.empty();
}

void Command::clearSelected()
{
    scheduleDispatcher.reset();
}

void Command::dispatchAll()
{
    if (!scheduleDispatcher.NativeManager::empty())
    {
        for (const auto& spec = scheduleDispatcher.nativeCategories;
             const auto index :
             std::views::iota(0U, spec.size()) | std::views::filter([&spec](const auto i) { return spec.test(i); }))
        {
            builtInNotifier.notify(static_cast<Category>(index));
        }
    }

    if (!scheduleDispatcher.ExtraManager::empty())
    {
        if (scheduleDispatcher.extraHelping)
        {
            if (auto whichUsed = std::views::keys(scheduleDispatcher.extraChoiceRegistry)
                    | std::views::filter([this](const auto& subCLIName)
                                         { return mainCLI.isSubcommandUsed(subCLIName); });
                std::ranges::distance(whichUsed) != 0)
            {
                std::cout << mainCLI.at<utility::argument::Argument>(*std::ranges::begin(whichUsed)).help().str()
                          << std::flush;
            }
            return;
        }

        for ([[maybe_unused]] const auto& [categoryName, categoryAttr] : scheduleDispatcher.extraChoiceRegistry
                 | std::views::filter([this](const auto& subCLIPair)
                                      { return scheduleDispatcher.extraChecklist.at(subCLIPair.first).present(); })
                 | std::views::values | std::views::join)
        {
            using schedule::extra::RunCandidates;
            utility::common::patternMatch(
                categoryAttr.event,
                [this, &candidates = categoryAttr.choices](auto&& event)
                { applyingForwarder.onMessage(RunCandidates<std::decay_t<decltype(event)>>{candidates}); });
        }
    }
}

void Command::checkExcessArgs()
{
    if (anySelected())
    {
        clearSelected();
        throw std::runtime_error{"Excessive arguments were found."};
    }
}

//! @brief Launch the TCP client for console mode.
//! @param client - TCP client to be launched
template <>
void Command::launchClient<utility::socket::TCPSocket>(std::shared_ptr<utility::socket::TCPSocket>& client)
{
    client->subscribeRawMessage(
        [&client](char* const bytes, const std::size_t size)
        {
            try
            {
                MACRO_DEFER([]() { notifyClientOutputDone(); });
                if (client->stopRequested() || onParsing4Client(bytes, size))
                {
                    return;
                }
                client->requestStop();
            }
            catch (const std::exception& err)
            {
                LOG_WRN << err.what();
            }
        });
    client->connect(view::insight::currentTCPHost(), view::insight::currentTCPPort());
}

//! @brief Launch the UDP client for console mode.
//! @param client - UDP client to be launched
template <>
void Command::launchClient<utility::socket::UDPSocket>(std::shared_ptr<utility::socket::UDPSocket>& client)
{
    client->subscribeRawMessage(
        [&client](char* const bytes, const std::size_t size, const std::string& /*ip*/, const std::uint16_t /*port*/)
        {
            try
            {
                MACRO_DEFER([]() { notifyClientOutputDone(); });
                if (client->stopRequested() || onParsing4Client(bytes, size))
                {
                    return;
                }
                client->requestStop();
            }
            catch (const std::exception& err)
            {
                LOG_WRN << err.what();
            }
        });
    client->receive();
    client->connect(view::insight::currentUDPHost(), view::insight::currentUDPPort());
}

void Command::executeInConsole() const
{
    if (!configure::detail::activateHelper())
    {
        std::cout << "exit" << std::endl;
        return;
    }

    const auto pendingInputs = mainCLI.get<std::vector<std::string>>(toString(Category::console));
    if (pendingInputs.empty())
    {
        return;
    }

    constexpr std::string_view greeting = "> ";
    const auto session = std::make_unique<console::Console>(greeting);
    auto tempClient = std::make_shared<utility::socket::UDPSocket>();
    launchClient(tempClient);
    registerOnConsole(*session, tempClient);

    for (std::ostringstream out{}; const auto& input : pendingInputs)
    {
        try
        {
            using RetCode = console::Console::RetCode;
            out << greeting << input << '\n';
            std::cout << out.str() << std::flush;
            out.str("");
            out.clear();
            if (session->optionExecutor(input) == RetCode::quit)
            {
                break;
            }
        }
        catch (const std::exception& err)
        {
            LOG_WRN << err.what();
            interactionLatency();
        }
    }
    tempClient->send(buildDisconnectRequest());
    tempClient->join();
    interactionLatency();
}

void Command::showHelpMessage() const
{
    std::cout << mainCLI.help().str() << std::flush;
}

void Command::dumpConfiguration()
{
    std::cout << configure::dumpDefaultConfig() << std::endl;
}

void Command::displayVersionInfo() const
{
    validateDependencies();

    const auto briefReview = std::format(
        "\033[7m\033[49m{}"
#ifndef NDEBUG
        "            DEBUG VERSION {} "
#else
        "          RELEASE VERSION {} "
#endif
        "\033[0m\nBuilt {} with {} for {} on {}.\n{}\n",
        build::banner(),
        mainCLI.version(),
        build::revision(),
        build::compiler(),
        build::processor(),
        build::date(),
        build::copyright());
    std::cout << briefReview << std::flush;
}

void Command::validateDependencies() const
{
    const bool isNativeVerMatched = utility::common::areStringsEqual(
        mainCLI.version().data(),
        utility::argument::version(),
        utility::benchmark::version(),
        utility::common::version(),
        utility::currying::version(),
        utility::fsm::version(),
        utility::io::version(),
        utility::json::version(),
        utility::macro::version(),
        utility::memory::version(),
        utility::reflection::version(),
        utility::socket::version(),
        utility::thread::version(),
        utility::time::version());
    if (!isNativeVerMatched)
    {
        throw std::runtime_error{std::format(
            "Dependencies version number mismatch. Expected main version: {} ({}).",
            mainCLI.title(),
            mainCLI.version())};
    }

    const auto& choiceRegistry = scheduleDispatcher.extraChoiceRegistry;
    const bool isExtraVerMatched = (versionLinks.count({subCLIAppAlgo.title(), subCLIAppAlgo.version()})
                                    == choiceRegistry.at(subCLIAppAlgo.title()).size())
        && (versionLinks.count({subCLIAppDp.title(), subCLIAppDp.version()})
            == choiceRegistry.at(subCLIAppDp.title()).size())
        && (versionLinks.count({subCLIAppDs.title(), subCLIAppDs.version()})
            == choiceRegistry.at(subCLIAppDs.title()).size())
        && (versionLinks.count({subCLIAppNum.title(), subCLIAppNum.version()})
            == choiceRegistry.at(subCLIAppNum.title()).size());
    if (!isExtraVerMatched)
    {
        throw std::runtime_error{std::format(
            "Dependencies version number mismatch. Expected sub-version: {} ({}), {} ({}), {} ({}), {} ({}).",
            subCLIAppAlgo.title(),
            subCLIAppAlgo.version(),
            subCLIAppDp.title(),
            subCLIAppDp.version(),
            subCLIAppDs.title(),
            subCLIAppDs.version(),
            subCLIAppNum.title(),
            subCLIAppNum.version())};
    }
}

void Command::enterConsoleMode()
try
{
    if (!configure::detail::activateHelper())
    {
        std::cout << "exit" << std::endl;
        return;
    }
    LOG_DBG << "Enter console mode.";

    interactionLatency();
    const char* const userEnv = std::getenv("USER"); // NOLINT(concurrency-mt-unsafe)
    const std::string userName = userEnv ? userEnv : "USER";
    std::array<char, HOST_NAME_MAX> hostName{};
    if (::gethostname(hostName.data(), hostName.size()) != 0)
    {
        std::strncpy(hostName.data(), "HOSTNAME", hostName.size() - 1);
        hostName[HOST_NAME_MAX - 1] = '\0';
    }
    const auto greeting = userName + '@' + hostName.data() + " foo > ";
    const auto session = std::make_unique<console::Console>(greeting);
    auto permClient = std::make_shared<utility::socket::TCPSocket>();
    launchClient(permClient);
    registerOnConsole(*session, permClient);

    std::cout << build::banner() << std::endl;
    using RetCode = console::Console::RetCode;
    auto retCode = RetCode::success;
    do
    {
        try
        {
            retCode = session->readLine();
        }
        catch (const std::exception& err)
        {
            LOG_WRN << err.what();
        }
        session->setGreeting(greeting);
        interactionLatency();
    }
    while (retCode != RetCode::quit);
    permClient->send(buildDisconnectRequest());
    permClient->join();
    interactionLatency();

    LOG_DBG << "Exit console mode.";
}
catch (const std::exception& err)
{
    LOG_ERR << err.what();
}

auto Command::processConsoleInputs(const std::function<void()>& handling)
{
    using RetCode = console::Console::RetCode;
    auto retCode = RetCode::success;
    try
    {
        if (handling)
        {
            handling();
        }
    }
    catch (const std::exception& err)
    {
        retCode = RetCode::error;
        LOG_WRN << err.what();
    }
    interactionLatency();
    return retCode;
}

template <typename Sock>
void Command::registerOnConsole(console::Console& session, std::shared_ptr<Sock>& client)
{
    static constexpr auto gracefulReset = []<help::ExtHelper Helper>() constexpr
    {
        using namespace help; // NOLINT(google-build-using-namespace)
        triggerEvent<Helper>(ExtEvent::reload);
        triggerEvent<Helper>(ExtEvent::startup);
    };
    const auto asyncReqSender = [&client](const auto& inputs)
    {
        return processConsoleInputs(
            [&client, &inputs]()
            {
                auto reqBuffer = utility::common::base64Encode(std::accumulate(
                    inputs.cbegin(),
                    inputs.cend(),
                    std::string{},
                    [](const auto& acc, const auto& token) { return acc.empty() ? token : (acc + ' ' + token); }));
                client->send(std::move(reqBuffer));
                waitClientOutputDone();
            });
    };

    session.registerOption(
        "refresh",
        "refresh the outputs",
        [](const auto& /*inputs*/)
        {
            return processConsoleInputs(
                []()
                {
                    using log::Log;
                    gracefulReset.template operator()<Log>();
                    LOG_INF_F("Refreshed the {} outputs.", Log::name);
                });
        });
    session.registerOption(
        "reconnect",
        "reconnect to the servers",
        [&client](const auto& /*inputs*/)
        {
            return processConsoleInputs(
                [&client]()
                {
                    client->send(buildDisconnectRequest());
                    client->join();
                    interactionLatency();
                    client.reset();

                    using view::View;
                    gracefulReset.template operator()<View>();
                    client = std::make_shared<Sock>();
                    launchClient(client);
                    LOG_INF_F("Reconnected to the {} servers.", View::name);
                });
        });

    auto supportedOptions = view::insight::currentSupportedOptions();
    decltype(supportedOptions) validOptions{};
    for (auto iterator = supportedOptions.cbegin(); iterator != supportedOptions.cend();)
    {
        auto node = supportedOptions.extract(iterator++);
        auto& key = node.key();
        key.erase(
            std::ranges::remove_if(key, [l = std::locale{}](const auto c) { return std::isspace(c, l); }).begin(),
            key.cend());
        validOptions.insert(std::move(node));
    }
    for (const auto& [name, description] : validOptions)
    {
        session.registerOption(name, description, asyncReqSender);
    }
}

bool Command::onParsing4Client(char* const bytes, const std::size_t size)
{
    return (size == 0) || view::View::Controller().onParsing(bytes, size);
}

void Command::waitClientOutputDone()
{
    view::View::Completion().waitTaskDone();
}

void Command::notifyClientOutputDone()
{
    view::View::Completion().notifyTaskDone();
}

std::string Command::buildDisconnectRequest()
{
    return utility::common::base64Encode(view::exitSymbol);
}

void Command::interactionLatency()
{
    constexpr auto latency = std::chrono::milliseconds{10};
    std::this_thread::sleep_for(latency);
}

//! @brief Safely execute the command line interfaces using the given arguments.
//! @param argc - argument count
//! @param argv - argument vector
//! @return successful or failed to execute
bool executeCLI(const int argc, const char* const argv[])
try
{
    cliSem.acquire();
    const bool status = getInstance().execute(argc, argv);
    cliSem.release();
    return status;
}
catch (...)
{
    cliSem.release();
    throw;
}
} // namespace command

//! @brief Perform the specific operation for Category::console.
template <>
template <>
void command::LocalNotifier::Handler<command::Category::console>::execute() const
{
    ctx.executeInConsole();
}

//! @brief Perform the specific operation for Category::dump.
template <>
template <>
void command::LocalNotifier::Handler<command::Category::dump>::execute() const
{
    command::Command::dumpConfiguration();
}

//! @brief Perform the specific operation for Category::help.
template <>
template <>
void command::LocalNotifier::Handler<command::Category::help>::execute() const
{
    ctx.showHelpMessage();
}

//! @brief Perform the specific operation for Category::version.
template <>
template <>
void command::LocalNotifier::Handler<command::Category::version>::execute() const
{
    ctx.displayVersionInfo();
}
} // namespace application
