//! @file argument.cpp
//! @author ryftchen
//! @brief The definitions (argument) in the utility module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen. All rights reserved.

#include "argument.hpp"
#include <iterator>
#include <numeric>

namespace utility::argument
{
ArgumentRegister& ArgumentRegister::help(const std::string& str)
{
    helpStr = str;
    return *this;
}

ArgumentRegister& ArgumentRegister::implicitValue(std::any value)
{
    implicitValues = std::move(value);
    argNumArgsRange = NArgsRange{0, 0};
    return *this;
}

ArgumentRegister& ArgumentRegister::required()
{
    isRequired = true;
    return *this;
}

ArgumentRegister& ArgumentRegister::appending()
{
    isRepeatable = true;
    return *this;
}

ArgumentRegister& ArgumentRegister::remaining()
{
    isAcceptOptionalLikeValue = true;
    return nArgs(NArgsPattern::any);
}

ArgumentRegister& ArgumentRegister::nArgs(const std::size_t numArgs)
{
    argNumArgsRange = NArgsRange{numArgs, numArgs};
    return *this;
}

ArgumentRegister& ArgumentRegister::nArgs(const std::size_t numArgsMin, const std::size_t numArgsMax)
{
    argNumArgsRange = NArgsRange{numArgsMin, numArgsMax};
    return *this;
}

ArgumentRegister& ArgumentRegister::nArgs(const NArgsPattern pattern)
{
    switch (pattern)
    {
        case NArgsPattern::optional:
            argNumArgsRange = NArgsRange{0, 1};
            break;
        case NArgsPattern::any:
            argNumArgsRange = NArgsRange{0, std::numeric_limits<std::size_t>::max()};
            break;
        case NArgsPattern::atLeastOne:
            argNumArgsRange = NArgsRange{1, std::numeric_limits<std::size_t>::max()};
            break;
        default:
            break;
    }
    return *this;
}

void ArgumentRegister::validate() const
{
    if (isOptional)
    {
        if (!isUsed && !defaultValues.has_value() && isRequired)
        {
            throwRequiredArgNotUsedException();
        }
        if (isUsed && isRequired && values.empty())
        {
            throwRequiredArgNoValueProvidedException();
        }
    }
    else
    {
        if (!argNumArgsRange.isContain(values.size()) && !defaultValues.has_value())
        {
            throwNArgsRangeValidationException();
        }
    }
}

std::size_t ArgumentRegister::getArgumentsLength() const
{
    return std::accumulate(
        std::begin(names),
        std::end(names),
        std::size_t(0),
        [](const auto& sum, const auto& str)
        {
            return (sum + str.size() + 1);
        });
}

std::vector<std::string> ArgumentRegister::splitHelpStringWithDelimiter(const char delim) const
{
    std::string origin = helpStr;
    if (0 == origin.find_first_of(delim))
    {
        origin.erase(0, 1);
    }
    if (origin.length() == (origin.find_last_of(delim) + 1))
    {
        origin.pop_back();
    }

    std::vector<std::string> splits;
    std::size_t pos = 0, prevPos = 0;
    do
    {
        pos = origin.substr(prevPos).find(delim);
        if (pos == std::string::npos)
        {
            splits.emplace_back(origin.substr(prevPos));
            break;
        }

        pos += prevPos;
        splits.emplace_back(origin.substr(prevPos, pos - prevPos));
        prevPos = pos + 1;
    }
    while (true);

    return splits;
}

//! @brief The operator (<<) overloading of the ArgumentRegister class.
//! @param os - output stream object
//! @param argument - specific ArgumentRegister object
//! @return reference of output stream object
std::ostream& operator<<(std::ostream& os, const ArgumentRegister& argument)
{
    std::stringstream nameStream;
    std::copy(
        std::begin(argument.names), std::end(argument.names), std::ostream_iterator<std::string>(nameStream, ", "));
    const auto osLastPos = os.tellp();
    os << nameStream.str().substr(0, nameStream.str().length() - 2);
    const auto alignmentLength = os.tellp() - osLastPos;
    os << "    ";

    std::vector<std::string> helpMsg =
        (!argument.helpStr.empty()) ? argument.splitHelpStringWithDelimiter('\n') : std::vector<std::string>{};
    if (helpMsg.size() > 0)
    {
        os << *helpMsg.cbegin();
        helpMsg.erase(helpMsg.cbegin());
    }

    if (argument.defaultValues.has_value())
    {
        if (!argument.helpStr.empty())
        {
            os << ' ';
        }
        os << "[default: " << argument.defaultValueRepresent << ']';
    }
    else if (argument.isRequired)
    {
        if (!argument.helpStr.empty())
        {
            os << ' ';
        }
        os << "[required]";
    }

    if (helpMsg.size() > 0)
    {
        os << '\n';
        std::string alignStr = "    ";
        alignStr.insert(0, alignmentLength, ' ');
        for (auto lineIter = std::cbegin(helpMsg); std::cend(helpMsg) != lineIter; ++lineIter)
        {
            os << alignStr << *lineIter;
            if (std::cend(helpMsg) != (lineIter + 1))
            {
                os << '\n';
            }
        }
    }

    os << '\n';
    return os;
}

void ArgumentRegister::throwNArgsRangeValidationException() const
{
    std::stringstream stream;
    if (!usedNameStr.empty())
    {
        stream << usedNameStr << ": ";
    }
    if (argNumArgsRange.isExact())
    {
        stream << argNumArgsRange.getMin();
    }
    else if (argNumArgsRange.isRightBounded())
    {
        stream << argNumArgsRange.getMin() << " to " << argNumArgsRange.getMax();
    }
    else
    {
        stream << argNumArgsRange.getMin() << " or more";
    }
    stream << " argument(str) expected. " << values.size() << " provided.";
    throw std::runtime_error(stream.str());
}

void ArgumentRegister::throwRequiredArgNotUsedException() const
{
    std::stringstream stream;
    stream << names[0] << ": required";
    throw std::runtime_error(stream.str());
}

void ArgumentRegister::throwRequiredArgNoValueProvidedException() const
{
    std::stringstream stream;
    stream << usedNameStr << ": no value provided";
    throw std::runtime_error(stream.str());
}

int ArgumentRegister::lookAhead(const std::string_view str)
{
    if (str.empty())
    {
        return eof;
    }
    return static_cast<int>(static_cast<unsigned char>(str[0]));
}

bool ArgumentRegister::checkIfOptional(std::string_view name)
{
    return !checkIfNonOptional(name);
}

bool ArgumentRegister::checkIfNonOptional(std::string_view name)
{
    switch (lookAhead(name))
    {
        case '-':
        {
            name.remove_prefix(1);
            if (name.empty())
            {
                return true;
            }
            return false;
        }
        case eof:
            [[fallthrough]];
        default:
            return true;
    }
}

Argument::Argument(const Argument& arg) :
    title(arg.title),
    version(arg.version),
    isParsed(arg.isParsed),
    nonOptionalArguments(arg.nonOptionalArguments),
    optionalArguments(arg.optionalArguments)
{
    for (auto iterator = std::begin(nonOptionalArguments); std::end(nonOptionalArguments) != iterator; ++iterator)
    {
        indexArgument(iterator);
    }
    for (auto iterator = std::begin(optionalArguments); std::end(optionalArguments) != iterator; ++iterator)
    {
        indexArgument(iterator);
    }
}

Argument& Argument::operator=(const Argument& arg)
{
    auto temp = arg;
    std::swap(*this, temp);
    return *this;
}

void Argument::parseArgs(const std::vector<std::string>& arguments)
{
    parseArgsInternal(arguments);
    for ([[maybe_unused]] const auto& [unused, argument] : argumentMap)
    {
        argument->validate();
    }
}

void Argument::parseArgs(int argc, const char* const argv[])
{
    std::vector<std::string> arguments;
    std::copy(argv, argv + argc, std::back_inserter(arguments));
    parseArgs(arguments);
}

bool Argument::isUsed(const std::string_view argName) const
{
    return (*this)[argName].isUsed;
}

ArgumentRegister& Argument::operator[](const std::string_view argName) const
{
    auto iterator = argumentMap.find(argName);
    if (argumentMap.end() != iterator)
    {
        return *(iterator->second);
    }
    if ('-' != argName.front())
    {
        std::string name(argName);

        name = '-' + name;
        iterator = argumentMap.find(name);
        if (argumentMap.end() != iterator)
        {
            return *(iterator->second);
        }

        name = '-' + name;
        iterator = argumentMap.find(name);
        if (argumentMap.end() != iterator)
        {
            return *(iterator->second);
        }
    }
    throw std::logic_error("No such argument: " + std::string(argName));
}

//! @brief The operator (<<) overloading of the Argument class.
//! @param os - output stream object
//! @param parser - specific Argument object
//! @return reference of output stream object
std::ostream& operator<<(std::ostream& os, const Argument& parser)
{
    os.setf(std::ios_base::left);
    os << "Usage: " << parser.title << " <options...> ";
    std::size_t longestArgLength = parser.getLengthOfLongestArgument();

    for (const auto& argument : parser.nonOptionalArguments)
    {
        os << '[' << argument.names.front() << "...] ";
    }
    os << "\n\n";

    if (!parser.optionalArguments.empty())
    {
        os << "optional:\n";
    }

    for (const auto& argument : parser.optionalArguments)
    {
        os.width(longestArgLength);
        os << argument;
    }
    os << '\n';

    if (!parser.nonOptionalArguments.empty())
    {
        os << "non-optional:\n";
    }

    for (const auto& argument : parser.nonOptionalArguments)
    {
        os.width(longestArgLength);
        os << argument;
    }

    return os;
}

std::stringstream Argument::help() const
{
    std::stringstream out;
    out << *this;
    return out;
}

void Argument::parseArgsInternal(const std::vector<std::string>& arguments)
{
    if (title.empty() && !arguments.empty())
    {
        title = arguments.front();
    }
    auto end = std::end(arguments);
    auto nonOptionalArgumentIter = std::begin(nonOptionalArguments);
    for (auto iterator = std::next(std::begin(arguments)); end != iterator;)
    {
        const auto& currentArgument = *iterator;
        if (ArgumentRegister::checkIfNonOptional(currentArgument))
        {
            if (std::end(nonOptionalArguments) == nonOptionalArgumentIter)
            {
                throw std::runtime_error("Maximum number of non-optional arguments exceeded.");
            }
            auto argument = nonOptionalArgumentIter++;
            iterator = argument->consume(iterator, end);
            continue;
        }

        auto argMapIter = argumentMap.find(currentArgument);
        if (argumentMap.end() != argMapIter)
        {
            auto argument = argMapIter->second;
            iterator = argument->consume(std::next(iterator), end, argMapIter->first);
        }
        else if (const auto& compoundArg = currentArgument;
                 (compoundArg.size() > 1) && ('-' == compoundArg[0]) && ('-' != compoundArg[1]))
        {
            ++iterator;
            for (std::size_t i = 1; i < compoundArg.size(); ++i)
            {
                auto hypotheticalArg = std::string{'-', compoundArg[i]};
                auto argMapIter2 = argumentMap.find(hypotheticalArg);
                if (argumentMap.end() != argMapIter2)
                {
                    auto argument = argMapIter2->second;
                    iterator = argument->consume(iterator, end, argMapIter2->first);
                }
                else
                {
                    throw std::runtime_error("Unknown argument: " + currentArgument);
                }
            }
        }
        else
        {
            throw std::runtime_error("Unknown argument: " + currentArgument);
        }
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
    for ([[maybe_unused]] const auto& [unused, argument] : argumentMap)
    {
        maxSize = std::max(maxSize, argument->getArgumentsLength());
    }
    return maxSize;
}

void Argument::indexArgument(ListIterator iterator)
{
    for (const auto& name : std::as_const(iterator->names))
    {
        argumentMap.insert_or_assign(name, iterator);
    }
}
} // namespace utility::argument
