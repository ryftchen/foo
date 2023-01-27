//! @file apply_numeric.cpp
//! @author ryftchen
//! @brief The definitions (apply_numeric) in the application module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023
#include "apply_numeric.hpp"
#include <variant>
#include "application/include/command.hpp"
#include "numeric/include/arithmetic.hpp"
#include "numeric/include/divisor.hpp"
#include "numeric/include/integral.hpp"
#include "numeric/include/prime.hpp"
#include "utility/include/hash.hpp"
#include "utility/include/log.hpp"
#include "utility/include/thread.hpp"

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
              << taskType << "END" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << "\r\n"     \
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

//! @brief Set input parameters.
namespace input
{
//! @brief One of integers for arithmetic methods.
constexpr int integerForArithmetic1 = 1073741823;
//! @brief One of integers for arithmetic methods.
constexpr int integerForArithmetic2 = -2;
//! @brief One of integers for divisor methods.
constexpr int integerForDivisor1 = 2 * 2 * 3 * 3 * 5 * 5 * 7 * 7;
//! @brief One of integers for divisor methods.
constexpr int integerForDivisor2 = 2 * 3 * 5 * 7 * 11 * 13 * 17;
//! @brief Max positive integer for prime methods.
constexpr uint32_t maxPositiveIntegerForPrime = 997;

//! @brief Expression example 1.
class Expression1 : public numeric::integral::expression::Expression
{
public:
    //! @brief The operator (()) overloading of Expression1 class.
    //! @param x - independent variable
    //! @return dependent variable
    double operator()(const double x) const override { return ((x * std::sin(x)) / (1.0 + std::cos(x) * std::cos(x))); }

    //! @brief Left endpoint.
    static constexpr double range1{-M_PI / 2.0};
    //! @brief Right endpoint.
    static constexpr double range2{2.0 * M_PI};
    //! @brief Expression example 1.
    static constexpr std::string_view exprDescr{"I=∫(-π/2→2π)x*sin(x)/(1+(cos(x))^2)dx"};
};

//! @brief Expression example 2.
class Expression2 : public numeric::integral::expression::Expression
{
public:
    //! @brief The operator (()) overloading of Expression2 class.
    //! @param x - independent variable
    //! @return dependent variable
    double operator()(const double x) const override
    {
        return (x + 10.0 * std::sin(5.0 * x) + 7.0 * std::cos(4.0 * x));
    }

    //! @brief Left endpoint.
    static constexpr double range1{0.0};
    //! @brief Right endpoint.
    static constexpr double range2{9.0};
    //! @brief Expression example 2.
    static constexpr std::string_view exprDescr{"I=∫(0→9)x+10sin(5x)+7cos(4x)dx"};
};
} // namespace input

//! @brief Run arithmetic tasks.
//! @param targets - vector of target methods
void runArithmetic(const std::vector<std::string>& targets)
{
    if (getBit<ArithmeticMethod>().none())
    {
        return;
    }

    using numeric::arithmetic::ArithmeticSolution;
    using numeric::arithmetic::TargetBuilder;
    using utility::hash::operator""_bkdrHash;

    APP_NUM_PRINT_TASK_BEGIN_TITLE(Type::arithmetic);
    auto* threads = command::getMemoryForMultithreading().newElement(std::min(
        static_cast<uint32_t>(getBit<ArithmeticMethod>().count()),
        static_cast<uint32_t>(Bottom<ArithmeticMethod>::value)));

    const std::shared_ptr<TargetBuilder> builder =
        std::make_shared<TargetBuilder>(input::integerForArithmetic1, input::integerForArithmetic2);
    const auto arithmeticFunctor = [&](const std::string& threadName, int (*methodPtr)(const int, const int))
    {
        threads->enqueue(
            threadName, methodPtr, std::get<0>(builder->getIntegers()), std::get<1>(builder->getIntegers()));
    };

    for (int i = 0; i < Bottom<ArithmeticMethod>::value; ++i)
    {
        if (!getBit<ArithmeticMethod>().test(ArithmeticMethod(i)))
        {
            continue;
        }

        const std::string targetMethod = targets.at(i), threadName = "a_" + targetMethod;
        switch (utility::hash::bkdrHash(targetMethod.data()))
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
                LOG_DBG("execute to run unknown arithmetic method.");
                break;
        }
    }

    command::getMemoryForMultithreading().deleteElement(threads);
    APP_NUM_PRINT_TASK_END_TITLE(Type::arithmetic);
}

//! @brief Update arithmetic methods in tasks.
//! @param target - target method
void updateArithmeticTask(const std::string& target)
{
    using utility::hash::operator""_bkdrHash;
    switch (utility::hash::bkdrHash(target.c_str()))
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
            throw std::runtime_error("Unexpected task of arithmetic: " + target);
    }
}

//! @brief Run divisor tasks.
//! @param targets - vector of target methods
void runDivisor(const std::vector<std::string>& targets)
{
    if (getBit<DivisorMethod>().none())
    {
        return;
    }

    using numeric::divisor::DivisorSolution;
    using numeric::divisor::TargetBuilder;
    using utility::hash::operator""_bkdrHash;

    APP_NUM_PRINT_TASK_BEGIN_TITLE(Type::divisor);
    auto* threads = command::getMemoryForMultithreading().newElement(std::min(
        static_cast<uint32_t>(getBit<DivisorMethod>().count()), static_cast<uint32_t>(Bottom<DivisorMethod>::value)));

    const std::shared_ptr<TargetBuilder> builder =
        std::make_shared<TargetBuilder>(input::integerForDivisor1, input::integerForDivisor2);
    const auto divisorFunctor = [&](const std::string& threadName, std::vector<int> (*methodPtr)(int, int))
    {
        threads->enqueue(
            threadName, methodPtr, std::get<0>(builder->getIntegers()), std::get<1>(builder->getIntegers()));
    };

    for (int i = 0; i < Bottom<DivisorMethod>::value; ++i)
    {
        if (!getBit<DivisorMethod>().test(DivisorMethod(i)))
        {
            continue;
        }

        const std::string targetMethod = targets.at(i), threadName = "d_" + targetMethod;
        switch (utility::hash::bkdrHash(targetMethod.data()))
        {
            case "euc"_bkdrHash:
                divisorFunctor(threadName, &DivisorSolution::euclideanMethod);
                break;
            case "ste"_bkdrHash:
                divisorFunctor(threadName, &DivisorSolution::steinMethod);
                break;
            default:
                LOG_DBG("execute to run unknown divisor method.");
                break;
        }
    }

    command::getMemoryForMultithreading().deleteElement(threads);
    APP_NUM_PRINT_TASK_END_TITLE(Type::divisor);
}

//! @brief Update divisor methods in tasks.
//! @param target - target method
void updateDivisorTask(const std::string& target)
{
    using utility::hash::operator""_bkdrHash;
    switch (utility::hash::bkdrHash(target.c_str()))
    {
        case "euc"_bkdrHash:
            setBit<DivisorMethod>(DivisorMethod::euclidean);
            break;
        case "ste"_bkdrHash:
            setBit<DivisorMethod>(DivisorMethod::stein);
            break;
        default:
            getBit<DivisorMethod>().reset();
            throw std::runtime_error("Unexpected task of divisor: " + target);
    }
}

//! @brief Run integral tasks.
//! @param targets - vector of target methods
void runIntegral(const std::vector<std::string>& targets)
{
    if (getBit<IntegralMethod>().none())
    {
        return;
    }

    using Expression1 = input::Expression1;
    using Expression2 = input::Expression2;
    typedef std::variant<Expression1, Expression2> IntegralExprTarget;

#ifndef _NO_PRINT_AT_RUNTIME
    const auto printFunctor = [](const IntegralExprTarget& expression)
    {
        constexpr std::string_view prefix{"\r\nIntegral expression: "};
        std::visit(
            numeric::integral::expression::ExprOverloaded{
                [&prefix](const Expression1& /*unused*/)
                {
                    std::cout << prefix << Expression1::exprDescr << std::endl;
                },
                [&prefix](const Expression2& /*unused*/)
                {
                    std::cout << prefix << Expression2::exprDescr << std::endl;
                },
            },
            expression);
    };
#endif
    const auto resultFunctor = [targets](
                                   const numeric::integral::expression::Expression& expression,
                                   const numeric::integral::expression::ExprRange<double, double>& range)
    {
        static_assert(numeric::integral::epsilon > 0.0);
        auto* threads = command::getMemoryForMultithreading().newElement(std::min(
            static_cast<uint32_t>(getBit<IntegralMethod>().count()),
            static_cast<uint32_t>(Bottom<IntegralMethod>::value)));
        const auto integralFunctor =
            [&](const std::string& threadName, const std::shared_ptr<numeric::integral::IntegralSolution>& classPtr)
        {
            threads->enqueue(
                threadName,
                &numeric::integral::IntegralSolution::operator(),
                classPtr,
                range.range1,
                range.range2,
                numeric::integral::epsilon);
        };

        using utility::hash::operator""_bkdrHash;
        for (int i = 0; i < Bottom<IntegralMethod>::value; ++i)
        {
            if (!getBit<IntegralMethod>().test(IntegralMethod(i)))
            {
                continue;
            }

            const std::string targetMethod = targets.at(i), threadName = "i_" + targetMethod;
            switch (utility::hash::bkdrHash(targetMethod.data()))
            {
                case "tra"_bkdrHash:
                    integralFunctor(threadName, std::make_shared<numeric::integral::Trapezoidal>(expression));
                    break;
                case "sim"_bkdrHash:
                    integralFunctor(threadName, std::make_shared<numeric::integral::Simpson>(expression));
                    break;
                case "rom"_bkdrHash:
                    integralFunctor(threadName, std::make_shared<numeric::integral::Romberg>(expression));
                    break;
                case "gau"_bkdrHash:
                    integralFunctor(threadName, std::make_shared<numeric::integral::Gauss>(expression));
                    break;
                case "mon"_bkdrHash:
                    integralFunctor(threadName, std::make_shared<numeric::integral::MonteCarlo>(expression));
                    break;
                default:
                    LOG_DBG("execute to run unknown integral method.");
                    break;
            }
        }
        command::getMemoryForMultithreading().deleteElement(threads);
    };

    APP_NUM_PRINT_TASK_BEGIN_TITLE(Type::integral);

    const std::unordered_multimap<
        numeric::integral::expression::ExprRange<double, double>,
        IntegralExprTarget,
        numeric::integral::expression::ExprMapHash>
        integralExprMap{
            {{Expression1::range1, Expression1::range2, Expression1::exprDescr}, Expression1()},
            {{Expression2::range1, Expression2::range2, Expression2::exprDescr}, Expression2()}};
    for ([[maybe_unused]] const auto& [range, expression] : integralExprMap)
    {
#ifndef _NO_PRINT_AT_RUNTIME
        printFunctor(expression);
#endif
        switch (expression.index())
        {
            case 0:
                resultFunctor(std::get<0>(expression), range);
                break;
            case 1:
                resultFunctor(std::get<1>(expression), range);
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
    using utility::hash::operator""_bkdrHash;
    switch (utility::hash::bkdrHash(target.c_str()))
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
            throw std::runtime_error("Unexpected task of integral: " + target);
    }
}

//! @brief Run prime tasks.
//! @param targets - vector of target methods
void runPrime(const std::vector<std::string>& targets)
{
    if (getBit<PrimeMethod>().none())
    {
        return;
    }

    using numeric::prime::PrimeSolution;
    using numeric::prime::TargetBuilder;
    using utility::hash::operator""_bkdrHash;

    APP_NUM_PRINT_TASK_BEGIN_TITLE(Type::prime);
    auto* threads = command::getMemoryForMultithreading().newElement(std::min(
        static_cast<uint32_t>(getBit<PrimeMethod>().count()), static_cast<uint32_t>(Bottom<PrimeMethod>::value)));

    const std::shared_ptr<TargetBuilder> builder = std::make_shared<TargetBuilder>(input::maxPositiveIntegerForPrime);
    const auto primeFunctor = [&](const std::string& threadName, std::vector<uint32_t> (*methodPtr)(const uint32_t))
    {
        threads->enqueue(threadName, methodPtr, builder->getMaxPositiveInteger());
    };

    for (int i = 0; i < Bottom<PrimeMethod>::value; ++i)
    {
        if (!getBit<PrimeMethod>().test(PrimeMethod(i)))
        {
            continue;
        }

        const std::string targetMethod = targets.at(i), threadName = "p_" + targetMethod;
        switch (utility::hash::bkdrHash(targetMethod.data()))
        {
            case "era"_bkdrHash:
                primeFunctor(threadName, &PrimeSolution::eratosthenesMethod);
                break;
            case "eul"_bkdrHash:
                primeFunctor(threadName, &PrimeSolution::eulerMethod);
                break;
            default:
                LOG_DBG("execute to run unknown prime method.");
                break;
        }
    }

    command::getMemoryForMultithreading().deleteElement(threads);
    APP_NUM_PRINT_TASK_END_TITLE(Type::prime);
}

//! @brief Update prime methods in tasks.
//! @param target - target method
void updatePrimeTask(const std::string& target)
{
    using utility::hash::operator""_bkdrHash;
    switch (utility::hash::bkdrHash(target.c_str()))
    {
        case "era"_bkdrHash:
            setBit<PrimeMethod>(PrimeMethod::eratosthenes);
            break;
        case "eul"_bkdrHash:
            setBit<PrimeMethod>(PrimeMethod::euler);
            break;
        default:
            getBit<PrimeMethod>().reset();
            throw std::runtime_error("Unexpected task of prime: " + target);
    }
}
} // namespace application::app_num
