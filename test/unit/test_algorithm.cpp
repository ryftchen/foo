//! @file test_algorithm.cpp
//! @author ryftchen
//! @brief The definitions (test_algorithm) in the test module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include <gtest/gtest.h>
#include <syncstream>

#include "application/example/include/apply_algorithm.hpp"

//! @brief The test module.
namespace test // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Algorithm-testing-related functions in the test module.
namespace tst_algo
{
//! @brief Print the progress of the algorithm task tests.
//! @param title - task title
//! @param state - task state
//! @param align - alignment width
static void printTaskProgress(const std::string_view title, const std::string_view state, const std::uint8_t align = 50)
{
    std::osyncstream(std::cout) << "TEST ALGORITHM: " << std::setiosflags(std::ios_base::left) << std::setfill('.')
                                << std::setw(align) << title << state << std::resetiosflags(std::ios_base::left)
                                << std::setfill(' ') << std::endl;
}

//! @brief Test base of match.
class MatchTestBase : public ::testing::Test
{
private:
    //! @brief Alias for the input builder.
    using InputBuilder = application::app_algo::match::InputBuilder;
    //! @brief Prepare scenario.
    static void prepareScenario()
    {
        namespace input = application::app_algo::match::input;
        fixture = std::make_unique<InputBuilder>(input::patternString);
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
    inline static const std::string_view title{algorithm::match::description()};
    //! @brief System under test.
    [[no_unique_address]] const algorithm::match::Match sut{};
    //! @brief Fixture data.
    inline static std::unique_ptr<InputBuilder> fixture{};
    //! @brief Expected result.
    static constexpr std::int64_t expRes{49702};
};

//! @brief Test for the Rabin-Karp method in the solution of match.
TEST_F(MatchTestBase, RKMethod)
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
TEST_F(MatchTestBase, KMPMethod)
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
TEST_F(MatchTestBase, BMMethod)
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
TEST_F(MatchTestBase, HorspoolMethod)
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
TEST_F(MatchTestBase, SundayMethod)
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
private:
    //! @brief Alias for the input builder.
    using InputBuilder = application::app_algo::notation::InputBuilder;
    //! @brief Prepare scenario.
    static void prepareScenario()
    {
        namespace input = application::app_algo::notation::input;
        fixture = std::make_unique<InputBuilder>(input::infixString);
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
    inline static const std::string_view title{algorithm::notation::description()};
    //! @brief System under test.
    [[no_unique_address]] const algorithm::notation::Notation sut{};
    //! @brief Fixture data.
    inline static std::unique_ptr<InputBuilder> fixture{};
    //! @brief Expected result 1.
    static constexpr std::string_view expRes1{"+a-*b^-^cde+f*ghi"};
    //! @brief Expected result 2.
    static constexpr std::string_view expRes2{"abcd^e-fgh*+^*+i-"};
};

//! @brief Test for the prefix method in the solution of notation.
TEST_F(NotationTestBase, PrefixMethod)
{
    ASSERT_EQ(expRes1, sut.prefix(fixture->getInfixNotation()));
}

//! @brief Test for the postfix method in the solution of notation.
TEST_F(NotationTestBase, PostfixMethod)
{
    ASSERT_EQ(expRes2, sut.postfix(fixture->getInfixNotation()));
}

//! @brief Test base of optimal.
class OptimalTestBase : public ::testing::Test
{
private:
    //! @brief Alias for the input builder.
    using InputBuilder = application::app_algo::optimal::InputBuilder;
    //! @brief Prepare scenario.
    static void prepareScenario()
    {
        using application::app_algo::optimal::input::SphericalBessel;
        fixture = std::make_unique<InputBuilder>(
            SphericalBessel{}, SphericalBessel::range1, SphericalBessel::range2, SphericalBessel::funcDescr);
        sut.clear();
        sut["Gradient"] = std::make_unique<algorithm::optimal::Gradient>(fixture->getFunction());
        sut["Tabu"] = std::make_unique<algorithm::optimal::Tabu>(fixture->getFunction());
        sut["Annealing"] = std::make_unique<algorithm::optimal::Annealing>(fixture->getFunction());
        sut["Particle"] = std::make_unique<algorithm::optimal::Particle>(fixture->getFunction());
        sut["Ant"] = std::make_unique<algorithm::optimal::Ant>(fixture->getFunction());
        sut["Genetic"] = std::make_unique<algorithm::optimal::Genetic>(fixture->getFunction());
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
    inline static const std::string_view title{algorithm::optimal::description()};
    //! @brief System under test.
    inline static std::unordered_map<std::string, std::unique_ptr<algorithm::optimal::Optimal>> sut{};
    //! @brief Fixture data.
    inline static std::unique_ptr<InputBuilder> fixture{};
    //! @brief Expected result.
    static constexpr double expRes{-0.21723};
    //! @brief Allowable absolute error.
    static constexpr double absErr{0.1 * ((expRes < 0.0) ? -expRes : expRes)};
    //! @brief Default precision.
    static constexpr double defPrec{algorithm::optimal::epsilon};
};

// NOLINTBEGIN(bugprone-unchecked-optional-access)
//! @brief Test for the gradient descent method in the solution of optimal.
TEST_F(OptimalTestBase, GradientDescentMethod)
{
    auto& gradient = *sut.at("Gradient");
    const auto result = gradient(fixture->getRanges().first, fixture->getRanges().second, defPrec);
    ASSERT_TRUE(result.has_value());
    EXPECT_GT(std::get<0>(result.value()), expRes - absErr);
    EXPECT_LT(std::get<0>(result.value()), expRes + absErr);
}

//! @brief Test for the tabu method in the solution of optimal.
TEST_F(OptimalTestBase, TabuMethod)
{
    auto& tabu = *sut.at("Tabu");
    const auto result = tabu(fixture->getRanges().first, fixture->getRanges().second, defPrec);
    ASSERT_TRUE(result.has_value());
    EXPECT_GT(std::get<0>(result.value()), expRes - absErr);
    EXPECT_LT(std::get<0>(result.value()), expRes + absErr);
}

//! @brief Test for the simulated annealing method in the solution of optimal.
TEST_F(OptimalTestBase, SimulatedAnnealingMethod)
{
    auto& annealing = *sut.at("Annealing");
    const auto result = annealing(fixture->getRanges().first, fixture->getRanges().second, defPrec);
    ASSERT_TRUE(result.has_value());
    EXPECT_GT(std::get<0>(result.value()), expRes - absErr);
    EXPECT_LT(std::get<0>(result.value()), expRes + absErr);
}

//! @brief Test for the particle swarm method in the solution of optimal.
TEST_F(OptimalTestBase, ParticleSwarmMethod)
{
    auto& particle = *sut.at("Particle");
    const auto result = particle(fixture->getRanges().first, fixture->getRanges().second, defPrec);
    ASSERT_TRUE(result.has_value());
    EXPECT_GT(std::get<0>(result.value()), expRes - absErr);
    EXPECT_LT(std::get<0>(result.value()), expRes + absErr);
}

//! @brief Test for the ant colony method in the solution of optimal.
TEST_F(OptimalTestBase, AntColonyMethod)
{
    auto& ant = *sut.at("Ant");
    const auto result = ant(fixture->getRanges().first, fixture->getRanges().second, defPrec);
    ASSERT_TRUE(result.has_value());
    EXPECT_GT(std::get<0>(result.value()), expRes - absErr);
    EXPECT_LT(std::get<0>(result.value()), expRes + absErr);
}

//! @brief Test for the genetic method in the solution of optimal.
TEST_F(OptimalTestBase, GeneticMethod)
{
    auto& genetic = *sut.at("Genetic");
    const auto result = genetic(fixture->getRanges().first, fixture->getRanges().second, defPrec);
    ASSERT_TRUE(result.has_value());
    EXPECT_GT(std::get<0>(result.value()), expRes - absErr);
    EXPECT_LT(std::get<0>(result.value()), expRes + absErr);
}
// NOLINTEND(bugprone-unchecked-optional-access)

//! @brief Test base of search.
class SearchTestBase : public ::testing::Test
{
private:
    //! @brief Alias for the input builder.
    using InputBuilder = application::app_algo::search::InputBuilder<float>;
    //! @brief Prepare scenario.
    static void prepareScenario()
    {
        namespace input = application::app_algo::search::input;
        fixture = std::make_unique<InputBuilder>(input::arrayLength, input::arrayRangeMin, input::arrayRangeMax);
        expRes.clear();
        const std::span<const float> orderedArray{fixture->getOrderedArray().get(), fixture->getLength()};
        const float searchKey = fixture->getSearchKey();
        const auto range = std::ranges::equal_range(orderedArray, searchKey);
        for (auto iterator = range.begin(); iterator != range.end(); ++iterator)
        {
            expRes.emplace(std::distance(orderedArray.begin(), iterator));
        }
    }
    //! @brief Reset scenario.
    static void resetScenario()
    {
        fixture.reset();
        expRes.clear();
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
    inline static const std::string_view title{algorithm::search::description()};
    //! @brief System under test.
    [[no_unique_address]] const algorithm::search::Search<float> sut{};
    //! @brief Fixture data.
    inline static std::unique_ptr<InputBuilder> fixture{};
    //! @brief Expected result.
    inline static std::set<std::int64_t> expRes{};
};

//! @brief Test for the binary method in the solution of search.
TEST_F(SearchTestBase, BinaryMethod)
{
    ASSERT_TRUE(
        expRes.contains(sut.binary(fixture->getOrderedArray().get(), fixture->getLength(), fixture->getSearchKey())));
}

//! @brief Test for the interpolation method in the solution of search.
TEST_F(SearchTestBase, InterpolationMethod)
{
    ASSERT_TRUE(expRes.contains(
        sut.interpolation(fixture->getOrderedArray().get(), fixture->getLength(), fixture->getSearchKey())));
}

//! @brief Test for the Fibonacci method in the solution of search.
TEST_F(SearchTestBase, FibonacciMethod)
{
    ASSERT_TRUE(expRes.contains(
        sut.fibonacci(fixture->getOrderedArray().get(), fixture->getLength(), fixture->getSearchKey())));
}

//! @brief Test base of sort.
class SortTestBase : public ::testing::Test
{
private:
    //! @brief Alias for the input builder.
    using InputBuilder = application::app_algo::sort::InputBuilder<std::int32_t>;
    //! @brief Prepare scenario.
    static void prepareScenario()
    {
        namespace input = application::app_algo::sort::input;
        fixture = std::make_unique<InputBuilder>(input::arrayLength, input::arrayRangeMin, input::arrayRangeMax);
        expRes = std::vector<std::int32_t>{
            fixture->getRandomArray().get(), fixture->getRandomArray().get() + fixture->getLength()};
        std::ranges::sort(expRes);
    }
    //! @brief Reset scenario.
    static void resetScenario()
    {
        fixture.reset();
        expRes.clear();
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
    inline static const std::string_view title{algorithm::sort::description()};
    //! @brief System under test.
    [[no_unique_address]] algorithm::sort::Sort<std::int32_t> sut{};
    //! @brief Fixture data.
    inline static std::unique_ptr<InputBuilder> fixture{};
    //! @brief Expected result.
    inline static std::vector<std::int32_t> expRes{};
};

//! @brief Test for the bubble method in the solution of sort.
TEST_F(SortTestBase, BubbleMethod)
{
    ASSERT_EQ(expRes, sut.bubble(fixture->getRandomArray().get(), fixture->getLength()));
}

//! @brief Test for the selection method in the solution of sort.
TEST_F(SortTestBase, SelectionMethod)
{
    ASSERT_EQ(expRes, sut.selection(fixture->getRandomArray().get(), fixture->getLength()));
}

//! @brief Test for the insertion method in the solution of sort.
TEST_F(SortTestBase, InsertionMethod)
{
    ASSERT_EQ(expRes, sut.insertion(fixture->getRandomArray().get(), fixture->getLength()));
}

//! @brief Test for the shell method in the solution of sort.
TEST_F(SortTestBase, ShellMethod)
{
    ASSERT_EQ(expRes, sut.shell(fixture->getRandomArray().get(), fixture->getLength()));
}

//! @brief Test for the merge method in the solution of sort.
TEST_F(SortTestBase, MergeMethod)
{
    ASSERT_EQ(expRes, sut.merge(fixture->getRandomArray().get(), fixture->getLength()));
}

//! @brief Test for the quick method in the solution of sort.
TEST_F(SortTestBase, QuickMethod)
{
    ASSERT_EQ(expRes, sut.quick(fixture->getRandomArray().get(), fixture->getLength()));
}

//! @brief Test for the heap method in the solution of sort.
TEST_F(SortTestBase, HeapMethod)
{
    ASSERT_EQ(expRes, sut.heap(fixture->getRandomArray().get(), fixture->getLength()));
}

//! @brief Test for the counting method in the solution of sort.
TEST_F(SortTestBase, CountingMethod)
{
    ASSERT_EQ(expRes, sut.counting(fixture->getRandomArray().get(), fixture->getLength()));
}

//! @brief Test for the bucket method in the solution of sort.
TEST_F(SortTestBase, BucketMethod)
{
    ASSERT_EQ(expRes, sut.bucket(fixture->getRandomArray().get(), fixture->getLength()));
}

//! @brief Test for the radix method in the solution of sort.
TEST_F(SortTestBase, RadixMethod)
{
    ASSERT_EQ(expRes, sut.radix(fixture->getRandomArray().get(), fixture->getLength()));
}
} // namespace tst_algo
} // namespace test
