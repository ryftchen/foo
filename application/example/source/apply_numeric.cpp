//! @file apply_numeric.cpp
//! @author ryftchen
//! @brief The definitions (apply_numeric) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#include "apply_numeric.hpp"

#ifndef __PRECOMPILED_HEADER
#include <iomanip>
#include <syncstream>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

#include "application/core/include/action.hpp"
#include "application/core/include/log.hpp"
#include "utility/include/currying.hpp"

//! @brief Title of printing when numeric tasks are beginning.
#define APP_NUM_PRINT_TASK_BEGIN_TITLE(category)                                                                    \
    std::osyncstream(std::cout) << "\nNUMERIC TASK: " << std::setiosflags(std::ios_base::left) << std::setfill('.') \
                                << std::setw(50) << category << "BEGIN" << std::resetiosflags(std::ios_base::left)  \
                                << std::setfill(' ') << std::endl;                                                  \
    {
//! @brief Title of printing when numeric tasks are ending.
#define APP_NUM_PRINT_TASK_END_TITLE(category)                                                                      \
    }                                                                                                               \
    std::osyncstream(std::cout) << "\nNUMERIC TASK: " << std::setiosflags(std::ios_base::left) << std::setfill('.') \
                                << std::setw(50) << category << "END" << std::resetiosflags(std::ios_base::left)    \
                                << std::setfill(' ') << '\n'                                                        \
                                << std::endl;

namespace application::app_num
{
//! @brief Alias for Category.
using Category = ApplyNumeric::Category;

//! @brief Get the numeric choice manager.
//! @return reference of the ApplyNumeric object
ApplyNumeric& manager()
{
    static ApplyNumeric manager{};
    return manager;
}

//! @brief Get the task name curried.
//! @return task name curried
static const auto& getTaskNameCurried()
{
    static const auto curried =
        utility::currying::curry(action::presetTaskName, utility::reflection::TypeInfo<ApplyNumeric>::name);
    return curried;
}

//! @brief Convert category enumeration to string.
//! @param cat - the specific value of Category enum
//! @return category name
constexpr std::string_view toString(const Category cat)
{
    switch (cat)
    {
        case Category::arithmetic:
            return utility::reflection::TypeInfo<ArithmeticMethod>::name;
        case Category::divisor:
            return utility::reflection::TypeInfo<DivisorMethod>::name;
        case Category::integral:
            return utility::reflection::TypeInfo<IntegralMethod>::name;
        case Category::prime:
            return utility::reflection::TypeInfo<PrimeMethod>::name;
        default:
            return "";
    }
}

//! @brief Get the bit flags of the category in numeric choices.
//! @tparam Cat - the specific value of Category enum
//! @return reference of the category bit flags
template <Category Cat>
constexpr auto& getCategoryOpts()
{
    return std::invoke(
        utility::reflection::TypeInfo<ApplyNumeric>::fields.find(REFLECTION_STR(toString(Cat))).value, manager());
}

//! @brief Get the alias of the category in numeric choices.
//! @tparam Cat - the specific value of Category enum
//! @return alias of the category name
template <Category Cat>
constexpr std::string_view getCategoryAlias()
{
    constexpr auto attr = utility::reflection::TypeInfo<ApplyNumeric>::fields.find(REFLECTION_STR(toString(Cat)))
                              .attrs.find(REFLECTION_STR("alias"));
    static_assert(attr.hasValue);
    return attr.value;
}

//! @brief Abbreviation value for the target method.
//! @tparam T - type of target method
//! @param method - target method
//! @return abbreviation value
template <class T>
consteval std::size_t abbrVal(const T method)
{
    using TypeInfo = utility::reflection::TypeInfo<T>;
    static_assert(Bottom<T>::value == TypeInfo::fields.size);

    std::size_t value = 0;
    TypeInfo::fields.forEach(
        [method, &value](auto field)
        {
            if (field.name == toString(method))
            {
                static_assert(1 == field.attrs.size);
                auto attr = field.attrs.find(REFLECTION_STR("choice"));
                static_assert(attr.hasValue);
                value = utility::common::operator""_bkdrHash(attr.value, 0);
            }
        });
    return value;
}

//! @brief Get the title of a particular method in numeric choices.
//! @tparam T - type of target method
//! @param method - target method
//! @return initial capitalized title
template <class T>
std::string getTitle(const T method)
{
    std::string title(toString(method));
    title.at(0) = std::toupper(title.at(0));
    return title;
}

//! @brief Mapping table for enum and string about arithmetic methods. X macro.
#define APP_NUM_ARITHMETIC_METHOD_TABLE    \
    ELEM(addition, "addition")             \
    ELEM(subtraction, "subtraction")       \
    ELEM(multiplication, "multiplication") \
    ELEM(division, "division")
//! @brief Convert method enumeration to string.
//! @param method - the specific value of ArithmeticMethod enum
//! @return method name
constexpr std::string_view toString(const ArithmeticMethod method)
{
//! @cond
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {APP_NUM_ARITHMETIC_METHOD_TABLE};
    static_assert((sizeof(table) / sizeof(table[0])) == Bottom<ArithmeticMethod>::value);
    return table[method];
//! @endcond
#undef ELEM
}
#undef APP_NUM_ARITHMETIC_METHOD_TABLE

//! @brief Mapping table for enum and string about divisor methods. X macro.
#define APP_NUM_DIVISOR_METHOD_TABLE \
    ELEM(euclidean, "euclidean")     \
    ELEM(stein, "stein")
//! @brief Convert method enumeration to string.
//! @param method - the specific value of DivisorMethod enum
//! @return method name
constexpr std::string_view toString(const DivisorMethod method)
{
//! @cond
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {APP_NUM_DIVISOR_METHOD_TABLE};
    static_assert((sizeof(table) / sizeof(table[0])) == Bottom<DivisorMethod>::value);
    return table[method];
//! @endcond
#undef ELEM
}
#undef APP_NUM_DIVISOR_METHOD_TABLE

//! @brief Mapping table for enum and string about integral methods. X macro.
#define APP_NUM_INTEGRAL_METHOD_TABLE \
    ELEM(trapezoidal, "trapezoidal")  \
    ELEM(simpson, "simpson")          \
    ELEM(romberg, "romberg")          \
    ELEM(gauss, "gauss")              \
    ELEM(monteCarlo, "monteCarlo")
//! @brief Convert method enumeration to string.
//! @param method - the specific value of IntegralMethod enum
//! @return method name
constexpr std::string_view toString(const IntegralMethod method)
{
//! @cond
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {APP_NUM_INTEGRAL_METHOD_TABLE};
    static_assert((sizeof(table) / sizeof(table[0])) == Bottom<IntegralMethod>::value);
    return table[method];
//! @endcond
#undef ELEM
}
#undef APP_NUM_INTEGRAL_METHOD_TABLE

//! @brief Mapping table for enum and string about prime methods. X macro.
#define APP_NUM_PRIME_METHOD_TABLE     \
    ELEM(eratosthenes, "eratosthenes") \
    ELEM(euler, "euler")
//! @brief Convert method enumeration to string.
//! @param method - the specific value of PrimeMethod enum
//! @return method name
constexpr std::string_view toString(const PrimeMethod method)
{
//! @cond
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {APP_NUM_PRIME_METHOD_TABLE};
    static_assert((sizeof(table) / sizeof(table[0])) == Bottom<PrimeMethod>::value);
    return table[method];
//! @endcond
#undef ELEM
}
#undef APP_NUM_PRIME_METHOD_TABLE

namespace arithmetic
{
//! @brief Display the contents of the arithmetic result.
//! @param method - the specific value of ArithmeticMethod enum
//! @param result - arithmetic result
//! @param a - first integer for elementary arithmetic
//! @param b - second integer for elementary arithmetic
//! @param op - operator of arithmetic
static void displayResult(
    const ArithmeticMethod method, const std::int32_t result, const std::int32_t a, const std::int32_t b, const char op)
{
    COMMON_PRINT("\n==> %-14s Method <==\n(%d) %c (%d) = %d\n", getTitle(method).data(), a, op, b, result);
}

void ArithmeticSolution::additionMethod(const std::int32_t augend, const std::int32_t addend)
try
{
    const auto calc = numeric::arithmetic::Arithmetic().addition(augend, addend);
    displayResult(ArithmeticMethod::addition, calc, augend, addend, '+');
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __func__ << ": " << err.what();
}

void ArithmeticSolution::subtractionMethod(const std::int32_t minuend, const std::int32_t subtrahend)
try
{
    const auto calc = numeric::arithmetic::Arithmetic().subtraction(minuend, subtrahend);
    displayResult(ArithmeticMethod::subtraction, calc, minuend, subtrahend, '-');
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __func__ << ": " << err.what();
}

void ArithmeticSolution::multiplicationMethod(const std::int32_t multiplier, const std::int32_t multiplicand)
try
{
    const auto calc = numeric::arithmetic::Arithmetic().multiplication(multiplier, multiplicand);
    displayResult(ArithmeticMethod::multiplication, calc, multiplier, multiplicand, '*');
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __func__ << ": " << err.what();
}

void ArithmeticSolution::divisionMethod(const std::int32_t dividend, const std::int32_t divisor)
try
{
    const auto calc = numeric::arithmetic::Arithmetic().division(dividend, divisor);
    displayResult(ArithmeticMethod::division, calc, dividend, divisor, '/');
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __func__ << ": " << err.what();
}
} // namespace arithmetic

//! @brief Update arithmetic-related choice.
//! @param target - target method
template <>
void updateChoice<ArithmeticMethod>(const std::string& target)
{
    constexpr auto category = Category::arithmetic;
    auto& bitFlag = getCategoryOpts<category>();

    switch (utility::common::bkdrHash(target.c_str()))
    {
        case abbrVal(ArithmeticMethod::addition):
            bitFlag.set(ArithmeticMethod::addition);
            break;
        case abbrVal(ArithmeticMethod::subtraction):
            bitFlag.set(ArithmeticMethod::subtraction);
            break;
        case abbrVal(ArithmeticMethod::multiplication):
            bitFlag.set(ArithmeticMethod::multiplication);
            break;
        case abbrVal(ArithmeticMethod::division):
            bitFlag.set(ArithmeticMethod::division);
            break;
        default:
            bitFlag.reset();
            throw std::logic_error("Unexpected " + std::string{toString(category)} + " method: " + target + '.');
    }
}

//! @brief Run arithmetic-related choices.
//! @param candidates - container for the candidate target methods
template <>
void runChoices<ArithmeticMethod>(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::arithmetic;
    const auto& bitFlag = getCategoryOpts<category>();
    if (bitFlag.none())
    {
        return;
    }

    APP_NUM_PRINT_TASK_BEGIN_TITLE(category);
    using arithmetic::ArithmeticSolution, arithmetic::InputBuilder, arithmetic::input::integerA,
        arithmetic::input::integerB;

    auto& pooling = action::resourcePool();
    auto* const threads = pooling.newElement(std::min(
        static_cast<std::uint32_t>(bitFlag.count()), static_cast<std::uint32_t>(Bottom<ArithmeticMethod>::value)));
    const auto inputs = std::make_shared<InputBuilder>(integerA, integerB);
    const auto arithmeticFunctor =
        [threads, inputs](const std::string& threadName, void (*targetMethod)(const std::int32_t, const std::int32_t))
    {
        threads->enqueue(
            threadName, targetMethod, std::get<0>(inputs->getIntegers()), std::get<1>(inputs->getIntegers()));
    };
    const auto name = utility::currying::curry(getTaskNameCurried(), getCategoryAlias<category>());

    for (std::uint8_t i = 0; i < Bottom<ArithmeticMethod>::value; ++i)
    {
        if (!bitFlag.test(ArithmeticMethod(i)))
        {
            continue;
        }

        const std::string target = candidates.at(i);
        switch (utility::common::bkdrHash(target.data()))
        {
            case abbrVal(ArithmeticMethod::addition):
                arithmeticFunctor(name(target), &ArithmeticSolution::additionMethod);
                break;
            case abbrVal(ArithmeticMethod::subtraction):
                arithmeticFunctor(name(target), &ArithmeticSolution::subtractionMethod);
                break;
            case abbrVal(ArithmeticMethod::multiplication):
                arithmeticFunctor(name(target), &ArithmeticSolution::multiplicationMethod);
                break;
            case abbrVal(ArithmeticMethod::division):
                arithmeticFunctor(name(target), &ArithmeticSolution::divisionMethod);
                break;
            default:
                throw std::logic_error("Unknown " + std::string{toString(category)} + " method: " + target + '.');
        }
    }

    pooling.deleteElement(threads);
    APP_NUM_PRINT_TASK_END_TITLE(category);
}

namespace divisor
{
//! @brief Display the contents of the divisor result.
//! @param method - the specific value of DivisorMethod enum
//! @param result - divisor result
//! @param interval - time interval
static void displayResult(const DivisorMethod method, const std::vector<std::int32_t>& result, const double interval)
{
    const std::uint32_t arrayBufferSize = result.size() * maxAlignOfPrint;
    std::vector<char> arrayBuffer(arrayBufferSize + 1);
    COMMON_PRINT(
        "\n==> %-9s Method <==\n%s\nrun time: %8.5f ms\n",
        getTitle(method).data(),
        InputBuilder::template spliceAllIntegers<std::int32_t>(result, arrayBuffer.data(), arrayBufferSize + 1),
        interval);
}

void DivisorSolution::euclideanMethod(std::int32_t a, std::int32_t b)
try
{
    TIME_BEGIN(timing);
    const auto& coll = numeric::divisor::Divisor().euclidean(a, b);
    TIME_END(timing);
    displayResult(DivisorMethod::euclidean, coll, TIME_INTERVAL(timing));
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __func__ << ": " << err.what();
}

void DivisorSolution::steinMethod(std::int32_t a, std::int32_t b)
try
{
    TIME_BEGIN(timing);
    const auto& coll = numeric::divisor::Divisor().stein(a, b);
    TIME_END(timing);
    displayResult(DivisorMethod::stein, coll, TIME_INTERVAL(timing));
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __func__ << ": " << err.what();
}
} // namespace divisor

//! @brief Update divisor-related choice.
//! @param target - target method
template <>
void updateChoice<DivisorMethod>(const std::string& target)
{
    constexpr auto category = Category::divisor;
    auto& bitFlag = getCategoryOpts<category>();

    switch (utility::common::bkdrHash(target.c_str()))
    {
        case abbrVal(DivisorMethod::euclidean):
            bitFlag.set(DivisorMethod::euclidean);
            break;
        case abbrVal(DivisorMethod::stein):
            bitFlag.set(DivisorMethod::stein);
            break;
        default:
            bitFlag.reset();
            throw std::logic_error("Unexpected " + std::string{toString(category)} + " method: " + target + '.');
    }
}

//! @brief Run divisor-related choices.
//! @param candidates - container for the candidate target methods
template <>
void runChoices<DivisorMethod>(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::divisor;
    const auto& bitFlag = getCategoryOpts<category>();
    if (bitFlag.none())
    {
        return;
    }

    APP_NUM_PRINT_TASK_BEGIN_TITLE(category);
    using divisor::DivisorSolution, divisor::InputBuilder, divisor::input::integerA, divisor::input::integerB;

    auto& pooling = action::resourcePool();
    auto* const threads = pooling.newElement(std::min(
        static_cast<std::uint32_t>(bitFlag.count()), static_cast<std::uint32_t>(Bottom<DivisorMethod>::value)));
    const auto inputs = std::make_shared<InputBuilder>(integerA, integerB);
    const auto divisorFunctor =
        [threads, inputs](const std::string& threadName, void (*targetMethod)(std::int32_t, std::int32_t))
    {
        threads->enqueue(
            threadName, targetMethod, std::get<0>(inputs->getIntegers()), std::get<1>(inputs->getIntegers()));
    };
    const auto name = utility::currying::curry(getTaskNameCurried(), getCategoryAlias<category>());

    for (std::uint8_t i = 0; i < Bottom<DivisorMethod>::value; ++i)
    {
        if (!bitFlag.test(DivisorMethod(i)))
        {
            continue;
        }

        const std::string target = candidates.at(i);
        switch (utility::common::bkdrHash(target.data()))
        {
            case abbrVal(DivisorMethod::euclidean):
                divisorFunctor(name(target), &DivisorSolution::euclideanMethod);
                break;
            case abbrVal(DivisorMethod::stein):
                divisorFunctor(name(target), &DivisorSolution::steinMethod);
                break;
            default:
                throw std::logic_error("Unknown " + std::string{toString(category)} + " method: " + target + '.');
        }
    }

    pooling.deleteElement(threads);
    APP_NUM_PRINT_TASK_END_TITLE(category);
}

namespace integral
{
//! @brief Display the contents of the integral result.
//! @param method - the specific value of IntegralMethod enum
//! @param result - integral result
//! @param interval - time interval
static void displayResult(const IntegralMethod method, const double result, const double interval)
{
    COMMON_PRINT(
        "\n==> %-11s Method <==\nI(def)=%+.5f, run time: %8.5f ms\n", getTitle(method).data(), result, interval);
}

void IntegralSolution::trapezoidalMethod(const Expression& expr, double lower, double upper)
try
{
    TIME_BEGIN(timing);
    const auto sum = numeric::integral::Trapezoidal(expr)(lower, upper, numeric::integral::epsilon);
    TIME_END(timing);
    displayResult(IntegralMethod::trapezoidal, sum, TIME_INTERVAL(timing));
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __func__ << ": " << err.what();
}

void IntegralSolution::adaptiveSimpsonMethod(const Expression& expr, const double lower, const double upper)
try
{
    TIME_BEGIN(timing);
    const auto sum = numeric::integral::Trapezoidal(expr)(lower, upper, numeric::integral::epsilon);
    TIME_END(timing);
    displayResult(IntegralMethod::simpson, sum, TIME_INTERVAL(timing));
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __func__ << ": " << err.what();
}

void IntegralSolution::rombergMethod(const Expression& expr, const double lower, const double upper)
try
{
    TIME_BEGIN(timing);
    const auto sum = numeric::integral::Romberg(expr)(lower, upper, numeric::integral::epsilon);
    TIME_END(timing);
    displayResult(IntegralMethod::romberg, sum, TIME_INTERVAL(timing));
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __func__ << ": " << err.what();
}

void IntegralSolution::gaussLegendreMethod(const Expression& expr, const double lower, const double upper)
try
{
    TIME_BEGIN(timing);
    const auto sum = numeric::integral::Gauss(expr)(lower, upper, numeric::integral::epsilon);
    TIME_END(timing);
    displayResult(IntegralMethod::gauss, sum, TIME_INTERVAL(timing));
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __func__ << ": " << err.what();
}

void IntegralSolution::monteCarloMethod(const Expression& expr, const double lower, const double upper)
try
{
    TIME_BEGIN(timing);
    const auto sum = numeric::integral::MonteCarlo(expr)(lower, upper, numeric::integral::epsilon);
    TIME_END(timing);
    displayResult(IntegralMethod::monteCarlo, sum, TIME_INTERVAL(timing));
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __func__ << ": " << err.what();
}
} // namespace integral

//! @brief Update integral-related choice.
//! @param target - target method
template <>
void updateChoice<IntegralMethod>(const std::string& target)
{
    constexpr auto category = Category::integral;
    auto& bitFlag = getCategoryOpts<category>();

    switch (utility::common::bkdrHash(target.c_str()))
    {
        case abbrVal(IntegralMethod::trapezoidal):
            bitFlag.set(IntegralMethod::trapezoidal);
            break;
        case abbrVal(IntegralMethod::simpson):
            bitFlag.set(IntegralMethod::simpson);
            break;
        case abbrVal(IntegralMethod::romberg):
            bitFlag.set(IntegralMethod::romberg);
            break;
        case abbrVal(IntegralMethod::gauss):
            bitFlag.set(IntegralMethod::gauss);
            break;
        case abbrVal(IntegralMethod::monteCarlo):
            bitFlag.set(IntegralMethod::monteCarlo);
            break;
        default:
            bitFlag.reset();
            throw std::logic_error("Unexpected " + std::string{toString(category)} + " method: " + target + '.');
    }
}

//! @brief Run integral-related choices.
//! @param candidates - container for the candidate target methods
template <>
void runChoices<IntegralMethod>(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::integral;
    const auto& bitFlag = getCategoryOpts<category>();
    if (bitFlag.none())
    {
        return;
    }

    using integral::InputBuilder, integral::input::Expression1;
    const auto calcExpr =
        [&candidates, bitFlag](const integral::Expression& expression, const integral::ExprRange<double, double>& range)
    {
        auto& pooling = action::resourcePool();
        auto* const threads = pooling.newElement(std::min(
            static_cast<std::uint32_t>(bitFlag.count()), static_cast<std::uint32_t>(Bottom<IntegralMethod>::value)));
        const auto integralFunctor = [threads, &expression, &range](
                                         const std::string& threadName,
                                         void (*targetMethod)(const integral::Expression&, const double, const double))
        {
            threads->enqueue(threadName, targetMethod, std::ref(expression), range.range1, range.range2);
        };
        const auto name = utility::currying::curry(getTaskNameCurried(), getCategoryAlias<category>());

        using integral::IntegralSolution;
        for (std::uint8_t i = 0; i < Bottom<IntegralMethod>::value; ++i)
        {
            if (!bitFlag.test(IntegralMethod(i)))
            {
                continue;
            }

            const std::string target = candidates.at(i);
            switch (utility::common::bkdrHash(target.data()))
            {
                case abbrVal(IntegralMethod::trapezoidal):
                    integralFunctor(name(target), &IntegralSolution::trapezoidalMethod);
                    break;
                case abbrVal(IntegralMethod::simpson):
                    integralFunctor(name(target), &IntegralSolution::adaptiveSimpsonMethod);
                    break;
                case abbrVal(IntegralMethod::romberg):
                    integralFunctor(name(target), &IntegralSolution::rombergMethod);
                    break;
                case abbrVal(IntegralMethod::gauss):
                    integralFunctor(name(target), &IntegralSolution::gaussLegendreMethod);
                    break;
                case abbrVal(IntegralMethod::monteCarlo):
                    integralFunctor(name(target), &IntegralSolution::monteCarloMethod);
                    break;
                default:
                    throw std::logic_error("Unknown " + std::string{toString(category)} + " method: " + target + '.');
            }
        }
        pooling.deleteElement(threads);
    };

    APP_NUM_PRINT_TASK_BEGIN_TITLE(category);

    const auto inputs = std::make_shared<InputBuilder<Expression1>>(integral::IntegralExprMap<Expression1>{
        {{Expression1::range1, Expression1::range2, Expression1::exprDescr}, Expression1{}}});
    for ([[maybe_unused]] const auto& [range, expression] : inputs->getExpressionMap())
    {
        inputs->printExpression(expression);
        switch (expression.index())
        {
            case 0:
                calcExpr(std::get<0>(expression), range);
                break;
            [[unlikely]] default:
                break;
        }
    }

    APP_NUM_PRINT_TASK_END_TITLE(category);
}

namespace prime
{
//! @brief Display the contents of the prime result.
//! @param method - the specific value of PrimeMethod enum
//! @param result - prime result
//! @param interval - time interval
static void displayResult(const PrimeMethod method, const std::vector<std::uint32_t>& result, const double interval)
{
    const std::uint32_t arrayBufferSize = result.size() * maxAlignOfPrint;
    std::vector<char> arrayBuffer(arrayBufferSize + 1);
    COMMON_PRINT(
        "\n==> %-9s Method <==\n%s\nrun time: %8.5f ms\n",
        getTitle(method).data(),
        InputBuilder::template spliceAllIntegers<std::uint32_t>(result, arrayBuffer.data(), arrayBufferSize + 1),
        interval);
}

void PrimeSolution::eratosthenesMethod(const std::uint32_t max)
try
{
    TIME_BEGIN(timing);
    const auto& coll = numeric::prime::Prime().eratosthenes(max);
    TIME_END(timing);
    displayResult(PrimeMethod::eratosthenes, coll, TIME_INTERVAL(timing));
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __func__ << ": " << err.what();
}

void PrimeSolution::eulerMethod(const std::uint32_t max)
try
{
    TIME_BEGIN(timing);
    const auto& coll = numeric::prime::Prime().euler(max);
    TIME_END(timing);
    displayResult(PrimeMethod::euler, coll, TIME_INTERVAL(timing));
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __func__ << ": " << err.what();
}
} // namespace prime

//! @brief Update prime-related choice.
//! @param target - target method
template <>
void updateChoice<PrimeMethod>(const std::string& target)
{
    constexpr auto category = Category::prime;
    auto& bitFlag = getCategoryOpts<category>();

    switch (utility::common::bkdrHash(target.c_str()))
    {
        case abbrVal(PrimeMethod::eratosthenes):
            bitFlag.set(PrimeMethod::eratosthenes);
            break;
        case abbrVal(PrimeMethod::euler):
            bitFlag.set(PrimeMethod::euler);
            break;
        default:
            bitFlag.reset();
            throw std::logic_error("Unexpected " + std::string{toString(category)} + " method: " + target + '.');
    }
}

//! @brief Run prime-related choices.
//! @param candidates - container for the candidate target methods
template <>
void runChoices<PrimeMethod>(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::prime;
    const auto& bitFlag = getCategoryOpts<category>();
    if (bitFlag.none())
    {
        return;
    }

    APP_NUM_PRINT_TASK_BEGIN_TITLE(category);
    using prime::PrimeSolution, prime::InputBuilder, prime::input::maxPositiveInteger;

    auto& pooling = action::resourcePool();
    auto* const threads = pooling.newElement(
        std::min(static_cast<std::uint32_t>(bitFlag.count()), static_cast<std::uint32_t>(Bottom<PrimeMethod>::value)));
    const auto inputs = std::make_shared<InputBuilder>(maxPositiveInteger);
    const auto primeFunctor =
        [threads, inputs](const std::string& threadName, void (*targetMethod)(const std::uint32_t))
    {
        threads->enqueue(threadName, targetMethod, inputs->getMaxPositiveInteger());
    };
    const auto name = utility::currying::curry(getTaskNameCurried(), getCategoryAlias<category>());

    for (std::uint8_t i = 0; i < Bottom<PrimeMethod>::value; ++i)
    {
        if (!bitFlag.test(PrimeMethod(i)))
        {
            continue;
        }

        const std::string target = candidates.at(i);
        switch (utility::common::bkdrHash(target.data()))
        {
            case abbrVal(PrimeMethod::eratosthenes):
                primeFunctor(name(target), &PrimeSolution::eratosthenesMethod);
                break;
            case abbrVal(PrimeMethod::euler):
                primeFunctor(name(target), &PrimeSolution::eulerMethod);
                break;
            default:
                throw std::logic_error("Unknown " + std::string{toString(category)} + " method: " + target + '.');
        }
    }

    pooling.deleteElement(threads);
    APP_NUM_PRINT_TASK_END_TITLE(category);
}
} // namespace application::app_num
