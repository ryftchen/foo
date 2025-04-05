//! @file json.cpp
//! @author ryftchen
//! @brief The definitions (json) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

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
//! @return JSON data
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
//! @return JSON object
static JSON parseObject(const std::string_view fmt, std::size_t& offset)
{
    auto object = JSON::make(JSON::Type::object);
    ++offset;
    consumeWhitespace(fmt, offset);
    if ('}' == fmt.at(offset))
    {
        ++offset;
        return object;
    }

    for (;;)
    {
        const auto key = parseNext(fmt, offset);
        consumeWhitespace(fmt, offset);
        if (':' != fmt.at(offset))
        {
            throw std::runtime_error{"For JSON object, expected ':', found '" + std::string{fmt.at(offset)} + "'."};
        }
        consumeWhitespace(fmt, ++offset);
        const auto val = parseNext(fmt, offset);
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
            throw std::runtime_error{
                "For JSON object, expected ',' or '}', found '" + std::string{fmt.at(offset)} + "'."};
        }
    }

    return object;
}

//! @brief Parse array in JSON.
//! @param fmt - formatted string
//! @param offset - data offset
//! @return JSON array
static JSON parseArray(const std::string_view fmt, std::size_t& offset)
{
    auto array = JSON::make(JSON::Type::array);
    ++offset;
    consumeWhitespace(fmt, offset);
    if (']' == fmt.at(offset))
    {
        ++offset;
        return array;
    }

    for (std::size_t index = 0;;)
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
            throw std::runtime_error{
                "For JSON array, expected ',' or ']', found '" + std::string{fmt.at(offset)} + "'."};
        }
    }

    return array;
}

//! @brief Check whether the target character is hexadecimal.
//! @param c - target character
//! @return be hexadecimal or not
static bool isHexCharacter(const char c)
{
    return ((c >= '0') && (c <= '9')) || ((c >= 'a') && (c <= 'f')) || ((c >= 'A') && (c <= 'F'));
}

//! @brief Parse string in JSON.
//! @param fmt - formatted string
//! @param offset - data offset
//! @return JSON string
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
                    val += "\\u";
                    for (std::uint8_t i = 1; i <= 4; ++i)
                    {
                        c = fmt.at(offset + i);
                        if (!isHexCharacter(c))
                        {
                            throw std::runtime_error{
                                "For JSON string, expected hex character in unicode escape, found '" + std::string{c}
                                + "'."};
                        }
                        val += c;
                    }
                    offset += 4;
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
            throw std::runtime_error{"JSON syntax error, at \"" + val + "\" and after."};
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
            throw std::runtime_error{
                "For JSON number, expected a number for exponent, found '" + std::string{c} + "'."};
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
//! @return JSON number
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
        throw std::runtime_error{"For JSON number, unexpected character '" + std::string{c} + "'."};
    }
    --offset;

    JSON number{};
    if (constexpr std::uint8_t base = 10; isFloating)
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
//! @return JSON boolean
static JSON parseBoolean(const std::string_view fmt, std::size_t& offset)
{
    constexpr std::string_view trueLit = "true", falseLit = "false";
    JSON boolean{};
    if (fmt.substr(offset, trueLit.length()) == trueLit)
    {
        boolean = true;
    }
    else if (fmt.substr(offset, falseLit.length()) == falseLit)
    {
        boolean = false;
    }
    else
    {
        throw std::runtime_error{
            "For JSON boolean, expected \"" + std::string{trueLit} + "\" or \"" + std::string{falseLit} + "\", found \""
            + std::string{fmt.substr(offset, falseLit.length())} + "\"."};
    }
    offset += (boolean.toBoolean() ? trueLit.length() : falseLit.length());

    return boolean;
}

//! @brief Parse null in JSON.
//! @param fmt - formatted string
//! @param offset - data offset
//! @return JSON null
static JSON parseNull(const std::string_view fmt, std::size_t& offset)
{
    constexpr std::string_view nullLit = "null";
    if (fmt.substr(offset, nullLit.length()) != nullLit)
    {
        throw std::runtime_error{
            "For JSON null, expected \"" + std::string{nullLit} + "\", found \""
            + std::string{fmt.substr(offset, nullLit.length())} + "\"."};
    }
    offset += nullLit.length();

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
            break;
    }

    throw std::runtime_error{"JSON syntax error, unknown starting character '" + std::string{c} + "'."};
}

JSON::JSON(const Type type)
{
    switch (type)
    {
        case Type::null:
            setType<Null>();
            break;
        case Type::object:
            setType<Object>();
            break;
        case Type::array:
            setType<Array>();
            break;
        case Type::string:
            setType<String>();
            break;
        case Type::floating:
            setType<Floating>();
            break;
        case Type::integral:
            setType<Integral>();
            break;
        case Type::boolean:
            setType<Boolean>();
            break;
        default:
            break;
    }
}

JSON::JSON(const std::initializer_list<JSON>& list)
{
    setType<Object>();
    for (const auto* iterator = list.begin(); list.end() != iterator; std::advance(iterator, 2))
    {
        operator[](iterator->toString()) = *std::next(iterator);
    }
}

JSON::JSON(JSON&& json) noexcept
{
    data = std::move(json.data);
    json.data = Data{};
}

JSON& JSON::operator=(JSON&& json) noexcept
{
    if (this != &json)
    {
        data = std::move(json.data);
        json.data = Data{};
    }

    return *this;
}

JSON JSON::make(const Type type)
{
    return JSON(type);
}

JSON JSON::load(const std::string_view fmt)
{
    std::size_t offset = 0;
    auto object = parseNext(fmt, offset);
    if ((offset + 1) <= fmt.length())
    {
        throw std::runtime_error{"JSON syntax error, expected 'EOF' (" + std::string{fmt} + ")."};
    }

    return object;
}

JSON& JSON::operator[](const std::string_view key)
{
    setType<Object>();
    return std::get<Object>(data.value).operator[](key.data());
}

JSON& JSON::operator[](std::size_t index)
{
    setType<Array>();
    auto& arrayVal = std::get<Array>(data.value);
    if (index >= arrayVal.size())
    {
        arrayVal.resize(index + 1);
    }

    return arrayVal.operator[](index);
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
    return std::holds_alternative<Array>(data.value) ? static_cast<int>(std::get<Array>(data.value).size()) : -1;
}

int JSON::size() const
{
    return std::visit(
        DataVisitor{
            [](const Object& val) { return static_cast<int>(val.size()); },
            [](const Array& val) { return static_cast<int>(val.size()); },
            [](const auto& /*val*/) { return -1; }},
        data.value);
}

bool JSON::hasKey(const std::string_view key) const
{
    return std::holds_alternative<Object>(data.value) ? std::get<Object>(data.value).contains(key.data()) : false;
}

bool JSON::isNullType() const
{
    return std::holds_alternative<Null>(data.value);
}

bool JSON::isObjectType() const
{
    return std::holds_alternative<Object>(data.value);
}

bool JSON::isArrayType() const
{
    return std::holds_alternative<Array>(data.value);
}

bool JSON::isStringType() const
{
    return std::holds_alternative<String>(data.value);
}

bool JSON::isFloatingType() const
{
    return std::holds_alternative<Floating>(data.value);
}

bool JSON::isIntegralType() const
{
    return std::holds_alternative<Integral>(data.value);
}

bool JSON::isBooleanType() const
{
    return std::holds_alternative<Boolean>(data.value);
}

JSON::String JSON::toString() const
{
    return std::visit(
        DataVisitor{
            [](const String& val) -> String { return jsonEscape(val); },
            [this](const Object& /*val*/) -> String { return dumpMinified(); },
            [this](const Array& /*val*/) -> String { return dumpMinified(); },
            [](const Floating& val) -> String { return std::to_string(val); },
            [](const Integral& val) -> String { return std::to_string(val); },
            [](const Boolean& val) -> String { return val ? "true" : "false"; },
            [](const Null& /*val*/) -> String { return "null"; },
            [](const auto& /*val*/) -> String { return {}; }},
        data.value);
}

JSON::String JSON::toUnescapedString() const
{
    return std::visit(
        DataVisitor{
            [](const String& val) -> String { return val; },
            [this](const Object& /*val*/) -> String { return dumpMinified(); },
            [this](const Array& /*val*/) -> String { return dumpMinified(); },
            [](const Floating& val) -> String { return std::to_string(val); },
            [](const Integral& val) -> String { return std::to_string(val); },
            [](const Boolean& val) -> String { return val ? "true" : "false"; },
            [](const Null& /*val*/) -> String { return "null"; },
            [](const auto& /*val*/) -> String { return {}; }},
        data.value);
}

JSON::Floating JSON::toFloating() const
{
    return std::visit(
        DataVisitor{
            [](const Floating& val) -> Floating { return val; },
            [](const Integral& val) -> Floating { return val; },
            [](const Boolean& val) -> Floating { return val; },
            [](const String& val) -> Floating
            {
                double parsed = 0.0;
                try
                {
                    parsed = std::stod(val);
                }
                catch (const std::exception& err)
                {
                    throw std::logic_error{
                        "Failed to convert the string value to floating in JSON, " + std::string{err.what()} + '.'};
                }
                return parsed;
            },
            [](const auto& /*val*/) -> Floating
            { throw std::logic_error{"Failed to convert the value to floating in JSON."}; }},
        data.value);
}

JSON::Integral JSON::toIntegral() const
{
    return std::visit(
        DataVisitor{
            [](const Integral& val) -> Integral { return val; },
            [](const Boolean& val) -> Integral { return val; },
            [](const Floating& val) -> Integral { return val; },
            [](const String& val) -> Integral
            {
                long long parsed = 0;
                const auto result = std::from_chars(val.c_str(), val.c_str() + val.size(), parsed);
                if (!static_cast<bool>(result.ec))
                {
                    return parsed;
                }
                throw std::logic_error{"Failed to convert the string value to integral in JSON."};
            },
            [](const auto& /*val*/) -> Integral
            { throw std::logic_error{"Failed to convert the value to integral in JSON."}; }},
        data.value);
}

JSON::Boolean JSON::toBoolean() const
{
    return std::visit(
        DataVisitor{
            [](const Boolean& val) -> Boolean { return val; },
            [](const Floating& val) -> Boolean { return val; },
            [](const Integral& val) -> Boolean { return val; },
            [](const String& val) -> Boolean
            {
                if (val.find("true") != std::string::npos)
                {
                    return true;
                }
                if (val.find("false") != std::string::npos)
                {
                    return false;
                }
                int parsed = 0;
                const auto result = std::from_chars(val.c_str(), val.c_str() + val.size(), parsed);
                if (!static_cast<bool>(result.ec))
                {
                    return parsed;
                }
                throw std::logic_error{"Failed to convert the string value to boolean in JSON."};
            },
            [](const auto& /*val*/) -> Boolean
            { throw std::logic_error{"Failed to convert the value to boolean in JSON."}; }},
        data.value);
}

JSON::JSONWrapper<JSON::Object> JSON::objectRange()
{
    return std::holds_alternative<Object>(data.value) ? JSONWrapper<Object>{&std::get<Object>(data.value)}
                                                      : JSONWrapper<Object>{nullptr};
}

JSON::JSONWrapper<JSON::Array> JSON::arrayRange()
{
    return std::holds_alternative<Array>(data.value) ? JSONWrapper<Array>{&std::get<Array>(data.value)}
                                                     : JSONWrapper<Array>{nullptr};
}

JSON::JSONConstWrapper<JSON::Object> JSON::objectRange() const
{
    return std::holds_alternative<Object>(data.value) ? JSONConstWrapper<Object>{&std::get<Object>(data.value)}
                                                      : JSONConstWrapper<Object>{nullptr};
}

JSON::JSONConstWrapper<JSON::Array> JSON::arrayRange() const
{
    return std::holds_alternative<Array>(data.value) ? JSONConstWrapper<Array>{&std::get<Array>(data.value)}
                                                     : JSONConstWrapper<Array>{nullptr};
}

std::string JSON::dump(const std::uint32_t depth, const std::string_view tab) const
{
    return std::visit(
        DataVisitor{
            [](const Null& /*val*/) -> std::string { return "null"; },
            [depth, &tab](const Object& val) -> std::string
            {
                std::string pad{};
                for (std::uint32_t i = 0; i < depth; ++i)
                {
                    pad += tab;
                }
                std::string s("{\n");
                for (bool skip = true; const auto& p : val)
                {
                    if (!skip)
                    {
                        s += ",\n";
                    }
                    s += (pad + '\"' + p.first + "\": " + p.second.dump(depth + 1, tab));
                    skip = false;
                }
                s += ('\n' + pad.erase(0, tab.length()) + '}');
                return s;
            },
            [depth, &tab](const Array& val) -> std::string
            {
                std::string s("[");
                for (bool skip = true; const auto& p : val)
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
            },
            [](const String& val) -> std::string { return '\"' + jsonEscape(val) + '\"'; },
            [](const Floating& val) -> std::string { return std::to_string(val); },
            [](const Integral& val) -> std::string { return std::to_string(val); },
            [](const Boolean& val) -> std::string { return val ? "true" : "false"; },
            [](const auto& /*val*/) -> std::string { return {}; }},
        data.value);
}

std::string JSON::dumpMinified() const
{
    return std::visit(
        DataVisitor{
            [](const Null& /*val*/) -> std::string { return "null"; },
            [](const Object& val) -> std::string
            {
                std::string s("{");
                for (bool skip = true; const auto& p : val)
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
            },
            [](const Array& val) -> std::string
            {
                std::string s("[");
                for (bool skip = true; const auto& p : val)
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
            },
            [](const String& val) -> std::string { return '\"' + jsonEscape(val) + '\"'; },
            [](const Floating& val) -> std::string { return std::to_string(val); },
            [](const Integral& val) -> std::string { return std::to_string(val); },
            [](const Boolean& val) -> std::string { return val ? "true" : "false"; },
            [](const auto& /*val*/) -> std::string { return {}; }},
        data.value);
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
