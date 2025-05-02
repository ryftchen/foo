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
public:
    //! @brief Construct a new MatchTestBase object.
    MatchTestBase() = default;
    //! @brief Destroy the MatchTestBase object.
    ~MatchTestBase() override = default;

protected:
    //! @brief Set up the test case.
    static void SetUpTestSuite()
    {
        TST_ALGO_PRINT_TASK_TITLE("MATCH", "BEGIN");
        inputs = std::make_shared<match::InputBuilder>(match::input::patternString);
    }
    //! @brief Tear down the test case.
    static void TearDownTestSuite()
    {
        TST_ALGO_PRINT_TASK_TITLE("MATCH", "END");
        inputs.reset();
    }
    //! @brief Set up.
    void SetUp() override {}
    //! @brief Tear down.
    void TearDown() override {}

    //! @brief Test suite.
    const algorithm::match::Match sut{};
    //! @brief Input builder.
    static std::shared_ptr<match::InputBuilder> inputs;
};
std::shared_ptr<match::InputBuilder> MatchTestBase::inputs = {};

//! @brief Test for the Rabin-Karp method in the solution of match.
TEST_F(MatchTestBase, rkMethod)
{
    ASSERT_EQ(
        49702,
        sut.rk(
            inputs->getMatchingText().get(),
            inputs->getSinglePattern().get(),
            inputs->getTextLength(),
            inputs->getPatternLength()));
}

//! @brief Test for the Knuth-Morris-Pratt method in the solution of match.
TEST_F(MatchTestBase, kmpMethod)
{
    ASSERT_EQ(
        49702,
        sut.kmp(
            inputs->getMatchingText().get(),
            inputs->getSinglePattern().get(),
            inputs->getTextLength(),
            inputs->getPatternLength()));
}

//! @brief Test for the Boyer-Moore method in the solution of match.
TEST_F(MatchTestBase, bmMethod)
{
    ASSERT_EQ(
        49702,
        sut.bm(
            inputs->getMatchingText().get(),
            inputs->getSinglePattern().get(),
            inputs->getTextLength(),
            inputs->getPatternLength()));
}

//! @brief Test for the Horspool method in the solution of match.
TEST_F(MatchTestBase, horspoolMethod)
{
    ASSERT_EQ(
        49702,
        sut.horspool(
            inputs->getMatchingText().get(),
            inputs->getSinglePattern().get(),
            inputs->getTextLength(),
            inputs->getPatternLength()));
}

//! @brief Test for the Sunday method in the solution of match.
TEST_F(MatchTestBase, sundayMethod)
{
    ASSERT_EQ(
        49702,
        sut.sunday(
            inputs->getMatchingText().get(),
            inputs->getSinglePattern().get(),
            inputs->getTextLength(),
            inputs->getPatternLength()));
}

//! @brief Test base of notation.
class NotationTestBase : public ::testing::Test
{
public:
    //! @brief Construct a new NotationTestBase object.
    NotationTestBase() = default;
    //! @brief Destroy the NotationTestBase object.
    ~NotationTestBase() override = default;

protected:
    //! @brief Set up the test case.
    static void SetUpTestSuite()
    {
        TST_ALGO_PRINT_TASK_TITLE("NOTATION", "BEGIN");
        inputs = std::make_shared<notation::InputBuilder>(notation::input::infixString);
    }
    //! @brief Tear down the test case.
    static void TearDownTestSuite()
    {
        TST_ALGO_PRINT_TASK_TITLE("NOTATION", "END");
        inputs.reset();
    }
    //! @brief Set up.
    void SetUp() override {}
    //! @brief Tear down.
    void TearDown() override {}

    //! @brief Test suite.
    const algorithm::notation::Notation sut{};
    //! @brief Input builder.
    static std::shared_ptr<notation::InputBuilder> inputs;
};
std::shared_ptr<notation::InputBuilder> NotationTestBase::inputs = {};

//! @brief Test for the prefix method in the solution of notation.
TEST_F(NotationTestBase, prefixMethod)
{
    ASSERT_EQ("+a-*b^-^cde+f*ghi", sut.prefix(inputs->getInfixNotation()));
}

//! @brief Test for the postfix method in the solution of notation.
TEST_F(NotationTestBase, postfixMethod)
{
    ASSERT_EQ("abcd^e-fgh*+^*+i-", sut.postfix(inputs->getInfixNotation()));
}

//! @brief Test base of optimal.
class OptimalTestBase : public ::testing::Test
{
public:
    //! @brief Construct a new OptimalTestBase object.
    OptimalTestBase() = default;
    //! @brief Destroy the OptimalTestBase object.
    ~OptimalTestBase() override = default;

protected:
    //! @brief Set up the test case.
    static void SetUpTestSuite()
    {
        TST_ALGO_PRINT_TASK_TITLE("OPTIMAL", "BEGIN");
        using Rastrigin = optimal::input::Rastrigin;
        inputs = std::make_shared<optimal::InputBuilder>(
            Rastrigin{}, Rastrigin::range1, Rastrigin::range2, Rastrigin::funcDescr);
    }
    //! @brief Tear down the test case.
    static void TearDownTestSuite()
    {
        TST_ALGO_PRINT_TASK_TITLE("OPTIMAL", "END");
        inputs.reset();
    }
    //! @brief Set up.
    void SetUp() override {}
    //! @brief Tear down.
    void TearDown() override {}

    //! @brief Input builder.
    static std::shared_ptr<optimal::InputBuilder> inputs;
    //! @brief Allowable error.
    static constexpr double error{1e-3};
};
std::shared_ptr<optimal::InputBuilder> OptimalTestBase::inputs = {};

//! @brief Test for the gradient descent method in the solution of optimal.
TEST_F(OptimalTestBase, gradientDescentMethod)
{
    const std::shared_ptr<algorithm::optimal::Optimal> gradient =
        std::make_shared<algorithm::optimal::Gradient>(inputs->getFunction());
    std::optional<std::tuple<double, double>> result = std::nullopt;

    ASSERT_NO_THROW(
        result = (*gradient)(inputs->getRanges().first, inputs->getRanges().second, algorithm::optimal::epsilon));
    ASSERT_TRUE(result.has_value());
    EXPECT_GT(std::get<0>(result.value()), 0.0 - error);
    EXPECT_LT(std::get<0>(result.value()), 0.0 + error);
}

//! @brief Test for the tabu method in the solution of optimal.
TEST_F(OptimalTestBase, tabuMethod)
{
    const std::shared_ptr<algorithm::optimal::Optimal> tabu =
        std::make_shared<algorithm::optimal::Tabu>(inputs->getFunction());
    std::optional<std::tuple<double, double>> result = std::nullopt;

    ASSERT_NO_THROW(
        result = (*tabu)(inputs->getRanges().first, inputs->getRanges().second, algorithm::optimal::epsilon));
    ASSERT_TRUE(result.has_value());
    EXPECT_GT(std::get<0>(result.value()), 0.0 - error);
    EXPECT_LT(std::get<0>(result.value()), 0.0 + error);
}

//! @brief Test for the simulated annealing method in the solution of optimal.
TEST_F(OptimalTestBase, simulatedAnnealingMethod)
{
    const std::shared_ptr<algorithm::optimal::Optimal> annealing =
        std::make_shared<algorithm::optimal::Annealing>(inputs->getFunction());
    std::optional<std::tuple<double, double>> result = std::nullopt;

    ASSERT_NO_THROW(
        result = (*annealing)(inputs->getRanges().first, inputs->getRanges().second, algorithm::optimal::epsilon));
    ASSERT_TRUE(result.has_value());
    EXPECT_GT(std::get<0>(result.value()), 0.0 - error);
    EXPECT_LT(std::get<0>(result.value()), 0.0 + error);
}

//! @brief Test for the particle swarm method in the solution of optimal.
TEST_F(OptimalTestBase, particleSwarmMethod)
{
    const std::shared_ptr<algorithm::optimal::Optimal> particle =
        std::make_shared<algorithm::optimal::Particle>(inputs->getFunction());
    std::optional<std::tuple<double, double>> result = std::nullopt;

    ASSERT_NO_THROW(
        result = (*particle)(inputs->getRanges().first, inputs->getRanges().second, algorithm::optimal::epsilon));
    ASSERT_TRUE(result.has_value());
    EXPECT_GT(std::get<0>(result.value()), 0.0 - error);
    EXPECT_LT(std::get<0>(result.value()), 0.0 + error);
}

//! @brief Test for the ant colony method in the solution of optimal.
TEST_F(OptimalTestBase, antColonyMethod)
{
    const std::shared_ptr<algorithm::optimal::Optimal> ant =
        std::make_shared<algorithm::optimal::Ant>(inputs->getFunction());
    std::optional<std::tuple<double, double>> result = std::nullopt;

    ASSERT_NO_THROW(
        result = (*ant)(inputs->getRanges().first, inputs->getRanges().second, algorithm::optimal::epsilon));
    ASSERT_TRUE(result.has_value());
    EXPECT_GT(std::get<0>(result.value()), 0.0 - error);
    EXPECT_LT(std::get<0>(result.value()), 0.0 + error);
}

//! @brief Test for the genetic method in the solution of optimal.
TEST_F(OptimalTestBase, geneticMethod)
{
    const std::shared_ptr<algorithm::optimal::Optimal> genetic =
        std::make_shared<algorithm::optimal::Genetic>(inputs->getFunction());
    std::optional<std::tuple<double, double>> result = std::nullopt;

    ASSERT_NO_THROW(
        result = (*genetic)(inputs->getRanges().first, inputs->getRanges().second, algorithm::optimal::epsilon));
    ASSERT_TRUE(result.has_value());
    EXPECT_GT(std::get<0>(result.value()), 0.0 - error);
    EXPECT_LT(std::get<0>(result.value()), 0.0 + error);
}

//! @brief Test base of search.
class SearchTestBase : public ::testing::Test
{
public:
    //! @brief Construct a new SearchTestBase object.
    SearchTestBase() = default;
    //! @brief Destroy the SearchTestBase object.
    ~SearchTestBase() override = default;

private:
    //! @brief Update expected result.
    static void updateExpectation()
    {
        if (!inputs)
        {
            return;
        }
        const auto* const orderedArray = inputs->getOrderedArray().get();
        const auto length = inputs->getLength();
        const auto searchKey = inputs->getSearchKey();
        for (std::uint32_t i = 0; i < length; ++i)
        {
            if (searchKey == orderedArray[i])
            {
                expColl.emplace(i);
            }
        }
    }

protected:
    //! @brief Set up the test case.
    static void SetUpTestSuite()
    {
        TST_ALGO_PRINT_TASK_TITLE("SEARCH", "BEGIN");
        inputs = std::make_shared<search::InputBuilder<float>>(
            search::input::arrayLength, search::input::arrayRangeMin, search::input::arrayRangeMax);
        updateExpectation();
    }
    //! @brief Tear down the test case.
    static void TearDownTestSuite()
    {
        TST_ALGO_PRINT_TASK_TITLE("SEARCH", "END");
        inputs.reset();
    }
    //! @brief Set up.
    void SetUp() override {}
    //! @brief Tear down.
    void TearDown() override {}

    //! @brief Test suite.
    const algorithm::search::Search<float> sut{};
    //! @brief Input builder.
    static std::shared_ptr<search::InputBuilder<float>> inputs;
    //! @brief Expected result.
    static std::set<std::int64_t> expColl;
};
std::shared_ptr<search::InputBuilder<float>> SearchTestBase::inputs = {};
std::set<std::int64_t> SearchTestBase::expColl = {};

//! @brief Test for the binary method in the solution of search.
TEST_F(SearchTestBase, binaryMethod)
{
    ASSERT_TRUE(
        expColl.contains(sut.binary(inputs->getOrderedArray().get(), inputs->getLength(), inputs->getSearchKey())));
}

//! @brief Test for the interpolation method in the solution of search.
TEST_F(SearchTestBase, interpolationMethod)
{
    ASSERT_TRUE(expColl.contains(
        sut.interpolation(inputs->getOrderedArray().get(), inputs->getLength(), inputs->getSearchKey())));
}

//! @brief Test for the Fibonacci method in the solution of search.
TEST_F(SearchTestBase, fibonacciMethod)
{
    ASSERT_TRUE(
        expColl.contains(sut.fibonacci(inputs->getOrderedArray().get(), inputs->getLength(), inputs->getSearchKey())));
}

//! @brief Test base of sort.
class SortTestBase : public ::testing::Test
{
public:
    //! @brief Construct a new SortTestBase object.
    SortTestBase() = default;
    //! @brief Destroy the SortTestBase object.
    ~SortTestBase() override = default;

private:
    //! @brief Update expected result.
    static void updateExpectation()
    {
        if (!inputs)
        {
            return;
        }
        expColl = std::vector<std::int32_t>(
            inputs->getRandomArray().get(), inputs->getRandomArray().get() + inputs->getLength());
        std::sort(expColl.begin(), expColl.end());
    }

protected:
    //! @brief Set up the test case.
    static void SetUpTestSuite()
    {
        TST_ALGO_PRINT_TASK_TITLE("SORT", "BEGIN");
        inputs = std::make_shared<sort::InputBuilder<std::int32_t>>(
            sort::input::arrayLength, sort::input::arrayRangeMin, sort::input::arrayRangeMax);
        updateExpectation();
    }
    //! @brief Tear down the test case.
    static void TearDownTestSuite()
    {
        TST_ALGO_PRINT_TASK_TITLE("SORT", "END");
        inputs.reset();
    }
    //! @brief Set up.
    void SetUp() override {}
    //! @brief Tear down.
    void TearDown() override {}

    //! @brief Test suite.
    algorithm::sort::Sort<std::int32_t> sut{};
    //! @brief Input builder.
    static std::shared_ptr<sort::InputBuilder<std::int32_t>> inputs;
    //! @brief Expected result.
    static std::vector<std::int32_t> expColl;
};
std::shared_ptr<sort::InputBuilder<std::int32_t>> SortTestBase::inputs = {};
std::vector<std::int32_t> SortTestBase::expColl = {};

//! @brief Test for the bubble method in the solution of sort.
TEST_F(SortTestBase, bubbleMethod)
{
    ASSERT_EQ(expColl, sut.bubble(inputs->getRandomArray().get(), inputs->getLength()));
}

//! @brief Test for the selection method in the solution of sort.
TEST_F(SortTestBase, selectionMethod)
{
    ASSERT_EQ(expColl, sut.selection(inputs->getRandomArray().get(), inputs->getLength()));
}

//! @brief Test for the insertion method in the solution of sort.
TEST_F(SortTestBase, insertionMethod)
{
    ASSERT_EQ(expColl, sut.insertion(inputs->getRandomArray().get(), inputs->getLength()));
}

//! @brief Test for the shell method in the solution of sort.
TEST_F(SortTestBase, shellMethod)
{
    ASSERT_EQ(expColl, sut.shell(inputs->getRandomArray().get(), inputs->getLength()));
}

//! @brief Test for the merge method in the solution of sort.
TEST_F(SortTestBase, mergeMethod)
{
    ASSERT_EQ(expColl, sut.merge(inputs->getRandomArray().get(), inputs->getLength()));
}

//! @brief Test for the quick method in the solution of sort.
TEST_F(SortTestBase, quickMethod)
{
    ASSERT_EQ(expColl, sut.quick(inputs->getRandomArray().get(), inputs->getLength()));
}

//! @brief Test for the heap method in the solution of sort.
TEST_F(SortTestBase, heapMethod)
{
    ASSERT_EQ(expColl, sut.heap(inputs->getRandomArray().get(), inputs->getLength()));
}

//! @brief Test for the counting method in the solution of sort.
TEST_F(SortTestBase, countingMethod)
{
    ASSERT_EQ(expColl, sut.counting(inputs->getRandomArray().get(), inputs->getLength()));
}

//! @brief Test for the bucket method in the solution of sort.
TEST_F(SortTestBase, bucketMethod)
{
    ASSERT_EQ(expColl, sut.bucket(inputs->getRandomArray().get(), inputs->getLength()));
}

//! @brief Test for the radix method in the solution of sort.
TEST_F(SortTestBase, radixMethod)
{
    ASSERT_EQ(expColl, sut.radix(inputs->getRandomArray().get(), inputs->getLength()));
}
} // namespace tst_algo
} // namespace test
