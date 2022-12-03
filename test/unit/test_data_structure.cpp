#include <gtest/gtest.h>
#include "data_structure/include/linear.hpp"
#include "data_structure/include/tree.hpp"

namespace test::tst_ds
{
constexpr uint32_t titleWidthForPrintTest = 50;

class LinearTestBase : public ::testing::Test
{
public:
    LinearTestBase() = default;
    ~LinearTestBase() override = default;

    static void SetUpTestCase()
    {
        std::cout << "TEST DATA STRUCTURE: " << std::setiosflags(std::ios_base::left) << std::setfill('.')
                  << std::setw(titleWidthForPrintTest) << "LINEAR"
                  << "BEGIN" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl;
        linear = std::make_shared<date_structure::linear::LinearStructure>();
    };
    static void TearDownTestCase()
    {
        std::cout << "TEST DATA STRUCTURE: " << std::setiosflags(std::ios_base::left) << std::setfill('.')
                  << std::setw(titleWidthForPrintTest) << "LINEAR"
                  << "END" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl;
        linear.reset();
    }
    void SetUp() override{};
    void TearDown() override{};

    static std::shared_ptr<date_structure::linear::LinearStructure> linear;
};

std::shared_ptr<date_structure::linear::LinearStructure> LinearTestBase::linear = nullptr;

TEST_F(LinearTestBase, linkedListInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(linear->linkedListInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

TEST_F(LinearTestBase, stackInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(linear->stackInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

TEST_F(LinearTestBase, queueInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(linear->queueInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

class TreeTestBase : public ::testing::Test
{
public:
    TreeTestBase() = default;
    ~TreeTestBase() override = default;

    static void SetUpTestCase()
    {
        std::cout << "TEST DATA STRUCTURE: " << std::setiosflags(std::ios_base::left) << std::setfill('.')
                  << std::setw(titleWidthForPrintTest) << "TREE"
                  << "BEGIN" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl;
        tree = std::make_shared<date_structure::tree::TreeStructure>();
    };
    static void TearDownTestCase()
    {
        std::cout << "TEST DATA STRUCTURE: " << std::setiosflags(std::ios_base::left) << std::setfill('.')
                  << std::setw(titleWidthForPrintTest) << "TREE"
                  << "END" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl;
        tree.reset();
    }
    void SetUp() override{};
    void TearDown() override{};

    static std::shared_ptr<date_structure::tree::TreeStructure> tree;
};

std::shared_ptr<date_structure::tree::TreeStructure> TreeTestBase::tree = nullptr;

TEST_F(TreeTestBase, bsInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(tree->bsInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

TEST_F(TreeTestBase, avlInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(tree->avlInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

TEST_F(TreeTestBase, splayInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(tree->splayInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}
} // namespace test::tst_ds
