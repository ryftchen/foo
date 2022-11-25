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

#define NUMERIC_PRINT_TASK_BEGIN_TITLE(taskType)                                                                   \
    std::cout << "\r\n"                                                                                            \
              << "NUMERIC TASK: " << std::setiosflags(std::ios_base::left) << std::setfill('.') << std::setw(50)   \
              << taskType << "BEGIN" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl; \
    {
#define NUMERIC_PRINT_TASK_END_TITLE(taskType)                                                                   \
    }                                                                                                            \
    std::cout << "\r\n"                                                                                          \
              << "NUMERIC TASK: " << std::setiosflags(std::ios_base::left) << std::setfill('.') << std::setw(50) \
              << taskType << "END" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << "\r\n"     \
              << std::endl;

namespace app_num
{
using Type = NumericTask::Type;
template <class T>
using Bottom = NumericTask::Bottom<T>;
using ArithmeticMethod = NumericTask::ArithmeticMethod;
using DivisorMethod = NumericTask::DivisorMethod;
using IntegralMethod = NumericTask::IntegralMethod;
using PrimeMethod = NumericTask::PrimeMethod;

NumericTask& getTask()
{
    static NumericTask task;
    return task;
}

namespace input
{
constexpr int integerForArithmetic1 = 1073741823;
constexpr int integerForArithmetic2 = -2;
constexpr int integerForDivisor1 = 2 * 2 * 3 * 3 * 5 * 5 * 7 * 7;
constexpr int integerForDivisor2 = 2 * 3 * 5 * 7 * 11 * 13 * 17;
constexpr uint32_t maxPositiveIntegerForPrime = 997;

class Expression1 : public num_integral::expression::Expression
{
public:
    double operator()(const double x) const override { return ((x * std::sin(x)) / (1.0 + std::cos(x) * std::cos(x))); }

    static constexpr double range1{-M_PI / 2.0};
    static constexpr double range2{2.0 * M_PI};
    static constexpr std::string_view exprStr{"I=∫(-π/2→2π)x*sin(x)/(1+(cos(x))^2)dx"};
};

class Expression2 : public num_integral::expression::Expression
{
public:
    double operator()(const double x) const override
    {
        return (x + 10.0 * std::sin(5.0 * x) + 7.0 * std::cos(4.0 * x));
    }

    static constexpr double range1{0.0};
    static constexpr double range2{9.0};
    static constexpr std::string_view exprStr{"I=∫(0→9)x+10sin(5x)+7cos(4x)dx"};
};
} // namespace input

void runArithmetic(const std::vector<std::string>& targets)
{
    if (getBit<ArithmeticMethod>().none())
    {
        return;
    }

    NUMERIC_PRINT_TASK_BEGIN_TITLE(Type::arithmetic);
    auto* threads = app_command::getMemoryForMultithreading().newElement(std::min(
        static_cast<uint32_t>(getBit<ArithmeticMethod>().count()),
        static_cast<uint32_t>(Bottom<ArithmeticMethod>::value)));

    using input::integerForArithmetic1;
    using input::integerForArithmetic2;
    using num_arithmetic::ArithmeticSolution;
    const std::shared_ptr<ArithmeticSolution> arithmetic =
        std::make_shared<ArithmeticSolution>(integerForArithmetic1, integerForArithmetic2);
    const auto arithmeticFunctor =
        [&](const std::string& threadName, int (ArithmeticSolution::*methodPtr)(const int, const int) const)
    {
        threads->enqueue(threadName, methodPtr, arithmetic, integerForArithmetic1, integerForArithmetic2);
    };

    using util_hash::operator""_bkdrHash;
    for (int i = 0; i < Bottom<ArithmeticMethod>::value; ++i)
    {
        if (!getBit<ArithmeticMethod>().test(ArithmeticMethod(i)))
        {
            continue;
        }

        const std::string targetMethod = targets.at(i), threadName = "a_" + targetMethod;
        switch (util_hash::bkdrHash(targetMethod.data()))
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

    app_command::getMemoryForMultithreading().deleteElement(threads);
    NUMERIC_PRINT_TASK_END_TITLE(Type::arithmetic);
}

void updateArithmeticTask(const std::string& target)
{
    using util_hash::operator""_bkdrHash;
    switch (util_hash::bkdrHash(target.c_str()))
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

void runDivisor(const std::vector<std::string>& targets)
{
    if (getBit<DivisorMethod>().none())
    {
        return;
    }

    NUMERIC_PRINT_TASK_BEGIN_TITLE(Type::divisor);
    auto* threads = app_command::getMemoryForMultithreading().newElement(std::min(
        static_cast<uint32_t>(getBit<DivisorMethod>().count()), static_cast<uint32_t>(Bottom<DivisorMethod>::value)));

    using input::integerForDivisor1;
    using input::integerForDivisor2;
    using num_divisor::DivisorSolution;
    const std::shared_ptr<DivisorSolution> divisor =
        std::make_shared<DivisorSolution>(integerForDivisor1, integerForDivisor2);
    const auto divisorFunctor =
        [&](const std::string& threadName, std::vector<int> (DivisorSolution::*methodPtr)(int, int) const)
    {
        threads->enqueue(threadName, methodPtr, divisor, integerForDivisor1, integerForDivisor2);
    };

    using util_hash::operator""_bkdrHash;
    for (int i = 0; i < Bottom<DivisorMethod>::value; ++i)
    {
        if (!getBit<DivisorMethod>().test(DivisorMethod(i)))
        {
            continue;
        }

        const std::string targetMethod = targets.at(i), threadName = "d_" + targetMethod;
        switch (util_hash::bkdrHash(targetMethod.data()))
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

    app_command::getMemoryForMultithreading().deleteElement(threads);
    NUMERIC_PRINT_TASK_END_TITLE(Type::divisor);
}

void updateDivisorTask(const std::string& target)
{
    using util_hash::operator""_bkdrHash;
    switch (util_hash::bkdrHash(target.c_str()))
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

void runIntegral(const std::vector<std::string>& targets)
{
    if (getBit<IntegralMethod>().none())
    {
        return;
    }

    using Expression1 = input::Expression1;
    using Expression2 = input::Expression2;
    typedef std::variant<Expression1, Expression2> IntegralExprTarget;
    const auto printFunctor = [](const IntegralExprTarget& expression)
    {
        constexpr std::string_view prefix{"\r\nIntegral expression: "};
        std::visit(
            num_integral::expression::ExprOverloaded{
                [&prefix](const Expression1& /*unused*/)
                {
                    std::cout << prefix << Expression1::exprStr << std::endl;
                },
                [&prefix](const Expression2& /*unused*/)
                {
                    std::cout << prefix << Expression2::exprStr << std::endl;
                },
            },
            expression);
    };
    const auto resultFunctor = [targets](
                                   const num_integral::expression::Expression& expression,
                                   const num_integral::expression::ExprRange<double, double>& range)
    {
        static_assert(num_integral::epsilon > 0.0);
        auto* threads = app_command::getMemoryForMultithreading().newElement(std::min(
            static_cast<uint32_t>(getBit<IntegralMethod>().count()),
            static_cast<uint32_t>(Bottom<IntegralMethod>::value)));
        const auto integralFunctor =
            [&](const std::string& threadName, const std::shared_ptr<num_integral::IntegralSolution>& classPtr)
        {
            threads->enqueue(
                threadName,
                &num_integral::IntegralSolution::operator(),
                classPtr,
                range.range1,
                range.range2,
                num_integral::epsilon);
        };

        using util_hash::operator""_bkdrHash;
        for (int i = 0; i < Bottom<IntegralMethod>::value; ++i)
        {
            if (!getBit<IntegralMethod>().test(IntegralMethod(i)))
            {
                continue;
            }

            const std::string targetMethod = targets.at(i), threadName = "i_" + targetMethod;
            switch (util_hash::bkdrHash(targetMethod.data()))
            {
                case "tra"_bkdrHash:
                    integralFunctor(threadName, std::make_shared<num_integral::Trapezoidal>(expression));
                    break;
                case "sim"_bkdrHash:
                    integralFunctor(threadName, std::make_shared<num_integral::Simpson>(expression));
                    break;
                case "rom"_bkdrHash:
                    integralFunctor(threadName, std::make_shared<num_integral::Romberg>(expression));
                    break;
                case "gau"_bkdrHash:
                    integralFunctor(threadName, std::make_shared<num_integral::Gauss>(expression));
                    break;
                case "mon"_bkdrHash:
                    integralFunctor(threadName, std::make_shared<num_integral::MonteCarlo>(expression));
                    break;
                default:
                    LOG_DBG("execute to run unknown integral method.");
                    break;
            }
        }
        app_command::getMemoryForMultithreading().deleteElement(threads);
    };

    NUMERIC_PRINT_TASK_BEGIN_TITLE(Type::integral);

    const std::unordered_multimap<
        num_integral::expression::ExprRange<double, double>,
        IntegralExprTarget,
        num_integral::expression::ExprMapHash>
        integralExprMap{
            {{Expression1::range1, Expression1::range2, Expression1::exprStr}, Expression1()},
            {{Expression2::range1, Expression2::range2, Expression2::exprStr}, Expression2()}};
    for ([[maybe_unused]] const auto& [range, expression] : integralExprMap)
    {
        printFunctor(expression);
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

    NUMERIC_PRINT_TASK_END_TITLE(Type::integral);
}

void updateIntegralTask(const std::string& target)
{
    using util_hash::operator""_bkdrHash;
    switch (util_hash::bkdrHash(target.c_str()))
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

void runPrime(const std::vector<std::string>& targets)
{
    if (getBit<PrimeMethod>().none())
    {
        return;
    }

    NUMERIC_PRINT_TASK_BEGIN_TITLE(Type::prime);
    auto* threads = app_command::getMemoryForMultithreading().newElement(std::min(
        static_cast<uint32_t>(getBit<PrimeMethod>().count()), static_cast<uint32_t>(Bottom<PrimeMethod>::value)));

    using input::maxPositiveIntegerForPrime;
    using num_prime::PrimeSolution;
    const std::shared_ptr<PrimeSolution> prime = std::make_shared<PrimeSolution>(maxPositiveIntegerForPrime);
    const auto primeFunctor =
        [&](const std::string& threadName, std::vector<uint32_t> (PrimeSolution::*methodPtr)(const uint32_t) const)
    {
        threads->enqueue(threadName, methodPtr, prime, maxPositiveIntegerForPrime);
    };

    using util_hash::operator""_bkdrHash;
    for (int i = 0; i < Bottom<PrimeMethod>::value; ++i)
    {
        if (!getBit<PrimeMethod>().test(PrimeMethod(i)))
        {
            continue;
        }

        const std::string targetMethod = targets.at(i), threadName = "p_" + targetMethod;
        switch (util_hash::bkdrHash(targetMethod.data()))
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

    app_command::getMemoryForMultithreading().deleteElement(threads);
    NUMERIC_PRINT_TASK_END_TITLE(Type::prime);
}

void updatePrimeTask(const std::string& target)
{
    using util_hash::operator""_bkdrHash;
    switch (util_hash::bkdrHash(target.c_str()))
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
} // namespace app_num