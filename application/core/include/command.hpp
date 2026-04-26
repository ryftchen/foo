//! @file command.hpp
//! @author ryftchen
//! @brief The declarations (command) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2026 ryftchen. All rights reserved.

#pragma once

#include "build.hpp"
#include "console.hpp"
#include "schedule.hpp"

#include "utility/include/argument.hpp"

//! @brief The application module.
namespace application // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Command-line-related functions in the application module.
namespace command
{
//! @brief Instance title.
inline constexpr std::string title = "commander";

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

    friend Command& getInstance();

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
    utility::argument::Argument mainCLI{"foo", build::version()};
    //! @brief Parse argument helper to apply algorithm.
    utility::argument::Argument subCLIAppAlgo{schedule::meta::name<reg_algo::ApplyAlgorithm>(), reg_algo::version()};
    //! @brief Parse argument helper to apply design pattern.
    utility::argument::Argument subCLIAppDp{schedule::meta::name<reg_dp::ApplyDesignPattern>(), reg_dp::version()};
    //! @brief Parse argument helper to apply data structure.
    utility::argument::Argument subCLIAppDs{schedule::meta::name<reg_ds::ApplyDataStructure>(), reg_ds::version()};
    //! @brief Parse argument helper to apply numeric.
    utility::argument::Argument subCLIAppNum{schedule::meta::name<reg_num::ApplyNumeric>(), reg_num::version()};
    //! @brief The short prefix for the option.
    const std::string shortPrefix{"-"};
    //! @brief The Long prefix for the option.
    const std::string longPrefix{"--"};
    //! @brief Flag to indicate whether the command is faulty.
    std::atomic_bool isFaulty{false};

    friend bool executeCLI(const int argc, const char* const argv[]);
    //! @brief Interface used to execute.
    //! @param argc - argument count
    //! @param argv - argument vector
    //! @return successful or failed to execute
    bool execute(const int argc, const char* const argv[]);
    //! @brief Initialize the parse argument helpers for native.
    void initializeNativeCLI();
    //! @brief Initialize the parse argument helpers for extra.
    void initializeExtraCLI();
    //! @brief Set up the main command line interface.
    void mainCLISetup();
    //! @brief Set up the sub-command line interface.
    //! @tparam SubCLI - type of sub-cli
    template <typename SubCLI>
    void subCLISetup();
    //! @brief Resolve target sub-cli.
    //! @tparam Mapped - type of sub-cli or sub-cli's category
    //! @return parse argument helper to apply
    template <typename Mapped>
    utility::argument::Argument& resolveSubCLI();
    //! @brief Inject a new sub-cli.
    //! @tparam SubCLI - type of sub-cli
    //! @tparam Intf - type of wrapped interface
    template <typename SubCLI, typename Intf>
    void injectNewSubCLI(Intf&& intf);
    //! @brief Add a new category registration to the sub-cli.
    //! @tparam Cat - type of sub-cli's category
    //! @param version - category version
    template <typename Cat>
    void addNewCategoryToSubCLI(const std::string_view version);
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
    //! @brief Clear all type tasks.
    void clearSelected();
    //! @brief Dispatch all specific tasks.
    void dispatchAll();
    //! @brief Check for excessive arguments.
    void checkExcessArgs();
    //! @brief Execute the command line of console mode.
    void executeInConsole() const;
    //! @brief Show help message.
    void showHelpMessage() const;
    //! @brief Dump configuration.
    static void dumpConfiguration();
    //! @brief Display version information.
    void displayVersionInfo() const;
    //! @brief Validate the version of all dependencies.
    void validateDependencies() const;

    friend schedule::native::Notifier<Command>;
    //! @brief Local notification for native type tasks.
    schedule::native::Notifier<Command> builtInNotifier{};
    //! @brief Forward messages for extra type tasks.
    schedule::extra::MessageForwarder applyingForwarder{};
    //! @brief Dispatch all types of tasks.
    schedule::TaskScheduler scheduleDispatcher{};
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
            std::size_t seed = 0;
            seed ^= std::hash<std::string>()(key.first) + magicNumber + (seed << leftShift) + (seed >> rightShift);
            seed ^= std::hash<std::string>()(key.second) + magicNumber + (seed << leftShift) + (seed >> rightShift);
            return seed;
        }
    };
    //! @brief Mapping table of related versions. Fill as needed.
    std::unordered_multimap<VerLinkKey, std::string, VerLinkHash> versionLinks;

    //! @brief Go to console mode for troubleshooting.
    static void enterConsoleMode();
    //! @brief Register the command line to console mode.
    //! @tparam Sock - type of client
    //! @param session - console to be registered
    //! @param client - client used to send
    template <typename Sock>
    static void registerOnConsole(console::Console& session, std::shared_ptr<Sock>& client);
    //! @brief Launch the client for console mode.
    //! @tparam Sock - type of client
    //! @param client - client to be launched
    template <typename Sock>
    static void launchClient(std::shared_ptr<Sock>& client);
    //! @brief Process the inputs  in console mode.
    //! @param handling - handling for inputs
    //! @return console return code
    static auto processConsoleInputs(const std::function<void()>& handling);
    //! @brief Parse the message inside the client in console mode.
    //! @param bytes - message buffer
    //! @param size - message length
    //! @return need to continue parsing or not
    static bool onParsing4Client(char* const bytes, const std::size_t size);
    //! @brief Await outside the client in console mode.
    static void waitClientOutputDone();
    //! @brief Awaken inside the client in console mode.
    static void notifyClientOutputDone();
    //! @brief Build the exit request message in console mode.
    //! @return exit request message
    static std::string buildDisconnectRequest();
    //! @brief Console latency in the millisecond range.
    static void interactionLatency();
};

extern bool executeCLI(const int argc, const char* const argv[]);
} // namespace command
} // namespace application
