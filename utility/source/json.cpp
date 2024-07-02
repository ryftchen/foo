//! @file json.cpp
//! @author ryftchen
//! @brief The definitions (json) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#include "json.hpp"

#include <charconv>
#include <cmath>
#include <stdexcept>

namespace utility::json
{
//! @brief Function version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    static const char* const ver = "0.1.0";
    return ver;
}

//! @brief Parse the next data in JSON.
//! @param fmt - formatted string
//! @param offset - data offset
static JSON parseNext(const std::string& fmt, std::size_t& offset);

//! @brief Escape for JSON.
//! @param fmt - formatted string
//! @return string after escape
static std::string jsonEscape(const std::string& fmt)
{
    std::string output;
    for (std::size_t i = 0; i < fmt.length(); ++i)
    {
        switch (fmt.at(i))
        {
            case '\"':
                output += "\\\"";
                break;
            case '\\':
                output += "\\\\";
                break;
            case '\b':
                output += "\\b";
                break;
            case '\f':
                output += "\\f";
                break;
            case '\n':
                output += "\\n";
                break;
            case '\r':
                output += "\\r";
                break;
            case '\t':
                output += "\\t";
                break;
            default:
                output += fmt.at(i);
                break;
        }
    }
    return output;
}

//! @brief Consume whitespace.
//! @param fmt - formatted string
//! @param offset - data offset
static void consumeWhitespace(const std::string& fmt, std::size_t& offset)
{
    while (std::isspace(fmt.at(offset)))
    {
        ++offset;
    }
}

//! @brief Parse object in JSON.
//! @param fmt - formatted string
//! @param offset - data offset
static JSON parseObject(const std::string& fmt, std::size_t& offset)
{
    JSON object = JSON::make(JSON::Type::object);

    ++offset;
    consumeWhitespace(fmt, offset);
    if ('}' == fmt.at(offset))
    {
        ++offset;
        return object;
    }

    for (;;)
    {
        const JSON key = parseNext(fmt, offset);
        consumeWhitespace(fmt, offset);
        if (':' != fmt.at(offset))
        {
            throw std::runtime_error("JSON object: Expected colon, found '" + std::string{fmt.at(offset)} + "'.");
        }
        consumeWhitespace(fmt, ++offset);
        const JSON value = parseNext(fmt, offset);
        object[key.toString()] = value;

        consumeWhitespace(fmt, offset);
        if (',' == fmt.at(offset))
        {
            ++offset;
            continue;
        }
        else if ('}' == fmt.at(offset))
        {
            ++offset;
            break;
        }
        else
        {
            throw std::runtime_error("JSON object: Expected comma, found '" + std::string{fmt.at(offset)} + "'.");
        }
    }
    return object;
}

//! @brief Parse array in JSON.
//! @param fmt - formatted string
//! @param offset - data offset
static JSON parseArray(const std::string& fmt, std::size_t& offset)
{
    JSON array = JSON::make(JSON::Type::array);
    ++offset;
    consumeWhitespace(fmt, offset);
    if (']' == fmt.at(offset))
    {
        ++offset;
        return array;
    }

    std::size_t index = 0;
    for (;;)
    {
        array[index++] = parseNext(fmt, offset);
        consumeWhitespace(fmt, offset);

        if (',' == fmt.at(offset))
        {
            ++offset;
            continue;
        }
        else if (']' == fmt.at(offset))
        {
            ++offset;
            break;
        }
        else
        {
            throw std::runtime_error("JSON array: Expected ',' or ']', found '" + std::string{fmt.at(offset)} + "'.");
        }
    }
    return array;
}

//! @brief Parse string in JSON.
//! @param fmt - formatted string
//! @param offset - data offset
static JSON parseString(const std::string& fmt, std::size_t& offset)
{
    std::string val;
    for (char c = fmt.at(++offset); '\"' != c; c = fmt.at(++offset))
    {
        if ('\\' == c)
        {
            switch (fmt.at(++offset))
            {
                case '\"':
                    val += '\"';
                    break;
                case '\\':
                    val += '\\';
                    break;
                case '/':
                    val += '/';
                    break;
                case 'b':
                    val += '\b';
                    break;
                case 'f':
                    val += '\f';
                    break;
                case 'n':
                    val += '\n';
                    break;
                case 'r':
                    val += '\r';
                    break;
                case 't':
                    val += '\t';
                    break;
                case 'u':
                {
                    val += "\\u";
                    for (std::uint8_t i = 1; i <= 4; ++i)
                    {
                        c = fmt.at(offset + i);
                        if (((c >= '0') && (c <= '9')) || ((c >= 'a') && (c <= 'f')) || ((c >= 'A') && (c <= 'F')))
                        {
                            val += c;
                        }
                        else
                        {
                            throw std::runtime_error(
                                "JSON string: Expected hex character in unicode escape, found '" + std::string{c}
                                + "'.");
                        }
                    }
                    offset += 4;
                }
                break;
                default:
                    val += '\\';
                    break;
            }
        }
        else
        {
            val += c;
        }
    }
    ++offset;
    return val;
}

//! @brief Extract the exponent.
//! @param fmt - formatted string
//! @param offset - data offset
//! @return exponent string
static std::string extractExponent(const std::string& fmt, std::size_t& offset)
{
    char c = fmt.at(offset);
    std::string expStr;
    if ('-' == c)
    {
        ++offset;
        expStr += '-';
    }
    else if ('+' == c)
    {
        ++offset;
    }

    for (;;)
    {
        c = fmt.at(offset++);
        if ((c >= '0') && (c <= '9'))
        {
            expStr += c;
        }
        else if (!std::isspace(c) && (',' != c) && (']' != c) && ('}' != c))
        {
            throw std::runtime_error("JSON number: Expected a number for exponent, found '" + std::string{c} + "'.");
        }
        else
        {
            break;
        }
    }
    return expStr;
}

//! @brief Parse number in JSON.
//! @param fmt - formatted string
//! @param offset - data offset
static JSON parseNumber(const std::string& fmt, std::size_t& offset)
{
    std::string val;
    char c;
    bool isFloating = false;
    for (;;)
    {
        c = fmt.at(offset++);
        if (('-' == c) || ((c >= '0') && (c <= '9')))
        {
            val += c;
        }
        else if ('.' == c)
        {
            val += c;
            isFloating = true;
        }
        else
        {
            break;
        }
    }

    long long exp = 0;
    std::string expStr;
    if (('E' == c) || ('e' == c))
    {
        expStr = extractExponent(fmt, offset);
        exp = std::stol(expStr);
    }
    else if (!std::isspace(c) && (',' != c) && (']' != c) && ('}' != c))
    {
        throw std::runtime_error("JSON number: Unexpected character '" + std::string{c} + "'.");
    }
    --offset;

    constexpr std::uint8_t base = 10;
    JSON number;
    if (isFloating)
    {
        number = std::stod(val) * std::pow(base, exp);
    }
    else if (!expStr.empty())
    {
        number = std::stol(val) * std::pow(base, exp);
    }
    else
    {
        number = std::stol(val);
    }
    return number;
}

//! @brief Parse boolean in JSON.
//! @param fmt - formatted string
//! @param offset - data offset
static JSON parseBoolean(const std::string& fmt, std::size_t& offset)
{
    JSON boolean;
    const std::string trueStr = "true", falseStr = "false";
    if (fmt.substr(offset, trueStr.length()) == trueStr)
    {
        boolean = true;
    }
    else if (fmt.substr(offset, falseStr.length()) == falseStr)
    {
        boolean = false;
    }
    else
    {
        throw std::runtime_error(
            "JSON boolean: Expected 'true' or 'false', found '" + fmt.substr(offset, falseStr.length()) + "'.");
    }
    offset += (boolean.toBoolean() ? trueStr.length() : falseStr.length());
    return boolean;
}

//! @brief Parse null in JSON.
//! @param fmt - formatted string
//! @param offset - data offset
static JSON parseNull(const std::string& fmt, std::size_t& offset)
{
    const std::string nullStr = "null";
    if (fmt.substr(offset, nullStr.length()) != nullStr)
    {
        throw std::runtime_error("JSON null: Expected 'null', found '" + fmt.substr(offset, nullStr.length()) + "'.");
    }
    offset += nullStr.length();
    return {};
}

static JSON parseNext(const std::string& fmt, std::size_t& offset)
{
    consumeWhitespace(fmt, offset);
    char value = fmt.at(offset);
    switch (value)
    {
        case '{':
            return parseObject(fmt, offset);
        case '[':
            return parseArray(fmt, offset);
        case '\"':
            return parseString(fmt, offset);
        case 't':
            [[fallthrough]];
        case 'f':
            return parseBoolean(fmt, offset);
        case 'n':
            return parseNull(fmt, offset);
        default:
            if (((value >= '0') && (value <= '9')) || ('-' == value))
            {
                return parseNumber(fmt, offset);
            }
    }
    throw std::runtime_error("JSON syntax error, unknown starting character '" + std::string{value} + "'.");
}

JSON::JSON(const JSON::Type type) : JSON()
{
    setType(type);
}

JSON::~JSON()
{
    switch (type)
    {
        case Type::object:
            delete data.map;
            break;
        case Type::array:
            delete data.list;
            break;
        case Type::string:
            delete data.string;
            break;
        default:
            break;
    }
}

JSON::JSON(const std::initializer_list<JSON>& list) : JSON()
{
    setType(Type::object);
    for (const auto* iter = list.begin(); list.end() != iter; std::advance(iter, 2))
    {
        operator[](iter->toString()) = *std::next(iter);
    }
}

JSON::JSON(const JSON& json)
{
    switch (json.type)
    {
        case Type::object:
            data.map = new std::map<std::string, JSON>{json.data.map->cbegin(), json.data.map->cend()};
            break;
        case Type::array:
            data.list = new std::deque<JSON>{json.data.list->cbegin(), json.data.list->cend()};
            break;
        case Type::string:
            data.string = new std::string{*json.data.string};
            break;
        default:
            data = json.data;
            break;
    }
    type = json.type;
}

JSON::JSON(JSON&& json) noexcept : data(json.data), type(json.type)
{
    json.type = Type::null;
    json.data.map = nullptr;
}

JSON& JSON::operator=(const JSON& json)
{
    clearData();
    switch (json.type)
    {
        case Type::object:
            if (nullptr != json.data.map)
            {
                data.map = new std::map<std::string, JSON>{json.data.map->cbegin(), json.data.map->cend()};
            }
            break;
        case Type::array:
            if (nullptr != json.data.list)
            {
                data.list = new std::deque<JSON>{json.data.list->cbegin(), json.data.list->cend()};
            }
            break;
        case Type::string:
            if (nullptr != json.data.string)
            {
                data.string = new std::string{*json.data.string};
            }
            break;
        default:
            data = json.data;
            break;
    }
    type = json.type;
    return *this;
}

JSON& JSON::operator=(JSON&& json) noexcept
{
    clearData();
    data = json.data;
    type = json.type;
    json.data.map = nullptr;
    json.type = Type::null;
    return *this;
}

JSON JSON::make(const JSON::Type type)
{
    return JSON(type);
}

JSON JSON::load(const std::string& fmt)
{
    std::size_t offset = 0;
    return parseNext(fmt, offset);
}

JSON& JSON::operator[](const std::string& key)
{
    setType(Type::object);
    return data.map->operator[](key);
}

JSON& JSON::operator[](std::size_t index)
{
    setType(Type::array);
    if (index >= data.list->size())
    {
        data.list->resize(index + 1);
    }
    return data.list->operator[](index);
}

JSON& JSON::at(const std::string& key)
{
    return operator[](key);
}

const JSON& JSON::at(const std::string& key) const
{
    return data.map->at(key);
}

JSON& JSON::at(std::size_t index)
{
    return operator[](index);
}

const JSON& JSON::at(std::size_t index) const
{
    return data.list->at(index);
}

int JSON::length() const
{
    if (Type::array == type)
    {
        return static_cast<int>(data.list->size());
    }
    return -1;
}

int JSON::size() const
{
    if (Type::object == type)
    {
        return static_cast<int>(data.map->size());
    }
    else if (Type::array == type)
    {
        return static_cast<int>(data.list->size());
    }
    return -1;
}

bool JSON::hasKey(const std::string& key) const
{
    if (Type::object == type)
    {
        return data.map->cend() != data.map->find(key);
    }
    return false;
}

JSON::Type JSON::getType() const
{
    return type;
}

bool JSON::isNullType() const
{
    return Type::null == type;
}

bool JSON::isObjectType() const
{
    return Type::object == type;
}

bool JSON::isArrayType() const
{
    return Type::array == type;
}

bool JSON::isStringType() const
{
    return Type::string == type;
}

bool JSON::isFloatingType() const
{
    return Type::floating == type;
}

bool JSON::isIntegralType() const
{
    return Type::integral == type;
}

bool JSON::isBooleanType() const
{
    return Type::boolean == type;
}

std::string JSON::toString() const
{
    switch (type)
    {
        case Type::string:
            return jsonEscape(*data.string);
        case Type::object:
            return dumpMinified();
        case Type::array:
            return dumpMinified();
        case Type::floating:
            return std::to_string(data.floating);
        case Type::integral:
            return std::to_string(data.integral);
        case Type::boolean:
            return data.boolean ? std::string{"true"} : std::string{"false"};
        case Type::null:
            return std::string{"null"};
        default:
            break;
    }
    return std::string{""};
}

std::string JSON::toUnescapedString() const
{
    switch (type)
    {
        case Type::string:
            return std::string{*data.string};
        case Type::object:
            return dumpMinified();
        case Type::array:
            return dumpMinified();
        case Type::floating:
            return std::to_string(data.floating);
        case Type::integral:
            return std::to_string(data.integral);
        case Type::boolean:
            return data.boolean ? std::string{"true"} : std::string{"false"};
        case Type::null:
            return std::string{"null"};
        default:
            break;
    }
    return std::string{""};
}

double JSON::toFloating() const
{
    switch (type)
    {
        case Type::floating:
            return data.floating;
        case Type::integral:
            return data.integral;
        case Type::boolean:
            return data.boolean;
        case Type::string:
        {
            double parsed = 0.0;
            try
            {
                parsed = std::stod(*data.string);
            }
            catch (const std::exception& err)
            {
                throw std::logic_error(
                    "Failed to convert the string value to floating in JSON, " + std::string{err.what()} + '.');
            }
            return parsed;
        }
        default:
            throw std::logic_error("Failed to convert the value to floating in JSON.");
    }
    return 0.0;
}

long long JSON::toIntegral() const
{
    switch (type)
    {
        case Type::integral:
            return data.integral;
        case Type::boolean:
            return data.boolean;
        case Type::floating:
            return data.floating;
        case Type::string:
        {
            long long parsed = 0;
            const std::from_chars_result result =
                std::from_chars(data.string->data(), data.string->data() + data.string->size(), parsed);
            if (!static_cast<bool>(result.ec))
            {
                return parsed;
            }
            throw std::logic_error("Failed to convert the string value to integral in JSON.");
        }
        default:
            throw std::logic_error("Failed to convert the value to integral in JSON.");
    }
    return 0;
}

bool JSON::toBoolean() const
{
    switch (type)
    {
        case Type::boolean:
            return data.boolean;
        case Type::floating:
            return data.floating;
        case Type::integral:
            return data.integral;
        case Type::string:
        {
            if (data.string->find("true") != std::string::npos)
            {
                return true;
            }
            if (data.string->find("false") != std::string::npos)
            {
                return false;
            }
            int parsed = 0;
            const std::from_chars_result result =
                std::from_chars(data.string->data(), data.string->data() + data.string->size(), parsed);
            if (!static_cast<bool>(result.ec))
            {
                return parsed;
            }
            throw std::logic_error("Failed to convert the string value to boolean in JSON.");
        }
        default:
            throw std::logic_error("Failed to convert the value to boolean in JSON.");
    }
    return false;
}

JSON::JSONWrapper<std::map<std::string, JSON>> JSON::objectRange()
{
    if (Type::object == type)
    {
        return JSONWrapper<std::map<std::string, JSON>>{data.map};
    }
    return JSONWrapper<std::map<std::string, JSON>>{nullptr};
}

JSON::JSONWrapper<std::deque<JSON>> JSON::arrayRange()
{
    if (Type::array == type)
    {
        return JSONWrapper<std::deque<JSON>>{data.list};
    }
    return JSONWrapper<std::deque<JSON>>{nullptr};
}

JSON::JSONConstWrapper<std::map<std::string, JSON>> JSON::objectRange() const
{
    if (Type::object == type)
    {
        return JSONConstWrapper<std::map<std::string, JSON>>{data.map};
    }
    return JSONConstWrapper<std::map<std::string, JSON>>{nullptr};
}

JSON::JSONConstWrapper<std::deque<JSON>> JSON::arrayRange() const
{
    if (Type::array == type)
    {
        return JSONConstWrapper<std::deque<JSON>>{data.list};
    }
    return JSONConstWrapper<std::deque<JSON>>{nullptr};
}

std::string JSON::dump(const std::uint32_t depth, const std::string& tab) const
{
    switch (type)
    {
        case Type::null:
            return "null";
        case Type::object:
        {
            std::string pad;
            for (std::uint32_t i = 0; i < depth; ++i)
            {
                pad += tab;
            }
            std::string s = "{\n";
            for (bool skip = true; const auto& p : *data.map)
            {
                if (!skip)
                {
                    s += ",\n";
                }
                s += (pad + '\"' + p.first + "\": " + p.second.dump(depth + 1, tab));
                skip = false;
            }
            s += ('\n' + pad.erase(0, tab.size()) + '}');
            return s;
        }
        case Type::array:
        {
            std::string s = "[";
            for (bool skip = true; const auto& p : *data.list)
            {
                if (!skip)
                {
                    s += ", ";
                }
                s += p.dump(depth + 1, tab);
                skip = false;
            }
            s += ']';
            return s;
        }
        case Type::string:
            return '\"' + jsonEscape(*data.string) + '\"';
        case Type::floating:
            return std::to_string(data.floating);
        case Type::integral:
            return std::to_string(data.integral);
        case Type::boolean:
            return data.boolean ? "true" : "false";
        default:
            break;
    }
    return "";
}

std::string JSON::dumpMinified() const
{
    switch (type)
    {
        case Type::null:
            return "null";
        case Type::object:
        {
            std::string s = "{";
            for (bool skip = true; const auto& p : *data.map)
            {
                if (!skip)
                {
                    s += ',';
                }
                s += ('\"' + p.first + "\":" + p.second.dumpMinified());
                skip = false;
            }
            s += '}';
            return s;
        }
        case Type::array:
        {
            std::string s = "[";
            for (bool skip = true; const auto& p : *data.list)
            {
                if (!skip)
                {
                    s += ',';
                }
                s += p.dumpMinified();
                skip = false;
            }
            s += ']';
            return s;
        }
        case Type::string:
            return '\"' + jsonEscape(*data.string) + '\"';
        case Type::floating:
            return std::to_string(data.floating);
        case Type::integral:
            return std::to_string(data.integral);
        case Type::boolean:
            return data.boolean ? "true" : "false";
        default:
            break;
    }
    return "";
}

void JSON::setType(const JSON::Type t)
{
    if (t == type)
    {
        return;
    }

    clearData();
    switch (t)
    {
        case Type::null:
            data.map = nullptr;
            break;
        case Type::object:
            data.map = new std::map<std::string, JSON>{};
            break;
        case Type::array:
            data.list = new std::deque<JSON>{};
            break;
        case Type::string:
            data.string = new std::string{};
            break;
        case Type::floating:
            data.floating = 0.0;
            break;
        case Type::integral:
            data.integral = 0;
            break;
        case Type::boolean:
            data.boolean = false;
            break;
    }

    type = t;
}

void JSON::clearData()
{
    switch (type)
    {
        case Type::object:
            delete data.map;
            data.map = nullptr;
            break;
        case Type::array:
            delete data.list;
            data.list = nullptr;
            break;
        case Type::string:
            delete data.string;
            data.string = nullptr;
            break;
        default:
            break;
    }
}

//! @brief The operator (<<) overloading of the JSON class.
//! @param os - output stream object
//! @param json - specific JSON object
//! @return reference of the output stream object
std::ostream& operator<<(std::ostream& os, const JSON& json)
{
    os << json.dump();
    return os;
}

//! @brief Make an JSON array.
//! @return JSON array
JSON array()
{
    return JSON::make(JSON::Type::array);
}

//! @brief Make an JSON object.
//! @return JSON object
JSON object()
{
    return JSON::make(JSON::Type::object);
}
} // namespace utility::json
