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
    && !std::is_move_constructible_v<T> && !std::is_move_assignable_v<T> && requires (T /*helper*/) {
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
        [[unlikely]] default:
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
    return stringify.at(cat);
}

// clang-format off
//! @brief Mapping table for enum and attribute about command categories. X macro.
#define COMMAND_CATEGORY_X_MACRO_MAPPING                                                    \
    X(Category::console, "run options in console mode and exit\nseparate with quotes", "c") \
    X(Category::dump   , "dump default configuration and exit"                       , "d") \
    X(Category::help   , "show this help message and exit"                           , "h") \
    X(Category::version, "show version information and exit"                         , "v")
// clang-format on
consteval std::string_view Command::getDescr(const Category cat)
{
//! @cond
#define X(enum, descr, alias) {descr, alias},
    constexpr std::string_view table[][2] = {COMMAND_CATEGORY_X_MACRO_MAPPING};
    static_assert((sizeof(table) / sizeof(table[0])) == Bottom<Category>::value);
    return table[cat][0];
//! @endcond
#undef X
}

consteval std::string_view Command::getAlias(const Category cat)
{
//! @cond
#define X(enum, descr, alias) {descr, alias},
    constexpr std::string_view table[][2] = {COMMAND_CATEGORY_X_MACRO_MAPPING};
    static_assert((sizeof(table) / sizeof(table[0])) == Bottom<Category>::value);
    return table[cat][1];
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
        .metavar("CMD")
        .help(getDescr(Category::console));
    builtInNotifier.attach(Category::console, std::make_shared<LocalNotifier::Handler<Category::console>>(*this));
}

//! @brief Setup the sub-command line interface (algorithm module).
template <>
void Command::setupSubCLI<reg_algo::ApplyAlgorithm>() // NOLINT(readability-function-size)
{
    using Intf = ExtraManager::Intf;
    using Attr = ExtraManager::Attr;
    using action::name, action::descr, action::alias;
    constexpr std::string_view helpDescr = getDescr(Category::help), optMetavar = "OPT";
    const std::string helpArg1 = shortPrefix + std::string{getAlias(Category::help)},
                      helpArg2 = longPrefix + std::string{toString(Category::help)};
    std::vector<std::string> candidates{};

    auto& algoTable = taskDispatcher.extraChoiceRegistry[subCLIAppAlgo.title()];
    taskDispatcher.extraChecklist.emplace(
        subCLIAppAlgo.title(),
        Intf{[]() { return !reg_algo::manager().empty(); }, []() { reg_algo::manager().reset(); }});
    subCLIAppAlgo.addDescription(descr<reg_algo::ApplyAlgorithm>());
    subCLIAppAlgo.addArgument(helpArg1, helpArg2).argsNum(0).implicitValue(true).help(helpDescr);
    candidates = extractChoices<reg_algo::MatchMethod>();
    algoTable.emplace(name<reg_algo::MatchMethod>(), Attr{candidates, reg_algo::MatchMethod{}});
    subCLIAppAlgo
        .addArgument(
            shortPrefix + std::string{alias<reg_algo::MatchMethod>()},
            longPrefix + std::string{name<reg_algo::MatchMethod>()})
        .argsNum(0, candidates.size())
        .defaultValue<std::vector<std::string>>(std::move(candidates))
        .remaining()
        .metavar(optMetavar)
        .help(descr<reg_algo::MatchMethod>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<reg_algo::MatchMethod>& msg)
                                      { reg_algo::updateChoice<reg_algo::MatchMethod>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<reg_algo::MatchMethod>& msg)
                                      { reg_algo::runChoices<reg_algo::MatchMethod>(msg.coll); });
    versionLinks.emplace(
        VerLinkKey{name<reg_algo::ApplyAlgorithm>(), reg_algo::match::version()}, name<reg_algo::MatchMethod>());
    candidates = extractChoices<reg_algo::NotationMethod>();
    algoTable.emplace(name<reg_algo::NotationMethod>(), Attr{candidates, reg_algo::NotationMethod{}});
    subCLIAppAlgo
        .addArgument(
            shortPrefix + std::string{alias<reg_algo::NotationMethod>()},
            longPrefix + std::string{name<reg_algo::NotationMethod>()})
        .argsNum(0, candidates.size())
        .defaultValue<std::vector<std::string>>(std::move(candidates))
        .remaining()
        .metavar(optMetavar)
        .help(descr<reg_algo::NotationMethod>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<reg_algo::NotationMethod>& msg)
                                      { reg_algo::updateChoice<reg_algo::NotationMethod>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<reg_algo::NotationMethod>& msg)
                                      { reg_algo::runChoices<reg_algo::NotationMethod>(msg.coll); });
    versionLinks.emplace(
        VerLinkKey{name<reg_algo::ApplyAlgorithm>(), reg_algo::notation::version()}, name<reg_algo::NotationMethod>());
    candidates = extractChoices<reg_algo::OptimalMethod>();
    algoTable.emplace(name<reg_algo::OptimalMethod>(), Attr{candidates, reg_algo::OptimalMethod{}});
    subCLIAppAlgo
        .addArgument(
            shortPrefix + std::string{alias<reg_algo::OptimalMethod>()},
            longPrefix + std::string{name<reg_algo::OptimalMethod>()})
        .argsNum(0, candidates.size())
        .defaultValue<std::vector<std::string>>(std::move(candidates))
        .remaining()
        .metavar(optMetavar)
        .help(descr<reg_algo::OptimalMethod>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<reg_algo::OptimalMethod>& msg)
                                      { reg_algo::updateChoice<reg_algo::OptimalMethod>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<reg_algo::OptimalMethod>& msg)
                                      { reg_algo::runChoices<reg_algo::OptimalMethod>(msg.coll); });
    versionLinks.emplace(
        VerLinkKey{name<reg_algo::ApplyAlgorithm>(), reg_algo::optimal::version()}, name<reg_algo::OptimalMethod>());
    candidates = extractChoices<reg_algo::SearchMethod>();
    algoTable.emplace(name<reg_algo::SearchMethod>(), Attr{candidates, reg_algo::SearchMethod{}});
    subCLIAppAlgo
        .addArgument(
            shortPrefix + std::string{alias<reg_algo::SearchMethod>()},
            longPrefix + std::string{name<reg_algo::SearchMethod>()})
        .argsNum(0, candidates.size())
        .defaultValue<std::vector<std::string>>(std::move(candidates))
        .remaining()
        .metavar(optMetavar)
        .help(descr<reg_algo::SearchMethod>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<reg_algo::SearchMethod>& msg)
                                      { reg_algo::updateChoice<reg_algo::SearchMethod>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<reg_algo::SearchMethod>& msg)
                                      { reg_algo::runChoices<reg_algo::SearchMethod>(msg.coll); });
    versionLinks.emplace(
        VerLinkKey{name<reg_algo::ApplyAlgorithm>(), reg_algo::search::version()}, name<reg_algo::SearchMethod>());
    candidates = extractChoices<reg_algo::SortMethod>();
    algoTable.emplace(name<reg_algo::SortMethod>(), Attr{candidates, reg_algo::SortMethod{}});
    subCLIAppAlgo
        .addArgument(
            shortPrefix + std::string{alias<reg_algo::SortMethod>()},
            longPrefix + std::string{name<reg_algo::SortMethod>()})
        .argsNum(0, candidates.size())
        .defaultValue<std::vector<std::string>>(std::move(candidates))
        .remaining()
        .metavar(optMetavar)
        .help(descr<reg_algo::SortMethod>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<reg_algo::SortMethod>& msg)
                                      { reg_algo::updateChoice<reg_algo::SortMethod>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<reg_algo::SortMethod>& msg)
                                      { reg_algo::runChoices<reg_algo::SortMethod>(msg.coll); });
    versionLinks.emplace(
        VerLinkKey{name<reg_algo::ApplyAlgorithm>(), reg_algo::sort::version()}, name<reg_algo::SortMethod>());
    mainCLI.addSubParser(subCLIAppAlgo);
}

//! @brief Setup the sub-command line interface (design pattern module).
template <>
void Command::setupSubCLI<reg_dp::ApplyDesignPattern>()
{
    using Intf = ExtraManager::Intf;
    using Attr = ExtraManager::Attr;
    using action::name, action::descr, action::alias;
    constexpr std::string_view helpDescr = getDescr(Category::help), optMetavar = "OPT";
    const std::string helpArg1 = shortPrefix + std::string{getAlias(Category::help)},
                      helpArg2 = longPrefix + std::string{toString(Category::help)};
    std::vector<std::string> candidates{};

    auto& dpTable = taskDispatcher.extraChoiceRegistry[subCLIAppDp.title()];
    taskDispatcher.extraChecklist.emplace(
        subCLIAppDp.title(), Intf{[]() { return !reg_dp::manager().empty(); }, []() { reg_dp::manager().reset(); }});
    subCLIAppDp.addDescription(descr<reg_dp::ApplyDesignPattern>());
    subCLIAppDp.addArgument(helpArg1, helpArg2).argsNum(0).implicitValue(true).help(helpDescr);
    candidates = extractChoices<reg_dp::BehavioralInstance>();
    dpTable.emplace(name<reg_dp::BehavioralInstance>(), Attr{candidates, reg_dp::BehavioralInstance{}});
    subCLIAppDp
        .addArgument(
            shortPrefix + std::string{alias<reg_dp::BehavioralInstance>()},
            longPrefix + std::string{name<reg_dp::BehavioralInstance>()})
        .argsNum(0, candidates.size())
        .defaultValue<std::vector<std::string>>(std::move(candidates))
        .remaining()
        .metavar(optMetavar)
        .help(descr<reg_dp::BehavioralInstance>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<reg_dp::BehavioralInstance>& msg)
                                      { reg_dp::updateChoice<reg_dp::BehavioralInstance>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<reg_dp::BehavioralInstance>& msg)
                                      { reg_dp::runChoices<reg_dp::BehavioralInstance>(msg.coll); });
    versionLinks.emplace(
        VerLinkKey{name<reg_dp::ApplyDesignPattern>(), reg_dp::behavioral::version()},
        name<reg_dp::BehavioralInstance>());
    candidates = extractChoices<reg_dp::CreationalInstance>();
    dpTable.emplace(name<reg_dp::CreationalInstance>(), Attr{candidates, reg_dp::CreationalInstance{}});
    subCLIAppDp
        .addArgument(
            shortPrefix + std::string{alias<reg_dp::CreationalInstance>()},
            longPrefix + std::string{name<reg_dp::CreationalInstance>()})
        .argsNum(0, candidates.size())
        .defaultValue<std::vector<std::string>>(std::move(candidates))
        .remaining()
        .metavar(optMetavar)
        .help(descr<reg_dp::CreationalInstance>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<reg_dp::CreationalInstance>& msg)
                                      { reg_dp::updateChoice<reg_dp::CreationalInstance>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<reg_dp::CreationalInstance>& msg)
                                      { reg_dp::runChoices<reg_dp::CreationalInstance>(msg.coll); });
    versionLinks.emplace(
        VerLinkKey{name<reg_dp::ApplyDesignPattern>(), reg_dp::creational::version()},
        name<reg_dp::CreationalInstance>());
    candidates = extractChoices<reg_dp::StructuralInstance>();
    dpTable.emplace(name<reg_dp::StructuralInstance>(), Attr{candidates, reg_dp::StructuralInstance{}});
    subCLIAppDp
        .addArgument(
            shortPrefix + std::string{alias<reg_dp::StructuralInstance>()},
            longPrefix + std::string{name<reg_dp::StructuralInstance>()})
        .argsNum(0, candidates.size())
        .defaultValue<std::vector<std::string>>(std::move(candidates))
        .remaining()
        .metavar(optMetavar)
        .help(descr<reg_dp::StructuralInstance>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<reg_dp::StructuralInstance>& msg)
                                      { reg_dp::updateChoice<reg_dp::StructuralInstance>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<reg_dp::StructuralInstance>& msg)
                                      { reg_dp::runChoices<reg_dp::StructuralInstance>(msg.coll); });
    versionLinks.emplace(
        VerLinkKey{name<reg_dp::ApplyDesignPattern>(), reg_dp::structural::version()},
        name<reg_dp::StructuralInstance>());
    mainCLI.addSubParser(subCLIAppDp);
}

//! @brief Setup the sub-command line interface (data structure module).
template <>
void Command::setupSubCLI<reg_ds::ApplyDataStructure>()
{
    using Intf = ExtraManager::Intf;
    using Attr = ExtraManager::Attr;
    using action::name, action::descr, action::alias;
    constexpr std::string_view helpDescr = getDescr(Category::help), optMetavar = "OPT";
    const std::string helpArg1 = shortPrefix + std::string{getAlias(Category::help)},
                      helpArg2 = longPrefix + std::string{toString(Category::help)};
    std::vector<std::string> candidates{};

    auto& dsTable = taskDispatcher.extraChoiceRegistry[subCLIAppDs.title()];
    taskDispatcher.extraChecklist.emplace(
        subCLIAppDs.title(), Intf{[]() { return !reg_ds::manager().empty(); }, []() { reg_ds::manager().reset(); }});
    subCLIAppDs.addDescription(descr<reg_ds::ApplyDataStructure>());
    subCLIAppDs.addArgument(helpArg1, helpArg2).argsNum(0).implicitValue(true).help(helpDescr);
    candidates = extractChoices<reg_ds::CacheInstance>();
    dsTable.emplace(name<reg_ds::CacheInstance>(), Attr{candidates, reg_ds::CacheInstance{}});
    subCLIAppDs
        .addArgument(
            shortPrefix + std::string{alias<reg_ds::CacheInstance>()},
            longPrefix + std::string{name<reg_ds::CacheInstance>()})
        .argsNum(0, candidates.size())
        .defaultValue<std::vector<std::string>>(std::move(candidates))
        .remaining()
        .metavar(optMetavar)
        .help(descr<reg_ds::CacheInstance>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<reg_ds::CacheInstance>& msg)
                                      { reg_ds::updateChoice<reg_ds::CacheInstance>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<reg_ds::CacheInstance>& msg)
                                      { reg_ds::runChoices<reg_ds::CacheInstance>(msg.coll); });
    versionLinks.emplace(
        VerLinkKey{name<reg_ds::ApplyDataStructure>(), reg_ds::cache::version()}, name<reg_ds::CacheInstance>());
    candidates = extractChoices<reg_ds::FilterInstance>();
    dsTable.emplace(name<reg_ds::FilterInstance>(), Attr{candidates, reg_ds::FilterInstance{}});
    subCLIAppDs
        .addArgument(
            shortPrefix + std::string{alias<reg_ds::FilterInstance>()},
            longPrefix + std::string{name<reg_ds::FilterInstance>()})
        .argsNum(0, candidates.size())
        .defaultValue<std::vector<std::string>>(std::move(candidates))
        .remaining()
        .metavar(optMetavar)
        .help(descr<reg_ds::FilterInstance>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<reg_ds::FilterInstance>& msg)
                                      { reg_ds::updateChoice<reg_ds::FilterInstance>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<reg_ds::FilterInstance>& msg)
                                      { reg_ds::runChoices<reg_ds::FilterInstance>(msg.coll); });
    versionLinks.emplace(
        VerLinkKey{name<reg_ds::ApplyDataStructure>(), reg_ds::filter::version()}, name<reg_ds::FilterInstance>());
    candidates = extractChoices<reg_ds::LinearInstance>();
    dsTable.emplace(name<reg_ds::LinearInstance>(), Attr{candidates, reg_ds::LinearInstance{}});
    subCLIAppDs
        .addArgument(
            shortPrefix + std::string{alias<reg_ds::LinearInstance>()},
            longPrefix + std::string{name<reg_ds::LinearInstance>()})
        .argsNum(0, candidates.size())
        .defaultValue<std::vector<std::string>>(std::move(candidates))
        .remaining()
        .metavar(optMetavar)
        .help(descr<reg_ds::LinearInstance>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<reg_ds::LinearInstance>& msg)
                                      { reg_ds::updateChoice<reg_ds::LinearInstance>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<reg_ds::LinearInstance>& msg)
                                      { reg_ds::runChoices<reg_ds::LinearInstance>(msg.coll); });
    versionLinks.emplace(
        VerLinkKey{name<reg_ds::ApplyDataStructure>(), reg_ds::linear::version()}, name<reg_ds::LinearInstance>());
    candidates = extractChoices<reg_ds::TreeInstance>();
    dsTable.emplace(name<reg_ds::TreeInstance>(), Attr{candidates, reg_ds::TreeInstance{}});
    subCLIAppDs
        .addArgument(
            shortPrefix + std::string{alias<reg_ds::TreeInstance>()},
            longPrefix + std::string{name<reg_ds::TreeInstance>()})
        .argsNum(0, candidates.size())
        .defaultValue<std::vector<std::string>>(std::move(candidates))
        .remaining()
        .metavar(optMetavar)
        .help(descr<reg_ds::TreeInstance>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<reg_ds::TreeInstance>& msg)
                                      { reg_ds::updateChoice<reg_ds::TreeInstance>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<reg_ds::TreeInstance>& msg)
                                      { reg_ds::runChoices<reg_ds::TreeInstance>(msg.coll); });
    versionLinks.emplace(
        VerLinkKey{name<reg_ds::ApplyDataStructure>(), reg_ds::tree::version()}, name<reg_ds::TreeInstance>());
    mainCLI.addSubParser(subCLIAppDs);
}

//! @brief Setup the sub-command line interface (numeric module).
template <>
void Command::setupSubCLI<reg_num::ApplyNumeric>()
{
    using Intf = ExtraManager::Intf;
    using Attr = ExtraManager::Attr;
    using action::name, action::descr, action::alias;
    constexpr std::string_view helpDescr = getDescr(Category::help), optMetavar = "OPT";
    const std::string helpArg1 = shortPrefix + std::string{getAlias(Category::help)},
                      helpArg2 = longPrefix + std::string{toString(Category::help)};
    std::vector<std::string> candidates{};

    auto& numTable = taskDispatcher.extraChoiceRegistry[subCLIAppNum.title()];
    taskDispatcher.extraChecklist.emplace(
        subCLIAppNum.title(), Intf{[]() { return !reg_num::manager().empty(); }, []() { reg_num::manager().reset(); }});
    subCLIAppNum.addDescription(descr<reg_num::ApplyNumeric>());
    subCLIAppNum.addArgument(helpArg1, helpArg2).argsNum(0).implicitValue(true).help(helpDescr);
    candidates = extractChoices<reg_num::ArithmeticMethod>();
    numTable.emplace(name<reg_num::ArithmeticMethod>(), Attr{candidates, reg_num::ArithmeticMethod{}});
    subCLIAppNum
        .addArgument(
            shortPrefix + std::string{alias<reg_num::ArithmeticMethod>()},
            longPrefix + std::string{name<reg_num::ArithmeticMethod>()})
        .argsNum(0, candidates.size())
        .defaultValue<std::vector<std::string>>(std::move(candidates))
        .remaining()
        .metavar(optMetavar)
        .help(descr<reg_num::ArithmeticMethod>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<reg_num::ArithmeticMethod>& msg)
                                      { reg_num::updateChoice<reg_num::ArithmeticMethod>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<reg_num::ArithmeticMethod>& msg)
                                      { reg_num::runChoices<reg_num::ArithmeticMethod>(msg.coll); });
    versionLinks.emplace(
        VerLinkKey{name<reg_num::ApplyNumeric>(), reg_num::arithmetic::version()}, name<reg_num::ArithmeticMethod>());
    candidates = extractChoices<reg_num::DivisorMethod>();
    numTable.emplace(name<reg_num::DivisorMethod>(), Attr{candidates, reg_num::DivisorMethod{}});
    subCLIAppNum
        .addArgument(
            shortPrefix + std::string{alias<reg_num::DivisorMethod>()},
            longPrefix + std::string{name<reg_num::DivisorMethod>()})
        .argsNum(0, candidates.size())
        .defaultValue<std::vector<std::string>>(std::move(candidates))
        .remaining()
        .metavar(optMetavar)
        .help(descr<reg_num::DivisorMethod>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<reg_num::DivisorMethod>& msg)
                                      { reg_num::updateChoice<reg_num::DivisorMethod>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<reg_num::DivisorMethod>& msg)
                                      { reg_num::runChoices<reg_num::DivisorMethod>(msg.coll); });
    versionLinks.emplace(
        VerLinkKey{name<reg_num::ApplyNumeric>(), reg_num::divisor::version()}, name<reg_num::DivisorMethod>());
    candidates = extractChoices<reg_num::IntegralMethod>();
    numTable.emplace(name<reg_num::IntegralMethod>(), Attr{candidates, reg_num::IntegralMethod{}});
    subCLIAppNum
        .addArgument(
            shortPrefix + std::string{alias<reg_num::IntegralMethod>()},
            longPrefix + std::string{name<reg_num::IntegralMethod>()})
        .argsNum(0, candidates.size())
        .defaultValue<std::vector<std::string>>(std::move(candidates))
        .remaining()
        .metavar(optMetavar)
        .help(descr<reg_num::IntegralMethod>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<reg_num::IntegralMethod>& msg)
                                      { reg_num::updateChoice<reg_num::IntegralMethod>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<reg_num::IntegralMethod>& msg)
                                      { reg_num::runChoices<reg_num::IntegralMethod>(msg.coll); });
    versionLinks.emplace(
        VerLinkKey{name<reg_num::ApplyNumeric>(), reg_num::integral::version()}, name<reg_num::IntegralMethod>());
    candidates = extractChoices<reg_num::PrimeMethod>();
    numTable.emplace(name<reg_num::PrimeMethod>(), Attr{candidates, reg_num::PrimeMethod{}});
    subCLIAppNum
        .addArgument(
            shortPrefix + std::string{alias<reg_num::PrimeMethod>()},
            longPrefix + std::string{name<reg_num::PrimeMethod>()})
        .argsNum(0, candidates.size())
        .defaultValue<std::vector<std::string>>(std::move(candidates))
        .remaining()
        .metavar(optMetavar)
        .help(descr<reg_num::PrimeMethod>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<reg_num::PrimeMethod>& msg)
                                      { reg_num::updateChoice<reg_num::PrimeMethod>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<reg_num::PrimeMethod>& msg)
                                      { reg_num::runChoices<reg_num::PrimeMethod>(msg.coll); });
    versionLinks.emplace(
        VerLinkKey{name<reg_num::ApplyNumeric>(), reg_num::prime::version()}, name<reg_num::PrimeMethod>());
    mainCLI.addSubParser(subCLIAppNum);
}

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
             | std::views::filter([this](const auto i) { return mainCLI.isUsed(toString(Category(i))); }))
    {
        checkForExcessiveArguments();
        bits.set(Category(index));
    }

    for (constexpr auto helpArgInExtra = toString(Category::help);
         [[maybe_unused]] const auto& [subCLIName, categoryMap] :
         taskDispatcher.extraChoiceRegistry
             | std::views::filter(
                 [this](const auto& subCLIPair)
                 { return mainCLI.isSubCommandUsed(subCLIPair.first) ? (checkForExcessiveArguments(), true) : false; }))
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
                     { return subCLI.isUsed(categoryPair.first) ? (checkForExcessiveArguments(), true) : false; }))
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
            builtInNotifier.notify(Category(index));
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
    std::vector<std::string> choices{};
    choices.reserve(utility::reflection::TypeInfo<T>::fields.size);
    utility::reflection::TypeInfo<T>::fields.forEach(
        [&choices](const auto field)
        {
            static_assert(field.attrs.size == 1);
            const auto attr = field.attrs.find(REFLECTION_STR("choice"));
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
        disableWait4Client();
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
        disableWait4Client();
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
    udpClient->toSend(buildExitRequest4Client());
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
    std::cout << note::overview() << std::flush;
}

void Command::checkForExcessiveArguments()
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
    tcpClient->toSend(buildExitRequest4Client());
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
            enableWait4Client();
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
                client->toSend(buildExitRequest4Client());
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

void Command::enableWait4Client()
{
    view::View::Access().enableWait();
}

void Command::disableWait4Client()
{
    view::View::Access().disableWait();
}

std::string Command::buildExitRequest4Client()
{
    return utility::common::base64Encode(view::exitSymbol);
}

void Command::interactionLatency()
{
    constexpr std::uint16_t latency = 10;
    std::this_thread::sleep_for(std::chrono::milliseconds{latency});
}

void Command::validateDependenciesVersion() const
{
    if (!utility::common::areStringsEqual(
            mainCLI.version().data(),
            utility::argument::version(),
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
            utility::time::version()))
    {
        throw std::runtime_error{"Main dependency version number mismatch."};
    }

    if (const auto& choiceRegistry = taskDispatcher.extraChoiceRegistry;
        (versionLinks.count({subCLIAppAlgo.title(), subCLIAppAlgo.version()})
         != choiceRegistry.at(subCLIAppAlgo.title()).size())
        || (versionLinks.count({subCLIAppDp.title(), subCLIAppDp.version()})
            != choiceRegistry.at(subCLIAppDp.title()).size())
        || (versionLinks.count({subCLIAppDs.title(), subCLIAppDs.version()})
            != choiceRegistry.at(subCLIAppDs.title()).size())
        || (versionLinks.count({subCLIAppNum.title(), subCLIAppNum.version()})
            != choiceRegistry.at(subCLIAppNum.title()).size()))
    {
        throw std::runtime_error{"Sub-dependency version number mismatch."};
    }
}
} // namespace application::command
