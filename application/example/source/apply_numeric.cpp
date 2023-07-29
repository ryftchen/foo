//! @file apply_numeric.cpp
//! @author ryftchen
//! @brief The definitions (apply_numeric) in the application module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen.

#include "apply_numeric.hpp"
#ifndef __PRECOMPILED_HEADER
#include <iomanip>
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
#define APP_NUM_PRINT_TASK_BEGIN_TITLE(taskType)                                                                   \
    std::cout << "\r\n"                                                                                            \
              << "NUMERIC TASK: " << std::setiosflags(std::ios_base::left) << std::setfill('.') << std::setw(50)   \
              << taskType << "BEGIN" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl; \
    {
//! @brief Title of printing when numeric tasks are ending.
#define APP_NUM_PRINT_TASK_END_TITLE(taskType)                                                                   \
    }                                                                                                            \
    std::cout << "\r\n"                                                                                          \
              << "NUMERIC TASK: " << std::setiosflags(std::ios_base::left) << std::setfill('.') << std::setw(50) \
              << taskType << "END" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << '\n'       \
              << std::endl;

namespace application::app_num
{
//! @brief Alias for Type.
using Type = NumericTask::Type;
//! @brief Alias for Bottom.
//! @tparam T - type of specific enum
template <class T>
using Bottom = NumericTask::Bottom<T>;
//! @brief Alias for ArithmeticMethod.
using ArithmeticMethod = NumericTask::ArithmeticMethod;
//! @brief Alias for DivisorMethod.
using DivisorMethod = NumericTask::DivisorMethod;
//! @brief Alias for IntegralMethod.
using IntegralMethod = NumericTask::IntegralMethod;
//! @brief Alias for PrimeMethod.
using PrimeMethod = NumericTask::PrimeMethod;

//! @brief Get the numeric task.
//! @return reference of NumericTask object
NumericTask& getTask()
{
    static NumericTask task;
    return task;
}

namespace arithmetic
{
//! @brief Display arithmetic result.
#define ARITHMETIC_RESULT "\r\n==> %-14s Method <==\n(%d) %c (%d) = %d\n"
//! @brief Print arithmetic result content.
#define ARITHMETIC_PRINT_RESULT_CONTENT(method, a, operator, b, result) \
    COMMON_PRINT(ARITHMETIC_RESULT, method, a, operator, b, result)

void ArithmeticSolution::additionMethod(const int augend, const int addend)
{
    try
    {
        const auto sum = numeric::arithmetic::Arithmetic().addition(augend, addend);
        ARITHMETIC_PRINT_RESULT_CONTENT("Addition", augend, '+', addend, sum);
    }
    catch (const std::exception& error)
    {
        LOG_ERR(error.what());
    }
}

void ArithmeticSolution::subtractionMethod(const int minuend, const int subtrahend)
{
    try
    {
        const auto difference = numeric::arithmetic::Arithmetic().subtraction(minuend, subtrahend);
        ARITHMETIC_PRINT_RESULT_CONTENT("Subtraction", minuend, '-', subtrahend, difference);
    }
    catch (const std::exception& error)
    {
        LOG_ERR(error.what());
    }
}

void ArithmeticSolution::multiplicationMethod(const int multiplier, const int multiplicand)
{
    try
    {
        const auto product = numeric::arithmetic::Arithmetic().multiplication(multiplier, multiplicand);
        ARITHMETIC_PRINT_RESULT_CONTENT("Multiplication", multiplier, '*', multiplicand, product);
    }
    catch (const std::exception& error)
    {
        LOG_ERR(error.what());
    }
}

void ArithmeticSolution::divisionMethod(const int dividend, const int divisor)
{
    try
    {
        const auto quotient = numeric::arithmetic::Arithmetic().division(dividend, divisor);
        ARITHMETIC_PRINT_RESULT_CONTENT("Division", dividend, '/', divisor, quotient);
    }
    catch (const std::exception& error)
    {
        LOG_ERR(error.what());
    }
}
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
    using utility::common::operator""_bkdrHash;

    APP_NUM_PRINT_TASK_BEGIN_TITLE(Type::arithmetic);
    auto* threads = command::getPublicThreadPool().newElement(std::min(
        static_cast<std::uint32_t>(getBit<ArithmeticMethod>().count()),
        static_cast<std::uint32_t>(Bottom<ArithmeticMethod>::value)));

    const auto builder = std::make_shared<TargetBuilder>(input::integerForArithmetic1, input::integerForArithmetic2);
    const auto arithmeticFunctor = [&](const std::string& threadName, void (*methodPtr)(const int, const int))
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

        const std::string targetMethod = targets.at(i), threadName = "a_" + targetMethod;
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
                LOG_DBG("Execute to apply an unknown arithmetic method.");
                break;
        }
    }

    command::getPublicThreadPool().deleteElement(threads);
    APP_NUM_PRINT_TASK_END_TITLE(Type::arithmetic);
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
            method,                                                                                             \
            TargetBuilder::template spliceAllIntegers<int>(divisorContainer, arrayBuffer, arrayBufferSize + 1), \
            TIME_INTERVAL(timing));                                                                             \
    }                                                                                                           \
    while (0)

void DivisorSolution::euclideanMethod(int a, int b)
{
    try
    {
        TIME_BEGIN(timing);
        const auto divisorContainer = numeric::divisor::Divisor().euclidean(a, b);
        TIME_END(timing);
        DIVISOR_PRINT_RESULT_CONTENT("Euclidean");
    }
    catch (const std::exception& error)
    {
        LOG_ERR(error.what());
    }
}

void DivisorSolution::steinMethod(int a, int b)
{
    try
    {
        TIME_BEGIN(timing);
        const auto divisorContainer = numeric::divisor::Divisor().stein(a, b);
        TIME_END(timing);
        DIVISOR_PRINT_RESULT_CONTENT("Stein");
    }
    catch (const std::exception& error)
    {
        LOG_ERR(error.what());
    }
}
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
    using utility::common::operator""_bkdrHash;

    APP_NUM_PRINT_TASK_BEGIN_TITLE(Type::divisor);
    auto* threads = command::getPublicThreadPool().newElement(std::min(
        static_cast<std::uint32_t>(getBit<DivisorMethod>().count()),
        static_cast<std::uint32_t>(Bottom<DivisorMethod>::value)));

    const auto builder = std::make_shared<TargetBuilder>(input::integerForDivisor1, input::integerForDivisor2);
    const auto divisorFunctor = [&](const std::string& threadName, void (*methodPtr)(int, int))
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

        const std::string targetMethod = targets.at(i), threadName = "d_" + targetMethod;
        switch (utility::common::bkdrHash(targetMethod.data()))
        {
            case "euc"_bkdrHash:
                divisorFunctor(threadName, &DivisorSolution::euclideanMethod);
                break;
            case "ste"_bkdrHash:
                divisorFunctor(threadName, &DivisorSolution::steinMethod);
                break;
            default:
                LOG_DBG("Execute to apply an unknown divisor method.");
                break;
        }
    }

    command::getPublicThreadPool().deleteElement(threads);
    APP_NUM_PRINT_TASK_END_TITLE(Type::divisor);
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
    COMMON_PRINT(INTEGRAL_RESULT(def), method, sum, TIME_INTERVAL(timing))

void IntegralSolution::trapezoidalMethod(const Expression& expr, double lower, double upper)
{
    try
    {
        TIME_BEGIN(timing);
        const auto sum = numeric::integral::Trapezoidal(expr)(lower, upper, numeric::integral::epsilon);
        TIME_END(timing);
        INTEGRAL_PRINT_RESULT_CONTENT("Trapezoidal", sum);
    }
    catch (const std::exception& error)
    {
        LOG_ERR(error.what());
    }
}

void IntegralSolution::adaptiveSimpsonMethod(const Expression& expr, const double lower, const double upper)
{
    try
    {
        TIME_BEGIN(timing);
        const auto sum = numeric::integral::Trapezoidal(expr)(lower, upper, numeric::integral::epsilon);
        TIME_END(timing);
        INTEGRAL_PRINT_RESULT_CONTENT("Simpson", sum);
    }
    catch (const std::exception& error)
    {
        LOG_ERR(error.what());
    }
}

void IntegralSolution::rombergMethod(const Expression& expr, const double lower, const double upper)
{
    try
    {
        TIME_BEGIN(timing);
        const auto sum = numeric::integral::Romberg(expr)(lower, upper, numeric::integral::epsilon);
        TIME_END(timing);
        INTEGRAL_PRINT_RESULT_CONTENT("Romberg", sum);
    }
    catch (const std::exception& error)
    {
        LOG_ERR(error.what());
    }
}

void IntegralSolution::gaussLegendreMethod(const Expression& expr, const double lower, const double upper)
{
    try
    {
        TIME_BEGIN(timing);
        const auto sum = numeric::integral::Gauss(expr)(lower, upper, numeric::integral::epsilon);
        TIME_END(timing);
        INTEGRAL_PRINT_RESULT_CONTENT("Gauss", sum);
    }
    catch (const std::exception& error)
    {
        LOG_ERR(error.what());
    }
}

void IntegralSolution::monteCarloMethod(const Expression& expr, const double lower, const double upper)
{
    try
    {
        TIME_BEGIN(timing);
        const auto sum = numeric::integral::MonteCarlo(expr)(lower, upper, numeric::integral::epsilon);
        TIME_END(timing);
        INTEGRAL_PRINT_RESULT_CONTENT("MonteCarlo", sum);
    }
    catch (const std::exception& error)
    {
        LOG_ERR(error.what());
    }
}
} // namespace integral

//! @brief Run integral tasks.
//! @param targets - container of target methods
void runIntegralTasks(const std::vector<std::string>& targets)
{
    if (getBit<IntegralMethod>().none())
    {
        return;
    }

    using Expression1 = input::Expression1;
    typedef std::variant<Expression1> IntegralExprTarget;
    const auto printFunctor = [](const IntegralExprTarget& expression)
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
    const auto resultFunctor =
        [targets](const integral::Expression& expression, const integral::ExprRange<double, double>& range)
    {
        auto* threads = command::getPublicThreadPool().newElement(std::min(
            static_cast<std::uint32_t>(getBit<IntegralMethod>().count()),
            static_cast<std::uint32_t>(Bottom<IntegralMethod>::value)));
        const auto integralFunctor = [&](const std::string& threadName,
                                         void (*methodPtr)(const integral::Expression&, const double, const double))
        {
            threads->enqueue(threadName, methodPtr, std::ref(expression), range.range1, range.range2);
        };

        using integral::IntegralSolution;
        using utility::common::operator""_bkdrHash;
        for (std::uint8_t i = 0; i < Bottom<IntegralMethod>::value; ++i)
        {
            if (!getBit<IntegralMethod>().test(IntegralMethod(i)))
            {
                continue;
            }

            const std::string targetMethod = targets.at(i), threadName = "i_" + targetMethod;
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
                    LOG_DBG("Execute to apply an unknown integral method.");
                    break;
            }
        }
        command::getPublicThreadPool().deleteElement(threads);
    };

    APP_NUM_PRINT_TASK_BEGIN_TITLE(Type::integral);

    const std::unordered_multimap<integral::ExprRange<double, double>, IntegralExprTarget, integral::ExprMapHash>
        integralExprMap{
            {{Expression1::range1, Expression1::range2, Expression1::exprDescr}, Expression1{}},
        };
    for ([[maybe_unused]] const auto& [range, expression] : integralExprMap)
    {
        printFunctor(expression);
        switch (expression.index())
        {
            case 0:
                resultFunctor(std::get<0>(expression), range);
                break;
                [[unlikely]] default : break;
        }
    }

    APP_NUM_PRINT_TASK_END_TITLE(Type::integral);
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
            method,                                                                    \
            TargetBuilder::template spliceAllIntegers<std::uint32_t>(                  \
                primeContainer, arrayBuffer, arrayBufferSize + 1),                     \
            TIME_INTERVAL(timing));                                                    \
    }                                                                                  \
    while (0)

void PrimeSolution::eratosthenesMethod(const std::uint32_t max)
{
    try
    {
        TIME_BEGIN(timing);
        const auto primeContainer = numeric::prime::Prime().eratosthenes(max);
        TIME_END(timing);
        PRIME_PRINT_RESULT_CONTENT("Eratosthenes");
    }
    catch (const std::exception& error)
    {
        LOG_ERR(error.what());
    }
}

void PrimeSolution::eulerMethod(const std::uint32_t max)
{
    try
    {
        TIME_BEGIN(timing);
        const auto primeContainer = numeric::prime::Prime().euler(max);
        TIME_END(timing);
        PRIME_PRINT_RESULT_CONTENT("Euler");
    }
    catch (const std::exception& error)
    {
        LOG_ERR(error.what());
    }
}
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
    using utility::common::operator""_bkdrHash;

    APP_NUM_PRINT_TASK_BEGIN_TITLE(Type::prime);
    auto* threads = command::getPublicThreadPool().newElement(std::min(
        static_cast<std::uint32_t>(getBit<PrimeMethod>().count()),
        static_cast<std::uint32_t>(Bottom<PrimeMethod>::value)));

    const auto builder = std::make_shared<TargetBuilder>(input::maxPositiveIntegerForPrime);
    const auto primeFunctor = [&](const std::string& threadName, void (*methodPtr)(const std::uint32_t))
    {
        threads->enqueue(threadName, methodPtr, builder->getMaxPositiveInteger());
    };

    for (std::uint8_t i = 0; i < Bottom<PrimeMethod>::value; ++i)
    {
        if (!getBit<PrimeMethod>().test(PrimeMethod(i)))
        {
            continue;
        }

        const std::string targetMethod = targets.at(i), threadName = "p_" + targetMethod;
        switch (utility::common::bkdrHash(targetMethod.data()))
        {
            case "era"_bkdrHash:
                primeFunctor(threadName, &PrimeSolution::eratosthenesMethod);
                break;
            case "eul"_bkdrHash:
                primeFunctor(threadName, &PrimeSolution::eulerMethod);
                break;
            default:
                LOG_DBG("Execute to apply an unknown prime method.");
                break;
        }
    }

    command::getPublicThreadPool().deleteElement(threads);
    APP_NUM_PRINT_TASK_END_TITLE(Type::prime);
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
