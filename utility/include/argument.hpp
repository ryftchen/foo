#pragma once

#include <algorithm>
#include <any>
#include <list>
#include <map>
#include <sstream>
#include <variant>

namespace utility::argument
{
template <typename T, typename = void>
struct HasContainerTraits : std::false_type
{
};

template <>
struct HasContainerTraits<std::string> : std::false_type
{
};

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

template <typename T>
static constexpr bool isContainer = HasContainerTraits<T>::value;

template <typename T, typename = void>
struct HasStreamableTraits : std::false_type
{
};

template <typename T>
struct HasStreamableTraits<T, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<T>())>>
    : std::true_type
{
};

template <typename T>
static constexpr bool isStreamable = HasStreamableTraits<T>::value;

constexpr std::size_t representMaxContainerSize = 5;

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

template <class Function, class Tuple, class Extra, std::size_t... I>
constexpr decltype(auto) applyPlusOneImpl(
    Function&& func,
    Tuple&& tup,
    Extra&& ext,
    std::index_sequence<I...> /*unused*/)
{
    return std::invoke(
        std::forward<Function>(func), std::get<I>(std::forward<Tuple>(tup))..., std::forward<Extra>(ext));
}

template <class Function, class Tuple, class Extra>
constexpr decltype(auto) applyPlusOne(Function&& func, Tuple&& tup, Extra&& ext)
{
    return applyPlusOneImpl(
        std::forward<Function>(func),
        std::forward<Tuple>(tup),
        std::forward<Extra>(ext),
        std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<Tuple>>>{});
}

enum class NArgsPattern
{
    optional,
    any,
    atLeastOne
};

class Argument;

class ArgumentRegister
{
public:
    template <std::size_t N, std::size_t... I>
    explicit ArgumentRegister(std::array<std::string_view, N>&& array, std::index_sequence<I...> /*unused*/);
    template <std::size_t N>
    explicit ArgumentRegister(std::array<std::string_view, N>&& array);

    ArgumentRegister& help(std::string str);
    template <typename T>
    ArgumentRegister& defaultValue(T&& value);
    ArgumentRegister& implicitValue(std::any value);
    ArgumentRegister& required();
    ArgumentRegister& appending();
    ArgumentRegister& remaining();
    template <class Function, class... Args>
    auto action(Function&& callable, Args&&... boundArgs)
        -> std::enable_if_t<std::is_invocable_v<Function, Args..., std::string const>, ArgumentRegister&>;
    ArgumentRegister& nArgs(std::size_t numArgs);
    ArgumentRegister& nArgs(std::size_t numArgsMin, std::size_t numArgsMax);
    ArgumentRegister& nArgs(NArgsPattern pattern);
    template <typename Iterator>
    Iterator consume(Iterator start, Iterator end, const std::string_view usedName = {});
    void validate() const;
    [[nodiscard]] std::size_t getArgumentsLength() const;
    template <typename T>
    bool operator!=(const T& rhs) const;
    template <typename T>
    bool operator==(const T& rhs) const;
    friend class Argument;
    friend auto operator<<(std::ostream& os, const Argument& parser) -> std::ostream&;
    friend std::ostream& operator<<(std::ostream& os, const ArgumentRegister& argument);

private:
    using ValuedAction = std::function<std::any(const std::string&)>;
    using VoidAction = std::function<void(const std::string&)>;
    std::vector<std::string> names;
    std::string_view usedNameStr;
    std::string helpStr;
    std::any defaultValues;
    std::string defaultValueRepresent;
    std::any implicitValues;
    std::variant<ValuedAction, VoidAction> actions{
        std::in_place_type<ValuedAction>,
        [](const std::string& value)
        {
            return value;
        }};
    std::vector<std::any> values;
    bool isAcceptOptionalLikeValue{false};
    bool isOptional{true};
    bool isRequired{true};
    bool isRepeatable{true};
    bool isUsed{true};
    static constexpr int eof{std::char_traits<char>::eof()};

    class NArgsRange
    {
        std::size_t min;
        std::size_t max;

    public:
        NArgsRange(std::size_t minimum, std::size_t maximum) : min(minimum), max(maximum)
        {
            if (minimum > maximum)
            {
                throw std::logic_error("argument: Range of number of arguments is invalid.");
            }
        }

        [[nodiscard]] bool contains(std::size_t value) const { return (value >= min) && (value <= max); }
        [[nodiscard]] bool isExact() const { return (min == max); }
        [[nodiscard]] bool isRightBounded() const { return max < std::numeric_limits<std::size_t>::max(); }
        [[nodiscard]] std::size_t getMin() const { return min; }
        [[nodiscard]] std::size_t getMax() const { return max; }
    };
    NArgsRange argNumArgsRange{1, 1};

    void throwNargsRangeValidationException() const;
    void throwRequiredArgNotUsedException() const;
    void throwRequiredArgNoValueProvidedException() const;
    static auto lookAhead(const std::string_view str) -> int;
    static bool checkIfOptional(std::string_view name);
    static bool checkIfNonOptional(std::string_view name);
    template <typename T>
    T get() const;
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

template <std::size_t N>
ArgumentRegister::ArgumentRegister(std::array<std::string_view, N>&& array) :
    ArgumentRegister(std::move(array), std::make_index_sequence<N>{})
{
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
                return applyPlusOne(func, tup, opt);
            });
    }
    return *this;
}

template <typename Iterator>
Iterator ArgumentRegister::consume(Iterator start, Iterator end, const std::string_view usedName)
{
    if (!isRepeatable && isUsed)
    {
        throw std::runtime_error("argument: Duplicate argument.");
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
                throw std::runtime_error("argument: Too few arguments.");
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
    throw std::runtime_error("argument: Too few arguments for '" + std::string(usedNameStr) + "'.");
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

    throw std::logic_error("argument: No value provided for '" + names.back() + "'.");
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

class Argument
{
public:
    explicit Argument(std::string title = {}, std::string version = "1.0");
    ~Argument() = default;
    Argument(const Argument& arg);
    Argument(Argument&&) noexcept = default;
    Argument& operator=(const Argument& arg);
    Argument& operator=(Argument&&) = default;

    template <typename... Args>
    ArgumentRegister& addArgument(Args... fewArgs);
    void parseArgs(const std::vector<std::string>& arguments);
    void parseArgs(int argc, const char* const argv[]);
    template <typename T = std::string>
    T get(const std::string_view argName) const;
    [[nodiscard]] bool isUsed(const std::string_view argName) const;
    ArgumentRegister& operator[](const std::string_view argName) const;
    [[nodiscard]] auto help() const -> std::stringstream;
    std::string title;
    std::string version;

private:
    using ListIterator = std::list<ArgumentRegister>::iterator;
    bool isParsed{false};
    std::list<ArgumentRegister> nonOptionalArguments;
    std::list<ArgumentRegister> optionalArguments;
    std::map<std::string_view, ListIterator, std::less<>> argumentMap;

    void parseArgsInternal(const std::vector<std::string>& arguments);
    [[nodiscard]] std::size_t getLengthOfLongestArgument() const;
    void indexArgument(ListIterator iterator);

protected:
    friend auto operator<<(std::ostream& stream, const Argument& parser) -> std::ostream&;
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
        throw std::logic_error("argument: Nothing parsed, no arguments are available.");
    }
    return (*this)[argName].get<T>();
}
} // namespace utility::argument
