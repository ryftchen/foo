//! @file apply_algorithm.hpp
//! @author ryftchen
//! @brief The declarations (apply_algorithm) in the application module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen. All rights reserved.

#pragma once

#ifndef __PRECOMPILED_HEADER
#include <mpfr.h>
#include <algorithm>
#include <bitset>
#include <cassert>
#include <cstring>
#include <iostream>
#include <memory>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER
#include "algorithm/include/optimal.hpp"

#if defined(__clang__) || defined(__GNUC__)
//! @brief The restrict type qualifier.
#define restrict __restrict // NOLINT(readability-identifier-naming)
#else
#define restrict // NOLINT(readability-identifier-naming)
#endif // defined(__clang__) || defined(__GNUC__)

//! @brief Algorithm-applying-related functions in the application module.
namespace application::app_algo
{
//! @brief Manage algorithm tasks.
class AlgorithmTask
{
public:
    //! @brief Represent the maximum value of an enum.
    //! @tparam T - type of specific enum
    template <class T>
    struct Bottom;

    //! @brief Enumerate specific algorithm tasks.
    enum Category : std::uint8_t
    {
        //! @brief Match.
        match,
        //! @brief Notation.
        notation,
        //! @brief Optimal.
        optimal,
        //! @brief Search.
        search,
        //! @brief Sort.
        sort
    };

    //! @brief Enumerate specific match methods.
    enum MatchMethod : std::uint8_t
    {
        //! @brief Rabin-Karp.
        rabinKarp,
        //! @brief Knuth-Morris-Pratt.
        knuthMorrisPratt,
        //! @brief Boyer-Moore.
        boyerMoore,
        //! @brief Horspool.
        horspool,
        //! @brief Sunday.
        sunday
    };
    //! @brief Store the maximum value of the MatchMethod enum.
    template <>
    struct Bottom<MatchMethod>
    {
        //! @brief Maximum value of the MatchMethod enum.
        static constexpr std::uint8_t value{5};
    };

    //! @brief Enumerate specific notation methods.
    enum NotationMethod : std::uint8_t
    {
        //! @brief Prefix.
        prefix,
        //! @brief Postfix.
        postfix
    };
    //! @brief Store the maximum value of the NotationMethod enum.
    template <>
    struct Bottom<NotationMethod>
    {
        //! @brief Maximum value of the NotationMethod enum.
        static constexpr std::uint8_t value{2};
    };

    //! @brief Enumerate specific optimal methods.
    enum OptimalMethod : std::uint8_t
    {
        //! @brief Gradient.
        gradient,
        //! @brief Annealing.
        annealing,
        //! @brief Particle.
        particle,
        //! @brief Genetic.
        genetic
    };
    //! @brief Store the maximum value of the OptimalMethod enum.
    template <>
    struct Bottom<OptimalMethod>
    {
        //! @brief Maximum value of the OptimalMethod enum.
        static constexpr std::uint8_t value{4};
    };

    //! @brief Enumerate specific search methods.
    enum SearchMethod : std::uint8_t
    {
        //! @brief Binary.
        binary,
        //! @brief Interpolation.
        interpolation,
        //! @brief Fibonacci.
        fibonacci
    };
    //! @brief Store the maximum value of the SearchMethod enum.
    template <>
    struct Bottom<SearchMethod>
    {
        //! @brief Maximum value of the SearchMethod enum.
        static constexpr std::uint8_t value{3};
    };

    //! @brief Enumerate specific sort methods.
    enum SortMethod : std::uint8_t
    {
        //! @brief Bubble.
        bubble,
        //! @brief Selection.
        selection,
        //! @brief Insertion.
        insertion,
        //! @brief Shell.
        shell,
        //! @brief Merge.
        merge,
        //! @brief Quick.
        quick,
        //! @brief Heap.
        heap,
        //! @brief Counting.
        counting,
        //! @brief Bucket.
        bucket,
        //! @brief Radix.
        radix
    };
    //! @brief Store the maximum value of the SortMethod enum.
    template <>
    struct Bottom<SortMethod>
    {
        //! @brief Maximum value of the SortMethod enum.
        static constexpr std::uint8_t value{10};
    };

    //! @brief Bit flags for managing match methods.
    std::bitset<Bottom<MatchMethod>::value> matchBit;
    //! @brief Bit flags for managing notation methods.
    std::bitset<Bottom<NotationMethod>::value> notationBit;
    //! @brief Bit flags for managing optimal methods.
    std::bitset<Bottom<OptimalMethod>::value> optimalBit;
    //! @brief Bit flags for managing search methods.
    std::bitset<Bottom<SearchMethod>::value> searchBit;
    //! @brief Bit flags for managing sort methods.
    std::bitset<Bottom<SortMethod>::value> sortBit;

    //! @brief Check whether any algorithm tasks do not exist.
    //! @return any algorithm tasks do not exist or exist
    [[nodiscard]] inline bool empty() const
    {
        return (matchBit.none() && notationBit.none() && optimalBit.none() && searchBit.none() && sortBit.none());
    }
    //! @brief Reset bit flags that manage algorithm tasks.
    inline void reset()
    {
        matchBit.reset();
        notationBit.reset();
        optimalBit.reset();
        searchBit.reset();
        sortBit.reset();
    }

protected:
    //! @brief The operator (<<) overloading of the Category enum.
    //! @param os - output stream object
    //! @param cat - the specific value of Category enum
    //! @return reference of the output stream object
    friend std::ostream& operator<<(std::ostream& os, const Category cat)
    {
        switch (cat)
        {
            case Category::match:
                os << "MATCH";
                break;
            case Category::notation:
                os << "NOTATION";
                break;
            case Category::optimal:
                os << "OPTIMAL";
                break;
            case Category::search:
                os << "SEARCH";
                break;
            case Category::sort:
                os << "SORT";
                break;
            default:
                os << "UNKNOWN: " << static_cast<std::underlying_type_t<Category>>(cat);
        }
        return os;
    }
};
extern AlgorithmTask& getTask();

//! @brief Get the bit flags of the method in algorithm tasks.
//! @tparam T - type of method
//! @return reference of the method bit flags
template <typename T>
auto& getBit()
{
    if constexpr (std::is_same_v<T, AlgorithmTask::MatchMethod>)
    {
        return getTask().matchBit;
    }
    else if constexpr (std::is_same_v<T, AlgorithmTask::NotationMethod>)
    {
        return getTask().notationBit;
    }
    else if constexpr (std::is_same_v<T, AlgorithmTask::OptimalMethod>)
    {
        return getTask().optimalBit;
    }
    else if constexpr (std::is_same_v<T, AlgorithmTask::SearchMethod>)
    {
        return getTask().searchBit;
    }
    else if constexpr (std::is_same_v<T, AlgorithmTask::SortMethod>)
    {
        return getTask().sortBit;
    }
}

//! @brief Set the bit flags of the method in algorithm tasks
//! @tparam T - type of method
//! @param index - method index
template <typename T>
void setBit(const int index)
{
    if constexpr (std::is_same_v<T, AlgorithmTask::MatchMethod>)
    {
        getTask().matchBit.set(AlgorithmTask::MatchMethod(index));
    }
    else if constexpr (std::is_same_v<T, AlgorithmTask::NotationMethod>)
    {
        getTask().notationBit.set(AlgorithmTask::NotationMethod(index));
    }
    else if constexpr (std::is_same_v<T, AlgorithmTask::OptimalMethod>)
    {
        getTask().optimalBit.set(AlgorithmTask::OptimalMethod(index));
    }
    else if constexpr (std::is_same_v<T, AlgorithmTask::SearchMethod>)
    {
        getTask().searchBit.set(AlgorithmTask::SearchMethod(index));
    }
    else if constexpr (std::is_same_v<T, AlgorithmTask::SortMethod>)
    {
        getTask().sortBit.set(AlgorithmTask::SortMethod(index));
    }
}

//! @brief Apply match.
namespace match
{
//! @brief Set input parameters.
namespace input
{
//! @brief Single pattern for match methods.
constexpr std::string_view patternString = "12345";
} // namespace input

//! @brief Solution of match.
class MatchSolution
{
public:
    //! @brief Destroy the MatchSolution object.
    virtual ~MatchSolution() = default;

    //! @brief The Rabin-Karp method.
    //! @param text - matching text
    //! @param pattern - single pattern
    //! @param textLen - length of matching text
    //! @param patternLen - length of single pattern
    static void rkMethod(
        const char* text,
        const char* pattern,
        const std::uint32_t textLen,
        const std::uint32_t patternLen);
    //! @brief The Knuth-Morris-Pratt method.
    //! @param text - matching text
    //! @param pattern - single pattern
    //! @param textLen - length of matching text
    //! @param patternLen - length of single pattern
    static void kmpMethod(
        const char* text,
        const char* pattern,
        const std::uint32_t textLen,
        const std::uint32_t patternLen);
    //! @brief The Boyer-Moore method.
    //! @param text - matching text
    //! @param pattern - single pattern
    //! @param textLen - length of matching text
    //! @param patternLen - length of single pattern
    static void bmMethod(
        const char* text,
        const char* pattern,
        const std::uint32_t textLen,
        const std::uint32_t patternLen);
    //! @brief The Horspool method.
    //! @param text - matching text
    //! @param pattern - single pattern
    //! @param textLen - length of matching text
    //! @param patternLen - length of single pattern
    static void horspoolMethod(
        const char* text,
        const char* pattern,
        const std::uint32_t textLen,
        const std::uint32_t patternLen);
    //! @brief The Sunday method.
    //! @param text - matching text
    //! @param pattern - single pattern
    //! @param textLen - length of matching text
    //! @param patternLen - length of single pattern
    static void sundayMethod(
        const char* text,
        const char* pattern,
        const std::uint32_t textLen,
        const std::uint32_t patternLen);
};

//! @brief Maximum number per line of printing.
constexpr std::uint32_t maxNumPerLineOfPrint = 50;

//! @brief Builder for the target.
class TargetBuilder
{
public:
    //! @brief Construct a new TargetBuilder object.
    //! @param singlePattern - single pattern
    explicit TargetBuilder(const std::string_view singlePattern) :
        marchingText(std::make_unique<char[]>(calculatePrecision(maxDigit))), singlePattern(singlePattern)
    {
        setMatchingText(marchingText.get(), maxDigit);
    }
    //! @brief Destroy the TargetBuilder object.
    virtual ~TargetBuilder() { ::mpfr_free_cache(); }

    //! @brief Construct a new TargetBuilder object.
    TargetBuilder(const TargetBuilder&) = delete;
    //! @brief The operator (=) overloading of TargetBuilder class.
    //! @return reference of the TargetBuilder object
    TargetBuilder& operator=(const TargetBuilder&) = delete;

    //! @brief Maximum digit for the target text.
    static constexpr std::uint32_t maxDigit{100000};

    //! @brief Get the matching text.
    //! @return matching text
    [[nodiscard]] inline const std::unique_ptr<char[]>& getMatchingText() const { return marchingText; }
    //! @brief Get the single pattern.
    //! @return single pattern
    [[nodiscard]] inline std::string_view getSinglePattern() const { return singlePattern; }

private:
    //! @brief Matching text.
    const std::unique_ptr<char[]> marchingText;
    //! @brief Single pattern.
    const std::string_view singlePattern;

    //! @brief Base number for converting the digit to precision.
    static constexpr int mpfrBase{10};

    //! @brief Set the matching text.
    //! @param text - target matching text
    //! @param textLen - length of matching text
    static void setMatchingText(char* text, const std::uint32_t textLen)
    {
        assert((nullptr != text) && (textLen > 0));
        ::mpfr_t x;
        ::mpfr_init2(x, calculatePrecision(textLen));
        ::mpfr_const_pi(x, ::MPFR_RNDN);
        ::mpfr_exp_t mpfrDecimalLocation;
        ::mpfr_get_str(text, &mpfrDecimalLocation, mpfrBase, 0, x, ::MPFR_RNDN);
        ::mpfr_clear(x);

        assert('\0' != *text);
        text[textLen] = '\0';

#ifdef __RUNTIME_PRINTING
        std::string out(text);
        out.insert(1, ".");
        std::cout << "\r\nπ " << textLen << " digits:\n"
                  << out.substr(0, std::min(textLen, maxNumPerLineOfPrint)) << std::endl;
        if (textLen > maxNumPerLineOfPrint)
        {
            std::cout << "...\n...\n...\n"
                      << ((textLen > (maxNumPerLineOfPrint * 2))
                              ? out.substr(out.length() - maxNumPerLineOfPrint, out.length())
                              : out.substr(maxNumPerLineOfPrint + 1, out.length()))
                      << std::endl;
        }
#endif // __RUNTIME_PRINTING
    }
    //! @brief Calculate precision by digit.
    //! @param digit - digit for the target text
    //! @return precision converted from digit
    static inline int calculatePrecision(const std::uint32_t digit)
    {
        return static_cast<int>(std::ceil(static_cast<double>(digit) * std::log2(mpfrBase)));
    }
};
} // namespace match
extern void runMatchTasks(const std::vector<std::string>& targets);
extern void updateMatchTask(const std::string& target);

//! @brief Apply notation.
namespace notation
{
//! @brief Set input parameters.
namespace input
{
//! @brief Infix string for notation methods.
constexpr std::string_view infixString = "a+b*(c^d-e)^(f+g*h)-i";
} // namespace input

//! @brief Solution of notation.
class NotationSolution
{
public:
    //! @brief Destroy the NotationSolution object.
    virtual ~NotationSolution() = default;

    //! @brief The prefix method.
    //! @param infixNotation - infix notation
    static void prefixMethod(const std::string& infixNotation);
    //! @brief The postfix method.
    //! @param infixNotation - infix notation
    static void postfixMethod(const std::string& infixNotation);
};

//! @brief Builder for the target.
class TargetBuilder
{
public:
    //! @brief Construct a new TargetBuilder object.
    //! @param infixNotation - infix notation
    explicit TargetBuilder(const std::string_view infixNotation) : infixNotation(infixNotation)
    {
#ifdef __RUNTIME_PRINTING
        std::cout << "\r\nInfix notation: " << infixNotation << std::endl;
#endif // __RUNTIME_PRINTING
    }
    //! @brief Destroy the TargetBuilder object.
    virtual ~TargetBuilder() = default;

    //! @brief Get the infix notation.
    //! @return infix notation
    [[nodiscard]] inline std::string_view getInfixNotation() const { return infixNotation; }

private:
    //! @brief Infix notation.
    const std::string_view infixNotation;
};
} // namespace notation
extern void runNotationTasks(const std::vector<std::string>& targets);
extern void updateNotationTask(const std::string& target);

//! @brief Apply optimal.
namespace optimal
{
//! @brief Set input parameters.
namespace input
{
//! @brief Rastrigin function.
class Rastrigin : public algorithm::optimal::Function
{
public:
    //! @brief The operator (()) overloading of Rastrigin class.
    //! @param x - independent variable
    //! @return dependent variable
    double operator()(const double x) const override
    {
        // f(x)=An+Σ(1→n)[(Xi)^2-Acos(2π*Xi)],A=10,x∈[-5.12,5.12],f(min)=0
        return (x * x - 10.0 * std::cos(2.0 * std::numbers::pi * x) + 10.0);
    }

    //! @brief Left endpoint.
    static constexpr double range1{-5.12};
    //! @brief Right endpoint.
    static constexpr double range2{5.12};
    //! @brief One-dimensional Rastrigin.
    static constexpr std::string_view funcDescr{
        "f(x)=An+Σ(1→n)[(Xi)^2-Acos(2π*Xi)],A=10,x∈[-5.12,5.12] (one-dimensional Rastrigin)"};
};

//! @brief Griewank function.
class Griewank : public algorithm::optimal::Function
{
public:
    //! @brief The operator (()) overloading of Griewank class.
    //! @param x - independent variable
    //! @return dependent variable
    double operator()(const double x) const override
    {
        // f(x)=1+1/4000*Σ(1→n)[(Xi)^2]-Π(1→n)[cos(Xi/(i)^(1/2))],x∈[-600,600],f(min)=0
        return (1.0 + 1.0 / 4000.0 * x * x - std::cos(x));
    }

    //! @brief Left endpoint.
    static constexpr double range1{-600.0};
    //! @brief Right endpoint.
    static constexpr double range2{600.0};
    //! @brief One-dimensional Griewank.
    static constexpr std::string_view funcDescr{
        "f(x)=1+1/4000*Σ(1→n)[(Xi)^2]-Π(1→n)[cos(Xi/(i)^(1/2))],x∈[-600,600] (one-dimensional Griewank)"};
};
} // namespace input

//! @brief Alias for the target function.
using Function = algorithm::optimal::Function;

//! @brief Solution of optimal.
class OptimalSolution
{
public:
    //! @brief Destroy the OptimalSolution object.
    virtual ~OptimalSolution() = default;

    //! @brief The gradient descent method.
    //! @param func - target function
    //! @param left - left endpoint
    //! @param right - right endpoint
    static void gradientDescentMethod(const Function& func, const double left, const double right);
    //! @brief The simulated annealing method.
    //! @param func - target function
    //! @param left - left endpoint
    //! @param right - right endpoint
    static void simulatedAnnealingMethod(const Function& func, const double left, const double right);
    //! @brief The particle swarm method.
    //! @param func - target function
    //! @param left - left endpoint
    //! @param right - right endpoint
    static void particleSwarmMethod(const Function& func, const double left, const double right);
    //! @brief The genetic method.
    //! @param func - target function
    //! @param left - left endpoint
    //! @param right - right endpoint
    static void geneticMethod(const Function& func, const double left, const double right);
};

//! @brief Function object's helper type for the visitor.
//! @tparam Ts - type of visitor
template <class... Ts>
struct FuncOverloaded : Ts...
{
    using Ts::operator()...;
};

//! @brief Explicit deduction guide for FuncOverloaded.
//! @tparam Ts - type of visitor
template <class... Ts>
FuncOverloaded(Ts...) -> FuncOverloaded<Ts...>;

//! @brief Range properties of the function.
//! @tparam T1 - type of left endpoint
//! @tparam T2 - type of right endpoint
template <typename T1, typename T2>
struct FuncRange
{
    //! @brief Construct a new FuncRange object.
    //! @param range1 - left endpoint
    //! @param range2 - light endpoint
    //! @param funcDescr - function description
    FuncRange(const T1& range1, const T2& range2, const std::string_view funcDescr) :
        range1(range1), range2(range2), funcDescr(funcDescr){};
    //! @brief Construct a new FuncRange object.
    FuncRange() = delete;
    //! @brief Left endpoint.
    T1 range1;
    //! @brief Right endpoint.
    T2 range2;
    //! @brief Function description.
    std::string_view funcDescr;

    //! @brief The operator (==) overloading of FuncRange class.
    //! @param rhs - right-hand side
    //! @return be equal or not equal
    bool operator==(const FuncRange& rhs) const
    {
        return (std::tie(rhs.range1, rhs.range2, rhs.funcDescr) == std::tie(range1, range2, funcDescr));
    }
};
//! @brief Mapping hash value for the function.
struct FuncMapHash
{
    //! @brief The operator (()) overloading of FuncMapHash class.
    //! @tparam T1 - type of left endpoint
    //! @tparam T2 - type of right endpoint
    //! @param range - range properties of the function
    //! @return hash value
    template <typename T1, typename T2>
    std::size_t operator()(const FuncRange<T1, T2>& range) const
    {
        std::size_t hash1 = std::hash<T1>()(range.range1);
        std::size_t hash2 = std::hash<T2>()(range.range2);
        std::size_t hash3 = std::hash<std::string_view>()(range.funcDescr);
        return (hash1 ^ hash2 ^ hash3);
    }
};
} // namespace optimal
extern void runOptimalTasks(const std::vector<std::string>& targets);
extern void updateOptimalTask(const std::string& target);

//! @brief Apply search.
namespace search
{
//! @brief Set input parameters.
namespace input
{
//! @brief Minimum of the array for search methods.
constexpr double arrayRange1 = -50.0;
//! @brief Maximum of the array for search methods.
constexpr double arrayRange2 = 150.0;
//! @brief Length of the array for search methods.
constexpr std::uint32_t arrayLength = 53;
} // namespace input

//! @brief Solution of search.
class SearchSolution
{
public:
    //! @brief Destroy the SearchSolution object.
    virtual ~SearchSolution() = default;

    //! @brief The binary method.
    //! @param array - array to be searched
    //! @param length - length of array
    //! @param key - search key
    static void binaryMethod(const double* const array, const std::uint32_t length, const double key);
    //! @brief Interpolation.
    //! @param array - array to be searched
    //! @param length - length of array
    //! @param key - search key
    static void interpolationMethod(const double* const array, const std::uint32_t length, const double key);
    //! @brief Fibonacci.
    //! @param array - array to be searched
    //! @param length - length of array
    //! @param key - search key
    static void fibonacciMethod(const double* const array, const std::uint32_t length, const double key);
};

//! @brief Maximum alignment length per element of printing.
constexpr std::uint8_t maxAlignOfPrint = 16;
//! @brief Maximum columns per row of printing.
constexpr std::uint8_t maxColumnOfPrint = 10;

//! @brief Check whether it is the number type.
//! @tparam T - type of inspection to be performed
//! @return be number or not
template <typename T>
constexpr bool isNumber()
{
    return (std::is_integral<T>::value || std::is_floating_point<T>::value);
}

//! @brief Builder for the target.
//! @tparam T - type of builder for the target
template <class T>
class TargetBuilder
{
public:
    //! @brief Construct a new TargetBuilder object.
    //! @param length - length of array
    //! @param left - the left boundary of the array
    //! @param right - the right boundary of the array
    TargetBuilder(const std::uint32_t length, const T left, const T right) :
        length(length), orderedArray(std::make_unique<T[]>(length))
    {
        setOrderedArray<T>(orderedArray.get(), length, left, right);
        searchKey = orderedArray[length / 2];
    }
    //! @brief Destroy the TargetBuilder object.
    virtual ~TargetBuilder() = default;
    //! @brief Construct a new TargetBuilder object.
    //! @param rhs - right-hand side
    TargetBuilder(const TargetBuilder& rhs) : length(rhs.length), orderedArray(std::make_unique<T[]>(rhs.length))
    {
        deepCopy(rhs);
        searchKey = orderedArray[length / 2];
    }
    //! @brief The operator (!=) overloading of TargetBuilder class.
    //! @param rhs - right-hand side
    //! @return reference of the TargetBuilder object
    TargetBuilder<T>& operator=(const TargetBuilder& rhs)
    {
        deepCopy(rhs);
        return *this;
    }

    //! @brief Get the ordered array.
    //! @return ordered array
    inline const std::unique_ptr<T[]>& getOrderedArray() const { return orderedArray; }
    //! @brief Get the length.
    //! @return length
    [[nodiscard]] inline std::uint32_t getLength() const { return length; }
    //! @brief Get the search key.
    //! @return T search key
    inline T getSearchKey() const { return searchKey; }
    //! @brief Splice from array for printing.
    //! @tparam V - type of array
    //! @param array - target array
    //! @param length - length of array
    //! @param buffer - buffer for printing
    //! @param bufferSize - size of buffer
    //! @return buffer after splicing
    template <typename V>
    requires(isNumber<V>())
    static char* spliceAll(
        const T* const restrict array,
        const std::uint32_t length,
        char* const restrict buffer,
        const std::uint32_t bufferSize)
    {
        std::uint32_t align = 0;
        for (std::uint32_t i = 0; i < length; ++i)
        {
            align = std::max(static_cast<std::uint32_t>(std::to_string(*(array + i)).length()), align);
        }

        const char* format = " ";
        if constexpr (std::is_integral<T>::value)
        {
            format = "%*d ";
        }
        else if constexpr (std::is_floating_point<T>::value)
        {
            format = "%*.5f ";
        }

        int formatSize = 0;
        std::uint32_t completeSize = 0;
        for (std::uint32_t i = 0; i < length; ++i)
        {
            formatSize =
                std::snprintf(buffer + completeSize, bufferSize - completeSize, format, align + 1, *(array + i));
            if ((formatSize < 0) || (formatSize >= static_cast<int>(bufferSize - completeSize)))
            {
                break;
            }
            completeSize += formatSize;

            if ((0 == (i + 1) % maxColumnOfPrint) && ((i + 1) != length))
            {
                formatSize = std::snprintf(buffer + completeSize, bufferSize - completeSize, "\n");
                if ((formatSize < 0) || (formatSize >= static_cast<int>(bufferSize - completeSize)))
                {
                    break;
                }
                completeSize += formatSize;
            }
        }

        return buffer;
    }

private:
    //! @brief Ordered array.
    const std::unique_ptr<T[]> orderedArray;
    //! @brief Length of the ordered array.
    const std::uint32_t length;
    //! @brief Search key.
    T searchKey{0};

    //! @brief Deep copy for copy constructor.
    //! @param rhs - right-hand side
    void deepCopy(const TargetBuilder& rhs) const
    {
        std::memcpy(orderedArray.get(), rhs.orderedArray.get(), length * sizeof(T));
    }
    //! @brief Set the ordered array.
    //! @tparam V - the specific type of integral
    //! @param array - ordered array
    //! @param length - length of the ordered array
    //! @param left - the left boundary of the ordered array
    //! @param right - the left right of the ordered array
    template <typename V>
    requires std::is_integral<V>::value
    static void setOrderedArray(T array[], const std::uint32_t length, const T left, const T right)
    {
        std::mt19937 engine{std::random_device{}()};
        std::uniform_int_distribution<int> dist(left, right);
        for (std::uint32_t i = 0; i < length; ++i)
        {
            array[i] = dist(engine);
        }
        std::sort(array, array + length);

#ifdef __RUNTIME_PRINTING
        const std::uint32_t arrayBufferSize = length * maxAlignOfPrint;
        char arrayBuffer[arrayBufferSize + 1];
        arrayBuffer[0] = '\0';
        std::cout << "\r\nGenerate " << length << " ordered integral numbers from " << left << " to " << right << ":\n"
                  << spliceAll<T>(array, length, arrayBuffer, arrayBufferSize + 1) << std::endl;
#endif // __RUNTIME_PRINTING
    }
    //! @brief Set the ordered array.
    //! @tparam V - the specific type of floating point
    //! @param array - ordered array
    //! @param length - length of the ordered array
    //! @param left - the left boundary of the ordered array
    //! @param right - the left right of the ordered array
    template <typename V>
    requires std::is_floating_point<V>::value
    static void setOrderedArray(T array[], const std::uint32_t length, const T left, const T right)
    {
        std::mt19937 engine{std::random_device{}()};
        std::uniform_real_distribution<double> dist(left, right);
        for (std::uint32_t i = 0; i < length; ++i)
        {
            array[i] = dist(engine);
        }
        std::sort(array, array + length);

#ifdef __RUNTIME_PRINTING
        const std::uint32_t arrayBufferSize = length * maxAlignOfPrint;
        char arrayBuffer[arrayBufferSize + 1];
        arrayBuffer[0] = '\0';
        std::cout << "\r\nGenerate " << length << " ordered floating point numbers from " << left << " to " << right
                  << ":\n"
                  << spliceAll<T>(array, length, arrayBuffer, arrayBufferSize + 1) << std::endl;
#endif // __RUNTIME_PRINTING
    }
};
} // namespace search
extern void runSearchTasks(const std::vector<std::string>& targets);
extern void updateSearchTask(const std::string& target);

//! @brief Apply sort.
namespace sort
{
//! @brief Set input parameters.
namespace input
{
//! @brief Minimum of the array for sort methods.
constexpr int arrayRange1 = -50;
//! @brief Maximum of the array for sort methods.
constexpr int arrayRange2 = 150;
//! @brief Length of the array for sort methods.
constexpr std::uint32_t arrayLength = 53;
} // namespace input

//! @brief Solution of sort.
class SortSolution
{
public:
    //! @brief Destroy the SortSolution object.
    virtual ~SortSolution() = default;

    //! @brief The bubble method.
    //! @param array - array to be sorted
    //! @param length - length of array
    static void bubbleMethod(int* const array, const std::uint32_t length);
    //! @brief The selection method.
    //! @param array - array to be sorted
    //! @param length - length of array
    static void selectionMethod(int* const array, const std::uint32_t length);
    //! @brief The insertion method.
    //! @param array - array to be sorted
    //! @param length - length of array
    static void insertionMethod(int* const array, const std::uint32_t length);
    //! @brief The shell method.
    //! @param array - array to be sorted
    //! @param length - length of array
    static void shellMethod(int* const array, const std::uint32_t length);
    //! @brief The merge method.
    //! @param array - array to be sorted
    //! @param length - length of array
    static void mergeMethod(int* const array, const std::uint32_t length);
    //! @brief The quick method.
    //! @param array - array to be sorted
    //! @param length - length of array
    static void quickMethod(int* const array, const std::uint32_t length);
    //! @brief The heap method.
    //! @param array - array to be sorted
    //! @param length - length of array
    static void heapMethod(int* const array, const std::uint32_t length);
    //! @brief The counting method.
    //! @param array - array to be sorted
    //! @param length - length of array
    static void countingMethod(int* const array, const std::uint32_t length);
    //! @brief The bucket method.
    //! @param array - array to be sorted
    //! @param length - length of array
    static void bucketMethod(int* const array, const std::uint32_t length);
    //! @brief The radix method.
    //! @param array - array to be sorted
    //! @param length - length of array
    static void radixMethod(int* const array, const std::uint32_t length);
};

//! @brief Maximum alignment length per element of printing.
constexpr std::uint8_t maxAlignOfPrint = 16;
//! @brief Maximum columns per row of printing.
constexpr std::uint8_t maxColumnOfPrint = 10;

//! @brief Check whether it is the number type.
//! @tparam T - type of inspection to be performed
//! @return be number or not
template <typename T>
constexpr bool isNumber()
{
    return (std::is_integral<T>::value || std::is_floating_point<T>::value);
}

//! @brief Builder for the target.
//! @tparam T - type of builder for the target
template <class T>
class TargetBuilder
{
public:
    //! @brief Construct a new TargetBuilder object.
    //! @param length - length of array
    //! @param left - the left boundary of the array
    //! @param right - the right boundary of the array
    TargetBuilder(const std::uint32_t length, const T left, const T right) :
        length(length), randomArray(std::make_unique<T[]>(length))
    {
        setRandomArray<T>(randomArray.get(), length, left, right);
    }
    //! @brief Destroy the TargetBuilder object.
    virtual ~TargetBuilder() = default;
    //! @brief Construct a new TargetBuilder object.
    //! @param rhs - right-hand side
    TargetBuilder(const TargetBuilder& rhs) : length(rhs.length), randomArray(std::make_unique<T[]>(rhs.length))
    {
        deepCopy(rhs);
    }
    //! @brief The operator (!=) overloading of TargetBuilder class.
    //! @param rhs - right-hand side
    //! @return reference of the TargetBuilder object
    TargetBuilder<T>& operator=(const TargetBuilder& rhs)
    {
        deepCopy(rhs);
        return *this;
    }

    //! @brief Get the random array.
    //! @return random array
    inline const std::unique_ptr<T[]>& getRandomArray() const { return randomArray; }
    //! @brief Get the length.
    //! @return length
    [[nodiscard]] inline std::uint32_t getLength() const { return length; }
    //! @brief Splice from array for printing.
    //! @tparam V - type of array
    //! @param array - target array
    //! @param length - length of array
    //! @param buffer - buffer for printing
    //! @param bufferSize - size of buffer
    //! @return buffer after splicing
    template <typename V>
    requires(isNumber<V>())
    static char* spliceAll(
        const T* const restrict array,
        const std::uint32_t length,
        char* const restrict buffer,
        const std::uint32_t bufferSize)
    {
        std::uint32_t align = 0;
        for (std::uint32_t i = 0; i < length; ++i)
        {
            align = std::max(static_cast<std::uint32_t>(std::to_string(*(array + i)).length()), align);
        }

        const char* format = " ";
        if constexpr (std::is_integral<T>::value)
        {
            format = "%*d ";
        }
        else if constexpr (std::is_floating_point<T>::value)
        {
            format = "%*.5f ";
        }

        int formatSize = 0;
        std::uint32_t completeSize = 0;
        for (std::uint32_t i = 0; i < length; ++i)
        {
            formatSize =
                std::snprintf(buffer + completeSize, bufferSize - completeSize, format, align + 1, *(array + i));
            if ((formatSize < 0) || (formatSize >= static_cast<int>(bufferSize - completeSize)))
            {
                break;
            }
            completeSize += formatSize;

            if ((0 == (i + 1) % maxColumnOfPrint) && ((i + 1) != length))
            {
                formatSize = std::snprintf(buffer + completeSize, bufferSize - completeSize, "\n");
                if ((formatSize < 0) || (formatSize >= static_cast<int>(bufferSize - completeSize)))
                {
                    break;
                }
                completeSize += formatSize;
            }
        }

        return buffer;
    }

private:
    //! @brief Random array.
    const std::unique_ptr<T[]> randomArray;
    //! @brief Length of the random array.
    const std::uint32_t length;

    //! @brief Deep copy for copy constructor.
    //! @param rhs - right-hand side
    void deepCopy(const TargetBuilder& rhs) const
    {
        std::memcpy(randomArray.get(), rhs.randomArray.get(), length * sizeof(T));
    }
    //! @brief Set the random array.
    //! @tparam V - the specific type of integral
    //! @param array - random array
    //! @param length - length of the random array
    //! @param left - the left boundary of the random array
    //! @param right - the left right of the random array
    template <typename V>
    requires std::is_integral<V>::value
    static void setRandomArray(T array[], const std::uint32_t length, const T left, const T right)
    {
        std::mt19937 engine{std::random_device{}()};
        std::uniform_int_distribution<int> dist(left, right);
        for (std::uint32_t i = 0; i < length; ++i)
        {
            array[i] = dist(engine);
        }

#ifdef __RUNTIME_PRINTING
        const std::uint32_t arrayBufferSize = length * maxAlignOfPrint;
        char arrayBuffer[arrayBufferSize + 1];
        arrayBuffer[0] = '\0';
        std::cout << "\r\nGenerate " << length << " random integral numbers from " << left << " to " << right << ":\n"
                  << spliceAll<T>(array, length, arrayBuffer, arrayBufferSize + 1) << std::endl;
#endif // __RUNTIME_PRINTING
    }
    //! @brief Set the random array.
    //! @tparam V - the specific type of floating point
    //! @param array - random array
    //! @param length - length of the random array
    //! @param left - the left boundary of the random array
    //! @param right - the left right of the random array
    template <typename V>
    requires std::is_floating_point<V>::value
    static void setRandomArray(T array[], const std::uint32_t length, const T left, const T right)
    {
        std::mt19937 engine{std::random_device{}()};
        std::uniform_real_distribution<double> dist(left, right);
        for (std::uint32_t i = 0; i < length; ++i)
        {
            array[i] = dist(engine);
        }

#ifdef __RUNTIME_PRINTING
        const std::uint32_t arrayBufferSize = length * maxAlignOfPrint;
        char arrayBuffer[arrayBufferSize + 1];
        arrayBuffer[0] = '\0';
        std::cout << "\r\nGenerate " << length << " random floating point numbers from " << left << " to " << right
                  << ":\n"
                  << spliceAll<T>(array, length, arrayBuffer, arrayBufferSize + 1) << std::endl;
#endif // __RUNTIME_PRINTING
    }
};
} // namespace sort
extern void runSortTasks(const std::vector<std::string>& targets);
extern void updateSortTask(const std::string& target);
} // namespace application::app_algo

#undef restrict
