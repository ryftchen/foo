//! @file test_algorithm.cpp
//! @author ryftchen
//! @brief The definitions (test_algorithm) in the test module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023

#include <gtest/gtest.h>
#include "algorithm/include/match.hpp"
#include "algorithm/include/notation.hpp"
#include "algorithm/include/optimal.hpp"
#include "algorithm/include/search.hpp"
#include "algorithm/include/sort.hpp"
#include "application/example/include/apply_algorithm.hpp"

//! @brief Title of printing for algorithm task tests.
#define TST_ALGO_PRINT_TASK_TITLE(taskType, taskState)                                                             \
    std::cout << "TEST ALGORITHM: " << std::setiosflags(std::ios_base::left) << std::setfill('.') << std::setw(50) \
              << taskType << taskState << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl;

//! @brief Algorithm-testing-related functions in the test module.
namespace test::tst_algo
{
//! @brief Test base of match.
class MatchTestBase : public ::testing::Test
{
public:
    //! @brief Construct a new MatchTestBase object
    MatchTestBase() = default;
    //! @brief Destroy the MatchTestBase object.
    ~MatchTestBase() override = default;

    //! @brief Set up the test case.
    static void SetUpTestCase()
    {
        TST_ALGO_PRINT_TASK_TITLE("MATCH", "BEGIN");
        builder = std::make_shared<application::app_algo::match::TargetBuilder>(
            application::app_algo::input::singlePatternForMatch);
    };
    //! @brief Tear down the test case.
    static void TearDownTestCase()
    {
        TST_ALGO_PRINT_TASK_TITLE("MATCH", "END");
        builder.reset();
    }
    //! @brief Set up.
    void SetUp() override{};
    //! @brief Tear down.
    void TearDown() override{};

    //! @brief Target builder.
    static std::shared_ptr<application::app_algo::match::TargetBuilder> builder;
};

std::shared_ptr<application::app_algo::match::TargetBuilder> MatchTestBase::builder = nullptr;

//! @brief Test for the Rabin-Karp method in the solution of match.
TEST_F(MatchTestBase, rkMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(
        49702,
        algorithm::match::Match::rk(
            builder->getMatchingText().get(),
            builder->getSinglePattern().data(),
            std::string_view(builder->getMatchingText().get()).length(),
            builder->getSinglePattern().length()));
}

//! @brief Test for the Knuth-Morris-Pratt method in the solution of match.
TEST_F(MatchTestBase, kmpMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(
        49702,
        algorithm::match::Match::kmp(
            builder->getMatchingText().get(),
            builder->getSinglePattern().data(),
            std::string_view(builder->getMatchingText().get()).length(),
            builder->getSinglePattern().length()));
}

//! @brief Test for the Boyer-Moore method in the solution of match.
TEST_F(MatchTestBase, bmMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(
        49702,
        algorithm::match::Match::bm(
            builder->getMatchingText().get(),
            builder->getSinglePattern().data(),
            std::string_view(builder->getMatchingText().get()).length(),
            builder->getSinglePattern().length()));
}

//! @brief Test for the Horspool method in the solution of match.
TEST_F(MatchTestBase, horspoolMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(
        49702,
        algorithm::match::Match::horspool(
            builder->getMatchingText().get(),
            builder->getSinglePattern().data(),
            std::string_view(builder->getMatchingText().get()).length(),
            builder->getSinglePattern().length()));
}

//! @brief Test for the Sunday method in the solution of match.
TEST_F(MatchTestBase, sundayMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(
        49702,
        algorithm::match::Match::sunday(
            builder->getMatchingText().get(),
            builder->getSinglePattern().data(),
            std::string_view(builder->getMatchingText().get()).length(),
            builder->getSinglePattern().length()));
}

//! @brief Test base of notation.
class NotationTestBase : public ::testing::Test
{
public:
    //! @brief Construct a new NotationTestBase object.
    NotationTestBase() = default;
    //! @brief Destroy the NotationTestBase object.
    ~NotationTestBase() override = default;

    //! @brief Set up the test case.
    static void SetUpTestCase()
    {
        TST_ALGO_PRINT_TASK_TITLE("NOTATION", "BEGIN");
        builder = std::make_shared<application::app_algo::notation::TargetBuilder>(
            application::app_algo::input::infixForNotation);
    };
    //! @brief Tear down the test case.
    static void TearDownTestCase()
    {
        TST_ALGO_PRINT_TASK_TITLE("NOTATION", "END");
        builder.reset();
    }
    //! @brief Set up.
    void SetUp() override{};
    //! @brief Tear down.
    void TearDown() override{};

    //! @brief Target builder.
    static std::shared_ptr<application::app_algo::notation::TargetBuilder> builder;
};

std::shared_ptr<application::app_algo::notation::TargetBuilder> NotationTestBase::builder = nullptr;

//! @brief Test for the prefix method in the solution of notation.
TEST_F(NotationTestBase, prefixMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ("+a-*b^-^cde+f*ghi", algorithm::notation::Notation::prefix(std::string{builder->getInfixNotation()}));
}

//! @brief Test for the postfix method in the solution of notation.
TEST_F(NotationTestBase, postfixMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ("abcd^e-fgh*+^*+i-", algorithm::notation::Notation::postfix(std::string{builder->getInfixNotation()}));
}

//! @brief Test base of optimal.
class OptimalTestBase : public ::testing::Test
{
public:
    //! @brief Construct a new OptimalTestBase object.
    OptimalTestBase() = default;
    //! @brief Destroy the OptimalTestBase object.
    ~OptimalTestBase() override = default;

    //! @brief Set up the test case.
    static void SetUpTestCase() { TST_ALGO_PRINT_TASK_TITLE("OPTIMAL", "BEGIN"); };
    //! @brief Tear down the test case.
    static void TearDownTestCase() { TST_ALGO_PRINT_TASK_TITLE("OPTIMAL", "END"); }
    //! @brief Set up.
    void SetUp() override{};
    //! @brief Tear down.
    void TearDown() override{};

    //! @brief Rastrigin function object.
    static constexpr application::app_algo::input::Rastrigin rastrigin{};
    //! @brief Allowable error.
    static constexpr double error{1e-4};
};

//! @brief Test for the gradient descent method in the solution of optimal.
TEST_F(OptimalTestBase, gradientDescentMethod) // NOLINT(cert-err58-cpp)
{
    const std::shared_ptr<algorithm::optimal::Optimal> gradient =
        std::make_shared<algorithm::optimal::Gradient>(rastrigin);
    std::optional<std::tuple<double, double>> result = std::nullopt;

    // NOLINTNEXTLINE(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
    ASSERT_NO_THROW(result = (*gradient)(rastrigin.range1, rastrigin.range2, algorithm::optimal::epsilon));
    ASSERT_TRUE(result.has_value());
    EXPECT_GT(get<0>(result.value()), 0.0 - error);
    EXPECT_LT(get<0>(result.value()), 0.0 + error);
}

//! @brief Test for the simulated annealing method in the solution of optimal.
TEST_F(OptimalTestBase, simulatedAnnealingMethod) // NOLINT(cert-err58-cpp)
{
    const std::shared_ptr<algorithm::optimal::Optimal> annealing =
        std::make_shared<algorithm::optimal::Annealing>(rastrigin);
    std::optional<std::tuple<double, double>> result = std::nullopt;

    // NOLINTNEXTLINE(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
    ASSERT_NO_THROW(result = (*annealing)(rastrigin.range1, rastrigin.range2, algorithm::optimal::epsilon));
    ASSERT_TRUE(result.has_value());
    EXPECT_GT(get<0>(result.value()), 0.0 - error);
    EXPECT_LT(get<0>(result.value()), 0.0 + error);
}

//! @brief Test for the particle swarm method in the solution of optimal.
TEST_F(OptimalTestBase, particleSwarmMethod) // NOLINT(cert-err58-cpp)
{
    const std::shared_ptr<algorithm::optimal::Optimal> particle =
        std::make_shared<algorithm::optimal::Particle>(rastrigin);
    std::optional<std::tuple<double, double>> result = std::nullopt;

    // NOLINTNEXTLINE(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
    ASSERT_NO_THROW(result = (*particle)(rastrigin.range1, rastrigin.range2, algorithm::optimal::epsilon));
    ASSERT_TRUE(result.has_value());
    EXPECT_GT(get<0>(result.value()), 0.0 - error);
    EXPECT_LT(get<0>(result.value()), 0.0 + error);
}

//! @brief Test for the genetic method in the solution of optimal.
TEST_F(OptimalTestBase, geneticMethod) // NOLINT(cert-err58-cpp)
{
    const std::shared_ptr<algorithm::optimal::Optimal> genetic =
        std::make_shared<algorithm::optimal::Genetic>(rastrigin);
    std::optional<std::tuple<double, double>> result = std::nullopt;

    // NOLINTNEXTLINE(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
    ASSERT_NO_THROW(result = (*genetic)(rastrigin.range1, rastrigin.range2, algorithm::optimal::epsilon));
    ASSERT_TRUE(result.has_value());
    EXPECT_GT(get<0>(result.value()), 0.0 - error);
    EXPECT_LT(get<0>(result.value()), 0.0 + error);
}

//! @brief Test base of search.
class SearchTestBase : public ::testing::Test
{
public:
    //! @brief Construct a new SearchTestBase object.
    SearchTestBase() = default;
    //! @brief Destroy the SearchTestBase object.
    ~SearchTestBase() override = default;

    //! @brief Set up the test case.
    static void SetUpTestCase()
    {
        TST_ALGO_PRINT_TASK_TITLE("SEARCH", "BEGIN");
        builder = std::make_shared<application::app_algo::search::TargetBuilder<double>>(
            application::app_algo::input::arrayLengthForSearch,
            application::app_algo::input::arrayRangeForSearch1,
            application::app_algo::input::arrayRangeForSearch2);
    };
    //! @brief Tear down the test case.
    static void TearDownTestCase()
    {
        TST_ALGO_PRINT_TASK_TITLE("SEARCH", "END");
        builder.reset();
    }
    //! @brief Set up.
    void SetUp() override{};
    //! @brief Tear down.
    void TearDown() override{};

    //! @brief Target builder.
    static std::shared_ptr<application::app_algo::search::TargetBuilder<double>> builder;
};

std::shared_ptr<application::app_algo::search::TargetBuilder<double>> SearchTestBase::builder = nullptr;

//! @brief Test for the binary method in the solution of search.
TEST_F(SearchTestBase, binaryMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(
        builder->getLength() / 2,
        algorithm::search::Search<double>::binary(
            builder->getOrderedArray().get(), builder->getLength(), builder->getSearchKey()));
}

//! @brief Test for the interpolation method in the solution of search.
TEST_F(SearchTestBase, interpolationMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(
        builder->getLength() / 2,
        algorithm::search::Search<double>::interpolation(
            builder->getOrderedArray().get(), builder->getLength(), builder->getSearchKey()));
}

//! @brief Test for the Fibonacci method in the solution of search.
TEST_F(SearchTestBase, fibonacciMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(
        builder->getLength() / 2,
        algorithm::search::Search<double>::fibonacci(
            builder->getOrderedArray().get(), builder->getLength(), builder->getSearchKey()));
}

//! @brief Test base of sort.
class SortTestBase : public ::testing::Test
{
public:
    //! @brief Construct a new SortTestBase object.
    SortTestBase() = default;
    //! @brief Destroy the SortTestBase object.
    ~SortTestBase() override = default;

    //! @brief Set up the test case.
    static void SetUpTestCase()
    {
        TST_ALGO_PRINT_TASK_TITLE("SORT", "BEGIN");
        builder = std::make_shared<application::app_algo::sort::TargetBuilder<int>>(
            application::app_algo::input::arrayLengthForSort,
            application::app_algo::input::arrayRangeForSort1,
            application::app_algo::input::arrayRangeForSort2);
    };
    //! @brief Tear down the test case.
    static void TearDownTestCase()
    {
        TST_ALGO_PRINT_TASK_TITLE("SORT", "END");
        builder.reset();
    }
    //! @brief Set up.
    void SetUp() override{};
    //! @brief Tear down.
    void TearDown() override{};

    //! @brief Target builder.
    static std::shared_ptr<application::app_algo::sort::TargetBuilder<int>> builder;
};

std::shared_ptr<application::app_algo::sort::TargetBuilder<int>> SortTestBase::builder = nullptr;

//! @brief Test for the bubble method in the solution of sort.
TEST_F(SortTestBase, bubbleMethod) // NOLINT(cert-err58-cpp)
{
    std::vector<int> sortVector(
        builder->getRandomArray().get(), builder->getRandomArray().get() + builder->getLength());
    std::sort(sortVector.begin(), sortVector.end());
    ASSERT_EQ(sortVector, algorithm::sort::Sort<int>::bubble(builder->getRandomArray().get(), builder->getLength()));
}

//! @brief Test for the selection method in the solution of sort.
TEST_F(SortTestBase, selectionMethod) // NOLINT(cert-err58-cpp)
{
    std::vector<int> sortVector(
        builder->getRandomArray().get(), builder->getRandomArray().get() + builder->getLength());
    std::sort(sortVector.begin(), sortVector.end());
    ASSERT_EQ(sortVector, algorithm::sort::Sort<int>::selection(builder->getRandomArray().get(), builder->getLength()));
}

//! @brief Test for the insertion method in the solution of sort.
TEST_F(SortTestBase, insertionMethod) // NOLINT(cert-err58-cpp)
{
    std::vector<int> sortVector(
        builder->getRandomArray().get(), builder->getRandomArray().get() + builder->getLength());
    std::sort(sortVector.begin(), sortVector.end());
    ASSERT_EQ(sortVector, algorithm::sort::Sort<int>::insertion(builder->getRandomArray().get(), builder->getLength()));
}

//! @brief Test for the shell method in the solution of sort.
TEST_F(SortTestBase, shellMethod) // NOLINT(cert-err58-cpp)
{
    std::vector<int> sortVector(
        builder->getRandomArray().get(), builder->getRandomArray().get() + builder->getLength());
    std::sort(sortVector.begin(), sortVector.end());
    ASSERT_EQ(sortVector, algorithm::sort::Sort<int>::shell(builder->getRandomArray().get(), builder->getLength()));
}

//! @brief Test for the merge method in the solution of sort.
TEST_F(SortTestBase, mergeMethod) // NOLINT(cert-err58-cpp)
{
    std::vector<int> sortVector(
        builder->getRandomArray().get(), builder->getRandomArray().get() + builder->getLength());
    std::sort(sortVector.begin(), sortVector.end());
    ASSERT_EQ(sortVector, algorithm::sort::Sort<int>::merge(builder->getRandomArray().get(), builder->getLength()));
}

//! @brief Test for the quick method in the solution of sort.
TEST_F(SortTestBase, quickMethod) // NOLINT(cert-err58-cpp)
{
    std::vector<int> sortVector(
        builder->getRandomArray().get(), builder->getRandomArray().get() + builder->getLength());
    std::sort(sortVector.begin(), sortVector.end());
    ASSERT_EQ(sortVector, algorithm::sort::Sort<int>::quick(builder->getRandomArray().get(), builder->getLength()));
}

//! @brief Test for the heap method in the solution of sort.
TEST_F(SortTestBase, heapMethod) // NOLINT(cert-err58-cpp)
{
    std::vector<int> sortVector(
        builder->getRandomArray().get(), builder->getRandomArray().get() + builder->getLength());
    std::sort(sortVector.begin(), sortVector.end());
    ASSERT_EQ(sortVector, algorithm::sort::Sort<int>::heap(builder->getRandomArray().get(), builder->getLength()));
}

//! @brief Test for the counting method in the solution of sort.
TEST_F(SortTestBase, countingMethod) // NOLINT(cert-err58-cpp)
{
    std::vector<int> sortVector(
        builder->getRandomArray().get(), builder->getRandomArray().get() + builder->getLength());
    std::sort(sortVector.begin(), sortVector.end());
    ASSERT_EQ(sortVector, algorithm::sort::Sort<int>::counting(builder->getRandomArray().get(), builder->getLength()));
}

//! @brief Test for the bucket method in the solution of sort.
TEST_F(SortTestBase, bucketMethod) // NOLINT(cert-err58-cpp)
{
    std::vector<int> sortVector(
        builder->getRandomArray().get(), builder->getRandomArray().get() + builder->getLength());
    std::sort(sortVector.begin(), sortVector.end());
    ASSERT_EQ(sortVector, algorithm::sort::Sort<int>::bucket(builder->getRandomArray().get(), builder->getLength()));
}

//! @brief Test for the radix method in the solution of sort.
TEST_F(SortTestBase, radixMethod) // NOLINT(cert-err58-cpp)
{
    std::vector<int> sortVector(
        builder->getRandomArray().get(), builder->getRandomArray().get() + builder->getLength());
    std::sort(sortVector.begin(), sortVector.end());
    ASSERT_EQ(sortVector, algorithm::sort::Sort<int>::radix(builder->getRandomArray().get(), builder->getLength()));
}
} // namespace test::tst_algo
