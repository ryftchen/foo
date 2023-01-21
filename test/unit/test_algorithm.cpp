#include <gtest/gtest.h>
#include "algorithm/include/match.hpp"
#include "algorithm/include/notation.hpp"
#include "algorithm/include/optimal.hpp"
#include "algorithm/include/search.hpp"
#include "algorithm/include/sort.hpp"

namespace test::tst_algo
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
        builder = std::make_shared<algorithm::match::TargetBuilder>(algorithm::match::maxDigit, singlePatternForMatch);
    };
    static void TearDownTestCase()
    {
        std::cout << "TEST ALGORITHM: " << std::setiosflags(std::ios_base::left) << std::setfill('.')
                  << std::setw(titleWidthForPrintTest) << "MATCH"
                  << "END" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl;
        builder.reset();
    }
    void SetUp() override{};
    void TearDown() override{};

    static std::shared_ptr<algorithm::match::TargetBuilder> builder;
    static constexpr std::string_view singlePatternForMatch{"12345"};
};

std::shared_ptr<algorithm::match::TargetBuilder> MatchTestBase::builder = nullptr;

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
        builder = std::make_shared<algorithm::notation::TargetBuilder>(infixForNotation);
    };
    static void TearDownTestCase()
    {
        std::cout << "TEST ALGORITHM: " << std::setiosflags(std::ios_base::left) << std::setfill('.')
                  << std::setw(titleWidthForPrintTest) << "NOTATION"
                  << "END" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl;
        builder.reset();
    }
    void SetUp() override{};
    void TearDown() override{};

    static std::shared_ptr<algorithm::notation::TargetBuilder> builder;
    static constexpr std::string_view infixForNotation{"a+b*(c^d-e)^(f+g*h)-i"};
};

std::shared_ptr<algorithm::notation::TargetBuilder> NotationTestBase::builder = nullptr;

TEST_F(NotationTestBase, prefixMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(
        "+a-*b^-^cde+f*ghi",
        algorithm::notation::NotationSolution::prefixMethod(std::string{builder->getInfixNotation()}));
}

TEST_F(NotationTestBase, postfixMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(
        "abcd^e-fgh*+^*+i-",
        algorithm::notation::NotationSolution::postfixMethod(std::string{builder->getInfixNotation()}));
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

    class Griewank : public algorithm::optimal::function::Function
    {
    public:
        double operator()(const double x) const override
        {
            // f(x)=1+1/4000*Σ(1→n)[(Xi)^2]-Π(1→n)[cos(Xi/(i)^(1/2))],x∈[-600,600],f(min)=0
            return (1.0 + 1.0 / 4000.0 * x * x - std::cos(x));
        }

        static constexpr double range1{-600.0};
        static constexpr double range2{600.0};
        static constexpr std::string_view funcDescr{
            "f(x)=1+1/4000*Σ(1→n)[(Xi)^2]-Π(1→n)[cos(Xi/(i)^(1/2))],x∈[-600,600] (one-dimensional Griewank)"};
    } griewank;

    class Rastrigin : public algorithm::optimal::function::Function
    {
    public:
        double operator()(const double x) const override
        {
            // f(x)=An+Σ(1→n)[(Xi)^2-Acos(2π*Xi)],A=10,x∈[-5.12,5.12],f(min)=0
            return (x * x - 10.0 * std::cos(2.0 * M_PI * x) + 10.0);
        }

        static constexpr double range1{-5.12};
        static constexpr double range2{5.12};
        static constexpr std::string_view funcDescr{
            "f(x)=An+Σ(1→n)[(Xi)^2-Acos(2π*Xi)],A=10,x∈[-5.12,5.12] (one-dimensional Rastrigin)"};
    } rastrigin;
};

TEST_F(OptimalTestBase, gradient) // NOLINT(cert-err58-cpp)
{
    std::shared_ptr<algorithm::optimal::OptimalSolution> gradient =
        std::make_shared<algorithm::optimal::Gradient>(griewank);
#ifndef _NO_PRINT_AT_RUNTIME
    std::cout << "\r\nOptimal function: " << griewank.funcDescr << std::endl;
#endif
    auto result = (*gradient)(griewank.range1, griewank.range2, algorithm::optimal::epsilon);
    ASSERT_TRUE(result.has_value());
    ASSERT_GT(get<0>(result.value()), 0.0 - 0.05);
    ASSERT_LT(get<0>(result.value()), 0.0 + 0.05);

    gradient = std::make_shared<algorithm::optimal::Gradient>(rastrigin);
#ifndef _NO_PRINT_AT_RUNTIME
    std::cout << "\r\nOptimal function: " << rastrigin.funcDescr << std::endl;
#endif
    result = (*gradient)(rastrigin.range1, rastrigin.range2, algorithm::optimal::epsilon);
    ASSERT_TRUE(result.has_value());
    ASSERT_GT(get<0>(result.value()), 0.0 - 0.05);
    ASSERT_LT(get<0>(result.value()), 0.0 + 0.05);
}

TEST_F(OptimalTestBase, annealing) // NOLINT(cert-err58-cpp)
{
    std::shared_ptr<algorithm::optimal::OptimalSolution> annealing =
        std::make_shared<algorithm::optimal::Annealing>(griewank);
#ifndef _NO_PRINT_AT_RUNTIME
    std::cout << "\r\nOptimal function: " << griewank.funcDescr << std::endl;
#endif
    auto result = (*annealing)(griewank.range1, griewank.range2, algorithm::optimal::epsilon);
    ASSERT_TRUE(result.has_value());
    ASSERT_GT(get<0>(result.value()), 0.0 - 0.05);
    ASSERT_LT(get<0>(result.value()), 0.0 + 0.05);

    annealing = std::make_shared<algorithm::optimal::Annealing>(rastrigin);
#ifndef _NO_PRINT_AT_RUNTIME
    std::cout << "\r\nOptimal function: " << rastrigin.funcDescr << std::endl;
#endif
    result = (*annealing)(rastrigin.range1, rastrigin.range2, algorithm::optimal::epsilon);
    ASSERT_TRUE(result.has_value());
    ASSERT_GT(get<0>(result.value()), 0.0 - 0.05);
    ASSERT_LT(get<0>(result.value()), 0.0 + 0.05);
}

TEST_F(OptimalTestBase, particle) // NOLINT(cert-err58-cpp)
{
    std::shared_ptr<algorithm::optimal::OptimalSolution> particle =
        std::make_shared<algorithm::optimal::Particle>(griewank);
#ifndef _NO_PRINT_AT_RUNTIME
    std::cout << "\r\nOptimal function: " << griewank.funcDescr << std::endl;
#endif
    auto result = (*particle)(griewank.range1, griewank.range2, algorithm::optimal::epsilon);
    ASSERT_TRUE(result.has_value());
    ASSERT_GT(get<0>(result.value()), 0.0 - 0.05);
    ASSERT_LT(get<0>(result.value()), 0.0 + 0.05);

    particle = std::make_shared<algorithm::optimal::Particle>(rastrigin);
#ifndef _NO_PRINT_AT_RUNTIME
    std::cout << "\r\nOptimal function: " << rastrigin.funcDescr << std::endl;
#endif
    result = (*particle)(rastrigin.range1, rastrigin.range2, algorithm::optimal::epsilon);
    ASSERT_TRUE(result.has_value());
    ASSERT_GT(get<0>(result.value()), 0.0 - 0.05);
    ASSERT_LT(get<0>(result.value()), 0.0 + 0.05);
}

TEST_F(OptimalTestBase, genetic) // NOLINT(cert-err58-cpp)
{
    std::shared_ptr<algorithm::optimal::OptimalSolution> genetic =
        std::make_shared<algorithm::optimal::Genetic>(griewank);
#ifndef _NO_PRINT_AT_RUNTIME
    std::cout << "\r\nOptimal function: " << griewank.funcDescr << std::endl;
#endif
    auto result = (*genetic)(griewank.range1, griewank.range2, algorithm::optimal::epsilon);
    ASSERT_TRUE(result.has_value());
    ASSERT_GT(get<0>(result.value()), 0.0 - 0.05);
    ASSERT_LT(get<0>(result.value()), 0.0 + 0.05);

    genetic = std::make_shared<algorithm::optimal::Genetic>(rastrigin);
#ifndef _NO_PRINT_AT_RUNTIME
    std::cout << "\r\nOptimal function: " << rastrigin.funcDescr << std::endl;
#endif
    result = (*genetic)(rastrigin.range1, rastrigin.range2, algorithm::optimal::epsilon);
    ASSERT_TRUE(result.has_value());
    ASSERT_GT(get<0>(result.value()), 0.0 - 0.05);
    ASSERT_LT(get<0>(result.value()), 0.0 + 0.05);
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
        builder = std::make_shared<algorithm::search::TargetBuilder<double>>(
            arrayLengthForSearch, arrayRangeForSearch1, arrayRangeForSearch2);
    };
    static void TearDownTestCase()
    {
        std::cout << "TEST ALGORITHM: " << std::setiosflags(std::ios_base::left) << std::setfill('.')
                  << std::setw(titleWidthForPrintTest) << "SEARCH"
                  << "END" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl;
        builder.reset();
    }
    void SetUp() override{};
    void TearDown() override{};

    static std::shared_ptr<algorithm::search::TargetBuilder<double>> builder;
    static constexpr double arrayRangeForSearch1{-50.0};
    static constexpr double arrayRangeForSearch2{150.0};
    static constexpr uint32_t arrayLengthForSearch{53};
};

std::shared_ptr<algorithm::search::TargetBuilder<double>> SearchTestBase::builder = nullptr;

TEST_F(SearchTestBase, binaryMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(
        builder->getLength() / 2,
        algorithm::search::SearchSolution<double>::binaryMethod(
            builder->getOrderedArray().get(), builder->getLength(), builder->getSearchKey()));
}

TEST_F(SearchTestBase, interpolationMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(
        builder->getLength() / 2,
        algorithm::search::SearchSolution<double>::interpolationMethod(
            builder->getOrderedArray().get(), builder->getLength(), builder->getSearchKey()));
}

TEST_F(SearchTestBase, fibonacciMethod) // NOLINT(cert-err58-cpp)
{
    ASSERT_EQ(
        builder->getLength() / 2,
        algorithm::search::SearchSolution<double>::fibonacciMethod(
            builder->getOrderedArray().get(), builder->getLength(), builder->getSearchKey()));
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
        builder = std::make_shared<algorithm::sort::TargetBuilder<int>>(
            arrayLengthForSort, arrayRangeForSort1, arrayRangeForSort2);
    };
    static void TearDownTestCase()
    {
        std::cout << "TEST ALGORITHM: " << std::setiosflags(std::ios_base::left) << std::setfill('.')
                  << std::setw(titleWidthForPrintTest) << "SORT"
                  << "END" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl;
        builder.reset();
    }
    void SetUp() override{};
    void TearDown() override{};

    static std::shared_ptr<algorithm::sort::TargetBuilder<int>> builder;
    static constexpr int arrayRangeForSort1{-50};
    static constexpr int arrayRangeForSort2{150};
    static constexpr uint32_t arrayLengthForSort{53};
};

std::shared_ptr<algorithm::sort::TargetBuilder<int>> SortTestBase::builder = nullptr;

TEST_F(SortTestBase, bubbleMethod) // NOLINT(cert-err58-cpp)
{
    std::vector<int> sortVector(
        builder->getRandomArray().get(), builder->getRandomArray().get() + builder->getLength());
    std::sort(sortVector.begin(), sortVector.end());
    ASSERT_EQ(
        sortVector,
        algorithm::sort::SortSolution<int>::bubbleMethod(builder->getRandomArray().get(), builder->getLength()));
}

TEST_F(SortTestBase, selectionMethod) // NOLINT(cert-err58-cpp)
{
    std::vector<int> sortVector(
        builder->getRandomArray().get(), builder->getRandomArray().get() + builder->getLength());
    std::sort(sortVector.begin(), sortVector.end());
    ASSERT_EQ(
        sortVector,
        algorithm::sort::SortSolution<int>::selectionMethod(builder->getRandomArray().get(), builder->getLength()));
}

TEST_F(SortTestBase, insertionMethod) // NOLINT(cert-err58-cpp)
{
    std::vector<int> sortVector(
        builder->getRandomArray().get(), builder->getRandomArray().get() + builder->getLength());
    std::sort(sortVector.begin(), sortVector.end());
    ASSERT_EQ(
        sortVector,
        algorithm::sort::SortSolution<int>::insertionMethod(builder->getRandomArray().get(), builder->getLength()));
}

TEST_F(SortTestBase, shellMethod) // NOLINT(cert-err58-cpp)
{
    std::vector<int> sortVector(
        builder->getRandomArray().get(), builder->getRandomArray().get() + builder->getLength());
    std::sort(sortVector.begin(), sortVector.end());
    ASSERT_EQ(
        sortVector,
        algorithm::sort::SortSolution<int>::shellMethod(builder->getRandomArray().get(), builder->getLength()));
}

TEST_F(SortTestBase, mergeMethod) // NOLINT(cert-err58-cpp)
{
    std::vector<int> sortVector(
        builder->getRandomArray().get(), builder->getRandomArray().get() + builder->getLength());
    std::sort(sortVector.begin(), sortVector.end());
    ASSERT_EQ(
        sortVector,
        algorithm::sort::SortSolution<int>::mergeMethod(builder->getRandomArray().get(), builder->getLength()));
}

TEST_F(SortTestBase, quickMethod) // NOLINT(cert-err58-cpp)
{
    std::vector<int> sortVector(
        builder->getRandomArray().get(), builder->getRandomArray().get() + builder->getLength());
    std::sort(sortVector.begin(), sortVector.end());
    ASSERT_EQ(
        sortVector,
        algorithm::sort::SortSolution<int>::quickMethod(builder->getRandomArray().get(), builder->getLength()));
}

TEST_F(SortTestBase, heapMethod) // NOLINT(cert-err58-cpp)
{
    std::vector<int> sortVector(
        builder->getRandomArray().get(), builder->getRandomArray().get() + builder->getLength());
    std::sort(sortVector.begin(), sortVector.end());
    ASSERT_EQ(
        sortVector,
        algorithm::sort::SortSolution<int>::heapMethod(builder->getRandomArray().get(), builder->getLength()));
}

TEST_F(SortTestBase, countingMethod) // NOLINT(cert-err58-cpp)
{
    std::vector<int> sortVector(
        builder->getRandomArray().get(), builder->getRandomArray().get() + builder->getLength());
    std::sort(sortVector.begin(), sortVector.end());
    ASSERT_EQ(
        sortVector,
        algorithm::sort::SortSolution<int>::countingMethod(builder->getRandomArray().get(), builder->getLength()));
}

TEST_F(SortTestBase, bucketMethod) // NOLINT(cert-err58-cpp)
{
    std::vector<int> sortVector(
        builder->getRandomArray().get(), builder->getRandomArray().get() + builder->getLength());
    std::sort(sortVector.begin(), sortVector.end());
    ASSERT_EQ(
        sortVector,
        algorithm::sort::SortSolution<int>::bucketMethod(builder->getRandomArray().get(), builder->getLength()));
}

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
