#include <gtest/gtest.h>
#include <cmath>
#include "numeric/include/arithmetic.hpp"
#include "numeric/include/divisor.hpp"
#include "numeric/include/integral.hpp"
#include "numeric/include/prime.hpp"

namespace test::tst_num
{
constexpr uint32_t titleWidthForPrintTest = 50;

class ArithmeticTestBase : public ::testing::Test
{
public:
    ArithmeticTestBase() = default;
    ~ArithmeticTestBase() override = default;

    static void SetUpTestCase()
    {
        std::cout << "TEST NUMERIC: " << std::setiosflags(std::ios_base::left) << std::setfill('.')
                  << std::setw(titleWidthForPrintTest) << "ARITHMETIC"
                  << "BEGIN" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl;
        arithmetic =
            std::make_shared<numeric::arithmetic::ArithmeticSolution>(integerForArithmetic1, integerForArithmetic2);
    };
    static void TearDownTestCase()
    {
        std::cout << "TEST NUMERIC: " << std::setiosflags(std::ios_base::left) << std::setfill('.')
                  << std::setw(titleWidthForPrintTest) << "ARITHMETIC"
                  << "END" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl;
        arithmetic.reset();
    }
    void SetUp() override{};
    void TearDown() override{};

    static std::shared_ptr<numeric::arithmetic::ArithmeticSolution> arithmetic;
    static constexpr int integerForArithmetic1 = 1073741823;
    static constexpr int integerForArithmetic2 = -2;
};

std::shared_ptr<numeric::arithmetic::ArithmeticSolution> ArithmeticTestBase::arithmetic = nullptr;

TEST_F(ArithmeticTestBase, additionMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(
        1073741821,
        arithmetic->additionMethod(std::get<0>(arithmetic->getIntegers()), std::get<1>(arithmetic->getIntegers())));
}

TEST_F(ArithmeticTestBase, subtractionMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(
        1073741825,
        arithmetic->subtractionMethod(std::get<0>(arithmetic->getIntegers()), std::get<1>(arithmetic->getIntegers())));
}

TEST_F(ArithmeticTestBase, multiplicationMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(
        -2147483646,
        arithmetic->multiplicationMethod(
            std::get<0>(arithmetic->getIntegers()), std::get<1>(arithmetic->getIntegers())));
}

TEST_F(ArithmeticTestBase, divisionMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(
        -536870911,
        arithmetic->divisionMethod(std::get<0>(arithmetic->getIntegers()), std::get<1>(arithmetic->getIntegers())));
}

class DivisorTestBase : public ::testing::Test
{
public:
    DivisorTestBase() = default;
    ~DivisorTestBase() override = default;

    static void SetUpTestCase()
    {
        std::cout << "TEST NUMERIC: " << std::setiosflags(std::ios_base::left) << std::setfill('.')
                  << std::setw(titleWidthForPrintTest) << "DIVISOR"
                  << "BEGIN" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl;
        divisor = std::make_shared<numeric::divisor::DivisorSolution>(integerForDivisor1, integerForDivisor2);
    };
    static void TearDownTestCase()
    {
        std::cout << "TEST NUMERIC: " << std::setiosflags(std::ios_base::left) << std::setfill('.')
                  << std::setw(titleWidthForPrintTest) << "DIVISOR"
                  << "END" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl;
        divisor.reset();
    }
    void SetUp() override{};
    void TearDown() override{};

    static std::shared_ptr<numeric::divisor::DivisorSolution> divisor;
    static constexpr int integerForDivisor1 = 2 * 2 * 3 * 3 * 5 * 5 * 7 * 7;
    static constexpr int integerForDivisor2 = 2 * 3 * 5 * 7 * 11 * 13 * 17;
    const std::vector<int> divisorVector{1, 2, 3, 5, 6, 7, 10, 14, 15, 21, 30, 35, 42, 70, 105, 210};
};

std::shared_ptr<numeric::divisor::DivisorSolution> DivisorTestBase::divisor = nullptr;

TEST_F(DivisorTestBase, euclideanMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(
        divisorVector,
        divisor->euclideanMethod(std::get<0>(divisor->getIntegers()), std::get<1>(divisor->getIntegers())));
}

TEST_F(DivisorTestBase, steinMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(
        divisorVector, divisor->steinMethod(std::get<0>(divisor->getIntegers()), std::get<1>(divisor->getIntegers())));
}

class IntegralTestBase : public ::testing::Test
{
public:
    IntegralTestBase() = default;
    ~IntegralTestBase() override = default;

    static void SetUpTestCase()
    {
        std::cout << "TEST NUMERIC: " << std::setiosflags(std::ios_base::left) << std::setfill('.')
                  << std::setw(titleWidthForPrintTest) << "INTEGRAL"
                  << "BEGIN" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl;
    };
    static void TearDownTestCase()
    {
        std::cout << "TEST NUMERIC: " << std::setiosflags(std::ios_base::left) << std::setfill('.')
                  << std::setw(titleWidthForPrintTest) << "INTEGRAL"
                  << "END" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl;
    }
    void SetUp() override{};
    void TearDown() override{};

    class Expression1 : public numeric::integral::expression::Expression
    {
    public:
        double operator()(const double x) const override
        {
            return ((x * std::sin(x)) / (1.0 + std::cos(x) * std::cos(x)));
        }

        static constexpr double range1{-M_PI / 2.0};
        static constexpr double range2{2.0 * M_PI};
        static constexpr std::string_view exprStr{"I=∫(-π/2→2π)x*sin(x)/(1+(cos(x))^2)dx"};
    } expression1;

    class Expression2 : public numeric::integral::expression::Expression
    {
    public:
        double operator()(const double x) const override
        {
            return (x + 10.0 * std::sin(5.0 * x) + 7.0 * std::cos(4.0 * x));
        }

        static constexpr double range1{0.0};
        static constexpr double range2{9.0};
        static constexpr std::string_view exprStr{"I=∫(0→9)x+10sin(5x)+7cos(4x)dx"};
    } expression2;
};

TEST_F(IntegralTestBase, trapezoidal) // NOLINT(cert-err58-cpp)
{
    std::shared_ptr<numeric::integral::IntegralSolution> trapezoidal =
        std::make_shared<numeric::integral::Trapezoidal>(expression1);
#ifndef _NO_PRINT_AT_RUNTIME
    std::cout << "\r\nIntegral expression: " << expression1.exprStr << std::endl;
#endif
    auto result = (*trapezoidal)(expression1.range1, expression1.range2, numeric::integral::epsilon);
    ASSERT_GT(result, -4.08951 - 0.5);
    ASSERT_LT(result, -4.08951 + 0.5);

    trapezoidal = std::make_shared<numeric::integral::Trapezoidal>(expression2);
#ifndef _NO_PRINT_AT_RUNTIME
    std::cout << "\r\nIntegral expression: " << expression2.exprStr << std::endl;
#endif
    result = (*trapezoidal)(expression2.range1, expression2.range2, numeric::integral::epsilon);
    ASSERT_GT(result, +39.71374 - 0.5);
    ASSERT_LT(result, +39.71374 + 0.5);
}

TEST_F(IntegralTestBase, simpson) // NOLINT(cert-err58-cpp)
{
    std::shared_ptr<numeric::integral::IntegralSolution> simpson =
        std::make_shared<numeric::integral::Simpson>(expression1);
#ifndef _NO_PRINT_AT_RUNTIME
    std::cout << "\r\nIntegral expression: " << expression1.exprStr << std::endl;
#endif
    auto result = (*simpson)(expression1.range1, expression1.range2, numeric::integral::epsilon);
    ASSERT_GT(result, -4.08951 - 0.5);
    ASSERT_LT(result, -4.08951 + 0.5);

    simpson = std::make_shared<numeric::integral::Simpson>(expression2);
#ifndef _NO_PRINT_AT_RUNTIME
    std::cout << "\r\nIntegral expression: " << expression2.exprStr << std::endl;
#endif
    result = (*simpson)(expression2.range1, expression2.range2, numeric::integral::epsilon);
    ASSERT_GT(result, +39.71374 - 0.5);
    ASSERT_LT(result, +39.71374 + 0.5);
}

TEST_F(IntegralTestBase, romberg) // NOLINT(cert-err58-cpp)
{
    std::shared_ptr<numeric::integral::IntegralSolution> romberg =
        std::make_shared<numeric::integral::Romberg>(expression1);
#ifndef _NO_PRINT_AT_RUNTIME
    std::cout << "\r\nIntegral expression: " << expression1.exprStr << std::endl;
#endif
    auto result = (*romberg)(expression1.range1, expression1.range2, numeric::integral::epsilon);
    ASSERT_GT(result, -4.08951 - 0.5);
    ASSERT_LT(result, -4.08951 + 0.5);

    romberg = std::make_shared<numeric::integral::Romberg>(expression2);
#ifndef _NO_PRINT_AT_RUNTIME
    std::cout << "\r\nIntegral expression: " << expression2.exprStr << std::endl;
#endif
    result = (*romberg)(expression2.range1, expression2.range2, numeric::integral::epsilon);
    ASSERT_GT(result, +39.71374 - 0.5);
    ASSERT_LT(result, +39.71374 + 0.5);
}

TEST_F(IntegralTestBase, gauss) // NOLINT(cert-err58-cpp)
{
    std::shared_ptr<numeric::integral::IntegralSolution> gauss =
        std::make_shared<numeric::integral::Gauss>(expression1);
#ifndef _NO_PRINT_AT_RUNTIME
    std::cout << "\r\nIntegral expression: " << expression1.exprStr << std::endl;
#endif
    auto result = (*gauss)(expression1.range1, expression1.range2, numeric::integral::epsilon);
    ASSERT_GT(result, -4.08951 - 0.5);
    ASSERT_LT(result, -4.08951 + 0.5);

    gauss = std::make_shared<numeric::integral::Gauss>(expression2);
#ifndef _NO_PRINT_AT_RUNTIME
    std::cout << "\r\nIntegral expression: " << expression2.exprStr << std::endl;
#endif
    result = (*gauss)(expression2.range1, expression2.range2, numeric::integral::epsilon);
    ASSERT_GT(result, +39.71374 - 0.5);
    ASSERT_LT(result, +39.71374 + 0.5);
}

TEST_F(IntegralTestBase, monteCarlo) // NOLINT(cert-err58-cpp)
{
    std::shared_ptr<numeric::integral::IntegralSolution> monteCarlo =
        std::make_shared<numeric::integral::MonteCarlo>(expression1);
#ifndef _NO_PRINT_AT_RUNTIME
    std::cout << "\r\nIntegral expression: " << expression1.exprStr << std::endl;
#endif
    auto result = (*monteCarlo)(expression1.range1, expression1.range2, numeric::integral::epsilon);
    ASSERT_GT(result, -4.08951 - 0.5);
    ASSERT_LT(result, -4.08951 + 0.5);

    monteCarlo = std::make_shared<numeric::integral::MonteCarlo>(expression2);
#ifndef _NO_PRINT_AT_RUNTIME
    std::cout << "\r\nIntegral expression: " << expression2.exprStr << std::endl;
#endif
    result = (*monteCarlo)(expression2.range1, expression2.range2, numeric::integral::epsilon);
    ASSERT_GT(result, +39.71374 - 0.5);
    ASSERT_LT(result, +39.71374 + 0.5);
}

class PrimeTestBase : public ::testing::Test
{
public:
    PrimeTestBase() = default;
    ~PrimeTestBase() override = default;

    static void SetUpTestCase()
    {
        std::cout << "TEST NUMERIC: " << std::setiosflags(std::ios_base::left) << std::setfill('.')
                  << std::setw(titleWidthForPrintTest) << "PRIME"
                  << "BEGIN" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl;
        prime = std::make_shared<numeric::prime::PrimeSolution>(maxPositiveIntegerForPrime);
    };
    static void TearDownTestCase()
    {
        std::cout << "TEST NUMERIC: " << std::setiosflags(std::ios_base::left) << std::setfill('.')
                  << std::setw(titleWidthForPrintTest) << "PRIME"
                  << "END" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl;
        prime.reset();
    }
    void SetUp() override{};
    void TearDown() override{};

    static std::shared_ptr<numeric::prime::PrimeSolution> prime;
    static constexpr uint32_t maxPositiveIntegerForPrime = 997;
    const std::vector<uint32_t> primeVector{
        2,   3,   5,   7,   11,  13,  17,  19,  23,  29,  31,  37,  41,  43,  47,  53,  59,  61,  67,  71,  73,
        79,  83,  89,  97,  101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181,
        191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281, 283, 293, 307,
        311, 313, 317, 331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 419, 421, 431, 433,
        439, 443, 449, 457, 461, 463, 467, 479, 487, 491, 499, 503, 509, 521, 523, 541, 547, 557, 563, 569, 571,
        577, 587, 593, 599, 601, 607, 613, 617, 619, 631, 641, 643, 647, 653, 659, 661, 673, 677, 683, 691, 701,
        709, 719, 727, 733, 739, 743, 751, 757, 761, 769, 773, 787, 797, 809, 811, 821, 823, 827, 829, 839, 853,
        857, 859, 863, 877, 881, 883, 887, 907, 911, 919, 929, 937, 941, 947, 953, 967, 971, 977, 983, 991, 997};
};

std::shared_ptr<numeric::prime::PrimeSolution> PrimeTestBase::prime = nullptr;

TEST_F(PrimeTestBase, eratosthenesMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(primeVector, prime->eratosthenesMethod(prime->getMaxPositiveInteger()));
}

TEST_F(PrimeTestBase, eulerMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(primeVector, prime->eulerMethod(prime->getMaxPositiveInteger()));
}
} // namespace test::tst_num