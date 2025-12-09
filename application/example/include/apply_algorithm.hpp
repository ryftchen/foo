//! @file apply_algorithm.hpp
//! @author ryftchen
//! @brief The declarations (apply_algorithm) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#ifndef _PRECOMPILED_HEADER
#include <gsl/gsl_sf.h>
#include <mpfr.h>
#include <cstring>
#include <iostream>
#include <memory>
#else
#include "application/pch/precompiled_header.hpp"
#endif // _PRECOMPILED_HEADER

#include "algorithm/include/match.hpp"
#include "algorithm/include/notation.hpp"
#include "algorithm/include/optimal.hpp"
#include "algorithm/include/search.hpp"
#include "algorithm/include/sort.hpp"

//! @brief The application module.
namespace application // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Algorithm-applying-related functions in the application module.
namespace app_algo
{
//! @brief Apply match.
namespace match
{
//! @brief The version used to apply.
const char* const version = algorithm::match::version();

//! @brief Set input parameters.
namespace input
{
//! @brief Single pattern for match methods.
constexpr std::string_view patternString = "12345";
} // namespace input

//! @brief Maximum number per line of printing.
constexpr std::uint32_t maxNumPerLineOfPrint = 50;

//! @brief Builder for the input.
class InputBuilder
{
public:
    //! @brief Construct a new InputBuilder object.
    //! @param pattern - single pattern
    explicit InputBuilder(const std::string_view pattern) :
        marchingText{std::make_unique<unsigned char[]>(maxDigit + 1)},
        textLen{maxDigit},
        singlePattern{std::make_unique<unsigned char[]>(pattern.length() + 1)},
        patternLen{static_cast<std::uint32_t>(pattern.length())}
    {
        marchingText[0] = '\0';
        singlePattern[0] = '\0';
        createMatchingText(marchingText.get(), maxDigit);
        std::memcpy(singlePattern.get(), pattern.data(), pattern.length() * sizeof(unsigned char));
    }
    //! @brief Destroy the InputBuilder object.
    virtual ~InputBuilder() { ::mpfr_free_cache(); }
    //! @brief Construct a new InputBuilder object.
    InputBuilder(const InputBuilder&) = delete;
    //! @brief Construct a new InputBuilder object.
    InputBuilder(InputBuilder&&) = delete;
    //! @brief The operator (=) overloading of InputBuilder class.
    //! @return reference of the InputBuilder object
    InputBuilder& operator=(const InputBuilder&) = delete;
    //! @brief The operator (=) overloading of InputBuilder class.
    //! @return reference of the InputBuilder object
    InputBuilder& operator=(InputBuilder&&) = delete;

    //! @brief Maximum digit for the target text.
    static constexpr std::uint32_t maxDigit{100'000};
    //! @brief Get the matching text.
    //! @return matching text
    [[nodiscard]] const std::unique_ptr<unsigned char[]>& getMatchingText() const noexcept { return marchingText; }
    //! @brief Get the length of the matching text.
    //! @return length of the matching text
    [[nodiscard]] std::uint32_t getTextLength() const noexcept { return textLen; }
    //! @brief Get the single pattern.
    //! @return single pattern
    [[nodiscard]] const std::unique_ptr<unsigned char[]>& getSinglePattern() const noexcept { return singlePattern; }
    //! @brief Get the length of the single pattern.
    //! @return length of the single pattern
    [[nodiscard]] std::uint32_t getPatternLength() const noexcept { return patternLen; }

private:
    //! @brief Matching text.
    const std::unique_ptr<unsigned char[]> marchingText;
    //! @brief Length of the matching text.
    const std::uint32_t textLen{0};
    //! @brief Single pattern.
    const std::unique_ptr<unsigned char[]> singlePattern;
    //! @brief Length of the single pattern.
    const std::uint32_t patternLen{0};

    //! @brief Base number for converting the digit to precision.
    static constexpr int mpfrBase{10};
    //! @brief Create the matching text.
    //! @param text - target matching text
    //! @param textLen - length of matching text
    static void createMatchingText(unsigned char* const text, const std::uint32_t textLen)
    {
        if (!text || (textLen == 0))
        {
            return;
        }

        ::mpfr_t operand{};
        ::mpfr_init2(operand, calculatePrecision(textLen));
        ::mpfr_const_pi(operand, ::MPFR_RNDN);
        ::mpfr_exp_t decimalLocation = 0;
        char* const piText = ::mpfr_get_str(nullptr, &decimalLocation, mpfrBase, 0, operand, ::MPFR_RNDN);

        if (std::strlen(piText) != 0)
        {
            piText[textLen] = '\0';
            std::memcpy(text, piText, textLen * sizeof(unsigned char));
#ifdef _RUNTIME_PRINTING
            std::string brief(text, text + textLen);
            if (textLen > 1)
            {
                brief = std::string{brief.at(0)} + '.' + brief.substr(1, brief.length());
            }
            std::cout << "\nπ " << textLen << " digits:\n"
                      << brief.substr(0, std::min(textLen, maxNumPerLineOfPrint)) << std::endl;
            if (textLen > maxNumPerLineOfPrint)
            {
                std::cout << "...\n"
                          << ((textLen > (maxNumPerLineOfPrint * 2))
                                  ? brief.substr(brief.length() - maxNumPerLineOfPrint, brief.length())
                                  : brief.substr(maxNumPerLineOfPrint + 1, brief.length()))
                          << std::endl;
            }
#endif // _RUNTIME_PRINTING
        }
        ::mpfr_clear(operand);
        ::mpfr_free_str(piText);
    }
    //! @brief Calculate precision by digit.
    //! @param digit - digit for the target text
    //! @return precision converted from digit
    static int calculatePrecision(const std::uint32_t digit)
    {
        return 1 + static_cast<int>(std::ceil(static_cast<double>(digit) * std::log2(mpfrBase)));
    }
};
} // namespace match
extern void applyingMatch(const std::vector<std::string>& candidates);

//! @brief Apply notation.
namespace notation
{
//! @brief The version used to apply.
const char* const version = algorithm::notation::version();

//! @brief Set input parameters.
namespace input
{
//! @brief Infix string for notation methods.
constexpr std::string_view infixString = "a+b*(c^d-e)^(f+g*h)-i";
} // namespace input

//! @brief Builder for the input.
class InputBuilder
{
public:
    //! @brief Construct a new InputBuilder object.
    //! @param infixNotation - infix notation
    explicit InputBuilder(const std::string_view infixNotation) : infixNotation{infixNotation}
    {
#ifdef _RUNTIME_PRINTING
        std::cout << "\nInfix notation:\n" << infixNotation << std::endl;
#endif // _RUNTIME_PRINTING
    }

    //! @brief Get the infix notation.
    //! @return infix notation
    [[nodiscard]] std::string getInfixNotation() const noexcept { return infixNotation; }

private:
    //! @brief Infix notation.
    const std::string infixNotation;
};
} // namespace notation
extern void applyingNotation(const std::vector<std::string>& candidates);

//! @brief Apply optimal.
namespace optimal
{
//! @brief The version used to apply.
const char* const version = algorithm::optimal::version();

//! @brief Alias for the target function.
using Function = std::function<double(const double)>;
//! @brief Wrapper for the target function.
class FuncIntf
{
public:
    //! @brief Construct a new FuncIntf object.
    FuncIntf() = default;
    //! @brief Destroy the FuncIntf object.
    virtual ~FuncIntf() = default;
    //! @brief Construct a new FuncIntf object.
    FuncIntf(const FuncIntf&) = default;
    //! @brief Construct a new FuncIntf object.
    FuncIntf(FuncIntf&&) noexcept = default;
    //! @brief The operator (=) overloading of FuncIntf class.
    //! @return reference of the FuncIntf object
    FuncIntf& operator=(const FuncIntf&) = default;
    //! @brief The operator (=) overloading of FuncIntf class.
    //! @return reference of the FuncIntf object
    FuncIntf& operator=(FuncIntf&&) noexcept = default;

    //! @brief The operator (()) overloading of FuncIntf class.
    //! @param x - independent variable
    //! @return dependent variable
    virtual double operator()(const double x) const = 0;
    //! @brief The operator (Function) overloading of FuncIntf class.
    //! @return Function object
    virtual explicit operator Function() const
    {
        return [this](const double x) { return operator()(x); };
    }
};

//! @brief Set input parameters.
namespace input
{
//! @brief Spherical Bessel.
class SphericalBessel : public FuncIntf
{
public:
    //! @brief The operator (()) overloading of SphericalBessel class.
    //! @param x - independent variable
    //! @return dependent variable
    double operator()(const double x) const override { return ::gsl_sf_bessel_j0(x); }

    //! @brief Left endpoint.
    static constexpr double range1{0.0};
    //! @brief Right endpoint.
    static constexpr double range2{20.0};
    //! @brief Spherical Bessel function of the first kind.
    static constexpr std::string_view funcDescr{"f(x)=j₀(x),x∈[0,20] (Spherical Bessel function of the first kind)"};
};
} // namespace input

//! @brief Builder for the input.
class InputBuilder
{
public:
    //! @brief Construct a new Input Builder object.
    //! @param function - target function
    //! @param range1 - left endpoint
    //! @param range2 - right endpoint
    //! @param funcDescr - function description
    InputBuilder(
        Function function,
        const double range1,
        const double range2,
        [[maybe_unused]] const std::string_view funcDescr) :
        function{std::move(function)}, range1{range1}, range2{range2}
    {
#ifdef _RUNTIME_PRINTING
        std::cout << "\nOptimal function:\n" << funcDescr << std::endl;
#endif // _RUNTIME_PRINTING
    }

    //! @brief Get the target function.
    //! @return target function
    [[nodiscard]] Function getFunction() const noexcept { return function; }
    //! @brief Get the pair of ranges.
    //! @return pair of ranges
    [[nodiscard]] std::pair<double, double> getRanges() const noexcept { return std::make_pair(range1, range2); }

private:
    //! @brief Target function.
    const Function function;
    //! @brief Left endpoint.
    const double range1{0.0};
    //! @brief Right endpoint.
    const double range2{0.0};
};
} // namespace optimal
extern void applyingOptimal(const std::vector<std::string>& candidates);

//! @brief Apply search.
namespace search
{
//! @brief The version used to apply.
const char* const version = algorithm::search::version();

//! @brief Set input parameters.
namespace input
{
//! @brief Minimum of the array for search methods.
constexpr float arrayRangeMin = -50.0;
//! @brief Maximum of the array for search methods.
constexpr float arrayRangeMax = 150.0;
//! @brief Length of the array for search methods.
constexpr std::uint32_t arrayLength = 53;
} // namespace input

//! @brief Maximum alignment length per element of printing.
constexpr std::uint8_t maxAlignOfPrint = 16;
//! @brief Maximum columns per row of printing.
constexpr std::uint8_t maxColumnOfPrint = 5;

//! @brief Builder for the input.
//! @tparam Elem - type of builder for the target
template <typename Elem>
class InputBuilder
{
public:
    //! @brief Construct a new InputBuilder object.
    //! @param length - length of array
    //! @param left - left boundary of the array
    //! @param right - right boundary of the array
    InputBuilder(const std::uint32_t length, const Elem left, const Elem right) :
        orderedArray{std::make_unique<Elem[]>(length + 1)}, length{length}
    {
        orderedArray[0] = '\0';
        setOrderedArray(orderedArray.get(), length, left, right);
    }
    //! @brief Destroy the InputBuilder object.
    virtual ~InputBuilder() = default;
    //! @brief Construct a new InputBuilder object.
    //! @param rhs - right-hand side
    InputBuilder(const InputBuilder& rhs) : orderedArray{std::make_unique<Elem[]>(rhs.length + 1)}, length{rhs.length}
    {
        orderedArray[0] = '\0';
        clone(rhs);
    }
    //! @brief Construct a new InputBuilder object.
    InputBuilder(InputBuilder&&) noexcept = default;
    //! @brief The operator (!=) overloading of InputBuilder class.
    //! @param rhs - right-hand side
    //! @return reference of the InputBuilder object
    InputBuilder<Elem>& operator=(const InputBuilder& rhs)
    {
        if (&rhs != this)
        {
            clone(rhs);
        }
        return *this;
    }
    //! @brief The operator (=) overloading of InputBuilder class.
    //! @return reference of the InputBuilder object
    InputBuilder& operator=(InputBuilder&&) noexcept = default;

    //! @brief Get the ordered array.
    //! @return ordered array
    const std::unique_ptr<Elem[]>& getOrderedArray() const noexcept { return orderedArray; }
    //! @brief Get the length.
    //! @return length
    [[nodiscard]] std::uint32_t getLength() const noexcept { return length; }
    //! @brief Get the search key.
    //! @return search key
    Elem getSearchKey() const noexcept { return orderedArray[length / 2]; }
    //! @brief Splice from array for printing.
    //! @param array - target array
    //! @param length - length of array
    //! @param fmtBuffer - buffer for printing
    //! @param bufferSize - size of the buffer
    //! @return buffer after splicing
    static char* spliceAll(
        const Elem* const array, const std::uint32_t length, char* const fmtBuffer, const std::uint32_t bufferSize)
    {
        if (!array || (length == 0) || !fmtBuffer || (bufferSize == 0))
        {
            return fmtBuffer;
        }

        std::uint32_t align = 0;
        for (std::uint32_t i = 0; i < length; ++i)
        {
            align = std::max(static_cast<std::uint32_t>(std::to_string(*(array + i)).length()), align);
        }

        std::array<char, 16> spliceFmt{};
        if constexpr (std::is_integral_v<Elem>)
        {
            std::snprintf(spliceFmt.data(), spliceFmt.size(), "%%%dd%%c", align + 1);
        }
        else if constexpr (std::is_floating_point_v<Elem>)
        {
            std::snprintf(spliceFmt.data(), spliceFmt.size(), "%%%d.5f%%c", align + 1);
        }
        else
        {
            return fmtBuffer;
        }

        for (std::uint32_t i = 0, offset = 0; i < length; ++i)
        {
            const char sep = (((i + 1) % maxColumnOfPrint == 0) && ((i + 1) != length)) ? '\n' : ' ';
            const int written =
                std::snprintf(fmtBuffer + offset, bufferSize - offset, spliceFmt.data(), *(array + i), sep);
            if ((written < 0) || (written >= static_cast<int>(bufferSize - offset)))
            {
                break;
            }
            offset += written;
        }
        return fmtBuffer;
    }

private:
    //! @brief Ordered array.
    const std::unique_ptr<Elem[]> orderedArray{};
    //! @brief Length of the ordered array.
    const std::uint32_t length{0};

    //! @brief Deep copy for copy constructor.
    //! @param rhs - right-hand side
    void clone(const InputBuilder& rhs) const
    {
        if (rhs.orderedArray && orderedArray)
        {
            std::memcpy(orderedArray.get(), rhs.orderedArray.get(), length * sizeof(Elem));
        }
    }
    //! @brief Set the ordered array.
    //! @param array - ordered array
    //! @param length - length of the ordered array
    //! @param left - left boundary of the ordered array
    //! @param right - left right of the ordered array
    static void setOrderedArray(Elem array[], const std::uint32_t length, const Elem left, const Elem right)
    requires std::is_integral_v<Elem>
    {
        if (!array || (length == 0) || (left > right))
        {
            return;
        }

        std::ranlux48 engine(std::random_device{}());
        std::uniform_int_distribution<Elem> dist(left, right);
        for (std::uint32_t i = 0; i < length; ++i)
        {
            array[i] = dist(engine);
        }
        std::sort(array, array + length);
#ifdef _RUNTIME_PRINTING
        const std::uint32_t bufferSize = length * maxAlignOfPrint;
        std::vector<char> fmtBuffer(bufferSize + 1);
        std::cout << "\nGenerate " << length << " ordered integral numbers from " << left << " to " << right << ":\n"
                  << spliceAll(array, length, fmtBuffer.data(), bufferSize + 1) << std::endl;
#endif // _RUNTIME_PRINTING
    }
    //! @brief Set the ordered array.
    //! @param array - ordered array
    //! @param length - length of the ordered array
    //! @param left - left boundary of the ordered array
    //! @param right - left right of the ordered array
    static void setOrderedArray(Elem array[], const std::uint32_t length, const Elem left, const Elem right)
    requires std::is_floating_point_v<Elem>
    {
        if (!array || (length == 0) || (left > right))
        {
            return;
        }

        std::ranlux48 engine(std::random_device{}());
        std::uniform_real_distribution<Elem> dist(left, right);
        for (std::uint32_t i = 0; i < length; ++i)
        {
            array[i] = dist(engine);
        }
        std::sort(array, array + length);
#ifdef _RUNTIME_PRINTING
        const std::uint32_t bufferSize = length * maxAlignOfPrint;
        std::vector<char> fmtBuffer(bufferSize + 1);
        std::cout << "\nGenerate " << length << " ordered floating point numbers from " << left << " to " << right
                  << ":\n"
                  << spliceAll(array, length, fmtBuffer.data(), bufferSize + 1) << std::endl;
#endif // _RUNTIME_PRINTING
    }
};
} // namespace search
extern void applyingSearch(const std::vector<std::string>& candidates);

//! @brief Apply sort.
namespace sort
{
//! @brief The version used to apply.
const char* const version = algorithm::sort::version();

//! @brief Set input parameters.
namespace input
{
//! @brief Minimum of the array for sort methods.
constexpr std::int32_t arrayRangeMin = -50;
//! @brief Maximum of the array for sort methods.
constexpr std::int32_t arrayRangeMax = 150;
//! @brief Length of the array for sort methods.
constexpr std::uint32_t arrayLength = 53;
} // namespace input

//! @brief Maximum alignment length per element of printing.
constexpr std::uint8_t maxAlignOfPrint = 16;
//! @brief Maximum columns per row of printing.
constexpr std::uint8_t maxColumnOfPrint = 10;

//! @brief Builder for the input.
//! @tparam Elem - type of builder for the target
template <typename Elem>
class InputBuilder
{
public:
    //! @brief Construct a new InputBuilder object.
    //! @param length - length of array
    //! @param left - left boundary of the array
    //! @param right - right boundary of the array
    InputBuilder(const std::uint32_t length, const Elem left, const Elem right) :
        randomArray{std::make_unique<Elem[]>(length + 1)}, length{length}
    {
        randomArray[0] = '\0';
        setRandomArray(randomArray.get(), length, left, right);
    }
    //! @brief Destroy the InputBuilder object.
    virtual ~InputBuilder() = default;
    //! @brief Construct a new InputBuilder object.
    //! @param rhs - right-hand side
    InputBuilder(const InputBuilder& rhs) : randomArray{std::make_unique<Elem[]>(rhs.length + 1)}, length{rhs.length}
    {
        randomArray[0] = '\0';
        clone(rhs);
    }
    //! @brief Construct a new InputBuilder object.
    InputBuilder(InputBuilder&&) noexcept = default;
    //! @brief The operator (!=) overloading of InputBuilder class.
    //! @param rhs - right-hand side
    //! @return reference of the InputBuilder object
    InputBuilder<Elem>& operator=(const InputBuilder& rhs)
    {
        if (&rhs != this)
        {
            clone(rhs);
        }
        return *this;
    }
    //! @brief The operator (=) overloading of InputBuilder class.
    //! @return reference of the InputBuilder object
    InputBuilder& operator=(InputBuilder&&) noexcept = default;

    //! @brief Get the random array.
    //! @return random array
    const std::unique_ptr<Elem[]>& getRandomArray() const noexcept { return randomArray; }
    //! @brief Get the length.
    //! @return length
    [[nodiscard]] std::uint32_t getLength() const noexcept { return length; }
    //! @brief Splice from array for printing.
    //! @param array - target array
    //! @param length - length of array
    //! @param fmtBuffer - buffer for printing
    //! @param bufferSize - size of the buffer
    //! @return buffer after splicing
    static char* spliceAll(
        const Elem* const array, const std::uint32_t length, char* const fmtBuffer, const std::uint32_t bufferSize)
    {
        if (!array || (length == 0) || !fmtBuffer || (bufferSize == 0))
        {
            return fmtBuffer;
        }

        std::uint32_t align = 0;
        for (std::uint32_t i = 0; i < length; ++i)
        {
            align = std::max(static_cast<std::uint32_t>(std::to_string(*(array + i)).length()), align);
        }

        std::array<char, 16> spliceFmt{};
        if constexpr (std::is_integral_v<Elem>)
        {
            std::snprintf(spliceFmt.data(), spliceFmt.size(), "%%%dd%%c", align + 1);
        }
        else if constexpr (std::is_floating_point_v<Elem>)
        {
            std::snprintf(spliceFmt.data(), spliceFmt.size(), "%%%d.5f%%c", align + 1);
        }
        else
        {
            return fmtBuffer;
        }

        for (std::uint32_t i = 0, offset = 0; i < length; ++i)
        {
            const char sep = (((i + 1) % maxColumnOfPrint == 0) && ((i + 1) != length)) ? '\n' : ' ';
            const int written =
                std::snprintf(fmtBuffer + offset, bufferSize - offset, spliceFmt.data(), *(array + i), sep);
            if ((written < 0) || (written >= static_cast<int>(bufferSize - offset)))
            {
                break;
            }
            offset += written;
        }
        return fmtBuffer;
    }

private:
    //! @brief Random array.
    const std::unique_ptr<Elem[]> randomArray{};
    //! @brief Length of the random array.
    const std::uint32_t length{0};

    //! @brief Deep copy for copy constructor.
    //! @param rhs - right-hand side
    void clone(const InputBuilder& rhs) const
    {
        if (rhs.randomArray && randomArray)
        {
            std::memcpy(randomArray.get(), rhs.randomArray.get(), length * sizeof(Elem));
        }
    }
    //! @brief Set the random array.
    //! @param array - random array
    //! @param length - length of the random array
    //! @param left - left boundary of the random array
    //! @param right - left right of the random array
    static void setRandomArray(Elem array[], const std::uint32_t length, const Elem left, const Elem right)
    requires std::is_integral_v<Elem>
    {
        if (!array || (length == 0) || (left > right))
        {
            return;
        }

        std::ranlux48 engine(std::random_device{}());
        std::uniform_int_distribution<Elem> dist(left, right);
        for (std::uint32_t i = 0; i < length; ++i)
        {
            array[i] = dist(engine);
        }
#ifdef _RUNTIME_PRINTING
        const std::uint32_t bufferSize = length * maxAlignOfPrint;
        std::vector<char> fmtBuffer(bufferSize + 1);
        std::cout << "\nGenerate " << length << " random integral numbers from " << left << " to " << right << ":\n"
                  << spliceAll(array, length, fmtBuffer.data(), bufferSize + 1) << std::endl;
#endif // _RUNTIME_PRINTING
    }
    //! @brief Set the random array.
    //! @param array - random array
    //! @param length - length of the random array
    //! @param left - left boundary of the random array
    //! @param right - left right of the random array
    static void setRandomArray(Elem array[], const std::uint32_t length, const Elem left, const Elem right)
    requires std::is_floating_point_v<Elem>
    {
        if (!array || (length == 0) || (left > right))
        {
            return;
        }

        std::ranlux48 engine(std::random_device{}());
        std::uniform_real_distribution<Elem> dist(left, right);
        for (std::uint32_t i = 0; i < length; ++i)
        {
            array[i] = dist(engine);
        }
#ifdef _RUNTIME_PRINTING
        const std::uint32_t bufferSize = length * maxAlignOfPrint;
        std::vector<char> fmtBuffer(bufferSize + 1);
        std::cout << "\nGenerate " << length << " random floating point numbers from " << left << " to " << right
                  << ":\n"
                  << spliceAll(array, length, fmtBuffer.data(), bufferSize + 1) << std::endl;
#endif // _RUNTIME_PRINTING
    }
};
} // namespace sort
extern void applyingSort(const std::vector<std::string>& candidates);
} // namespace app_algo
} // namespace application
