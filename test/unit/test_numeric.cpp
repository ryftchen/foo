//! @file test_numeric.cpp
//! @author ryftchen
//! @brief The definitions (test_numeric) in the test module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023
#include <gtest/gtest.h>
#include <cmath>
#include "numeric/include/arithmetic.hpp"
#include "numeric/include/divisor.hpp"
#include "numeric/include/integral.hpp"
#include "numeric/include/prime.hpp"

//! @brief Title of printing for numeric task tests.
#define TST_NUM_PRINT_TASK_TITLE(taskType, taskState)                                                            \
    std::cout << "TEST NUMERIC: " << std::setiosflags(std::ios_base::left) << std::setfill('.') << std::setw(50) \
              << taskType << taskState << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl;

//! @brief Numeric-testing-related functions in the test module.
namespace test::tst_num
{
//! @brief Test base of arithmetic.
class ArithmeticTestBase : public ::testing::Test
{
public:
    //! @brief Construct a new ArithmeticTestBase object.
    ArithmeticTestBase() = default;
    //! @brief Destroy the ArithmeticTestBase object.
    ~ArithmeticTestBase() override = default;

    //! @brief Set up the test case.
    static void SetUpTestCase()
    {
        TST_NUM_PRINT_TASK_TITLE("ARITHMETIC", "BEGIN");
        builder = std::make_shared<numeric::arithmetic::TargetBuilder>(integerForArithmetic1, integerForArithmetic2);
    };
    //! @brief Tear down the test case.
    static void TearDownTestCase()
    {
        TST_NUM_PRINT_TASK_TITLE("ARITHMETIC", "END");
        builder.reset();
    }
    //! @brief Set up.
    void SetUp() override{};
    //! @brief Tear down.
    void TearDown() override{};

    //! @brief Target builder.
    static std::shared_ptr<numeric::arithmetic::TargetBuilder> builder;
    //! @brief One of integers for arithmetic methods.
    static constexpr int integerForArithmetic1{1073741823};
    //! @brief One of integers for arithmetic methods.
    static constexpr int integerForArithmetic2{-2};
};

std::shared_ptr<numeric::arithmetic::TargetBuilder> ArithmeticTestBase::builder = nullptr;

//! @brief Test for the addition method in the solution of arithmetic.
TEST_F(ArithmeticTestBase, additionMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(
        1073741821,
        numeric::arithmetic::ArithmeticSolution::additionMethod(
            std::get<0>(builder->getIntegers()), std::get<1>(builder->getIntegers())));
}

//! @brief Test for the subtraction method in the solution of arithmetic.
TEST_F(ArithmeticTestBase, subtractionMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(
        1073741825,
        numeric::arithmetic::ArithmeticSolution::subtractionMethod(
            std::get<0>(builder->getIntegers()), std::get<1>(builder->getIntegers())));
}

//! @brief Test for the multiplication method in the solution of arithmetic.
TEST_F(ArithmeticTestBase, multiplicationMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(
        -2147483646,
        numeric::arithmetic::ArithmeticSolution::multiplicationMethod(
            std::get<0>(builder->getIntegers()), std::get<1>(builder->getIntegers())));
}

//! @brief Test for the division method in the solution of arithmetic.
TEST_F(ArithmeticTestBase, divisionMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(
        -536870911,
        numeric::arithmetic::ArithmeticSolution::divisionMethod(
            std::get<0>(builder->getIntegers()), std::get<1>(builder->getIntegers())));
}

//! @brief Test base of divisor.
class DivisorTestBase : public ::testing::Test
{
public:
    //! @brief Construct a new DivisorTestBase object.
    DivisorTestBase() = default;
    //! @brief Destroy the DivisorTestBase object.
    ~DivisorTestBase() override = default;

    //! @brief Set up the test case.
    static void SetUpTestCase()
    {
        TST_NUM_PRINT_TASK_TITLE("DIVISOR", "BEGIN");
        builder = std::make_shared<numeric::divisor::TargetBuilder>(integerForDivisor1, integerForDivisor2);
    };
    //! @brief Tear down the test case.
    static void TearDownTestCase()
    {
        TST_NUM_PRINT_TASK_TITLE("DIVISOR", "END");
        builder.reset();
    }
    //! @brief Set up.
    void SetUp() override{};
    //! @brief Tear down.
    void TearDown() override{};

    //! @brief Target builder.
    static std::shared_ptr<numeric::divisor::TargetBuilder> builder;
    //! @brief One of integers for divisor methods.
    static constexpr int integerForDivisor1{2 * 2 * 3 * 3 * 5 * 5 * 7 * 7};
    //! @brief One of integers for divisor methods.
    static constexpr int integerForDivisor2{2 * 3 * 5 * 7 * 11 * 13 * 17};
    //! @brief Expected result.
    const std::vector<int> divisorVector{1, 2, 3, 5, 6, 7, 10, 14, 15, 21, 30, 35, 42, 70, 105, 210};
};

std::shared_ptr<numeric::divisor::TargetBuilder> DivisorTestBase::builder = nullptr;

//! @brief Test for the Euclidean method in the solution of divisor.
TEST_F(DivisorTestBase, euclideanMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(
        divisorVector,
        numeric::divisor::DivisorSolution::euclideanMethod(
            std::get<0>(builder->getIntegers()), std::get<1>(builder->getIntegers())));
}

//! @brief Test for the Stein method in the solution of divisor.
TEST_F(DivisorTestBase, steinMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(
        divisorVector,
        numeric::divisor::DivisorSolution::steinMethod(
            std::get<0>(builder->getIntegers()), std::get<1>(builder->getIntegers())));
}

//! @brief Test base of integral.
class IntegralTestBase : public ::testing::Test
{
public:
    //! @brief Construct a new IntegralTestBase object.
    IntegralTestBase() = default;
    //! @brief Destroy the IntegralTestBase object.
    ~IntegralTestBase() override = default;

    //! @brief Set up the test case.
    static void SetUpTestCase() { TST_NUM_PRINT_TASK_TITLE("INTEGRAL", "BEGIN"); };
    //! @brief Tear down the test case.
    static void TearDownTestCase() { TST_NUM_PRINT_TASK_TITLE("INTEGRAL", "END"); }
    //! @brief Set up.
    void SetUp() override{};
    //! @brief Tear down.
    void TearDown() override{};

    //! @brief Expression example 1.
    class Expression1 : public numeric::integral::expression::Expression
    {
    public:
        //! @brief The operator (()) overloading of Expression1 class.
        //! @param x - independent variable
        //! @return dependent variable
        double operator()(const double x) const override
        {
            return ((x * std::sin(x)) / (1.0 + std::cos(x) * std::cos(x)));
        }

        //! @brief Left endpoint.
        static constexpr double range1{-M_PI / 2.0};
        //! @brief Right endpoint.
        static constexpr double range2{2.0 * M_PI};
        //! @brief Expression example 1.
        static constexpr std::string_view exprDescr{"I=∫(-π/2→2π)x*sin(x)/(1+(cos(x))^2)dx"};
    } /** Expression example 1 object. */ expression1;

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
    } /** Expression example 2 object. */ expression2;
};

//! @brief Test for the trapezoidal method in the solution of integral.
TEST_F(IntegralTestBase, trapezoidal) // NOLINT(cert-err58-cpp)
{
    std::shared_ptr<numeric::integral::IntegralSolution> trapezoidal =
        std::make_shared<numeric::integral::Trapezoidal>(expression1);
#ifndef __RUNTIME_NO_PRINTING__
    std::cout << "\r\nIntegral expression: " << expression1.exprDescr << std::endl;
#endif
    auto result = (*trapezoidal)(expression1.range1, expression1.range2, numeric::integral::epsilon);
    ASSERT_GT(result, -4.08951 - 0.5);
    ASSERT_LT(result, -4.08951 + 0.5);

    trapezoidal = std::make_shared<numeric::integral::Trapezoidal>(expression2);
#ifndef __RUNTIME_NO_PRINTING__
    std::cout << "\r\nIntegral expression: " << expression2.exprDescr << std::endl;
#endif
    result = (*trapezoidal)(expression2.range1, expression2.range2, numeric::integral::epsilon);
    ASSERT_GT(result, +39.71374 - 0.5);
    ASSERT_LT(result, +39.71374 + 0.5);
}

//! @brief Test for the adaptive Simpson's 1/3 method in the solution of integral.
TEST_F(IntegralTestBase, simpson) // NOLINT(cert-err58-cpp)
{
    std::shared_ptr<numeric::integral::IntegralSolution> simpson =
        std::make_shared<numeric::integral::Simpson>(expression1);
#ifndef __RUNTIME_NO_PRINTING__
    std::cout << "\r\nIntegral expression: " << expression1.exprDescr << std::endl;
#endif
    auto result = (*simpson)(expression1.range1, expression1.range2, numeric::integral::epsilon);
    ASSERT_GT(result, -4.08951 - 0.5);
    ASSERT_LT(result, -4.08951 + 0.5);

    simpson = std::make_shared<numeric::integral::Simpson>(expression2);
#ifndef __RUNTIME_NO_PRINTING__
    std::cout << "\r\nIntegral expression: " << expression2.exprDescr << std::endl;
#endif
    result = (*simpson)(expression2.range1, expression2.range2, numeric::integral::epsilon);
    ASSERT_GT(result, +39.71374 - 0.5);
    ASSERT_LT(result, +39.71374 + 0.5);
}

//! @brief Test for the Romberg method in the solution of integral.
TEST_F(IntegralTestBase, romberg) // NOLINT(cert-err58-cpp)
{
    std::shared_ptr<numeric::integral::IntegralSolution> romberg =
        std::make_shared<numeric::integral::Romberg>(expression1);
#ifndef __RUNTIME_NO_PRINTING__
    std::cout << "\r\nIntegral expression: " << expression1.exprDescr << std::endl;
#endif
    auto result = (*romberg)(expression1.range1, expression1.range2, numeric::integral::epsilon);
    ASSERT_GT(result, -4.08951 - 0.5);
    ASSERT_LT(result, -4.08951 + 0.5);

    romberg = std::make_shared<numeric::integral::Romberg>(expression2);
#ifndef __RUNTIME_NO_PRINTING__
    std::cout << "\r\nIntegral expression: " << expression2.exprDescr << std::endl;
#endif
    result = (*romberg)(expression2.range1, expression2.range2, numeric::integral::epsilon);
    ASSERT_GT(result, +39.71374 - 0.5);
    ASSERT_LT(result, +39.71374 + 0.5);
}

//! @brief Test for the Gauss-Legendre's 5-points method in the solution of integral.
TEST_F(IntegralTestBase, gauss) // NOLINT(cert-err58-cpp)
{
    std::shared_ptr<numeric::integral::IntegralSolution> gauss =
        std::make_shared<numeric::integral::Gauss>(expression1);
#ifndef __RUNTIME_NO_PRINTING__
    std::cout << "\r\nIntegral expression: " << expression1.exprDescr << std::endl;
#endif
    auto result = (*gauss)(expression1.range1, expression1.range2, numeric::integral::epsilon);
    ASSERT_GT(result, -4.08951 - 0.5);
    ASSERT_LT(result, -4.08951 + 0.5);

    gauss = std::make_shared<numeric::integral::Gauss>(expression2);
#ifndef __RUNTIME_NO_PRINTING__
    std::cout << "\r\nIntegral expression: " << expression2.exprDescr << std::endl;
#endif
    result = (*gauss)(expression2.range1, expression2.range2, numeric::integral::epsilon);
    ASSERT_GT(result, +39.71374 - 0.5);
    ASSERT_LT(result, +39.71374 + 0.5);
}

//! @brief Test for the Monte-Carlo method in the solution of integral.
TEST_F(IntegralTestBase, monteCarlo) // NOLINT(cert-err58-cpp)
{
    std::shared_ptr<numeric::integral::IntegralSolution> monteCarlo =
        std::make_shared<numeric::integral::MonteCarlo>(expression1);
#ifndef __RUNTIME_NO_PRINTING__
    std::cout << "\r\nIntegral expression: " << expression1.exprDescr << std::endl;
#endif
    auto result = (*monteCarlo)(expression1.range1, expression1.range2, numeric::integral::epsilon);
    ASSERT_GT(result, -4.08951 - 0.5);
    ASSERT_LT(result, -4.08951 + 0.5);

    monteCarlo = std::make_shared<numeric::integral::MonteCarlo>(expression2);
#ifndef __RUNTIME_NO_PRINTING__
    std::cout << "\r\nIntegral expression: " << expression2.exprDescr << std::endl;
#endif
    result = (*monteCarlo)(expression2.range1, expression2.range2, numeric::integral::epsilon);
    ASSERT_GT(result, +39.71374 - 0.5);
    ASSERT_LT(result, +39.71374 + 0.5);
}

//! @brief Test base of prime.
class PrimeTestBase : public ::testing::Test
{
public:
    //! @brief Construct a new PrimeTestBase object.
    PrimeTestBase() = default;
    //! @brief Destroy the PrimeTestBase object.
    ~PrimeTestBase() override = default;

    //! @brief Set up the test case.
    static void SetUpTestCase()
    {
        TST_NUM_PRINT_TASK_TITLE("PRIME", "BEGIN");
        builder = std::make_shared<numeric::prime::TargetBuilder>(maxPositiveIntegerForPrime);
    };
    //! @brief Tear down the test case.
    static void TearDownTestCase()
    {
        TST_NUM_PRINT_TASK_TITLE("PRIME", "END");
        builder.reset();
    }
    //! @brief Set up.
    void SetUp() override{};
    //! @brief Tear down.
    void TearDown() override{};

    //! @brief Target builder.
    static std::shared_ptr<numeric::prime::TargetBuilder> builder;
    //! @brief Max positive integer for prime methods.
    static constexpr uint32_t maxPositiveIntegerForPrime{997};
    //! @brief Expected result.
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

std::shared_ptr<numeric::prime::TargetBuilder> PrimeTestBase::builder = nullptr;

//! @brief Test for the Eratosthenes method in the solution of prime.
TEST_F(PrimeTestBase, eratosthenesMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(primeVector, numeric::prime::PrimeSolution::eratosthenesMethod(builder->getMaxPositiveInteger()));
}

//! @brief Test for the Euler method in the solution of prime.
TEST_F(PrimeTestBase, eulerMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(primeVector, numeric::prime::PrimeSolution::eulerMethod(builder->getMaxPositiveInteger()));
}
} // namespace test::tst_num