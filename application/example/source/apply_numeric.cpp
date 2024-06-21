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

#include "application/core/include/command.hpp"
#include "application/core/include/log.hpp"
#include "utility/include/currying.hpp"

//! @brief Title of printing when numeric tasks are beginning.
#define APP_NUM_PRINT_TASK_BEGIN_TITLE(category)                                                                   \
    std::osyncstream(std::cout) << "\r\n"                                                                          \
                                << "NUMERIC TASK: " << std::setiosflags(std::ios_base::left) << std::setfill('.')  \
                                << std::setw(50) << category << "BEGIN" << std::resetiosflags(std::ios_base::left) \
                                << std::setfill(' ') << std::endl;                                                 \
    {
//! @brief Title of printing when numeric tasks are ending.
#define APP_NUM_PRINT_TASK_END_TITLE(category)                                                                    \
    }                                                                                                             \
    std::osyncstream(std::cout) << "\r\n"                                                                         \
                                << "NUMERIC TASK: " << std::setiosflags(std::ios_base::left) << std::setfill('.') \
                                << std::setw(50) << category << "END" << std::resetiosflags(std::ios_base::left)  \
                                << std::setfill(' ') << '\n'                                                      \
                                << std::endl;

namespace application::app_num
{
//! @brief Alias for Category.
using Category = NumericTask::Category;

//! @brief Get the numeric task.
//! @return reference of the NumericTask object
NumericTask& getTask()
{
    static NumericTask task{};
    return task;
}

//! @brief Get the task name curried.
//! @return task name curried
static const auto& getTaskNameCurried()
{
    static const auto curried =
        utility::currying::curry(command::presetTaskName, utility::reflection::TypeInfo<NumericTask>::name);
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

//! @brief Get the bit flags of the category in numeric tasks.
//! @tparam Cat - the specific value of Category enum
//! @return reference of the category bit flags
template <Category Cat>
constexpr auto& getCategoryBit()
{
    return std::invoke(
        utility::reflection::TypeInfo<NumericTask>::fields.find(REFLECTION_STR(toString(Cat))).value, getTask());
}

//! @brief Get the alias of the category in numeric tasks.
//! @tparam Cat - the specific value of Category enum
//! @return alias of the category name
template <Category Cat>
constexpr std::string_view getCategoryAlias()
{
    constexpr auto attr = utility::reflection::TypeInfo<NumericTask>::fields.find(REFLECTION_STR(toString(Cat)))
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
                auto attr = field.attrs.find(REFLECTION_STR("task"));
                static_assert(attr.hasValue);
                value = utility::common::operator""_bkdrHash(attr.value, 0);
            }
        });
    return value;
}

//! @brief Get the title of a particular method in numeric tasks.
//! @tparam T - type of target method
//! @param method - target method
//! @return initial capitalized title
template <class T>
std::string getTitle(const T method)
{
    std::string title = std::string{toString(method)};
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
    return table[method];
//! @endcond
#undef ELEM
}
#undef APP_NUM_PRIME_METHOD_TABLE

namespace arithmetic
{
//! @brief Display arithmetic result.
#define ARITHMETIC_RESULT "\r\n==> %-14s Method <==\n(%d) %c (%d) = %d\n"
//! @brief Print arithmetic result content.
#define ARITHMETIC_PRINT_RESULT_CONTENT(method, a, operator, b, result) \
    COMMON_PRINT(ARITHMETIC_RESULT, getTitle(method).data(), a, operator, b, result)

void ArithmeticSolution::additionMethod(const std::int32_t augend, const std::int32_t addend)
try
{
    const auto sum = numeric::arithmetic::Arithmetic().addition(augend, addend);
    ARITHMETIC_PRINT_RESULT_CONTENT(ArithmeticMethod::addition, augend, '+', addend, sum);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void ArithmeticSolution::subtractionMethod(const std::int32_t minuend, const std::int32_t subtrahend)
try
{
    const auto difference = numeric::arithmetic::Arithmetic().subtraction(minuend, subtrahend);
    ARITHMETIC_PRINT_RESULT_CONTENT(ArithmeticMethod::subtraction, minuend, '-', subtrahend, difference);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void ArithmeticSolution::multiplicationMethod(const std::int32_t multiplier, const std::int32_t multiplicand)
try
{
    const auto product = numeric::arithmetic::Arithmetic().multiplication(multiplier, multiplicand);
    ARITHMETIC_PRINT_RESULT_CONTENT(ArithmeticMethod::multiplication, multiplier, '*', multiplicand, product);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void ArithmeticSolution::divisionMethod(const std::int32_t dividend, const std::int32_t divisor)
try
{
    const auto quotient = numeric::arithmetic::Arithmetic().division(dividend, divisor);
    ARITHMETIC_PRINT_RESULT_CONTENT(ArithmeticMethod::division, dividend, '/', divisor, quotient);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

#undef ARITHMETIC_RESULT
#undef ARITHMETIC_PRINT_RESULT_CONTENT
} // namespace arithmetic

//! @brief Run arithmetic tasks.
//! @param candidates - container for the candidate target methods
void runArithmeticTasks(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::arithmetic;
    const auto& bitFlag = getCategoryBit<category>();
    if (bitFlag.none())
    {
        return;
    }

    APP_NUM_PRINT_TASK_BEGIN_TITLE(category);
    using arithmetic::ArithmeticSolution, arithmetic::InputBuilder, arithmetic::input::integerA,
        arithmetic::input::integerB;

    auto& pooling = command::getPublicThreadPool();
    auto* const threads = pooling.newElement(std::min(
        static_cast<std::uint32_t>(bitFlag.count()), static_cast<std::uint32_t>(Bottom<ArithmeticMethod>::value)));
    const auto inputs = std::make_shared<InputBuilder>(integerA, integerB);
    const auto arithmeticFunctor =
        [threads, inputs](const std::string& threadName, void (*methodPtr)(const std::int32_t, const std::int32_t))
    {
        threads->enqueue(threadName, methodPtr, std::get<0>(inputs->getIntegers()), std::get<1>(inputs->getIntegers()));
    };
    const auto name = utility::currying::curry(getTaskNameCurried(), getCategoryAlias<category>());

    for (std::uint8_t i = 0; i < Bottom<ArithmeticMethod>::value; ++i)
    {
        if (!bitFlag.test(ArithmeticMethod(i)))
        {
            continue;
        }

        const std::string targetMethod = candidates.at(i);
        switch (utility::common::bkdrHash(targetMethod.data()))
        {
            case abbrVal(ArithmeticMethod::addition):
                arithmeticFunctor(name(targetMethod), &ArithmeticSolution::additionMethod);
                break;
            case abbrVal(ArithmeticMethod::subtraction):
                arithmeticFunctor(name(targetMethod), &ArithmeticSolution::subtractionMethod);
                break;
            case abbrVal(ArithmeticMethod::multiplication):
                arithmeticFunctor(name(targetMethod), &ArithmeticSolution::multiplicationMethod);
                break;
            case abbrVal(ArithmeticMethod::division):
                arithmeticFunctor(name(targetMethod), &ArithmeticSolution::divisionMethod);
                break;
            default:
                LOG_INF << "Execute to apply an unknown " << toString(category) << " method.";
                break;
        }
    }

    pooling.deleteElement(threads);
    APP_NUM_PRINT_TASK_END_TITLE(category);
}

//! @brief Update arithmetic methods in tasks.
//! @param target - target method
void updateArithmeticTask(const std::string& target)
{
    constexpr auto category = Category::arithmetic;
    auto& bitFlag = getCategoryBit<category>();

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
            throw std::runtime_error("Unexpected " + std::string{toString(category)} + " method: " + target + '.');
    }
}

namespace divisor
{
//! @brief Display divisor result.
#define DIVISOR_RESULT "\r\n==> %-9s Method <==\n%s\nrun time: %8.5f ms\n"
//! @brief Print divisor result content.
#define DIVISOR_PRINT_RESULT_CONTENT(method)                                                                          \
    do                                                                                                                \
    {                                                                                                                 \
        const std::uint32_t arrayBufferSize = resCntr.size() * maxAlignOfPrint;                                       \
        std::vector<char> arrayBuffer(arrayBufferSize + 1);                                                           \
        COMMON_PRINT(                                                                                                 \
            DIVISOR_RESULT,                                                                                           \
            getTitle(method).data(),                                                                                  \
            InputBuilder::template spliceAllIntegers<std::int32_t>(resCntr, arrayBuffer.data(), arrayBufferSize + 1), \
            TIME_INTERVAL(timing));                                                                                   \
    }                                                                                                                 \
    while (0)

void DivisorSolution::euclideanMethod(std::int32_t a, std::int32_t b)
try
{
    TIME_BEGIN(timing);
    const auto resCntr = numeric::divisor::Divisor().euclidean(a, b);
    TIME_END(timing);
    DIVISOR_PRINT_RESULT_CONTENT(DivisorMethod::euclidean);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void DivisorSolution::steinMethod(std::int32_t a, std::int32_t b)
try
{
    TIME_BEGIN(timing);
    const auto resCntr = numeric::divisor::Divisor().stein(a, b);
    TIME_END(timing);
    DIVISOR_PRINT_RESULT_CONTENT(DivisorMethod::stein);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

#undef DIVISOR_RESULT
#undef DIVISOR_PRINT_RESULT_CONTENT
} // namespace divisor

//! @brief Run divisor tasks.
//! @param candidates - container for the candidate target methods
void runDivisorTasks(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::divisor;
    const auto& bitFlag = getCategoryBit<category>();
    if (bitFlag.none())
    {
        return;
    }

    APP_NUM_PRINT_TASK_BEGIN_TITLE(category);
    using divisor::DivisorSolution, divisor::InputBuilder, divisor::input::integerA, divisor::input::integerB;

    auto& pooling = command::getPublicThreadPool();
    auto* const threads = pooling.newElement(std::min(
        static_cast<std::uint32_t>(bitFlag.count()), static_cast<std::uint32_t>(Bottom<DivisorMethod>::value)));
    const auto inputs = std::make_shared<InputBuilder>(integerA, integerB);
    const auto divisorFunctor =
        [threads, inputs](const std::string& threadName, void (*methodPtr)(std::int32_t, std::int32_t))
    {
        threads->enqueue(threadName, methodPtr, std::get<0>(inputs->getIntegers()), std::get<1>(inputs->getIntegers()));
    };
    const auto name = utility::currying::curry(getTaskNameCurried(), getCategoryAlias<category>());

    for (std::uint8_t i = 0; i < Bottom<DivisorMethod>::value; ++i)
    {
        if (!bitFlag.test(DivisorMethod(i)))
        {
            continue;
        }

        const std::string targetMethod = candidates.at(i);
        switch (utility::common::bkdrHash(targetMethod.data()))
        {
            case abbrVal(DivisorMethod::euclidean):
                divisorFunctor(name(targetMethod), &DivisorSolution::euclideanMethod);
                break;
            case abbrVal(DivisorMethod::stein):
                divisorFunctor(name(targetMethod), &DivisorSolution::steinMethod);
                break;
            default:
                LOG_INF << "Execute to apply an unknown " << toString(category) << " method.";
                break;
        }
    }

    pooling.deleteElement(threads);
    APP_NUM_PRINT_TASK_END_TITLE(category);
}

//! @brief Update divisor methods in tasks.
//! @param target - target method
void updateDivisorTask(const std::string& target)
{
    constexpr auto category = Category::divisor;
    auto& bitFlag = getCategoryBit<category>();

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
            throw std::runtime_error("Unexpected " + std::string{toString(category)} + " method: " + target + '.');
    }
}

namespace integral
{
//! @brief Display integral result.
#define INTEGRAL_RESULT(opt) "\r\n==> %-11s Method <==\nI(" #opt ")=%+.5f, run time: %8.5f ms\n"
//! @brief Print integral result content.
#define INTEGRAL_PRINT_RESULT_CONTENT(method, sum) \
    COMMON_PRINT(INTEGRAL_RESULT(def), getTitle(method).data(), sum, TIME_INTERVAL(timing))

void IntegralSolution::trapezoidalMethod(const Expression& expr, double lower, double upper)
try
{
    TIME_BEGIN(timing);
    const auto sum = numeric::integral::Trapezoidal(expr)(lower, upper, numeric::integral::epsilon);
    TIME_END(timing);
    INTEGRAL_PRINT_RESULT_CONTENT(IntegralMethod::trapezoidal, sum);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void IntegralSolution::adaptiveSimpsonMethod(const Expression& expr, const double lower, const double upper)
try
{
    TIME_BEGIN(timing);
    const auto sum = numeric::integral::Trapezoidal(expr)(lower, upper, numeric::integral::epsilon);
    TIME_END(timing);
    INTEGRAL_PRINT_RESULT_CONTENT(IntegralMethod::simpson, sum);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void IntegralSolution::rombergMethod(const Expression& expr, const double lower, const double upper)
try
{
    TIME_BEGIN(timing);
    const auto sum = numeric::integral::Romberg(expr)(lower, upper, numeric::integral::epsilon);
    TIME_END(timing);
    INTEGRAL_PRINT_RESULT_CONTENT(IntegralMethod::romberg, sum);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void IntegralSolution::gaussLegendreMethod(const Expression& expr, const double lower, const double upper)
try
{
    TIME_BEGIN(timing);
    const auto sum = numeric::integral::Gauss(expr)(lower, upper, numeric::integral::epsilon);
    TIME_END(timing);
    INTEGRAL_PRINT_RESULT_CONTENT(IntegralMethod::gauss, sum);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void IntegralSolution::monteCarloMethod(const Expression& expr, const double lower, const double upper)
try
{
    TIME_BEGIN(timing);
    const auto sum = numeric::integral::MonteCarlo(expr)(lower, upper, numeric::integral::epsilon);
    TIME_END(timing);
    INTEGRAL_PRINT_RESULT_CONTENT(IntegralMethod::monteCarlo, sum);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

#undef INTEGRAL_RESULT
#undef INTEGRAL_PRINT_RESULT_CONTENT
} // namespace integral

//! @brief Run integral tasks.
//! @param candidates - container for the candidate target methods
void runIntegralTasks(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::integral;
    const auto& bitFlag = getCategoryBit<category>();
    if (bitFlag.none())
    {
        return;
    }

    using integral::InputBuilder, integral::input::Expression1;
    const auto calcExpr =
        [&candidates, bitFlag](const integral::Expression& expression, const integral::ExprRange<double, double>& range)
    {
        auto& pooling = command::getPublicThreadPool();
        auto* const threads = pooling.newElement(std::min(
            static_cast<std::uint32_t>(bitFlag.count()), static_cast<std::uint32_t>(Bottom<IntegralMethod>::value)));
        const auto integralFunctor = [threads, &expression, &range](
                                         const std::string& threadName,
                                         void (*methodPtr)(const integral::Expression&, const double, const double))
        {
            threads->enqueue(threadName, methodPtr, std::ref(expression), range.range1, range.range2);
        };
        const auto name = utility::currying::curry(getTaskNameCurried(), getCategoryAlias<category>());

        using integral::IntegralSolution;
        for (std::uint8_t i = 0; i < Bottom<IntegralMethod>::value; ++i)
        {
            if (!bitFlag.test(IntegralMethod(i)))
            {
                continue;
            }

            const std::string targetMethod = candidates.at(i);
            switch (utility::common::bkdrHash(targetMethod.data()))
            {
                case abbrVal(IntegralMethod::trapezoidal):
                    integralFunctor(name(targetMethod), &IntegralSolution::trapezoidalMethod);
                    break;
                case abbrVal(IntegralMethod::simpson):
                    integralFunctor(name(targetMethod), &IntegralSolution::adaptiveSimpsonMethod);
                    break;
                case abbrVal(IntegralMethod::romberg):
                    integralFunctor(name(targetMethod), &IntegralSolution::rombergMethod);
                    break;
                case abbrVal(IntegralMethod::gauss):
                    integralFunctor(name(targetMethod), &IntegralSolution::gaussLegendreMethod);
                    break;
                case abbrVal(IntegralMethod::monteCarlo):
                    integralFunctor(name(targetMethod), &IntegralSolution::monteCarloMethod);
                    break;
                default:
                    LOG_INF << "Execute to apply an unknown " << toString(category) << " method.";
                    break;
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

//! @brief Update integral methods in tasks.
//! @param target - target method
void updateIntegralTask(const std::string& target)
{
    constexpr auto category = Category::integral;
    auto& bitFlag = getCategoryBit<category>();

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
            throw std::runtime_error("Unexpected " + std::string{toString(category)} + " method: " + target + '.');
    }
}

namespace prime
{
//! @brief Display prime result.
#define PRIME_RESULT "\r\n==> %-9s Method <==\n%s\nrun time: %8.5f ms\n"
//! @brief Print prime result content.
#define PRIME_PRINT_RESULT_CONTENT(method)                                                                             \
    do                                                                                                                 \
    {                                                                                                                  \
        const std::uint32_t arrayBufferSize = resCntr.size() * maxAlignOfPrint;                                        \
        std::vector<char> arrayBuffer(arrayBufferSize + 1);                                                            \
        COMMON_PRINT(                                                                                                  \
            PRIME_RESULT,                                                                                              \
            getTitle(method).data(),                                                                                   \
            InputBuilder::template spliceAllIntegers<std::uint32_t>(resCntr, arrayBuffer.data(), arrayBufferSize + 1), \
            TIME_INTERVAL(timing));                                                                                    \
    }                                                                                                                  \
    while (0)

void PrimeSolution::eratosthenesMethod(const std::uint32_t max)
try
{
    TIME_BEGIN(timing);
    const auto resCntr = numeric::prime::Prime().eratosthenes(max);
    TIME_END(timing);
    PRIME_PRINT_RESULT_CONTENT(PrimeMethod::eratosthenes);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void PrimeSolution::eulerMethod(const std::uint32_t max)
try
{
    TIME_BEGIN(timing);
    const auto resCntr = numeric::prime::Prime().euler(max);
    TIME_END(timing);
    PRIME_PRINT_RESULT_CONTENT(PrimeMethod::euler);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

#undef PRIME_RESULT
#undef PRIME_PRINT_RESULT_CONTENT
} // namespace prime

//! @brief Run prime tasks.
//! @param candidates - container for the candidate target methods
void runPrimeTasks(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::prime;
    const auto& bitFlag = getCategoryBit<category>();
    if (bitFlag.none())
    {
        return;
    }

    APP_NUM_PRINT_TASK_BEGIN_TITLE(category);
    using prime::PrimeSolution, prime::InputBuilder, prime::input::maxPositiveInteger;

    auto& pooling = command::getPublicThreadPool();
    auto* const threads = pooling.newElement(
        std::min(static_cast<std::uint32_t>(bitFlag.count()), static_cast<std::uint32_t>(Bottom<PrimeMethod>::value)));
    const auto inputs = std::make_shared<InputBuilder>(maxPositiveInteger);
    const auto primeFunctor = [threads, inputs](const std::string& threadName, void (*methodPtr)(const std::uint32_t))
    {
        threads->enqueue(threadName, methodPtr, inputs->getMaxPositiveInteger());
    };
    const auto name = utility::currying::curry(getTaskNameCurried(), getCategoryAlias<category>());

    for (std::uint8_t i = 0; i < Bottom<PrimeMethod>::value; ++i)
    {
        if (!bitFlag.test(PrimeMethod(i)))
        {
            continue;
        }

        const std::string targetMethod = candidates.at(i);
        switch (utility::common::bkdrHash(targetMethod.data()))
        {
            case abbrVal(PrimeMethod::eratosthenes):
                primeFunctor(name(targetMethod), &PrimeSolution::eratosthenesMethod);
                break;
            case abbrVal(PrimeMethod::euler):
                primeFunctor(name(targetMethod), &PrimeSolution::eulerMethod);
                break;
            default:
                LOG_INF << "Execute to apply an unknown " << toString(category) << " method.";
                break;
        }
    }

    pooling.deleteElement(threads);
    APP_NUM_PRINT_TASK_END_TITLE(category);
}

//! @brief Update prime methods in tasks.
//! @param target - target method
void updatePrimeTask(const std::string& target)
{
    constexpr auto category = Category::prime;
    auto& bitFlag = getCategoryBit<category>();

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
            throw std::runtime_error("Unexpected " + std::string{toString(category)} + " method: " + target + '.');
    }
}
} // namespace application::app_num
