//! @file argument.cpp
//! @author ryftchen
//! @brief The definitions (argument) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "argument.hpp"

#include <iomanip>
#include <numeric>
#include <ranges>
#include <utility>

namespace utility::argument
{
//! @brief Function version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    static const char* const ver = "0.1.0";
    return ver;
}

Register& Register::help(const std::string_view content)
{
    helpCont = content;
    return *this;
}

Register& Register::metavar(const std::string_view content)
{
    metavarCont = content;
    return *this;
}

Register& Register::defaultValue(const std::string_view value)
{
    return defaultValue(std::string{value});
}

Register& Register::implicitValue(std::any value)
{
    implicitVal = std::move(value);
    argsNumRange = ArgsNumRange{0, 0};

    return *this;
}

Register& Register::required()
{
    isRequired = true;
    return *this;
}

Register& Register::appending()
{
    isRepeatable = true;
    return *this;
}

Register& Register::remaining()
{
    optionalAsValue = true;
    return argsNum(ArgsNumPattern::any);
}

Register& Register::argsNum(const std::size_t num)
{
    argsNumRange = ArgsNumRange{num, num};
    return *this;
}

Register& Register::argsNum(const std::size_t numMin, const std::size_t numMax)
{
    argsNumRange = ArgsNumRange{numMin, numMax};
    return *this;
}

Register& Register::argsNum(const ArgsNumPattern pattern)
{
    switch (pattern)
    {
        case ArgsNumPattern::optional:
            argsNumRange = ArgsNumRange{0, 1};
            break;
        case ArgsNumPattern::any:
            argsNumRange = ArgsNumRange{0, std::numeric_limits<std::size_t>::max()};
            break;
        case ArgsNumPattern::atLeastOne:
            argsNumRange = ArgsNumRange{1, std::numeric_limits<std::size_t>::max()};
            break;
        default:
            break;
    }

    return *this;
}

void Register::validate() const
{
    if (isOptional)
    {
        if (!isUsed && !defaultVal.has_value() && isRequired)
        {
            throwRequiredArgNotUsedException();
        }
        if (isUsed && isRequired && values.empty())
        {
            throwRequiredArgNoValueProvidedException();
        }
    }
    else if (!argsNumRange.isContain(values.size()) && !defaultVal.has_value())
    {
        throwArgsNumRangeValidationException();
    }
}

std::string Register::getInlineUsage() const
{
    std::string longestName(names.at(0));
    for (const auto& str : names)
    {
        if (str.length() > longestName.length())
        {
            longestName = str;
        }
    }
    std::ostringstream out{};
    if (!isRequired)
    {
        out << '[';
    }
    out << longestName;
    const std::string metavar = !metavarCont.empty() ? metavarCont : "VAR";
    if (argsNumRange.getMax() > 0)
    {
        out << ' ' << metavar;
        if (argsNumRange.getMax() > 1)
        {
            out << "...";
        }
    }
    if (!isRequired)
    {
        out << ']';
    }

    return std::move(out).str();
}

std::size_t Register::getArgumentsLength() const
{
    const std::size_t namesSize = std::accumulate(
        names.cbegin(), names.cend(), 0, [](const auto sum, const auto& str) { return sum + str.length(); });
    if (checkIfPositional(names.at(0), prefixChars))
    {
        return !metavarCont.empty() ? metavarCont.length() : (namesSize + (names.size() - 1));
    }

    std::size_t size = namesSize + 2 * (names.size() - 1);
    if (!metavarCont.empty() && (ArgsNumRange{1, 1} == argsNumRange))
    {
        size += metavarCont.length() + 1;
    }

    return size;
}

void Register::throwArgsNumRangeValidationException() const
{
    std::ostringstream out{};
    out << (!usedName.empty() ? usedName : names.at(0)) << ": " << argsNumRange.getMin();
    if (!argsNumRange.isExact())
    {
        if (argsNumRange.isRightBounded())
        {
            out << " to " << argsNumRange.getMax();
        }
        else
        {
            out << " or more";
        }
    }
    out << " argument(s) expected. " << values.size() << " provided.";

    throw std::runtime_error{out.str()};
}

void Register::throwRequiredArgNotUsedException() const
{
    throw std::runtime_error{names.at(0) + ": required."};
}

void Register::throwRequiredArgNoValueProvidedException() const
{
    throw std::runtime_error{usedName + ": no value provided."};
}

int Register::lookAhead(const std::string_view name)
{
    return name.empty() ? eof : static_cast<int>(static_cast<unsigned char>(name.front()));
}

bool Register::checkIfOptional(const std::string_view name, const std::string_view prefix)
{
    return !checkIfPositional(name, prefix);
}

bool Register::checkIfPositional(const std::string_view name, const std::string_view prefix)
{
    const int first = lookAhead(name);
    if (eof == first)
    {
        return true;
    }
    else if (prefix.find(static_cast<char>(first)) != std::string_view::npos)
    {
        std::string_view remain(name);
        remain.remove_prefix(1);
        return remain.empty();
    }

    return true;
}

//! @brief The operator (<<) overloading of the Register class.
//! @param os - output stream object
//! @param reg - specific Register object
//! @return reference of the output stream object
std::ostream& operator<<(std::ostream& os, const Register& reg)
{
    std::ostringstream out{};
    if (reg.checkIfPositional(reg.names.at(0), reg.prefixChars))
    {
        out << (!reg.metavarCont.empty() ? reg.metavarCont : join(reg.names.cbegin(), reg.names.cend(), " "));
    }
    else
    {
        out << join(reg.names.cbegin(), reg.names.cend(), ", ");
        if (!reg.metavarCont.empty() && (Register::ArgsNumRange{1, 1} == reg.argsNumRange))
        {
            out << ' ' << reg.metavarCont;
        }
    }

    const auto streamWidth = os.width();
    const auto namePadding = std::string(out.str().length(), ' ');
    os << out.str();

    std::size_t pos = 0, prev = 0;
    bool firstLine = true;
    const std::string_view helpView = reg.helpCont;
    const char* tabSpace = "    ";
    while (std::string::npos != (pos = reg.helpCont.find('\n', prev)))
    {
        const auto line = helpView.substr(prev, pos - prev + 1);
        if (firstLine)
        {
            os << tabSpace << line;
            firstLine = false;
        }
        else
        {
            os.width(streamWidth);
            os << namePadding << tabSpace << line;
        }
        prev += pos - prev + 1;
    }

    if (firstLine)
    {
        os << tabSpace << reg.helpCont;
    }
    else
    {
        const auto remain = helpView.substr(prev, reg.helpCont.length() - prev);
        if (!remain.empty())
        {
            os.width(streamWidth);
            os << namePadding << tabSpace << remain;
        }
    }

    if (!reg.helpCont.empty())
    {
        os << ' ';
    }
    os << reg.argsNumRange;

    if (reg.defaultVal.has_value() && (Register::ArgsNumRange{0, 0} != reg.argsNumRange))
    {
        os << "[default: " << reg.representedDefVal << ']';
    }
    else if (reg.isRequired)
    {
        os << "[required]";
    }
    os << '\n';

    return os;
}

Argument::Argument(const Argument& arg) :
    titleName{arg.titleName},
    versionNumber{arg.versionNumber},
    descrText{arg.descrText},
    prefixChars{arg.prefixChars},
    assignChars{arg.assignChars},
    isParsed{arg.isParsed},
    optionalArgs{arg.optionalArgs},
    positionalArgs{arg.positionalArgs},
    parserPath{arg.parserPath},
    subParsers{arg.subParsers}
{
    for (auto iterator = optionalArgs.begin(); optionalArgs.end() != iterator; ++iterator)
    {
        indexArgument(iterator);
    }
    for (auto iterator = positionalArgs.begin(); positionalArgs.end() != iterator; ++iterator)
    {
        indexArgument(iterator);
    }
    for (auto iterator = subParsers.begin(); subParsers.end() != iterator; ++iterator)
    {
        subParserMap.insert_or_assign(iterator->get().titleName, iterator);
        subParserUsed.insert_or_assign(iterator->get().titleName, false);
    }
}

Argument& Argument::operator=(const Argument& arg)
{
    if (this != &arg)
    {
        auto temp = arg;
        std::swap(*this, temp);
    }

    return *this;
}

Argument::operator bool() const
{
    const bool isArgUsed = std::any_of(
                   argumentMap.cbegin(), argumentMap.cend(), [](const auto& iter) { return iter.second->isUsed; }),
               isSubParserUsed = std::any_of(
                   subParserUsed.cbegin(), subParserUsed.cend(), [](const auto& iter) { return iter.second; });
    return isParsed && (isArgUsed || isSubParserUsed);
}

Register& Argument::operator[](const std::string_view argName) const
{
    auto iterator = argumentMap.find(argName);
    if (argumentMap.cend() != iterator)
    {
        return *(iterator->second);
    }

    if (!isValidPrefixChar(argName.at(0)))
    {
        std::string name(argName);
        const char legalPrefixChar = getAnyValidPrefixChar();
        const auto prefix = std::string(1, legalPrefixChar);

        name = prefix + name;
        iterator = argumentMap.find(name);
        if (argumentMap.cend() != iterator)
        {
            return *(iterator->second);
        }

        name = prefix + name;
        iterator = argumentMap.find(name);
        if (argumentMap.cend() != iterator)
        {
            return *(iterator->second);
        }
    }

    throw std::runtime_error{"No such argument: " + std::string{argName} + '.'};
}

Argument& Argument::addDescription(const std::string_view text)
{
    descrText = text;
    return *this;
}

void Argument::parseArgs(const std::vector<std::string>& arguments)
{
    parseArgsInternal(arguments);
    for (const auto& argument : std::views::values(argumentMap))
    {
        argument->validate();
    }
}

void Argument::parseArgs(const int argc, const char* const argv[])
{
    parseArgs(std::vector<std::string>{argv, argv + argc});
}

bool Argument::isUsed(const std::string_view argName) const
{
    return (*this)[argName].isUsed;
}

std::string Argument::title() const
{
    return titleName;
}

std::string Argument::version() const
{
    return versionNumber;
}

std::ostringstream Argument::help() const
{
    std::ostringstream out{};
    out << *this;

    return out;
}

std::string Argument::usage() const
{
    std::ostringstream out{};
    out << "usage: " << ((parserPath.find(' ' + titleName) == std::string::npos) ? titleName : parserPath);

    for (const auto& argument : optionalArgs)
    {
        out << ' ' << argument.getInlineUsage();
    }
    for (const auto& argument : positionalArgs)
    {
        out << ' ' << (!argument.metavarCont.empty() ? argument.metavarCont : argument.names.at(0));
    }

    if (!subParserMap.empty())
    {
        out << " {";
        for (std::size_t i = 0; const auto& command : std::views::keys(subParserMap))
        {
            if (0 != i)
            {
                out << ',';
            }
            out << command;
            ++i;
        }
        out << '}';
    }

    return std::move(out).str();
}

void Argument::addSubParser(Argument& parser)
{
    parser.parserPath = titleName + ' ' + parser.titleName;
    const auto iterator = subParsers.emplace(subParsers.cend(), parser);
    subParserMap.insert_or_assign(parser.titleName, iterator);
    subParserUsed.insert_or_assign(parser.titleName, false);
}

bool Argument::isValidPrefixChar(const char c) const
{
    return prefixChars.find(c) != std::string::npos;
}

char Argument::getAnyValidPrefixChar() const
{
    return prefixChars.at(0);
}

std::vector<std::string> Argument::preprocessArguments(const std::vector<std::string>& rawArguments) const
{
    std::vector<std::string> arguments{};
    for (const auto& arg : rawArguments)
    {
        const auto startWithPrefixChars = [this](const std::string_view str)
        {
            if (str.length() > 1)
            {
                const auto legalPrefix = [this](const char c) { return prefixChars.find(c) != std::string::npos; };
                return legalPrefix(str.at(0)) && legalPrefix(str.at(1));
            }
            return false;
        };

        if (const auto assignCharPos = arg.find_first_of(assignChars); (argumentMap.cend() == argumentMap.find(arg))
            && startWithPrefixChars(arg) && (std::string::npos != assignCharPos))
        {
            if (const auto optName = arg.substr(0, assignCharPos); argumentMap.cend() != argumentMap.find(optName))
            {
                arguments.emplace_back(optName);
                arguments.emplace_back(arg.substr(assignCharPos + 1));
                continue;
            }
        }
        arguments.emplace_back(arg);
    }

    return arguments;
}

void Argument::parseArgsInternal(const std::vector<std::string>& rawArguments)
{
    const auto arguments = preprocessArguments(rawArguments);
    if (titleName.empty() && !arguments.empty())
    {
        titleName = arguments.front();
    }

    const auto ending = arguments.cend();
    auto positionalArgIter = positionalArgs.begin();
    for (auto iterator = std::next(arguments.begin()); ending != iterator;)
    {
        const auto& currentArg = *iterator;
        if (Register::checkIfPositional(currentArg, prefixChars))
        {
            if (positionalArgs.cend() != positionalArgIter)
            {
                const auto argument = positionalArgIter++;
                iterator = argument->consume(iterator, ending);
                continue;
            }

            const std::string_view maybeCommand = currentArg;
            if (const auto subParserIter = subParserMap.find(maybeCommand); subParserMap.cend() != subParserIter)
            {
                const auto unprocessedArgs = std::vector<std::string>(iterator, ending);
                isParsed = true;
                subParserUsed[maybeCommand] = true;
                return subParserIter->second->get().parseArgs(unprocessedArgs);
            }
            throw std::runtime_error{"Maximum number of positional arguments exceeded."};
        }

        iterator = processRegisteredArgument(iterator, ending, currentArg);
    }
    isParsed = true;
}

std::size_t Argument::getLengthOfLongestArgument() const
{
    if (argumentMap.empty())
    {
        return 0;
    }

    std::size_t maxSize = 0;
    for (const auto& argument : std::views::values(argumentMap))
    {
        maxSize = std::max<std::size_t>(maxSize, argument->getArgumentsLength());
    }
    for (const auto& command : std::views::keys(subParserMap))
    {
        maxSize = std::max<std::size_t>(maxSize, command.length());
    }

    return maxSize;
}

void Argument::indexArgument(const RegisterIter& iterator)
{
    for (const auto& name : std::as_const(iterator->names))
    {
        argumentMap.insert_or_assign(name, iterator);
    }
}

//! @brief The operator (<<) overloading of the Argument class.
//! @param os - output stream object
//! @param arg - specific Argument object
//! @return reference of the output stream object
std::ostream& operator<<(std::ostream& os, const Argument& arg)
{
    os.setf(std::ios_base::left);
    const auto longestArgLen = arg.getLengthOfLongestArgument();

    os << arg.usage() << "\n\n";

    if (!arg.descrText.empty())
    {
        os << arg.descrText << "\n\n";
    }

    if (!arg.optionalArgs.empty())
    {
        os << "optional:\n";
    }
    for (const auto& argument : arg.optionalArgs)
    {
        os.width(static_cast<std::streamsize>(longestArgLen));
        os << argument;
    }

    if (!arg.positionalArgs.empty())
    {
        os << (arg.optionalArgs.empty() ? "" : "\n") << "positional:\n";
    }
    for (const auto& argument : arg.positionalArgs)
    {
        os.width(static_cast<std::streamsize>(longestArgLen));
        os << argument;
    }

    if (!arg.subParserMap.empty())
    {
        os << (arg.optionalArgs.empty() ? (arg.positionalArgs.empty() ? "" : "\n") : "\n") << "sub-command:\n";
        for (const auto& [command, subParser] : arg.subParserMap)
        {
            os << std::setw(static_cast<int>(longestArgLen)) << command << "    " << subParser->get().descrText << '\n';
        }
    }
    os << std::flush;

    return os;
}
} // namespace utility::argument
