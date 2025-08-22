//! @file command.cpp
//! @author ryftchen
//! @brief The definitions (command) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

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
#endif // _PRECOMPILED_HEADER

#include "utility/include/benchmark.hpp"
#include "utility/include/currying.hpp"
#include "utility/include/time.hpp"

namespace application::command
{
//! @brief Anonymous namespace.
inline namespace
{
//! @brief Constraint for external helpers.
//! @tparam T - type of helper
template <typename T>
concept HelperType = !std::is_constructible_v<T> && !std::is_copy_constructible_v<T> && !std::is_copy_assignable_v<T>
    && !std::is_move_constructible_v<T> && !std::is_move_assignable_v<T> && requires (const T& /*helper*/) {
           { T::getInstance() } -> std::same_as<T&>;
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
} // namespace

//! @brief Trigger the external helper with event.
//! @tparam Helper - type of helper
//! @param event - target event
template <HelperType Helper>
requires std::derived_from<Helper, utility::fsm::FSM<Helper>>
static void triggerHelper(const ExtEvent event)
{
    if (!configure::detail::activateHelper())
    {
        return;
    }

    switch (event)
    {
        case ExtEvent::startup:
            typename Helper::Access().startup();
            return;
        case ExtEvent::shutdown:
            typename Helper::Access().shutdown();
            return;
        case ExtEvent::reload:
            typename Helper::Access().reload();
            return;
        default:
            break;
    }
}

//! @brief Helper daemon function.
//! @tparam Helpers - type of arguments of helper
template <HelperType... Helpers>
requires (std::derived_from<Helpers, utility::fsm::FSM<Helpers>> && ...)
static void helperDaemon()
{
    utility::thread::Thread extendedJob(sizeof...(Helpers));
    (extendedJob.enqueue(Helpers::name, &Helpers::service, &Helpers::getInstance()), ...);
}

//! @brief Coroutine for managing the lifecycle of helper components.
//! @tparam Hs - type of helpers
//! @return object that represents the execution of the coroutine
template <typename... Hs>
static action::Awaitable helperLifecycle()
{
    if (!configure::detail::activateHelper())
    {
        co_return;
    }

    std::latch waitPoint(1);
    const std::jthread daemon(
        [&waitPoint]()
        {
            helperDaemon<Hs...>();
            waitPoint.count_down();
        });
    std::barrier syncPoint(sizeof...(Hs) + 1);
    static const auto publish = [&syncPoint](const ExtEvent event)
    {
        std::vector<std::jthread> senders{};
        senders.reserve(sizeof...(Hs));
        (senders.emplace_back(std::jthread{[&syncPoint, event]()
                                           {
                                               triggerHelper<Hs>(event);
                                               syncPoint.arrive_and_wait();
                                           }}),
         ...);
        syncPoint.arrive_and_wait();
    };

    co_await std::suspend_always{};
    publish(ExtEvent::startup);
    co_await std::suspend_always{};
    publish(ExtEvent::shutdown);

    waitPoint.wait();
}

//! @brief Convert category enumeration to string.
//! @param cat - specific value of Category enum
//! @return category name
static constexpr std::string_view toString(const Category cat)
{
    constexpr std::array<std::string_view, Bottom<Category>::value> stringify = {
        MACRO_STRINGIFY(console), MACRO_STRINGIFY(dump), MACRO_STRINGIFY(help), MACRO_STRINGIFY(version)};
    return stringify.at(static_cast<std::uint8_t>(cat));
}

// clang-format off
//! @brief Mapping table for enum and attribute about command categories. X macro.
#define COMMAND_CATEGORY_X_MACRO_MAPPING                                \
    X(Category::console, "c", "run options in console mode and exit\n"  \
                              "separate with quotes"                  ) \
    X(Category::dump   , "d", "dump default configuration and exit"   ) \
    X(Category::help   , "h", "show this help message and exit"       ) \
    X(Category::version, "v", "show version information and exit"     )
// clang-format on
consteval std::string_view Command::getAlias(const Category cat)
{
//! @cond
#define X(enum, descr, alias) {descr, alias},
    constexpr std::string_view table[][2] = {COMMAND_CATEGORY_X_MACRO_MAPPING};
    static_assert((sizeof(table) / sizeof(table[0])) == Bottom<Category>::value);
    return table[static_cast<std::uint8_t>(cat)][0];
//! @endcond
#undef X
}

consteval std::string_view Command::getDescr(const Category cat)
{
//! @cond
#define X(enum, descr, alias) {descr, alias},
    constexpr std::string_view table[][2] = {COMMAND_CATEGORY_X_MACRO_MAPPING};
    static_assert((sizeof(table) / sizeof(table[0])) == Bottom<Category>::value);
    return table[static_cast<std::uint8_t>(cat)][1];
//! @endcond
#undef X
}
#undef COMMAND_CATEGORY_X_MACRO_MAPPING

Command::Command()
{
    initializeNativeCLI();
    initializeExtraCLI();
}

Command::~Command()
{
    taskDispatcher.reset();
}

Command& Command::getInstance()
{
    static Command commander{};
    return commander;
}

bool Command::execute(const int argc, const char* const argv[])
try
{
    auto launcher = helperLifecycle<log::Log, view::View>();
    if (!launcher.done())
    {
        launcher.resume();
    }

    if (argc == 1)
    {
        enterConsoleMode();
    }
    else
    {
        constexpr std::uint8_t endNum = 2;
        utility::thread::Thread scheduledJob(endNum);
        scheduledJob.enqueue(title + "-front", &Command::frontEndHandler, this, argc, argv);
        scheduledJob.enqueue(title + "-back", &Command::backEndHandler, this);
    }

    if (!launcher.done())
    {
        launcher.resume();
    }

    return !isFaulty.load();
}
catch (const std::exception& err)
{
    isFaulty.store(true);
    LOG_ERR << err.what();

    return !isFaulty.load();
}

void Command::setupMainCLI()
{
    using ArgsNumPattern = utility::argument::ArgsNumPattern;
    mainCLI
        .addArgument(
            shortPrefix + std::string{getAlias(Category::help)}, longPrefix + std::string{toString(Category::help)})
        .argsNum(0)
        .implicitValue(true)
        .help(getDescr(Category::help));
    builtInNotifier.attach(Category::help, std::make_shared<LocalNotifier::Handler<Category::help>>(*this));
    mainCLI
        .addArgument(
            shortPrefix + std::string{getAlias(Category::version)},
            longPrefix + std::string{toString(Category::version)})
        .argsNum(0)
        .implicitValue(true)
        .help(getDescr(Category::version));
    builtInNotifier.attach(Category::version, std::make_shared<LocalNotifier::Handler<Category::version>>(*this));
    mainCLI
        .addArgument(
            shortPrefix + std::string{getAlias(Category::dump)}, longPrefix + std::string{toString(Category::dump)})
        .argsNum(0)
        .implicitValue(true)
        .help(getDescr(Category::dump));
    builtInNotifier.attach(Category::dump, std::make_shared<LocalNotifier::Handler<Category::dump>>(*this));
    mainCLI
        .addArgument(
            shortPrefix + std::string{getAlias(Category::console)},
            longPrefix + std::string{toString(Category::console)})
        .argsNum(ArgsNumPattern::any)
        .defaultValue<std::vector<std::string>>({"usage"})
        .appending()
        .action(
            [](const std::string& input)
            {
                if (std::all_of(
                        input.cbegin(), input.cend(), [l = std::locale{}](const auto c) { return std::isspace(c, l); }))
                {
                    throw std::runtime_error{"Invalid " + std::string{toString(Category::console)} + " command."};
                }
                return input;
            })
        .metaVariable("CMD")
        .help(getDescr(Category::console));
    builtInNotifier.attach(Category::console, std::make_shared<LocalNotifier::Handler<Category::console>>(*this));
}

// NOLINTBEGIN(google-build-using-namespace, readability-function-size)
//! @brief Setup the sub-command line interface (algorithm module).
template <>
void Command::setupSubCLI<reg_algo::ApplyAlgorithm>()
{
    using namespace reg_algo;
    using Intf = ExtraManager::Intf;
    using Attr = ExtraManager::Attr;
    using action::name, action::alias, action::descr;
    constexpr std::string_view helpDescr = getDescr(Category::help);
    const std::string helpArg1 = shortPrefix + std::string{getAlias(Category::help)},
                      helpArg2 = longPrefix + std::string{toString(Category::help)};
    auto& algoTable = taskDispatcher.extraChoiceRegistry[subCLIAppAlgo.title()];
    std::vector<std::string> candidates{};

    taskDispatcher.extraChecklist.emplace(subCLIAppAlgo.title(), Intf{&manage::present, &manage::clear});
    subCLIAppAlgo.addDescription(descr<ApplyAlgorithm>());
    subCLIAppAlgo.addArgument(helpArg1, helpArg2).argsNum(0).implicitValue(true).help(helpDescr);

    candidates = extractChoices<MatchMethod>();
    algoTable.emplace(name<MatchMethod>(), Attr{candidates, MatchMethod{}});
    subCLIAppAlgo
        .addArgument(shortPrefix + std::string{alias<MatchMethod>()}, longPrefix + std::string{name<MatchMethod>()})
        .argsNum(0, candidates.size())
        .defaultValue<std::vector<std::string>>(std::move(candidates))
        .remaining()
        .metaVariable(metaVar)
        .help(descr<MatchMethod>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<MatchMethod>& msg)
                                      { updateChoice<MatchMethod>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<MatchMethod>& msg)
                                      { runChoices<MatchMethod>(msg.coll); });
    versionLinks.emplace(VerLinkKey{name<ApplyAlgorithm>(), match::version()}, name<MatchMethod>());
    candidates = extractChoices<NotationMethod>();
    algoTable.emplace(name<NotationMethod>(), Attr{candidates, NotationMethod{}});
    subCLIAppAlgo
        .addArgument(
            shortPrefix + std::string{alias<NotationMethod>()}, longPrefix + std::string{name<NotationMethod>()})
        .argsNum(0, candidates.size())
        .defaultValue<std::vector<std::string>>(std::move(candidates))
        .remaining()
        .metaVariable(metaVar)
        .help(descr<NotationMethod>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<NotationMethod>& msg)
                                      { updateChoice<NotationMethod>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<NotationMethod>& msg)
                                      { runChoices<NotationMethod>(msg.coll); });
    versionLinks.emplace(VerLinkKey{name<ApplyAlgorithm>(), notation::version()}, name<NotationMethod>());
    candidates = extractChoices<OptimalMethod>();
    algoTable.emplace(name<OptimalMethod>(), Attr{candidates, OptimalMethod{}});
    subCLIAppAlgo
        .addArgument(shortPrefix + std::string{alias<OptimalMethod>()}, longPrefix + std::string{name<OptimalMethod>()})
        .argsNum(0, candidates.size())
        .defaultValue<std::vector<std::string>>(std::move(candidates))
        .remaining()
        .metaVariable(metaVar)
        .help(descr<OptimalMethod>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<OptimalMethod>& msg)
                                      { updateChoice<OptimalMethod>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<OptimalMethod>& msg)
                                      { runChoices<OptimalMethod>(msg.coll); });
    versionLinks.emplace(VerLinkKey{name<ApplyAlgorithm>(), optimal::version()}, name<OptimalMethod>());
    candidates = extractChoices<SearchMethod>();
    algoTable.emplace(name<SearchMethod>(), Attr{candidates, SearchMethod{}});
    subCLIAppAlgo
        .addArgument(shortPrefix + std::string{alias<SearchMethod>()}, longPrefix + std::string{name<SearchMethod>()})
        .argsNum(0, candidates.size())
        .defaultValue<std::vector<std::string>>(std::move(candidates))
        .remaining()
        .metaVariable(metaVar)
        .help(descr<SearchMethod>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<SearchMethod>& msg)
                                      { updateChoice<SearchMethod>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<SearchMethod>& msg)
                                      { runChoices<SearchMethod>(msg.coll); });
    versionLinks.emplace(VerLinkKey{name<ApplyAlgorithm>(), search::version()}, name<SearchMethod>());
    candidates = extractChoices<SortMethod>();
    algoTable.emplace(name<SortMethod>(), Attr{candidates, SortMethod{}});
    subCLIAppAlgo
        .addArgument(shortPrefix + std::string{alias<SortMethod>()}, longPrefix + std::string{name<SortMethod>()})
        .argsNum(0, candidates.size())
        .defaultValue<std::vector<std::string>>(std::move(candidates))
        .remaining()
        .metaVariable(metaVar)
        .help(descr<SortMethod>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<SortMethod>& msg)
                                      { updateChoice<SortMethod>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<SortMethod>& msg)
                                      { runChoices<SortMethod>(msg.coll); });
    versionLinks.emplace(VerLinkKey{name<ApplyAlgorithm>(), sort::version()}, name<SortMethod>());

    mainCLI.addSubParser(subCLIAppAlgo);
}

//! @brief Setup the sub-command line interface (design pattern module).
template <>
void Command::setupSubCLI<reg_dp::ApplyDesignPattern>()
{
    using namespace reg_dp;
    using Intf = ExtraManager::Intf;
    using Attr = ExtraManager::Attr;
    using action::name, action::alias, action::descr;
    constexpr std::string_view helpDescr = getDescr(Category::help);
    const std::string helpArg1 = shortPrefix + std::string{getAlias(Category::help)},
                      helpArg2 = longPrefix + std::string{toString(Category::help)};
    auto& dpTable = taskDispatcher.extraChoiceRegistry[subCLIAppDp.title()];
    std::vector<std::string> candidates{};

    taskDispatcher.extraChecklist.emplace(subCLIAppDp.title(), Intf{&manage::present, &manage::clear});
    subCLIAppDp.addDescription(descr<ApplyDesignPattern>());
    subCLIAppDp.addArgument(helpArg1, helpArg2).argsNum(0).implicitValue(true).help(helpDescr);

    candidates = extractChoices<BehavioralInstance>();
    dpTable.emplace(name<BehavioralInstance>(), Attr{candidates, BehavioralInstance{}});
    subCLIAppDp
        .addArgument(
            shortPrefix + std::string{alias<BehavioralInstance>()},
            longPrefix + std::string{name<BehavioralInstance>()})
        .argsNum(0, candidates.size())
        .defaultValue<std::vector<std::string>>(std::move(candidates))
        .remaining()
        .metaVariable(metaVar)
        .help(descr<BehavioralInstance>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<BehavioralInstance>& msg)
                                      { updateChoice<BehavioralInstance>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<BehavioralInstance>& msg)
                                      { runChoices<BehavioralInstance>(msg.coll); });
    versionLinks.emplace(VerLinkKey{name<ApplyDesignPattern>(), behavioral::version()}, name<BehavioralInstance>());
    candidates = extractChoices<CreationalInstance>();
    dpTable.emplace(name<CreationalInstance>(), Attr{candidates, CreationalInstance{}});
    subCLIAppDp
        .addArgument(
            shortPrefix + std::string{alias<CreationalInstance>()},
            longPrefix + std::string{name<CreationalInstance>()})
        .argsNum(0, candidates.size())
        .defaultValue<std::vector<std::string>>(std::move(candidates))
        .remaining()
        .metaVariable(metaVar)
        .help(descr<CreationalInstance>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<CreationalInstance>& msg)
                                      { updateChoice<CreationalInstance>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<CreationalInstance>& msg)
                                      { runChoices<CreationalInstance>(msg.coll); });
    versionLinks.emplace(VerLinkKey{name<ApplyDesignPattern>(), creational::version()}, name<CreationalInstance>());
    candidates = extractChoices<StructuralInstance>();
    dpTable.emplace(name<StructuralInstance>(), Attr{candidates, StructuralInstance{}});
    subCLIAppDp
        .addArgument(
            shortPrefix + std::string{alias<StructuralInstance>()},
            longPrefix + std::string{name<StructuralInstance>()})
        .argsNum(0, candidates.size())
        .defaultValue<std::vector<std::string>>(std::move(candidates))
        .remaining()
        .metaVariable(metaVar)
        .help(descr<StructuralInstance>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<StructuralInstance>& msg)
                                      { updateChoice<StructuralInstance>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<StructuralInstance>& msg)
                                      { runChoices<StructuralInstance>(msg.coll); });
    versionLinks.emplace(VerLinkKey{name<ApplyDesignPattern>(), structural::version()}, name<StructuralInstance>());

    mainCLI.addSubParser(subCLIAppDp);
}

//! @brief Setup the sub-command line interface (data structure module).
template <>
void Command::setupSubCLI<reg_ds::ApplyDataStructure>()
{
    using namespace reg_ds;
    using Intf = ExtraManager::Intf;
    using Attr = ExtraManager::Attr;
    using action::name, action::alias, action::descr;
    constexpr std::string_view helpDescr = getDescr(Category::help);
    const std::string helpArg1 = shortPrefix + std::string{getAlias(Category::help)},
                      helpArg2 = longPrefix + std::string{toString(Category::help)};
    auto& dsTable = taskDispatcher.extraChoiceRegistry[subCLIAppDs.title()];
    std::vector<std::string> candidates{};

    taskDispatcher.extraChecklist.emplace(subCLIAppDs.title(), Intf{&manage::present, &manage::clear});
    subCLIAppDs.addDescription(descr<ApplyDataStructure>());
    subCLIAppDs.addArgument(helpArg1, helpArg2).argsNum(0).implicitValue(true).help(helpDescr);

    candidates = extractChoices<CacheInstance>();
    dsTable.emplace(name<CacheInstance>(), Attr{candidates, CacheInstance{}});
    subCLIAppDs
        .addArgument(shortPrefix + std::string{alias<CacheInstance>()}, longPrefix + std::string{name<CacheInstance>()})
        .argsNum(0, candidates.size())
        .defaultValue<std::vector<std::string>>(std::move(candidates))
        .remaining()
        .metaVariable(metaVar)
        .help(descr<CacheInstance>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<CacheInstance>& msg)
                                      { updateChoice<CacheInstance>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<CacheInstance>& msg)
                                      { runChoices<CacheInstance>(msg.coll); });
    versionLinks.emplace(VerLinkKey{name<ApplyDataStructure>(), cache::version()}, name<CacheInstance>());
    candidates = extractChoices<FilterInstance>();
    dsTable.emplace(name<FilterInstance>(), Attr{candidates, FilterInstance{}});
    subCLIAppDs
        .addArgument(
            shortPrefix + std::string{alias<FilterInstance>()}, longPrefix + std::string{name<FilterInstance>()})
        .argsNum(0, candidates.size())
        .defaultValue<std::vector<std::string>>(std::move(candidates))
        .remaining()
        .metaVariable(metaVar)
        .help(descr<FilterInstance>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<FilterInstance>& msg)
                                      { updateChoice<FilterInstance>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<FilterInstance>& msg)
                                      { runChoices<FilterInstance>(msg.coll); });
    versionLinks.emplace(VerLinkKey{name<ApplyDataStructure>(), filter::version()}, name<FilterInstance>());
    candidates = extractChoices<GraphInstance>();
    dsTable.emplace(name<GraphInstance>(), Attr{candidates, GraphInstance{}});
    subCLIAppDs
        .addArgument(shortPrefix + std::string{alias<GraphInstance>()}, longPrefix + std::string{name<GraphInstance>()})
        .argsNum(0, candidates.size())
        .defaultValue<std::vector<std::string>>(std::move(candidates))
        .remaining()
        .metaVariable(metaVar)
        .help(descr<GraphInstance>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<GraphInstance>& msg)
                                      { updateChoice<GraphInstance>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<GraphInstance>& msg)
                                      { runChoices<GraphInstance>(msg.coll); });
    versionLinks.emplace(VerLinkKey{name<ApplyDataStructure>(), graph::version()}, name<GraphInstance>());
    candidates = extractChoices<HeapInstance>();
    dsTable.emplace(name<HeapInstance>(), Attr{candidates, HeapInstance{}});
    subCLIAppDs
        .addArgument(shortPrefix + std::string{alias<HeapInstance>()}, longPrefix + std::string{name<HeapInstance>()})
        .argsNum(0, candidates.size())
        .defaultValue<std::vector<std::string>>(std::move(candidates))
        .remaining()
        .metaVariable(metaVar)
        .help(descr<HeapInstance>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<HeapInstance>& msg)
                                      { updateChoice<HeapInstance>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<HeapInstance>& msg)
                                      { runChoices<HeapInstance>(msg.coll); });
    versionLinks.emplace(VerLinkKey{name<ApplyDataStructure>(), heap::version()}, name<HeapInstance>());
    candidates = extractChoices<LinearInstance>();
    dsTable.emplace(name<LinearInstance>(), Attr{candidates, LinearInstance{}});
    subCLIAppDs
        .addArgument(
            shortPrefix + std::string{alias<LinearInstance>()}, longPrefix + std::string{name<LinearInstance>()})
        .argsNum(0, candidates.size())
        .defaultValue<std::vector<std::string>>(std::move(candidates))
        .remaining()
        .metaVariable(metaVar)
        .help(descr<LinearInstance>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<LinearInstance>& msg)
                                      { updateChoice<LinearInstance>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<LinearInstance>& msg)
                                      { runChoices<LinearInstance>(msg.coll); });
    versionLinks.emplace(VerLinkKey{name<ApplyDataStructure>(), linear::version()}, name<LinearInstance>());
    candidates = extractChoices<TreeInstance>();
    dsTable.emplace(name<TreeInstance>(), Attr{candidates, TreeInstance{}});
    subCLIAppDs
        .addArgument(shortPrefix + std::string{alias<TreeInstance>()}, longPrefix + std::string{name<TreeInstance>()})
        .argsNum(0, candidates.size())
        .defaultValue<std::vector<std::string>>(std::move(candidates))
        .remaining()
        .metaVariable(metaVar)
        .help(descr<TreeInstance>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<TreeInstance>& msg)
                                      { updateChoice<TreeInstance>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<TreeInstance>& msg)
                                      { runChoices<TreeInstance>(msg.coll); });
    versionLinks.emplace(VerLinkKey{name<ApplyDataStructure>(), tree::version()}, name<TreeInstance>());

    mainCLI.addSubParser(subCLIAppDs);
}

//! @brief Setup the sub-command line interface (numeric module).
template <>
void Command::setupSubCLI<reg_num::ApplyNumeric>()
{
    using namespace reg_num;
    using Intf = ExtraManager::Intf;
    using Attr = ExtraManager::Attr;
    using action::name, action::alias, action::descr;
    constexpr std::string_view helpDescr = getDescr(Category::help);
    const std::string helpArg1 = shortPrefix + std::string{getAlias(Category::help)},
                      helpArg2 = longPrefix + std::string{toString(Category::help)};
    auto& numTable = taskDispatcher.extraChoiceRegistry[subCLIAppNum.title()];
    std::vector<std::string> candidates{};

    taskDispatcher.extraChecklist.emplace(subCLIAppNum.title(), Intf{&manage::present, &manage::clear});
    subCLIAppNum.addDescription(descr<ApplyNumeric>());
    subCLIAppNum.addArgument(helpArg1, helpArg2).argsNum(0).implicitValue(true).help(helpDescr);

    candidates = extractChoices<ArithmeticMethod>();
    numTable.emplace(name<ArithmeticMethod>(), Attr{candidates, ArithmeticMethod{}});
    subCLIAppNum
        .addArgument(
            shortPrefix + std::string{alias<ArithmeticMethod>()}, longPrefix + std::string{name<ArithmeticMethod>()})
        .argsNum(0, candidates.size())
        .defaultValue<std::vector<std::string>>(std::move(candidates))
        .remaining()
        .metaVariable(metaVar)
        .help(descr<ArithmeticMethod>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<ArithmeticMethod>& msg)
                                      { updateChoice<ArithmeticMethod>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<ArithmeticMethod>& msg)
                                      { runChoices<ArithmeticMethod>(msg.coll); });
    versionLinks.emplace(VerLinkKey{name<ApplyNumeric>(), arithmetic::version()}, name<ArithmeticMethod>());
    candidates = extractChoices<DivisorMethod>();
    numTable.emplace(name<DivisorMethod>(), Attr{candidates, DivisorMethod{}});
    subCLIAppNum
        .addArgument(shortPrefix + std::string{alias<DivisorMethod>()}, longPrefix + std::string{name<DivisorMethod>()})
        .argsNum(0, candidates.size())
        .defaultValue<std::vector<std::string>>(std::move(candidates))
        .remaining()
        .metaVariable(metaVar)
        .help(descr<DivisorMethod>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<DivisorMethod>& msg)
                                      { updateChoice<DivisorMethod>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<DivisorMethod>& msg)
                                      { runChoices<DivisorMethod>(msg.coll); });
    versionLinks.emplace(VerLinkKey{name<ApplyNumeric>(), divisor::version()}, name<DivisorMethod>());
    candidates = extractChoices<IntegralMethod>();
    numTable.emplace(name<IntegralMethod>(), Attr{candidates, IntegralMethod{}});
    subCLIAppNum
        .addArgument(
            shortPrefix + std::string{alias<IntegralMethod>()}, longPrefix + std::string{name<IntegralMethod>()})
        .argsNum(0, candidates.size())
        .defaultValue<std::vector<std::string>>(std::move(candidates))
        .remaining()
        .metaVariable(metaVar)
        .help(descr<IntegralMethod>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<IntegralMethod>& msg)
                                      { updateChoice<IntegralMethod>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<IntegralMethod>& msg)
                                      { runChoices<IntegralMethod>(msg.coll); });
    versionLinks.emplace(VerLinkKey{name<ApplyNumeric>(), integral::version()}, name<IntegralMethod>());
    candidates = extractChoices<PrimeMethod>();
    numTable.emplace(name<PrimeMethod>(), Attr{candidates, PrimeMethod{}});
    subCLIAppNum
        .addArgument(shortPrefix + std::string{alias<PrimeMethod>()}, longPrefix + std::string{name<PrimeMethod>()})
        .argsNum(0, candidates.size())
        .defaultValue<std::vector<std::string>>(std::move(candidates))
        .remaining()
        .metaVariable(metaVar)
        .help(descr<PrimeMethod>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<PrimeMethod>& msg)
                                      { updateChoice<PrimeMethod>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<PrimeMethod>& msg)
                                      { runChoices<PrimeMethod>(msg.coll); });
    versionLinks.emplace(VerLinkKey{name<ApplyNumeric>(), prime::version()}, name<PrimeMethod>());

    mainCLI.addSubParser(subCLIAppNum);
}
// NOLINTEND(google-build-using-namespace, readability-function-size)

void Command::initializeNativeCLI()
{
    setupMainCLI();
}

void Command::initializeExtraCLI()
{
    setupSubCLI<reg_algo::ApplyAlgorithm>();
    setupSubCLI<reg_dp::ApplyDesignPattern>();
    setupSubCLI<reg_ds::ApplyDataStructure>();
    setupSubCLI<reg_num::ApplyNumeric>();
}

void Command::frontEndHandler(const int argc, const char* const argv[])
try
{
    std::unique_lock<std::mutex> parserLock(parserMtx);
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
        dispatch();
    }
}
catch (const std::exception& err)
{
    isFaulty.store(true);
    LOG_WRN << err.what();
}

void Command::precheck()
{
    for (auto& bits = taskDispatcher.nativeCategories;
         const auto index : std::views::iota(0U, bits.size())
             | std::views::filter([this](const auto i) { return mainCLI.isUsed(toString(static_cast<Category>(i))); }))
    {
        checkExcessArgs();
        bits.set(index);
    }

    for (constexpr auto helpArgInExtra = toString(Category::help);
         [[maybe_unused]] const auto& [subCLIName, categoryMap] :
         taskDispatcher.extraChoiceRegistry
             | std::views::filter(
                 [this](const auto& subCLIPair)
                 { return mainCLI.isSubCommandUsed(subCLIPair.first) ? (checkExcessArgs(), true) : false; }))
    {
        const auto& subCLI = mainCLI.at<utility::argument::Argument>(subCLIName);
        const bool notAssigned = !subCLI;
        taskDispatcher.extraHelpOnly = notAssigned || subCLI.isUsed(helpArgInExtra);
        if (notAssigned)
        {
            return;
        }

        for ([[maybe_unused]] const auto& [categoryName, categoryTrait] :
             categoryMap
                 | std::views::filter(
                     [this, &subCLI](const auto& categoryPair)
                     { return subCLI.isUsed(categoryPair.first) ? (checkExcessArgs(), true) : false; }))
        {
            for (const auto& target : subCLI.get<std::vector<std::string>>(categoryName))
            {
                std::visit(
                    action::EvtVisitor{
                        [this, &target](auto&& event)
                        { applyingForwarder.onMessage(action::UpdateChoice<std::decay_t<decltype(event)>>{target}); }},
                    categoryTrait.event);
            }
        }
    }
}

bool Command::anySelected() const
{
    return !taskDispatcher.empty();
}

void Command::dispatch()
{
    if (!taskDispatcher.NativeManager::empty())
    {
        for (const auto& bits = taskDispatcher.nativeCategories;
             const auto index :
             std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); }))
        {
            builtInNotifier.notify(static_cast<Category>(index));
        }
    }

    if (!taskDispatcher.ExtraManager::empty())
    {
        if (taskDispatcher.extraHelpOnly)
        {
            if (auto filtered = std::views::keys(taskDispatcher.extraChoiceRegistry)
                    | std::views::filter([this](const auto& subCLIName)
                                         { return mainCLI.isSubCommandUsed(subCLIName); });
                std::ranges::distance(filtered) != 0)
            {
                std::cout << mainCLI.at<utility::argument::Argument>(*std::ranges::begin(filtered)).help().str()
                          << std::flush;
            }
            return;
        }

        for ([[maybe_unused]] const auto& [categoryName, categoryTrait] : taskDispatcher.extraChoiceRegistry
                 | std::views::filter([this](const auto& subCLIPair)
                                      { return taskDispatcher.extraChecklist.at(subCLIPair.first).present(); })
                 | std::views::values | std::views::join)
        {
            std::visit(
                action::EvtVisitor{
                    [this, &candidates = categoryTrait.choices](auto&& event)
                    { applyingForwarder.onMessage(action::RunChoices<std::decay_t<decltype(event)>>{candidates}); }},
                categoryTrait.event);
        }
    }
}

template <typename T>
std::vector<std::string> Command::extractChoices()
{
    constexpr auto refl = REFLECTION_STR("choice");
    std::vector<std::string> choices{};
    choices.reserve(utility::reflection::TypeInfo<T>::fields.size);
    utility::reflection::TypeInfo<T>::fields.forEach(
        [refl, &choices](const auto field)
        {
            static_assert(field.attrs.contains(refl) && (field.attrs.size == 1));
            const auto attr = field.attrs.find(refl);
            static_assert(attr.hasValue);
            choices.emplace_back(attr.value);
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
            if (!client->stopRequested() && onParsing4Client(buffer, length))
            {
                client->requestStop();
            }
        }
        catch (const std::exception& err)
        {
            LOG_WRN << err.what();
        }
        notifyClientOutputDone();
    };
    client->toConnect(view::info::viewerTCPHost(), view::info::viewerTCPPort());
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
            if (!client->stopRequested() && onParsing4Client(buffer, length))
            {
                client->requestStop();
            }
        }
        catch (const std::exception& err)
        {
            LOG_WRN << err.what();
        }
        notifyClientOutputDone();
    };
    client->toReceive();
    client->toConnect(view::info::viewerUDPHost(), view::info::viewerUDPPort());
}

void Command::executeInConsole() const
{
    if (!configure::detail::activateHelper())
    {
        std::cout << "exit" << std::endl;
        return;
    }

    const auto& pendingInputs = mainCLI.get<std::vector<std::string>>(toString(Category::console));
    if (pendingInputs.empty())
    {
        return;
    }

    constexpr std::string_view greeting = "> ";
    const auto session = std::make_unique<console::Console>(greeting);
    auto udpClient = std::make_shared<utility::socket::UDPSocket>();
    launchClient(udpClient);
    registerOnConsole(*session, udpClient);

    for (const auto& opt : pendingInputs)
    {
        try
        {
            using RetCode = console::Console::RetCode;
            std::cout << greeting << opt << std::endl;
            if (session->optionExecutor(opt) == RetCode::quit)
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
    udpClient->toSend(buildDisconnectReq());
    udpClient->toJoin();
    interactionLatency();
}

void Command::showHelpMessage() const
{
    std::cout << mainCLI.help().str() << std::flush;
}

void Command::dumpConfiguration()
{
    std::cout << configure::getDefaultConfiguration() << std::endl;
}

void Command::displayVersionInfo() const
{
    validateDependenciesVersion();

    const auto basicInfo = std::format(
                   "\033[7m\033[49m{}"
#ifndef NDEBUG
                   "            DEBUG VERSION {} "
#else
                   "          RELEASE VERSION {} "
#endif // NDEBUG
                   "\033[0m\n",
                   note::banner(),
                   mainCLI.version()),
               additionalInfo = std::format(
                   "{}\nBuilt with {} for {} on {}.\n",
                   note::copyright(),
                   note::compiler(),
                   note::processor(),
                   note::date());
    std::cout << basicInfo << additionalInfo << std::flush;
}

void Command::checkExcessArgs()
{
    if (anySelected())
    {
        taskDispatcher.reset();
        throw std::runtime_error{"Excessive arguments."};
    }
}

//! @brief Perform the specific operation for Category::console.
template <>
template <>
void Command::LocalNotifier::Handler<Category::console>::execute() const
{
    subject.executeInConsole();
}

//! @brief Perform the specific operation for Category::dump.
template <>
template <>
void Command::LocalNotifier::Handler<Category::dump>::execute() const
{
    subject.dumpConfiguration();
}

//! @brief Perform the specific operation for Category::help.
template <>
template <>
void Command::LocalNotifier::Handler<Category::help>::execute() const
{
    subject.showHelpMessage();
}

//! @brief Perform the specific operation for Category::version.
template <>
template <>
void Command::LocalNotifier::Handler<Category::version>::execute() const
{
    subject.displayVersionInfo();
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
    char hostName[HOST_NAME_MAX] = {'\0'};
    if (::gethostname(hostName, HOST_NAME_MAX))
    {
        std::strncpy(hostName, "HOSTNAME", HOST_NAME_MAX - 1);
        hostName[HOST_NAME_MAX - 1] = '\0';
    }
    const auto greeting = userName + '@' + std::string{hostName} + " foo > ";
    const auto session = std::make_unique<console::Console>(greeting);
    auto tcpClient = std::make_shared<utility::socket::TCPSocket>();
    launchClient(tcpClient);
    registerOnConsole(*session, tcpClient);

    std::cout << note::banner() << std::endl;
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
    tcpClient->toSend(buildDisconnectReq());
    tcpClient->toJoin();
    interactionLatency();

    LOG_DBG << "Exit console mode.";
}
catch (const std::exception& err)
{
    LOG_ERR << err.what();
}

template <typename T>
void Command::registerOnConsole(console::Console& session, std::shared_ptr<T>& client)
{
    static constexpr auto gracefulReset = []<HelperType Helper>() constexpr
    {
        triggerHelper<Helper>(ExtEvent::reload);
        triggerHelper<Helper>(ExtEvent::startup);
    };
    const auto asyncReqSender = [&client](const console::Console::Args& inputs)
    {
        using RetCode = console::Console::RetCode;
        auto retCode = RetCode::success;
        try
        {
            auto reqBuffer = utility::common::base64Encode(std::accumulate(
                inputs.cbegin(),
                inputs.cend(),
                std::string{},
                [](const auto& acc, const auto& token) { return acc.empty() ? token : (acc + ' ' + token); }));
            client->toSend(std::move(reqBuffer));
            waitClientOutputDone();
        }
        catch (const std::exception& err)
        {
            retCode = RetCode::error;
            LOG_WRN << err.what();
            interactionLatency();
        }
        return retCode;
    };

    session.registerOption(
        "refresh",
        "refresh the outputs",
        [](const console::Console::Args& /*inputs*/)
        {
            using RetCode = console::Console::RetCode;
            auto retCode = RetCode::success;
            try
            {
                utility::common::invokeCallableWith<log::Log>(gracefulReset);
                LOG_INF_F("Refreshed the {} outputs.", log::Log::name);
            }
            catch (const std::exception& err)
            {
                retCode = RetCode::error;
                LOG_WRN << err.what();
            }
            interactionLatency();
            return retCode;
        });
    session.registerOption(
        "reconnect",
        "reconnect to the servers",
        [&client](const console::Console::Args& /*inputs*/)
        {
            using RetCode = console::Console::RetCode;
            auto retCode = RetCode::success;
            try
            {
                client->toSend(buildDisconnectReq());
                client->toJoin();
                interactionLatency();
                client.reset();

                utility::common::invokeCallableWith<view::View>(gracefulReset);
                client = std::make_shared<T>();
                launchClient(client);
                LOG_INF_F("Reconnected to the {} servers.", view::View::name);
            }
            catch (const std::exception& err)
            {
                retCode = RetCode::error;
                LOG_WRN << err.what();
            }
            interactionLatency();
            return retCode;
        });

    auto supportedOptions = view::info::viewerSupportedOptions();
    decltype(supportedOptions) validOptions{};
    for (auto iterator = supportedOptions.cbegin(); iterator != supportedOptions.cend();)
    {
        auto node = supportedOptions.extract(iterator++);
        auto& key = node.key();
        key.erase(
            std::remove_if(key.begin(), key.end(), [l = std::locale{}](const auto c) { return std::isspace(c, l); }),
            key.cend());
        validOptions.insert(std::move(node));
    }
    for (const auto& [name, description] : validOptions)
    {
        session.registerOption(name, description, asyncReqSender);
    }
}

bool Command::onParsing4Client(char* buffer, const int length)
{
    return (length != 0) ? view::View::Access().onParsing(buffer, length) : false;
}

void Command::waitClientOutputDone()
{
    view::View::Sync().waitTaskDone();
}

void Command::notifyClientOutputDone()
{
    view::View::Sync().notifyTaskDone();
}

std::string Command::buildDisconnectReq()
{
    return utility::common::base64Encode(view::exitSymbol);
}

void Command::interactionLatency()
{
    constexpr auto latency = std::chrono::milliseconds{10};
    std::this_thread::sleep_for(latency);
}

void Command::validateDependenciesVersion() const
{
    const auto& choiceRegistry = taskDispatcher.extraChoiceRegistry;
    const bool isNativeMatched = utility::common::areStringsEqual(
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
                   utility::time::version()),
               isExtraMatched = (versionLinks.count({subCLIAppAlgo.title(), subCLIAppAlgo.version()})
                                 == choiceRegistry.at(subCLIAppAlgo.title()).size())
        && (versionLinks.count({subCLIAppDp.title(), subCLIAppDp.version()})
            == choiceRegistry.at(subCLIAppDp.title()).size())
        && (versionLinks.count({subCLIAppDs.title(), subCLIAppDs.version()})
            == choiceRegistry.at(subCLIAppDs.title()).size())
        && (versionLinks.count({subCLIAppNum.title(), subCLIAppNum.version()})
            == choiceRegistry.at(subCLIAppNum.title()).size());
    if (!isNativeMatched || !isExtraMatched)
    {
        throw std::runtime_error{std::format(
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
            subCLIAppNum.version())};
    }
}
} // namespace application::command
