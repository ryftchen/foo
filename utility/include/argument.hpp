//! @file argument.hpp
//! @author ryftchen
//! @brief The declarations (argument) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#pragma once

#include <any>
#include <cstdint>
#include <functional>
#include <limits>
#include <list>
#include <map>
#include <optional>
#include <sstream>
#include <variant>

//! @brief The utility module.
namespace utility // NOLINT (modernize-concat-nested-namespaces)
{
//! @brief Argument-parsing-related functions in the utility module.
namespace argument
{
extern const char* version() noexcept;

//! @brief Confirm container traits. Value is false.
//! @tparam T - type to be confirmed
template <typename T, typename = void>
struct HasContainerTraits : std::false_type
{
};
//! @brief Confirm container traits (std::string). Value is false.
template <>
struct HasContainerTraits<std::string> : std::false_type
{
};
//! @brief Confirm container traits (std::string_view). Value is false.
template <>
struct HasContainerTraits<std::string_view> : std::false_type
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
        decltype(std::declval<T>().size())>> : std::true_type
{
};
//! @brief Confirm whether it is a container.
//! @tparam T - type to be confirmed
template <typename T>
static constexpr bool isContainer = HasContainerTraits<T>::value;

//! @brief Confirm streamable traits. Value is false.
//! @tparam T - type to be confirmed
template <typename T, typename = void>
struct HasStreamableTraits : std::false_type
{
};
//! @brief Confirm streamable traits. Value is true.
//! @tparam T - type to be confirmed
template <typename T>
struct HasStreamableTraits<T, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<T>())>>
    : std::true_type
{
};
//! @brief Confirm whether it is streamable.
//! @tparam T - type to be confirmed
template <typename T>
static constexpr bool isStreamable = HasStreamableTraits<T>::value;

//! @brief Maximum container size for representing.
constexpr std::size_t representMaxContainerSize = 5;

//! @brief Represent target value.
//! @tparam T - type of target value
//! @param val - target value
//! @return content to be represented
template <typename T>
std::string represent(const T& val)
{
    if constexpr (std::is_same_v<T, bool>)
    {
        return val ? "true" : "false";
    }
    else if constexpr (std::is_convertible_v<T, std::string_view>)
    {
        return '"' + std::string{std::string_view{val}} + '"';
    }
    else if constexpr (isContainer<T>)
    {
        std::ostringstream out{};
        out << '{';
        const auto size = val.size();
        if (size > 1)
        {
            out << represent(*val.begin());
            std::for_each(
                std::next(val.begin()),
                std::next(
                    val.begin(),
                    static_cast<typename T::iterator::difference_type>(
                        std::min<std::size_t>(size, representMaxContainerSize) - 1)),
                [&out](const auto& v)
                {
                    out << ' ' << represent(v);
                });
            if (size <= representMaxContainerSize)
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
            out << represent(*std::prev(val.end()));
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

//! @brief Implementation of wrapping function calls that have scope.
//! @tparam Func - type of callable function
//! @tparam Tuple - type of bound arguments tuple
//! @tparam Extra - type of extra option
//! @tparam I - number of sequence which converted from bound arguments tuple
//! @return wrapping of calls
template <class Func, class Tuple, class Extra, std::size_t... I>
constexpr decltype(auto) applyScopedOneImpl(
    Func&& func, Tuple&& tup, Extra&& ext, const std::index_sequence<I...>& /*sequence*/)
{
    return std::invoke(std::forward<Func>(func), std::get<I>(std::forward<Tuple>(tup))..., std::forward<Extra>(ext));
}

//! @brief Wrap function calls that have scope.
//! @tparam Func - type of callable function
//! @tparam Tuple - type of bound arguments tuple
//! @tparam Extra - type of extra option
//! @param func - callable function
//! @param tup - bound arguments tuple
//! @param ext - extra option
//! @return wrapping of calls
template <class Func, class Tuple, class Extra>
constexpr decltype(auto) applyScopedOne(Func&& func, Tuple&& tup, Extra&& ext)
{
    return applyScopedOneImpl(
        std::forward<Func>(func),
        std::forward<Tuple>(tup),
        std::forward<Extra>(ext),
        std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<Tuple>>>{});
}

//! @brief Join a series of strings into a single string using a separator.
//! @tparam StrIter - type of iterator
//! @param first - iterator pointing to the beginning of the range
//! @param last - iterator pointing to the end of the range
//! @param separator - separator to be used between strings
//! @return joined string
template <typename StrIter>
std::string join(StrIter first, StrIter last, const std::string_view separator)
{
    if (first == last)
    {
        return {};
    }

    std::ostringstream value{};
    value << *first;
    ++first;
    while (first != last)
    {
        value << separator << *first;
        ++first;
    }
    return std::move(value).str();
}

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

class Argument;

//! @brief Argument register.
class Register
{
public:
    //! @brief Construct a new Register object.
    //! @tparam N - number of arguments
    //! @tparam I - index of sequences related to arguments
    //! @param prefix - prefix characters
    //! @param array - array of arguments to be registered
    //! @param sequence - sequences related to arguments
    template <std::size_t N, std::size_t... I>
    explicit Register(
        const std::string_view prefix,
        std::array<std::string_view, N>&& array,
        const std::index_sequence<I...>& sequence);
    //! @brief Construct a new Register object.
    //! @tparam N - number of arguments
    //! @param prefix - prefix characters
    //! @param array - array of arguments to be registered
    template <std::size_t N>
    explicit Register(const std::string_view prefix, std::array<std::string_view, N>&& array) :
        Register(prefix, std::move(array), std::make_index_sequence<N>{})
    {
    }

    //! @brief Set help message.
    //! @param content - help message content
    //! @return reference of the Register object
    Register& help(const std::string_view content);
    //! @brief Set metavar message.
    //! @param content - metavar message content
    //! @return reference of the Register object
    Register& metavar(const std::string_view content);
    //! @brief Set default value.
    //! @tparam T - type of default value
    //! @param value - default value
    //! @return reference of the Register object
    template <typename T>
    Register& defaultVal(T&& value);
    //! @brief Set default value.
    //! @param value - default value
    //! @return reference of the Register object
    Register& defaultVal(const char* value);
    //! @brief Set implicit value.
    //! @param value - implicit value
    //! @return reference of the Register object
    Register& implicitVal(std::any value);
    //! @brief Set the argument property to be required.
    //! @return reference of the Register object
    Register& required();
    //! @brief Set the argument property to be appending.
    //! @return reference of the Register object
    Register& appending();
    //! @brief Set the argument property to be remaining.
    //! @return reference of the Register object
    Register& remaining();
    //! @brief The action of specific arguments.
    //! @tparam Func - type of callable function
    //! @tparam Args - type of bound arguments
    //! @param callable - callable function
    //! @param boundArgs - bound arguments
    //! @return reference of the Register object
    template <class Func, class... Args>
    auto action(Func&& callable, Args&&... boundArgs)
        -> std::enable_if_t<std::is_invocable_v<Func, Args..., const std::string&>, Register&>;
    //! @brief Set number of arguments.
    //! @param num - number of arguments
    //! @return reference of the Register object
    Register& argsNum(const std::size_t num);
    //! @brief Set minimum number and maximum number of arguments.
    //! @param numMin - minimum number
    //! @param numMax - maximum number
    //! @return reference of the Register object
    Register& argsNum(const std::size_t numMin, const std::size_t numMax);
    //! @brief Set number of arguments with pattern.
    //! @param pattern - argument pattern
    //! @return reference of the Register object
    Register& argsNum(const ArgsNumPattern pattern);
    //! @brief Consume arguments.
    //! @tparam Iterator - type of argument iterator
    //! @param start - start argument iterator
    //! @param end - end argument iterator
    //! @param argName - target argument name
    //! @return argument iterator between start and end
    template <typename Iterator>
    Iterator consume(Iterator start, Iterator end, const std::string_view argName = {});
    //! @brief Validate all arguments.
    void validate() const;
    //! @brief Get the inline usage.
    //! @return inline usage
    [[nodiscard]] std::string getInlineUsage() const;
    //! @brief Get the length of all arguments.
    //! @return length of all arguments
    [[nodiscard]] std::size_t getArgumentsLength() const;
    //! @brief The operator (!=) overloading of Register class.
    //! @tparam T - type of right-hand side
    //! @param rhs - right-hand side
    //! @return be not equal or equal
    template <typename T>
    bool operator!=(const T& rhs) const;
    //! @brief The operator (==) overloading of Register class.
    //! @tparam T - type of right-hand side
    //! @param rhs - right-hand side
    //! @return be equal or not equal
    template <typename T>
    bool operator==(const T& rhs) const;
    friend class Argument;

private:
    //! @brief Alias for function which has valued return.
    using ValuedAction = std::function<std::any(const std::string&)>;
    //! @brief Alias for function which has void return.
    using VoidAction = std::function<void(const std::string&)>;
    //! @brief All argument names.
    std::vector<std::string> names{};
    //! @brief Used argument name.
    std::string_view usedName{};
    //! @brief Help message content.
    std::string helpContent{};
    //! @brief Metavar message content.
    std::string metavarContent{};
    //! @brief Default value.
    std::any defaultValue{};
    //! @brief Default value content to be represented.
    std::string defaultValueRepresent{};
    //! @brief Implicit value.
    std::any implicitValue{};
    //! @brief All actions of arguments.
    std::variant<ValuedAction, VoidAction> actions{
        std::in_place_type<ValuedAction>,
        [](const std::string& value)
        {
            return value;
        }};
    //! @brief Values from all arguments.
    std::vector<std::any> values{};
    //! @brief Flag to indicate whether to accept optional like value.
    bool isAcceptOptionalLikeValue{false};
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
    std::string_view prefixChars{};

    //! @brief Indicate the range for the number of arguments.
    class ArgsNumRange
    {
    public:
        //! @brief Construct a new ArgsNumRange object.
        //! @param minimum - minimum of range
        //! @param maximum - maximum of range
        ArgsNumRange(const std::size_t minimum, const std::size_t maximum) : min(minimum), max(maximum)
        {
            if (minimum > maximum)
            {
                throw std::runtime_error("The range of number of arguments is invalid.");
            }
        }

        //! @brief Check whether the number of arguments is within the range.
        //! @param value - number of arguments
        //! @return be contain or not contain
        [[nodiscard]] bool isContain(const std::size_t value) const { return (value >= min) && (value <= max); }
        //! @brief Check whether the number of arguments is exact.
        //! @return be exact or not exact
        [[nodiscard]] bool isExact() const { return min == max; }
        //! @brief Check that the range's maximum is not greater than the type's maximum.
        //! @return be not greater or greater
        [[nodiscard]] bool isRightBounded() const { return max < std::numeric_limits<std::size_t>::max(); }
        //! @brief Get the minimum of the range.
        //! @return minimum of range
        [[nodiscard]] std::size_t getMin() const { return min; }
        //! @brief Get the maximum of the range.
        //! @return maximum of range
        [[nodiscard]] std::size_t getMax() const { return max; }
        //! @brief The operator (<<) overloading of the ArgsNumRange class.
        //! @param os - output stream object
        //! @param range - specific ArgsNumRange object
        //! @return reference of the output stream object
        friend std::ostream& operator<<(std::ostream& os, const ArgsNumRange& range)
        {
            if (range.min == range.max)
            {
                if ((0 != range.min) && (1 != range.min))
                {
                    os << "[args: " << range.min << "] ";
                }
            }
            else
            {
                if (std::numeric_limits<std::size_t>::max() == range.max)
                {
                    os << "[args: " << range.min << " or more] ";
                }
                else
                {
                    os << "[args=" << range.min << ".." << range.max << "] ";
                }
            }
            return os;
        }
        //! @brief The operator (==) overloading of Register class.
        //! @param rhs - right-hand side
        //! @return be equal or not equal
        bool operator==(const ArgsNumRange& rhs) const { return (rhs.min == min) && (rhs.max == max); }
        //! @brief The operator (!=) overloading of Register class.
        //! @param rhs - right-hand side
        //! @return be not equal or equal
        bool operator!=(const ArgsNumRange& rhs) const { return !(*this == rhs); }

        //! @brief Minimum of range.
        std::size_t min{0};
        //! @brief Maximum of range.
        std::size_t max{0};
    };
    //! @brief The range for the number of arguments.
    ArgsNumRange argsNumRange{1, 1};

    //! @brief Throw an exception when ArgsNumRange is invalid.
    [[noreturn]] void throwArgsNumRangeValidationException() const;
    //! @brief Throw an exception when the required argument is not used.
    [[noreturn]] void throwRequiredArgNotUsedException() const;
    //! @brief Throw an exception when the required argument has no value provided.
    [[noreturn]] void throwRequiredArgNoValueProvidedException() const;
    //! @brief Find the character in the argument.
    //! @param name - name of argument
    //! @return character
    static int lookAhead(const std::string_view name);
    //! @brief Check whether the argument is optional.
    //! @param name - name of argument
    //! @param prefix - prefix characters
    //! @return be optional or not optional
    static bool checkIfOptional(const std::string_view name, const std::string_view prefix);
    //! @brief Check whether the argument is positional.
    //! @param name - name of argument
    //! @param prefix - prefix characters
    //! @return be positional or not positional
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

protected:
    friend std::ostream& operator<<(std::ostream& os, const Register& reg);
    friend std::ostream& operator<<(std::ostream& os, const Argument& arg);
};

template <std::size_t N, std::size_t... I>
Register::Register(
    const std::string_view prefix,
    std::array<std::string_view, N>&& array,
    const std::index_sequence<I...>& /*sequence*/) :
    isAcceptOptionalLikeValue(false),
    isOptional((checkIfOptional(array[I], prefix) || ...)),
    isRequired(false),
    isRepeatable(false),
    isUsed(false),
    prefixChars(prefix)
{
    (static_cast<void>(names.emplace_back(array[I])), ...);
    std::sort(
        names.begin(),
        names.end(),
        [](const auto& lhs, const auto& rhs)
        {
            return (lhs.size() == rhs.size()) ? (lhs < rhs) : (lhs.size() < rhs.size());
        });
}

template <typename T>
Register& Register::defaultVal(T&& value)
{
    defaultValueRepresent = represent(value);
    defaultValue = std::forward<T>(value);
    return *this;
}

template <class Func, class... Args>
auto Register::action(Func&& callable, Args&&... boundArgs)
    -> std::enable_if_t<std::is_invocable_v<Func, Args..., const std::string&>, Register&>
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
        actions.emplace<ActionType>(
            [func = std::forward<Func>(callable),
             tup = std::make_tuple(std::forward<Args>(boundArgs)...)](const std::string& opt) mutable
            {
                return applyScopedOne(func, tup, opt);
            });
    }
    return *this;
}

template <typename Iterator>
Iterator Register::consume(Iterator start, Iterator end, const std::string_view argName)
{
    if (!isRepeatable && isUsed)
    {
        throw std::runtime_error("Duplicate argument.");
    }

    isUsed = true;
    usedName = argName;
    const auto numMax = argsNumRange.getMax(), numMin = argsNumRange.getMin();
    std::size_t dist = 0;
    if (0 == numMax)
    {
        values.emplace_back(implicitValue);
        std::visit(
            [](const auto& func)
            {
                func({});
            },
            actions);
        return start;
    }
    if ((dist = static_cast<std::size_t>(std::distance(start, end))) >= numMin)
    {
        if (numMax < dist)
        {
            end = std::next(start, static_cast<typename Iterator::difference_type>(numMax));
        }
        if (!isAcceptOptionalLikeValue)
        {
            end = std::find_if(start, end, std::bind(checkIfOptional, std::placeholders::_1, prefixChars));
            dist = static_cast<std::size_t>(std::distance(start, end));
            if (dist < numMin)
            {
                throw std::runtime_error("Too few arguments.");
            }
        }

        struct ActionApply
        {
            void operator()(const ValuedAction& func) const
            {
                std::transform(first, last, std::back_inserter(self.values), func);
            }
            void operator()(const VoidAction& func) const
            {
                std::for_each(first, last, func);
                if (!self.defaultValue.has_value())
                {
                    if (!self.isAcceptOptionalLikeValue)
                    {
                        self.values.resize(static_cast<std::size_t>(std::distance(first, last)));
                    }
                }
            }

            Iterator first{};
            Iterator last{};
            Register& self;
        };
        std::visit(ActionApply{start, end, *this}, actions);
        return end;
    }
    if (defaultValue.has_value())
    {
        return start;
    }
    throw std::runtime_error("Too few arguments for '" + std::string{usedName} + "'.");
}

template <typename T>
bool Register::operator!=(const T& rhs) const
{
    return !(*this == rhs);
}

template <typename T>
bool Register::operator==(const T& rhs) const
{
    if constexpr (!isContainer<T>)
    {
        return rhs == get<T>();
    }
    else
    {
        using ValueType = typename T::value_type;

        const auto& lhs = get<T>();
        return std::equal(
            std::begin(lhs),
            std::end(lhs),
            std::begin(rhs),
            std::end(rhs),
            [](const auto& lhs, const auto& rhs)
            {
                return rhs == std::any_cast<const ValueType&>(lhs);
            });
    }
}

template <typename T>
T Register::get() const
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
    if (defaultValue.has_value())
    {
        return std::any_cast<T>(defaultValue);
    }
    if constexpr (isContainer<T>)
    {
        if (!isAcceptOptionalLikeValue)
        {
            return anyCastContainer<T>(values);
        }
    }

    throw std::runtime_error("No value specified for '" + names.back() + "'.");
}

template <typename T>
std::optional<T> Register::present() const
{
    if (defaultValue.has_value())
    {
        throw std::runtime_error("Default value always presents.");
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
T Register::anyCastContainer(const std::vector<std::any>& operand)
{
    using ValueType = typename T::value_type;

    T result{};
    std::transform(
        std::begin(operand),
        std::end(operand),
        std::back_inserter(result),
        [](const auto& value)
        {
            return std::any_cast<ValueType>(value);
        });
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
        titleName(title), versionNumber(version), parserPath(title)
    {
    }
    //! @brief Destroy the Argument object.
    ~Argument() = default;
    //! @brief Construct a new Argument object.
    //! @param arg - the old object for copy constructor
    Argument(const Argument& arg);
    //! @brief Construct a new Argument object.
    Argument(Argument&&) noexcept = default;
    //! @brief The operator (=) overloading of Argument class.
    //! @param arg - the old object for copy assignment operator
    //! @return reference of the Argument object
    Argument& operator=(const Argument& arg);
    //! @brief The operator (=) overloading of Argument class.
    //! @return reference of the Argument object
    Argument& operator=(Argument&&) = default;
    //! @brief The operator (bool) overloading of Argument class.
    explicit operator bool() const;

    //! @brief Add a single argument.
    //! @tparam ArgsType - type of arguments
    //! @param fewArgs - argument name
    //! @return reference of the Register object
    template <typename... ArgsType>
    Register& addArgument(ArgsType... fewArgs);
    //! @brief Add a descrText.
    //! @param text - descrText text
    //! @return reference of the Register object
    Argument& addDescription(const std::string_view text);
    //! @brief Get the Register or Argument instance by name.
    //! @tparam T - type of instance
    //! @param name - instance name
    //! @return Register or Argument instance
    template <typename T = Register>
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
    //! @return be used or not used
    [[nodiscard]] bool isUsed(const std::string_view argName) const;
    //! @brief Check whether the sub-command is used.
    //! @param subCommandName - target sub-command name
    //! @return be used or not used
    [[nodiscard]] inline auto isSubCommandUsed(const std::string_view subCommandName) const;
    //! @brief Check whether the sub-command is used.
    //! @param subParser - target sub-parser
    //! @return be used or not used
    [[nodiscard]] inline auto isSubCommandUsed(const Argument& subParser) const;
    //! @brief The operator ([]) overloading of Register class.
    //! @param argName - target argument name
    //! @return reference of the Register object
    Register& operator[](const std::string_view argName) const;
    //! @brief Get the title name.
    //! @return title name
    std::string title() const;
    //! @brief Get the version number.
    //! @return version number
    std::string version() const;
    //! @brief Get the help message content.
    //! @return help message content
    [[nodiscard]] std::ostringstream help() const;
    //! @brief Get the usage content.
    //! @return usage content
    [[nodiscard]] std::string usage() const;
    //! @brief Add a sub-parser.
    //! @param parser - sub-parser
    void addSubParser(Argument& parser);

private:
    //! @brief Title name.
    std::string titleName{};
    //! @brief Version number.
    std::string versionNumber{};

    //! @brief Alias for iterator in all Register instance.
    using RegisterIter = std::list<Register>::iterator;
    //! @brief Alias for iterator in all Argument instance.
    using ArgumentIter = std::list<std::reference_wrapper<Argument>>::iterator;
    //! @brief Description text.
    std::string descrText{};
    //! @brief Prefix characters.
    std::string prefixChars{"-"};
    //! @brief Assign characters.
    std::string assignChars{"="};
    //! @brief Flag to indicate whether to be parsed.
    bool isParsed{false};
    //! @brief List of optional arguments.
    std::list<Register> optionalArguments{};
    //! @brief List of positional arguments.
    std::list<Register> positionalArguments{};
    //! @brief Mapping table of argument.
    std::unordered_map<std::string_view, RegisterIter> argumentMap{};
    //! @brief Current parser path.
    std::string parserPath{};
    //! @brief List of sub-parsers.
    std::list<std::reference_wrapper<Argument>> subParsers{};
    //! @brief Mapping table of sub-parser.
    std::map<std::string_view, ArgumentIter> subParserMap{};
    //! @brief Mapping table of sub-parser usage.
    std::map<std::string_view, bool> subParserUsed{};

    //! @brief Check whether the prefix character is valid.
    //! @param c - prefix character
    //! @return be valid or invalid
    [[nodiscard]] bool isValidPrefixChar(const char c) const;
    //! @brief Get any valid prefix character.
    //! @return valid prefix character
    [[nodiscard]] char getAnyValidPrefixChar() const;
    //! @brief Preprocess all raw arguments.
    //! @param rawArguments - container of all raw arguments
    //! @return preprocessed argument container
    [[nodiscard]] std::vector<std::string> preprocessArguments(const std::vector<std::string>& rawArguments) const;
    //! @brief Parse all input arguments for internal.
    //! @param rawArguments - container of all raw arguments
    void parseArgsInternal(const std::vector<std::string>& rawArguments);
    //! @brief Get the length of the longest argument.
    //! @return length of the longest argument
    [[nodiscard]] std::size_t getLengthOfLongestArgument() const;
    //! @brief Make index for argumentMap.
    //! @param iterator - iterator in all argument registers
    void indexArgument(const RegisterIter& iterator);

protected:
    friend std::ostream& operator<<(std::ostream& os, const Argument& arg);
};

template <typename... ArgsType>
Register& Argument::addArgument(ArgsType... fewArgs)
{
    using ArrayOfSv = std::array<std::string_view, sizeof...(ArgsType)>;

    const auto argument = optionalArguments.emplace(std::cend(optionalArguments), prefixChars, ArrayOfSv{fewArgs...});
    if (!argument->isOptional)
    {
        positionalArguments.splice(std::cend(positionalArguments), optionalArguments, argument);
    }

    indexArgument(argument);
    return *argument;
}

template <typename T>
T& Argument::at(const std::string_view name)
{
    if constexpr (std::is_same_v<T, Register>)
    {
        return (*this)[name];
    }
    else
    {
        const auto subParserIter = subParserMap.find(name);
        if (subParserMap.cend() != subParserIter)
        {
            return subParserIter->second->get();
        }
        throw std::runtime_error("No such sub-parser: " + std::string{name} + '.');
    }
}

template <typename T>
T Argument::get(const std::string_view argName) const
{
    if (!isParsed)
    {
        throw std::runtime_error("Nothing parsed, no arguments are available.");
    }
    return (*this)[argName].get<T>();
}

template <typename T>
std::optional<T> Argument::present(const std::string_view argName) const
{
    return (*this)[argName].present<T>();
}

inline auto Argument::isSubCommandUsed(const std::string_view subCommandName) const
{
    return subParserUsed.at(subCommandName);
}

inline auto Argument::isSubCommandUsed(const Argument& subParser) const
{
    return isSubCommandUsed(subParser.titleName);
}
} // namespace argument
} // namespace utility
