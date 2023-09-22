//! @file json.hpp
//! @author ryftchen
//! @brief The declarations (json) in the utility module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen. All rights reserved.

#pragma once

#include <deque>
#include <map>
#include <string>

namespace utility::json
{
class JSON
{
public:
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

    JSON() : data(), type(Type::null){};
    ~JSON();
    explicit JSON(const Type type);
    JSON(const JSON& json);
    JSON(JSON&& json) noexcept;
    JSON(std::initializer_list<JSON> list);
    JSON& operator=(const JSON& json);
    JSON& operator=(JSON&& json) noexcept;

    // NOLINTBEGIN(google-explicit-constructor)
    template <typename T>
    JSON(T b, typename std::enable_if<std::is_same<T, bool>::value>::type* /*unused*/ = 0) :
        data(b), type(Type::boolean){};
    template <typename T>
    JSON(
        T i,
        typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value>::type* /*unused*/ = 0) :
        data(static_cast<long long>(i)), type(Type::integral){};
    template <typename T>
    JSON(T f, typename std::enable_if<std::is_floating_point<T>::value>::type* /*unused*/ = 0) :
        data(static_cast<double>(f)), type(Type::floating){};
    template <typename T>
    JSON(T s, typename std::enable_if<std::is_convertible<T, std::string>::value>::type* /*unused*/ = nullptr) :
        data(std::string(s)), type(Type::string){};
    JSON(std::nullptr_t /*null*/) : data(), type(Type::null){};
    // NOLINTEND(google-explicit-constructor)

    template <typename Container>
    class JSONWrapper
    {
    public:
        explicit JSONWrapper(Container* ctr) : object(ctr){};
        explicit JSONWrapper(std::nullptr_t /*null*/) : object(nullptr){};

        typename Container::iterator begin() { return object ? object->begin() : typename Container::iterator(); }
        typename Container::iterator end() { return object ? object->end() : typename Container::iterator(); }
        [[nodiscard]] typename Container::const_iterator begin() const
        {
            return object ? object->begin() : typename Container::iterator();
        }
        [[nodiscard]] typename Container::const_iterator end() const
        {
            return object ? object->end() : typename Container::iterator();
        }
        Container* object;
    };

    template <typename Container>
    class JSONConstWrapper
    {
    public:
        explicit JSONConstWrapper(const Container* ctr) : object(ctr){};
        explicit JSONConstWrapper(std::nullptr_t /*null*/) : object(nullptr){};

        [[nodiscard]] typename Container::const_iterator begin() const
        {
            return object ? object->begin() : typename Container::const_iterator();
        }
        [[nodiscard]] typename Container::const_iterator end() const
        {
            return object ? object->end() : typename Container::const_iterator();
        }
        const Container* object;
    };

    static JSON make(Type type);
    static JSON load(const std::string& str);
    template <typename T>
    void append(T arg);
    template <typename T, typename... U>
    void append(T arg, U... args);
    // NOLINTBEGIN(misc-unconventional-assign-operator)
    template <typename T>
    typename std::enable_if<std::is_same<T, bool>::value, JSON&>::type operator=(T b);
    template <typename T>
    typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value, JSON&>::type operator=(T i);
    template <typename T>
    typename std::enable_if<std::is_floating_point<T>::value, JSON&>::type operator=(T f);
    template <typename T>
    typename std::enable_if<std::is_convertible<T, std::string>::value, JSON&>::type operator=(T s);
    // NOLINTEND(misc-unconventional-assign-operator)
    JSON& operator[](const std::string& key);
    JSON& operator[](std::size_t index);
    JSON& at(const std::string& key);
    [[nodiscard]] const JSON& at(const std::string& key) const;
    JSON& at(std::size_t index);
    [[nodiscard]] const JSON& at(std::size_t index) const;
    [[nodiscard]] int length() const;
    [[nodiscard]] bool hasKey(const std::string& key) const;
    [[nodiscard]] int size() const;
    [[nodiscard]] Type jsonType() const;
    [[nodiscard]] bool isNullType() const;
    [[nodiscard]] bool isArrayType() const;
    [[nodiscard]] bool isBooleanType() const;
    [[nodiscard]] bool isFloatingType() const;
    [[nodiscard]] bool isIntegralType() const;
    [[nodiscard]] bool isStringType() const;
    [[nodiscard]] bool isObjectType() const;
    [[nodiscard]] std::string toString() const;
    [[nodiscard]] std::string toUnescapedString() const;
    [[nodiscard]] double toFloat() const;
    [[nodiscard]] long long toInt() const;
    [[nodiscard]] bool toBool() const;
    JSONWrapper<std::map<std::string, JSON>> objectRange();
    JSONWrapper<std::deque<JSON>> arrayRange();
    [[nodiscard]] JSONConstWrapper<std::map<std::string, JSON>> objectRange() const;
    [[nodiscard]] JSONConstWrapper<std::deque<JSON>> arrayRange() const;
    [[nodiscard]] std::string dump(int depth = 1, std::string tab = "  ") const;
    [[nodiscard]] std::string dumpMinified() const;

    union Data
    {
        explicit Data(double d) : floatVal(d){};
        explicit Data(long long i) : intVal(i){};
        explicit Data(bool b) : boolVal(b){};
        explicit Data(std::string s) : stringVal(new std::string(s)){};
        Data() : intVal(0){};

        std::deque<JSON>* listVal;
        std::map<std::string, JSON>* mapVal;
        std::string* stringVal;
        double floatVal;
        long long intVal;
        bool boolVal;
    } data;

private:
    void setType(const Type t);
    void clearData();

    Type type{Type::null};

protected:
    friend std::ostream& operator<<(std::ostream& os, const JSON& json);
};

template <typename T>
void JSON::append(T arg)
{
    setType(Type::array);
    data.listVal->emplace_back(arg);
}

template <typename T, typename... U>
void JSON::append(T arg, U... args)
{
    append(arg);
    append(args...);
}

// NOLINTBEGIN(misc-unconventional-assign-operator)
template <typename T>
typename std::enable_if<std::is_same<T, bool>::value, JSON&>::type JSON::operator=(T b)
{
    setType(Type::boolean);
    data.boolVal = b;
    return *this;
}

template <typename T>
typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value, JSON&>::type JSON::operator=(T i)
{
    setType(Type::integral);
    data.intVal = i;
    return *this;
}

template <typename T>
typename std::enable_if<std::is_floating_point<T>::value, JSON&>::type JSON::operator=(T f)
{
    setType(Type::floating);
    data.floatVal = f;
    return *this;
}

template <typename T>
typename std::enable_if<std::is_convertible<T, std::string>::value, JSON&>::type JSON::operator=(T s)
{
    setType(Type::string);
    *data.stringVal = std::string(s);
    return *this;
}
// NOLINTEND(misc-unconventional-assign-operator)

extern JSON array();
template <typename... T>
extern JSON array(T... args)
{
    JSON arr = JSON::make(JSON::Type::array);
    arr.append(args...);
    return arr;
}
extern JSON object();
} // namespace utility::json
