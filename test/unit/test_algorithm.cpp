//! @file test_algorithm.cpp
//! @author ryftchen
//! @brief The definitions (test_algorithm) in the test module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#include <gtest/gtest.h>
#include "algorithm/include/match.hpp"
#include "algorithm/include/notation.hpp"
#include "algorithm/include/optimal.hpp"
#include "algorithm/include/search.hpp"
#include "algorithm/include/sort.hpp"

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
        builder = std::make_shared<algorithm::match::TargetBuilder>(algorithm::match::maxDigit, singlePatternForMatch);
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
    static std::shared_ptr<algorithm::match::TargetBuilder> builder;
    //! @brief Single pattern for match methods.
    static constexpr std::string_view singlePatternForMatch{"12345"};
};

std::shared_ptr<algorithm::match::TargetBuilder> MatchTestBase::builder = nullptr;

//! @brief Test for the Rabin-Karp method in the solution of match.
TEST_F(MatchTestBase, rkMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(
        49702,
        algorithm::match::MatchSolution::rkMethod(
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
        algorithm::match::MatchSolution::kmpMethod(
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
        algorithm::match::MatchSolution::bmMethod(
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
        algorithm::match::MatchSolution::horspoolMethod(
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
        algorithm::match::MatchSolution::sundayMethod(
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
        builder = std::make_shared<algorithm::notation::TargetBuilder>(infixForNotation);
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
    static std::shared_ptr<algorithm::notation::TargetBuilder> builder;
    //! @brief Infix for notation methods.
    static constexpr std::string_view infixForNotation{"a+b*(c^d-e)^(f+g*h)-i"};
};

std::shared_ptr<algorithm::notation::TargetBuilder> NotationTestBase::builder = nullptr;

//! @brief Test for the prefix method in the solution of notation.
TEST_F(NotationTestBase, prefixMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(
        "+a-*b^-^cde+f*ghi",
        algorithm::notation::NotationSolution::prefixMethod(std::string{builder->getInfixNotation()}));
}

//! @brief Test for the postfix method in the solution of notation.
TEST_F(NotationTestBase, postfixMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(
        "abcd^e-fgh*+^*+i-",
        algorithm::notation::NotationSolution::postfixMethod(std::string{builder->getInfixNotation()}));
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

    //! @brief Griewank function.
    class Griewank : public algorithm::optimal::function::Function
    {
    public:
        //! @brief The operator (()) overloading of Griewank class.
        //! @param x - independent variable
        //! @return dependent variable
        double operator()(const double x) const override
        {
            // f(x)=1+1/4000*Σ(1→n)[(Xi)^2]-Π(1→n)[cos(Xi/(i)^(1/2))],x∈[-600,600],f(min)=0
            return (1.0 + 1.0 / 4000.0 * x * x - std::cos(x));
        }

        //! @brief Left endpoint.
        static constexpr double range1{-600.0};
        //! @brief Right endpoint.
        static constexpr double range2{600.0};
        //! @brief One-dimensional Griewank.
        static constexpr std::string_view funcDescr{
            "f(x)=1+1/4000*Σ(1→n)[(Xi)^2]-Π(1→n)[cos(Xi/(i)^(1/2))],x∈[-600,600] (one-dimensional Griewank)"};
    } /** @brief Griewank function object. */ griewank;

    //! @brief Rastrigin function.
    class Rastrigin : public algorithm::optimal::function::Function
    {
    public:
        //! @brief The operator (()) overloading of Rastrigin class.
        //! @param x - independent variable
        //! @return dependent variable
        double operator()(const double x) const override
        {
            // f(x)=An+Σ(1→n)[(Xi)^2-Acos(2π*Xi)],A=10,x∈[-5.12,5.12],f(min)=0
            return (x * x - 10.0 * std::cos(2.0 * M_PI * x) + 10.0);
        }

        //! @brief Left endpoint.
        static constexpr double range1{-5.12};
        //! @brief Right endpoint.
        static constexpr double range2{5.12};
        //! @brief One-dimensional Rastrigin.
        static constexpr std::string_view funcDescr{
            "f(x)=An+Σ(1→n)[(Xi)^2-Acos(2π*Xi)],A=10,x∈[-5.12,5.12] (one-dimensional Rastrigin)"};
    } /** @brief Rastrigin function object. */ rastrigin;
};

//! @brief Test for the gradient descent method in the solution of optimal.
TEST_F(OptimalTestBase, gradient) // NOLINT(cert-err58-cpp)
{
    std::shared_ptr<algorithm::optimal::OptimalSolution> gradient =
        std::make_shared<algorithm::optimal::Gradient>(griewank);
#ifdef __RUNTIME_PRINTING
    std::cout << "\r\nOptimal function: " << griewank.funcDescr << std::endl;
#endif
    auto result = (*gradient)(griewank.range1, griewank.range2, algorithm::optimal::epsilon);
    ASSERT_TRUE(result.has_value());
    ASSERT_GT(get<0>(result.value()), 0.0 - 0.05);
    ASSERT_LT(get<0>(result.value()), 0.0 + 0.05);

    gradient = std::make_shared<algorithm::optimal::Gradient>(rastrigin);
#ifdef __RUNTIME_PRINTING
    std::cout << "\r\nOptimal function: " << rastrigin.funcDescr << std::endl;
#endif
    result = (*gradient)(rastrigin.range1, rastrigin.range2, algorithm::optimal::epsilon);
    ASSERT_TRUE(result.has_value());
    ASSERT_GT(get<0>(result.value()), 0.0 - 0.05);
    ASSERT_LT(get<0>(result.value()), 0.0 + 0.05);
}

//! @brief Test for the simulated annealing method in the solution of optimal.
TEST_F(OptimalTestBase, annealing) // NOLINT(cert-err58-cpp)
{
    std::shared_ptr<algorithm::optimal::OptimalSolution> annealing =
        std::make_shared<algorithm::optimal::Annealing>(griewank);
#ifdef __RUNTIME_PRINTING
    std::cout << "\r\nOptimal function: " << griewank.funcDescr << std::endl;
#endif
    auto result = (*annealing)(griewank.range1, griewank.range2, algorithm::optimal::epsilon);
    ASSERT_TRUE(result.has_value());
    ASSERT_GT(get<0>(result.value()), 0.0 - 0.05);
    ASSERT_LT(get<0>(result.value()), 0.0 + 0.05);

    annealing = std::make_shared<algorithm::optimal::Annealing>(rastrigin);
#ifdef __RUNTIME_PRINTING
    std::cout << "\r\nOptimal function: " << rastrigin.funcDescr << std::endl;
#endif
    result = (*annealing)(rastrigin.range1, rastrigin.range2, algorithm::optimal::epsilon);
    ASSERT_TRUE(result.has_value());
    ASSERT_GT(get<0>(result.value()), 0.0 - 0.05);
    ASSERT_LT(get<0>(result.value()), 0.0 + 0.05);
}

//! @brief Test for the particle swarm method in the solution of optimal.
TEST_F(OptimalTestBase, particle) // NOLINT(cert-err58-cpp)
{
    std::shared_ptr<algorithm::optimal::OptimalSolution> particle =
        std::make_shared<algorithm::optimal::Particle>(griewank);
#ifdef __RUNTIME_PRINTING
    std::cout << "\r\nOptimal function: " << griewank.funcDescr << std::endl;
#endif
    auto result = (*particle)(griewank.range1, griewank.range2, algorithm::optimal::epsilon);
    ASSERT_TRUE(result.has_value());
    ASSERT_GT(get<0>(result.value()), 0.0 - 0.05);
    ASSERT_LT(get<0>(result.value()), 0.0 + 0.05);

    particle = std::make_shared<algorithm::optimal::Particle>(rastrigin);
#ifdef __RUNTIME_PRINTING
    std::cout << "\r\nOptimal function: " << rastrigin.funcDescr << std::endl;
#endif
    result = (*particle)(rastrigin.range1, rastrigin.range2, algorithm::optimal::epsilon);
    ASSERT_TRUE(result.has_value());
    ASSERT_GT(get<0>(result.value()), 0.0 - 0.05);
    ASSERT_LT(get<0>(result.value()), 0.0 + 0.05);
}

//! @brief Test for the genetic method in the solution of optimal.
TEST_F(OptimalTestBase, genetic) // NOLINT(cert-err58-cpp)
{
    std::shared_ptr<algorithm::optimal::OptimalSolution> genetic =
        std::make_shared<algorithm::optimal::Genetic>(griewank);
#ifdef __RUNTIME_PRINTING
    std::cout << "\r\nOptimal function: " << griewank.funcDescr << std::endl;
#endif
    auto result = (*genetic)(griewank.range1, griewank.range2, algorithm::optimal::epsilon);
    ASSERT_TRUE(result.has_value());
    ASSERT_GT(get<0>(result.value()), 0.0 - 0.05);
    ASSERT_LT(get<0>(result.value()), 0.0 + 0.05);

    genetic = std::make_shared<algorithm::optimal::Genetic>(rastrigin);
#ifdef __RUNTIME_PRINTING
    std::cout << "\r\nOptimal function: " << rastrigin.funcDescr << std::endl;
#endif
    result = (*genetic)(rastrigin.range1, rastrigin.range2, algorithm::optimal::epsilon);
    ASSERT_TRUE(result.has_value());
    ASSERT_GT(get<0>(result.value()), 0.0 - 0.05);
    ASSERT_LT(get<0>(result.value()), 0.0 + 0.05);
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
        builder = std::make_shared<algorithm::search::TargetBuilder<double>>(
            arrayLengthForSearch, arrayRangeForSearch1, arrayRangeForSearch2);
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
    static std::shared_ptr<algorithm::search::TargetBuilder<double>> builder;
    //! @brief Minimum of the array for search methods.
    static constexpr double arrayRangeForSearch1{-50.0};
    //! @brief Maximum of the array for search methods.
    static constexpr double arrayRangeForSearch2{150.0};
    //! @brief Length of the array for search methods.
    static constexpr uint32_t arrayLengthForSearch{53};
};

std::shared_ptr<algorithm::search::TargetBuilder<double>> SearchTestBase::builder = nullptr;

//! @brief Test for the binary method in the solution of search.
TEST_F(SearchTestBase, binaryMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(
        builder->getLength() / 2,
        algorithm::search::SearchSolution<double>::binaryMethod(
            builder->getOrderedArray().get(), builder->getLength(), builder->getSearchKey()));
}

//! @brief Test for the interpolation method in the solution of search.
TEST_F(SearchTestBase, interpolationMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(
        builder->getLength() / 2,
        algorithm::search::SearchSolution<double>::interpolationMethod(
            builder->getOrderedArray().get(), builder->getLength(), builder->getSearchKey()));
}

//! @brief Test for the Fibonacci method in the solution of search.
TEST_F(SearchTestBase, fibonacciMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(
        builder->getLength() / 2,
        algorithm::search::SearchSolution<double>::fibonacciMethod(
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
        builder = std::make_shared<algorithm::sort::TargetBuilder<int>>(
            arrayLengthForSort, arrayRangeForSort1, arrayRangeForSort2);
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
    static std::shared_ptr<algorithm::sort::TargetBuilder<int>> builder;
    //! @brief Minimum of the array for sort methods.
    static constexpr int arrayRangeForSort1{-50};
    //! @brief Maximum of the array for sort methods.
    static constexpr int arrayRangeForSort2{150};
    //! @brief Length of the array for sort methods.
    static constexpr uint32_t arrayLengthForSort{53};
};

std::shared_ptr<algorithm::sort::TargetBuilder<int>> SortTestBase::builder = nullptr;

//! @brief Test for the bubble method in the solution of sort.
TEST_F(SortTestBase, bubbleMethod) // NOLINT(cert-err58-cpp)
{
    std::vector<int> sortVector(
        builder->getRandomArray().get(), builder->getRandomArray().get() + builder->getLength());
    std::sort(sortVector.begin(), sortVector.end());
    ASSERT_EQ(
        sortVector,
        algorithm::sort::SortSolution<int>::bubbleMethod(builder->getRandomArray().get(), builder->getLength()));
}

//! @brief Test for the selection method in the solution of sort.
TEST_F(SortTestBase, selectionMethod) // NOLINT(cert-err58-cpp)
{
    std::vector<int> sortVector(
        builder->getRandomArray().get(), builder->getRandomArray().get() + builder->getLength());
    std::sort(sortVector.begin(), sortVector.end());
    ASSERT_EQ(
        sortVector,
        algorithm::sort::SortSolution<int>::selectionMethod(builder->getRandomArray().get(), builder->getLength()));
}

//! @brief Test for the insertion method in the solution of sort.
TEST_F(SortTestBase, insertionMethod) // NOLINT(cert-err58-cpp)
{
    std::vector<int> sortVector(
        builder->getRandomArray().get(), builder->getRandomArray().get() + builder->getLength());
    std::sort(sortVector.begin(), sortVector.end());
    ASSERT_EQ(
        sortVector,
        algorithm::sort::SortSolution<int>::insertionMethod(builder->getRandomArray().get(), builder->getLength()));
}

//! @brief Test for the shell method in the solution of sort.
TEST_F(SortTestBase, shellMethod) // NOLINT(cert-err58-cpp)
{
    std::vector<int> sortVector(
        builder->getRandomArray().get(), builder->getRandomArray().get() + builder->getLength());
    std::sort(sortVector.begin(), sortVector.end());
    ASSERT_EQ(
        sortVector,
        algorithm::sort::SortSolution<int>::shellMethod(builder->getRandomArray().get(), builder->getLength()));
}

//! @brief Test for the merge method in the solution of sort.
TEST_F(SortTestBase, mergeMethod) // NOLINT(cert-err58-cpp)
{
    std::vector<int> sortVector(
        builder->getRandomArray().get(), builder->getRandomArray().get() + builder->getLength());
    std::sort(sortVector.begin(), sortVector.end());
    ASSERT_EQ(
        sortVector,
        algorithm::sort::SortSolution<int>::mergeMethod(builder->getRandomArray().get(), builder->getLength()));
}

//! @brief Test for the quick method in the solution of sort.
TEST_F(SortTestBase, quickMethod) // NOLINT(cert-err58-cpp)
{
    std::vector<int> sortVector(
        builder->getRandomArray().get(), builder->getRandomArray().get() + builder->getLength());
    std::sort(sortVector.begin(), sortVector.end());
    ASSERT_EQ(
        sortVector,
        algorithm::sort::SortSolution<int>::quickMethod(builder->getRandomArray().get(), builder->getLength()));
}

//! @brief Test for the heap method in the solution of sort.
TEST_F(SortTestBase, heapMethod) // NOLINT(cert-err58-cpp)
{
    std::vector<int> sortVector(
        builder->getRandomArray().get(), builder->getRandomArray().get() + builder->getLength());
    std::sort(sortVector.begin(), sortVector.end());
    ASSERT_EQ(
        sortVector,
        algorithm::sort::SortSolution<int>::heapMethod(builder->getRandomArray().get(), builder->getLength()));
}

//! @brief Test for the counting method in the solution of sort.
TEST_F(SortTestBase, countingMethod) // NOLINT(cert-err58-cpp)
{
    std::vector<int> sortVector(
        builder->getRandomArray().get(), builder->getRandomArray().get() + builder->getLength());
    std::sort(sortVector.begin(), sortVector.end());
    ASSERT_EQ(
        sortVector,
        algorithm::sort::SortSolution<int>::countingMethod(builder->getRandomArray().get(), builder->getLength()));
}

//! @brief Test for the bucket method in the solution of sort.
TEST_F(SortTestBase, bucketMethod) // NOLINT(cert-err58-cpp)
{
    std::vector<int> sortVector(
        builder->getRandomArray().get(), builder->getRandomArray().get() + builder->getLength());
    std::sort(sortVector.begin(), sortVector.end());
    ASSERT_EQ(
        sortVector,
        algorithm::sort::SortSolution<int>::bucketMethod(builder->getRandomArray().get(), builder->getLength()));
}

//! @brief Test for the radix method in the solution of sort.
TEST_F(SortTestBase, radixMethod) // NOLINT(cert-err58-cpp)
{
    std::vector<int> sortVector(
        builder->getRandomArray().get(), builder->getRandomArray().get() + builder->getLength());
    std::sort(sortVector.begin(), sortVector.end());
    ASSERT_EQ(
        sortVector,
        algorithm::sort::SortSolution<int>::radixMethod(builder->getRandomArray().get(), builder->getLength()));
}
} // namespace test::tst_algo
