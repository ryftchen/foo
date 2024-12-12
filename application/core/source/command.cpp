//! @file command.cpp
//! @author ryftchen
//! @brief The definitions (command) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#include "command.hpp"
#include "log.hpp"
#include "view.hpp"

#ifndef __PRECOMPILED_HEADER
#include <barrier>
#include <coroutine>
#include <latch>
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
concept HelperType = !std::is_constructible_v<T> && !std::is_copy_constructible_v<T> && !std::is_copy_assignable_v<T>
    && !std::is_move_constructible_v<T> && !std::is_move_assignable_v<T> &&
    requires (T /*helper*/)
{
    {
        T::getInstance()
    } -> std::same_as<T&>;
};

//! @brief Enumerate specific events to control external helpers.
enum class ExtEvent : std::uint8_t
{
    //! @brief Start.
    start,
    //! @brief Stop.
    stop,
    //! @brief Reset.
    reset
};

//! @brief Get the helper name.
//! @tparam Helper - type of helper
//! @return helper name
template <HelperType Helper>
consteval std::string_view getHelperName()
{
    if constexpr (std::is_same_v<Helper, log::Log>)
    {
        return "logger";
    }
    else if constexpr (std::is_same_v<Helper, view::View>)
    {
        return "viewer";
    }

    return "helper";
}

//! @brief Trigger the external helper with event.
//! @tparam Helper - type of helper
//! @param event - target event
template <HelperType Helper>
requires std::derived_from<Helper, utility::fsm::FSM<Helper>>
void triggerHelper(const ExtEvent event)
{
    if (!config::detail::activateHelper())
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

//! @brief Helper daemon function.
//! @tparam Helpers - type of arguments of helper
template <HelperType... Helpers>
requires (std::derived_from<Helpers, utility::fsm::FSM<Helpers>> && ...)
void helperDaemon()
{
    utility::thread::Thread extendingThd(sizeof...(Helpers));
    (extendingThd.enqueue(getHelperName<Helpers>(), &Helpers::stateController, &Helpers::getInstance()), ...);
}

//! @brief Awaitable coroutine.
class Awaitable
{
public:
    // NOLINTBEGIN(readability-identifier-naming)
    //! @brief Promise type for use in coroutines.
    struct promise_type
    {
        //! @brief Get the return object for the coroutine.
        //! @return awaitable instance
        Awaitable get_return_object() { return Awaitable{std::coroutine_handle<promise_type>::from_promise(*this)}; }
        //! @brief Initial suspend point of the coroutine.
        //! @return std::suspend_never object indicating that the coroutine should not be suspended initially
        static std::suspend_never initial_suspend() noexcept { return {}; }
        //! @brief Final suspend point of the coroutine.
        //! @return std::suspend_always object indicating that the coroutine should be suspended finally
        static std::suspend_always final_suspend() noexcept { return {}; }
        //! @brief Complete the coroutine without returning a value.
        static void return_void() noexcept {}
        //! @brief Handle exceptions thrown within the coroutine.
        static void unhandled_exception() { std::rethrow_exception(std::current_exception()); }
    };
    // NOLINTEND(readability-identifier-naming)

    //! @brief Construct a new Awaitable object.
    //! @param handle - coroutine handle
    explicit Awaitable(std::coroutine_handle<promise_type> handle) : handle(handle) {}
    //! @brief Destroy the Awaitable object.
    ~Awaitable()
    {
        if (handle)
        {
            handle.destroy();
        }
    }
    //! @brief Construct a new Awaitable object.
    Awaitable(const Awaitable&) = delete;
    //! @brief Construct a new Awaitable object.
    Awaitable(Awaitable&&) = delete;
    //! @brief The operator (=) overloading of Awaitable class.
    //! @return reference of the Awaitable object
    Awaitable& operator=(const Awaitable&) = delete;
    //! @brief The operator (=) overloading of Awaitable class.
    //! @return reference of the Awaitable object
    Awaitable& operator=(Awaitable&&) = delete;

    //! @brief Resume the execution of the coroutine if it is suspended.
    void resume() const
    {
        if (handle)
        {
            handle.resume();
        }
    }
    //! @brief Check whether the coroutine has been completed.
    //! @return be done or not
    [[nodiscard]] bool done() const { return handle ? handle.done() : true; }

private:
    //! @brief Coroutine handle.
    std::coroutine_handle<promise_type> handle{};
};

//! @brief Coroutine for managing the lifecycle of helper components.
//! @tparam Helpers - type of arguments of helper
//! @return object that represents the execution of the coroutine
template <HelperType... Helpers>
Awaitable helperLifecycle()
{
    if (!config::detail::activateHelper())
    {
        co_return;
    }

    std::latch awaitDaemon(1);
    const std::jthread daemon(
        [&awaitDaemon]()
        {
            helperDaemon<Helpers...>();
            awaitDaemon.count_down();
        });
    std::barrier awaitPublish(sizeof...(Helpers) + 1);
    const auto publish = [&awaitPublish](const ExtEvent event)
    {
        std::vector<std::jthread> senders(sizeof...(Helpers));
        (senders.emplace_back(std::jthread{[&awaitPublish, event]()
                                           {
                                               triggerHelper<Helpers>(event);
                                               awaitPublish.arrive_and_wait();
                                           }}),
         ...);
        awaitPublish.arrive_and_wait();
    };

    co_await std::suspend_always{};
    publish(ExtEvent::start);
    co_await std::suspend_always{};
    publish(ExtEvent::stop);
    awaitDaemon.wait();
}
} // namespace

//! @brief Mapping table for enum and string about command categories. X macro.
#define COMMAND_CATEGORY_TABLE \
    ELEM(console, "console")   \
    ELEM(dump, "dump")         \
    ELEM(help, "help")         \
    ELEM(version, "version")
//! @brief Convert category enumeration to string.
//! @param cat - the specific value of Category enum
//! @return category name
constexpr std::string_view toString(const Category cat)
{
//! @cond
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {COMMAND_CATEGORY_TABLE};
    static_assert((sizeof(table) / sizeof(table[0])) == Bottom<Category>::value);
    return table[cat];
//! @endcond
#undef ELEM
}
#undef COMMAND_CATEGORY_TABLE

Command::Command()
{
    initializeCLI();
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

void Command::execManager(const int argc, const char* const argv[])
try
{
    auto launcher = helperLifecycle<log::Log, view::View>();
    if (!launcher.done())
    {
        launcher.resume();
    }

    if (1 == argc)
    {
        enterConsoleMode();
    }
    else
    {
        constexpr std::uint8_t endNum = 2;
        utility::thread::Thread handlingThd(endNum);
        handlingThd.enqueue("commander(FE)", &Command::frontEndHandler, this, argc, argv);
        handlingThd.enqueue("commander(BE)", &Command::backEndHandler, this);
    }

    if (!launcher.done())
    {
        launcher.resume();
    }
}
catch (const std::exception& err)
{
    LOG_ERR << err.what();
}

// NOLINTNEXTLINE(readability-function-size)
void Command::initializeCLI()
{
    mainCLI.addArgument("-h", "--help").argsNum(0).implicitVal(true).help("show help and exit");
    defaultNotifier.attach(Category::help, std::make_shared<Notifier::Handler<Category::help>>(*this));
    mainCLI.addArgument("-v", "--version").argsNum(0).implicitVal(true).help("show version and exit");
    defaultNotifier.attach(Category::version, std::make_shared<Notifier::Handler<Category::version>>(*this));
    mainCLI.addArgument("-d", "--dump").argsNum(0).implicitVal(true).help("dump default configuration and exit");
    defaultNotifier.attach(Category::dump, std::make_shared<Notifier::Handler<Category::dump>>(*this));
    mainCLI.addArgument("-c", "--console")
        .argsNum(utility::argument::ArgsNumPattern::any)
        .defaultVal<std::vector<std::string>>({"usage"})
        .appending()
        .action(
            [](const std::string& input)
            {
                if (std::all_of(
                        input.cbegin(), input.cend(), [l = std::locale{}](const auto c) { return std::isspace(c, l); }))
                {
                    throw std::runtime_error("Invalid console command.");
                }
                return input;
            })
        .metavar("CMD")
        .help("run options in console mode and exit\n"
              "separate with quotes");
    defaultNotifier.attach(Category::console, std::make_shared<Notifier::Handler<Category::console>>(*this));

    SubCLIName title{};
    CategoryName category{};
    ChoiceContainer choices{};
    auto& checklist = taskDispatcher.extraChecklist;

    title = subCLIAppAlgo.title();
    auto& algoTable = extraChoices[title];
    checklist[title] =
        ExtraManager::IntfWrap{[]() { return !app_algo::manager().empty(); }, []() { app_algo::manager().reset(); }};
    subCLIAppAlgo.addDescription(getDescr<app_algo::ApplyAlgorithm>());
    subCLIAppAlgo.addArgument("-h", "--help").argsNum(0).implicitVal(true).help("show help and exit");
    category = TypeInfo<app_algo::MatchMethod>::name;
    choices = extractChoices<app_algo::MatchMethod>();
    algoTable[category] = CategoryExtAttr{choices, app_algo::MatchMethod{}};
    subCLIAppAlgo
        .addArgument("-" + std::string{getAlias<app_algo::ApplyAlgorithm, app_algo::MatchMethod>()}, "--" + category)
        .argsNum(0, choices.size())
        .defaultVal<std::vector<std::string>>(std::move(choices))
        .remaining()
        .metavar("OPT")
        .help(getDescr<app_algo::MatchMethod>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<app_algo::MatchMethod>& msg)
                                      { app_algo::updateChoice<app_algo::MatchMethod>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<app_algo::MatchMethod>& msg)
                                      { app_algo::runChoices<app_algo::MatchMethod>(msg.coll); });
    category = TypeInfo<app_algo::NotationMethod>::name;
    choices = extractChoices<app_algo::NotationMethod>();
    algoTable[category] = CategoryExtAttr{choices, app_algo::NotationMethod{}};
    subCLIAppAlgo
        .addArgument("-" + std::string{getAlias<app_algo::ApplyAlgorithm, app_algo::NotationMethod>()}, "--" + category)
        .argsNum(0, choices.size())
        .defaultVal<std::vector<std::string>>(std::move(choices))
        .remaining()
        .metavar("OPT")
        .help(getDescr<app_algo::NotationMethod>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<app_algo::NotationMethod>& msg)
                                      { app_algo::updateChoice<app_algo::NotationMethod>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<app_algo::NotationMethod>& msg)
                                      { app_algo::runChoices<app_algo::NotationMethod>(msg.coll); });
    category = TypeInfo<app_algo::OptimalMethod>::name;
    choices = extractChoices<app_algo::OptimalMethod>();
    algoTable[category] = CategoryExtAttr{choices, app_algo::OptimalMethod{}};
    subCLIAppAlgo
        .addArgument("-" + std::string{getAlias<app_algo::ApplyAlgorithm, app_algo::OptimalMethod>()}, "--" + category)
        .argsNum(0, choices.size())
        .defaultVal<std::vector<std::string>>(std::move(choices))
        .remaining()
        .metavar("OPT")
        .help(getDescr<app_algo::OptimalMethod>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<app_algo::OptimalMethod>& msg)
                                      { app_algo::updateChoice<app_algo::OptimalMethod>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<app_algo::OptimalMethod>& msg)
                                      { app_algo::runChoices<app_algo::OptimalMethod>(msg.coll); });
    category = TypeInfo<app_algo::SearchMethod>::name;
    choices = extractChoices<app_algo::SearchMethod>();
    algoTable[category] = CategoryExtAttr{choices, app_algo::SearchMethod{}};
    subCLIAppAlgo
        .addArgument("-" + std::string{getAlias<app_algo::ApplyAlgorithm, app_algo::SearchMethod>()}, "--" + category)
        .argsNum(0, choices.size())
        .defaultVal<std::vector<std::string>>(std::move(choices))
        .remaining()
        .metavar("OPT")
        .help(getDescr<app_algo::SearchMethod>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<app_algo::SearchMethod>& msg)
                                      { app_algo::updateChoice<app_algo::SearchMethod>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<app_algo::SearchMethod>& msg)
                                      { app_algo::runChoices<app_algo::SearchMethod>(msg.coll); });
    category = TypeInfo<app_algo::SortMethod>::name;
    choices = extractChoices<app_algo::SortMethod>();
    algoTable[category] = CategoryExtAttr{choices, app_algo::SortMethod{}};
    subCLIAppAlgo
        .addArgument("-" + std::string{getAlias<app_algo::ApplyAlgorithm, app_algo::SortMethod>()}, "--" + category)
        .argsNum(0, choices.size())
        .defaultVal<std::vector<std::string>>(std::move(choices))
        .remaining()
        .metavar("OPT")
        .help(getDescr<app_algo::SortMethod>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<app_algo::SortMethod>& msg)
                                      { app_algo::updateChoice<app_algo::SortMethod>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<app_algo::SortMethod>& msg)
                                      { app_algo::runChoices<app_algo::SortMethod>(msg.coll); });
    mainCLI.addSubParser(subCLIAppAlgo);

    title = subCLIAppDp.title();
    auto& dpTable = extraChoices[title];
    checklist[title] =
        ExtraManager::IntfWrap{[]() { return !app_dp::manager().empty(); }, []() { app_dp::manager().reset(); }};
    subCLIAppDp.addDescription(getDescr<app_dp::ApplyDesignPattern>());
    subCLIAppDp.addArgument("-h", "--help").argsNum(0).implicitVal(true).help("show help and exit");
    category = TypeInfo<app_dp::BehavioralInstance>::name;
    choices = extractChoices<app_dp::BehavioralInstance>();
    dpTable[category] = CategoryExtAttr{choices, app_dp::BehavioralInstance{}};
    subCLIAppDp
        .addArgument(
            "-" + std::string{getAlias<app_dp::ApplyDesignPattern, app_dp::BehavioralInstance>()}, "--" + category)
        .argsNum(0, choices.size())
        .defaultVal<std::vector<std::string>>(std::move(choices))
        .remaining()
        .metavar("OPT")
        .help(getDescr<app_dp::BehavioralInstance>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<app_dp::BehavioralInstance>& msg)
                                      { app_dp::updateChoice<app_dp::BehavioralInstance>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<app_dp::BehavioralInstance>& msg)
                                      { app_dp::runChoices<app_dp::BehavioralInstance>(msg.coll); });
    category = TypeInfo<app_dp::CreationalInstance>::name;
    choices = extractChoices<app_dp::CreationalInstance>();
    dpTable[category] = CategoryExtAttr{choices, app_dp::CreationalInstance{}};
    subCLIAppDp
        .addArgument(
            "-" + std::string{getAlias<app_dp::ApplyDesignPattern, app_dp::CreationalInstance>()}, "--" + category)
        .argsNum(0, choices.size())
        .defaultVal<std::vector<std::string>>(std::move(choices))
        .remaining()
        .metavar("OPT")
        .help(getDescr<app_dp::CreationalInstance>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<app_dp::CreationalInstance>& msg)
                                      { app_dp::updateChoice<app_dp::CreationalInstance>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<app_dp::CreationalInstance>& msg)
                                      { app_dp::runChoices<app_dp::CreationalInstance>(msg.coll); });
    category = TypeInfo<app_dp::StructuralInstance>::name;
    choices = extractChoices<app_dp::StructuralInstance>();
    dpTable[category] = CategoryExtAttr{choices, app_dp::StructuralInstance{}};
    subCLIAppDp
        .addArgument(
            "-" + std::string{getAlias<app_dp::ApplyDesignPattern, app_dp::StructuralInstance>()}, "--" + category)
        .argsNum(0, choices.size())
        .defaultVal<std::vector<std::string>>(std::move(choices))
        .remaining()
        .metavar("OPT")
        .help(getDescr<app_dp::StructuralInstance>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<app_dp::StructuralInstance>& msg)
                                      { app_dp::updateChoice<app_dp::StructuralInstance>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<app_dp::StructuralInstance>& msg)
                                      { app_dp::runChoices<app_dp::StructuralInstance>(msg.coll); });
    mainCLI.addSubParser(subCLIAppDp);

    title = subCLIAppDs.title();
    auto& dsTable = extraChoices[title];
    checklist[title] =
        ExtraManager::IntfWrap{[]() { return !app_ds::manager().empty(); }, []() { app_ds::manager().reset(); }};
    subCLIAppDs.addDescription(getDescr<app_ds::ApplyDataStructure>());
    subCLIAppDs.addArgument("-h", "--help").argsNum(0).implicitVal(true).help("show help and exit");
    category = TypeInfo<app_ds::LinearInstance>::name;
    choices = extractChoices<app_ds::LinearInstance>();
    dsTable[category] = CategoryExtAttr{choices, app_ds::LinearInstance{}};
    subCLIAppDs
        .addArgument("-" + std::string{getAlias<app_ds::ApplyDataStructure, app_ds::LinearInstance>()}, "--" + category)
        .argsNum(0, choices.size())
        .defaultVal<std::vector<std::string>>(std::move(choices))
        .remaining()
        .metavar("OPT")
        .help(getDescr<app_ds::LinearInstance>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<app_ds::LinearInstance>& msg)
                                      { app_ds::updateChoice<app_ds::LinearInstance>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<app_ds::LinearInstance>& msg)
                                      { app_ds::runChoices<app_ds::LinearInstance>(msg.coll); });
    category = TypeInfo<app_ds::TreeInstance>::name;
    choices = extractChoices<app_ds::TreeInstance>();
    dsTable[category] = CategoryExtAttr{choices, app_ds::TreeInstance{}};
    subCLIAppDs
        .addArgument("-" + std::string{getAlias<app_ds::ApplyDataStructure, app_ds::TreeInstance>()}, "--" + category)
        .argsNum(0, choices.size())
        .defaultVal<std::vector<std::string>>(std::move(choices))
        .remaining()
        .metavar("OPT")
        .help(getDescr<app_ds::TreeInstance>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<app_ds::TreeInstance>& msg)
                                      { app_ds::updateChoice<app_ds::TreeInstance>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<app_ds::TreeInstance>& msg)
                                      { app_ds::runChoices<app_ds::TreeInstance>(msg.coll); });
    mainCLI.addSubParser(subCLIAppDs);

    title = subCLIAppNum.title();
    auto& numTable = extraChoices[title];
    checklist[title] =
        ExtraManager::IntfWrap{[]() { return !app_num::manager().empty(); }, []() { app_num::manager().reset(); }};
    subCLIAppNum.addDescription(getDescr<app_num::ApplyNumeric>());
    subCLIAppNum.addArgument("-h", "--help").argsNum(0).implicitVal(true).help("show help and exit");
    category = TypeInfo<app_num::ArithmeticMethod>::name;
    choices = extractChoices<app_num::ArithmeticMethod>();
    numTable[category] = CategoryExtAttr{choices, app_num::ArithmeticMethod{}};
    subCLIAppNum
        .addArgument("-" + std::string{getAlias<app_num::ApplyNumeric, app_num::ArithmeticMethod>()}, "--" + category)
        .argsNum(0, choices.size())
        .defaultVal<std::vector<std::string>>(std::move(choices))
        .remaining()
        .metavar("OPT")
        .help(getDescr<app_num::ArithmeticMethod>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<app_num::ArithmeticMethod>& msg)
                                      { app_num::updateChoice<app_num::ArithmeticMethod>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<app_num::ArithmeticMethod>& msg)
                                      { app_num::runChoices<app_num::ArithmeticMethod>(msg.coll); });
    category = TypeInfo<app_num::DivisorMethod>::name;
    choices = extractChoices<app_num::DivisorMethod>();
    numTable[category] = CategoryExtAttr{choices, app_num::DivisorMethod{}};
    subCLIAppNum
        .addArgument("-" + std::string{getAlias<app_num::ApplyNumeric, app_num::DivisorMethod>()}, "--" + category)
        .argsNum(0, choices.size())
        .defaultVal<std::vector<std::string>>(std::move(choices))
        .remaining()
        .metavar("OPT")
        .help(getDescr<app_num::DivisorMethod>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<app_num::DivisorMethod>& msg)
                                      { app_num::updateChoice<app_num::DivisorMethod>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<app_num::DivisorMethod>& msg)
                                      { app_num::runChoices<app_num::DivisorMethod>(msg.coll); });
    category = TypeInfo<app_num::IntegralMethod>::name;
    choices = extractChoices<app_num::IntegralMethod>();
    numTable[category] = CategoryExtAttr{choices, app_num::IntegralMethod{}};
    subCLIAppNum
        .addArgument("-" + std::string{getAlias<app_num::ApplyNumeric, app_num::IntegralMethod>()}, "--" + category)
        .argsNum(0, choices.size())
        .defaultVal<std::vector<std::string>>(std::move(choices))
        .remaining()
        .metavar("OPT")
        .help(getDescr<app_num::IntegralMethod>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<app_num::IntegralMethod>& msg)
                                      { app_num::updateChoice<app_num::IntegralMethod>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<app_num::IntegralMethod>& msg)
                                      { app_num::runChoices<app_num::IntegralMethod>(msg.coll); });
    category = TypeInfo<app_num::PrimeMethod>::name;
    choices = extractChoices<app_num::PrimeMethod>();
    numTable[category] = CategoryExtAttr{choices, app_num::PrimeMethod{}};
    subCLIAppNum
        .addArgument("-" + std::string{getAlias<app_num::ApplyNumeric, app_num::PrimeMethod>()}, "--" + category)
        .argsNum(0, choices.size())
        .defaultVal<std::vector<std::string>>(std::move(choices))
        .remaining()
        .metavar("OPT")
        .help(getDescr<app_num::PrimeMethod>());
    applyingForwarder.registerHandler([](const action::UpdateChoice<app_num::PrimeMethod>& msg)
                                      { app_num::updateChoice<app_num::PrimeMethod>(msg.cho); });
    applyingForwarder.registerHandler([](const action::RunChoices<app_num::PrimeMethod>& msg)
                                      { app_num::runChoices<app_num::PrimeMethod>(msg.coll); });
    mainCLI.addSubParser(subCLIAppNum);
}

void Command::frontEndHandler(const int argc, const char* const argv[])
try
{
    std::unique_lock<std::mutex> parserLock(parserMtx);
    mainCLI.parseArgs(argc, argv);
    validate();

    isParsed.store(true);
    parserLock.unlock();
    parserCond.notify_one();
}
catch (const std::exception& err)
{
    isParsed.store(true);
    parserCond.notify_one();
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
    LOG_WRN << err.what();
}

void Command::validate()
{
    auto& bits = taskDispatcher.nativeCategories;
    auto indices = std::views::iota(0U, bits.size())
        | std::views::filter([this](const auto i) { return mainCLI.isUsed(toString(Category(i))); });
    for (const auto index : indices)
    {
        checkForExcessiveArguments();
        bits.set(Category(index));
    }

    for ([[maybe_unused]] const auto& [subCLIName, categoryMap] :
         extraChoices
             | std::views::filter(
                 [this](const auto& subCLIPair)
                 { return mainCLI.isSubCommandUsed(subCLIPair.first) ? (checkForExcessiveArguments(), true) : false; }))
    {
        const auto& subCLI = mainCLI.at<utility::argument::Argument>(subCLIName);
        taskDispatcher.extraHelpOnly = !subCLI || subCLI.isUsed("help");
        if (!subCLI)
        {
            return;
        }

        for ([[maybe_unused]] const auto& [categoryName, categoryAttr] :
             categoryMap
                 | std::views::filter(
                     [this, &subCLI](const auto& categoryPair)
                     { return subCLI.isUsed(categoryPair.first) ? (checkForExcessiveArguments(), true) : false; }))
        {
            const auto& pendingTasks = subCLI.get<std::vector<std::string>>(categoryName);
            for (const auto& target : pendingTasks)
            {
                std::visit(
                    action::EvtVisitor{[this, &target](auto&& event) {
                        applyingForwarder.onMessage(action::UpdateChoice<std::decay_t<decltype(event)>>{target});
                    }},
                    categoryAttr.event);
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
        const auto& bits = taskDispatcher.nativeCategories;
        auto indices =
            std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); });
        for (const auto index : indices)
        {
            defaultNotifier.notify(Category(index));
        }
    }
    else if (!taskDispatcher.ExtraManager::empty())
    {
        if (taskDispatcher.extraHelpOnly)
        {
            auto filtered = std::views::keys(extraChoices)
                | std::views::filter([this](const auto& subCLIName) { return mainCLI.isSubCommandUsed(subCLIName); });
            if (std::ranges::distance(filtered) != 0)
            {
                const auto& subCLI = mainCLI.at<utility::argument::Argument>(*std::ranges::begin(filtered));
                std::cout << subCLI.help().str() << std::flush;
            }
            return;
        }

        for ([[maybe_unused]] const auto& [categoryName, categoryAttr] : extraChoices
                 | std::views::filter([this](const auto& subCLIPair)
                                      { return taskDispatcher.extraChecklist[subCLIPair.first].present(); })
                 | std::views::values | std::views::join)
        {
            const auto& candidates = categoryAttr.choices;
            std::visit(
                action::EvtVisitor{[this, &candidates](auto&& event) {
                    applyingForwarder.onMessage(action::RunChoices<std::decay_t<decltype(event)>>{candidates});
                }},
                categoryAttr.event);
        }
    }
}

template <typename T>
Command::ChoiceContainer Command::extractChoices()
{
    using TypeInfo = utility::reflection::TypeInfo<T>;
    ChoiceContainer choices{};
    choices.reserve(TypeInfo::fields.size);
    TypeInfo::fields.forEach([&choices](const auto field)
                             { choices.emplace_back(field.attrs.find(REFLECTION_STR("choice")).value); });

    return choices;
}

auto Command::parseMessageInsideClient(char* buffer, const int length)
{
    return view::View::getInstance().parseTLVPacket(buffer, length);
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
            if ((0 != length) && parseMessageInsideClient(buffer, length).stopTag)
            {
                client->asyncExit();
            }
        }
        catch (const std::exception& err)
        {
            LOG_WRN << err.what();
        }
        awakenInsideClient();
    };
    client->toConnect(view::info::viewerTCPHost(), view::info::viewerTCPPort());
}

//! @brief Launch the UDP client for console mode.
//! @param client - UDP client to be launched
template <>
void Command::launchClient<utility::socket::UDPSocket>(std::shared_ptr<utility::socket::UDPSocket>& client)
{
    client->onRawMessageReceived =
        [&client](char* buffer, const int length, const std::string_view /*ip*/, const std::uint16_t /*port*/)
    {
        try
        {
            if ((0 != length) && parseMessageInsideClient(buffer, length).stopTag)
            {
                client->asyncExit();
            }
        }
        catch (const std::exception& err)
        {
            LOG_WRN << err.what();
        }
        awakenInsideClient();
    };
    client->toReceive();
    client->toConnect(view::info::viewerUDPHost(), view::info::viewerUDPPort());
}

void Command::executeInConsole() const
{
    if (!config::detail::activateHelper())
    {
        std::cout << "exit" << std::endl;
        return;
    }

    const auto& pendingInputs = mainCLI.get<std::vector<std::string>>(toString(Category::console));
    if (pendingInputs.empty())
    {
        return;
    }

    auto udpClient = std::make_shared<utility::socket::UDPSocket>();
    launchClient(udpClient);
    const auto session = std::make_shared<console::Console>(" > ");
    registerOnConsole(*session, udpClient);
    for (const auto& option : pendingInputs)
    {
        session->optionExecutor(option);
    }

    udpClient->toSend(utility::common::base64Encode("stop"));
    udpClient->waitIfAlive();
    utility::time::millisecondLevelSleep(latency);
}

void Command::showHelpMessage() const
{
    std::cout << mainCLI.help().str() << std::flush;
}

void Command::dumpConfiguration()
{
    std::cout << config::getDefaultConfiguration() << std::endl;
}

void Command::showVersionIcon() const
{
    validateDependenciesVersion();
    std::cout << utility::io::executeCommand(std::format(
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
        note::buildDate()))
              << std::flush;
}

void Command::checkForExcessiveArguments()
{
    if (anySelected())
    {
        taskDispatcher.reset();
        throw std::runtime_error("Excessive arguments.");
    }
}

//! @brief Perform the specific operation for Category::console.
template <>
void Command::Notifier::Handler<Category::console>::execute() const
{
    obj.executeInConsole();
}

//! @brief Perform the specific operation for Category::dump.
template <>
void Command::Notifier::Handler<Category::dump>::execute() const
{
    obj.dumpConfiguration();
}

//! @brief Perform the specific operation for Category::help.
template <>
void Command::Notifier::Handler<Category::help>::execute() const
{
    obj.showHelpMessage();
}

//! @brief Perform the specific operation for Category::version.
template <>
void Command::Notifier::Handler<Category::version>::execute() const
{
    obj.showVersionIcon();
}

void Command::enterConsoleMode()
try
{
    if (!config::detail::activateHelper())
    {
        std::cout << "exit" << std::endl;
        return;
    }

#ifndef NDEBUG
    LOG_DBG << "Enter console mode.";
#endif // NDEBUG
    using enum console::Console::RetCode;
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
    const auto session = std::make_shared<console::Console>(greeting);
    registerOnConsole(*session, tcpClient);

    int retVal = success;
    do
    {
        try
        {
            retVal = session->readLine();
        }
        catch (const std::exception& err)
        {
            LOG_WRN << err.what();
        }
        session->setGreeting(greeting);
        utility::time::millisecondLevelSleep(latency);
    }
    while (quit != retVal);

    tcpClient->toSend(utility::common::base64Encode("stop"));
    tcpClient->waitIfAlive();
    utility::time::millisecondLevelSleep(latency);
#ifndef NDEBUG
    LOG_DBG << "Exit console mode.";
#endif // NDEBUG
}
catch (const std::exception& err)
{
    LOG_ERR << err.what();
}

template <typename T>
void Command::registerOnConsole(console::Console& session, std::shared_ptr<T>& client)
{
    using console::Console;
    using enum Console::RetCode;
    static constexpr auto resetter = []<HelperType Helper>() constexpr
    {
        triggerHelper<Helper>(ExtEvent::reset);
        triggerHelper<Helper>(ExtEvent::start);
    };
    const auto sender = [&client](const Console::Args& inputs)
    {
        auto retVal = success;
        try
        {
            client->toSend(utility::common::base64Encode(std::accumulate(
                inputs.cbegin(),
                inputs.cend(),
                std::string{},
                [](const auto& acc, const auto& token) { return acc.empty() ? token : (acc + ' ' + token); })));
            awaitOutsideClient();
        }
        catch (const std::exception& err)
        {
            retVal = error;
            LOG_WRN << err.what();
            utility::time::millisecondLevelSleep(latency);
        }
        return retVal;
    };

    session.registerOption(
        "refresh",
        "refresh the outputs",
        [](const Console::Args& /*inputs*/)
        {
            auto retVal = success;
            try
            {
                resetter.template operator()<log::Log>();

                LOG_INF << "Refreshed the outputs.";
            }
            catch (const std::exception& err)
            {
                retVal = error;
                LOG_WRN << err.what();
            }
            utility::time::millisecondLevelSleep(latency);
            return retVal;
        });
    session.registerOption(
        "reconnect",
        "reconnect to the servers",
        [&client](const Console::Args& /*inputs*/)
        {
            auto retVal = success;
            try
            {
                client->toSend(utility::common::base64Encode("stop"));
                client->waitIfAlive();
                utility::time::millisecondLevelSleep(latency);
                client.reset();
                resetter.template operator()<view::View>();

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
        });
    for (const auto& [name, attr] : view::info::getAllOptions())
    {
        session.registerOption(name, attr.prompt, sender);
    }
}

void Command::awaitOutsideClient()
{
    view::View::getInstance().awaitDueToOutput();
}

void Command::awakenInsideClient()
{
    view::View::getInstance().awakenDueToOutput();
}

void Command::validateDependenciesVersion() const
{
    using utility::common::allStrEqual;
    if (!allStrEqual(
            mainCLI.version().data(),
            utility::argument::version(),
            utility::common::version(),
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
    std::string banner{};
    banner += R"(  ______   ______     ______    \n)";
    banner += R"( /\  ___\ /\  __ \   /\  __ \   \n)";
    banner += R"( \ \  __\ \ \ \/\ \  \ \ \/\ \  \n)";
    banner += R"(  \ \_\    \ \_____\  \ \_____\ \n)";
    banner += R"(   \/_/     \/_____/   \/_____/ \n)";

    return banner;
}
} // namespace application::command
