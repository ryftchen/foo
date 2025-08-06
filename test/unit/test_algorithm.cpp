//! @file test_algorithm.cpp
//! @author ryftchen
//! @brief The definitions (test_algorithm) in the test module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include <gtest/gtest.h>
#include <syncstream>

#include "application/example/include/apply_algorithm.hpp"

//! @brief Title of printing for algorithm task tests.
#define TST_ALGO_PRINT_TASK_TITLE(category, state)                                                                   \
    std::osyncstream(std::cout) << "TEST ALGORITHM: " << std::setiosflags(std::ios_base::left) << std::setfill('.')  \
                                << std::setw(50) << (category) << (state) << std::resetiosflags(std::ios_base::left) \
                                << std::setfill(' ') << std::endl;

//! @brief The test module.
namespace test // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Algorithm-testing-related functions in the test module.
namespace tst_algo
{
using namespace application::app_algo; // NOLINT(google-build-using-namespace)

//! @brief Test base of match.
class MatchTestBase : public ::testing::Test
{
protected:
    //! @brief Set up the test case.
    static void SetUpTestSuite()
    {
        TST_ALGO_PRINT_TASK_TITLE("MATCH", "BEGIN");
        fixture = std::make_shared<match::InputBuilder>(match::input::patternString);
    }
    //! @brief Tear down the test case.
    static void TearDownTestSuite()
    {
        TST_ALGO_PRINT_TASK_TITLE("MATCH", "END");
        fixture.reset();
    }
    //! @brief Set up.
    void SetUp() override {}
    //! @brief Tear down.
    void TearDown() override {}

    //! @brief System under test.
    const algorithm::match::Match sut{};
    //! @brief Fixture data.
    static std::shared_ptr<match::InputBuilder> fixture;
    //! @brief Expected result.
    static constexpr std::int64_t expRes{49702};
};
std::shared_ptr<match::InputBuilder> MatchTestBase::fixture = {};

//! @brief Test for the Rabin-Karp method in the solution of match.
TEST_F(MatchTestBase, rkMethod)
{
    ASSERT_EQ(
        expRes,
        sut.rk(
            fixture->getMatchingText().get(),
            fixture->getSinglePattern().get(),
            fixture->getTextLength(),
            fixture->getPatternLength()));
}

//! @brief Test for the Knuth-Morris-Pratt method in the solution of match.
TEST_F(MatchTestBase, kmpMethod)
{
    ASSERT_EQ(
        expRes,
        sut.kmp(
            fixture->getMatchingText().get(),
            fixture->getSinglePattern().get(),
            fixture->getTextLength(),
            fixture->getPatternLength()));
}

//! @brief Test for the Boyer-Moore method in the solution of match.
TEST_F(MatchTestBase, bmMethod)
{
    ASSERT_EQ(
        expRes,
        sut.bm(
            fixture->getMatchingText().get(),
            fixture->getSinglePattern().get(),
            fixture->getTextLength(),
            fixture->getPatternLength()));
}

//! @brief Test for the Horspool method in the solution of match.
TEST_F(MatchTestBase, horspoolMethod)
{
    ASSERT_EQ(
        expRes,
        sut.horspool(
            fixture->getMatchingText().get(),
            fixture->getSinglePattern().get(),
            fixture->getTextLength(),
            fixture->getPatternLength()));
}

//! @brief Test for the Sunday method in the solution of match.
TEST_F(MatchTestBase, sundayMethod)
{
    ASSERT_EQ(
        expRes,
        sut.sunday(
            fixture->getMatchingText().get(),
            fixture->getSinglePattern().get(),
            fixture->getTextLength(),
            fixture->getPatternLength()));
}

//! @brief Test base of notation.
class NotationTestBase : public ::testing::Test
{
protected:
    //! @brief Set up the test case.
    static void SetUpTestSuite()
    {
        TST_ALGO_PRINT_TASK_TITLE("NOTATION", "BEGIN");
        fixture = std::make_shared<notation::InputBuilder>(notation::input::infixString);
    }
    //! @brief Tear down the test case.
    static void TearDownTestSuite()
    {
        TST_ALGO_PRINT_TASK_TITLE("NOTATION", "END");
        fixture.reset();
    }
    //! @brief Set up.
    void SetUp() override {}
    //! @brief Tear down.
    void TearDown() override {}

    //! @brief System under test.
    const algorithm::notation::Notation sut{};
    //! @brief Fixture data.
    static std::shared_ptr<notation::InputBuilder> fixture;
    //! @brief Expected result 1.
    static constexpr std::string_view expRes1{"+a-*b^-^cde+f*ghi"};
    //! @brief Expected result 2.
    static constexpr std::string_view expRes2{"abcd^e-fgh*+^*+i-"};
};
std::shared_ptr<notation::InputBuilder> NotationTestBase::fixture = {};

//! @brief Test for the prefix method in the solution of notation.
TEST_F(NotationTestBase, prefixMethod)
{
    ASSERT_EQ(expRes1, sut.prefix(fixture->getInfixNotation()));
}

//! @brief Test for the postfix method in the solution of notation.
TEST_F(NotationTestBase, postfixMethod)
{
    ASSERT_EQ(expRes2, sut.postfix(fixture->getInfixNotation()));
}

//! @brief Test base of optimal.
class OptimalTestBase : public ::testing::Test
{
protected:
    //! @brief Set up the test case.
    static void SetUpTestSuite()
    {
        TST_ALGO_PRINT_TASK_TITLE("OPTIMAL", "BEGIN");
        using SphericalBessel = optimal::input::SphericalBessel;
        fixture = std::make_shared<optimal::InputBuilder>(
            SphericalBessel{}, SphericalBessel::range1, SphericalBessel::range2, SphericalBessel::funcDescr);
    }
    //! @brief Tear down the test case.
    static void TearDownTestSuite()
    {
        TST_ALGO_PRINT_TASK_TITLE("OPTIMAL", "END");
        fixture.reset();
    }
    //! @brief Set up.
    void SetUp() override {}
    //! @brief Tear down.
    void TearDown() override {}

    //! @brief System under test.
    //! @tparam T - type of system under test
    //! @return system under test
    template <typename T>
    static std::unique_ptr<algorithm::optimal::Optimal> sut()
    {
        return std::make_unique<T>(fixture->getFunction());
    }
    //! @brief Fixture data.
    static std::shared_ptr<optimal::InputBuilder> fixture;
    //! @brief Expected result.
    static constexpr double expRes{-0.21723};
    //! @brief Allowable absolute error.
    static constexpr double absErr{0.1 * ((expRes < 0.0) ? -expRes : expRes)};
};
std::shared_ptr<optimal::InputBuilder> OptimalTestBase::fixture = {};

//! @brief Test for the gradient descent method in the solution of optimal.
TEST_F(OptimalTestBase, gradientDescentMethod)
{
    const auto result = (*sut<algorithm::optimal::Gradient>())(
        fixture->getRanges().first, fixture->getRanges().second, algorithm::optimal::epsilon);
    ASSERT_TRUE(result.has_value());
    EXPECT_GT(std::get<0>(result.value()), expRes - absErr);
    EXPECT_LT(std::get<0>(result.value()), expRes + absErr);
}

//! @brief Test for the tabu method in the solution of optimal.
TEST_F(OptimalTestBase, tabuMethod)
{
    const auto result = (*sut<algorithm::optimal::Tabu>())(
        fixture->getRanges().first, fixture->getRanges().second, algorithm::optimal::epsilon);
    ASSERT_TRUE(result.has_value());
    EXPECT_GT(std::get<0>(result.value()), expRes - absErr);
    EXPECT_LT(std::get<0>(result.value()), expRes + absErr);
}

//! @brief Test for the simulated annealing method in the solution of optimal.
TEST_F(OptimalTestBase, simulatedAnnealingMethod)
{
    const auto result = (*sut<algorithm::optimal::Annealing>())(
        fixture->getRanges().first, fixture->getRanges().second, algorithm::optimal::epsilon);
    ASSERT_TRUE(result.has_value());
    EXPECT_GT(std::get<0>(result.value()), expRes - absErr);
    EXPECT_LT(std::get<0>(result.value()), expRes + absErr);
}

//! @brief Test for the particle swarm method in the solution of optimal.
TEST_F(OptimalTestBase, particleSwarmMethod)
{
    const auto result = (*sut<algorithm::optimal::Particle>())(
        fixture->getRanges().first, fixture->getRanges().second, algorithm::optimal::epsilon);
    ASSERT_TRUE(result.has_value());
    EXPECT_GT(std::get<0>(result.value()), expRes - absErr);
    EXPECT_LT(std::get<0>(result.value()), expRes + absErr);
}

//! @brief Test for the ant colony method in the solution of optimal.
TEST_F(OptimalTestBase, antColonyMethod)
{
    const auto result = (*sut<algorithm::optimal::Ant>())(
        fixture->getRanges().first, fixture->getRanges().second, algorithm::optimal::epsilon);
    ASSERT_TRUE(result.has_value());
    EXPECT_GT(std::get<0>(result.value()), expRes - absErr);
    EXPECT_LT(std::get<0>(result.value()), expRes + absErr);
}

//! @brief Test for the genetic method in the solution of optimal.
TEST_F(OptimalTestBase, geneticMethod)
{
    const auto result = (*sut<algorithm::optimal::Genetic>())(
        fixture->getRanges().first, fixture->getRanges().second, algorithm::optimal::epsilon);
    ASSERT_TRUE(result.has_value());
    EXPECT_GT(std::get<0>(result.value()), expRes - absErr);
    EXPECT_LT(std::get<0>(result.value()), expRes + absErr);
}

//! @brief Test base of search.
class SearchTestBase : public ::testing::Test
{
private:
    //! @brief Update expected result.
    static void updateExpRes()
    {
        const float* const orderedArray = fixture->getOrderedArray().get();
        const std::uint32_t length = fixture->getLength();
        const float searchKey = fixture->getSearchKey();
        for (std::uint32_t i = 0; i < length; ++i)
        {
            if (orderedArray[i] == searchKey)
            {
                expRes.emplace(i);
            }
        }
    }

protected:
    //! @brief Set up the test case.
    static void SetUpTestSuite()
    {
        TST_ALGO_PRINT_TASK_TITLE("SEARCH", "BEGIN");
        fixture = std::make_shared<search::InputBuilder<float>>(
            search::input::arrayLength, search::input::arrayRangeMin, search::input::arrayRangeMax);
        updateExpRes();
    }
    //! @brief Tear down the test case.
    static void TearDownTestSuite()
    {
        TST_ALGO_PRINT_TASK_TITLE("SEARCH", "END");
        fixture.reset();
    }
    //! @brief Set up.
    void SetUp() override {}
    //! @brief Tear down.
    void TearDown() override {}

    //! @brief System under test.
    const algorithm::search::Search<float> sut{};
    //! @brief Fixture data.
    static std::shared_ptr<search::InputBuilder<float>> fixture;
    //! @brief Expected result.
    static std::set<std::int64_t> expRes;
};
std::shared_ptr<search::InputBuilder<float>> SearchTestBase::fixture = {};
std::set<std::int64_t> SearchTestBase::expRes = {};

//! @brief Test for the binary method in the solution of search.
TEST_F(SearchTestBase, binaryMethod)
{
    ASSERT_TRUE(
        expRes.contains(sut.binary(fixture->getOrderedArray().get(), fixture->getLength(), fixture->getSearchKey())));
}

//! @brief Test for the interpolation method in the solution of search.
TEST_F(SearchTestBase, interpolationMethod)
{
    ASSERT_TRUE(expRes.contains(
        sut.interpolation(fixture->getOrderedArray().get(), fixture->getLength(), fixture->getSearchKey())));
}

//! @brief Test for the Fibonacci method in the solution of search.
TEST_F(SearchTestBase, fibonacciMethod)
{
    ASSERT_TRUE(expRes.contains(
        sut.fibonacci(fixture->getOrderedArray().get(), fixture->getLength(), fixture->getSearchKey())));
}

//! @brief Test base of sort.
class SortTestBase : public ::testing::Test
{
private:
    //! @brief Update expected result.
    static void updateExpRes()
    {
        expRes = std::vector<std::int32_t>(
            fixture->getRandomArray().get(), fixture->getRandomArray().get() + fixture->getLength());
        std::sort(expRes.begin(), expRes.end());
    }

protected:
    //! @brief Set up the test case.
    static void SetUpTestSuite()
    {
        TST_ALGO_PRINT_TASK_TITLE("SORT", "BEGIN");
        fixture = std::make_shared<sort::InputBuilder<std::int32_t>>(
            sort::input::arrayLength, sort::input::arrayRangeMin, sort::input::arrayRangeMax);
        updateExpRes();
    }
    //! @brief Tear down the test case.
    static void TearDownTestSuite()
    {
        TST_ALGO_PRINT_TASK_TITLE("SORT", "END");
        fixture.reset();
    }
    //! @brief Set up.
    void SetUp() override {}
    //! @brief Tear down.
    void TearDown() override {}

    //! @brief System under test.
    algorithm::sort::Sort<std::int32_t> sut{};
    //! @brief Fixture data.
    static std::shared_ptr<sort::InputBuilder<std::int32_t>> fixture;
    //! @brief Expected result.
    static std::vector<std::int32_t> expRes;
};
std::shared_ptr<sort::InputBuilder<std::int32_t>> SortTestBase::fixture = {};
std::vector<std::int32_t> SortTestBase::expRes = {};

//! @brief Test for the bubble method in the solution of sort.
TEST_F(SortTestBase, bubbleMethod)
{
    ASSERT_EQ(expRes, sut.bubble(fixture->getRandomArray().get(), fixture->getLength()));
}

//! @brief Test for the selection method in the solution of sort.
TEST_F(SortTestBase, selectionMethod)
{
    ASSERT_EQ(expRes, sut.selection(fixture->getRandomArray().get(), fixture->getLength()));
}

//! @brief Test for the insertion method in the solution of sort.
TEST_F(SortTestBase, insertionMethod)
{
    ASSERT_EQ(expRes, sut.insertion(fixture->getRandomArray().get(), fixture->getLength()));
}

//! @brief Test for the shell method in the solution of sort.
TEST_F(SortTestBase, shellMethod)
{
    ASSERT_EQ(expRes, sut.shell(fixture->getRandomArray().get(), fixture->getLength()));
}

//! @brief Test for the merge method in the solution of sort.
TEST_F(SortTestBase, mergeMethod)
{
    ASSERT_EQ(expRes, sut.merge(fixture->getRandomArray().get(), fixture->getLength()));
}

//! @brief Test for the quick method in the solution of sort.
TEST_F(SortTestBase, quickMethod)
{
    ASSERT_EQ(expRes, sut.quick(fixture->getRandomArray().get(), fixture->getLength()));
}

//! @brief Test for the heap method in the solution of sort.
TEST_F(SortTestBase, heapMethod)
{
    ASSERT_EQ(expRes, sut.heap(fixture->getRandomArray().get(), fixture->getLength()));
}

//! @brief Test for the counting method in the solution of sort.
TEST_F(SortTestBase, countingMethod)
{
    ASSERT_EQ(expRes, sut.counting(fixture->getRandomArray().get(), fixture->getLength()));
}

//! @brief Test for the bucket method in the solution of sort.
TEST_F(SortTestBase, bucketMethod)
{
    ASSERT_EQ(expRes, sut.bucket(fixture->getRandomArray().get(), fixture->getLength()));
}

//! @brief Test for the radix method in the solution of sort.
TEST_F(SortTestBase, radixMethod)
{
    ASSERT_EQ(expRes, sut.radix(fixture->getRandomArray().get(), fixture->getLength()));
}
} // namespace tst_algo
} // namespace test

#undef TST_ALGO_PRINT_TASK_TITLE
