//! @file json.hpp
//! @author ryftchen
//! @brief The declarations (json) in the utility module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen. All rights reserved.

#pragma once

#include <deque>
#include <map>
#include <string>

//! @brief JSON-related functions in the utility module.
namespace utility::json
{
//! @brief Javascript object notation.
class JSON
{
public:
    //! @brief Enumerate specific data types.
    enum class Type
    {
        null,
        object,
        array,
        string,
        floating,
        integral,
        boolean
    };
    //! @brief Construct a new JSON object.
    JSON() : data(), type(Type::null){};
    //! @brief Destroy the JSON object.
    ~JSON();
    //! @brief Construct a new JSON object.
    //! @param type - data type
    explicit JSON(const Type type);
    //! @brief Construct a new JSON object.
    //! @param list - initializer list for JSON
    JSON(const std::initializer_list<JSON>& list);
    //! @brief Construct a new JSON object.
    //! @param json - the old object for copy constructor
    JSON(const JSON& json);
    //! @brief Construct a new JSON object.
    //! @param json - the old object for move constructor
    JSON(JSON&& json) noexcept;
    //! @brief The operator (=) overloading of JSON class.
    //! @param json - the old object for copy assignment operator
    //! @return reference of the JSON object
    JSON& operator=(const JSON& json);
    //! @brief The operator (=) overloading of JSON class.
    //! @param json - the old object for move assignment operator
    //! @return reference of the JSON object
    JSON& operator=(JSON&& json) noexcept;

    // NOLINTBEGIN(google-explicit-constructor)
    //! @brief Construct a new JSON object.
    JSON(std::nullptr_t /*n*/) : data(), type(Type::null){};
    //! @brief Construct a new JSON object.
    //! @tparam T - type of string value
    //! @param s - string value
    template <typename T>
    JSON(const T s, typename std::enable_if<std::is_convertible<T, std::string>::value>::type* /*type*/ = nullptr) :
        data(std::string(s)), type(Type::string){};
    //! @brief Construct a new JSON object.
    //! @tparam T - type of floating value
    //! @param f - floating value
    template <typename T>
    JSON(const T f, typename std::enable_if<std::is_floating_point<T>::value>::type* /*type*/ = 0) :
        data(static_cast<double>(f)), type(Type::floating){};
    //! @brief Construct a new JSON object.
    //! @tparam T - type of integral value
    //! @param i - integral value
    template <typename T>
    JSON(
        const T i,
        typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value>::type* /*type*/ = 0) :
        data(static_cast<long long>(i)), type(Type::integral){};
    //! @brief Construct a new JSON object.
    //! @tparam T - type of boolean value
    //! @param b - boolean value
    template <typename T>
    JSON(const T b, typename std::enable_if<std::is_same<T, bool>::value>::type* /*type*/ = 0) :
        data(b), type(Type::boolean){};
    // NOLINTEND(google-explicit-constructor)
    // NOLINTBEGIN(misc-unconventional-assign-operator)
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
    // NOLINTEND(misc-unconventional-assign-operator)

    //! @brief JSON wrapper.
    //! @tparam Container - type of container
    template <typename Container>
    class JSONWrapper
    {
    public:
        //! @brief Construct a new JSONWrapper object.
        //! @param container - JSON object to be wrapped
        explicit JSONWrapper(Container* container) : object(container){};
        //! @brief Construct a new JSONWrapper object.
        explicit JSONWrapper(std::nullptr_t /*n*/) : object(nullptr){};

        //! @brief JSON object.
        Container* object;
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
        explicit JSONConstWrapper(const Container* container) : object(container){};
        //! @brief Construct a new JSONWrapper object.
        explicit JSONConstWrapper(std::nullptr_t /*n*/) : object(nullptr){};

        //! @brief JSON object.
        const Container* object;
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
    //! @brief Check if the type is null.
    //! @return null or not
    [[nodiscard]] bool isNullType() const;
    //! @brief Check if the type is object.
    //! @return object or not
    [[nodiscard]] bool isObjectType() const;
    //! @brief Check if the type is array.
    //! @return array or not
    [[nodiscard]] bool isArrayType() const;
    //! @brief Check if the type is string.
    //! @return string or not
    [[nodiscard]] bool isStringType() const;
    //! @brief Check if the type is floating.
    //! @return floating or not
    [[nodiscard]] bool isFloatingType() const;
    //! @brief Check if the type is integral.
    //! @return integral or not
    [[nodiscard]] bool isIntegralType() const;
    //! @brief Check if the type is boolean.
    //! @return boolean or not
    [[nodiscard]] bool isBooleanType() const;
    //! @brief Convert to string value.
    //! @return string value
    [[nodiscard]] std::string toString() const;
    //! @brief Convert to unescaped string value.
    //! @return unescaped string value
    [[nodiscard]] std::string toUnescapedString() const;
    //! @brief Convert to floating value.
    //! @return floating value
    [[nodiscard]] double toFloating() const;
    //! @brief Convert to integral value.
    //! @return integral value
    [[nodiscard]] long long toIntegral() const;
    //! @brief Convert to boolean value.
    //! @return boolean value
    [[nodiscard]] bool toBoolean() const;
    //! @brief Get the wrapper of the object range.
    //! @return wrapper of the object range
    JSONWrapper<std::map<std::string, JSON>> objectRange();
    //! @brief Get the wrapper of the array range.
    //! @return wrapper of the array range
    JSONWrapper<std::deque<JSON>> arrayRange();
    //! @brief Get the wrapper of the object range.
    //! @return wrapper of the object range
    [[nodiscard]] JSONConstWrapper<std::map<std::string, JSON>> objectRange() const;
    //! @brief Get the wrapper of the array range.
    //! @return wrapper of the array range
    [[nodiscard]] JSONConstWrapper<std::deque<JSON>> arrayRange() const;
    //! @brief Dump as formatted string.
    //! @param depth - target depth
    //! @param tab - tab string
    //! @return formatted string
    [[nodiscard]] std::string dump(const int depth = 1, const std::string& tab = "    ") const;
    //! @brief Dump as minified formatted string.
    //! @return minified formatted string
    [[nodiscard]] std::string dumpMinified() const;

    //! @brief Union for the data that stores JSON information.
    union Data
    {
        //! @brief Construct a new Data object.
        Data() : integral(0){};
        //! @brief Construct a new Data object.
        //! @param f - floating value
        explicit Data(const double f) : floating(f){};
        //! @brief Construct a new Data object.
        //! @param i - integral value
        explicit Data(const long long i) : integral(i){};
        //! @brief Construct a new Data object.
        //! @param b - boolean value
        explicit Data(const bool b) : boolean(b){};
        //! @brief Construct a new Data object.
        //! @param s - string value
        explicit Data(const std::string& s) : string(new std::string(s)){};

        //! @brief Pointer to the object or null value.
        std::map<std::string, JSON>* map;
        //! @brief Pointer to the array value.
        std::deque<JSON>* list;
        //! @brief Pointer to the string value.
        std::string* string;
        //! @brief Floating value.
        double floating;
        //! @brief Integral value.
        long long integral;
        //! @brief Boolean value.
        bool boolean;
    } /** @brief JSON valid data. */ data;

private:
    //! @brief Set the data type.
    //! @param t - target data type
    void setType(const Type t);
    //! @brief Clear the data.
    void clearData();

    //! @brief Data type.
    Type type{Type::null};

protected:
    friend std::ostream& operator<<(std::ostream& os, const JSON& json);
};

// NOLINTBEGIN(misc-unconventional-assign-operator)
template <typename T>
typename std::enable_if<std::is_convertible<T, std::string>::value, JSON&>::type JSON::operator=(const T s)
{
    setType(Type::string);
    *data.string = std::string(s);
    return *this;
}

template <typename T>
typename std::enable_if<std::is_floating_point<T>::value, JSON&>::type JSON::operator=(const T f)
{
    setType(Type::floating);
    data.floating = f;
    return *this;
}

template <typename T>
typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value, JSON&>::type JSON::operator=(
    const T i)
{
    setType(Type::integral);
    data.integral = i;
    return *this;
}

template <typename T>
typename std::enable_if<std::is_same<T, bool>::value, JSON&>::type JSON::operator=(const T b)
{
    setType(Type::boolean);
    data.boolean = b;
    return *this;
}
// NOLINTEND(misc-unconventional-assign-operator)

template <typename T>
void JSON::append(const T arg)
{
    setType(Type::array);
    data.list->emplace_back(arg);
}

template <typename T, typename... U>
void JSON::append(const T arg, const U... args)
{
    append(arg);
    append(args...);
}

extern JSON array();
extern JSON object();
} // namespace utility::json
