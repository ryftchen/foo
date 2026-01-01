//! @file test_numeric.cpp
//! @author ryftchen
//! @brief The definitions (test_numeric) in the test module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2026 ryftchen. All rights reserved.

#include <gtest/gtest.h>
#include <syncstream>

#include "application/example/include/apply_numeric.hpp"

//! @brief The test module.
namespace test // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Numeric-testing-related functions in the test module.
namespace tst_num
{
//! @brief Print the progress of the numeric task tests.
//! @param title - task title
//! @param state - task state
//! @param align - alignment width
static void printTaskProgress(const std::string_view title, const std::string_view state, const std::uint8_t align = 50)
{
    std::osyncstream(std::cout) << "TEST NUMERIC: " << std::setiosflags(std::ios_base::left) << std::setfill('.')
                                << std::setw(align) << title << state << std::resetiosflags(std::ios_base::left)
                                << std::setfill(' ') << std::endl;
}

//! @brief Test base of arithmetic.
class ArithmeticTestBase : public ::testing::Test
{
private:
    //! @brief Alias for the input builder.
    using InputBuilder = application::app_num::arithmetic::InputBuilder;
    //! @brief Prepare scenario.
    static void prepareScenario()
    {
        namespace input = application::app_num::arithmetic::input;
        fixture = std::make_unique<InputBuilder>(input::operandA, input::operandB);
    }
    //! @brief Reset scenario.
    static void resetScenario() { fixture.reset(); }

protected:
    //! @brief Set up the test case.
    static void SetUpTestSuite()
    {
        printTaskProgress(title, "BEGIN");
        prepareScenario();
    }
    //! @brief Tear down the test case.
    static void TearDownTestSuite()
    {
        printTaskProgress(title, "END");
        resetScenario();
    }

    //! @brief Test title.
    inline static const std::string_view title{numeric::arithmetic::description()};
    //! @brief System under test.
    [[no_unique_address]] const numeric::arithmetic::Arithmetic sut{};
    //! @brief Fixture data.
    inline static std::unique_ptr<InputBuilder> fixture{};
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
TEST_F(ArithmeticTestBase, AdditionMethod)
{
    ASSERT_EQ(expRes1, sut.addition(fixture->getOperands().first, fixture->getOperands().second));
}

//! @brief Test for the subtraction method in the calculation of arithmetic.
TEST_F(ArithmeticTestBase, SubtractionMethod)
{
    ASSERT_EQ(expRes2, sut.subtraction(fixture->getOperands().first, fixture->getOperands().second));
}

//! @brief Test for the multiplication method in the calculation of arithmetic.
TEST_F(ArithmeticTestBase, MultiplicationMethod)
{
    ASSERT_EQ(expRes3, sut.multiplication(fixture->getOperands().first, fixture->getOperands().second));
}

//! @brief Test for the division method in the calculation of arithmetic.
TEST_F(ArithmeticTestBase, DivisionMethod)
{
    ASSERT_EQ(expRes4, sut.division(fixture->getOperands().first, fixture->getOperands().second));
}

//! @brief Test base of divisor.
class DivisorTestBase : public ::testing::Test
{
private:
    //! @brief Alias for the input builder.
    using InputBuilder = application::app_num::divisor::InputBuilder;
    //! @brief Prepare scenario.
    static void prepareScenario()
    {
        namespace input = application::app_num::divisor::input;
        fixture = std::make_unique<InputBuilder>(input::numberA, input::numberB);
    }
    //! @brief Reset scenario.
    static void resetScenario() { fixture.reset(); }

protected:
    //! @brief Set up the test case.
    static void SetUpTestSuite()
    {
        printTaskProgress(title, "BEGIN");
        prepareScenario();
    }
    //! @brief Tear down the test case.
    static void TearDownTestSuite()
    {
        printTaskProgress(title, "END");
        resetScenario();
    }

    //! @brief Test title.
    inline static const std::string_view title{numeric::divisor::description()};
    //! @brief System under test.
    [[no_unique_address]] const numeric::divisor::Divisor sut{};
    //! @brief Fixture data.
    inline static std::unique_ptr<InputBuilder> fixture{};
    //! @brief Expected result.
    const std::set<std::int32_t> expRes{1, 2, 3, 5, 6, 7, 10, 14, 15, 21, 30, 35, 42, 70, 105, 210};
};

//! @brief Test for the Euclidean method in the calculation of divisor.
TEST_F(DivisorTestBase, EuclideanMethod)
{
    ASSERT_EQ(expRes, sut.euclidean(fixture->getNumbers().first, fixture->getNumbers().second));
}

//! @brief Test for the Stein method in the calculation of divisor.
TEST_F(DivisorTestBase, SteinMethod)
{
    ASSERT_EQ(expRes, sut.stein(fixture->getNumbers().first, fixture->getNumbers().second));
}

//! @brief Test base of integral.
class IntegralTestBase : public ::testing::Test
{
private:
    //! @brief Alias for the input builder.
    using InputBuilder = application::app_num::integral::InputBuilder;
    //! @brief Prepare scenario.
    static void prepareScenario()
    {
        using application::app_num::integral::input::CylindricalBessel;
        fixture = std::make_unique<InputBuilder>(
            CylindricalBessel{}, CylindricalBessel::range1, CylindricalBessel::range2, CylindricalBessel::exprDescr);
        sut.clear();
        sut["Trapezoidal"] = std::make_unique<numeric::integral::Trapezoidal>(fixture->getExpression());
        sut["Simpson"] = std::make_unique<numeric::integral::Simpson>(fixture->getExpression());
        sut["Romberg"] = std::make_unique<numeric::integral::Romberg>(fixture->getExpression());
        sut["Gauss"] = std::make_unique<numeric::integral::Gauss>(fixture->getExpression());
        sut["MonteCarlo"] = std::make_unique<numeric::integral::MonteCarlo>(fixture->getExpression());
    }
    //! @brief Reset scenario.
    static void resetScenario()
    {
        fixture.reset();
        sut.clear();
    }

protected:
    //! @brief Set up the test case.
    static void SetUpTestSuite()
    {
        printTaskProgress(title, "BEGIN");
        prepareScenario();
    }
    //! @brief Tear down the test case.
    static void TearDownTestSuite()
    {
        printTaskProgress(title, "END");
        resetScenario();
    }

    //! @brief Test title.
    inline static const std::string_view title{numeric::integral::description()};
    //! @brief System under test.
    inline static std::unordered_map<std::string, std::unique_ptr<numeric::integral::Integral>> sut{};
    //! @brief Fixture data.
    inline static std::unique_ptr<InputBuilder> fixture{};
    //! @brief Expected result.
    static constexpr double expRes{1.05838};
    //! @brief Allowable absolute error.
    static constexpr double absErr{0.1 * ((expRes < 0.0) ? -expRes : expRes)};
    //! @brief Default precision.
    static constexpr double defPrec{numeric::integral::epsilon};
};

//! @brief Test for the trapezoidal method in the calculation of integral.
TEST_F(IntegralTestBase, TrapezoidalMethod)
{
    const auto& trapezoidal = *sut.at("Trapezoidal");
    const auto result = trapezoidal(fixture->getRanges().first, fixture->getRanges().second, defPrec);
    EXPECT_GT(result, expRes - absErr);
    EXPECT_LT(result, expRes + absErr);
}

//! @brief Test for the adaptive Simpson's 1/3 method in the calculation of integral.
TEST_F(IntegralTestBase, AdaptiveSimpsonMethod)
{
    const auto& simpson = *sut.at("Simpson");
    const auto result = simpson(fixture->getRanges().first, fixture->getRanges().second, defPrec);
    EXPECT_GT(result, expRes - absErr);
    EXPECT_LT(result, expRes + absErr);
}

//! @brief Test for the Romberg method in the calculation of integral.
TEST_F(IntegralTestBase, RombergMethod)
{
    const auto& romberg = *sut.at("Romberg");
    const auto result = romberg(fixture->getRanges().first, fixture->getRanges().second, defPrec);
    EXPECT_GT(result, expRes - absErr);
    EXPECT_LT(result, expRes + absErr);
}

//! @brief Test for the Gauss-Legendre's 5-points method in the calculation of integral.
TEST_F(IntegralTestBase, GaussLegendreMethod)
{
    const auto& gauss = *sut.at("Gauss");
    const auto result = gauss(fixture->getRanges().first, fixture->getRanges().second, defPrec);
    EXPECT_GT(result, expRes - absErr);
    EXPECT_LT(result, expRes + absErr);
}

//! @brief Test for the Monte-Carlo method in the calculation of integral.
TEST_F(IntegralTestBase, MonteCarloMethod)
{
    const auto& monteCarlo = *sut.at("MonteCarlo");
    const auto result = monteCarlo(fixture->getRanges().first, fixture->getRanges().second, defPrec);
    EXPECT_GT(result, expRes - absErr);
    EXPECT_LT(result, expRes + absErr);
}

//! @brief Test base of prime.
class PrimeTestBase : public ::testing::Test
{
private:
    //! @brief Alias for the input builder.
    using InputBuilder = application::app_num::prime::InputBuilder;
    //! @brief Prepare scenario.
    static void prepareScenario()
    {
        namespace input = application::app_num::prime::input;
        fixture = std::make_unique<InputBuilder>(input::upperBound);
    }
    //! @brief Reset scenario.
    static void resetScenario() { fixture.reset(); }

protected:
    //! @brief Set up the test case.
    static void SetUpTestSuite()
    {
        printTaskProgress(title, "BEGIN");
        prepareScenario();
    }
    //! @brief Tear down the test case.
    static void TearDownTestSuite()
    {
        printTaskProgress(title, "END");
        resetScenario();
    }

    //! @brief Test title.
    inline static const std::string_view title{numeric::prime::description()};
    //! @brief System under test.
    [[no_unique_address]] const numeric::prime::Prime sut{};
    //! @brief Fixture data.
    inline static std::unique_ptr<InputBuilder> fixture{};
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
TEST_F(PrimeTestBase, EratosthenesMethod)
{
    ASSERT_EQ(expRes(), sut.eratosthenes(fixture->getUpperBound()));
}

//! @brief Test for the Euler method in the calculation of prime.
TEST_F(PrimeTestBase, EulerMethod)
{
    ASSERT_EQ(expRes(), sut.euler(fixture->getUpperBound()));
}
} // namespace tst_num
} // namespace test
