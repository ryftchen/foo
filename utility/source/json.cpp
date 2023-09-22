//! @file json.cpp
//! @author ryftchen
//! @brief The definitions (json) in the utility module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen. All rights reserved.

#include "json.hpp"
#include <charconv>
#include <cmath>
#include <stdexcept>

namespace utility::json
{
JSON parseNext(const std::string& str, std::size_t& offset);

std::string jsonEscape(const std::string& str)
{
    std::string output;
    for (std::size_t i = 0; i < str.length(); ++i)
    {
        switch (str.at(i))
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
                output += str.at(i);
                break;
        }
    }
    return output;
}

void consumeWhitespace(const std::string& str, std::size_t& offset)
{
    while (std::isspace(str.at(offset)))
    {
        ++offset;
    }
}

JSON parseObject(const std::string& str, std::size_t& offset)
{
    JSON object = JSON::make(JSON::Type::object);

    ++offset;
    consumeWhitespace(str, offset);
    if ('}' == str.at(offset))
    {
        ++offset;
        return object;
    }

    while (true)
    {
        JSON key = parseNext(str, offset);
        consumeWhitespace(str, offset);
        if (':' != str.at(offset))
        {
            throw std::runtime_error("Object in JSON: Expected colon, found '" + std::string{str.at(offset)} + "'.");
        }
        consumeWhitespace(str, ++offset);
        JSON value = parseNext(str, offset);
        object[key.toString()] = value;

        consumeWhitespace(str, offset);
        if (',' == str.at(offset))
        {
            ++offset;
            continue;
        }
        else if ('}' == str.at(offset))
        {
            ++offset;
            break;
        }
        else
        {
            throw std::runtime_error("Object in JSON: Expected comm, found '" + std::string{str.at(offset)} + "'.");
        }
    }
    return object;
}

JSON parseArray(const std::string& str, std::size_t& offset)
{
    JSON array = JSON::make(JSON::Type::array);
    std::size_t index = 0;

    ++offset;
    consumeWhitespace(str, offset);
    if (']' == str.at(offset))
    {
        ++offset;
        return array;
    }

    while (true)
    {
        array[index++] = parseNext(str, offset);
        consumeWhitespace(str, offset);

        if (',' == str.at(offset))
        {
            ++offset;
            continue;
        }
        else if (']' == str.at(offset))
        {
            ++offset;
            break;
        }
        else
        {
            throw std::runtime_error(
                "Array in JSON: Expected ',' or ']', found '" + std::string{str.at(offset)} + "'.");
        }
    }
    return array;
}

JSON parseString(const std::string& str, std::size_t& offset)
{
    std::string val;
    for (char c = str.at(++offset); '\"' != c; c = str.at(++offset))
    {
        if (c == '\\')
        {
            switch (str.at(++offset))
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
                        c = str.at(offset + i);
                        if (((c >= '0') && (c <= '9')) || ((c >= 'a') && (c <= 'f')) || ((c >= 'A') && (c <= 'F')))
                        {
                            val += c;
                        }
                        else
                        {
                            throw std::runtime_error(
                                "String in JSON: Expected hex character in unicode escape, found '" + std::string{c}
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
    return JSON(val); // NOLINT(modernize-return-braced-init-list)
}

JSON parseNumber(const std::string& str, std::size_t& offset)
{
    JSON number;
    std::string val, expStr;
    char c;
    bool isDouble = false;
    long long exp = 0;
    while (true)
    {
        c = str.at(offset++);
        if (('-' == c) || ((c >= '0') && (c <= '9')))
        {
            val += c;
        }
        else if ('.' == c)
        {
            val += c;
            isDouble = true;
        }
        else
        {
            break;
        }
    }
    if (('E' == c) || ('e' == c))
    {
        c = str.at(offset);
        if ('-' == c)
        {
            ++offset;
            expStr += '-';
        }
        if ('+' == c)
        {
            ++offset;
        }
        while (true)
        {
            c = str.at(offset++);
            if ((c >= '0') && (c <= '9'))
            {
                expStr += c;
            }
            else if (!std::isspace(c) && (c != ',') && (c != ']') && (c != '}'))
            {
                throw std::runtime_error(
                    "Number in JSON: Expected a number for exponent, found '" + std::string{c} + "'.");
            }
            else
            {
                break;
            }
        }
        exp = std::stol(expStr);
    }
    else if (!std::isspace(c) && (c != ',') && (c != ']') && (c != '}'))
    {
        throw std::runtime_error("Number in JSON: Unexpected character'" + std::string{c} + "'.");
    }
    --offset;

    constexpr std::uint8_t base = 10;
    if (isDouble)
    {
        number = std::stod(val) * std::pow(base, exp);
    }
    else
    {
        if (!expStr.empty())
        {
            number = std::stol(val) * std::pow(base, exp);
        }
        else
        {
            number = std::stol(val);
        }
    }
    return number;
}

JSON parseBool(const std::string& str, std::size_t& offset)
{
    const std::string trueStr = "true", falseStr = "false";
    JSON boolVal;
    if (str.substr(offset, trueStr.length()) == trueStr)
    {
        boolVal = true;
    }
    else if (str.substr(offset, falseStr.length()) == falseStr)
    {
        boolVal = false;
    }
    else
    {
        throw std::runtime_error(
            "Bool in JSON: Expected 'true' or 'false', found '" + str.substr(offset, falseStr.length()) + "'.");
    }
    offset += (boolVal.toBool() ? trueStr.length() : falseStr.length());
    return boolVal;
}

JSON parseNull(const std::string& str, std::size_t& offset)
{
    const std::string nullStr = "null";
    if (str.substr(offset, nullStr.length()) != nullStr)
    {
        throw std::runtime_error(
            "Null in JSON: Expected 'null', found '" + str.substr(offset, nullStr.length()) + "'.");
    }
    offset += nullStr.length();
    return {};
}

JSON parseNext(const std::string& str, std::size_t& offset)
{
    char value;
    consumeWhitespace(str, offset);
    value = str.at(offset);
    switch (value)
    {
        case '[':
            return parseArray(str, offset);
        case '{':
            return parseObject(str, offset);
        case '\"':
            return parseString(str, offset);
        case 't':
            [[fallthrough]];
        case 'f':
            return parseBool(str, offset);
        case 'n':
            return parseNull(str, offset);
        default:
            if (((value <= '9') && (value >= '0')) || ('-' == value))
            {
                return parseNumber(str, offset);
            }
    }
    throw std::runtime_error("JSON parsed: Unknown starting character '" + std::string{value} + "'.");
}

JSON::JSON(const JSON::Type type) : JSON()
{
    setType(type);
}

JSON::~JSON()
{
    switch (type)
    {
        case Type::array:
            delete data.listVal;
            break;
        case Type::object:
            delete data.mapVal;
            break;
        case Type::string:
            delete data.stringVal;
            break;
        default:
            break;
    }
}

JSON::JSON(const JSON& json)
{
    switch (json.type)
    {
        case Type::object:
            data.mapVal = new std::map<std::string, JSON>(json.data.mapVal->cbegin(), json.data.mapVal->cend());
            break;
        case Type::array:
            data.listVal = new std::deque<JSON>(json.data.listVal->cbegin(), json.data.listVal->cend());
            break;
        case Type::string:
            data.stringVal = new std::string(*json.data.stringVal);
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
    json.data.mapVal = nullptr;
}

JSON::JSON(std::initializer_list<JSON> list) : JSON()
{
    setType(Type::object);
    for (auto iter = list.begin(), end = list.end(); iter != end; ++iter, ++iter)
    {
        operator[](iter->toString()) = *std::next(iter);
    }
}

JSON& JSON::operator=(const JSON& json)
{
    clearData();
    switch (json.type)
    {
        case Type::object:
            if (nullptr != json.data.mapVal)
            {
                data.mapVal = new std::map<std::string, JSON>(json.data.mapVal->cbegin(), json.data.mapVal->cend());
            }
            break;
        case Type::array:
            if (nullptr != json.data.listVal)
            {
                data.listVal = new std::deque<JSON>(json.data.listVal->cbegin(), json.data.listVal->cend());
            }
            break;
        case Type::string:
            if (nullptr != json.data.stringVal)
            {
                data.stringVal = new std::string(*json.data.stringVal);
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
    json.data.mapVal = nullptr;
    json.type = Type::null;
    return *this;
}

JSON JSON::make(JSON::Type type)
{
    return JSON(type);
}

JSON JSON::load(const std::string& str)
{
    std::size_t offset = 0;
    return parseNext(str, offset);
}

JSON& JSON::operator[](const std::string& key)
{
    setType(Type::object);
    return data.mapVal->operator[](key);
}

JSON& JSON::operator[](std::size_t index)
{
    setType(Type::array);
    if (index >= data.listVal->size())
    {
        data.listVal->resize(index + 1);
    }
    return data.listVal->operator[](index);
}

JSON& JSON::at(const std::string& key)
{
    return operator[](key);
}

const JSON& JSON::at(const std::string& key) const
{
    return data.mapVal->at(key);
}

JSON& JSON::at(std::size_t index)
{
    return operator[](index);
}

const JSON& JSON::at(std::size_t index) const
{
    return data.listVal->at(index);
}

int JSON::length() const
{
    if (Type::array == type)
    {
        return static_cast<int>(data.listVal->size());
    }
    return -1;
}

bool JSON::hasKey(const std::string& key) const
{
    if (Type::object == type)
    {
        return data.mapVal->find(key) != data.mapVal->end();
    }
    return false;
}

int JSON::size() const
{
    if (Type::object == type)
    {
        return static_cast<int>(data.mapVal->size());
    }
    else if (Type::array == type)
    {
        return static_cast<int>(data.listVal->size());
    }
    return -1;
}

JSON::Type JSON::jsonType() const
{
    return type;
}

bool JSON::isNullType() const
{
    return Type::null == type;
}

bool JSON::isArrayType() const
{
    return Type::array == type;
}

bool JSON::isBooleanType() const
{
    return Type::boolean == type;
}

bool JSON::isFloatingType() const
{
    return Type::floating == type;
}

bool JSON::isIntegralType() const
{
    return Type::integral == type;
}

bool JSON::isStringType() const
{
    return Type::string == type;
}

bool JSON::isObjectType() const
{
    return Type::object == type;
}

std::string JSON::toString() const
{
    switch (type)
    {
        case Type::string:
            return jsonEscape(*data.stringVal);
        case Type::object:
            return dumpMinified();
        case Type::array:
            return dumpMinified();
        case Type::boolean:
            return data.boolVal ? std::string{"true"} : std::string{"false"};
        case Type::floating:
            return std::to_string(data.floatVal);
        case Type::integral:
            return std::to_string(data.intVal);
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
            return std::string{*data.stringVal};
        case Type::object:
            return dumpMinified();
        case Type::array:
            return dumpMinified();
        case Type::boolean:
            return data.boolVal ? std::string{"true"} : std::string{"false"};
        case Type::floating:
            return std::to_string(data.floatVal);
        case Type::integral:
            return std::to_string(data.intVal);
        case Type::null:
            return std::string{"null"};
        default:
            break;
    }
    return std::string{""};
}

double JSON::toFloat() const
{
    switch (type)
    {
        case Type::floating:
            return data.floatVal;
        case Type::boolean:
            return data.boolVal;
        case Type::integral:
            return data.intVal;
        case Type::string:
        {
            double parsed = 0.0;
            try
            {
                parsed = std::stod(*data.stringVal);
            }
            catch (const std::exception& error)
            {
                throw std::runtime_error("JSON parsed: Parsing float failed, " + std::string{error.what()} + ".");
            }
            return parsed;
        }
        default:
            break;
    }
    return 0.0;
}

long long JSON::toInt() const
{
    switch (type)
    {
        case Type::integral:
            return data.intVal;
        case Type::boolean:
            return data.boolVal;
        case Type::floating:
            return data.floatVal;
        case Type::string:
        {
            long long parsed = 0;
            const std::from_chars_result result =
                std::from_chars(data.stringVal->data(), data.stringVal->data() + data.stringVal->size(), parsed);
            if (!static_cast<bool>(result.ec))
            {
                return parsed;
            }
        }
        default:
            break;
    }
    return 0;
}

bool JSON::toBool() const
{
    switch (type)
    {
        case Type::boolean:
            return data.boolVal;
        case Type::integral:
            return data.intVal;
        case Type::floating:
            return data.floatVal;
        case Type::string:
        {
            if (data.stringVal->find("true") != std::string::npos)
            {
                return true;
            }
            if (data.stringVal->find("false") != std::string::npos)
            {
                return false;
            }
            int parsed = 0;
            const std::from_chars_result result =
                std::from_chars(data.stringVal->data(), data.stringVal->data() + data.stringVal->size(), parsed);
            if (!static_cast<bool>(result.ec))
            {
                return parsed;
            }
        }
        default:
            break;
    }
    return false;
}

JSON::JSONWrapper<std::map<std::string, JSON>> JSON::objectRange()
{
    if (Type::object == type)
    {
        return JSONWrapper<std::map<std::string, JSON>>(data.mapVal);
    }
    return JSONWrapper<std::map<std::string, JSON>>(nullptr);
}

JSON::JSONWrapper<std::deque<JSON>> JSON::arrayRange()
{
    if (Type::array == type)
    {
        return JSONWrapper<std::deque<JSON>>(data.listVal);
    }
    return JSONWrapper<std::deque<JSON>>(nullptr);
}

JSON::JSONConstWrapper<std::map<std::string, JSON>> JSON::objectRange() const
{
    if (Type::object == type)
    {
        return JSONConstWrapper<std::map<std::string, JSON>>(data.mapVal);
    }
    return JSONConstWrapper<std::map<std::string, JSON>>(nullptr);
}

JSON::JSONConstWrapper<std::deque<JSON>> JSON::arrayRange() const
{
    if (Type::array == type)
    {
        return JSONConstWrapper<std::deque<JSON>>(data.listVal);
    }
    return JSONConstWrapper<std::deque<JSON>>(nullptr);
}

std::string JSON::dump(int depth, std::string tab) const
{
    switch (type)
    {
        case Type::null:
            return "null";
        case Type::object:
        {
            std::string pad;
            for (int i = 0; i < depth; ++i, pad += tab)
            {
            }
            std::string s = "{\n";
            bool skip = true;
            for (auto& p : *data.mapVal)
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
            bool skip = true;
            for (auto& p : *data.listVal)
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
            return '\"' + jsonEscape(*data.stringVal) + '\"';
        case Type::floating:
            return std::to_string(data.floatVal);
        case Type::integral:
            return std::to_string(data.intVal);
        case Type::boolean:
            return data.boolVal ? "true" : "false";
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
            bool skip = true;
            for (auto& p : *data.mapVal)
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
            bool skip = true;
            for (auto& p : *data.listVal)
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
            return '\"' + jsonEscape(*data.stringVal) + '\"';
        case Type::floating:
            return std::to_string(data.floatVal);
        case Type::integral:
            return std::to_string(data.intVal);
        case Type::boolean:
            return data.boolVal ? "true" : "false";
        default:
            break;
    }
    return "";
}

void JSON::setType(const JSON::Type t)
{
    if (type == t)
    {
        return;
    }

    clearData();
    switch (t)
    {
        case Type::null:
            data.mapVal = nullptr;
            break;
        case Type::object:
            data.mapVal = new std::map<std::string, JSON>();
            break;
        case Type::array:
            data.listVal = new std::deque<JSON>();
            break;
        case Type::string:
            data.stringVal = new std::string();
            break;
        case Type::floating:
            data.floatVal = 0.0;
            break;
        case Type::integral:
            data.intVal = 0;
            break;
        case Type::boolean:
            data.boolVal = false;
            break;
    }

    type = t;
}

void JSON::clearData()
{
    switch (type)
    {
        case Type::object:
            delete data.mapVal;
            data.mapVal = nullptr;
            break;
        case Type::array:
            delete data.listVal;
            data.listVal = nullptr;
            break;
        case Type::string:
            delete data.stringVal;
            data.stringVal = nullptr;
            break;
        default:
            break;
    }
}

std::ostream& operator<<(std::ostream& os, const JSON& json)
{
    os << json.dump();
    return os;
}

JSON array()
{
    return JSON::make(JSON::Type::array);
}

JSON object()
{
    return JSON::make(JSON::Type::object);
}
} // namespace utility::json
