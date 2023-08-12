//! @file argument.hpp
//! @author ryftchen
//! @brief The declarations (argument) in the utility module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen. All rights reserved.

#pragma once

#include <algorithm>
#include <any>
#include <list>
#include <map>
#include <sstream>
#include <variant>

//! @brief Argument-parsing-related functions in the utility module.
namespace utility::argument
{
//! @brief Confirm container traits. Value is false.
//! @tparam T - type to be confirmed
//! @tparam typename - valid type or expression
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
//! @tparam typename - valid type or expression
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
        std::stringstream out;
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
                out << "...";
            }
        }
        if (size > 0)
        {
            out << represent(*std::prev(val.end()));
        }
        out << '}';
        return out.str();
    }
    else if constexpr (isStreamable<T>)
    {
        std::stringstream out;
        out << val;
        return out.str();
    }
    else
    {
        return " not representable ";
    }
}

//! @brief Implementation of wrapping function calls that have scope.
//! @tparam Function - type of callable function
//! @tparam Tuple - type of bound arguments tuple
//! @tparam Extra - type of extra option
//! @tparam I - number of sequence which converted from bound arguments tuple
//! @return wrapping of calls
template <class Function, class Tuple, class Extra, std::size_t... I>
constexpr decltype(auto) applyScopedOneImpl(
    Function&& func,
    Tuple&& tup,
    Extra&& ext,
    std::index_sequence<I...> /*sequence*/)
{
    return std::invoke(
        std::forward<Function>(func), std::get<I>(std::forward<Tuple>(tup))..., std::forward<Extra>(ext));
}

//! @brief Wrap function calls that have scope.
//! @tparam Function - type of callable function
//! @tparam Tuple - type of bound arguments tuple
//! @tparam Extra - type of extra option
//! @param func - callable function
//! @param tup - bound arguments tuple
//! @param ext - extra option
//! @return wrapping of calls
template <class Function, class Tuple, class Extra>
constexpr decltype(auto) applyScopedOne(Function&& func, Tuple&& tup, Extra&& ext)
{
    return applyScopedOneImpl(
        std::forward<Function>(func),
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
std::string join(StrIter first, StrIter last, const std::string& separator)
{
    if (first == last)
    {
        return "";
    }
    std::stringstream value;
    value << *first;
    ++first;
    while (first != last)
    {
        value << separator << *first;
        ++first;
    }
    return value.str();
}

//! @brief Enumerate specific argument patterns.
enum class NArgsPattern : std::uint8_t
{
    optional,
    any,
    atLeastOne
};

class Argument;

//! @brief Argument register.
class ArgumentRegister
{
public:
    //! @brief Construct a new ArgumentRegister object.
    //! @tparam N - number of arguments
    //! @tparam I - index of sequences related to arguments
    //! @param prefix - prefix characters
    //! @param array - array of arguments to be registered
    //! @param sequence - sequences related to arguments
    template <std::size_t N, std::size_t... I>
    explicit ArgumentRegister(
        std::string_view prefix,
        std::array<std::string_view, N>&& array,
        std::index_sequence<I...> sequence);
    //! @brief Construct a new ArgumentRegister object.
    //! @tparam N - number of arguments
    //! @param prefix - prefix characters
    //! @param array - array of arguments to be registered
    template <std::size_t N>
    explicit ArgumentRegister(std::string_view prefix, std::array<std::string_view, N>&& array) :
        ArgumentRegister(prefix, std::move(array), std::make_index_sequence<N>{}){};

    //! @brief Set help message.
    //! @param content - help message content
    //! @return reference of ArgumentRegister object
    ArgumentRegister& help(const std::string& content);
    //! @brief Set metavar message.
    //! @param content - metavar message content
    //! @return reference of ArgumentRegister object
    ArgumentRegister& metavar(const std::string& content);
    //! @brief Set default value.
    //! @tparam T - type of default value
    //! @param value - default value
    //! @return reference of ArgumentRegister object
    template <typename T>
    ArgumentRegister& defaultValue(T&& value);
    //! @brief Set default value.
    //! @param value - default value
    //! @return reference of ArgumentRegister object
    ArgumentRegister& defaultValue(const char* value);
    //! @brief Set implicit value
    //! @param value - implicit value
    //! @return reference of ArgumentRegister object
    ArgumentRegister& implicitValue(std::any value);
    //! @brief Set the argument property to be required.
    //! @return reference of ArgumentRegister object
    ArgumentRegister& required();
    //! @brief Set the argument property to be appending.
    //! @return reference of ArgumentRegister object
    ArgumentRegister& appending();
    //! @brief Set the argument property to be remaining.
    //! @return reference of ArgumentRegister object
    ArgumentRegister& remaining();
    //! @brief The action of specific arguments.
    //! @tparam Function - type of callable function
    //! @tparam Args - type of bound arguments
    //! @param callable - callable function
    //! @param boundArgs - bound arguments
    //! @return reference of ArgumentRegister object
    template <class Function, class... Args>
    auto action(Function&& callable, Args&&... boundArgs)
        -> std::enable_if_t<std::is_invocable_v<Function, Args..., const std::string>, ArgumentRegister&>;
    //! @brief Set number of arguments.
    //! @param numArgs - number of arguments
    //! @return reference of ArgumentRegister object
    ArgumentRegister& nArgs(const std::size_t numArgs);
    //! @brief Set minimum number and maximum number of arguments.
    //! @param numArgsMin - minimum number
    //! @param numArgsMax - maximum number
    //! @return reference of ArgumentRegister object
    ArgumentRegister& nArgs(const std::size_t numArgsMin, const std::size_t numArgsMax);
    //! @brief Set number of arguments with pattern.
    //! @param pattern - argument pattern
    //! @return reference of ArgumentRegister object
    ArgumentRegister& nArgs(const NArgsPattern pattern);
    //! @brief Consume arguments.
    //! @tparam Iterator - type of argument iterator
    //! @param start - start argument iterator
    //! @param end - end argument iterator
    //! @param argName - name of argument
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
    //! @brief The operator (!=) overloading of ArgumentRegister class.
    //! @tparam T - type of right-hand side
    //! @param rhs - right-hand side
    //! @return be not equal or equal
    template <typename T>
    bool operator!=(const T& rhs) const;
    //! @brief The operator (==) overloading of ArgumentRegister class.
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
    std::vector<std::string> names;
    //! @brief Used argument name.
    std::string_view usedName;
    //! @brief Help message content.
    std::string helpContent;
    //! @brief Metavar message content.
    std::string metavarContent;
    //! @brief Default values.
    std::any defaultValues;
    //! @brief Default value content to be represented.
    std::string defaultValueRepresent;
    //! @brief Implicit values.
    std::any implicitValues;
    //! @brief All actions of arguments.
    std::variant<ValuedAction, VoidAction> actions{
        std::in_place_type<ValuedAction>,
        [](const std::string& value)
        {
            return value;
        }};
    //! @brief Values from all arguments.
    std::vector<std::any> values;
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
    std::string_view prefixChars;

    //! @brief Indicate the range for the number of arguments.
    class NArgsRange
    {
        //! @brief Minimum of range.
        std::size_t min;
        //! @brief Maximum of range.
        std::size_t max;

    public:
        //! @brief Construct a new NArgsRange object
        //! @param minimum - minimum of range
        //! @param maximum - maximum of range
        NArgsRange(const std::size_t minimum, const std::size_t maximum) : min(minimum), max(maximum)
        {
            if (minimum > maximum)
            {
                throw std::logic_error("The range of number of arguments is invalid.");
            }
        }

        //! @brief Check if the number of arguments is within the range.
        //! @param value - number of arguments
        //! @return be contain or not contain
        [[nodiscard]] bool isContain(const std::size_t value) const { return (value >= min) && (value <= max); }
        //! @brief Check if the number of arguments is exact.
        //! @return be exact or not exact
        [[nodiscard]] bool isExact() const { return (min == max); }
        //! @brief Check that the range's maximum is not greater than the type's maximum.
        //! @return be not greater or greater
        [[nodiscard]] bool isRightBounded() const { return max < (std::numeric_limits<std::size_t>::max)(); }
        //! @brief Get the minimum of the range.
        //! @return minimum of range
        [[nodiscard]] std::size_t getMin() const { return min; }
        //! @brief Get the maximum of the range.
        //! @return maximum of range
        [[nodiscard]] std::size_t getMax() const { return max; }
        //! @brief The operator (<<) overloading of the NArgsRange class.
        //! @param os - output stream object
        //! @param range - specific NArgsRange object
        //! @return reference of output stream object
        friend std::ostream& operator<<(std::ostream& os, const NArgsRange& range)
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
                if ((std::numeric_limits<std::size_t>::max)() == range.max)
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

        bool operator==(const NArgsRange& rhs) const { return (rhs.min == min) && (rhs.max == max); }
        bool operator!=(const NArgsRange& rhs) const { return !(*this == rhs); }
    };
    //! @brief The range for the number of arguments.
    NArgsRange numArgsRange{1, 1};

    //! @brief Throw an exception when NargsRange is invalid.
    void throwNArgsRangeValidationException() const;
    //! @brief Throw an exception when the required argument is not used.
    void throwRequiredArgNotUsedException() const;
    //! @brief Throw an exception when the required argument has no value provided.
    void throwRequiredArgNoValueProvidedException() const;
    //! @brief Find the character in the argument.
    //! @param name - name of argument
    //! @return character
    static int lookAhead(const std::string_view name);
    //! @brief Check if the argument is optional.
    //! @param name - name of argument
    //! @param prefix - prefix characters
    //! @return be optional or not optional
    static bool checkIfOptional(std::string_view name, const std::string_view prefix);
    //! @brief Check if the argument is non-optional.
    //! @param name - name of argument
    //! @param prefix - prefix characters
    //! @return be non-optional or not non-optional
    static bool checkIfNonOptional(std::string_view name, const std::string_view prefix);
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
    friend std::ostream& operator<<(std::ostream& os, const ArgumentRegister& argReg);
    friend std::ostream& operator<<(std::ostream& os, const Argument& arg);
};

template <std::size_t N, std::size_t... I>
ArgumentRegister::ArgumentRegister(
    std::string_view prefix,
    std::array<std::string_view, N>&& array,
    std::index_sequence<I...> /*sequence*/) :
    isAcceptOptionalLikeValue(false),
    isOptional((checkIfOptional(array[I], prefix) || ...)),
    isRequired(false),
    isRepeatable(false),
    isUsed(false),
    prefixChars(prefix)
{
    ((void)names.emplace_back(array[I]), ...);
    std::sort(
        names.begin(),
        names.end(),
        [](const auto& lhs, const auto& rhs)
        {
            return (lhs.size() == rhs.size()) ? (lhs < rhs) : lhs.size() < rhs.size();
        });
}

template <typename T>
ArgumentRegister& ArgumentRegister::defaultValue(T&& value)
{
    defaultValueRepresent = represent(value);
    defaultValues = std::forward<T>(value);
    return *this;
}

template <class Function, class... Args>
auto ArgumentRegister::action(Function&& callable, Args&&... boundArgs)
    -> std::enable_if_t<std::is_invocable_v<Function, Args..., const std::string>, ArgumentRegister&>
{
    using ActionType = std::conditional_t<
        std::is_void_v<std::invoke_result_t<Function, Args..., const std::string>>,
        VoidAction,
        ValuedAction>;
    if constexpr (!sizeof...(Args))
    {
        actions.emplace<ActionType>(std::forward<Function>(callable));
    }
    else
    {
        actions.emplace<ActionType>(
            [func = std::forward<Function>(callable),
             tup = std::make_tuple(std::forward<Args>(boundArgs)...)](const std::string& opt) mutable
            {
                return applyScopedOne(func, tup, opt);
            });
    }
    return *this;
}

template <typename Iterator>
Iterator ArgumentRegister::consume(Iterator start, Iterator end, const std::string_view argName)
{
    if (!isRepeatable && isUsed)
    {
        throw std::runtime_error("Duplicate argument.");
    }

    isUsed = true;
    usedName = argName;
    const auto numArgsMax = numArgsRange.getMax();
    const auto numArgsMin = numArgsRange.getMin();
    std::size_t dist = 0;
    if (0 == numArgsMax)
    {
        values.emplace_back(implicitValues);
        std::visit(
            [](const auto& func)
            {
                func({});
            },
            actions);
        return start;
    }
    if ((dist = static_cast<std::size_t>(std::distance(start, end))) >= numArgsMin)
    {
        if (numArgsMax < dist)
        {
            end = std::next(start, static_cast<typename Iterator::difference_type>(numArgsMax));
        }
        if (!isAcceptOptionalLikeValue)
        {
            end = std::find_if(start, end, std::bind(checkIfOptional, std::placeholders::_1, prefixChars));
            dist = static_cast<std::size_t>(std::distance(start, end));
            if (dist < numArgsMin)
            {
                throw std::runtime_error("Too few arguments.");
            }
        }

        struct ActionApply
        {
            void operator()(ValuedAction& func) { std::transform(first, last, std::back_inserter(self.values), func); }
            void operator()(VoidAction& func)
            {
                std::for_each(first, last, func);
                if (!self.defaultValues.has_value())
                {
                    if (!self.isAcceptOptionalLikeValue)
                    {
                        self.values.resize(static_cast<std::size_t>(std::distance(first, last)));
                    }
                }
            }

            Iterator first;
            Iterator last;
            ArgumentRegister& self;
        };
        std::visit(ActionApply{start, end, *this}, actions);
        return end;
    }
    if (defaultValues.has_value())
    {
        return start;
    }
    throw std::runtime_error("Too few arguments for '" + std::string(usedName) + "'.");
}

template <typename T>
bool ArgumentRegister::operator!=(const T& rhs) const
{
    return !(*this == rhs);
}

template <typename T>
bool ArgumentRegister::operator==(const T& rhs) const
{
    if constexpr (!isContainer<T>)
    {
        return (rhs == get<T>());
    }
    else
    {
        using ValueType = typename T::value_type;
        auto lhs = get<T>();
        return std::equal(
            std::begin(lhs),
            std::end(lhs),
            std::begin(rhs),
            std::end(rhs),
            [](const auto& lhs, const auto& rhs)
            {
                return (rhs == std::any_cast<const ValueType&>(lhs));
            });
    }
}

template <typename T>
T ArgumentRegister::get() const
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
    if (defaultValues.has_value())
    {
        return std::any_cast<T>(defaultValues);
    }
    if constexpr (isContainer<T>)
    {
        if (!isAcceptOptionalLikeValue)
        {
            return anyCastContainer<T>(values);
        }
    }

    throw std::logic_error("No value specified for '" + names.back() + "'.");
}

template <typename T>
std::optional<T> ArgumentRegister::present() const
{
    if (defaultValues.has_value())
    {
        throw std::logic_error("Default value always presents.");
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
T ArgumentRegister::anyCastContainer(const std::vector<std::any>& operand)
{
    using ValueType = typename T::value_type;
    T result;
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
    //! @param version - version information
    explicit Argument(const std::string& title = {}, const std::string& version = "1.0") :
        title(title), version(version), parserPath(title){};
    //! @brief Destroy the Argument object.
    ~Argument() = default;
    //! @brief Construct a new Argument object.
    //! @param arg - the old object for copy constructor
    Argument(const Argument& arg);
    //! @brief Construct a new Argument object.
    Argument(Argument&&) noexcept = default;
    //! @brief The operator (=) overloading of Argument class.
    //! @param arg - the old object for copy assignment operator
    //! @return reference of Argument object
    Argument& operator=(const Argument& arg);
    //! @brief The operator (=) overloading of Argument class.
    //! @return reference of Argument object
    Argument& operator=(Argument&&) = default;
    //! @brief The operator (bool) overloading of Argument class.
    explicit operator bool() const;

    //! @brief Add a single argument.
    //! @tparam Args - type of argument
    //! @param fewArgs - argument name
    //! @return reference of ArgumentRegister object
    template <typename... TArgs>
    ArgumentRegister& addArgument(TArgs... fewArgs);
    //! @brief Add a parent argument.
    //! @tparam Args - type of argument
    //! @param fewArgs - argument name
    //! @return reference of ArgumentRegister object
    template <typename... TArgs>
    Argument& addParents(const TArgs&... fewArgs);
    //! @brief Add a descrText.
    //! @param text - descrText text
    //! @return reference of ArgumentRegister object
    Argument& addDescription(const std::string& text);
    //! @brief Get the ArgumentRegister or Argument instance by name.
    //! @tparam T - type of instance
    //! @param name - instance name
    //! @return ArgumentRegister or Argument instance
    template <typename T = ArgumentRegister>
    T& at(const std::string_view name);
    //! @brief Set the prefix characters
    //! @param prefix - prefix characters
    Argument& setPrefixChars(const std::string& prefix);
    //! @brief Set the assign characters
    //! @param assign - assign characters
    Argument& setAssignChars(const std::string& assign);
    //! @brief Parse all input arguments.
    //! @param arguments - container of all arguments
    void parseArgs(const std::vector<std::string>& arguments);
    //! @brief Parse all input arguments.
    //! @param argc - argument count
    //! @param argv - argument vector
    void parseArgs(const int argc, const char* const argv[]);
    //! @brief Parse only known arguments.
    //! @param arguments - container of all arguments
    //! @return container of remaining arguments
    std::vector<std::string> parseKnownArgs(const std::vector<std::string>& arguments);
    //! @brief Parse only known arguments.
    //! @param argc - argument count
    //! @param argv - argument vector
    //! @return container of remaining arguments
    std::vector<std::string> parseKnownArgs(const int argc, const char* const argv[]);
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
    //! @brief Check if the argument is used.
    //! @param argName - target argument name
    //! @return be used or not used
    [[nodiscard]] bool isUsed(const std::string_view argName) const;
    //! @brief Check if the sub-command is used.
    //! @param subCommandName - target sub-command name
    //! @return be used or not used
    [[nodiscard]] auto isSubCommandUsed(const std::string_view subCommandName) const;
    //! @brief Check if the sub-command is used.
    //! @param subParser - target sub-parser
    //! @return be used or not used
    [[nodiscard]] auto isSubCommandUsed(const Argument& subParser) const;
    //! @brief The operator ([]) overloading of ArgumentRegister class.
    //! @param argName - argument name
    //! @return reference of ArgumentRegister object
    ArgumentRegister& operator[](const std::string_view argName) const;
    //! @brief Get the help message content.
    //! @return help message content
    [[nodiscard]] std::stringstream help() const;
    //! @brief Get the usage content.
    //! @return usage content
    [[nodiscard]] std::string usage() const;
    //! @brief Add a sub-parser.
    //! @param parser - sub-parser
    void addSubParser(Argument& parser);

    //! @brief Title name.
    std::string title;
    //! @brief Version information.
    std::string version;

private:
    //! @brief Alias for iterator in all ArgumentRegister instance.
    using ArgumentRegisterIter = std::list<ArgumentRegister>::iterator;
    //! @brief Alias for iterator in all Argument instance.
    using ArgumentIter = std::list<std::reference_wrapper<Argument>>::iterator;
    //! @brief Description text.
    std::string descrText;
    //! @brief Prefix characters.
    std::string prefixChars{"-"};
    //! @brief Assign characters.
    std::string assignChars{"="};
    //! @brief Flag to indicate whether to be parsed.
    bool isParsed{false};
    //! @brief List of non-optional arguments.
    std::list<ArgumentRegister> nonOptionalArguments;
    //! @brief List of optional arguments.
    std::list<ArgumentRegister> optionalArguments;
    //! @brief Mapping table of argument.
    std::map<std::string_view, ArgumentRegisterIter> argumentMap;
    //! @brief Current parser path.
    std::string parserPath;
    //! @brief List of sub-parsers.
    std::list<std::reference_wrapper<Argument>> subParsers;
    //! @brief Mapping table of sub-parser.
    std::map<std::string_view, ArgumentIter> subParserMap;
    //! @brief Mapping table of sub-parser usage.
    std::map<std::string_view, bool> subParserUsed;

    //! @brief Check if the prefix character is valid.
    //! @param c - prefix character
    //! @return be valid or valid
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
    //! @brief Parse only known arguments for internal.
    //! @param rawArguments - container of all raw arguments
    //! @return container of remaining arguments
    std::vector<std::string> parseKnownArgsInternal(const std::vector<std::string>& rawArguments);
    //! @brief Get the length of the longest argument.
    //! @return length of the longest argument
    [[nodiscard]] std::size_t getLengthOfLongestArgument() const;
    //! @brief Make index for argumentMap.
    //! @param iterator - iterator in all argument registers
    void indexArgument(ArgumentRegisterIter iterator);

protected:
    friend std::ostream& operator<<(std::ostream& os, const Argument& arg);
};

template <typename... TArgs>
ArgumentRegister& Argument::addArgument(TArgs... fewArgs)
{
    using ArrayOfSv = std::array<std::string_view, sizeof...(TArgs)>;

    const auto argument = optionalArguments.emplace(std::cend(optionalArguments), prefixChars, ArrayOfSv{fewArgs...});
    if (!argument->isOptional)
    {
        nonOptionalArguments.splice(std::cend(nonOptionalArguments), optionalArguments, argument);
    }

    indexArgument(argument);
    return *argument;
}

template <typename... TArgs>
Argument& Argument::addParents(const TArgs&... fewArgs)
{
    for (const Argument& parentParser : {std::ref(fewArgs)...})
    {
        for (const auto& argument : parentParser.nonOptionalArguments)
        {
            const auto iterator = nonOptionalArguments.insert(std::cend(nonOptionalArguments), argument);
            indexArgument(iterator);
        }
        for (const auto& argument : parentParser.optionalArguments)
        {
            const auto iterator = optionalArguments.insert(std::cend(optionalArguments), argument);
            indexArgument(iterator);
        }
    }
    return *this;
}

template <typename T>
T& Argument::at(const std::string_view name)
{
    if constexpr (std::is_same_v<T, ArgumentRegister>)
    {
        return (*this)[name];
    }
    else
    {
        const auto subParserIter = subParserMap.find(name);
        if (subParserIter != subParserMap.end())
        {
            return subParserIter->second->get();
        }
        throw std::logic_error("No such sub-parser: " + std::string(name) + '.');
    }
}

template <typename T>
T Argument::get(const std::string_view argName) const
{
    if (!isParsed)
    {
        throw std::logic_error("Nothing parsed, no arguments are available.");
    }
    return (*this)[argName].get<T>();
}

template <typename T>
std::optional<T> Argument::present(const std::string_view argName) const
{
    return (*this)[argName].present<T>();
}
} // namespace utility::argument
