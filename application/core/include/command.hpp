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
enum Category : std::uint8_t
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
    //! @brief Destroy the Notifier object.
    virtual ~Notifier() = default;

    //! @brief The base action when notified.
    class ActionBase
    {
    public:
        //! @brief Destroy the ActionBase object.
        virtual ~ActionBase() = default;

        //! @brief Perform the specific operation.
        virtual void execute() const = 0;
    };
    //! @brief The action when notified.
    //! @tparam CRTP - type of derived class for CRTP
    template <typename CRTP>
    class Action : public ActionBase
    {
    public:
        //! @brief Perform the specific operation.
        void execute() const override { static_cast<const CRTP&>(*this).execute(); }
    };
    //! @brief The handler that performs an action when notified.
    //! @tparam key - specific key
    template <Key key>
    class Handler : public Action<Handler<key>>
    {
    public:
        //! @brief Construct a new Handler object.
        //! @param subject - involved subject
        explicit Handler(const Subject& subject) : subject{subject} {}
        //! @brief Construct a new Handler object.
        Handler() = delete;

        //! @brief Perform the specific operation.
        void execute() const override;

    private:
        //! @brief The involved subject.
        const Subject& subject{};
    };

    //! @brief Attach a handler with a specific key to the notifier.
    //! @param key - specific key
    //! @param handler - handler to be attached
    void attach(const Key key, std::shared_ptr<ActionBase> handler) { handlers[key] = std::move(handler); }
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
    std::map<Key, std::shared_ptr<ActionBase>> handlers{};
};

//! @brief Execute the command line.
class Command final
{
public:
    //! @brief Destroy the Command object.
    virtual ~Command();
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

    //! @brief Get the Command instance.
    //! @return reference of the Command object
    static Command& getInstance();
    //! @brief Instance title.
    static constexpr std::string title{"commander"};
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
    std::atomic<bool> isParsed{false};
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
    //! @brief Flag to indicate whether the command is faulty.
    std::atomic<bool> isFaulty{false};

    //! @brief Initialize the parse argument helpers for native.
    void initializeNativeCLI();
    //! @brief Initialize the parse argument helpers for extra.
    void initializeExtraCLI();
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
    void checkForExcessiveArguments();

    //! @brief Get the description.
    //! @param cat - specific value of Category enum
    //! @return description
    static consteval std::string_view getDescr(const Category cat);
    //! @brief Get the alias name.
    //! @param cat - specific value of Category enum
    //! @return alias name
    static consteval std::string_view getAlias(const Category cat);
    //! @brief Extract all choices in the sub-cli's category.
    //! @tparam T - type of sub-cli's category
    //! @return all choices
    template <typename T>
    static std::vector<std::string> extractChoices();

    //! @brief Manage tasks.
    class TaskManager
    {
    public:
        //! @brief Destroy the TaskManager object.
        virtual ~TaskManager() = default;

        //! @brief Check whether any tasks do not exist.
        //! @return any tasks do not exist or exist
        [[nodiscard]] virtual inline bool empty() const = 0;
        //! @brief Reset bit flags that manage all tasks.
        virtual inline void reset() = 0;
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
        //! @brief Alias for the trait of the registered sub-cli's category.
        struct Trait
        {
            //! @brief The candidates for the choice.
            const std::vector<std::string> choices;
            //! @brief The internal event for applying.
            const action::EventType event;
        };
        //! @brief Alias for the map of sub-cli's category name and Trait.
        using CategoryMap = std::map<std::string, Trait>;
        //! @brief Mapping table of all extra choices. Fill as needed.
        std::map<std::string, CategoryMap> extraChoiceRegistry;

        //! @brief Wrap interfaces to check for existing and reset extra choices.
        struct Intf
        {
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
    using RelVerPair = std::pair<std::string, std::string>;
    //! @brief Mapping hash value for the related versions.
    struct RelVerHash
    {
        //! @brief The operator (()) overloading of RelVerHash class.
        //! @param pair - pair of the sub-cli name and the sub-cli version
        //! @return hash value
        std::size_t operator()(const RelVerPair& pair) const
        {
            const std::size_t hash1 = std::hash<std::string>()(pair.first),
                              hash2 = std::hash<std::string>()(pair.second);
            constexpr std::size_t magicNumber = 0x9e3779b9, leftShift = 6, rightShift = 2;
            std::size_t seed = 0;
            seed ^= hash1 + magicNumber + (seed << leftShift) + (seed >> rightShift);
            seed ^= hash2 + magicNumber + (seed << leftShift) + (seed >> rightShift);

            return seed;
        }
    };
    //! @brief Mapping table of related versions. Fill as needed.
    std::unordered_multimap<RelVerPair, std::string, RelVerHash> relatedVersions;

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
    //! @brief Parse the message inside the client in console mode.
    //! @param buffer - message buffer
    //! @param length - message length
    //! @return need to stop the client connection or not
    static bool onParsing4Client(char* buffer, const int length);
    //! @brief Await outside the client in console mode.
    static void enableWait4Client();
    //! @brief Awaken inside the client in console mode.
    static void disableWait4Client();
    //! @brief Build the exit request message in console mode.
    //! @return exit request message
    static std::string buildExitRequest4Client();
    //! @brief Console latency in the millisecond range.
    static void interactionLatency();
    //! @brief Validate dependencies version.
    void validateDependenciesVersion() const;
};
} // namespace command
} // namespace application
