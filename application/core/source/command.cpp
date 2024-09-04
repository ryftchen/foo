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
#include <thread>
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
enum class ExtEvent : std::uint8_t
{
    //! @brief Start.
    start,
    //! @brief Stop.
    stop,
    //! @brief Reset.
    reset
};

//! @brief Awaitable coroutine.
class Awaitable
{
public:
    // NOLINTBEGIN (readability-identifier-naming)
    //! @brief Promise type for use in coroutines.
    struct promise_type
    {
        //! @brief Get the return object for the coroutine.
        //! @return awaitable instance
        Awaitable get_return_object() { return Awaitable{std::coroutine_handle<promise_type>::from_promise(*this)}; }
        //! @brief Initial suspend point of the coroutine.
        //! @return suspend_never object indicating that the coroutine should not be suspended initially
        std::suspend_never initial_suspend() noexcept { return {}; }
        //! @brief Final suspend point of the coroutine.
        //! @return suspend_always object indicating that the coroutine should be suspended finally
        std::suspend_always final_suspend() noexcept { return {}; }
        //! @brief Complete the coroutine without returning a value.
        void return_void() noexcept {}
        //! @brief Handle exceptions thrown within the coroutine.
        void unhandled_exception() { std::rethrow_exception(std::current_exception()); }
    };
    // NOLINTEND (readability-identifier-naming)

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
    void resume()
    {
        if (handle)
        {
            handle.resume();
        }
    }
    //! @brief Check if the coroutine has completed.
    //! @return be done or not
    [[nodiscard]] bool done() const { return handle ? handle.done() : true; }

private:
    //! @brief Coroutine handle.
    std::coroutine_handle<promise_type> handle{};
};
} // namespace

//! @brief Trigger the external helper with event.
//! @tparam Helper - target helper
//! @param event - target event
template <HelperType Helper>
static void triggerHelper(const ExtEvent event)
{
    if (!config::activateHelper())
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
static void helperDaemon()
{
    using log::Log, view::View;

    constexpr std::uint8_t helperNum = 2;
    utility::thread::Thread extensionThd(helperNum);
    extensionThd.enqueue("logger", &Log::stateController, &Log::getInstance());
    extensionThd.enqueue("viewer", &View::stateController, &View::getInstance());
}

//! @brief Coroutine for managing the lifecycle of helper components.
//! @return object that represents the execution of the coroutine
static Awaitable helperLifecycle()
{
    if (!config::activateHelper())
    {
        co_return;
    }

    std::latch awaitLatch(1);
    const std::jthread daemon(
        [&awaitLatch]()
        {
            helperDaemon();
            awaitLatch.count_down();
        });
    constexpr std::uint8_t helperNum = 2;
    std::barrier awaitBarrier(helperNum + 1);
    const auto asyncLauncher = [&awaitBarrier](const ExtEvent event)
    {
        const std::jthread send2Logger(
            [&awaitBarrier, event]()
            {
                triggerHelper<log::Log>(event);
                awaitBarrier.arrive_and_wait();
            }),
            send2Viewer(
                [&awaitBarrier, event]()
                {
                    triggerHelper<view::View>(event);
                    awaitBarrier.arrive_and_wait();
                });
        awaitBarrier.arrive_and_wait();
    };

    co_await std::suspend_always{};
    asyncLauncher(ExtEvent::start);
    co_await std::suspend_always{};
    asyncLauncher(ExtEvent::stop);
    awaitLatch.wait();
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
    auto launcher = helperLifecycle();
    launcher.resume();

    if (1 == argc)
    {
        enterConsoleMode();
    }
    else
    {
        constexpr std::uint8_t groundNum = 2;
        utility::thread::Thread handleThd(groundNum);
        handleThd.enqueue("commander@fg", &Command::foregroundHandler, this, argc, argv);
        handleThd.enqueue("commander@bg", &Command::backgroundHandler, this);
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
            [](const std::string& input)
            {
                if (std::none_of(
                        input.cbegin(),
                        input.cend(),
                        [l = std::locale{}](const auto c)
                        {
                            return !std::isspace(c, l);
                        }))
                {
                    throw std::invalid_argument("Invalid console command.");
                }
                return input;
            })
        .metavar("CMD")
        .help("run commands in console mode and exit\n"
              "separate with quotes");

    CategoryName category;
    ChoiceContainer choices;

    auto& algoTable = regularChoices[subCLIAppAlgo.title()];
    subCLIAppAlgo.addDescription(getDescr<app_algo::ApplyAlgorithm>());
    subCLIAppAlgo.addArgument("-h", "--help").argsNum(0).implicitVal(true).help("show help and exit");
    category = std::string{TypeInfo<app_algo::MatchMethod>::name};
    choices = extractChoices<app_algo::MatchMethod>();
    algoTable[category] = CategoryExtAttr{choices, app_algo::MatchMethod{}};
    subCLIAppAlgo
        .addArgument("-" + std::string{getAlias<app_algo::ApplyAlgorithm, app_algo::MatchMethod>()}, "--" + category)
        .argsNum(0, choices.size())
        .defaultVal<std::vector<std::string>>(std::move(choices))
        .remaining()
        .metavar("OPT")
        .help(getDescr<app_algo::MatchMethod>());
    messageForwarder.registerHandler(
        [](const action::UpdateChoice<app_algo::MatchMethod>& msg)
        {
            app_algo::updateChoice<app_algo::MatchMethod>(msg.cho);
        });
    messageForwarder.registerHandler(
        [](const action::RunChoices<app_algo::MatchMethod>& msg)
        {
            app_algo::runChoices<app_algo::MatchMethod>(msg.coll);
        });
    category = std::string{TypeInfo<app_algo::NotationMethod>::name};
    choices = extractChoices<app_algo::NotationMethod>();
    algoTable[category] = CategoryExtAttr{choices, app_algo::NotationMethod{}};
    subCLIAppAlgo
        .addArgument("-" + std::string{getAlias<app_algo::ApplyAlgorithm, app_algo::NotationMethod>()}, "--" + category)
        .argsNum(0, choices.size())
        .defaultVal<std::vector<std::string>>(std::move(choices))
        .remaining()
        .metavar("OPT")
        .help(getDescr<app_algo::NotationMethod>());
    messageForwarder.registerHandler(
        [](const action::UpdateChoice<app_algo::NotationMethod>& msg)
        {
            app_algo::updateChoice<app_algo::NotationMethod>(msg.cho);
        });
    messageForwarder.registerHandler(
        [](const action::RunChoices<app_algo::NotationMethod>& msg)
        {
            app_algo::runChoices<app_algo::NotationMethod>(msg.coll);
        });
    category = std::string{TypeInfo<app_algo::OptimalMethod>::name};
    choices = extractChoices<app_algo::OptimalMethod>();
    algoTable[category] = CategoryExtAttr{choices, app_algo::OptimalMethod{}};
    subCLIAppAlgo
        .addArgument("-" + std::string{getAlias<app_algo::ApplyAlgorithm, app_algo::OptimalMethod>()}, "--" + category)
        .argsNum(0, choices.size())
        .defaultVal<std::vector<std::string>>(std::move(choices))
        .remaining()
        .metavar("OPT")
        .help(getDescr<app_algo::OptimalMethod>());
    messageForwarder.registerHandler(
        [](const action::UpdateChoice<app_algo::OptimalMethod>& msg)
        {
            app_algo::updateChoice<app_algo::OptimalMethod>(msg.cho);
        });
    messageForwarder.registerHandler(
        [](const action::RunChoices<app_algo::OptimalMethod>& msg)
        {
            app_algo::runChoices<app_algo::OptimalMethod>(msg.coll);
        });
    category = std::string{TypeInfo<app_algo::SearchMethod>::name};
    choices = extractChoices<app_algo::SearchMethod>();
    algoTable[category] = CategoryExtAttr{choices, app_algo::SearchMethod{}};
    subCLIAppAlgo
        .addArgument("-" + std::string{getAlias<app_algo::ApplyAlgorithm, app_algo::SearchMethod>()}, "--" + category)
        .argsNum(0, choices.size())
        .defaultVal<std::vector<std::string>>(std::move(choices))
        .remaining()
        .metavar("OPT")
        .help(getDescr<app_algo::SearchMethod>());
    messageForwarder.registerHandler(
        [](const action::UpdateChoice<app_algo::SearchMethod>& msg)
        {
            app_algo::updateChoice<app_algo::SearchMethod>(msg.cho);
        });
    messageForwarder.registerHandler(
        [](const action::RunChoices<app_algo::SearchMethod>& msg)
        {
            app_algo::runChoices<app_algo::SearchMethod>(msg.coll);
        });
    category = std::string{TypeInfo<app_algo::SortMethod>::name};
    choices = extractChoices<app_algo::SortMethod>();
    algoTable[category] = CategoryExtAttr{choices, app_algo::SortMethod{}};
    subCLIAppAlgo
        .addArgument("-" + std::string{getAlias<app_algo::ApplyAlgorithm, app_algo::SortMethod>()}, "--" + category)
        .argsNum(0, choices.size())
        .defaultVal<std::vector<std::string>>(std::move(choices))
        .remaining()
        .metavar("OPT")
        .help(getDescr<app_algo::SortMethod>());
    messageForwarder.registerHandler(
        [](const action::UpdateChoice<app_algo::SortMethod>& msg)
        {
            app_algo::updateChoice<app_algo::SortMethod>(msg.cho);
        });
    messageForwarder.registerHandler(
        [](const action::RunChoices<app_algo::SortMethod>& msg)
        {
            app_algo::runChoices<app_algo::SortMethod>(msg.coll);
        });
    mainCLI.addSubParser(subCLIAppAlgo);

    auto& dpTable = regularChoices[subCLIAppDp.title()];
    subCLIAppDp.addDescription(getDescr<app_dp::ApplyDesignPattern>());
    subCLIAppDp.addArgument("-h", "--help").argsNum(0).implicitVal(true).help("show help and exit");
    category = std::string{TypeInfo<app_dp::BehavioralInstance>::name};
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
    messageForwarder.registerHandler(
        [](const action::UpdateChoice<app_dp::BehavioralInstance>& msg)
        {
            app_dp::updateChoice<app_dp::BehavioralInstance>(msg.cho);
        });
    messageForwarder.registerHandler(
        [](const action::RunChoices<app_dp::BehavioralInstance>& msg)
        {
            app_dp::runChoices<app_dp::BehavioralInstance>(msg.coll);
        });
    category = std::string{TypeInfo<app_dp::CreationalInstance>::name};
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
    messageForwarder.registerHandler(
        [](const action::UpdateChoice<app_dp::CreationalInstance>& msg)
        {
            app_dp::updateChoice<app_dp::CreationalInstance>(msg.cho);
        });
    messageForwarder.registerHandler(
        [](const action::RunChoices<app_dp::CreationalInstance>& msg)
        {
            app_dp::runChoices<app_dp::CreationalInstance>(msg.coll);
        });
    category = std::string{TypeInfo<app_dp::StructuralInstance>::name};
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
    messageForwarder.registerHandler(
        [](const action::UpdateChoice<app_dp::StructuralInstance>& msg)
        {
            app_dp::updateChoice<app_dp::StructuralInstance>(msg.cho);
        });
    messageForwarder.registerHandler(
        [](const action::RunChoices<app_dp::StructuralInstance>& msg)
        {
            app_dp::runChoices<app_dp::StructuralInstance>(msg.coll);
        });
    mainCLI.addSubParser(subCLIAppDp);

    auto& dsTable = regularChoices[subCLIAppDs.title()];
    subCLIAppDs.addDescription(getDescr<app_ds::ApplyDataStructure>());
    subCLIAppDs.addArgument("-h", "--help").argsNum(0).implicitVal(true).help("show help and exit");
    category = std::string{TypeInfo<app_ds::LinearInstance>::name};
    choices = extractChoices<app_ds::LinearInstance>();
    dsTable[category] = CategoryExtAttr{choices, app_ds::LinearInstance{}};
    subCLIAppDs
        .addArgument("-" + std::string{getAlias<app_ds::ApplyDataStructure, app_ds::LinearInstance>()}, "--" + category)
        .argsNum(0, choices.size())
        .defaultVal<std::vector<std::string>>(std::move(choices))
        .remaining()
        .metavar("OPT")
        .help(getDescr<app_ds::LinearInstance>());
    messageForwarder.registerHandler(
        [](const action::UpdateChoice<app_ds::LinearInstance>& msg)
        {
            app_ds::updateChoice<app_ds::LinearInstance>(msg.cho);
        });
    messageForwarder.registerHandler(
        [](const action::RunChoices<app_ds::LinearInstance>& msg)
        {
            app_ds::runChoices<app_ds::LinearInstance>(msg.coll);
        });
    category = std::string{TypeInfo<app_ds::TreeInstance>::name};
    choices = extractChoices<app_ds::TreeInstance>();
    dsTable[category] = CategoryExtAttr{choices, app_ds::TreeInstance{}};
    subCLIAppDs
        .addArgument("-" + std::string{getAlias<app_ds::ApplyDataStructure, app_ds::TreeInstance>()}, "--" + category)
        .argsNum(0, choices.size())
        .defaultVal<std::vector<std::string>>(std::move(choices))
        .remaining()
        .metavar("OPT")
        .help(getDescr<app_ds::TreeInstance>());
    messageForwarder.registerHandler(
        [](const action::UpdateChoice<app_ds::TreeInstance>& msg)
        {
            app_ds::updateChoice<app_ds::TreeInstance>(msg.cho);
        });
    messageForwarder.registerHandler(
        [](const action::RunChoices<app_ds::TreeInstance>& msg)
        {
            app_ds::runChoices<app_ds::TreeInstance>(msg.coll);
        });
    mainCLI.addSubParser(subCLIAppDs);

    auto& numTable = regularChoices[subCLIAppNum.title()];
    subCLIAppNum.addDescription(getDescr<app_num::ApplyNumeric>());
    subCLIAppNum.addArgument("-h", "--help").argsNum(0).implicitVal(true).help("show help and exit");
    category = std::string{TypeInfo<app_num::ArithmeticMethod>::name};
    choices = extractChoices<app_num::ArithmeticMethod>();
    numTable[category] = CategoryExtAttr{choices, app_num::ArithmeticMethod{}};
    subCLIAppNum
        .addArgument("-" + std::string{getAlias<app_num::ApplyNumeric, app_num::ArithmeticMethod>()}, "--" + category)
        .argsNum(0, choices.size())
        .defaultVal<std::vector<std::string>>(std::move(choices))
        .remaining()
        .metavar("OPT")
        .help(getDescr<app_num::ArithmeticMethod>());
    messageForwarder.registerHandler(
        [](const action::UpdateChoice<app_num::ArithmeticMethod>& msg)
        {
            app_num::updateChoice<app_num::ArithmeticMethod>(msg.cho);
        });
    messageForwarder.registerHandler(
        [](const action::RunChoices<app_num::ArithmeticMethod>& msg)
        {
            app_num::runChoices<app_num::ArithmeticMethod>(msg.coll);
        });
    category = std::string{TypeInfo<app_num::DivisorMethod>::name};
    choices = extractChoices<app_num::DivisorMethod>();
    numTable[category] = CategoryExtAttr{choices, app_num::DivisorMethod{}};
    subCLIAppNum
        .addArgument("-" + std::string{getAlias<app_num::ApplyNumeric, app_num::DivisorMethod>()}, "--" + category)
        .argsNum(0, choices.size())
        .defaultVal<std::vector<std::string>>(std::move(choices))
        .remaining()
        .metavar("OPT")
        .help(getDescr<app_num::DivisorMethod>());
    messageForwarder.registerHandler(
        [](const action::UpdateChoice<app_num::DivisorMethod>& msg)
        {
            app_num::updateChoice<app_num::DivisorMethod>(msg.cho);
        });
    messageForwarder.registerHandler(
        [](const action::RunChoices<app_num::DivisorMethod>& msg)
        {
            app_num::runChoices<app_num::DivisorMethod>(msg.coll);
        });
    category = std::string{TypeInfo<app_num::IntegralMethod>::name};
    choices = extractChoices<app_num::IntegralMethod>();
    numTable[category] = CategoryExtAttr{choices, app_num::IntegralMethod{}};
    subCLIAppNum
        .addArgument("-" + std::string{getAlias<app_num::ApplyNumeric, app_num::IntegralMethod>()}, "--" + category)
        .argsNum(0, choices.size())
        .defaultVal<std::vector<std::string>>(std::move(choices))
        .remaining()
        .metavar("OPT")
        .help(getDescr<app_num::IntegralMethod>());
    messageForwarder.registerHandler(
        [](const action::UpdateChoice<app_num::IntegralMethod>& msg)
        {
            app_num::updateChoice<app_num::IntegralMethod>(msg.cho);
        });
    messageForwarder.registerHandler(
        [](const action::RunChoices<app_num::IntegralMethod>& msg)
        {
            app_num::runChoices<app_num::IntegralMethod>(msg.coll);
        });
    category = std::string{TypeInfo<app_num::PrimeMethod>::name};
    choices = extractChoices<app_num::PrimeMethod>();
    numTable[category] = CategoryExtAttr{choices, app_num::PrimeMethod{}};
    subCLIAppNum
        .addArgument("-" + std::string{getAlias<app_num::ApplyNumeric, app_num::PrimeMethod>()}, "--" + category)
        .argsNum(0, choices.size())
        .defaultVal<std::vector<std::string>>(std::move(choices))
        .remaining()
        .metavar("OPT")
        .help(getDescr<app_num::PrimeMethod>());
    messageForwarder.registerHandler(
        [](const action::UpdateChoice<app_num::PrimeMethod>& msg)
        {
            app_num::updateChoice<app_num::PrimeMethod>(msg.cho);
        });
    messageForwarder.registerHandler(
        [](const action::RunChoices<app_num::PrimeMethod>& msg)
        {
            app_num::runChoices<app_num::PrimeMethod>(msg.coll);
        });
    mainCLI.addSubParser(subCLIAppNum);
}

void Command::foregroundHandler(const int argc, const char* const argv[])
try
{
    std::unique_lock<std::mutex> parserLock(parserMtx);
    mainCLI.parseArgs(argc, argv);
    validate();

    isParsed.store(true);
    parserLock.unlock();
    parserCv.notify_one();
}
catch (const std::exception& err)
{
    isParsed.store(true);
    parserCv.notify_one();
    LOG_WRN << err.what();
}

void Command::backgroundHandler()
try
{
    if (std::unique_lock<std::mutex> parserLock(parserMtx); true)
    {
        parserCv.wait(
            parserLock,
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
    for ([[maybe_unused]] const auto& [subCLIName, categoryMap] : regularChoices | std::views::filter(isSubCLIUsed))
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
        for ([[maybe_unused]] const auto& [categoryName, categoryAttr] :
             categoryMap | std::views::filter(isCategoryUsed))
        {
            const auto& pendingTasks = subCLI.get<std::vector<std::string>>(categoryName);
            for (const auto& target : pendingTasks)
            {
                std::visit(
                    action::EvtTypeOverloaded{[this, target](auto&& event)
                                              {
                                                  using EventType = std::decay_t<decltype(event)>;
                                                  messageForwarder.onMessage(action::UpdateChoice<EventType>{target});
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
                action::EvtTypeOverloaded{[this, candidates](auto&& event)
                                          {
                                              using EventType = std::decay_t<decltype(event)>;
                                              messageForwarder.onMessage(action::RunChoices<EventType>{candidates});
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
            if ((0 != length) && view::tryParseTLVPacket(buffer, length).stopTag)
            {
                client->asyncExit();
            }
        }
        catch (const std::exception& err)
        {
            LOG_WRN << err.what();
        }
        view::tryAwakenDueToOutput();
    };
    client->toConnect(view::currentViewerTCPHost(), view::currentViewerTCPPort());
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
            if ((0 != length) && view::tryParseTLVPacket(buffer, length).stopTag)
            {
                client->asyncExit();
            }
        }
        catch (const std::exception& err)
        {
            LOG_WRN << err.what();
        }
        view::tryAwakenDueToOutput();
    };
    client->toReceive();
    client->toConnect(view::currentViewerUDPHost(), view::currentViewerUDPPort());
}

void Command::executeInConsole() const
{
    if (!config::activateHelper())
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
    utility::time::millisecondLevelSleep(latency);
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
    if (!config::activateHelper())
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

    for (const auto& [optionName, optionAttr] : view::currentViewerOptions())
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
                    view::tryAwaitDueToOutput();
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
