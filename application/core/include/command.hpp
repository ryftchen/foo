//! @file command.hpp
//! @author ryftchen
//! @brief The declarations (command) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#pragma once

#include "action.hpp"
#include "console.hpp"
#include "note.hpp"

#include "utility/include/argument.hpp"

//! @brief The application module.
namespace application // NOLINT (modernize-concat-nested-namespaces)
{
//! @brief Command-line-related functions in the application module.
namespace command
{
//! @brief Represent the maximum value of an enum.
//! @tparam T - type of specific enum
template <class T>
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
    //! @brief Execution manager for running commander.
    //! @param argc - argument count
    //! @param argv - argument vector
    void execManager(const int argc, const char* const argv[]);

private:
    //! @brief Construct a new Command object.
    Command();

    //! @brief Mutex for controlling parser.
    mutable std::mutex parserMtx{};
    //! @brief The synchronization condition for foreground and background. Use with parserMtx.
    std::condition_variable parserCond{};
    //! @brief Flag to indicate whether parsing of arguments is completed.
    std::atomic<bool> isParsed{false};
    //! @brief Parse argument helper for commander.
    utility::argument::Argument mainCLI{"foo", note::version()};
    //! @brief Parse argument helper to apply algorithm.
    utility::argument::Argument subCLIAppAlgo{
        utility::reflection::TypeInfo<app_algo::ApplyAlgorithm>::name, note::version()};
    //! @brief Parse argument helper to apply design pattern.
    utility::argument::Argument subCLIAppDp{
        utility::reflection::TypeInfo<app_dp::ApplyDesignPattern>::name, note::version()};
    //! @brief Parse argument helper to apply data structure.
    utility::argument::Argument subCLIAppDs{
        utility::reflection::TypeInfo<app_ds::ApplyDataStructure>::name, note::version()};
    //! @brief Parse argument helper to apply numeric.
    utility::argument::Argument subCLIAppNum{
        utility::reflection::TypeInfo<app_num::ApplyNumeric>::name, note::version()};

    //! @brief Initialize the parse argument helpers.
    void initializeCLI();
    //! @brief Foreground handler for parsing command line arguments.
    //! @param argc - argument count
    //! @param argv - argument vector
    void foregroundHandler(const int argc, const char* const argv[]);
    //! @brief Background handler for performing the specific tasks.
    void backgroundHandler();
    //! @brief Pre-check the native type or extra type task.
    void validate();
    //! @brief Check whether any type tasks exist.
    //! @return any type tasks exist or do not exist
    bool anySelected() const;
    //! @brief Dispatch specific tasks.
    void dispatch();
    //! @brief Execute the command line of console mode.
    void executeInConsole() const;
    //! @brief Print help message.
    void showHelpMessage() const;
    //! @brief Dump configuration.
    static void dumpConfiguration();
    //! @brief Print version icon.
    void showVersionIcon() const;
    //! @brief Check for excessive arguments.
    void checkForExcessiveArguments();

    //! @brief Alias for the sub-cli name.
    using SubCLIName = std::string;
    //! @brief Alias for the category name.
    using CategoryName = std::string;
    //! @brief Alias for the target choice.
    using ChoiceName = std::string;
    //! @brief Alias for the collection of candidates for ChoiceName.
    using ChoiceContainer = std::vector<ChoiceName>;
    //! @brief Alias for the extend attribute of the target category.
    struct CategoryExtAttr
    {
        //! @brief The candidates for the choice.
        ChoiceContainer choices{};
        //! @brief The internal event for applying.
        action::EventType event{};
    };
    //! @brief Alias for the map of CategoryName and CategoryExtAttr.
    using CategoryExtMap = std::map<CategoryName, CategoryExtAttr>;
    //! @brief Alias for the map of SubCLIName and CategoryExtMap.
    using ExtraChoiceMap = std::map<SubCLIName, CategoryExtMap>;
    //! @brief Get the description.
    //! @tparam T - type of sub-cli or category
    //! @return description
    template <class T>
    static inline constexpr std::string_view getDescr()
    {
        using TypeInfo = utility::reflection::TypeInfo<T>;

        return TypeInfo::attrs.find(REFLECTION_STR("descr")).value;
    }
    //! @brief Get the alias name.
    //! @tparam SubCLI - type of sub-cli
    //! @tparam Cat - type of category
    //! @return alias name
    template <class SubCLI, class Cat>
    static inline constexpr std::string_view getAlias()
    {
        using SubCLITypeInfo = utility::reflection::TypeInfo<SubCLI>;
        using CatTypeInfo = utility::reflection::TypeInfo<Cat>;

        return SubCLITypeInfo::fields.find(REFLECTION_STR(CatTypeInfo::name)).attrs.find(REFLECTION_STR("alias")).value;
    }
    //! @brief Extract all choices in the category.
    //! @tparam Cat - type of category
    //! @return all choices
    template <class Cat>
    static ChoiceContainer extractChoices();
    //! @brief Mapping table of all extra choices. Fill as needed.
    ExtraChoiceMap extraChoices{};

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
    //! @brief Schedule all managed tasks.
    struct TaskDispatcher : public TaskManager
    {
        //! @brief Manage native categories.
        class NativeManager : public TaskManager
        {
        public:
            //! @brief Bit flags for managing native categories.
            std::bitset<Bottom<Category>::value> categories{};

            //! @brief Check whether any native categories do not exist.
            //! @return any native categories do not exist or exist
            [[nodiscard]] inline bool empty() const override { return categories.none(); }
            //! @brief Reset bit flags that manage native categories.
            inline void reset() override { categories.reset(); }
        } /** @brief Dispatch native type tasks. */ nativeManager{};
        //! @brief Manage extra choices of sub-cli.
        class ExtraManager : public TaskManager
        {
        public:
            //! @brief Wrap functions to check for existing and reset extra choices.
            struct IntWrap
            {
                //! @brief Check the existence status of the extra choice.
                std::function<bool()> present{};
                //! @brief Reset control of the extra choice.
                std::function<void()> reset{};
            };
            //! @brief Flag for help only.
            bool helpOnly{false};
            //! @brief Existence status and reset control of the sub-cli to which the extra choices belong.
            std::map<SubCLIName, IntWrap> checklist{};

            //! @brief Check whether any extra choices do not exist.
            //! @return any extra choices do not exist or exist
            [[nodiscard]] inline bool empty() const override
            {
                return !helpOnly
                    && std::none_of(
                        checklist.cbegin(),
                        checklist.cend(),
                        [](const auto& pair)
                        {
                            return pair.second.present();
                        });
            }
            //! @brief Reset bit flags that manage extra choices.
            inline void reset() override
            {
                helpOnly = false;
                std::for_each(
                    checklist.cbegin(),
                    checklist.cend(),
                    [](const auto& pair)
                    {
                        pair.second.reset();
                    });
            }
        } /** @brief Dispatch extra type tasks */ extraManager{};

        //! @brief Check whether any tasks do not exist.
        //! @return any tasks do not exist or exist
        [[nodiscard]] inline bool empty() const override { return nativeManager.empty() && extraManager.empty(); }
        //! @brief Reset bit flags that manage all tasks.
        inline void reset() override
        {
            nativeManager.reset();
            extraManager.reset();
        }
    } /** @brief Dispatch all types of tasks. */ taskDispatcher{};

    //! @brief Gather and notify handlers.
    class Notifier
    {
    public:
        //! @brief Destroy the Notifier object.
        virtual ~Notifier() = default;

        //! @brief A handler that performs an action when notified.
        class Handler
        {
        public:
            //! @brief Construct a new Handler object.
            //! @param procedure - procedure to be executed when the handler is updated
            explicit Handler(std::function<void()> procedure) : callback(std::move(procedure)) {}
            //! @brief Destroy the Handler object.
            virtual ~Handler() = default;

            //! @brief Call the stored callback function.
            void execute() const { callback(); }

        private:
            //! @brief Callback function to invoke.
            const std::function<void()> callback{};
        };
        //! @brief Attach a handler with a specific key to the notifier.
        //! @param key - unique identifier for the handler
        //! @param handler - handler to be attached
        void attach(const std::string_view key, const std::shared_ptr<Handler>& handler)
        {
            handlers[key.data()] = handler;
        }
        //! @brief Notify the handler associated with the given key.
        //! @param key - unique identifier for the handler
        void notify(const std::string_view key) const
        {
            const auto iter = handlers.find(key.data());
            if (iter != handlers.cend())
            {
                if (const auto handler = iter->second)
                {
                    handler->execute();
                }
            }
        }

    private:
        //! @brief Map of handlers identified by a key.
        std::map<std::string, std::shared_ptr<Handler>> handlers{};
    } /** @brief Notify for native type tasks. */ defaultNotifier{};
    //! @brief Forward messages for extra type tasks.
    action::MessageForwarder applyingForwarder{};

    //! @brief Enter console mode.
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
    //! @brief Validate dependencies version.
    void validateDependenciesVersion() const;
    //! @brief Get ASCII banner text.
    //! @return ASCII banner text content
    static std::string getIconBanner();

    //! @brief Latency (ms) for console.
    static constexpr std::uint16_t latency{10};
};
} // namespace command
} // namespace application
