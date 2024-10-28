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

    //! @brief Gather and notify observers.
    class Notifier
    {
    public:
        //! @brief Destroy the Notifier object.
        virtual ~Notifier() = default;

        //! @brief An observer that performs an action when notified.
        class Observer
        {
        public:
            //! @brief Construct a new Observer object.
            //! @param callback - function to be called when the observer is updated
            explicit Observer(const std::function<void()>& callback) : callback(callback) {}
            //! @brief Destroy the Observer object.
            virtual ~Observer() = default;

            //! @brief Call the stored callback function.
            void update() const { callback(); }

        private:
            //! @brief Callback function to invoke.
            const std::function<void()> callback{};
        };
        //! @brief Attach an observer with a specific key to the notifier.
        //! @param key - unique identifier for the observer
        //! @param observer - observer to attach
        void attach(const std::string_view key, const std::shared_ptr<Observer>& observer)
        {
            observers[key.data()] = observer;
        }
        //! @brief Notify the observer associated with the given key.
        //! @param key - unique identifier for the observer
        void notify(const std::string_view key) const
        {
            const auto iter = observers.find(key.data());
            if (iter != observers.cend())
            {
                if (const auto observer = iter->second)
                {
                    observer->update();
                }
            }
        }

    private:
        //! @brief Map of observers identified by a key.
        std::map<std::string, std::shared_ptr<Observer>> observers{};
    };
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

    //! @brief Manage native categories.
    class NativeManager
    {
    public:
        //! @brief Bit flags for managing native categories.
        std::bitset<Bottom<Category>::value> categories{};

        //! @brief Check whether any native categories do not exist.
        //! @return any native categories do not exist or exist
        [[nodiscard]] inline bool empty() const { return categories.none(); }
        //! @brief Reset bit flags that manage native categories.
        inline void reset() { categories.reset(); }
    };
    //! @brief Manage extra choices of sub-cli.
    class ExtraManager
    {
    public:
        //! @brief Enumerate specific extra choices.
        enum Order : std::uint8_t
        {
            //! @brief Algorithm.
            algorithm,
            //! @brief Design pattern.
            designPattern,
            //! @brief Data structure.
            dataStructure,
            //! @brief Numeric.
            numeric
        };
        //! @brief Flag for help only.
        bool helpOnly{false};

        //! @brief Check whether any extra choices do not exist.
        //! @return any extra choices do not exist or exist
        [[nodiscard]] inline bool empty() const
        {
            return app_algo::manager().empty() && app_ds::manager().empty() && app_dp::manager().empty()
                && app_num::manager().empty() && !helpOnly;
        }
        //! @brief Reset bit flags that manage extra choices.
        inline void reset()
        {
            app_algo::manager().reset();
            app_ds::manager().reset();
            app_dp::manager().reset();
            app_num::manager().reset();
            helpOnly = false;
        }
        //! @brief Get the existing order.
        //! @return existing order
        Order getExistingOrder()
        {
            const std::uint8_t validation = !app_algo::manager().empty() + !app_dp::manager().empty()
                + !app_ds::manager().empty() + !app_num::manager().empty();
            if (1 == validation)
            {
                if (!app_algo::manager().empty())
                {
                    return Order::algorithm;
                }
                else if (!app_dp::manager().empty())
                {
                    return Order::designPattern;
                }
                else if (!app_ds::manager().empty())
                {
                    return Order::dataStructure;
                }
                else if (!app_num::manager().empty())
                {
                    return Order::numeric;
                }
            }

            reset();
            throw std::logic_error("There can only be one order for the extra choices.");
        }
    };
    //! @brief Manage all types of tasks.
    struct TaskDispatcher
    {
        //! @brief Dispatch native type tasks.
        NativeManager nativeManager{};
        //! @brief Dispatch extra type tasks.
        ExtraManager extraManager{};

        //! @brief Check whether any tasks do not exist.
        //! @return any tasks do not exist or exist
        [[nodiscard]] inline bool empty() const { return nativeManager.empty() && extraManager.empty(); }
        //! @brief Reset bit flags that manage all tasks.
        inline void reset()
        {
            nativeManager.reset();
            extraManager.reset();
        }
    } /** @brief Dispatch all types of tasks. */ taskDispatcher{};
    //! @brief Notify for native type tasks.
    Notifier defaultNotifier{};
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
