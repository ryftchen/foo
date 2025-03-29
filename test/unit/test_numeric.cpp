//! @file test_numeric.cpp
//! @author ryftchen
//! @brief The definitions (test_numeric) in the test module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include <gtest/gtest.h>
#include <syncstream>

#include "application/example/include/apply_numeric.hpp"

//! @brief Title of printing for numeric task tests.
#define TST_NUM_PRINT_TASK_TITLE(category, state)                                                                 \
    std::osyncstream(std::cout) << "TEST NUMERIC: " << std::setiosflags(std::ios_base::left) << std::setfill('.') \
                                << std::setw(50) << category << state << std::resetiosflags(std::ios_base::left)  \
                                << std::setfill(' ') << std::endl;

//! @brief The test module.
namespace test // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Numeric-testing-related functions in the test module.
namespace tst_num
{
using namespace application::app_num; // NOLINT(google-build-using-namespace)

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
        inputs = std::make_shared<arithmetic::InputBuilder>(arithmetic::input::integerA, arithmetic::input::integerB);
    }
    //! @brief Tear down the test case.
    static void TearDownTestCase()
    {
        TST_NUM_PRINT_TASK_TITLE("ARITHMETIC", "END");
        inputs.reset();
    }
    //! @brief Set up.
    void SetUp() override {}
    //! @brief Tear down.
    void TearDown() override {}

    //! @brief Test suite.
    const numeric::arithmetic::Arithmetic sut{};
    //! @brief Input builder.
    static std::shared_ptr<arithmetic::InputBuilder> inputs;
};
std::shared_ptr<arithmetic::InputBuilder> ArithmeticTestBase::inputs = nullptr;

//! @brief Test for the addition method in the solution of arithmetic.
TEST_F(ArithmeticTestBase, additionMethod)
{
    ASSERT_EQ(0, sut.addition(inputs->getIntegers().first, inputs->getIntegers().second));
}

//! @brief Test for the subtraction method in the solution of arithmetic.
TEST_F(ArithmeticTestBase, subtractionMethod)
{
    ASSERT_EQ(92680, sut.subtraction(inputs->getIntegers().first, inputs->getIntegers().second));
}

//! @brief Test for the multiplication method in the solution of arithmetic.
TEST_F(ArithmeticTestBase, multiplicationMethod)
{
    ASSERT_EQ(-2147395600, sut.multiplication(inputs->getIntegers().first, inputs->getIntegers().second));
}

//! @brief Test for the division method in the solution of arithmetic.
TEST_F(ArithmeticTestBase, divisionMethod)
{
    ASSERT_EQ(-1, sut.division(inputs->getIntegers().first, inputs->getIntegers().second));
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
        inputs = std::make_shared<divisor::InputBuilder>(divisor::input::integerA, divisor::input::integerB);
    }
    //! @brief Tear down the test case.
    static void TearDownTestCase()
    {
        TST_NUM_PRINT_TASK_TITLE("DIVISOR", "END");
        inputs.reset();
    }
    //! @brief Set up.
    void SetUp() override {}
    //! @brief Tear down.
    void TearDown() override {}

    //! @brief Test suite.
    const numeric::divisor::Divisor sut{};
    //! @brief Input builder.
    static std::shared_ptr<divisor::InputBuilder> inputs;
    //! @brief Expected result.
    const std::set<std::int32_t> expColl{1, 2, 3, 5, 6, 7, 10, 14, 15, 21, 30, 35, 42, 70, 105, 210};
};
std::shared_ptr<divisor::InputBuilder> DivisorTestBase::inputs = nullptr;

//! @brief Test for the Euclidean method in the solution of divisor.
TEST_F(DivisorTestBase, euclideanMethod)
{
    ASSERT_EQ(expColl, sut.euclidean(inputs->getIntegers().first, inputs->getIntegers().second));
}

//! @brief Test for the Stein method in the solution of divisor.
TEST_F(DivisorTestBase, steinMethod)
{
    ASSERT_EQ(expColl, sut.stein(inputs->getIntegers().first, inputs->getIntegers().second));
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
    static void SetUpTestCase()
    {
        TST_NUM_PRINT_TASK_TITLE("INTEGRAL", "BEGIN");
        using Griewank = integral::input::Griewank;
        inputs = std::make_shared<integral::InputBuilder>(
            Griewank{}, Griewank::range1, Griewank::range2, Griewank::exprDescr);
    }
    //! @brief Tear down the test case.
    static void TearDownTestCase()
    {
        TST_NUM_PRINT_TASK_TITLE("INTEGRAL", "END");
        inputs.reset();
    }
    //! @brief Set up.
    void SetUp() override {}
    //! @brief Tear down.
    void TearDown() override {}

    //! @brief Input builder.
    static std::shared_ptr<integral::InputBuilder> inputs;
    //! @brief Allowable error.
    static constexpr double error{1e3};
};
std::shared_ptr<integral::InputBuilder> IntegralTestBase::inputs = nullptr;

//! @brief Test for the trapezoidal method in the solution of integral.
TEST_F(IntegralTestBase, trapezoidalMethod)
{
    const std::shared_ptr<numeric::integral::Integral> trapezoidal =
        std::make_shared<numeric::integral::Trapezoidal>(inputs->getExpression());
    double result = 0.0;

    ASSERT_NO_THROW(
        result = (*trapezoidal)(inputs->getRanges().first, inputs->getRanges().second, numeric::integral::epsilon));
    EXPECT_GT(result, +37199.91164 - error);
    EXPECT_LT(result, +37199.91164 + error);
}

//! @brief Test for the adaptive Simpson's 1/3 method in the solution of integral.
TEST_F(IntegralTestBase, adaptiveSimpsonMethod)
{
    const std::shared_ptr<numeric::integral::Integral> simpson =
        std::make_shared<numeric::integral::Simpson>(inputs->getExpression());
    double result = 0.0;

    ASSERT_NO_THROW(
        result = (*simpson)(inputs->getRanges().first, inputs->getRanges().second, numeric::integral::epsilon));
    EXPECT_GT(result, +37199.91164 - error);
    EXPECT_LT(result, +37199.91164 + error);
}

//! @brief Test for the Romberg method in the solution of integral.
TEST_F(IntegralTestBase, rombergMethod)
{
    const std::shared_ptr<numeric::integral::Integral> romberg =
        std::make_shared<numeric::integral::Romberg>(inputs->getExpression());
    double result = 0.0;

    ASSERT_NO_THROW(
        result = (*romberg)(inputs->getRanges().first, inputs->getRanges().second, numeric::integral::epsilon));
    EXPECT_GT(result, +37199.91164 - error);
    EXPECT_LT(result, +37199.91164 + error);
}

//! @brief Test for the Gauss-Legendre's 5-points method in the solution of integral.
TEST_F(IntegralTestBase, gaussLegendreMethod)
{
    const std::shared_ptr<numeric::integral::Integral> gauss =
        std::make_shared<numeric::integral::Gauss>(inputs->getExpression());
    double result = 0.0;

    ASSERT_NO_THROW(
        result = (*gauss)(inputs->getRanges().first, inputs->getRanges().second, numeric::integral::epsilon));
    EXPECT_GT(result, +37199.91164 - error);
    EXPECT_LT(result, +37199.91164 + error);
}

//! @brief Test for the Monte-Carlo method in the solution of integral.
TEST_F(IntegralTestBase, monteCarloMethod)
{
    const std::shared_ptr<numeric::integral::Integral> monteCarlo =
        std::make_shared<numeric::integral::MonteCarlo>(inputs->getExpression());
    double result = 0.0;

    ASSERT_NO_THROW(
        result = (*monteCarlo)(inputs->getRanges().first, inputs->getRanges().second, numeric::integral::epsilon));
    EXPECT_GT(result, +37199.91164 - error);
    EXPECT_LT(result, +37199.91164 + error);
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
        inputs = std::make_shared<prime::InputBuilder>(prime::input::maxPositiveInteger);
    }
    //! @brief Tear down the test case.
    static void TearDownTestCase()
    {
        TST_NUM_PRINT_TASK_TITLE("PRIME", "END");
        inputs.reset();
    }
    //! @brief Set up.
    void SetUp() override {}
    //! @brief Tear down.
    void TearDown() override {}

    //! @brief Test suite.
    const numeric::prime::Prime sut{};
    //! @brief Input builder.
    static std::shared_ptr<prime::InputBuilder> inputs;
    //! @brief Expected result.
    static constexpr auto expColl()
    {
        // NOLINTBEGIN(readability-magic-numbers)
        return std::vector<std::uint32_t>{
            2,   3,   5,   7,   11,  13,  17,  19,  23,  29,  31,  37,  41,  43,  47,  53,  59,  61,  67,  71,  73,
            79,  83,  89,  97,  101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181,
            191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281, 283, 293, 307,
            311, 313, 317, 331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 419, 421, 431, 433,
            439, 443, 449, 457, 461, 463, 467, 479, 487, 491, 499, 503, 509, 521, 523, 541, 547, 557, 563, 569, 571,
            577, 587, 593, 599, 601, 607, 613, 617, 619, 631, 641, 643, 647, 653, 659, 661, 673, 677, 683, 691, 701,
            709, 719, 727, 733, 739, 743, 751, 757, 761, 769, 773, 787, 797, 809, 811, 821, 823, 827, 829, 839, 853,
            857, 859, 863, 877, 881, 883, 887, 907, 911, 919, 929, 937, 941, 947, 953, 967, 971, 977, 983, 991, 997};
        // NOLINTEND(readability-magic-numbers)
    }
};
std::shared_ptr<prime::InputBuilder> PrimeTestBase::inputs = nullptr;

//! @brief Test for the Eratosthenes method in the solution of prime.
TEST_F(PrimeTestBase, eratosthenesMethod)
{
    ASSERT_EQ(expColl(), sut.eratosthenes(inputs->getMaxPositiveInteger()));
}

//! @brief Test for the Euler method in the solution of prime.
TEST_F(PrimeTestBase, eulerMethod)
{
    ASSERT_EQ(expColl(), sut.euler(inputs->getMaxPositiveInteger()));
}
} // namespace tst_num
} // namespace test
