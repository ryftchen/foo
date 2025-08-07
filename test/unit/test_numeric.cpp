//! @file test_numeric.cpp
//! @author ryftchen
//! @brief The definitions (test_numeric) in the test module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include <gtest/gtest.h>
#include <syncstream>

#include "application/example/include/apply_numeric.hpp"

//! @brief Title of printing for numeric task tests.
#define TST_NUM_PRINT_TASK_TITLE(category, state)                                                                    \
    std::osyncstream(std::cout) << "TEST NUMERIC: " << std::setiosflags(std::ios_base::left) << std::setfill('.')    \
                                << std::setw(50) << (category) << (state) << std::resetiosflags(std::ios_base::left) \
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
protected:
    //! @brief Set up the test case.
    static void SetUpTestSuite() { TST_NUM_PRINT_TASK_TITLE("ARITHMETIC", "BEGIN"); }
    //! @brief Tear down the test case.
    static void TearDownTestSuite() { TST_NUM_PRINT_TASK_TITLE("ARITHMETIC", "END"); }

    //! @brief System under test.
    const numeric::arithmetic::Arithmetic sut{};
    //! @brief Fixture data.
    const arithmetic::InputBuilder fixture{arithmetic::input::operandA, arithmetic::input::operandB};
    //! @brief Expected result 1.
    static constexpr std::int32_t expRes1{0};
    //! @brief Expected result 2.
    static constexpr std::int32_t expRes2{92680};
    //! @brief Expected result 3.
    static constexpr std::int32_t expRes3{-2147395600};
    //! @brief Expected result 4.
    static constexpr std::int32_t expRes4{-1};
};

//! @brief Test for the addition method in the calculation of arithmetic.
TEST_F(ArithmeticTestBase, additionMethod)
{
    ASSERT_EQ(expRes1, sut.addition(fixture.getOperands().first, fixture.getOperands().second));
}

//! @brief Test for the subtraction method in the calculation of arithmetic.
TEST_F(ArithmeticTestBase, subtractionMethod)
{
    ASSERT_EQ(expRes2, sut.subtraction(fixture.getOperands().first, fixture.getOperands().second));
}

//! @brief Test for the multiplication method in the calculation of arithmetic.
TEST_F(ArithmeticTestBase, multiplicationMethod)
{
    ASSERT_EQ(expRes3, sut.multiplication(fixture.getOperands().first, fixture.getOperands().second));
}

//! @brief Test for the division method in the calculation of arithmetic.
TEST_F(ArithmeticTestBase, divisionMethod)
{
    ASSERT_EQ(expRes4, sut.division(fixture.getOperands().first, fixture.getOperands().second));
}

//! @brief Test base of divisor.
class DivisorTestBase : public ::testing::Test
{
protected:
    //! @brief Set up the test case.
    static void SetUpTestSuite() { TST_NUM_PRINT_TASK_TITLE("DIVISOR", "BEGIN"); }
    //! @brief Tear down the test case.
    static void TearDownTestSuite() { TST_NUM_PRINT_TASK_TITLE("DIVISOR", "END"); }

    //! @brief System under test.
    const numeric::divisor::Divisor sut{};
    //! @brief Fixture data.
    const divisor::InputBuilder fixture{divisor::input::numberA, divisor::input::numberB};
    //! @brief Expected result.
    const std::set<std::int32_t> expRes{1, 2, 3, 5, 6, 7, 10, 14, 15, 21, 30, 35, 42, 70, 105, 210};
};

//! @brief Test for the Euclidean method in the calculation of divisor.
TEST_F(DivisorTestBase, euclideanMethod)
{
    ASSERT_EQ(expRes, sut.euclidean(fixture.getNumbers().first, fixture.getNumbers().second));
}

//! @brief Test for the Stein method in the calculation of divisor.
TEST_F(DivisorTestBase, steinMethod)
{
    ASSERT_EQ(expRes, sut.stein(fixture.getNumbers().first, fixture.getNumbers().second));
}

//! @brief Test base of integral.
class IntegralTestBase : public ::testing::Test
{
protected:
    //! @brief Set up the test case.
    static void SetUpTestSuite() { TST_NUM_PRINT_TASK_TITLE("INTEGRAL", "BEGIN"); }
    //! @brief Tear down the test case.
    static void TearDownTestSuite() { TST_NUM_PRINT_TASK_TITLE("INTEGRAL", "END"); }

    //! @brief Alias for the integral expression.
    using CylindricalBessel = integral::input::CylindricalBessel;
    //! @brief System under test.
    //! @tparam T - type of system under test
    //! @return system under test
    template <typename T>
    [[nodiscard]] std::unique_ptr<numeric::integral::Integral> sut() const
    {
        return std::make_unique<T>(fixture.getExpression());
    }
    //! @brief Fixture data.
    const integral::InputBuilder fixture{
        CylindricalBessel{}, CylindricalBessel::range1, CylindricalBessel::range2, CylindricalBessel::exprDescr};
    //! @brief Expected result.
    static constexpr double expRes{1.05838};
    //! @brief Allowable absolute error.
    static constexpr double absErr{0.1 * ((expRes < 0.0) ? -expRes : expRes)};
    //! @brief Precision of integral.
    static constexpr double prec{numeric::integral::epsilon};
};

//! @brief Test for the trapezoidal method in the calculation of integral.
TEST_F(IntegralTestBase, trapezoidalMethod)
{
    const auto result =
        (*sut<numeric::integral::Trapezoidal>())(fixture.getRanges().first, fixture.getRanges().second, prec);
    EXPECT_GT(result, expRes - absErr);
    EXPECT_LT(result, expRes + absErr);
}

//! @brief Test for the adaptive Simpson's 1/3 method in the calculation of integral.
TEST_F(IntegralTestBase, adaptiveSimpsonMethod)
{
    const auto result =
        (*sut<numeric::integral::Simpson>())(fixture.getRanges().first, fixture.getRanges().second, prec);
    EXPECT_GT(result, expRes - absErr);
    EXPECT_LT(result, expRes + absErr);
}

//! @brief Test for the Romberg method in the calculation of integral.
TEST_F(IntegralTestBase, rombergMethod)
{
    const auto result =
        (*sut<numeric::integral::Romberg>())(fixture.getRanges().first, fixture.getRanges().second, prec);
    EXPECT_GT(result, expRes - absErr);
    EXPECT_LT(result, expRes + absErr);
}

//! @brief Test for the Gauss-Legendre's 5-points method in the calculation of integral.
TEST_F(IntegralTestBase, gaussLegendreMethod)
{
    const auto result = (*sut<numeric::integral::Gauss>())(fixture.getRanges().first, fixture.getRanges().second, prec);
    EXPECT_GT(result, expRes - absErr);
    EXPECT_LT(result, expRes + absErr);
}

//! @brief Test for the Monte-Carlo method in the calculation of integral.
TEST_F(IntegralTestBase, monteCarloMethod)
{
    const auto result =
        (*sut<numeric::integral::MonteCarlo>())(fixture.getRanges().first, fixture.getRanges().second, prec);
    EXPECT_GT(result, expRes - absErr);
    EXPECT_LT(result, expRes + absErr);
}

//! @brief Test base of prime.
class PrimeTestBase : public ::testing::Test
{
protected:
    //! @brief Set up the test case.
    static void SetUpTestSuite() { TST_NUM_PRINT_TASK_TITLE("PRIME", "BEGIN"); }
    //! @brief Tear down the test case.
    static void TearDownTestSuite() { TST_NUM_PRINT_TASK_TITLE("PRIME", "END"); }

    //! @brief System under test.
    const numeric::prime::Prime sut{};
    //! @brief Fixture data.
    const prime::InputBuilder fixture{prime::input::upperBound};
    //! @brief Expected result.
    //! @return expected result
    static constexpr auto expRes()
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

//! @brief Test for the Eratosthenes method in the calculation of prime.
TEST_F(PrimeTestBase, eratosthenesMethod)
{
    ASSERT_EQ(expRes(), sut.eratosthenes(fixture.getUpperBound()));
}

//! @brief Test for the Euler method in the calculation of prime.
TEST_F(PrimeTestBase, eulerMethod)
{
    ASSERT_EQ(expRes(), sut.euler(fixture.getUpperBound()));
}
} // namespace tst_num
} // namespace test

#undef TST_NUM_PRINT_TASK_TITLE
