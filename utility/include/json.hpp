//! @file json.hpp
//! @author ryftchen
//! @brief The declarations (json) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2026 ryftchen. All rights reserved.

#pragma once

#include <deque>
#include <map>
#include <memory>
#include <variant>

//! @brief The utility module.
namespace utility // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief JSON-related functions in the utility module.
namespace json
{
//! @brief Brief function description.
//! @return function description (module_function)
inline static const char* description() noexcept
{
    return "UTIL_JSON";
}
extern const char* version() noexcept;

//! @brief Javascript object notation.
class JSON
{
public:
    //! @brief Enumerate specific data types.
    enum class Type : std::uint8_t
    {
        //! @brief Null.
        null,
        //! @brief Object.
        object,
        //! @brief Array.
        array,
        //! @brief String.
        string,
        //! @brief Floating.
        floating,
        //! @brief Integral.
        integral,
        //! @brief Boolean.
        boolean
    };
    //! @brief Alias for the JSON null.
    using Null = std::nullptr_t;
    //! @brief Alias for the JSON object.
    using Object = std::map<std::string, JSON>;
    //! @brief Alias for the JSON array.
    using Array = std::deque<JSON>;
    //! @brief Alias for the JSON string.
    using String = std::string;
    //! @brief Alias for the JSON number (floating).
    using Floating = double;
    //! @brief Alias for the JSON number (integral).
    using Integral = long long;
    //! @brief Alias for the JSON boolean.
    using Boolean = bool;

    //! @brief Construct a new JSON object.
    JSON() = default;
    //! @brief Construct a new JSON object.
    //! @param type - data type
    explicit JSON(const Type type);
    //! @brief Construct a new JSON object.
    //! @param list - initializer list for JSON
    JSON(const std::initializer_list<JSON>& list);
    //! @brief Destroy the JSON object.
    virtual ~JSON() = default;
    //! @brief Construct a new JSON object.
    JSON(const JSON&) = default;
    //! @brief Construct a new JSON object.
    //! @param json - object for move constructor
    JSON(JSON&& json) noexcept;
    //! @brief The operator (=) overloading of JSON class.
    //! @return reference of the JSON object
    JSON& operator=(const JSON&) = default;
    //! @brief The operator (=) overloading of JSON class.
    //! @param json - object for move assignment operator
    //! @return reference of the JSON object
    JSON& operator=(JSON&& json) noexcept;

    // NOLINTBEGIN(google-explicit-constructor)
    //! @brief Construct a new JSON object.
    JSON(const std::nullptr_t /*null*/) {}
    //! @brief Construct a new JSON object.
    //! @tparam Value - type of string value
    //! @param s - string value
    template <typename Value>
    JSON(const Value s, std::enable_if_t<std::is_convertible_v<Value, std::string>>* /*sfinae*/ = nullptr) :
        data{String{s}}
    {
    }
    //! @brief Construct a new JSON object.
    //! @tparam Value - type of floating value
    //! @param f - floating value
    template <typename Value>
    JSON(const Value f, std::enable_if_t<std::is_floating_point_v<Value>>* /*sfinae*/ = nullptr) :
        data{static_cast<Floating>(f)}
    {
    }
    //! @brief Construct a new JSON object.
    //! @tparam Value - type of integral value
    //! @param i - integral value
    template <typename Value>
    JSON(
        const Value i,
        std::enable_if_t<std::is_integral_v<Value> && !std::is_same_v<Value, bool>>* /*sfinae*/ = nullptr) :
        data{static_cast<Integral>(i)}
    {
    }
    //! @brief Construct a new JSON object.
    //! @tparam Value - type of boolean value
    //! @param b - boolean value
    template <typename Value>
    JSON(const Value b, std::enable_if_t<std::is_same_v<Value, bool>>* /*sfinae*/ = nullptr) :
        data{static_cast<Boolean>(b)}
    {
    }
    // NOLINTEND(google-explicit-constructor)
    // NOLINTBEGIN(misc-unconventional-assign-operator)
    //! @brief The operator (=) overloading of JSON class.
    //! @tparam Value - type of string value
    //! @param s - string value
    //! @return reference of the JSON object
    template <typename Value>
    std::enable_if_t<std::is_convertible_v<Value, std::string>, JSON&> operator=(const Value s);
    //! @brief The operator (=) overloading of JSON class.
    //! @tparam Value - type of floating value
    //! @param f - floating value
    //! @return reference of the JSON object
    template <typename Value>
    std::enable_if_t<std::is_floating_point_v<Value>, JSON&> operator=(const Value f);
    //! @brief The operator (=) overloading of JSON class.
    //! @tparam Value - type of integral value
    //! @param i - integral value
    //! @return reference of the JSON object
    template <typename Value>
    std::enable_if_t<std::is_integral_v<Value> && !std::is_same_v<Value, bool>, JSON&> operator=(const Value i);
    //! @brief The operator (=) overloading of JSON class.
    //! @tparam Value - type of boolean value
    //! @param b - boolean value
    //! @return reference of the JSON object
    template <typename Value>
    std::enable_if_t<std::is_same_v<Value, bool>, JSON&> operator=(const Value b);
    // NOLINTEND(misc-unconventional-assign-operator)

    //! @brief JSON wrapper.
    //! @tparam Container - type of container
    template <typename Container>
    class JSONWrapper
    {
    public:
        //! @brief Construct a new JSONWrapper object.
        //! @param container - JSON object to be wrapped
        explicit JSONWrapper(Container* container) : object{container} {}
        //! @brief Construct a new JSONWrapper object.
        explicit JSONWrapper(std::nullptr_t /*null*/) {}

        //! @brief Pointer to the JSON object.
        Container* object{nullptr};
        //! @brief Get the first iterator.
        //! @return first iterator
        typename Container::iterator begin() { return object ? object->begin() : typename Container::iterator(); }
        //! @brief Get the last iterator.
        //! @return last iterator
        typename Container::iterator end() { return object ? object->end() : typename Container::iterator(); }
        //! @brief Get the first const iterator.
        //! @return first const iterator
        typename Container::const_iterator begin() const
        {
            return object ? object->begin() : typename Container::iterator();
        }
        //! @brief Get the last const iterator.
        //! @return last const iterator
        typename Container::const_iterator end() const
        {
            return object ? object->end() : typename Container::iterator();
        }
    };

    //! @brief JSON wrapper.
    //! @tparam Container - type of container
    template <typename Container>
    class JSONConstWrapper
    {
    public:
        //! @brief Construct a new JSONWrapper object.
        //! @param container - JSON object to be wrapped
        explicit JSONConstWrapper(const Container* container) : object{container} {}
        //! @brief Construct a new JSONWrapper object.
        explicit JSONConstWrapper(const std::nullptr_t /*null*/) {}

        //! @brief Const pointer to the JSON object.
        const Container* object{nullptr};
        //! @brief Get the first const iterator.
        //! @return first const iterator
        typename Container::const_iterator begin() const
        {
            return object ? object->begin() : typename Container::const_iterator();
        }
        //! @brief Get the last const iterator.
        //! @return last const iterator
        typename Container::const_iterator end() const
        {
            return object ? object->end() : typename Container::const_iterator();
        }
    };

    //! @brief Make JSON object by data type.
    //! @param type - data type
    //! @return JSON object
    static JSON make(const Type type);
    //! @brief Load JSON object from string.
    //! @param fmt - formatted string to be parsed
    //! @return JSON object
    static JSON load(const std::string_view fmt);
    //! @brief Append item to array. Convert to array type.
    //! @tparam Item - type of item
    //! @param item - item to append
    template <typename Item>
    void append(const Item item);
    //! @brief Append multiple items to array. Convert to array type.
    //! @tparam I0 - type of first item
    //! @tparam Is - type of multiple items
    //! @param item0 - first item to append
    //! @param items - multiple items to append
    template <typename I0, typename... Is>
    void append(const I0 item0, const Is... items);
    //! @brief The operator ([]) overloading of JSON class.
    //! @param key - target key
    //! @return reference of the JSON object
    JSON& operator[](const std::string& key);
    //! @brief The operator ([]) overloading of JSON class.
    //! @param index - target index
    //! @return reference of the JSON object
    JSON& operator[](std::size_t index);
    //! @brief Get the JSON object by key.
    //! @param key - target key
    //! @return reference of the JSON object
    JSON& at(const std::string& key);
    //! @brief Get the JSON object by key.
    //! @param key - target key
    //! @return const reference of the JSON object
    [[nodiscard]] const JSON& at(const std::string& key) const;
    //! @brief Get the JSON object by index.
    //! @param index - target index
    //! @return reference of the JSON object
    JSON& at(std::size_t index);
    //! @brief Get the JSON object by index.
    //! @param index - target index
    //! @return const reference of the JSON object
    [[nodiscard]] const JSON& at(std::size_t index) const;
    //! @brief Get the length of the array.
    //! @return number of items stored in the array, -1 if type is not array
    [[nodiscard]] int length() const;
    //! @brief Get the size of the array or object.
    //! @return number of items stored in the array or object, -1 if type is neither array nor object
    [[nodiscard]] int size() const;
    //! @brief Check whether the key exists.
    //! @param key - target key
    //! @return exist or not
    [[nodiscard]] bool hasKey(const std::string& key) const;
    //! @brief Check whether the type is null.
    //! @return be null type or not
    [[nodiscard]] bool isNullType() const;
    //! @brief Check whether the type is object.
    //! @return be object type or not
    [[nodiscard]] bool isObjectType() const;
    //! @brief Check whether the type is array.
    //! @return be array type or not
    [[nodiscard]] bool isArrayType() const;
    //! @brief Check whether the type is string.
    //! @return be string type or not
    [[nodiscard]] bool isStringType() const;
    //! @brief Check whether the type is floating.
    //! @return be floating type or not
    [[nodiscard]] bool isFloatingType() const;
    //! @brief Check whether the type is integral.
    //! @return be integral type or not
    [[nodiscard]] bool isIntegralType() const;
    //! @brief Check whether the type is boolean.
    //! @return be boolean type or not
    [[nodiscard]] bool isBooleanType() const;
    //! @brief Convert to string value.
    //! @return string value
    [[nodiscard]] String asString() const;
    //! @brief Convert to unescaped string value.
    //! @return unescaped string value
    [[nodiscard]] String asUnescapedString() const;
    //! @brief Convert to floating value.
    //! @return floating value
    [[nodiscard]] Floating asFloating() const;
    //! @brief Convert to integral value.
    //! @return integral value
    [[nodiscard]] Integral asIntegral() const;
    //! @brief Convert to boolean value.
    //! @return boolean value
    [[nodiscard]] Boolean asBoolean() const;
    //! @brief Get the wrapper of the object range.
    //! @return wrapper of the object range
    JSONWrapper<Object> objectRange();
    //! @brief Get the wrapper of the array range.
    //! @return wrapper of the array range
    JSONWrapper<Array> arrayRange();
    //! @brief Get the wrapper of the object range.
    //! @return wrapper of the object range
    [[nodiscard]] JSONConstWrapper<Object> objectRange() const;
    //! @brief Get the wrapper of the array range.
    //! @return wrapper of the array range
    [[nodiscard]] JSONConstWrapper<Array> arrayRange() const;
    //! @brief Dump as formatted string.
    //! @param depth - target depth
    //! @param tab - tab string
    //! @return formatted string
    [[nodiscard]] std::string dump(const std::uint32_t depth = 1, const std::string_view tab = "  ") const;
    //! @brief Dump as minified formatted string.
    //! @return minified formatted string
    [[nodiscard]] std::string dumpMinified() const;

private:
    //! @brief The data that stores JSON information.
    struct Data
    {
        //! @brief Construct a new  Data object.
        Data() : value{nullptr} {}
        //! @brief Construct a new  Data object.
        //! @param s - string value
        explicit Data(const String& s) : value{std::make_shared<String>(s)} {}
        //! @brief Construct a new  Data object.
        //! @param f - floating value
        explicit Data(const Floating f) : value{f} {}
        //! @brief Construct a new  Data object.
        //! @param i - integral value
        explicit Data(const Integral i) : value{i} {}
        //! @brief Construct a new  Data object.
        //! @param b - boolean value
        explicit Data(const Boolean b) : value{b} {}

        //! @brief Alias for the pointer of Object. Non-fundamental type.
        using ObjectPtr = std::shared_ptr<Object>;
        //! @brief Alias for the pointer of Array. Non-fundamental type.
        using ArrayPtr = std::shared_ptr<Array>;
        //! @brief Alias for the pointer of String. Non-fundamental type.
        using StringPtr = std::shared_ptr<String>;
        //! @brief Alias for the data's value type.
        using ValueType =
            std::variant<std::monostate, Null, ObjectPtr, ArrayPtr, StringPtr, Floating, Integral, Boolean>;
        //! @brief Value of the data.
        ValueType value;
    } /** @brief JSON valid data. */ data;

    //! @brief Ensure the target type has been set.
    //! @tparam Type - type of data
    template <typename Type>
    void ensureType();
    //! @brief Check whether it holds data.
    //! @tparam Type - type of data
    //! @return holds or not
    template <typename Type>
    [[nodiscard]] bool holdsData() const;
    //! @brief Get the data.
    //! @tparam Type - type of data
    //! @return data value
    template <typename Type>
    const auto& getData() const;

    //! @brief Data's value object's helper type for the visitor.
    //! @tparam Ts - type of visitors
    template <typename... Ts>
    struct ValueVisitor : public Ts...
    {
        using Ts::operator()...;
    };

protected:
    friend std::ostream& operator<<(std::ostream& os, const JSON& json);
};

template <typename Type>
const auto& JSON::getData() const
{
    if constexpr (std::is_same_v<Type, Object>)
    {
        return std::get<Data::ObjectPtr>(data.value);
    }
    else if constexpr (std::is_same_v<Type, Array>)
    {
        return std::get<Data::ArrayPtr>(data.value);
    }
    else if constexpr (std::is_same_v<Type, String>)
    {
        return std::get<Data::StringPtr>(data.value);
    }
    else
    {
        return std::get<Type>(data.value);
    }
}

// NOLINTBEGIN(misc-unconventional-assign-operator)
template <typename Value>
std::enable_if_t<std::is_convertible_v<Value, std::string>, JSON&> JSON::operator=(const Value s)
{
    ensureType<String>();
    *getData<String>() = String{s};
    return *this;
}

template <typename Value>
std::enable_if_t<std::is_floating_point_v<Value>, JSON&> JSON::operator=(const Value f)
{
    ensureType<Floating>();
    data.value = static_cast<Floating>(f);
    return *this;
}

template <typename Value>
std::enable_if_t<std::is_integral_v<Value> && !std::is_same_v<Value, bool>, JSON&> JSON::operator=(const Value i)
{
    ensureType<Integral>();
    data.value = static_cast<Integral>(i);
    return *this;
}

template <typename Value>
std::enable_if_t<std::is_same_v<Value, bool>, JSON&> JSON::operator=(const Value b)
{
    ensureType<Boolean>();
    data.value = static_cast<Boolean>(b);
    return *this;
}
// NOLINTEND(misc-unconventional-assign-operator)

template <typename Item>
void JSON::append(const Item item)
{
    ensureType<Array>();
    getData<Array>()->emplace_back(item);
}

template <typename I0, typename... Is>
void JSON::append(const I0 item0, const Is... items)
{
    append(item0);
    append(items...);
}

template <typename Type>
void JSON::ensureType()
{
    if (holdsData<Type>())
    {
        return;
    }

    if constexpr (std::is_same_v<Type, Null>)
    {
        data = Data{};
    }
    else if constexpr (std::is_same_v<Type, Object>)
    {
        data.value = std::make_shared<Object>();
    }
    else if constexpr (std::is_same_v<Type, Array>)
    {
        data.value = std::make_shared<Array>();
    }
    else if constexpr (std::is_same_v<Type, String>)
    {
        data.value = std::make_shared<String>();
    }
    else if constexpr (std::is_same_v<Type, Floating>)
    {
        data.value = static_cast<Floating>(0.0);
    }
    else if constexpr (std::is_same_v<Type, Integral>)
    {
        data.value = static_cast<Integral>(0);
    }
    else if constexpr (std::is_same_v<Type, Boolean>)
    {
        data.value = static_cast<Boolean>(false);
    }
}

template <typename Type>
bool JSON::holdsData() const
{
    if constexpr (std::is_same_v<Type, Object>)
    {
        return std::holds_alternative<Data::ObjectPtr>(data.value);
    }
    else if constexpr (std::is_same_v<Type, Array>)
    {
        return std::holds_alternative<Data::ArrayPtr>(data.value);
    }
    else if constexpr (std::is_same_v<Type, String>)
    {
        return std::holds_alternative<Data::StringPtr>(data.value);
    }
    else
    {
        return std::holds_alternative<Type>(data.value);
    }
}

extern JSON array();
extern JSON object();
} // namespace json
} // namespace utility
