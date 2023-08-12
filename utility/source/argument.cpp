//! @file argument.cpp
//! @author ryftchen
//! @brief The definitions (argument) in the utility module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen. All rights reserved.

#include "argument.hpp"
#include <iomanip>
#include <iterator>
#include <numeric>

namespace utility::argument
{
ArgumentRegister& ArgumentRegister::help(const std::string& content)
{
    helpContent = content;
    return *this;
}

ArgumentRegister& ArgumentRegister::metavar(const std::string& content)
{
    metavarContent = content;
    return *this;
}

ArgumentRegister& ArgumentRegister::defaultValue(const char* value)
{
    return defaultValue(std::string(value));
}

ArgumentRegister& ArgumentRegister::implicitValue(std::any value)
{
    implicitValues = std::move(value);
    numArgsRange = NArgsRange{0, 0};
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
    numArgsRange = NArgsRange{numArgs, numArgs};
    return *this;
}

ArgumentRegister& ArgumentRegister::nArgs(const std::size_t numArgsMin, const std::size_t numArgsMax)
{
    numArgsRange = NArgsRange{numArgsMin, numArgsMax};
    return *this;
}

ArgumentRegister& ArgumentRegister::nArgs(const NArgsPattern pattern)
{
    switch (pattern)
    {
        case NArgsPattern::optional:
            numArgsRange = NArgsRange{0, 1};
            break;
        case NArgsPattern::any:
            numArgsRange = NArgsRange{0, (std::numeric_limits<std::size_t>::max)()};
            break;
        case NArgsPattern::atLeastOne:
            numArgsRange = NArgsRange{1, (std::numeric_limits<std::size_t>::max)()};
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
        if (!numArgsRange.isContain(values.size()) && !defaultValues.has_value())
        {
            throwNArgsRangeValidationException();
        }
    }
}

std::string ArgumentRegister::getInlineUsage() const
{
    std::stringstream usage;
    std::string longestName = names.front();
    for (const auto& str : names)
    {
        if (str.size() > longestName.size())
        {
            longestName = str;
        }
    }
    if (!isRequired)
    {
        usage << '[';
    }
    usage << longestName;
    const std::string metavar = !metavarContent.empty() ? metavarContent : "VAR";
    if (numArgsRange.getMax() > 0)
    {
        usage << ' ' << metavar;
        if (numArgsRange.getMax() > 1)
        {
            usage << "...";
        }
    }
    if (!isRequired)
    {
        usage << ']';
    }
    return usage.str();
}

std::size_t ArgumentRegister::getArgumentsLength() const
{
    const std::size_t namesSize = std::accumulate(
        std::cbegin(names),
        std::cend(names),
        std::size_t(0),
        [](const auto& sum, const auto& str)
        {
            return sum + str.size();
        });

    if (checkIfNonOptional(names.front(), prefixChars))
    {
        if (!metavarContent.empty())
        {
            return 2 + metavarContent.size();
        }

        return 2 + namesSize + (names.size() - 1);
    }
    std::size_t size = namesSize + 2 * (names.size() - 1);
    if (!metavarContent.empty() && numArgsRange == NArgsRange{1, 1})
    {
        size += metavarContent.size() + 1;
    }
    return size + 2;
}

void ArgumentRegister::throwNArgsRangeValidationException() const
{
    std::stringstream stream;
    if (!usedName.empty())
    {
        stream << usedName << ": ";
    }
    else
    {
        stream << names.front() << ": ";
    }
    if (numArgsRange.isExact())
    {
        stream << numArgsRange.getMin();
    }
    else if (numArgsRange.isRightBounded())
    {
        stream << numArgsRange.getMin() << " to " << numArgsRange.getMax();
    }
    else
    {
        stream << numArgsRange.getMin() << " or more";
    }
    stream << " argument(s) expected. " << values.size() << " provided.";
    throw std::runtime_error(stream.str());
}

void ArgumentRegister::throwRequiredArgNotUsedException() const
{
    std::stringstream stream;
    stream << names.front() << ": required";
    throw std::runtime_error(stream.str());
}

void ArgumentRegister::throwRequiredArgNoValueProvidedException() const
{
    std::stringstream stream;
    stream << usedName << ": no value provided";
    throw std::runtime_error(stream.str());
}

int ArgumentRegister::lookAhead(const std::string_view name)
{
    if (name.empty())
    {
        return eof;
    }
    return static_cast<int>(static_cast<unsigned char>(name[0]));
}

bool ArgumentRegister::checkIfOptional(std::string_view name, const std::string_view prefix)
{
    return !checkIfNonOptional(name, prefix);
}

bool ArgumentRegister::checkIfNonOptional(std::string_view name, const std::string_view prefix)
{
    const int first = lookAhead(name);
    if (eof == first)
    {
        return true;
    }
    else if (prefix.find(static_cast<char>(first)) != std::string_view::npos)
    {
        name.remove_prefix(1);
        if (name.empty())
        {
            return true;
        }
        return false;
    }
    return true;
}

//! @brief The operator (<<) overloading of the ArgumentRegister class.
//! @param os - output stream object
//! @param argReg - specific ArgumentRegister object
//! @return reference of output stream object
std::ostream& operator<<(std::ostream& os, const ArgumentRegister& argReg)
{
    std::stringstream nameStream;
    nameStream << "  ";
    if (argReg.checkIfNonOptional(argReg.names.front(), argReg.prefixChars))
    {
        if (!argReg.metavarContent.empty())
        {
            nameStream << argReg.metavarContent;
        }
        else
        {
            nameStream << join(argReg.names.cbegin(), argReg.names.cend(), " ");
        }
    }
    else
    {
        nameStream << join(argReg.names.cbegin(), argReg.names.cend(), ", ");
        if (!argReg.metavarContent.empty() && (ArgumentRegister::NArgsRange{1, 1} == argReg.numArgsRange))
        {
            nameStream << ' ' << argReg.metavarContent;
        }
    }

    const auto streamWidth = os.width();
    const std::string namePadding = std::string(nameStream.str().size(), ' ');
    std::size_t pos = 0;
    std::size_t prev = 0;
    bool firstLine = true;
    const char* hspace = "  ";
    os << nameStream.str();
    std::string_view helpView(argReg.helpContent);
    while ((pos = argReg.helpContent.find('\n', prev)) != std::string::npos)
    {
        auto line = helpView.substr(prev, pos - prev + 1);
        if (firstLine)
        {
            os << hspace << line;
            firstLine = false;
        }
        else
        {
            os.width(streamWidth);
            os << namePadding << hspace << line;
        }
        prev += pos - prev + 1;
    }
    if (firstLine)
    {
        os << hspace << argReg.helpContent;
    }
    else
    {
        const auto leftover = helpView.substr(prev, argReg.helpContent.size() - prev);
        if (!leftover.empty())
        {
            os.width(streamWidth);
            os << namePadding << hspace << leftover;
        }
    }

    if (!argReg.helpContent.empty())
    {
        os << ' ';
    }
    os << argReg.numArgsRange;

    if (argReg.defaultValues.has_value() && argReg.numArgsRange != ArgumentRegister::NArgsRange{0, 0})
    {
        os << "[default: " << argReg.defaultValueRepresent << ']';
    }
    else if (argReg.isRequired)
    {
        os << "[required]";
    }

    os << '\n';
    return os;
}

Argument::Argument(const Argument& arg) :
    title(arg.title),
    version(arg.version),
    descrText(arg.descrText),
    prefixChars(arg.prefixChars),
    assignChars(arg.assignChars),
    isParsed(arg.isParsed),
    nonOptionalArguments(arg.nonOptionalArguments),
    optionalArguments(arg.optionalArguments),
    parserPath(arg.parserPath),
    subParsers(arg.subParsers)
{
    for (auto iterator = std::begin(nonOptionalArguments); iterator != std::end(nonOptionalArguments); ++iterator)
    {
        indexArgument(iterator);
    }
    for (auto iterator = std::begin(optionalArguments); iterator != std::end(optionalArguments); ++iterator)
    {
        indexArgument(iterator);
    }
    for (auto iterator = std::begin(subParsers); iterator != std::end(subParsers); ++iterator)
    {
        subParserMap.insert_or_assign(iterator->get().title, iterator);
        subParserUsed.insert_or_assign(iterator->get().title, false);
    }
}

Argument& Argument::operator=(const Argument& arg)
{
    auto temp = arg;
    std::swap(*this, temp);
    return *this;
}

Argument::operator bool() const
{
    const auto isArgUsed = std::any_of(
        argumentMap.cbegin(),
        argumentMap.cend(),
        [](const auto& iterator)
        {
            return iterator.second->isUsed;
        });
    const auto isSubParserUsed = std::any_of(
        subParserUsed.cbegin(),
        subParserUsed.cend(),
        [](const auto& iterator)
        {
            return iterator.second;
        });

    return isParsed && (isArgUsed || isSubParserUsed);
}

Argument& Argument::addDescription(const std::string& text)
{
    descrText = text;
    return *this;
}

Argument& Argument::setPrefixChars(const std::string& prefix)
{
    prefixChars = prefix;
    return *this;
}

Argument& Argument::setAssignChars(const std::string& assign)
{
    assignChars = assign;
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

void Argument::parseArgs(const int argc, const char* const argv[])
{
    parseArgs({argv, argv + argc});
}

std::vector<std::string> Argument::parseKnownArgs(const std::vector<std::string>& arguments)
{
    auto unknownArguments = parseKnownArgsInternal(arguments);
    for ([[maybe_unused]] const auto& [unused, argument] : argumentMap)
    {
        argument->validate();
    }
    return unknownArguments;
}

std::vector<std::string> Argument::parseKnownArgs(const int argc, const char* const argv[])
{
    return parseKnownArgs({argv, argv + argc});
}

bool Argument::isUsed(const std::string_view argName) const
{
    return (*this)[argName].isUsed;
}

auto Argument::isSubCommandUsed(const std::string_view subCommandName) const
{
    return subParserUsed.at(subCommandName);
}

auto Argument::isSubCommandUsed(const Argument& subParser) const
{
    return isSubCommandUsed(subParser.title);
}

ArgumentRegister& Argument::operator[](const std::string_view argName) const
{
    auto iterator = argumentMap.find(argName);
    if (iterator != argumentMap.cend())
    {
        return *(iterator->second);
    }
    if (!isValidPrefixChar(argName.front()))
    {
        std::string name(argName);
        const auto legalPrefixChar = getAnyValidPrefixChar();
        const auto prefix = std::string(1, legalPrefixChar);

        name = prefix + name;
        iterator = argumentMap.find(name);
        if (iterator != argumentMap.cend())
        {
            return *(iterator->second);
        }

        name = prefix + name;
        iterator = argumentMap.find(name);
        if (iterator != argumentMap.cend())
        {
            return *(iterator->second);
        }
    }
    throw std::logic_error("No such argument: " + std::string(argName));
}

std::stringstream Argument::help() const
{
    std::stringstream out;
    out << *this;
    return out;
}

std::string Argument::usage() const
{
    std::stringstream stream;
    stream << "Usage: " << title;

    for (const auto& argument : optionalArguments)
    {
        stream << ' ' << argument.getInlineUsage();
    }
    for (const auto& argument : nonOptionalArguments)
    {
        if (!argument.metavarContent.empty())
        {
            stream << ' ' << argument.metavarContent;
        }
        else
        {
            stream << ' ' << argument.names.front();
        }
    }

    if (!subParserMap.empty())
    {
        stream << " {";
        std::size_t i = 0;
        for (const auto& [command, unused] : subParserMap)
        {
            if (0 == i)
            {
                stream << command;
            }
            else
            {
                stream << ',' << command;
            }
            ++i;
        }
        stream << '}';
    }

    return stream.str();
}

void Argument::addSubParser(Argument& parser)
{
    parser.parserPath = title + ' ' + parser.title;
    auto iterator = subParsers.emplace(std::cend(subParsers), parser);
    subParserMap.insert_or_assign(parser.title, iterator);
    subParserUsed.insert_or_assign(parser.title, false);
}

bool Argument::isValidPrefixChar(const char c) const
{
    return prefixChars.find(c) != std::string::npos;
}

char Argument::getAnyValidPrefixChar() const
{
    return prefixChars[0];
}

std::vector<std::string> Argument::preprocessArguments(const std::vector<std::string>& rawArguments) const
{
    std::vector<std::string> arguments{};
    for (const auto& arg : rawArguments)
    {
        const auto argumentStartsWithPrefixChars = [this](const std::string& str)
        {
            if (!str.empty())
            {
                const auto legalPrefix = [this](char c)
                {
                    return prefixChars.find(c) != std::string::npos;
                };

                const auto windowsStyle = legalPrefix('/');
                if (windowsStyle)
                {
                    if (legalPrefix(str[0]))
                    {
                        return true;
                    }
                }
                else
                {
                    if (str.size() > 1)
                    {
                        return (legalPrefix(str[0]) && legalPrefix(str[1]));
                    }
                }
            }
            return false;
        };

        const auto assignCharPos = arg.find_first_of(assignChars);
        if ((argumentMap.find(arg) == argumentMap.cend()) && argumentStartsWithPrefixChars(arg)
            && (std::string::npos != assignCharPos))
        {
            const std::string optName = arg.substr(0, assignCharPos);
            if (argumentMap.find(optName) != argumentMap.cend())
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
    if (title.empty() && !arguments.empty())
    {
        title = arguments.front();
    }

    const auto end = std::cend(arguments);
    auto nonOptionalArgumentIter = std::begin(nonOptionalArguments);
    for (auto iterator = std::next(std::begin(arguments)); end != iterator;)
    {
        const auto& currentArgument = *iterator;
        if (ArgumentRegister::checkIfNonOptional(currentArgument, prefixChars))
        {
            if (std::cend(nonOptionalArguments) == nonOptionalArgumentIter)
            {
                const std::string_view maybeCommand = currentArgument;
                const auto subParserIter = subParserMap.find(maybeCommand);
                if (subParserMap.cend() != subParserIter)
                {
                    const auto unprocessedArguments = std::vector<std::string>(iterator, end);
                    isParsed = true;
                    subParserUsed[maybeCommand] = true;
                    return subParserIter->second->get().parseArgs(unprocessedArguments);
                }

                throw std::runtime_error("Maximum number of non-optional arguments exceeded.");
            }
            const auto argument = nonOptionalArgumentIter++;
            iterator = argument->consume(iterator, end);
            continue;
        }

        const auto argMapIter = argumentMap.find(currentArgument);
        if (argumentMap.cend() != argMapIter)
        {
            const auto argument = argMapIter->second;
            iterator = argument->consume(std::next(iterator), end, argMapIter->first);
        }
        else if (const auto& compoundArg = currentArgument;
                 (compoundArg.size() > 1) && isValidPrefixChar(compoundArg[0]) && !isValidPrefixChar(compoundArg[1]))
        {
            ++iterator;
            for (std::size_t i = 1; i < compoundArg.size(); ++i)
            {
                const auto hypotheticalArg = std::string{'-', compoundArg[i]};
                const auto argMapIter2 = argumentMap.find(hypotheticalArg);
                if (argumentMap.cend() != argMapIter2)
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

std::vector<std::string> Argument::parseKnownArgsInternal(const std::vector<std::string>& rawArguments)
{
    const auto arguments = preprocessArguments(rawArguments);
    std::vector<std::string> unknownArguments{};
    if (title.empty() && !arguments.empty())
    {
        title = arguments.front();
    }

    const auto end = std::cend(arguments);
    auto nonOptionalArgumentIter = std::begin(nonOptionalArguments);
    for (auto iterator = std::next(std::begin(arguments)); end != iterator;)
    {
        const auto& currentArgument = *iterator;
        if (ArgumentRegister::checkIfNonOptional(currentArgument, prefixChars))
        {
            if (std::cend(nonOptionalArguments) == nonOptionalArgumentIter)
            {
                const std::string_view maybeCommand = currentArgument;
                const auto subParserIter = subParserMap.find(maybeCommand);
                if (subParserMap.cend() != subParserIter)
                {
                    const auto unprocessedArguments = std::vector<std::string>(iterator, end);
                    isParsed = true;
                    subParserUsed[maybeCommand] = true;
                    return subParserIter->second->get().parseKnownArgsInternal(unprocessedArguments);
                }

                unknownArguments.emplace_back(currentArgument);
                ++iterator;
            }
            else
            {
                const auto argument = nonOptionalArgumentIter++;
                iterator = argument->consume(iterator, end);
            }
            continue;
        }

        const auto argMapIter = argumentMap.find(currentArgument);
        if (argumentMap.cend() != argMapIter)
        {
            const auto argument = argMapIter->second;
            iterator = argument->consume(std::next(iterator), end, argMapIter->first);
        }
        else if (const auto& compoundArg = currentArgument;
                 (compoundArg.size() > 1) && isValidPrefixChar(compoundArg[0]) && !isValidPrefixChar(compoundArg[1]))
        {
            ++iterator;
            for (std::size_t i = 1; i < compoundArg.size(); ++i)
            {
                const auto hypotheticalArg = std::string{'-', compoundArg[i]};
                const auto argMapIter2 = argumentMap.find(hypotheticalArg);
                if (argumentMap.cend() != argMapIter2)
                {
                    const auto argument = argMapIter2->second;
                    iterator = argument->consume(iterator, end, argMapIter2->first);
                }
                else
                {
                    unknownArguments.emplace_back(currentArgument);
                    break;
                }
            }
        }
        else
        {
            unknownArguments.emplace_back(currentArgument);
            ++iterator;
        }
    }
    isParsed = true;
    return unknownArguments;
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
        maxSize = std::max<std::size_t>(maxSize, argument->getArgumentsLength());
    }
    for ([[maybe_unused]] const auto& [command, unused] : subParserMap)
    {
        maxSize = std::max<std::size_t>(maxSize, command.size());
    }
    return maxSize;
}

void Argument::indexArgument(ArgumentRegisterIter iterator)
{
    for (const auto& name : std::as_const(iterator->names))
    {
        argumentMap.insert_or_assign(name, iterator);
    }
}

//! @brief The operator (<<) overloading of the Argument class.
//! @param os - output stream object
//! @param arg - specific Argument object
//! @return reference of output stream object
std::ostream& operator<<(std::ostream& os, const Argument& arg)
{
    os.setf(std::ios_base::left);
    const auto longestArgLength = arg.getLengthOfLongestArgument();

    os << arg.usage() << "\n\n";

    if (!arg.descrText.empty())
    {
        os << arg.descrText << "\n\n";
    }

    if (!arg.optionalArguments.empty())
    {
        os << "optional:\n";
    }
    for (const auto& argument : arg.optionalArguments)
    {
        os.width(static_cast<std::streamsize>(longestArgLength));
        os << argument;
    }

    if (!arg.nonOptionalArguments.empty())
    {
        os << (arg.optionalArguments.empty() ? "" : "\n") << "non-optional:\n";
    }
    for (const auto& argument : arg.nonOptionalArguments)
    {
        os.width(static_cast<std::streamsize>(longestArgLength));
        os << argument;
    }

    if (!arg.subParserMap.empty())
    {
        os << (arg.optionalArguments.empty() ? (arg.nonOptionalArguments.empty() ? "" : "\n") : "\n")
           << "sub-commands:\n";
        for (const auto& [command, subParser] : arg.subParserMap)
        {
            os << std::setw(2) << ' ';
            os << std::setw(static_cast<int>(longestArgLength - 2)) << command;
            os << ' ' << subParser->get().descrText << '\n';
        }
    }

    return os;
}
} // namespace utility::argument
