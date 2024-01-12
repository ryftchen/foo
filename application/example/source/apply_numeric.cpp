//! @file apply_numeric.cpp
//! @author ryftchen
//! @brief The definitions (apply_numeric) in the application module.
//! @version 1.0
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
#include "numeric/include/arithmetic.hpp"
#include "numeric/include/divisor.hpp"
#include "numeric/include/integral.hpp"
#include "numeric/include/prime.hpp"

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

namespace arithmetic
{
//! @brief Display arithmetic result.
#define ARITHMETIC_RESULT "\r\n==> %-14s Method <==\n(%d) %c (%d) = %d\n"
//! @brief Print arithmetic result content.
#define ARITHMETIC_PRINT_RESULT_CONTENT(method, a, operator, b, result) \
    COMMON_PRINT(ARITHMETIC_RESULT, toString(method).data(), a, operator, b, result)
//! @brief Mapping table for enum and string about arithmetic methods. X macro.
#define ARITHMETIC_METHOD_TABLE            \
    ELEM(addition, "Addition")             \
    ELEM(subtraction, "Subtraction")       \
    ELEM(multiplication, "Multiplication") \
    ELEM(division, "Division")

//! @brief Convert method enumeration to string.
//! @param method - the specific value of ArithmeticMethod enum
//! @return method name
constexpr std::string_view toString(const ArithmeticMethod method)
{
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {ARITHMETIC_METHOD_TABLE};
#undef ELEM
    return table[method];
}

void ArithmeticSolution::additionMethod(const int augend, const int addend)
try
{
    const auto sum = numeric::arithmetic::Arithmetic().addition(augend, addend);
    ARITHMETIC_PRINT_RESULT_CONTENT(ArithmeticMethod::addition, augend, '+', addend, sum);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void ArithmeticSolution::subtractionMethod(const int minuend, const int subtrahend)
try
{
    const auto difference = numeric::arithmetic::Arithmetic().subtraction(minuend, subtrahend);
    ARITHMETIC_PRINT_RESULT_CONTENT(ArithmeticMethod::subtraction, minuend, '-', subtrahend, difference);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void ArithmeticSolution::multiplicationMethod(const int multiplier, const int multiplicand)
try
{
    const auto product = numeric::arithmetic::Arithmetic().multiplication(multiplier, multiplicand);
    ARITHMETIC_PRINT_RESULT_CONTENT(ArithmeticMethod::multiplication, multiplier, '*', multiplicand, product);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void ArithmeticSolution::divisionMethod(const int dividend, const int divisor)
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
#undef ARITHMETIC_METHOD_TABLE
} // namespace arithmetic

//! @brief Run arithmetic tasks.
//! @param targets - container of target methods
void runArithmeticTasks(const std::vector<std::string>& targets)
{
    if (getBit<ArithmeticMethod>().none())
    {
        return;
    }

    using arithmetic::ArithmeticSolution;
    using arithmetic::TargetBuilder;
    using arithmetic::input::integerA;
    using arithmetic::input::integerB;
    using utility::common::operator""_bkdrHash;

    APP_NUM_PRINT_TASK_BEGIN_TITLE(Category::arithmetic);
    auto* const threads = command::getPublicThreadPool().newElement(std::min(
        static_cast<std::uint32_t>(getBit<ArithmeticMethod>().count()),
        static_cast<std::uint32_t>(Bottom<ArithmeticMethod>::value)));

    const auto builder = std::make_shared<TargetBuilder>(integerA, integerB);
    const auto arithmeticFunctor =
        [threads, builder](const std::string& threadName, void (*methodPtr)(const int, const int))
    {
        threads->enqueue(
            threadName, methodPtr, std::get<0>(builder->getIntegers()), std::get<1>(builder->getIntegers()));
    };

    for (std::uint8_t i = 0; i < Bottom<ArithmeticMethod>::value; ++i)
    {
        if (!getBit<ArithmeticMethod>().test(ArithmeticMethod(i)))
        {
            continue;
        }

        const std::string targetMethod = targets.at(i), threadName = "task-num_a_" + targetMethod;
        switch (utility::common::bkdrHash(targetMethod.data()))
        {
            case "add"_bkdrHash:
                arithmeticFunctor(threadName, &ArithmeticSolution::additionMethod);
                break;
            case "sub"_bkdrHash:
                arithmeticFunctor(threadName, &ArithmeticSolution::subtractionMethod);
                break;
            case "mul"_bkdrHash:
                arithmeticFunctor(threadName, &ArithmeticSolution::multiplicationMethod);
                break;
            case "div"_bkdrHash:
                arithmeticFunctor(threadName, &ArithmeticSolution::divisionMethod);
                break;
            default:
                LOG_INF << "Execute to apply an unknown arithmetic method.";
                break;
        }
    }

    command::getPublicThreadPool().deleteElement(threads);
    APP_NUM_PRINT_TASK_END_TITLE(Category::arithmetic);
}

//! @brief Update arithmetic methods in tasks.
//! @param target - target method
void updateArithmeticTask(const std::string& target)
{
    using utility::common::operator""_bkdrHash;
    switch (utility::common::bkdrHash(target.c_str()))
    {
        case "add"_bkdrHash:
            setBit<ArithmeticMethod>(ArithmeticMethod::addition);
            break;
        case "sub"_bkdrHash:
            setBit<ArithmeticMethod>(ArithmeticMethod::subtraction);
            break;
        case "mul"_bkdrHash:
            setBit<ArithmeticMethod>(ArithmeticMethod::multiplication);
            break;
        case "div"_bkdrHash:
            setBit<ArithmeticMethod>(ArithmeticMethod::division);
            break;
        default:
            getBit<ArithmeticMethod>().reset();
            throw std::runtime_error("Unexpected arithmetic method: " + target + '.');
    }
}

namespace divisor
{
//! @brief Display divisor result.
#define DIVISOR_RESULT "\r\n==> %-9s Method <==\n%s\nrun time: %8.5f ms\n"
//! @brief Print divisor result content.
#define DIVISOR_PRINT_RESULT_CONTENT(method)                                                                    \
    do                                                                                                          \
    {                                                                                                           \
        const std::uint32_t arrayBufferSize = divisorContainer.size() * maxAlignOfPrint;                        \
        char arrayBuffer[arrayBufferSize + 1];                                                                  \
        arrayBuffer[0] = '\0';                                                                                  \
        COMMON_PRINT(                                                                                           \
            DIVISOR_RESULT,                                                                                     \
            toString(method).data(),                                                                            \
            TargetBuilder::template spliceAllIntegers<int>(divisorContainer, arrayBuffer, arrayBufferSize + 1), \
            TIME_INTERVAL(timing));                                                                             \
    }                                                                                                           \
    while (0)
//! @brief Mapping table for enum and string about divisor methods. X macro.
#define DIVISOR_METHOD_TABLE     \
    ELEM(euclidean, "Euclidean") \
    ELEM(stein, "Stein")

//! @brief Convert method enumeration to string.
//! @param method - the specific value of DivisorMethod enum
//! @return method name
constexpr std::string_view toString(const DivisorMethod method)
{
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {DIVISOR_METHOD_TABLE};
#undef ELEM
    return table[method];
}

void DivisorSolution::euclideanMethod(int a, int b)
try
{
    TIME_BEGIN(timing);
    const auto divisorContainer = numeric::divisor::Divisor().euclidean(a, b);
    TIME_END(timing);
    DIVISOR_PRINT_RESULT_CONTENT(DivisorMethod::euclidean);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void DivisorSolution::steinMethod(int a, int b)
try
{
    TIME_BEGIN(timing);
    const auto divisorContainer = numeric::divisor::Divisor().stein(a, b);
    TIME_END(timing);
    DIVISOR_PRINT_RESULT_CONTENT(DivisorMethod::stein);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

#undef DIVISOR_RESULT
#undef DIVISOR_PRINT_RESULT_CONTENT
#undef DIVISOR_METHOD_TABLE
} // namespace divisor

//! @brief Run divisor tasks.
//! @param targets - container of target methods
void runDivisorTasks(const std::vector<std::string>& targets)
{
    if (getBit<DivisorMethod>().none())
    {
        return;
    }

    using divisor::DivisorSolution;
    using divisor::TargetBuilder;
    using divisor::input::integerA;
    using divisor::input::integerB;
    using utility::common::operator""_bkdrHash;

    APP_NUM_PRINT_TASK_BEGIN_TITLE(Category::divisor);
    auto* const threads = command::getPublicThreadPool().newElement(std::min(
        static_cast<std::uint32_t>(getBit<DivisorMethod>().count()),
        static_cast<std::uint32_t>(Bottom<DivisorMethod>::value)));

    const auto builder = std::make_shared<TargetBuilder>(integerA, integerB);
    const auto divisorFunctor = [threads, builder](const std::string& threadName, void (*methodPtr)(int, int))
    {
        threads->enqueue(
            threadName, methodPtr, std::get<0>(builder->getIntegers()), std::get<1>(builder->getIntegers()));
    };

    for (std::uint8_t i = 0; i < Bottom<DivisorMethod>::value; ++i)
    {
        if (!getBit<DivisorMethod>().test(DivisorMethod(i)))
        {
            continue;
        }

        const std::string targetMethod = targets.at(i), threadName = "task-num_d_" + targetMethod;
        switch (utility::common::bkdrHash(targetMethod.data()))
        {
            case "euc"_bkdrHash:
                divisorFunctor(threadName, &DivisorSolution::euclideanMethod);
                break;
            case "ste"_bkdrHash:
                divisorFunctor(threadName, &DivisorSolution::steinMethod);
                break;
            default:
                LOG_INF << "Execute to apply an unknown divisor method.";
                break;
        }
    }

    command::getPublicThreadPool().deleteElement(threads);
    APP_NUM_PRINT_TASK_END_TITLE(Category::divisor);
}

//! @brief Update divisor methods in tasks.
//! @param target - target method
void updateDivisorTask(const std::string& target)
{
    using utility::common::operator""_bkdrHash;
    switch (utility::common::bkdrHash(target.c_str()))
    {
        case "euc"_bkdrHash:
            setBit<DivisorMethod>(DivisorMethod::euclidean);
            break;
        case "ste"_bkdrHash:
            setBit<DivisorMethod>(DivisorMethod::stein);
            break;
        default:
            getBit<DivisorMethod>().reset();
            throw std::runtime_error("Unexpected divisor method: " + target + '.');
    }
}

namespace integral
{
//! @brief Display integral result.
#define INTEGRAL_RESULT(opt) "\r\n==> %-11s Method <==\nI(" #opt ")=%+.5f, run time: %8.5f ms\n"
//! @brief Print integral result content.
#define INTEGRAL_PRINT_RESULT_CONTENT(method, sum) \
    COMMON_PRINT(INTEGRAL_RESULT(def), toString(method).data(), sum, TIME_INTERVAL(timing))
//! @brief Mapping table for enum and string about integral methods. X macro.
#define INTEGRAL_METHOD_TABLE        \
    ELEM(trapezoidal, "Trapezoidal") \
    ELEM(simpson, "Simpson")         \
    ELEM(romberg, "Romberg")         \
    ELEM(gauss, "Gauss")             \
    ELEM(monteCarlo, "MonteCarlo")

//! @brief Convert method enumeration to string.
//! @param method - the specific value of IntegralMethod enum
//! @return method name
constexpr std::string_view toString(const IntegralMethod method)
{
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {INTEGRAL_METHOD_TABLE};
#undef ELEM
    return table[method];
}

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
#undef INTEGRAL_METHOD_TABLE
} // namespace integral

//! @brief Run integral tasks.
//! @param targets - container of target methods
void runIntegralTasks(const std::vector<std::string>& targets)
{
    if (getBit<IntegralMethod>().none())
    {
        return;
    }

    using integral::input::Expression1;
    typedef std::variant<Expression1> IntegralExprTarget;
    constexpr auto printExpr = [](const IntegralExprTarget& expression)
    {
        constexpr std::string_view prefix{"\r\nIntegral expression:\n"};
        std::visit(
            integral::ExprOverloaded{
                [&prefix](const Expression1& /*expr*/)
                {
                    std::cout << prefix << Expression1::exprDescr << std::endl;
                },
            },
            expression);
    };
    const auto calcExpr = [&targets](const auto inputs)
    {
        auto* const threads = command::getPublicThreadPool().newElement(std::min(
            static_cast<std::uint32_t>(getBit<IntegralMethod>().count()),
            static_cast<std::uint32_t>(Bottom<IntegralMethod>::value)));
        const auto integralFunctor = [threads, &inputs](
                                         const std::string& threadName,
                                         void (*methodPtr)(const integral::Expression&, const double, const double))
        {
            threads->enqueue(
                threadName,
                [methodPtr, &inputs]()
                {
                    std::apply(methodPtr, inputs);
                });
        };

        using integral::IntegralSolution;
        using utility::common::operator""_bkdrHash;
        for (std::uint8_t i = 0; i < Bottom<IntegralMethod>::value; ++i)
        {
            if (!getBit<IntegralMethod>().test(IntegralMethod(i)))
            {
                continue;
            }

            const std::string targetMethod = targets.at(i), threadName = "task-num_i_" + targetMethod;
            switch (utility::common::bkdrHash(targetMethod.data()))
            {
                case "tra"_bkdrHash:
                    integralFunctor(threadName, &IntegralSolution::trapezoidalMethod);
                    break;
                case "sim"_bkdrHash:
                    integralFunctor(threadName, &IntegralSolution::adaptiveSimpsonMethod);
                    break;
                case "rom"_bkdrHash:
                    integralFunctor(threadName, &IntegralSolution::rombergMethod);
                    break;
                case "gau"_bkdrHash:
                    integralFunctor(threadName, &IntegralSolution::gaussLegendreMethod);
                    break;
                case "mon"_bkdrHash:
                    integralFunctor(threadName, &IntegralSolution::monteCarloMethod);
                    break;
                default:
                    LOG_INF << "Execute to apply an unknown integral method.";
                    break;
            }
        }
        command::getPublicThreadPool().deleteElement(threads);
    };

    APP_NUM_PRINT_TASK_BEGIN_TITLE(Category::integral);

    const std::unordered_multimap<integral::ExprRange<double, double>, IntegralExprTarget, integral::ExprMapHash>
        integralExprMap{
            {{Expression1::range1, Expression1::range2, Expression1::exprDescr}, Expression1{}},
        };
    for ([[maybe_unused]] const auto& [range, expression] : integralExprMap)
    {
        printExpr(expression);
        switch (expression.index())
        {
            case 0:
                calcExpr(std::make_tuple(std::get<0>(expression), range.range1, range.range2));
                break;
            default:
                break;
        }
    }

    APP_NUM_PRINT_TASK_END_TITLE(Category::integral);
}

//! @brief Update integral methods in tasks.
//! @param target - target method
void updateIntegralTask(const std::string& target)
{
    using utility::common::operator""_bkdrHash;
    switch (utility::common::bkdrHash(target.c_str()))
    {
        case "tra"_bkdrHash:
            setBit<IntegralMethod>(IntegralMethod::trapezoidal);
            break;
        case "sim"_bkdrHash:
            setBit<IntegralMethod>(IntegralMethod::simpson);
            break;
        case "rom"_bkdrHash:
            setBit<IntegralMethod>(IntegralMethod::romberg);
            break;
        case "gau"_bkdrHash:
            setBit<IntegralMethod>(IntegralMethod::gauss);
            break;
        case "mon"_bkdrHash:
            setBit<IntegralMethod>(IntegralMethod::monteCarlo);
            break;
        default:
            getBit<IntegralMethod>().reset();
            throw std::runtime_error("Unexpected integral method: " + target + '.');
    }
}

namespace prime
{
//! @brief Display prime result.
#define PRIME_RESULT "\r\n==> %-9s Method <==\n%s\nrun time: %8.5f ms\n"
//! @brief Print prime result content.
#define PRIME_PRINT_RESULT_CONTENT(method)                                             \
    do                                                                                 \
    {                                                                                  \
        const std::uint32_t arrayBufferSize = primeContainer.size() * maxAlignOfPrint; \
        char arrayBuffer[arrayBufferSize + 1];                                         \
        arrayBuffer[0] = '\0';                                                         \
        COMMON_PRINT(                                                                  \
            PRIME_RESULT,                                                              \
            toString(method).data(),                                                   \
            TargetBuilder::template spliceAllIntegers<std::uint32_t>(                  \
                primeContainer, arrayBuffer, arrayBufferSize + 1),                     \
            TIME_INTERVAL(timing));                                                    \
    }                                                                                  \
    while (0)
//! @brief Mapping table for enum and string about prime methods. X macro.
#define PRIME_METHOD_TABLE             \
    ELEM(eratosthenes, "Eratosthenes") \
    ELEM(euler, "Euler")

//! @brief Convert method enumeration to string.
//! @param method - the specific value of PrimeMethod enum
//! @return method name
constexpr std::string_view toString(const PrimeMethod method)
{
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {PRIME_METHOD_TABLE};
#undef ELEM
    return table[method];
}

void PrimeSolution::eratosthenesMethod(const std::uint32_t max)
try
{
    TIME_BEGIN(timing);
    const auto primeContainer = numeric::prime::Prime().eratosthenes(max);
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
    const auto primeContainer = numeric::prime::Prime().euler(max);
    TIME_END(timing);
    PRIME_PRINT_RESULT_CONTENT(PrimeMethod::euler);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

#undef PRIME_RESULT
#undef PRIME_PRINT_RESULT_CONTENT
#undef PRIME_METHOD_TABLE
} // namespace prime

//! @brief Run prime tasks.
//! @param targets - container of target methods
void runPrimeTasks(const std::vector<std::string>& targets)
{
    if (getBit<PrimeMethod>().none())
    {
        return;
    }

    using prime::PrimeSolution;
    using prime::TargetBuilder;
    using prime::input::maxPositiveInteger;
    using utility::common::operator""_bkdrHash;

    APP_NUM_PRINT_TASK_BEGIN_TITLE(Category::prime);
    auto* const threads = command::getPublicThreadPool().newElement(std::min(
        static_cast<std::uint32_t>(getBit<PrimeMethod>().count()),
        static_cast<std::uint32_t>(Bottom<PrimeMethod>::value)));

    const auto builder = std::make_shared<TargetBuilder>(maxPositiveInteger);
    const auto primeFunctor = [threads, builder](const std::string& threadName, void (*methodPtr)(const std::uint32_t))
    {
        threads->enqueue(threadName, methodPtr, builder->getMaxPositiveInteger());
    };

    for (std::uint8_t i = 0; i < Bottom<PrimeMethod>::value; ++i)
    {
        if (!getBit<PrimeMethod>().test(PrimeMethod(i)))
        {
            continue;
        }

        const std::string targetMethod = targets.at(i), threadName = "task-num_p_" + targetMethod;
        switch (utility::common::bkdrHash(targetMethod.data()))
        {
            case "era"_bkdrHash:
                primeFunctor(threadName, &PrimeSolution::eratosthenesMethod);
                break;
            case "eul"_bkdrHash:
                primeFunctor(threadName, &PrimeSolution::eulerMethod);
                break;
            default:
                LOG_INF << "Execute to apply an unknown prime method.";
                break;
        }
    }

    command::getPublicThreadPool().deleteElement(threads);
    APP_NUM_PRINT_TASK_END_TITLE(Category::prime);
}

//! @brief Update prime methods in tasks.
//! @param target - target method
void updatePrimeTask(const std::string& target)
{
    using utility::common::operator""_bkdrHash;
    switch (utility::common::bkdrHash(target.c_str()))
    {
        case "era"_bkdrHash:
            setBit<PrimeMethod>(PrimeMethod::eratosthenes);
            break;
        case "eul"_bkdrHash:
            setBit<PrimeMethod>(PrimeMethod::euler);
            break;
        default:
            getBit<PrimeMethod>().reset();
            throw std::runtime_error("Unexpected prime method: " + target + '.');
    }
}
} // namespace application::app_num
