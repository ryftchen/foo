//! @file argument.hpp
//! @author ryftchen
//! @brief The declarations (argument) in the utility module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

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
//! @brief Confirm container traits(std::string). Value is false.
template <>
struct HasContainerTraits<std::string> : std::false_type
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

//! @brief Max container size for representing.
constexpr std::size_t representMaxContainerSize = 5;

//! @brief Represent target value.
//! @tparam T - type of target value
//! @param val - target value
//! @return content to be represented
template <typename T>
std::string represent(T const& val)
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
        out << "{";
        const auto size = val.size();
        if (size > 1)
        {
            out << represent(*val.begin());
            std::for_each(
                std::next(val.begin()),
                std::next(val.begin(), std::min<std::size_t>(size, representMaxContainerSize) - 1),
                [&out](const auto& v)
                {
                    out << " " << represent(v);
                });
            if (size <= representMaxContainerSize)
            {
                out << " ";
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
        out << "}";
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
    std::index_sequence<I...> /*unused*/)
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

//! @brief Enumerate specific argument patterns.
enum class NArgsPattern : uint8_t
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
    //! @tparam I - number of sequences related to arguments
    //! @param array - array of arguments to be registered
    template <std::size_t N, std::size_t... I>
    explicit ArgumentRegister(std::array<std::string_view, N>&& array, std::index_sequence<I...> /*unused*/);
    //! @brief Construct a new ArgumentRegister object.
    //! @tparam N - number of arguments
    //! @param array - array of arguments to be registered
    template <std::size_t N>
    explicit ArgumentRegister(std::array<std::string_view, N>&& array) :
        ArgumentRegister(std::move(array), std::make_index_sequence<N>{}){};

    //! @brief Set help message.
    //! @param str - help message content
    //! @return reference of ArgumentRegister object
    ArgumentRegister& help(std::string str);
    //! @brief Set default value.
    //! @tparam T - type of default value
    //! @param value - default value
    //! @return reference of ArgumentRegister object
    template <typename T>
    ArgumentRegister& defaultValue(T&& value);
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
        -> std::enable_if_t<std::is_invocable_v<Function, Args..., std::string const>, ArgumentRegister&>;
    //! @brief Set number of arguments.
    //! @param numArgs - number of arguments
    //! @return reference of ArgumentRegister object
    ArgumentRegister& nArgs(std::size_t numArgs);
    //! @brief Set minimum number and maximum number of arguments.
    //! @param numArgsMin - minimum number
    //! @param numArgsMax - maximum number
    //! @return reference of ArgumentRegister object
    ArgumentRegister& nArgs(std::size_t numArgsMin, std::size_t numArgsMax);
    //! @brief Set number of arguments with pattern.
    //! @param pattern - argument pattern
    //! @return reference of ArgumentRegister object
    ArgumentRegister& nArgs(NArgsPattern pattern);
    //! @brief Consume arguments.
    //! @tparam Iterator - type of argument iterator
    //! @param start - start argument iterator
    //! @param end - end argument iterator
    //! @param usedName - name of argument
    //! @return argument iterator between start and end
    template <typename Iterator>
    Iterator consume(Iterator start, Iterator end, const std::string_view usedName = {});
    //! @brief Validate all arguments.
    void validate() const;
    //! @brief Get the length of all arguments.
    //! @return the length of all arguments
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
    friend auto operator<<(std::ostream& os, const Argument& parser) -> std::ostream&;
    friend std::ostream& operator<<(std::ostream& os, const ArgumentRegister& argument);

private:
    //! @brief Alias for function which has valued return.
    using ValuedAction = std::function<std::any(const std::string&)>;
    //! @brief Alias for function which has void return.
    using VoidAction = std::function<void(const std::string&)>;
    //! @brief All argument names.
    std::vector<std::string> names;
    //! @brief Used argument name.
    std::string_view usedNameStr;
    //! @brief Help message content.
    std::string helpStr;
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
        NArgsRange(std::size_t minimum, std::size_t maximum) : min(minimum), max(maximum)
        {
            if (minimum > maximum)
            {
                throw std::logic_error("<ARGUMENT> The range of number of arguments is invalid.");
            }
        }

        //! @brief Check if the number of arguments is within the range.
        //! @param value - number of arguments
        //! @return be contain or not contain
        [[nodiscard]] bool isContain(std::size_t value) const { return (value >= min) && (value <= max); }
        //! @brief Check if the number of arguments is exact.
        //! @return be exact or not exact
        [[nodiscard]] bool isExact() const { return (min == max); }
        //! @brief Check that the range's maximum is not greater than the type's maximum.
        //! @return be not greater or greater
        [[nodiscard]] bool isRightBounded() const { return max < std::numeric_limits<std::size_t>::max(); }
        //! @brief Get the minimum of the range.
        //! @return minimum of range
        [[nodiscard]] std::size_t getMin() const { return min; }
        //! @brief Get the maximum of the range.
        //! @return maximum of range
        [[nodiscard]] std::size_t getMax() const { return max; }
    };
    //! @brief The range for the number of arguments.
    NArgsRange argNumArgsRange{1, 1};

    //! @brief Throw an exception when NargsRange is invalid.
    void throwNArgsRangeValidationException() const;
    //! @brief Throw an exception when the required argument is not used.
    void throwRequiredArgNotUsedException() const;
    //! @brief Throw an exception when the required argument has no value provided.
    void throwRequiredArgNoValueProvidedException() const;
    //! @brief Find the character in the argument.
    //! @param str - name of argument
    //! @return character
    static auto lookAhead(const std::string_view str) -> int;
    //! @brief Check if the argument is optional.
    //! @param name - name of argument
    //! @return be optional or not optional
    static bool checkIfOptional(std::string_view name);
    //! @brief Check if the argument is non-optional.
    //! @param name - name of argument
    //! @return be non-optional or not non-optional
    static bool checkIfNonOptional(std::string_view name);
    //! @brief Get the member.
    //! @tparam T - type of member to be got
    //! @return member corresponding to the specific type
    template <typename T>
    T get() const;
    //! @brief Convert the container type.
    //! @tparam T - type of container
    //! @param operand - container to be converted
    //! @return container after converting type
    template <typename T>
    static auto anyCastContainer(const std::vector<std::any>& operand) -> T;
};

template <std::size_t N, std::size_t... I>
ArgumentRegister::ArgumentRegister(std::array<std::string_view, N>&& array, std::index_sequence<I...> /*unused*/) :
    isOptional((checkIfOptional(array[I]) || ...)), isRequired(false), isRepeatable(false), isUsed(false)
{
    ((void)names.emplace_back(array[I]), ...);
    std::sort(
        names.begin(),
        names.end(),
        [](const auto& lhs, const auto& rhs)
        {
            return (lhs.size() == rhs.size()) ? (lhs < rhs) : (lhs.size() < rhs.size());
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
    -> std::enable_if_t<std::is_invocable_v<Function, Args..., std::string const>, ArgumentRegister&>
{
    using ActionType = std::conditional_t<
        std::is_void_v<std::invoke_result_t<Function, Args..., std::string const>>,
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
             tup = std::make_tuple(std::forward<Args>(boundArgs)...)](std::string const& opt) mutable
            {
                return applyScopedOne(func, tup, opt);
            });
    }
    return *this;
}

template <typename Iterator>
Iterator ArgumentRegister::consume(Iterator start, Iterator end, const std::string_view usedName)
{
    if (!isRepeatable && isUsed)
    {
        throw std::runtime_error("<ARGUMENT> Duplicate argument.");
    }
    isUsed = true;
    usedNameStr = usedName;

    const auto numArgsMax = argNumArgsRange.getMax();
    const auto numArgsMin = argNumArgsRange.getMin();
    std::size_t dist = 0;
    if (!numArgsMax)
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
            end = std::next(start, numArgsMax);
        }
        if (!isAcceptOptionalLikeValue)
        {
            end = std::find_if(start, end, ArgumentRegister::checkIfOptional);
            dist = static_cast<std::size_t>(std::distance(start, end));
            if (dist < numArgsMin)
            {
                throw std::runtime_error("<ARGUMENT> Too few arguments.");
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
                        self.values.resize(std::distance(first, last));
                    }
                }
            }

            Iterator first, last;
            ArgumentRegister& self;
        };
        std::visit(ActionApply{start, end, *this}, actions);
        return end;
    }
    if (defaultValues.has_value())
    {
        return start;
    }
    throw std::runtime_error("<ARGUMENT> Too few arguments for '" + std::string(usedNameStr) + "'.");
}

template <typename T>
bool ArgumentRegister::operator!=(const T& rhs) const
{
    return !(rhs == *this);
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

    throw std::logic_error("<ARGUMENT> No value specified for '" + names.back() + "'.");
}

template <typename T>
auto ArgumentRegister::anyCastContainer(const std::vector<std::any>& operand) -> T
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
    explicit Argument(std::string title = {}, std::string version = "1.0") :
        title(std::move(title)), version(std::move(version)){};
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

    //! @brief Add a single argument.
    //! @tparam Args - type of argument
    //! @param fewArgs - argument name
    //! @return reference of ArgumentRegister object
    template <typename... Args>
    ArgumentRegister& addArgument(Args... fewArgs);
    //! @brief Parse all input arguments.
    //! @param arguments - vector of all arguments
    void parseArgs(const std::vector<std::string>& arguments);
    //! @brief Parse all input arguments.
    //! @param argc - argument count
    //! @param argv - argument vector
    void parseArgs(int argc, const char* const argv[]);
    //! @brief Get argument value by name.
    //! @tparam T - type of argument
    //! @param argName - target argument name
    //! @return argument value
    template <typename T = std::string>
    T get(const std::string_view argName) const;
    //! @brief Check if the argument is used.
    //! @param argName - target argument name
    //! @return be used or not used
    [[nodiscard]] bool isUsed(const std::string_view argName) const;
    //! @brief The operator ([]) overloading of ArgumentRegister class.
    //! @param argName - argument name
    //! @return reference of ArgumentRegister object
    ArgumentRegister& operator[](const std::string_view argName) const;
    //! @brief Get help message.
    //! @return Help message content.
    [[nodiscard]] auto help() const -> std::stringstream;
    //! @brief Title name.
    std::string title;
    //! @brief Version information.
    std::string version;

private:
    //! @brief Alias for iterator in all argument registers.
    using ListIterator = std::list<ArgumentRegister>::iterator;
    //! @brief Flag to indicate whether to be parsed.
    bool isParsed{false};
    //! @brief List of non-optional arguments.
    std::list<ArgumentRegister> nonOptionalArguments;
    //! @brief List of optional arguments.
    std::list<ArgumentRegister> optionalArguments;
    //! @brief Mapping table of argument.
    std::map<std::string_view, ListIterator, std::less<>> argumentMap;

    //! @brief Parse all input arguments for internal.
    //! @param arguments - vector of all arguments
    void parseArgsInternal(const std::vector<std::string>& arguments);
    //! @brief Get the length of the longest argument.
    //! @return length of the longest argument
    [[nodiscard]] std::size_t getLengthOfLongestArgument() const;
    //! @brief Make index for argumentMap.
    //! @param iterator - iterator in all argument registers
    void indexArgument(ListIterator iterator);

protected:
    friend auto operator<<(std::ostream& os, const Argument& parser) -> std::ostream&;
};

template <typename... Args>
ArgumentRegister& Argument::addArgument(Args... fewArgs)
{
    using ArrayOfSv = std::array<std::string_view, sizeof...(Args)>;
    auto argument = optionalArguments.emplace(std::cend(optionalArguments), ArrayOfSv{fewArgs...});

    if (!argument->isOptional)
    {
        nonOptionalArguments.splice(std::cend(nonOptionalArguments), optionalArguments, argument);
    }

    indexArgument(argument);
    return *argument;
}

template <typename T>
T Argument::get(const std::string_view argName) const
{
    if (!isParsed)
    {
        throw std::logic_error("<ARGUMENT> Nothing parsed, no arguments are available.");
    }
    return (*this)[argName].get<T>();
}
} // namespace utility::argument
