#include "argument.hpp"
#include <numeric>

ArgumentRegister& ArgumentRegister::help(std::string str)
{
    helpStr = std::move(str);
    return *this;
}

ArgumentRegister& ArgumentRegister::required()
{
    isRequired = true;
    return *this;
}

ArgumentRegister& ArgumentRegister::implicitValue(std::any value)
{
    implicitValues = std::move(value);
    argNumArgsRange = NArgsRange{0, 0};
    return *this;
}

auto& ArgumentRegister::append()
{
    isRepeatable = true;
    return *this;
}

ArgumentRegister& ArgumentRegister::nArgs(std::size_t numArgs)
{
    argNumArgsRange = NArgsRange{numArgs, numArgs};
    return *this;
}

ArgumentRegister& ArgumentRegister::nArgs(std::size_t numArgsMin, std::size_t numArgsMax)
{
    argNumArgsRange = NArgsRange{numArgsMin, numArgsMax};
    return *this;
}

ArgumentRegister& ArgumentRegister::nArgs(NArgsPattern pattern)
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

ArgumentRegister& ArgumentRegister::remaining()
{
    isAcceptOptionalLikeValue = true;
    return nArgs(NArgsPattern::any);
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
        if (!argNumArgsRange.contains(values.size()) && !defaultValues.has_value())
        {
            throwNargsRangeValidationException();
        }
    }
}

std::size_t ArgumentRegister::getArgumentsLength() const
{
    return std::accumulate(
        std::begin(names), std::end(names), std::size_t(0),
        [](const auto& sum, const auto& str)
        {
            return sum + str.size() + 1;
        });
}

std::ostream& operator<<(std::ostream& stream, const ArgumentRegister& argument)
{
    std::stringstream nameStream;
    std::copy(
        std::begin(argument.names), std::end(argument.names),
        std::ostream_iterator<std::string>(nameStream, " "));
    stream << nameStream.str() << "\t" << argument.helpStr;
    if (argument.defaultValues.has_value())
    {
        if (!argument.helpStr.empty())
        {
            stream << " ";
        }
        stream << "[default: " << argument.defaultValueRepresent << "]";
    }
    else if (argument.isRequired)
    {
        if (!argument.helpStr.empty())
        {
            stream << " ";
        }
        stream << "[required]";
    }
    stream << "\n";
    return stream;
}

void ArgumentRegister::throwNargsRangeValidationException() const
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

auto ArgumentRegister::lookAhead(std::string_view str) -> int
{
    if (str.empty())
    {
        return eof;
    }
    return static_cast<int>(static_cast<unsigned char>(str[0]));
}

bool ArgumentRegister::checkIfOptional(std::string_view name)
{
    return !checkIfPositional(name);
}

bool ArgumentRegister::checkIfPositional(std::string_view name)
{
    switch (lookAhead(name))
    {
        case eof:
            return true;
        case '-':
        {
            name.remove_prefix(1);
            if (name.empty())
            {
                return true;
            }
            return false;
        }
        default:
            return true;
    }
}

Argument::Argument(std::string programName, std::string programVersion) :
    programName(std::move(programName)), programVersion(std::move(programVersion))
{
}

Argument::Argument(const Argument& arg) :
    programName(arg.programName), programVersion(arg.programVersion), isParsed(arg.isParsed),
    positionalArguments(arg.positionalArguments), optionalArguments(arg.optionalArguments)
{
    for (auto iterator = std::begin(positionalArguments); std::end(positionalArguments) != iterator;
         ++iterator)
    {
        indexArgument(iterator);
    }
    for (auto iterator = std::begin(optionalArguments); std::end(optionalArguments) != iterator;
         ++iterator)
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

bool Argument::isUsed(std::string_view argName) const
{
    return (*this)[argName].isUsed;
}

ArgumentRegister& Argument::operator[](std::string_view argName) const
{
    auto iterator = argumentMap.find(argName);
    if (argumentMap.end() != iterator)
    {
        return *(iterator->second);
    }
    if ('-' != argName.front())
    {
        std::string name(argName);

        name = "-" + name;
        iterator = argumentMap.find(name);
        if (argumentMap.end() != iterator)
        {
            return *(iterator->second);
        }

        name = "-" + name;
        iterator = argumentMap.find(name);
        if (argumentMap.end() != iterator)
        {
            return *(iterator->second);
        }
    }
    throw std::logic_error("No such argument: " + std::string(argName));
}

auto operator<<(std::ostream& stream, const Argument& parser) -> std::ostream&
{
    stream.setf(std::ios_base::left);
    stream << "Usage: " << parser.programName << " [options] ";
    std::size_t longestArgLength = parser.getLengthOfLongestArgument();

    for (const auto& argument : parser.positionalArguments)
    {
        stream << argument.names.front() << " ";
    }
    stream << "\n\n";

    if (!parser.positionalArguments.empty())
    {
        stream << "Positional arguments:\n";
    }

    for (const auto& argument : parser.positionalArguments)
    {
        stream.width(longestArgLength);
        stream << argument;
    }

    if (!parser.optionalArguments.empty())
    {
        stream << (parser.positionalArguments.empty() ? "" : "\n") << "Optional arguments:\n";
    }

    for (const auto& argument : parser.optionalArguments)
    {
        stream.width(longestArgLength);
        stream << argument;
    }

    return stream;
}

auto Argument::help() const -> std::stringstream
{
    std::stringstream out;
    out << *this;
    return out;
}

void Argument::parseArgsInternal(const std::vector<std::string>& arguments)
{
    if (programName.empty() && !arguments.empty())
    {
        programName = arguments.front();
    }
    auto end = std::end(arguments);
    auto iterPositionalArgument = std::begin(positionalArguments);
    for (auto iterator = std::next(std::begin(arguments)); end != iterator;)
    {
        const auto& currentArgument = *iterator;
        if (ArgumentRegister::checkIfPositional(currentArgument))
        {
            if (std::end(positionalArguments) == iterPositionalArgument)
            {
                throw std::runtime_error("Maximum number of positional arguments exceeded.");
            }
            auto argument = iterPositionalArgument++;
            iterator = argument->consume(iterator, end);
            continue;
        }

        auto iterArgMap = argumentMap.find(currentArgument);
        if (argumentMap.end() != iterArgMap)
        {
            auto argument = iterArgMap->second;
            iterator = argument->consume(std::next(iterator), end, iterArgMap->first);
        }
        else if (const auto& compoundArg = currentArgument;
                 (compoundArg.size() > 1) && ('-' == compoundArg[0]) && ('-' != compoundArg[1]))
        {
            ++iterator;
            for (std::size_t j = 1; j < compoundArg.size(); ++j)
            {
                auto hypotheticalArg = std::string{'-', compoundArg[j]};
                auto iterArgMap2 = argumentMap.find(hypotheticalArg);
                if (argumentMap.end() != iterArgMap2)
                {
                    auto argument = iterArgMap2->second;
                    iterator = argument->consume(iterator, end, iterArgMap2->first);
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
