#include <gtest/gtest.h>
#include "algorithm/include/match.hpp"
#include "algorithm/include/notation.hpp"
#include "algorithm/include/optimal.hpp"
#include "algorithm/include/search.hpp"
#include "algorithm/include/sort.hpp"

namespace tst_algo
{
constexpr uint32_t titleWidthForPrintTest = 50;

class MatchTestBase : public ::testing::Test
{
public:
    MatchTestBase() = default;
    ~MatchTestBase() override = default;

    static void SetUpTestCase()
    {
        std::cout << "TEST ALGORITHM: " << std::setiosflags(std::ios_base::left) << std::setfill('.')
                  << std::setw(titleWidthForPrintTest) << "MATCH"
                  << "BEGIN" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl;
        match = std::make_shared<algo_match::MatchSolution>(algo_match::maxDigit);
    };
    static void TearDownTestCase()
    {
        std::cout << "TEST ALGORITHM: " << std::setiosflags(std::ios_base::left) << std::setfill('.')
                  << std::setw(titleWidthForPrintTest) << "MATCH"
                  << "END" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl;
        match.reset();
    }
    void SetUp() override{};
    void TearDown() override{};

    static std::shared_ptr<algo_match::MatchSolution> match;
    static constexpr std::string_view singlePatternForMatch{"12345"};
};

std::shared_ptr<algo_match::MatchSolution> MatchTestBase::match = nullptr;

TEST_F(MatchTestBase, rkMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(
        49702,
        match->rkMethod(
            match->getMatchingText().get(),
            singlePatternForMatch.data(),
            match->getLength(),
            singlePatternForMatch.length()));
}

TEST_F(MatchTestBase, kmpMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(
        49702,
        match->kmpMethod(
            match->getMatchingText().get(),
            singlePatternForMatch.data(),
            match->getLength(),
            singlePatternForMatch.length()));
}

TEST_F(MatchTestBase, bmMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(
        49702,
        match->bmMethod(
            match->getMatchingText().get(),
            singlePatternForMatch.data(),
            match->getLength(),
            singlePatternForMatch.length()));
}

TEST_F(MatchTestBase, horspoolMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(
        49702,
        match->horspoolMethod(
            match->getMatchingText().get(),
            singlePatternForMatch.data(),
            match->getLength(),
            singlePatternForMatch.length()));
}

TEST_F(MatchTestBase, sundayMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(
        49702,
        match->sundayMethod(
            match->getMatchingText().get(),
            singlePatternForMatch.data(),
            match->getLength(),
            singlePatternForMatch.length()));
}

class NotationTestBase : public ::testing::Test
{
public:
    NotationTestBase() = default;
    ~NotationTestBase() override = default;

    static void SetUpTestCase()
    {
        std::cout << "TEST ALGORITHM: " << std::setiosflags(std::ios_base::left) << std::setfill('.')
                  << std::setw(titleWidthForPrintTest) << "NOTATION"
                  << "BEGIN" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl;
        notation = std::make_shared<algo_notation::NotationSolution>(infixForNotation);
    };
    static void TearDownTestCase()
    {
        std::cout << "TEST ALGORITHM: " << std::setiosflags(std::ios_base::left) << std::setfill('.')
                  << std::setw(titleWidthForPrintTest) << "NOTATION"
                  << "END" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl;
        notation.reset();
    }
    void SetUp() override{};
    void TearDown() override{};

    static std::shared_ptr<algo_notation::NotationSolution> notation;
    static constexpr std::string_view infixForNotation{"a+b*(c^d-e)^(f+g*h)-i"};
};

std::shared_ptr<algo_notation::NotationSolution> NotationTestBase::notation = nullptr;

TEST_F(NotationTestBase, prefixMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ("+a-*b^-^cde+f*ghi", notation->prefixMethod(std::string{infixForNotation}));
}

TEST_F(NotationTestBase, postfixMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ("abcd^e-fgh*+^*+i-", notation->postfixMethod(std::string{infixForNotation}));
}

class OptimalTestBase : public ::testing::Test
{
public:
    OptimalTestBase() = default;
    ~OptimalTestBase() override = default;

    static void SetUpTestCase()
    {
        std::cout << "TEST ALGORITHM: " << std::setiosflags(std::ios_base::left) << std::setfill('.')
                  << std::setw(titleWidthForPrintTest) << "OPTIMAL"
                  << "BEGIN" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl;
    };
    static void TearDownTestCase()
    {
        std::cout << "TEST ALGORITHM: " << std::setiosflags(std::ios_base::left) << std::setfill('.')
                  << std::setw(titleWidthForPrintTest) << "OPTIMAL"
                  << "END" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl;
    }
    void SetUp() override{};
    void TearDown() override{};

    class Griewank : public algo_optimal::function::Function
    {
    public:
        double operator()(const double x) const override
        {
            // f(x)=1+1/4000*Σ(1→n)[(Xi)^2]-Π(1→n)[cos(Xi/(i)^(1/2))],x∈[-600,600],f(min)=0
            return (1.0 + 1.0 / 4000.0 * x * x - std::cos(x));
        }

        static constexpr double range1{-600.0};
        static constexpr double range2{600.0};
        static constexpr std::string_view funcStr{
            "f(x)=1+1/4000*Σ(1→n)[(Xi)^2]-Π(1→n)[cos(Xi/(i)^(1/2))],x∈[-600,600] (one-dimensional Griewank)"};
    } griewank;

    class Rastrigin : public algo_optimal::function::Function
    {
    public:
        double operator()(const double x) const override
        {
            // f(x)=An+Σ(1→n)[(Xi)^2-Acos(2π*Xi)],A=10,x∈[-5.12,5.12],f(min)=0
            return (x * x - 10.0 * std::cos(2.0 * M_PI * x) + 10.0);
        }

        static constexpr double range1{-5.12};
        static constexpr double range2{5.12};
        static constexpr std::string_view funcStr{
            "f(x)=An+Σ(1→n)[(Xi)^2-Acos(2π*Xi)],A=10,x∈[-5.12,5.12] (one-dimensional Rastrigin)"};
    } rastrigin;
};

TEST_F(OptimalTestBase, gradient) // NOLINT(cert-err58-cpp)
{
    constexpr std::string_view prefix{"\r\nOptimal function: "};

    std::cout << prefix << griewank.funcStr << std::endl;
    std::shared_ptr<algo_optimal::OptimalSolution> gradient = std::make_shared<algo_optimal::Gradient>(griewank);
    auto result = (*gradient)(griewank.range1, griewank.range2, algo_optimal::epsilon);
    ASSERT_TRUE(result.has_value());
    ASSERT_GT(get<0>(result.value()), -1.0);
    ASSERT_LT(get<0>(result.value()), 1.0);

    std::cout << prefix << rastrigin.funcStr << std::endl;
    gradient = std::make_shared<algo_optimal::Gradient>(rastrigin);
    result = (*gradient)(rastrigin.range1, rastrigin.range2, algo_optimal::epsilon);
    ASSERT_TRUE(result.has_value());
    ASSERT_GT(get<0>(result.value()), -1.0);
    ASSERT_LT(get<0>(result.value()), 1.0);
}

TEST_F(OptimalTestBase, annealing) // NOLINT(cert-err58-cpp)
{
    constexpr std::string_view prefix{"\r\nOptimal function: "};

    std::shared_ptr<algo_optimal::OptimalSolution> annealing = std::make_shared<algo_optimal::Annealing>(griewank);
    std::cout << prefix << griewank.funcStr << std::endl;
    auto result = (*annealing)(griewank.range1, griewank.range2, algo_optimal::epsilon);
    ASSERT_TRUE(result.has_value());
    ASSERT_GT(get<0>(result.value()), -1.0);
    ASSERT_LT(get<0>(result.value()), 1.0);

    std::cout << prefix << rastrigin.funcStr << std::endl;
    annealing = std::make_shared<algo_optimal::Annealing>(rastrigin);
    result = (*annealing)(rastrigin.range1, rastrigin.range2, algo_optimal::epsilon);
    ASSERT_TRUE(result.has_value());
    ASSERT_GT(get<0>(result.value()), -1.0);
    ASSERT_LT(get<0>(result.value()), 1.0);
}

TEST_F(OptimalTestBase, particle) // NOLINT(cert-err58-cpp)
{
    constexpr std::string_view prefix{"\r\nOptimal function: "};

    std::shared_ptr<algo_optimal::OptimalSolution> particle = std::make_shared<algo_optimal::Particle>(griewank);
    std::cout << prefix << griewank.funcStr << std::endl;
    auto result = (*particle)(griewank.range1, griewank.range2, algo_optimal::epsilon);
    ASSERT_TRUE(result.has_value());
    ASSERT_GT(get<0>(result.value()), -1.0);
    ASSERT_LT(get<0>(result.value()), 1.0);

    std::cout << prefix << rastrigin.funcStr << std::endl;
    particle = std::make_shared<algo_optimal::Particle>(rastrigin);
    result = (*particle)(rastrigin.range1, rastrigin.range2, algo_optimal::epsilon);
    ASSERT_TRUE(result.has_value());
    ASSERT_GT(get<0>(result.value()), -1.0);
    ASSERT_LT(get<0>(result.value()), 1.0);
}

TEST_F(OptimalTestBase, genetic) // NOLINT(cert-err58-cpp)
{
    constexpr std::string_view prefix{"\r\nOptimal function: "};

    std::shared_ptr<algo_optimal::OptimalSolution> genetic = std::make_shared<algo_optimal::Genetic>(griewank);
    std::cout << prefix << griewank.funcStr << std::endl;
    auto result = (*genetic)(griewank.range1, griewank.range2, algo_optimal::epsilon);
    ASSERT_TRUE(result.has_value());
    ASSERT_GT(get<0>(result.value()), -1.0);
    ASSERT_LT(get<0>(result.value()), 1.0);

    std::cout << prefix << rastrigin.funcStr << std::endl;
    genetic = std::make_shared<algo_optimal::Genetic>(rastrigin);
    result = (*genetic)(rastrigin.range1, rastrigin.range2, algo_optimal::epsilon);
    ASSERT_TRUE(result.has_value());
    ASSERT_GT(get<0>(result.value()), -1.0);
    ASSERT_LT(get<0>(result.value()), 1.0);
}

class SearchTestBase : public ::testing::Test
{
public:
    SearchTestBase() = default;
    ~SearchTestBase() override = default;

    static void SetUpTestCase()
    {
        std::cout << "TEST ALGORITHM: " << std::setiosflags(std::ios_base::left) << std::setfill('.')
                  << std::setw(titleWidthForPrintTest) << "SEARCH"
                  << "BEGIN" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl;
        search = std::make_shared<algo_search::SearchSolution<double>>(
            arrayLengthForSearch, arrayRangeForSearch1, arrayRangeForSearch2);
    };
    static void TearDownTestCase()
    {
        std::cout << "TEST ALGORITHM: " << std::setiosflags(std::ios_base::left) << std::setfill('.')
                  << std::setw(titleWidthForPrintTest) << "SEARCH"
                  << "END" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl;
        search.reset();
    }
    void SetUp() override{};
    void TearDown() override{};

    static std::shared_ptr<algo_search::SearchSolution<double>> search;
    static constexpr double arrayRangeForSearch1 = -50.0;
    static constexpr double arrayRangeForSearch2 = 150.0;
    static constexpr uint32_t arrayLengthForSearch = 53;
};

std::shared_ptr<algo_search::SearchSolution<double>> SearchTestBase::search = nullptr;

TEST_F(SearchTestBase, binaryMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(
        search->getLength() / 2,
        search->binaryMethod(search->getOrderedArray().get(), search->getLength(), search->getSearchedKey()));
}

TEST_F(SearchTestBase, interpolationMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(
        search->getLength() / 2,
        search->interpolationMethod(search->getOrderedArray().get(), search->getLength(), search->getSearchedKey()));
}

TEST_F(SearchTestBase, fibonacciMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(
        search->getLength() / 2,
        search->fibonacciMethod(search->getOrderedArray().get(), search->getLength(), search->getSearchedKey()));
}

class SortTestBase : public ::testing::Test
{
public:
    SortTestBase() = default;
    ~SortTestBase() override = default;

    static void SetUpTestCase()
    {
        std::cout << "TEST ALGORITHM: " << std::setiosflags(std::ios_base::left) << std::setfill('.')
                  << std::setw(titleWidthForPrintTest) << "SORT"
                  << "BEGIN" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl;
        sort =
            std::make_shared<algo_sort::SortSolution<int>>(arrayLengthForSort, arrayRangeForSort1, arrayRangeForSort2);
    };
    static void TearDownTestCase()
    {
        std::cout << "TEST ALGORITHM: " << std::setiosflags(std::ios_base::left) << std::setfill('.')
                  << std::setw(titleWidthForPrintTest) << "SORT"
                  << "END" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl;
        sort.reset();
    }
    void SetUp() override{};
    void TearDown() override{};

    static std::shared_ptr<algo_sort::SortSolution<int>> sort;
    static constexpr int arrayRangeForSort1 = -50;
    static constexpr int arrayRangeForSort2 = 150;
    static constexpr uint32_t arrayLengthForSort = 53;
};

std::shared_ptr<algo_sort::SortSolution<int>> SortTestBase::sort = nullptr;

TEST_F(SortTestBase, bubbleMethod) // NOLINT(cert-err58-cpp)
{
    std::vector<int> sortVector(sort->getRandomArray().get(), sort->getRandomArray().get() + sort->getLength());
    std::sort(sortVector.begin(), sortVector.end());
    ASSERT_EQ(sortVector, sort->bubbleMethod(sort->getRandomArray().get(), sort->getLength()));
}

TEST_F(SortTestBase, selectionMethod) // NOLINT(cert-err58-cpp)
{
    std::vector<int> sortVector(sort->getRandomArray().get(), sort->getRandomArray().get() + sort->getLength());
    std::sort(sortVector.begin(), sortVector.end());
    ASSERT_EQ(sortVector, sort->selectionMethod(sort->getRandomArray().get(), sort->getLength()));
}

TEST_F(SortTestBase, insertionMethod) // NOLINT(cert-err58-cpp)
{
    std::vector<int> sortVector(sort->getRandomArray().get(), sort->getRandomArray().get() + sort->getLength());
    std::sort(sortVector.begin(), sortVector.end());
    ASSERT_EQ(sortVector, sort->insertionMethod(sort->getRandomArray().get(), sort->getLength()));
}

TEST_F(SortTestBase, shellMethod) // NOLINT(cert-err58-cpp)
{
    std::vector<int> sortVector(sort->getRandomArray().get(), sort->getRandomArray().get() + sort->getLength());
    std::sort(sortVector.begin(), sortVector.end());
    ASSERT_EQ(sortVector, sort->shellMethod(sort->getRandomArray().get(), sort->getLength()));
}

TEST_F(SortTestBase, mergeMethod) // NOLINT(cert-err58-cpp)
{
    std::vector<int> sortVector(sort->getRandomArray().get(), sort->getRandomArray().get() + sort->getLength());
    std::sort(sortVector.begin(), sortVector.end());
    ASSERT_EQ(sortVector, sort->mergeMethod(sort->getRandomArray().get(), sort->getLength()));
}

TEST_F(SortTestBase, quickMethod) // NOLINT(cert-err58-cpp)
{
    std::vector<int> sortVector(sort->getRandomArray().get(), sort->getRandomArray().get() + sort->getLength());
    std::sort(sortVector.begin(), sortVector.end());
    ASSERT_EQ(sortVector, sort->quickMethod(sort->getRandomArray().get(), sort->getLength()));
}

TEST_F(SortTestBase, heapMethod) // NOLINT(cert-err58-cpp)
{
    std::vector<int> sortVector(sort->getRandomArray().get(), sort->getRandomArray().get() + sort->getLength());
    std::sort(sortVector.begin(), sortVector.end());
    ASSERT_EQ(sortVector, sort->heapMethod(sort->getRandomArray().get(), sort->getLength()));
}

TEST_F(SortTestBase, countingMethod) // NOLINT(cert-err58-cpp)
{
    std::vector<int> sortVector(sort->getRandomArray().get(), sort->getRandomArray().get() + sort->getLength());
    std::sort(sortVector.begin(), sortVector.end());
    ASSERT_EQ(sortVector, sort->countingMethod(sort->getRandomArray().get(), sort->getLength()));
}

TEST_F(SortTestBase, bucketMethod) // NOLINT(cert-err58-cpp)
{
    std::vector<int> sortVector(sort->getRandomArray().get(), sort->getRandomArray().get() + sort->getLength());
    std::sort(sortVector.begin(), sortVector.end());
    ASSERT_EQ(sortVector, sort->bucketMethod(sort->getRandomArray().get(), sort->getLength()));
}

TEST_F(SortTestBase, radixMethod) // NOLINT(cert-err58-cpp)
{
    std::vector<int> sortVector(sort->getRandomArray().get(), sort->getRandomArray().get() + sort->getLength());
    std::sort(sortVector.begin(), sortVector.end());
    ASSERT_EQ(sortVector, sort->radixMethod(sort->getRandomArray().get(), sort->getLength()));
}
} // namespace tst_algo
