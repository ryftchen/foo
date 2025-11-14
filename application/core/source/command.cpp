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
//! @brief The semaphore that controls the maximum access limit.
std::counting_semaphore<1> cliSem(1);
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

//! @brief Helper daemon service.
//! @tparam Helpers - type of arguments of helper
template <ExtHelper... Helpers>
requires (std::derived_from<Helpers, utility::fsm::FSM<Helpers>> && ...)
static void daemonService()
{
    utility::thread::Thread extendedJob(sizeof...(Helpers));
    (extendedJob.enqueue(Helpers::name, &Helpers::service, Helpers::getInstance()), ...);
}

//! @brief Coroutine for managing the lifecycle of helper components.
//! @tparam Hs - type of helpers
//! @return awaitable instance
template <typename... Hs>
static action::Awaitable launchLifecycle()
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

//! @brief Enter the next phase of the coroutine of helpers.
//! @param awaitable - awaitable instance
static void enterNextPhase(action::Awaitable& awaitable)
{
    if (!awaitable.done())
    {
        awaitable.resume();
    }
}
} // namespace help

//! @brief Convert category enumeration to string.
//! @param cat - native category
//! @return category name
static constexpr std::string_view toString(const Category cat)
{
    constexpr std::array<std::string_view, Bottom<Category>::value> stringify = {
        MACRO_STRINGIFY(console), MACRO_STRINGIFY(dump), MACRO_STRINGIFY(help), MACRO_STRINGIFY(version)};
    return stringify.at(static_cast<std::uint8_t>(cat));
}

//! @brief Get the Command instance.
//! @return reference of the Command object
Command& getInstance()
{
    static Command commander{};
    return commander;
}

template <typename Key, typename Subject>
void Notifier<Key, Subject>::attach(const Key key, std::shared_ptr<ProcBase> handler)
{
    handlers[key] = std::move(handler);
}

template <typename Key, typename Subject>
void Notifier<Key, Subject>::notify(const Key key) const
{
    if (handlers.contains(key))
    {
        handlers.at(key)->execute();
    }
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
consteval std::string_view Command::mappedAlias(const Category cat)
{
//! @cond
#define X(enum, descr, alias) {descr, alias},
    constexpr std::string_view table[][2] = {COMMAND_CATEGORY_X_MACRO_MAPPING};
    static_assert((sizeof(table) / sizeof(table[0])) == Bottom<Category>::value);
    return table[static_cast<std::uint8_t>(cat)][0];
//! @endcond
#undef X
}

consteval std::string_view Command::mappedDescr(const Category cat)
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
    clearSelected();
}

bool Command::execute(const int argc, const char* const argv[])
try
{
    isFaulty.store(false);
    isParsed.store(false);
    auto helpCtrl = help::launchLifecycle<log::Log, view::View>();
    help::enterNextPhase(helpCtrl);

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

    help::enterNextPhase(helpCtrl);
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
                if (std::all_of(
                        input.cbegin(), input.cend(), [l = std::locale{}](const auto c) { return std::isspace(c, l); }))
                {
                    throw std::runtime_error{"Invalid " + std::string{toString(Category::console)} + " command."};
                }
                return input;
            })
        .metaVariable("CMD")
        .help(mappedDescr(Category::console));
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
    constexpr std::string_view helpDescr = mappedDescr(Category::help);
    const std::string helpArg1 = shortPrefix + std::string{mappedAlias(Category::help)};
    const std::string helpArg2 = longPrefix + std::string{toString(Category::help)};
    auto& registry = taskDispatcher.extraChoiceRegistry[subCLIAppAlgo.title()];
    std::vector<std::string> candidates{};

    taskDispatcher.extraChecklist.emplace(subCLIAppAlgo.title(), Intf{manage::present, manage::clear});
    subCLIAppAlgo.addDescription(descr<ApplyAlgorithm>());
    subCLIAppAlgo.addArgument(helpArg1, helpArg2).argsNum(0).implicitValue(true).help(helpDescr);

    candidates = extractChoices<MatchMethod>();
    registry.emplace(name<MatchMethod>(), Attr{candidates, MatchMethod{}});
    subCLIAppAlgo
        .addArgument(shortPrefix + std::string{alias<MatchMethod>()}, longPrefix + std::string{name<MatchMethod>()})
        .argsNum(0, candidates.size())
        .defaultValue<decltype(candidates)>(std::move(candidates))
        .remaining()
        .metaVariable(metaVar)
        .help(descr<MatchMethod>());
    applyingForwarder.registerHandler([](const action::SetChoice<MatchMethod>& msg)
                                      { setChoice<MatchMethod>(msg.choice); });
    applyingForwarder.registerHandler([](const action::RunCandidates<MatchMethod>& msg)
                                      { runCandidates<MatchMethod>(msg.candidates); });
    versionLinks.emplace(VerLinkKey{name<ApplyAlgorithm>(), match::version()}, name<MatchMethod>());
    candidates = extractChoices<NotationMethod>();
    registry.emplace(name<NotationMethod>(), Attr{candidates, NotationMethod{}});
    subCLIAppAlgo
        .addArgument(
            shortPrefix + std::string{alias<NotationMethod>()}, longPrefix + std::string{name<NotationMethod>()})
        .argsNum(0, candidates.size())
        .defaultValue<decltype(candidates)>(std::move(candidates))
        .remaining()
        .metaVariable(metaVar)
        .help(descr<NotationMethod>());
    applyingForwarder.registerHandler([](const action::SetChoice<NotationMethod>& msg)
                                      { setChoice<NotationMethod>(msg.choice); });
    applyingForwarder.registerHandler([](const action::RunCandidates<NotationMethod>& msg)
                                      { runCandidates<NotationMethod>(msg.candidates); });
    versionLinks.emplace(VerLinkKey{name<ApplyAlgorithm>(), notation::version()}, name<NotationMethod>());
    candidates = extractChoices<OptimalMethod>();
    registry.emplace(name<OptimalMethod>(), Attr{candidates, OptimalMethod{}});
    subCLIAppAlgo
        .addArgument(shortPrefix + std::string{alias<OptimalMethod>()}, longPrefix + std::string{name<OptimalMethod>()})
        .argsNum(0, candidates.size())
        .defaultValue<decltype(candidates)>(std::move(candidates))
        .remaining()
        .metaVariable(metaVar)
        .help(descr<OptimalMethod>());
    applyingForwarder.registerHandler([](const action::SetChoice<OptimalMethod>& msg)
                                      { setChoice<OptimalMethod>(msg.choice); });
    applyingForwarder.registerHandler([](const action::RunCandidates<OptimalMethod>& msg)
                                      { runCandidates<OptimalMethod>(msg.candidates); });
    versionLinks.emplace(VerLinkKey{name<ApplyAlgorithm>(), optimal::version()}, name<OptimalMethod>());
    candidates = extractChoices<SearchMethod>();
    registry.emplace(name<SearchMethod>(), Attr{candidates, SearchMethod{}});
    subCLIAppAlgo
        .addArgument(shortPrefix + std::string{alias<SearchMethod>()}, longPrefix + std::string{name<SearchMethod>()})
        .argsNum(0, candidates.size())
        .defaultValue<decltype(candidates)>(std::move(candidates))
        .remaining()
        .metaVariable(metaVar)
        .help(descr<SearchMethod>());
    applyingForwarder.registerHandler([](const action::SetChoice<SearchMethod>& msg)
                                      { setChoice<SearchMethod>(msg.choice); });
    applyingForwarder.registerHandler([](const action::RunCandidates<SearchMethod>& msg)
                                      { runCandidates<SearchMethod>(msg.candidates); });
    versionLinks.emplace(VerLinkKey{name<ApplyAlgorithm>(), search::version()}, name<SearchMethod>());
    candidates = extractChoices<SortMethod>();
    registry.emplace(name<SortMethod>(), Attr{candidates, SortMethod{}});
    subCLIAppAlgo
        .addArgument(shortPrefix + std::string{alias<SortMethod>()}, longPrefix + std::string{name<SortMethod>()})
        .argsNum(0, candidates.size())
        .defaultValue<decltype(candidates)>(std::move(candidates))
        .remaining()
        .metaVariable(metaVar)
        .help(descr<SortMethod>());
    applyingForwarder.registerHandler([](const action::SetChoice<SortMethod>& msg)
                                      { setChoice<SortMethod>(msg.choice); });
    applyingForwarder.registerHandler([](const action::RunCandidates<SortMethod>& msg)
                                      { runCandidates<SortMethod>(msg.candidates); });
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
    constexpr std::string_view helpDescr = mappedDescr(Category::help);
    const std::string helpArg1 = shortPrefix + std::string{mappedAlias(Category::help)};
    const std::string helpArg2 = longPrefix + std::string{toString(Category::help)};
    auto& registry = taskDispatcher.extraChoiceRegistry[subCLIAppDp.title()];
    std::vector<std::string> candidates{};

    taskDispatcher.extraChecklist.emplace(subCLIAppDp.title(), Intf{manage::present, manage::clear});
    subCLIAppDp.addDescription(descr<ApplyDesignPattern>());
    subCLIAppDp.addArgument(helpArg1, helpArg2).argsNum(0).implicitValue(true).help(helpDescr);

    candidates = extractChoices<BehavioralInstance>();
    registry.emplace(name<BehavioralInstance>(), Attr{candidates, BehavioralInstance{}});
    subCLIAppDp
        .addArgument(
            shortPrefix + std::string{alias<BehavioralInstance>()},
            longPrefix + std::string{name<BehavioralInstance>()})
        .argsNum(0, candidates.size())
        .defaultValue<decltype(candidates)>(std::move(candidates))
        .remaining()
        .metaVariable(metaVar)
        .help(descr<BehavioralInstance>());
    applyingForwarder.registerHandler([](const action::SetChoice<BehavioralInstance>& msg)
                                      { setChoice<BehavioralInstance>(msg.choice); });
    applyingForwarder.registerHandler([](const action::RunCandidates<BehavioralInstance>& msg)
                                      { runCandidates<BehavioralInstance>(msg.candidates); });
    versionLinks.emplace(VerLinkKey{name<ApplyDesignPattern>(), behavioral::version()}, name<BehavioralInstance>());
    candidates = extractChoices<CreationalInstance>();
    registry.emplace(name<CreationalInstance>(), Attr{candidates, CreationalInstance{}});
    subCLIAppDp
        .addArgument(
            shortPrefix + std::string{alias<CreationalInstance>()},
            longPrefix + std::string{name<CreationalInstance>()})
        .argsNum(0, candidates.size())
        .defaultValue<decltype(candidates)>(std::move(candidates))
        .remaining()
        .metaVariable(metaVar)
        .help(descr<CreationalInstance>());
    applyingForwarder.registerHandler([](const action::SetChoice<CreationalInstance>& msg)
                                      { setChoice<CreationalInstance>(msg.choice); });
    applyingForwarder.registerHandler([](const action::RunCandidates<CreationalInstance>& msg)
                                      { runCandidates<CreationalInstance>(msg.candidates); });
    versionLinks.emplace(VerLinkKey{name<ApplyDesignPattern>(), creational::version()}, name<CreationalInstance>());
    candidates = extractChoices<StructuralInstance>();
    registry.emplace(name<StructuralInstance>(), Attr{candidates, StructuralInstance{}});
    subCLIAppDp
        .addArgument(
            shortPrefix + std::string{alias<StructuralInstance>()},
            longPrefix + std::string{name<StructuralInstance>()})
        .argsNum(0, candidates.size())
        .defaultValue<decltype(candidates)>(std::move(candidates))
        .remaining()
        .metaVariable(metaVar)
        .help(descr<StructuralInstance>());
    applyingForwarder.registerHandler([](const action::SetChoice<StructuralInstance>& msg)
                                      { setChoice<StructuralInstance>(msg.choice); });
    applyingForwarder.registerHandler([](const action::RunCandidates<StructuralInstance>& msg)
                                      { runCandidates<StructuralInstance>(msg.candidates); });
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
    constexpr std::string_view helpDescr = mappedDescr(Category::help);
    const std::string helpArg1 = shortPrefix + std::string{mappedAlias(Category::help)};
    const std::string helpArg2 = longPrefix + std::string{toString(Category::help)};
    auto& registry = taskDispatcher.extraChoiceRegistry[subCLIAppDs.title()];
    std::vector<std::string> candidates{};

    taskDispatcher.extraChecklist.emplace(subCLIAppDs.title(), Intf{manage::present, manage::clear});
    subCLIAppDs.addDescription(descr<ApplyDataStructure>());
    subCLIAppDs.addArgument(helpArg1, helpArg2).argsNum(0).implicitValue(true).help(helpDescr);

    candidates = extractChoices<CacheInstance>();
    registry.emplace(name<CacheInstance>(), Attr{candidates, CacheInstance{}});
    subCLIAppDs
        .addArgument(shortPrefix + std::string{alias<CacheInstance>()}, longPrefix + std::string{name<CacheInstance>()})
        .argsNum(0, candidates.size())
        .defaultValue<decltype(candidates)>(std::move(candidates))
        .remaining()
        .metaVariable(metaVar)
        .help(descr<CacheInstance>());
    applyingForwarder.registerHandler([](const action::SetChoice<CacheInstance>& msg)
                                      { setChoice<CacheInstance>(msg.choice); });
    applyingForwarder.registerHandler([](const action::RunCandidates<CacheInstance>& msg)
                                      { runCandidates<CacheInstance>(msg.candidates); });
    versionLinks.emplace(VerLinkKey{name<ApplyDataStructure>(), cache::version()}, name<CacheInstance>());
    candidates = extractChoices<FilterInstance>();
    registry.emplace(name<FilterInstance>(), Attr{candidates, FilterInstance{}});
    subCLIAppDs
        .addArgument(
            shortPrefix + std::string{alias<FilterInstance>()}, longPrefix + std::string{name<FilterInstance>()})
        .argsNum(0, candidates.size())
        .defaultValue<decltype(candidates)>(std::move(candidates))
        .remaining()
        .metaVariable(metaVar)
        .help(descr<FilterInstance>());
    applyingForwarder.registerHandler([](const action::SetChoice<FilterInstance>& msg)
                                      { setChoice<FilterInstance>(msg.choice); });
    applyingForwarder.registerHandler([](const action::RunCandidates<FilterInstance>& msg)
                                      { runCandidates<FilterInstance>(msg.candidates); });
    versionLinks.emplace(VerLinkKey{name<ApplyDataStructure>(), filter::version()}, name<FilterInstance>());
    candidates = extractChoices<GraphInstance>();
    registry.emplace(name<GraphInstance>(), Attr{candidates, GraphInstance{}});
    subCLIAppDs
        .addArgument(shortPrefix + std::string{alias<GraphInstance>()}, longPrefix + std::string{name<GraphInstance>()})
        .argsNum(0, candidates.size())
        .defaultValue<decltype(candidates)>(std::move(candidates))
        .remaining()
        .metaVariable(metaVar)
        .help(descr<GraphInstance>());
    applyingForwarder.registerHandler([](const action::SetChoice<GraphInstance>& msg)
                                      { setChoice<GraphInstance>(msg.choice); });
    applyingForwarder.registerHandler([](const action::RunCandidates<GraphInstance>& msg)
                                      { runCandidates<GraphInstance>(msg.candidates); });
    versionLinks.emplace(VerLinkKey{name<ApplyDataStructure>(), graph::version()}, name<GraphInstance>());
    candidates = extractChoices<HeapInstance>();
    registry.emplace(name<HeapInstance>(), Attr{candidates, HeapInstance{}});
    subCLIAppDs
        .addArgument(shortPrefix + std::string{alias<HeapInstance>()}, longPrefix + std::string{name<HeapInstance>()})
        .argsNum(0, candidates.size())
        .defaultValue<decltype(candidates)>(std::move(candidates))
        .remaining()
        .metaVariable(metaVar)
        .help(descr<HeapInstance>());
    applyingForwarder.registerHandler([](const action::SetChoice<HeapInstance>& msg)
                                      { setChoice<HeapInstance>(msg.choice); });
    applyingForwarder.registerHandler([](const action::RunCandidates<HeapInstance>& msg)
                                      { runCandidates<HeapInstance>(msg.candidates); });
    versionLinks.emplace(VerLinkKey{name<ApplyDataStructure>(), heap::version()}, name<HeapInstance>());
    candidates = extractChoices<LinearInstance>();
    registry.emplace(name<LinearInstance>(), Attr{candidates, LinearInstance{}});
    subCLIAppDs
        .addArgument(
            shortPrefix + std::string{alias<LinearInstance>()}, longPrefix + std::string{name<LinearInstance>()})
        .argsNum(0, candidates.size())
        .defaultValue<decltype(candidates)>(std::move(candidates))
        .remaining()
        .metaVariable(metaVar)
        .help(descr<LinearInstance>());
    applyingForwarder.registerHandler([](const action::SetChoice<LinearInstance>& msg)
                                      { setChoice<LinearInstance>(msg.choice); });
    applyingForwarder.registerHandler([](const action::RunCandidates<LinearInstance>& msg)
                                      { runCandidates<LinearInstance>(msg.candidates); });
    versionLinks.emplace(VerLinkKey{name<ApplyDataStructure>(), linear::version()}, name<LinearInstance>());
    candidates = extractChoices<TreeInstance>();
    registry.emplace(name<TreeInstance>(), Attr{candidates, TreeInstance{}});
    subCLIAppDs
        .addArgument(shortPrefix + std::string{alias<TreeInstance>()}, longPrefix + std::string{name<TreeInstance>()})
        .argsNum(0, candidates.size())
        .defaultValue<decltype(candidates)>(std::move(candidates))
        .remaining()
        .metaVariable(metaVar)
        .help(descr<TreeInstance>());
    applyingForwarder.registerHandler([](const action::SetChoice<TreeInstance>& msg)
                                      { setChoice<TreeInstance>(msg.choice); });
    applyingForwarder.registerHandler([](const action::RunCandidates<TreeInstance>& msg)
                                      { runCandidates<TreeInstance>(msg.candidates); });
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
    constexpr std::string_view helpDescr = mappedDescr(Category::help);
    const std::string helpArg1 = shortPrefix + std::string{mappedAlias(Category::help)};
    const std::string helpArg2 = longPrefix + std::string{toString(Category::help)};
    auto& registry = taskDispatcher.extraChoiceRegistry[subCLIAppNum.title()];
    std::vector<std::string> candidates{};

    taskDispatcher.extraChecklist.emplace(subCLIAppNum.title(), Intf{manage::present, manage::clear});
    subCLIAppNum.addDescription(descr<ApplyNumeric>());
    subCLIAppNum.addArgument(helpArg1, helpArg2).argsNum(0).implicitValue(true).help(helpDescr);

    candidates = extractChoices<ArithmeticMethod>();
    registry.emplace(name<ArithmeticMethod>(), Attr{candidates, ArithmeticMethod{}});
    subCLIAppNum
        .addArgument(
            shortPrefix + std::string{alias<ArithmeticMethod>()}, longPrefix + std::string{name<ArithmeticMethod>()})
        .argsNum(0, candidates.size())
        .defaultValue<decltype(candidates)>(std::move(candidates))
        .remaining()
        .metaVariable(metaVar)
        .help(descr<ArithmeticMethod>());
    applyingForwarder.registerHandler([](const action::SetChoice<ArithmeticMethod>& msg)
                                      { setChoice<ArithmeticMethod>(msg.choice); });
    applyingForwarder.registerHandler([](const action::RunCandidates<ArithmeticMethod>& msg)
                                      { runCandidates<ArithmeticMethod>(msg.candidates); });
    versionLinks.emplace(VerLinkKey{name<ApplyNumeric>(), arithmetic::version()}, name<ArithmeticMethod>());
    candidates = extractChoices<DivisorMethod>();
    registry.emplace(name<DivisorMethod>(), Attr{candidates, DivisorMethod{}});
    subCLIAppNum
        .addArgument(shortPrefix + std::string{alias<DivisorMethod>()}, longPrefix + std::string{name<DivisorMethod>()})
        .argsNum(0, candidates.size())
        .defaultValue<decltype(candidates)>(std::move(candidates))
        .remaining()
        .metaVariable(metaVar)
        .help(descr<DivisorMethod>());
    applyingForwarder.registerHandler([](const action::SetChoice<DivisorMethod>& msg)
                                      { setChoice<DivisorMethod>(msg.choice); });
    applyingForwarder.registerHandler([](const action::RunCandidates<DivisorMethod>& msg)
                                      { runCandidates<DivisorMethod>(msg.candidates); });
    versionLinks.emplace(VerLinkKey{name<ApplyNumeric>(), divisor::version()}, name<DivisorMethod>());
    candidates = extractChoices<IntegralMethod>();
    registry.emplace(name<IntegralMethod>(), Attr{candidates, IntegralMethod{}});
    subCLIAppNum
        .addArgument(
            shortPrefix + std::string{alias<IntegralMethod>()}, longPrefix + std::string{name<IntegralMethod>()})
        .argsNum(0, candidates.size())
        .defaultValue<decltype(candidates)>(std::move(candidates))
        .remaining()
        .metaVariable(metaVar)
        .help(descr<IntegralMethod>());
    applyingForwarder.registerHandler([](const action::SetChoice<IntegralMethod>& msg)
                                      { setChoice<IntegralMethod>(msg.choice); });
    applyingForwarder.registerHandler([](const action::RunCandidates<IntegralMethod>& msg)
                                      { runCandidates<IntegralMethod>(msg.candidates); });
    versionLinks.emplace(VerLinkKey{name<ApplyNumeric>(), integral::version()}, name<IntegralMethod>());
    candidates = extractChoices<PrimeMethod>();
    registry.emplace(name<PrimeMethod>(), Attr{candidates, PrimeMethod{}});
    subCLIAppNum
        .addArgument(shortPrefix + std::string{alias<PrimeMethod>()}, longPrefix + std::string{name<PrimeMethod>()})
        .argsNum(0, candidates.size())
        .defaultValue<decltype(candidates)>(std::move(candidates))
        .remaining()
        .metaVariable(metaVar)
        .help(descr<PrimeMethod>());
    applyingForwarder.registerHandler([](const action::SetChoice<PrimeMethod>& msg)
                                      { setChoice<PrimeMethod>(msg.choice); });
    applyingForwarder.registerHandler([](const action::RunCandidates<PrimeMethod>& msg)
                                      { runCandidates<PrimeMethod>(msg.candidates); });
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
    for (auto& spec = taskDispatcher.nativeCategories;
         const auto index : std::views::iota(0U, spec.size())
             | std::views::filter([this](const auto i) { return mainCLI.isUsed(toString(static_cast<Category>(i))); }))
    {
        checkExcessArgs();
        spec.set(index);
    }

    for (constexpr auto helpArgName = toString(Category::help);
         [[maybe_unused]] const auto& [subCLIName, categoryMap] : taskDispatcher.extraChoiceRegistry
             | std::views::filter([this](const auto& subCLIPair)
                                  { return mainCLI.isSubCommandUsed(subCLIPair.first) && (checkExcessArgs(), true); }))
    {
        const auto& subCLI = mainCLI.at<utility::argument::Argument>(subCLIName);
        const bool notAssigned = !subCLI;
        taskDispatcher.extraHelping = notAssigned || subCLI.isUsed(helpArgName);
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
                std::visit(
                    utility::common::VisitorOverload{
                        [this, &choice](auto&& event)
                        { applyingForwarder.onMessage(action::SetChoice<std::decay_t<decltype(event)>>{choice}); }},
                    categoryAttr.event);
            }
        }
    }
}

bool Command::anySelected() const
{
    return !taskDispatcher.empty();
}

void Command::clearSelected()
{
    taskDispatcher.reset();
}

void Command::dispatchAll()
{
    if (!taskDispatcher.NativeManager::empty())
    {
        for (const auto& spec = taskDispatcher.nativeCategories;
             const auto index :
             std::views::iota(0U, spec.size()) | std::views::filter([&spec](const auto i) { return spec.test(i); }))
        {
            builtInNotifier.notify(static_cast<Category>(index));
        }
    }

    if (!taskDispatcher.ExtraManager::empty())
    {
        if (taskDispatcher.extraHelping)
        {
            if (auto whichUsed = std::views::keys(taskDispatcher.extraChoiceRegistry)
                    | std::views::filter([this](const auto& subCLIName)
                                         { return mainCLI.isSubCommandUsed(subCLIName); });
                std::ranges::distance(whichUsed) != 0)
            {
                std::cout << mainCLI.at<utility::argument::Argument>(*std::ranges::begin(whichUsed)).help().str()
                          << std::flush;
            }
            return;
        }

        for ([[maybe_unused]] const auto& [categoryName, categoryAttr] : taskDispatcher.extraChoiceRegistry
                 | std::views::filter([this](const auto& subCLIPair)
                                      { return taskDispatcher.extraChecklist.at(subCLIPair.first).present(); })
                 | std::views::values | std::views::join)
        {
            std::visit(
                utility::common::VisitorOverload{
                    [this, &candidates = categoryAttr.choices](auto&& event)
                    { applyingForwarder.onMessage(action::RunCandidates<std::decay_t<decltype(event)>>{candidates}); }},
                categoryAttr.event);
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

template <typename Cat>
std::vector<std::string> Command::extractChoices()
{
    constexpr auto refl = REFLECTION_STR("choice");
    std::vector<std::string> choices{};
    choices.reserve(utility::reflection::TypeInfo<Cat>::fields.size);
    utility::reflection::TypeInfo<Cat>::fields.forEach(
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
    client->subscribeRawMessage(
        [&client](char* const bytes, const std::size_t size)
        {
            try
            {
                MACRO_DEFER([]() { notifyClientOutputDone(); });
                if (!client->stopRequested() && !onParsing4Client(bytes, size))
                {
                    client->requestStop();
                }
            }
            catch (const std::exception& err)
            {
                LOG_WRN << err.what();
            }
        });
    client->toConnect(view::info::currentTCPHost(), view::info::currentTCPPort());
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
                if (!client->stopRequested() && !onParsing4Client(bytes, size))
                {
                    client->requestStop();
                }
            }
            catch (const std::exception& err)
            {
                LOG_WRN << err.what();
            }
        });
    client->toReceive();
    client->toConnect(view::info::currentUDPHost(), view::info::currentUDPPort());
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

    for (const auto& input : pendingInputs)
    {
        try
        {
            using RetCode = console::Console::RetCode;
            std::cout << greeting << input << std::endl;
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
    std::cout << configure::dumpDefaultConfig() << std::endl;
}

void Command::displayVersionInfo() const
{
    validateDependencies();

    auto briefReview = std::format(
        "\033[7m\033[49m{}"
#ifndef NDEBUG
        "            DEBUG VERSION {} "
#else
        "          RELEASE VERSION {} "
#endif // NDEBUG
        "\033[0m\n",
        note::banner(),
        mainCLI.version());
    std::format_to(
        std::back_inserter(briefReview),
        "{}\nBuilt with {} for {} on {}.",
        note::copyright(),
        note::compiler(),
        note::processor(),
        note::date());
    std::cout << briefReview << std::endl;
}

//! @brief Perform the specific operation for Category::console.
template <>
template <>
void Command::LocalNotifier::Handler<Category::console>::execute() const
{
    inst.executeInConsole();
}

//! @brief Perform the specific operation for Category::dump.
template <>
template <>
void Command::LocalNotifier::Handler<Category::dump>::execute() const
{
    inst.dumpConfiguration();
}

//! @brief Perform the specific operation for Category::help.
template <>
template <>
void Command::LocalNotifier::Handler<Category::help>::execute() const
{
    inst.showHelpMessage();
}

//! @brief Perform the specific operation for Category::version.
template <>
template <>
void Command::LocalNotifier::Handler<Category::version>::execute() const
{
    inst.displayVersionInfo();
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

    const auto& choiceRegistry = taskDispatcher.extraChoiceRegistry;
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
                client->toSend(std::move(reqBuffer));
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
                    client->toSend(buildDisconnectReq());
                    client->toJoin();
                    interactionLatency();
                    client.reset();

                    using view::View;
                    gracefulReset.template operator()<View>();
                    client = std::make_shared<Sock>();
                    launchClient(client);
                    LOG_INF_F("Reconnected to the {} servers.", View::name);
                });
        });

    auto supportedOptions = view::info::currentSupportedOptions();
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

bool Command::onParsing4Client(char* const buffer, const std::size_t length)
{
    return (length == 0) || view::View::Access().onParsing(buffer, length);
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
} // namespace application::command
