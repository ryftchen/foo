//! @file command.hpp
//! @author ryftchen
//! @brief The declarations (command) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#pragma once

#include "apply.hpp"
#include "note.hpp"

#include "utility/include/argument.hpp"
#include "utility/include/console.hpp"

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

//! @brief Enumerate specific basic categories.
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

    //! @brief Mutex for controlling multi-threading.
    mutable std::mutex mtx{};
    //! @brief The synchronization condition for foreground and background. Use with mtx.
    std::condition_variable cv{};
    //! @brief Flag to indicate whether parsing of arguments is completed.
    std::atomic<bool> isParsed{false};
    //! @brief Parse argument helper for commander.
    utility::argument::Argument mainCLI{"foo", note::version()};
    //! @brief Parse argument helper to apply algorithm.
    utility::argument::Argument subCLIAppAlgo{
        utility::reflection::TypeInfo<app_algo::AlgorithmChoice>::name, note::version()};
    //! @brief Parse argument helper to apply design pattern.
    utility::argument::Argument subCLIAppDp{
        utility::reflection::TypeInfo<app_dp::DesignPatternChoice>::name, note::version()};
    //! @brief Parse argument helper to apply data structure.
    utility::argument::Argument subCLIAppDs{
        utility::reflection::TypeInfo<app_ds::DataStructureChoice>::name, note::version()};
    //! @brief Parse argument helper to apply numeric.
    utility::argument::Argument subCLIAppNum{
        utility::reflection::TypeInfo<app_num::NumericChoice>::name, note::version()};

    //! @brief Initialize the parse argument helpers.
    void initializeCLI();
    //! @brief Foreground handler for parsing command line arguments.
    //! @param argc - argument count
    //! @param argv - argument vector
    void foregroundHandler(const int argc, const char* const argv[]);
    //! @brief Background handler for performing the specific tasks.
    void backgroundHandler();
    //! @brief Pre-check the basic type or regular type task.
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
    void dumpConfiguration() const;
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
        apply::EventType event{};
    };
    //! @brief Alias for the map of CategoryName and CategoryExtAttr.
    using SubCLIMap = std::map<CategoryName, CategoryExtAttr>;
    //! @brief Alias for the map of SubCLIName and SubCLIMap.
    using RegularChoiceMap = std::map<SubCLIName, SubCLIMap>;

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

    // clang-format off
    //! @brief Mapping table of all basic categories.
    const std::map<CategoryName, void (Command::*)() const> basicCategories
    {
        // - Category -+----------- Functor -----------
        // ------------+-------------------------------
        { "console"    , &Command::executeInConsole  },
        { "dump"       , &Command::dumpConfiguration },
        { "help"       , &Command::showHelpMessage   },
        { "version"    , &Command::showVersionIcon   }
        // ------------+-------------------------------
    };
    // clang-format on
    //! @brief Mapping table of all regular choices. Fill as needed.
    RegularChoiceMap regularChoices{};

    //! @brief Manage basic categories.
    class BasicManager
    {
    public:
        //! @brief Bit flags for managing basic categories.
        std::bitset<Bottom<Category>::value> categories{};

        //! @brief Check whether any basic categories do not exist.
        //! @return any basic categories do not exist or exist
        [[nodiscard]] inline bool empty() const { return categories.none(); }
        //! @brief Reset bit flags that manage basic categories.
        inline void reset() { categories.reset(); }
    };

    //! @brief Manage regular choices of sub-cli.
    class RegularManager
    {
    public:
        //! @brief Enumerate specific regular choices.
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

        //! @brief Check whether any regular choices do not exist.
        //! @return any regular choices do not exist or exist
        [[nodiscard]] inline bool empty() const
        {
            return app_algo::manager().empty() && app_ds::manager().empty() && app_dp::manager().empty()
                && app_num::manager().empty() && !helpOnly;
        }
        //! @brief Reset bit flags that manage regular choices.
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
            throw std::logic_error("The current regular choice does not meet the requirement of having only one order");
        }
    };

    //! @brief Manage all types of tasks.
    struct DispatchManager
    {
        //! @brief Dispatch basic type tasks.
        BasicManager basicManager{};
        //! @brief Dispatch regular type tasks.
        RegularManager regularManager{};

        //! @brief Check whether any tasks do not exist.
        //! @return any tasks do not exist or exist
        [[nodiscard]] inline bool empty() const { return basicManager.empty() && regularManager.empty(); }
        //! @brief Reset bit flags that manage all tasks.
        inline void reset()
        {
            basicManager.reset();
            regularManager.reset();
        }
    } /** @brief Dispatch all types of tasks. */ dispatchManager{};
    //! @brief Forward messages for applying.
    apply::MessageForwarder applyMessage{};

    //! @brief Enter console mode.
    static void enterConsoleMode();
    //! @brief Register the command line to console mode.
    //! @tparam T - type of client
    //! @param console - console to be registered
    //! @param client - client used to send
    template <typename T>
    static void registerOnConsole(utility::console::Console& console, std::shared_ptr<T>& client);
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
