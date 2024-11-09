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
static JSON parseNext(const std::string_view fmt, std::size_t& offset);

//! @brief Escape for JSON.
//! @param fmt - formatted string
//! @return string after escape
static std::string jsonEscape(const std::string_view fmt)
{
    std::string output{};
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
static void consumeWhitespace(const std::string_view fmt, std::size_t& offset)
{
    while ((offset < fmt.length()) && std::isspace(fmt.at(offset)))
    {
        ++offset;
    }

    if (offset == fmt.length())
    {
        --offset;
    }
}

//! @brief Parse object in JSON.
//! @param fmt - formatted string
//! @param offset - data offset
static JSON parseObject(const std::string_view fmt, std::size_t& offset)
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
            throw std::runtime_error("JSON object: Expected ':', found '" + std::string{fmt.at(offset)} + "'.");
        }
        consumeWhitespace(fmt, ++offset);
        const JSON val = parseNext(fmt, offset);
        object[key.toString()] = val;

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
            throw std::runtime_error("JSON object: Expected ',' or '}', found '" + std::string{fmt.at(offset)} + "'.");
        }
    }
    return object;
}

//! @brief Parse array in JSON.
//! @param fmt - formatted string
//! @param offset - data offset
static JSON parseArray(const std::string_view fmt, std::size_t& offset)
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
static JSON parseString(const std::string_view fmt, std::size_t& offset)
{
    std::string val{};
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

        if ((offset + 1) == fmt.length())
        {
            throw std::runtime_error("JSON syntax error, at \"" + val + "\" and after.");
        }
    }
    ++offset;
    return val;
}

//! @brief Extract the exponent.
//! @param fmt - formatted string
//! @param offset - data offset
//! @return exponent string
static std::string extractExponent(const std::string_view fmt, std::size_t& offset)
{
    char c = fmt.at(offset);
    std::string expStr{};
    if ('-' == c)
    {
        ++offset;
        expStr += '-';
    }
    else if ('+' == c)
    {
        ++offset;
    }

    while (offset < fmt.length())
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
static JSON parseNumber(const std::string_view fmt, std::size_t& offset)
{
    std::string val{};
    char c = '\0';
    bool isFloating = false;
    while (offset < fmt.length())
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
    std::string expStr{};
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
    JSON number{};
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
static JSON parseBoolean(const std::string_view fmt, std::size_t& offset)
{
    JSON boolean{};
    constexpr std::string_view trueStr = "true", falseStr = "false";
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
            R"(JSON boolean: Expected "true" or "false", found ")" + std::string{fmt.substr(offset, falseStr.length())}
            + "\".");
    }
    offset += (boolean.toBoolean() ? trueStr.length() : falseStr.length());
    return boolean;
}

//! @brief Parse null in JSON.
//! @param fmt - formatted string
//! @param offset - data offset
static JSON parseNull(const std::string_view fmt, std::size_t& offset)
{
    constexpr std::string_view nullStr = "null";
    if (fmt.substr(offset, nullStr.length()) != nullStr)
    {
        throw std::runtime_error(
            R"(JSON null: Expected "null", found ")" + std::string{fmt.substr(offset, nullStr.length())} + "\".");
    }
    offset += nullStr.length();
    return {};
}

static JSON parseNext(const std::string_view fmt, std::size_t& offset)
{
    consumeWhitespace(fmt, offset);
    const char c = fmt.at(offset);
    switch (c)
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
            if (((c >= '0') && (c <= '9')) || ('-' == c))
            {
                return parseNumber(fmt, offset);
            }
    }
    throw std::runtime_error("JSON syntax error, unknown starting character '" + std::string{c} + "'.");
}

JSON::JSON(const JSON::Type type) : JSON()
{
    setType(type);
}

JSON::JSON(const std::initializer_list<JSON>& list) : JSON()
{
    setType(Type::object);
    for (const auto* iter = list.begin(); list.end() != iter; std::advance(iter, 2))
    {
        operator[](iter->toString()) = *std::next(iter);
    }
}

JSON::JSON(JSON&& json) noexcept
{
    data = std::move(json.data);
    type = json.type;
    json.data = Data{};
    json.type = Type::null;
}

JSON& JSON::operator=(JSON&& json) noexcept
{
    if (this != &json)
    {
        data = std::move(json.data);
        type = json.type;
        json.data = Data{};
        json.type = Type::null;
    }
    return *this;
}

JSON JSON::make(const JSON::Type type)
{
    return JSON(type);
}

JSON JSON::load(const std::string_view fmt)
{
    std::size_t offset = 0;
    JSON object = parseNext(fmt, offset);
    if ((offset + 1) <= fmt.length())
    {
        throw std::runtime_error("JSON syntax error, expected 'EOF' (" + std::string{fmt} + ").");
    }
    return object;
}

JSON& JSON::operator[](const std::string_view key)
{
    setType(Type::object);
    return std::get<Object>(data.value).operator[](key.data());
}

JSON& JSON::operator[](std::size_t index)
{
    setType(Type::array);
    if (index >= std::get<Array>(data.value).size())
    {
        std::get<Array>(data.value).resize(index + 1);
    }
    return std::get<Array>(data.value).operator[](index);
}

JSON& JSON::at(const std::string_view key)
{
    return operator[](key);
}

const JSON& JSON::at(const std::string_view key) const
{
    return std::get<Object>(data.value).at(key.data());
}

JSON& JSON::at(std::size_t index)
{
    return operator[](index);
}

const JSON& JSON::at(std::size_t index) const
{
    return std::get<Array>(data.value).at(index);
}

int JSON::length() const
{
    if (Type::array == type)
    {
        return static_cast<int>(std::get<Array>(data.value).size());
    }
    return -1;
}

int JSON::size() const
{
    if (Type::object == type)
    {
        return static_cast<int>(std::get<Object>(data.value).size());
    }
    else if (Type::array == type)
    {
        return static_cast<int>(std::get<Array>(data.value).size());
    }
    return -1;
}

bool JSON::hasKey(const std::string_view key) const
{
    if (Type::object == type)
    {
        return std::get<Object>(data.value).cend() != std::get<Object>(data.value).find(key.data());
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

JSON::String JSON::toString() const
{
    switch (type)
    {
        case Type::string:
            return jsonEscape(std::get<String>(data.value));
        case Type::object:
            return dumpMinified();
        case Type::array:
            return dumpMinified();
        case Type::floating:
            return std::to_string(std::get<Floating>(data.value));
        case Type::integral:
            return std::to_string(std::get<Integral>(data.value));
        case Type::boolean:
            return std::get<Boolean>(data.value) ? "true" : "false";
        case Type::null:
            return "null";
        default:
            break;
    }
    return {};
}

JSON::String JSON::toUnescapedString() const
{
    switch (type)
    {
        case Type::string:
            return std::get<String>(data.value);
        case Type::object:
            return dumpMinified();
        case Type::array:
            return dumpMinified();
        case Type::floating:
            return std::to_string(std::get<Floating>(data.value));
        case Type::integral:
            return std::to_string(std::get<Integral>(data.value));
        case Type::boolean:
            return std::get<Boolean>(data.value) ? "true" : "false";
        case Type::null:
            return "null";
        default:
            break;
    }
    return {};
}

JSON::Floating JSON::toFloating() const
{
    switch (type)
    {
        case Type::floating:
            return std::get<Floating>(data.value);
        case Type::integral:
            return std::get<Integral>(data.value);
        case Type::boolean:
            return std::get<Boolean>(data.value);
        case Type::string:
        {
            double parsed = 0.0;
            try
            {
                parsed = std::stod(std::get<String>(data.value));
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

JSON::Integral JSON::toIntegral() const
{
    switch (type)
    {
        case Type::integral:
            return std::get<Integral>(data.value);
        case Type::boolean:
            return std::get<Boolean>(data.value);
        case Type::floating:
            return std::get<Floating>(data.value);
        case Type::string:
        {
            long long parsed = 0;
            const std::from_chars_result result = std::from_chars(
                std::get<String>(data.value).data(),
                std::get<String>(data.value).data() + std::get<String>(data.value).size(),
                parsed);
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

JSON::Boolean JSON::toBoolean() const
{
    switch (type)
    {
        case Type::boolean:
            return std::get<Boolean>(data.value);
        case Type::floating:
            return std::get<Floating>(data.value);
        case Type::integral:
            return std::get<Integral>(data.value);
        case Type::string:
        {
            if (std::get<String>(data.value).find("true") != std::string::npos)
            {
                return true;
            }
            if (std::get<String>(data.value).find("false") != std::string::npos)
            {
                return false;
            }
            int parsed = 0;
            const std::from_chars_result result = std::from_chars(
                std::get<String>(data.value).data(),
                std::get<String>(data.value).data() + std::get<String>(data.value).size(),
                parsed);
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

JSON::JSONWrapper<JSON::Object> JSON::objectRange()
{
    if (Type::object == type)
    {
        return JSONWrapper<Object>{&std::get<Object>(data.value)};
    }
    return JSONWrapper<Object>{nullptr};
}

JSON::JSONWrapper<JSON::Array> JSON::arrayRange()
{
    if (Type::array == type)
    {
        return JSONWrapper<Array>{&std::get<Array>(data.value)};
    }
    return JSONWrapper<Array>{nullptr};
}

JSON::JSONConstWrapper<JSON::Object> JSON::objectRange() const
{
    if (Type::object == type)
    {
        return JSONConstWrapper<Object>{&std::get<Object>(data.value)};
    }
    return JSONConstWrapper<Object>{nullptr};
}

JSON::JSONConstWrapper<JSON::Array> JSON::arrayRange() const
{
    if (Type::array == type)
    {
        return JSONConstWrapper<Array>{&std::get<Array>(data.value)};
    }
    return JSONConstWrapper<Array>{nullptr};
}

std::string JSON::dump(const std::uint32_t depth, const std::string_view tab) const
{
    switch (type)
    {
        case Type::null:
            return "null";
        case Type::object:
        {
            std::string pad{};
            for (std::uint32_t i = 0; i < depth; ++i)
            {
                pad += tab;
            }
            std::string s = "{\n";
            for (bool skip = true; const auto& p : std::get<Object>(data.value))
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
            for (bool skip = true; const auto& p : std::get<Array>(data.value))
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
            return '\"' + jsonEscape(std::get<String>(data.value)) + '\"';
        case Type::floating:
            return std::to_string(std::get<Floating>(data.value));
        case Type::integral:
            return std::to_string(std::get<Integral>(data.value));
        case Type::boolean:
            return std::get<Boolean>(data.value) ? "true" : "false";
        default:
            break;
    }
    return {};
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
            for (bool skip = true; const auto& p : std::get<Object>(data.value))
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
            for (bool skip = true; const auto& p : std::get<Array>(data.value))
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
            return '\"' + jsonEscape(std::get<String>(data.value)) + '\"';
        case Type::floating:
            return std::to_string(std::get<Floating>(data.value));
        case Type::integral:
            return std::to_string(std::get<Integral>(data.value));
        case Type::boolean:
            return std::get<Boolean>(data.value) ? "true" : "false";
        default:
            break;
    }
    return {};
}

void JSON::setType(const JSON::Type t)
{
    if (t == type)
    {
        return;
    }

    switch (t)
    {
        case Type::null:
            data = Data{};
            break;
        case Type::object:
            data.value = Object{};
            break;
        case Type::array:
            data.value = Array{};
            break;
        case Type::string:
            data.value = String{};
            break;
        case Type::floating:
            data.value = static_cast<Floating>(0.0);
            break;
        case Type::integral:
            data.value = static_cast<Integral>(0);
            break;
        case Type::boolean:
            data.value = static_cast<Boolean>(false);
            break;
    }

    type = t;
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
