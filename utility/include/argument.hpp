//! @file argument.hpp
//! @author ryftchen
//! @brief The declarations (argument) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <list>
#include <map>
#include <optional>
#include <sstream>
#include <variant>

//! @brief The utility module.
namespace utility // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Argument-parsing-related functions in the utility module.
namespace argument
{
//! @brief Brief function description.
//! @return function description (module_function)
inline static const char* description() noexcept
{
    return "UTIL_ARGUMENT";
}
extern const char* version() noexcept;

//! @brief Confirm container traits. Value is false.
//! @tparam T - type to be confirmed
template <typename T, typename = void>
struct HasContainerTraits : public std::false_type
{
};
//! @brief Confirm container traits (std::string). Value is false.
template <>
struct HasContainerTraits<std::string> : public std::false_type
{
};
//! @brief Confirm container traits (std::string_view). Value is false.
template <>
struct HasContainerTraits<std::string_view> : public std::false_type
{
};
//! @brief Confirm container traits. Value is true.
//! @tparam T - type to be confirmed
template <typename T>
struct HasContainerTraits<
    T,
    std::void_t<
        typename T::value_type,
        decltype(std::declval<T>().begin()),
        decltype(std::declval<T>().end()),
        decltype(std::declval<T>().size())>> : public std::true_type
{
};
//! @brief Confirm whether it is a container.
//! @tparam T - type to be confirmed
template <typename T>
static constexpr bool isContainer = HasContainerTraits<T>::value;

//! @brief Confirm streamable traits. Value is false.
//! @tparam T - type to be confirmed
template <typename T, typename = void>
struct HasStreamableTraits : public std::false_type
{
};
//! @brief Confirm streamable traits. Value is true.
//! @tparam T - type to be confirmed
template <typename T>
struct HasStreamableTraits<T, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<T>())>>
    : public std::true_type
{
};
//! @brief Confirm whether it is streamable.
//! @tparam T - type to be confirmed
template <typename T>
static constexpr bool isStreamable = HasStreamableTraits<T>::value;

//! @brief Enumerate specific argument patterns.
enum class ArgsNumPattern : std::uint8_t
{
    //! @brief Optional.
    optional,
    //! @brief Any.
    any,
    //! @brief At least one.
    atLeastOne
};
//! @brief Indicate the range for the number of arguments.
class ArgsNumRange
{
public:
    //! @brief Construct a new ArgsNumRange object.
    //! @param minimum - minimum of range
    //! @param maximum - maximum of range
    ArgsNumRange(const std::size_t minimum, const std::size_t maximum);

    //! @brief The operator (==) overloading of Trait class.
    //! @param rhs - right-hand side
    //! @return be equal or not
    bool operator==(const ArgsNumRange& rhs) const;
    //! @brief The operator (!=) overloading of Trait class.
    //! @param rhs - right-hand side
    //! @return be unequal or not
    bool operator!=(const ArgsNumRange& rhs) const;

    friend class Trait;
    //! @brief Check whether the number of arguments is within the range.
    //! @param value - number of arguments
    //! @return within or not
    [[nodiscard]] bool within(const std::size_t value) const;
    //! @brief Check whether the number of arguments is exact.
    //! @return be exact or not
    [[nodiscard]] bool isExact() const;
    //! @brief Check whether the maximum of the range is set.
    //! @return exist or not
    [[nodiscard]] bool existRightBound() const;

private:
    //! @brief Minimum of range.
    std::size_t min{0};
    //! @brief Maximum of range.
    std::size_t max{0};

protected:
    friend std::ostream& operator<<(std::ostream& os, const ArgsNumRange& range);
};

class Argument;

//! @brief Argument trait.
class Trait
{
public:
    //! @brief Construct a new Trait object.
    //! @tparam N - number of arguments
    //! @tparam I - index of sequences related to arguments
    //! @param prefix - prefix characters
    //! @param collection - collection of arguments to be registered
    //! @param sequence - sequences related to arguments
    template <std::size_t N, std::size_t... I>
    explicit Trait(
        const std::string_view prefix,
        std::array<std::string_view, N>&& collection,
        const std::index_sequence<I...>& sequence);
    //! @brief Construct a new Trait object.
    //! @tparam N - number of arguments
    //! @param prefix - prefix characters
    //! @param collection - collection of arguments to be registered
    template <std::size_t N>
    explicit Trait(const std::string_view prefix, std::array<std::string_view, N>&& collection) :
        Trait(prefix, std::move(collection), std::make_index_sequence<N>{})
    {
    }

    //! @brief The operator (==) overloading of Trait class.
    //! @tparam T - type of right-hand side
    //! @param rhs - right-hand side
    //! @return be equal or not
    template <typename T>
    bool operator==(const T& rhs) const;
    //! @brief The operator (!=) overloading of Trait class.
    //! @tparam T - type of right-hand side
    //! @param rhs - right-hand side
    //! @return be unequal or not
    template <typename T>
    bool operator!=(const T& rhs) const;

    //! @brief Set help message.
    //! @param message - help message
    //! @return reference of the Trait object
    Trait& help(const std::string_view message);
    //! @brief Set meta variable.
    //! @param variable - meta variable
    //! @return reference of the Trait object
    Trait& metaVariable(const std::string_view variable);
    //! @brief Set default value.
    //! @tparam T - type of default value
    //! @param value - default value
    //! @return reference of the Trait object
    template <typename T>
    Trait& defaultValue(T&& value);
    //! @brief Set default value.
    //! @param value - default value
    //! @return reference of the Trait object
    Trait& defaultValue(const std::string_view value);
    //! @brief Set implicit value.
    //! @param value - implicit value
    //! @return reference of the Trait object
    Trait& implicitValue(std::any value);
    //! @brief Set the argument property to be required.
    //! @return reference of the Trait object
    Trait& required();
    //! @brief Set the argument property to be appending.
    //! @return reference of the Trait object
    Trait& appending();
    //! @brief Set the argument property to be remaining.
    //! @return reference of the Trait object
    Trait& remaining();
    //! @brief The action of specific arguments.
    //! @tparam Func - type of callable function
    //! @tparam Args - type of bound arguments
    //! @param callable - callable function
    //! @param boundArgs - bound arguments
    //! @return reference of the Trait object
    template <typename Func, typename... Args>
    auto action(Func&& callable, Args&&... boundArgs)
        -> std::enable_if_t<std::is_invocable_v<Func, Args..., const std::string&>, Trait&>;
    //! @brief Set number of arguments.
    //! @param num - number of arguments
    //! @return reference of the Trait object
    Trait& argsNum(const std::size_t num);
    //! @brief Set minimum number and maximum number of arguments.
    //! @param numMin - minimum number
    //! @param numMax - maximum number
    //! @return reference of the Trait object
    Trait& argsNum(const std::size_t numMin, const std::size_t numMax);
    //! @brief Set number of arguments with pattern.
    //! @param pattern - argument pattern
    //! @return reference of the Trait object
    Trait& argsNum(const ArgsNumPattern pattern);
    //! @brief Consume arguments.
    //! @tparam Iterator - type of argument iterator
    //! @param start - start argument iterator
    //! @param end - end argument iterator
    //! @param argName - target argument name
    //! @return argument iterator between start and end
    template <typename Iterator>
    Iterator consume(const Iterator start, Iterator end, const std::string_view argName = {});
    //! @brief Validate all arguments.
    void validate() const;
    //! @brief Get the inline usage.
    //! @return inline usage
    [[nodiscard]] std::string getInlineUsage() const;
    //! @brief Get the length of all arguments.
    //! @return length of all arguments
    [[nodiscard]] std::size_t getArgumentsLength() const;
    friend class Argument;

private:
    //! @brief Alias for the function which has valued return.
    using ValuedAction = std::function<std::any(const std::string&)>;
    //! @brief Alias for the function which has void return.
    using VoidAction = std::function<void(const std::string&)>;
    //! @brief All argument names.
    std::vector<std::string> names;
    //! @brief Used argument name.
    std::string usedName;
    //! @brief Help message.
    std::string helpMsg;
    //! @brief Meta variable.
    std::string metaVar;
    //! @brief Default value.
    std::any defaultVal;
    //! @brief Default value content to be represented.
    std::string representedDefVal;
    //! @brief Implicit value.
    std::any implicitVal;
    //! @brief All actions of arguments.
    std::variant<ValuedAction, VoidAction> actions{
        std::in_place_type<ValuedAction>, [](const std::string& value) { return value; }};
    //! @brief Values from all arguments.
    std::vector<std::any> values;
    //! @brief Flag to indicate whether to accept optional like value.
    bool optionalAsValue{false};
    //! @brief Flag to indicate whether to be optional.
    bool isOptional{true};
    //! @brief Flag to indicate whether to be required.
    bool isRequired{true};
    //! @brief Flag to indicate whether to be repeatable.
    bool isRepeatable{true};
    //! @brief Flag to indicate whether to be used.
    bool isUsed{true};
    //! @brief End of file in arguments.
    static constexpr int eof{std::char_traits<char>::eof()};
    //! @brief Prefix characters.
    std::string prefixChars;
    //! @brief Maximum size for representing.
    static constexpr std::size_t maxRepresentSize{5};
    //! @brief The range for the number of arguments.
    ArgsNumRange argsNumRange{1, 1};

    //! @brief Represent target value.
    //! @tparam T - type of target value
    //! @param val - target value
    //! @return content to be represented
    template <typename T>
    static std::string represent(const T& val);
    //! @brief Implementation of wrapping function calls that have scope.
    //! @tparam Func - type of callable function
    //! @tparam Tuple - type of bound arguments tuple
    //! @tparam Extra - type of extra option
    //! @tparam I - number of sequence which converted from bound arguments tuple
    //! @param func - callable function
    //! @param tup - bound arguments tuple
    //! @param ext - extra option
    //! @param seq - sequence which converted from bound arguments tuple
    //! @return wrapping of calls
    template <typename Func, typename Tuple, typename Extra, std::size_t... I>
    static constexpr decltype(auto) applyScopedOneImpl(
        Func&& func, Tuple&& tup, Extra&& ext, const std::index_sequence<I...>& seq);
    //! @brief Wrap function calls that have scope.
    //! @tparam Func - type of callable function
    //! @tparam Tuple - type of bound arguments tuple
    //! @tparam Extra - type of extra option
    //! @param func - callable function
    //! @param tup - bound arguments tuple
    //! @param ext - extra option
    //! @return wrapping of calls
    template <typename Func, typename Tuple, typename Extra>
    static constexpr decltype(auto) applyScopedOne(Func&& func, Tuple&& tup, Extra&& ext);
    //! @brief Throw an exception when the range for the number of arguments is invalid.
    [[noreturn]] void throwInvalidArgsNumRange() const;
    //! @brief Find the character in the argument.
    //! @param name - name of argument
    //! @return character
    static int lookAhead(const std::string_view name);
    //! @brief Check whether the argument is optional.
    //! @param name - name of argument
    //! @param prefix - prefix characters
    //! @return be optional or not
    static bool checkIfOptional(const std::string_view name, const std::string_view prefix);
    //! @brief Check whether the argument is positional.
    //! @param name - name of argument
    //! @param prefix - prefix characters
    //! @return be positional or not
    static bool checkIfPositional(const std::string_view name, const std::string_view prefix);
    //! @brief Get the member.
    //! @tparam T - type of member to be got
    //! @return member corresponding to the specific type
    template <typename T>
    T get() const;
    //! @brief Retrieves the value of the argument, if any.
    //! @tparam T - type of argument
    //! @return an optional that contains the value of the argument if it exists, otherwise an empty optional
    template <typename T>
    std::optional<T> present() const;
    //! @brief Convert the container type.
    //! @tparam T - type of container
    //! @param operand - container to be converted
    //! @return container after converting type
    template <typename T>
    static T anyCastContainer(const std::vector<std::any>& operand);
    //! @brief Wrap for applying actions.
    //! @tparam Iterator - type of argument iterator
    template <typename Iterator>
    struct ApplyAction
    {
        //! @brief The first argument iterator.
        const Iterator first{};
        //! @brief The last argument iterator.
        const Iterator last{};
        //! @brief The Trait instance.
        Trait& self;

        //! @brief The operator (()) overloading of ApplyAction struct.
        //! @param func - function which has valued return
        void operator()(const ValuedAction& func) const
        {
            std::transform(first, last, std::back_inserter(self.values), func);
        }
        //! @brief The operator (()) overloading of ApplyAction struct.
        //! @param func - function which has void return
        void operator()(const VoidAction& func) const
        {
            std::for_each(first, last, func);
            if (!self.defaultVal.has_value() && !self.optionalAsValue)
            {
                self.values.resize(static_cast<std::size_t>(std::distance(first, last)));
            }
        }
    };

protected:
    friend std::ostream& operator<<(std::ostream& os, const Trait& tra);
    friend std::ostream& operator<<(std::ostream& os, const Argument& arg);
};

template <std::size_t N, std::size_t... I>
Trait::Trait(
    const std::string_view prefix,
    std::array<std::string_view, N>&& collection,
    const std::index_sequence<I...>& /*sequence*/) :
    isOptional{(checkIfOptional(collection.at(I), prefix) || ...)},
    isRequired{false},
    isRepeatable{false},
    isUsed{false},
    prefixChars{prefix}
{
    (names.emplace_back(std::move(collection).at(I)), ...);
    std::sort(
        names.begin(),
        names.end(),
        [](const auto& lhs, const auto& rhs)
        { return (lhs.size() == rhs.size()) ? (lhs < rhs) : (lhs.size() < rhs.size()); });
}

template <typename T>
bool Trait::operator==(const T& rhs) const
{
    if constexpr (!isContainer<T>)
    {
        return rhs == get<T>();
    }
    else
    {
        const auto& lhs = get<T>();
        return std::equal(
            std::cbegin(lhs),
            std::cend(lhs),
            std::cbegin(rhs),
            std::cend(rhs),
            [](const auto& lhs, const auto& rhs) { return rhs == std::any_cast<const typename T::value_type&>(lhs); });
    }
}

template <typename T>
bool Trait::operator!=(const T& rhs) const
{
    return !(rhs == *this);
}

template <typename T>
Trait& Trait::defaultValue(T&& value)
{
    representedDefVal = represent(value);
    defaultVal = std::forward<T>(value);

    return *this;
}

template <typename Func, typename... Args>
auto Trait::action(Func&& callable, Args&&... boundArgs)
    -> std::enable_if_t<std::is_invocable_v<Func, Args..., const std::string&>, Trait&>
{
    using ActionType = std::conditional_t<
        std::is_void_v<std::invoke_result_t<Func, Args..., const std::string&>>,
        VoidAction,
        ValuedAction>;
    if constexpr (!sizeof...(Args))
    {
        actions.emplace<ActionType>(std::forward<Func>(callable));
    }
    else
    {
        actions.emplace<ActionType>([func = std::forward<Func>(callable),
                                     tup = std::make_tuple(std::forward<Args>(boundArgs)...)](const std::string& opt)
                                    { return applyScopedOne(func, tup, opt); });
    }

    return *this;
}

template <typename Iterator>
Iterator Trait::consume(const Iterator start, Iterator end, const std::string_view argName)
{
    if (!isRepeatable && isUsed)
    {
        throw std::runtime_error{"Duplicate argument."};
    }

    isUsed = true;
    usedName = argName;
    const auto numMin = argsNumRange.min, numMax = argsNumRange.max;
    if (numMax == 0)
    {
        values.emplace_back(implicitVal);
        std::visit([](const auto& func) { func({}); }, actions);
        return start;
    }
    if (auto dist = static_cast<std::size_t>(std::distance(start, end)); dist >= numMin)
    {
        if (dist > numMax)
        {
            end = std::next(start, static_cast<typename Iterator::difference_type>(numMax));
        }
        if (!optionalAsValue)
        {
            end = std::find_if(start, end, std::bind(checkIfOptional, std::placeholders::_1, prefixChars));
            dist = static_cast<std::size_t>(std::distance(start, end));
            if (dist < numMin)
            {
                throw std::runtime_error{"Too few arguments."};
            }
        }
        std::visit(ApplyAction<Iterator>{start, end, *this}, actions);

        return end;
    }
    if (defaultVal.has_value())
    {
        return start;
    }

    throw std::runtime_error{"Too few arguments for '" + usedName + "'."};
}

template <typename T>
std::string Trait::represent(const T& val)
{
    if constexpr (std::is_same_v<T, bool>)
    {
        return val ? "true" : "false";
    }
    else if constexpr (std::is_convertible_v<T, std::string_view>)
    {
        return '"' + val + '"';
    }
    else if constexpr (isContainer<T>)
    {
        std::ostringstream out{};
        out << '{';
        const auto size = val.size();
        if (size > 1)
        {
            out << represent(*std::cbegin(val));
            std::for_each(
                std::next(std::cbegin(val)),
                std::next(
                    std::cbegin(val),
                    static_cast<typename T::iterator::difference_type>(
                        std::min<std::size_t>(size, maxRepresentSize) - 1)),
                [&out](const auto& v) { out << ' ' << represent(v); });
            if (size <= maxRepresentSize)
            {
                out << ' ';
            }
            else
            {
                out << " ... ";
            }
        }
        if (size > 0)
        {
            out << represent(*std::prev(std::cend(val)));
        }
        out << '}';
        return std::move(out).str();
    }
    else if constexpr (isStreamable<T>)
    {
        std::ostringstream out{};
        out << val;
        return std::move(out).str();
    }
    else
    {
        return " not representable ";
    }
}

template <typename Func, typename Tuple, typename Extra, std::size_t... I>
constexpr decltype(auto) Trait::applyScopedOneImpl(
    Func&& func, Tuple&& tup, Extra&& ext, const std::index_sequence<I...>& /*seq*/)
{
    return std::invoke(std::forward<Func>(func), std::get<I>(std::forward<Tuple>(tup))..., std::forward<Extra>(ext));
}

template <typename Func, typename Tuple, typename Extra>
constexpr decltype(auto) Trait::applyScopedOne(Func&& func, Tuple&& tup, Extra&& ext)
{
    return applyScopedOneImpl(
        std::forward<Func>(func),
        std::forward<Tuple>(tup),
        std::forward<Extra>(ext),
        std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<Tuple>>>{});
}

template <typename T>
T Trait::get() const
{
    if (!values.empty())
    {
        if constexpr (isContainer<T>)
        {
            return anyCastContainer<T>(values);
        }
        else
        {
            return std::any_cast<T>(values.front());
        }
    }
    if (defaultVal.has_value())
    {
        return std::any_cast<T>(defaultVal);
    }
    if constexpr (isContainer<T>)
    {
        if (!optionalAsValue)
        {
            return anyCastContainer<T>(values);
        }
    }

    throw std::runtime_error{"No value specified for '" + names.back() + "'."};
}

template <typename T>
std::optional<T> Trait::present() const
{
    if (defaultVal.has_value())
    {
        throw std::runtime_error{"Default value always presents."};
    }
    if (values.empty())
    {
        return std::nullopt;
    }
    if constexpr (isContainer<T>)
    {
        return anyCastContainer<T>(values);
    }

    return std::any_cast<T>(values.front());
}

template <typename T>
T Trait::anyCastContainer(const std::vector<std::any>& operand)
{
    T result{};
    std::transform(
        operand.cbegin(),
        operand.cend(),
        std::back_inserter(result),
        [](const auto& value) { return std::any_cast<typename T::value_type>(value); });

    return result;
}

//! @brief Parse arguments.
class Argument
{
public:
    //! @brief Construct a new Argument object.
    //! @param title - title name
    //! @param version - version number
    explicit Argument(const std::string_view title = {}, const std::string_view version = "1.0.0") :
        titleName{title}, versionNumber{version}, parserPath{title}
    {
    }
    //! @brief Destroy the Argument object.
    virtual ~Argument() = default;
    //! @brief Construct a new Argument object.
    //! @param arg - object for copy constructor
    Argument(const Argument& arg);
    //! @brief Construct a new Argument object.
    Argument(Argument&&) noexcept = default;
    //! @brief The operator (=) overloading of Argument class.
    //! @param arg - object for copy assignment operator
    //! @return reference of the Argument object
    Argument& operator=(const Argument& arg);
    //! @brief The operator (=) overloading of Argument class.
    //! @return reference of the Argument object
    Argument& operator=(Argument&&) noexcept = default;

    //! @brief The operator (bool) overloading of Argument class.
    explicit operator bool() const;
    //! @brief The operator ([]) overloading of Trait class.
    //! @param argName - target argument name
    //! @return reference of the Trait object
    Trait& operator[](const std::string_view argName) const;

    //! @brief Add a single argument.
    //! @tparam ArgsType - type of arguments
    //! @param fewArgs - argument name
    //! @return reference of the Trait object
    template <typename... ArgsType>
    Trait& addArgument(ArgsType... fewArgs);
    //! @brief Add a descrText.
    //! @param text - descrText text
    //! @return reference of the Trait object
    Argument& addDescription(const std::string_view text);
    //! @brief Get the Trait or Argument instance by name.
    //! @tparam T - type of instance
    //! @param name - instance name
    //! @return Trait or Argument instance
    template <typename T = Trait>
    T& at(const std::string_view name);
    //! @brief Parse all input arguments.
    //! @param arguments - container of all arguments
    void parseArgs(const std::vector<std::string>& arguments);
    //! @brief Parse all input arguments.
    //! @param argc - argument count
    //! @param argv - argument vector
    void parseArgs(const int argc, const char* const argv[]);
    //! @brief Get argument value by name.
    //! @tparam T - type of argument
    //! @param argName - target argument name
    //! @return argument value
    template <typename T = std::string>
    T get(const std::string_view argName) const;
    //! @brief Retrieves the value of the argument, if any.
    //! @tparam T - type of argument
    //! @param argName - target argument name
    //! @return an optional that contains the value of the argument if it exists, otherwise an empty optional
    template <typename T = std::string>
    std::optional<T> present(const std::string_view argName) const;
    //! @brief Check whether the argument is used.
    //! @param argName - target argument name
    //! @return be used or not
    bool isUsed(const std::string_view argName) const;
    //! @brief Check whether the sub-command is used.
    //! @param subCommandName - target sub-command name
    //! @return be used or not
    bool isSubCommandUsed(const std::string_view subCommandName) const;
    //! @brief Check whether the sub-command is used.
    //! @param subParser - target sub-parser
    //! @return be used or not
    bool isSubCommandUsed(const Argument& subParser) const;
    //! @brief Get the title name.
    //! @return title name
    std::string title() const;
    //! @brief Get the version number.
    //! @return version number
    std::string version() const;
    //! @brief Get the help message content.
    //! @return help message content
    std::ostringstream help() const;
    //! @brief Get the usage content.
    //! @return usage content
    std::string usage() const;
    //! @brief Add a sub-parser.
    //! @param parser - sub-parser
    void addSubParser(Argument& parser);

private:
    //! @brief Title name.
    std::string titleName;
    //! @brief Version number.
    std::string versionNumber;

    //! @brief Alias for the iterator in all Trait instances.
    using TraitIter = std::list<Trait>::iterator;
    //! @brief Alias for the iterator in all Argument instances.
    using ArgumentIter = std::list<std::reference_wrapper<Argument>>::iterator;
    //! @brief Description text.
    std::string descrText;
    //! @brief Prefix characters.
    std::string prefixChars{"-"};
    //! @brief Assign characters.
    std::string assignChars{"="};
    //! @brief Flag to indicate whether to be parsed.
    bool isParsed{false};
    //! @brief List of optional arguments.
    std::list<Trait> optionalArgs;
    //! @brief List of positional arguments.
    std::list<Trait> positionalArgs;
    //! @brief Mapping table of argument.
    std::unordered_map<std::string_view, TraitIter> argumentMap;
    //! @brief Current parser path.
    std::string parserPath;
    //! @brief List of sub-parsers.
    std::list<std::reference_wrapper<Argument>> subParsers;
    //! @brief Mapping table of sub-parser.
    std::map<std::string_view, ArgumentIter> subParserMap;
    //! @brief Mapping table of sub-parser usage.
    std::map<std::string_view, bool> subParserUsed;

    //! @brief Check whether the prefix character is valid.
    //! @param c - prefix character
    //! @return be valid or invalid
    bool isValidPrefixChar(const char c) const;
    //! @brief Get any valid prefix character.
    //! @return valid prefix character
    char getAnyValidPrefixChar() const;
    //! @brief Preprocess all raw arguments.
    //! @param rawArguments - container of all raw arguments
    //! @return preprocessed argument container
    std::vector<std::string> preprocessArguments(const std::vector<std::string>& rawArguments) const;
    //! @brief Parse all input arguments for internal.
    //! @param rawArguments - container of all raw arguments
    void parseArgsInternal(const std::vector<std::string>& rawArguments);
    //! @brief Process the registered argument.
    //! @tparam Iterator - type of argument iterator
    //! @param current - current argument iterator
    //! @param end - end argument iterator
    //! @param argName - target argument name
    //! @return argument iterator between current and end
    template <typename Iterator>
    Iterator processRegArgument(Iterator current, const Iterator end, const std::string_view argName) const;
    //! @brief Get the length of the longest argument.
    //! @return length of the longest argument
    std::size_t getLengthOfLongestArgument() const;
    //! @brief Make index for argumentMap.
    //! @param iterator - iterator in all argument traits
    void indexArgument(const TraitIter& iterator);

protected:
    friend std::ostream& operator<<(std::ostream& os, const Argument& arg);
};

template <typename... ArgsType>
Trait& Argument::addArgument(ArgsType... fewArgs)
{
    const auto argument = optionalArgs.emplace(
        optionalArgs.cend(), prefixChars, std::array<std::string_view, sizeof...(ArgsType)>{fewArgs...});
    if (!argument->isOptional)
    {
        positionalArgs.splice(positionalArgs.cend(), optionalArgs, argument);
    }
    indexArgument(argument);

    return *argument;
}

template <typename T>
T& Argument::at(const std::string_view name)
{
    if constexpr (std::is_same_v<T, Trait>)
    {
        return (*this)[name];
    }
    else if (const auto subParserIter = subParserMap.find(name); subParserIter != subParserMap.cend())
    {
        return subParserIter->second->get();
    }

    throw std::runtime_error{"No such sub-parser: " + std::string{name} + '.'};
}

template <typename T>
T Argument::get(const std::string_view argName) const
{
    if (!isParsed)
    {
        throw std::runtime_error{"Nothing parsed, no arguments are available."};
    }

    return (*this)[argName].get<T>();
}

template <typename T>
std::optional<T> Argument::present(const std::string_view argName) const
{
    return (*this)[argName].present<T>();
}

template <typename Iterator>
Iterator Argument::processRegArgument(Iterator current, const Iterator end, const std::string_view argName) const
{
    if (const auto argMapIter = argumentMap.find(argName); argMapIter != argumentMap.cend())
    {
        const auto argument = argMapIter->second;
        current = argument->consume(std::next(current), end, argMapIter->first);
    }
    else if (const auto& compoundArg = argName; (compoundArg.length() > 1) && isValidPrefixChar(compoundArg.at(0))
             && !isValidPrefixChar(compoundArg.at(1)))
    {
        ++current;
        for (std::size_t i = 1; i < compoundArg.length(); ++i)
        {
            const auto hypotheticalArg = std::string{'-', compoundArg.at(i)};
            if (const auto argMapIter = argumentMap.find(hypotheticalArg); argMapIter != argumentMap.cend())
            {
                auto argument = argMapIter->second;
                current = argument->consume(current, end, argMapIter->first);
                continue;
            }
            throw std::runtime_error{"Unknown argument: " + std::string{argName} + '.'};
        }
    }
    else
    {
        throw std::runtime_error{"Unknown argument: " + std::string{argName} + '.'};
    }

    return current;
}
} // namespace argument
} // namespace utility
