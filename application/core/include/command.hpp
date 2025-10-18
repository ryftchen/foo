//! @file command.hpp
//! @author ryftchen
//! @brief The declarations (command) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include "action.hpp"
#include "console.hpp"
#include "note.hpp"

#include "utility/include/argument.hpp"

//! @brief The application module.
namespace application // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Command-line-related functions in the application module.
namespace command
{
//! @brief Represent the maximum value of an enum.
//! @tparam T - type of specific enum
template <typename T>
struct Bottom;

//! @brief Enumerate specific native categories.
enum class Category : std::uint8_t
{
    //! @brief Console.
    console,
    //! @brief Dump.
    dump,
    //! @brief Help.
    help,
    //! @brief Version.
    version
};
//! @brief Store the maximum value of the Category enum.
template <>
struct Bottom<Category>
{
    //! @brief Maximum value of the Category enum.
    static constexpr std::uint8_t value{4};
};

//! @brief Gather and notify handlers.
//! @tparam Key - type of key
//! @tparam Subject - type of subject
template <typename Key, typename Subject>
class Notifier
{
public:
    //! @brief The base routine when notified.
    class RoutineBase
    {
    public:
        //! @brief Construct a new RoutineBase object.
        RoutineBase() = default;
        //! @brief Destroy the RoutineBase object.
        virtual ~RoutineBase() = default;
        //! @brief Construct a new RoutineBase object.
        RoutineBase(const RoutineBase&) = default;
        //! @brief Construct a new RoutineBase object.
        RoutineBase(RoutineBase&&) noexcept = default;
        //! @brief The operator (=) overloading of RoutineBase class.
        //! @return reference of the RoutineBase object
        RoutineBase& operator=(const RoutineBase&) = default;
        //! @brief The operator (=) overloading of RoutineBase class.
        //! @return reference of the RoutineBase object
        RoutineBase& operator=(RoutineBase&&) noexcept = default;

        //! @brief Perform the specific operation.
        virtual void execute() const = 0;
    };
    //! @brief The routine when notified.
    //! @tparam CRTP - type of derived class for CRTP
    template <typename CRTP>
    class Routine : public RoutineBase
    {
    public:
        //! @brief Perform the specific operation.
        void execute() const override { static_cast<const CRTP&>(*this).execute(); }
    };
    //! @brief The handler used to trigger a routine when notified.
    //! @tparam key - specific key
    template <Key key>
    class Handler : public Routine<Handler<key>>
    {
    public:
        //! @brief Construct a new Handler object.
        //! @param subject - involved subject
        explicit Handler(const Subject& subject) : subject{subject} {}

        //! @brief Perform the specific operation.
        void execute() const override;

    private:
        //! @brief The involved subject.
        const Subject& subject{};
    };

    //! @brief Attach a handler with a specific key to the notifier.
    //! @param key - specific key
    //! @param handler - handler to be attached
    void attach(const Key key, std::shared_ptr<RoutineBase> handler) { handlers[key] = std::move(handler); }
    //! @brief Notify the handler associated with the given key.
    //! @param key - specific key
    void notify(const Key key) const
    {
        if (handlers.contains(key))
        {
            handlers.at(key)->execute();
        }
    }

private:
    //! @brief Map of handlers identified by a key.
    std::map<Key, std::shared_ptr<RoutineBase>> handlers{};
};

//! @brief Execute the command line.
class Command final
{
public:
    //! @brief Destroy the Command object.
    ~Command();
    //! @brief Construct a new Command object.
    Command(const Command&) = delete;
    //! @brief Construct a new Command object.
    Command(Command&&) = delete;
    //! @brief The operator (=) overloading of Command class.
    //! @return reference of the Command object
    Command& operator=(const Command&) = delete;
    //! @brief The operator (=) overloading of Command class.
    //! @return reference of the Command object
    Command& operator=(Command&&) = delete;

    //! @brief Instance title.
    static constexpr std::string title{"commander"};
    //! @brief Get the Command instance.
    //! @return reference of the Command object
    static Command& getInstance();
    //! @brief Interface used to execute.
    //! @param argc - argument count
    //! @param argv - argument vector
    //! @return successful or failed to execute
    bool execute(const int argc, const char* const argv[]);

private:
    //! @brief Construct a new Command object.
    Command();

    //! @brief Mutex for controlling parser.
    mutable std::mutex parserMtx;
    //! @brief The synchronization condition for foreground and background. Use with parserMtx.
    std::condition_variable parserCond;
    //! @brief Flag to indicate whether parsing of arguments is completed.
    std::atomic_bool isParsed{false};
    //! @brief Parse argument helper for commander.
    utility::argument::Argument mainCLI{"foo", note::version()};
    //! @brief Parse argument helper to apply algorithm.
    utility::argument::Argument subCLIAppAlgo{action::name<reg_algo::ApplyAlgorithm>(), reg_algo::version()};
    //! @brief Parse argument helper to apply design pattern.
    utility::argument::Argument subCLIAppDp{action::name<reg_dp::ApplyDesignPattern>(), reg_dp::version()};
    //! @brief Parse argument helper to apply data structure.
    utility::argument::Argument subCLIAppDs{action::name<reg_ds::ApplyDataStructure>(), reg_ds::version()};
    //! @brief Parse argument helper to apply numeric.
    utility::argument::Argument subCLIAppNum{action::name<reg_num::ApplyNumeric>(), reg_num::version()};
    //! @brief The short prefix for the option.
    const std::string shortPrefix{"-"};
    //! @brief The Long prefix for the option.
    const std::string longPrefix{"--"};
    //! @brief The meta variable for the option.
    static constexpr std::string_view metaVar{"OPT"};
    //! @brief Flag to indicate whether the command is faulty.
    std::atomic_bool isFaulty{false};

    //! @brief Initialize the parse argument helpers for native.
    void initializeNativeCLI();
    //! @brief Initialize the parse argument helpers for extra.
    void initializeExtraCLI();
    //! @brief Setup the main command line interface.
    void setupMainCLI();
    //! @brief Setup the sub-command line interface.
    //! @tparam T - type of type of sub-cli
    template <typename T>
    void setupSubCLI();
    //! @brief Front-end handler for parsing command line arguments.
    //! @param argc - argument count
    //! @param argv - argument vector
    void frontEndHandler(const int argc, const char* const argv[]);
    //! @brief Back-end handler for performing the specific tasks.
    void backEndHandler();
    //! @brief Precheck the native type or extra type task.
    void precheck();
    //! @brief Check whether any type tasks exist.
    //! @return any type tasks exist or do not exist
    bool anySelected() const;
    //! @brief Dispatch specific tasks.
    void dispatch();
    //! @brief Execute the command line of console mode.
    void executeInConsole() const;
    //! @brief Show help message.
    void showHelpMessage() const;
    //! @brief Dump configuration.
    static void dumpConfiguration();
    //! @brief Display version information.
    void displayVersionInfo() const;
    //! @brief Check for excessive arguments.
    void checkExcessArgs();

    //! @brief Map the alias name.
    //! @param cat - native category
    //! @return alias name
    static consteval std::string_view mappedAlias(const Category cat);
    //! @brief Map the description.
    //! @param cat - native category
    //! @return description
    static consteval std::string_view mappedDescr(const Category cat);
    //! @brief Extract all choices in the sub-cli's category.
    //! @tparam T - type of sub-cli's category
    //! @return all choices
    template <typename T>
    static std::vector<std::string> extractChoices();

    //! @brief Manage tasks.
    class TaskManager
    {
    public:
        //! @brief Construct a new TaskManager object.
        TaskManager() = default;
        //! @brief Destroy the TaskManager object.
        virtual ~TaskManager() = default;
        //! @brief Construct a new TaskManager object.
        TaskManager(const TaskManager&) = default;
        //! @brief Construct a new TaskManager object.
        TaskManager(TaskManager&&) noexcept = default;
        //! @brief The operator (=) overloading of TaskManager class.
        //! @return reference of the TaskManager object
        TaskManager& operator=(const TaskManager&) = default;
        //! @brief The operator (=) overloading of TaskManager class.
        //! @return reference of the TaskManager object
        TaskManager& operator=(TaskManager&&) noexcept = default;

        //! @brief Check whether any tasks do not exist.
        //! @return any tasks do not exist or exist
        [[nodiscard]] virtual bool empty() const = 0;
        //! @brief Reset bit flags that manage all tasks.
        virtual void reset() = 0;
    };
    //! @brief Manage native categories.
    class NativeManager : virtual public TaskManager
    {
    public:
        //! @brief Bit flags for managing native categories.
        std::bitset<Bottom<Category>::value> nativeCategories;

        //! @brief Check whether any native categories do not exist.
        //! @return any native categories do not exist or exist
        [[nodiscard]] bool empty() const override { return nativeCategories.none(); }
        //! @brief Reset bit flags that manage native categories.
        void reset() override { nativeCategories.reset(); }
    };
    //! @brief Manage extra choices of sub-cli.
    class ExtraManager : virtual public TaskManager
    {
    public:
        //! @brief Alias for the attribute of the registered sub-cli's category.
        struct Attr
        {
            //! @brief The candidates for the choice.
            const std::vector<std::string> choices;
            //! @brief The internal event for applying.
            const action::EventType event;
        };
        //! @brief Alias for the map of sub-cli's category name and Attr.
        using CategoryMap = std::map<std::string, Attr>;
        //! @brief Mapping table of all extra choices. Fill as needed.
        std::map<std::string, CategoryMap> extraChoiceRegistry;

        //! @brief Wrap interfaces to check for existing and reset extra choices.
        struct Intf
        {
            Intf(std::function<bool()> presentCb, std::function<void()> clearCb) :
                present{std::move(presentCb)}, clear{std::move(clearCb)}
            {
                if (!present || !clear)
                {
                    throw std::runtime_error{"Invalid sub-command interfaces are being used."};
                }
            }

            //! @brief Check the existence status of the extra choice.
            const std::function<bool()> present;
            //! @brief Reset control of the extra choice.
            const std::function<void()> clear;
        };
        //! @brief Existence status and reset control of the sub-cli to which the extra choices belong.
        std::map<std::string, Intf> extraChecklist;
        //! @brief Flag for help only.
        bool extraHelpOnly{false};

        //! @brief Check whether any extra choices do not exist.
        //! @return any extra choices do not exist or exist
        [[nodiscard]] bool empty() const override
        {
            return !extraHelpOnly
                && std::none_of(
                    extraChecklist.cbegin(),
                    extraChecklist.cend(),
                    [](const auto& pair) { return pair.second.present(); });
        }
        //! @brief Reset bit flags that manage extra choices.
        void reset() override
        {
            extraHelpOnly = false;
            std::for_each(
                extraChecklist.cbegin(), extraChecklist.cend(), [](const auto& pair) { pair.second.clear(); });
        }
    };
    //! @brief Schedule all managed tasks.
    class TaskDispatcher : public NativeManager, public ExtraManager
    {
    public:
        //! @brief Check whether any tasks do not exist.
        //! @return any tasks do not exist or exist
        [[nodiscard]] bool empty() const final { return NativeManager::empty() && ExtraManager::empty(); }
        //! @brief Reset bit flags that manage all tasks.
        void reset() final
        {
            NativeManager::reset();
            ExtraManager::reset();
        }
    } /** @brief Dispatch all types of tasks. */ taskDispatcher{};

    friend class Notifier<Category, Command>;
    //! @brief Alias for the local notifier.
    using LocalNotifier = Notifier<Category, Command>;
    //! @brief Local notification for native type tasks.
    LocalNotifier builtInNotifier{};
    //! @brief Forward messages for extra type tasks.
    action::MessageForwarder applyingForwarder{};
    //! @brief Alias for the pair of the sub-cli name and the sub-cli version.
    using VerLinkKey = std::pair<std::string, std::string>;
    //! @brief Mapping hash value for the related versions.
    struct VerLinkHash
    {
        //! @brief The operator (()) overloading of VerLinkHash struct.
        //! @param key - pair of the sub-cli name and the sub-cli version
        //! @return hash value
        std::size_t operator()(const VerLinkKey& key) const
        {
            constexpr std::size_t magicNumber = 0x9E3779B9;
            constexpr std::size_t leftShift = 6;
            constexpr std::size_t rightShift = 2;
            const std::size_t hash1 = std::hash<std::string>()(key.first);
            const std::size_t hash2 = std::hash<std::string>()(key.second);
            std::size_t seed = 0;
            seed ^= hash1 + magicNumber + (seed << leftShift) + (seed >> rightShift);
            seed ^= hash2 + magicNumber + (seed << leftShift) + (seed >> rightShift);

            return seed;
        }
    };
    //! @brief Mapping table of related versions. Fill as needed.
    std::unordered_multimap<VerLinkKey, std::string, VerLinkHash> versionLinks;

    //! @brief Go to console mode for troubleshooting.
    static void enterConsoleMode();
    //! @brief Register the command line to console mode.
    //! @tparam T - type of client
    //! @param session - console to be registered
    //! @param client - client used to send
    template <typename T>
    static void registerOnConsole(console::Console& session, std::shared_ptr<T>& client);
    //! @brief Launch the client for console mode.
    //! @tparam T - type of client
    //! @param client - client to be launched
    template <typename T>
    static void launchClient(std::shared_ptr<T>& client);
    //! @brief Process the inputs  in console mode.
    //! @param handling - handling for inputs
    //! @return console return code
    static auto processConsoleInputs(const std::function<void()>& handling);
    //! @brief Parse the message inside the client in console mode.
    //! @param buffer - message buffer
    //! @param length - message length
    //! @return need to continue parsing or not
    static bool onParsing4Client(char* buffer, const int length);
    //! @brief Await outside the client in console mode.
    static void waitClientOutputDone();
    //! @brief Awaken inside the client in console mode.
    static void notifyClientOutputDone();
    //! @brief Build the exit request message in console mode.
    //! @return exit request message
    static std::string buildDisconnectReq();
    //! @brief Console latency in the millisecond range.
    static void interactionLatency();
    //! @brief Validate dependencies version.
    void validateDependenciesVersion() const;
};
} // namespace command
} // namespace application
