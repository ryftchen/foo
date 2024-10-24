//! @file json.hpp
//! @author ryftchen
//! @brief The declarations (json) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#pragma once

#include <cstdint>
#include <deque>
#include <map>
#include <string>
#include <variant>

//! @brief The utility module.
namespace utility // NOLINT (modernize-concat-nested-namespaces)
{
//! @brief JSON-related functions in the utility module.
namespace json
{
extern const char* version() noexcept;

//! @brief Javascript object notation.
class JSON
{
public:
    //! @brief Enumerate specific data types.
    enum class Type
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
    JSON() : data(), type(Type::null) {}
    //! @brief Destroy the JSON object.
    ~JSON() = default;
    //! @brief Construct a new JSON object.
    //! @param type - data type
    explicit JSON(const Type type);
    //! @brief Construct a new JSON object.
    //! @param list - initializer list for JSON
    JSON(const std::initializer_list<JSON>& list);
    //! @brief Construct a new JSON object.
    //! @param json - the old object for copy constructor
    JSON(const JSON& json) = default;
    //! @brief Construct a new JSON object.
    //! @param json - the old object for move constructor
    JSON(JSON&& json) noexcept;
    //! @brief The operator (=) overloading of JSON class.
    //! @param json - the old object for copy assignment operator
    //! @return reference of the JSON object
    JSON& operator=(const JSON& json) = default;
    //! @brief The operator (=) overloading of JSON class.
    //! @param json - the old object for move assignment operator
    //! @return reference of the JSON object
    JSON& operator=(JSON&& json) noexcept;

    // NOLINTBEGIN (google-explicit-constructor)
    //! @brief Construct a new JSON object.
    JSON(std::nullptr_t /*n*/) : data(), type(Type::null) {}
    //! @brief Construct a new JSON object.
    //! @tparam T - type of string value
    //! @param s - string value
    template <typename T>
    JSON(const T s, typename std::enable_if<std::is_convertible<T, std::string>::value>::type* /*type*/ = nullptr) :
        data(String{s}), type(Type::string)
    {
    }
    //! @brief Construct a new JSON object.
    //! @tparam T - type of floating value
    //! @param f - floating value
    template <typename T>
    JSON(const T f, typename std::enable_if<std::is_floating_point<T>::value>::type* /*type*/ = 0) :
        data(static_cast<Floating>(f)), type(Type::floating)
    {
    }
    //! @brief Construct a new JSON object.
    //! @tparam T - type of integral value
    //! @param i - integral value
    template <typename T>
    JSON(
        const T i,
        typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value>::type* /*type*/ = 0) :
        data(static_cast<Integral>(i)), type(Type::integral)
    {
    }
    //! @brief Construct a new JSON object.
    //! @tparam T - type of boolean value
    //! @param b - boolean value
    template <typename T>
    JSON(const T b, typename std::enable_if<std::is_same<T, bool>::value>::type* /*type*/ = 0) :
        data(static_cast<Boolean>(b)), type(Type::boolean)
    {
    }
    // NOLINTEND (google-explicit-constructor)
    // NOLINTBEGIN (misc-unconventional-assign-operator)
    //! @brief The operator (=) overloading of JSON class.
    //! @tparam T - type of string value
    //! @param s - string value
    //! @return reference of the JSON object
    template <typename T>
    typename std::enable_if<std::is_convertible<T, std::string>::value, JSON&>::type operator=(const T s);
    //! @brief The operator (=) overloading of JSON class.
    //! @tparam T - type of floating value
    //! @param f - floating value
    //! @return reference of the JSON object
    template <typename T>
    typename std::enable_if<std::is_floating_point<T>::value, JSON&>::type operator=(const T f);
    //! @brief The operator (=) overloading of JSON class.
    //! @tparam T - type of integral value
    //! @param i - integral value
    //! @return reference of the JSON object
    template <typename T>
    typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value, JSON&>::type operator=(
        const T i);
    //! @brief The operator (=) overloading of JSON class.
    //! @tparam T - type of boolean value
    //! @param b - boolean value
    //! @return reference of the JSON object
    template <typename T>
    typename std::enable_if<std::is_same<T, bool>::value, JSON&>::type operator=(const T b);
    // NOLINTEND (misc-unconventional-assign-operator)

    //! @brief JSON wrapper.
    //! @tparam Container - type of container
    template <typename Container>
    class JSONWrapper
    {
    public:
        //! @brief Construct a new JSONWrapper object.
        //! @param container - JSON object to be wrapped
        explicit JSONWrapper(Container* container) : object(container) {}
        //! @brief Construct a new JSONWrapper object.
        explicit JSONWrapper(std::nullptr_t /*n*/) : object(nullptr) {}

        //! @brief Pointer to the JSON object.
        Container* object{nullptr};
        //! @brief Get the first iterator.
        //! @return first iterator
        inline typename Container::iterator begin()
        {
            return object ? object->begin() : typename Container::iterator();
        }
        //! @brief Get the last iterator.
        //! @return last iterator
        inline typename Container::iterator end() { return object ? object->end() : typename Container::iterator(); }
        //! @brief Get the first const iterator.
        //! @return first const iterator
        [[nodiscard]] inline typename Container::const_iterator begin() const
        {
            return object ? object->begin() : typename Container::iterator();
        }
        //! @brief Get the last const iterator.
        //! @return last const iterator
        [[nodiscard]] inline typename Container::const_iterator end() const
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
        explicit JSONConstWrapper(const Container* container) : object(container) {}
        //! @brief Construct a new JSONWrapper object.
        explicit JSONConstWrapper(const std::nullptr_t /*n*/) : object(nullptr) {}

        //! @brief Const pointer to the JSON object.
        const Container* object{nullptr};
        //! @brief Get the first const iterator.
        //! @return first const iterator
        [[nodiscard]] inline typename Container::const_iterator begin() const
        {
            return object ? object->begin() : typename Container::const_iterator();
        }
        //! @brief Get the last const iterator.
        //! @return last const iterator
        [[nodiscard]] inline typename Container::const_iterator end() const
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
    static JSON load(const std::string& fmt);
    //! @brief Append item to array. Convert to array type.
    //! @tparam T - type of arg
    //! @param arg - item
    template <typename T>
    void append(const T arg);
    //! @brief Append multiple items to array. Convert to array type.
    //! @tparam T - type of arg
    //! @tparam U - type of args
    //! @param arg - item
    //! @param args - multiple items
    template <typename T, typename... U>
    void append(const T arg, const U... args);
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
    //! @brief Check that the key exists.
    //! @param key - target key
    //! @return exist or not exist
    [[nodiscard]] bool hasKey(const std::string& key) const;
    //! @brief Get the data type.
    //! @return data type
    [[nodiscard]] Type getType() const;
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
    [[nodiscard]] String toString() const;
    //! @brief Convert to unescaped string value.
    //! @return unescaped string value
    [[nodiscard]] String toUnescapedString() const;
    //! @brief Convert to floating value.
    //! @return floating value
    [[nodiscard]] Floating toFloating() const;
    //! @brief Convert to integral value.
    //! @return integral value
    [[nodiscard]] Integral toIntegral() const;
    //! @brief Convert to boolean value.
    //! @return boolean value
    [[nodiscard]] Boolean toBoolean() const;
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
    [[nodiscard]] std::string dump(const std::uint32_t depth = 1, const std::string& tab = "    ") const;
    //! @brief Dump as minified formatted string.
    //! @return minified formatted string
    [[nodiscard]] std::string dumpMinified() const;

    //! @brief Alias for the value in the data.
    using Value = std::variant<std::monostate, Null, Object, Array, String, Floating, Integral, Boolean>;
    //! @brief The data that stores JSON information.
    struct Data
    {
        //! @brief Construct a new Data object.
        Data() : value(nullptr) {}
        //! @brief Construct a new Data object.
        //! @param o - object value
        explicit Data(const Object& o) : value(o) {}
        //! @brief Construct a new Data object.
        //! @param a - array value
        explicit Data(const Array& a) : value(a) {}
        //! @brief Construct a new Data object.
        //! @param s - string value
        explicit Data(const String& s) : value(s) {}
        //! @brief Construct a new Data object.
        //! @param f - floating value
        explicit Data(const Floating f) : value(f) {}
        //! @brief Construct a new Data object.
        //! @param i - integral value
        explicit Data(const Integral i) : value(i) {}
        //! @brief Construct a new Data object.
        //! @param b - boolean value
        explicit Data(const Boolean b) : value(b) {}

        //! @brief Value of the data.
        Value value{};
    } /** @brief JSON valid data. */ data{};

private:
    //! @brief Set the data type.
    //! @param t - target data type
    void setType(const Type t);

    //! @brief Data type.
    Type type{Type::null};

protected:
    friend std::ostream& operator<<(std::ostream& os, const JSON& json);
};

// NOLINTBEGIN (misc-unconventional-assign-operator)
template <typename T>
typename std::enable_if<std::is_convertible<T, std::string>::value, JSON&>::type JSON::operator=(const T s)
{
    setType(Type::string);
    data.value = String{s};
    return *this;
}

template <typename T>
typename std::enable_if<std::is_floating_point<T>::value, JSON&>::type JSON::operator=(const T f)
{
    setType(Type::floating);
    data.value = static_cast<Floating>(f);
    return *this;
}

template <typename T>
typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value, JSON&>::type JSON::operator=(
    const T i)
{
    setType(Type::integral);
    data.value = static_cast<Integral>(i);
    return *this;
}

template <typename T>
typename std::enable_if<std::is_same<T, bool>::value, JSON&>::type JSON::operator=(const T b)
{
    setType(Type::boolean);
    data.value = static_cast<Boolean>(b);
    return *this;
}
// NOLINTEND (misc-unconventional-assign-operator)

template <typename T>
void JSON::append(const T arg)
{
    setType(Type::array);
    std::get<Array>(data.value).emplace_back(arg);
}

template <typename T, typename... U>
void JSON::append(const T arg, const U... args)
{
    append(arg);
    append(args...);
}

extern JSON array();
extern JSON object();
} // namespace json
} // namespace utility
