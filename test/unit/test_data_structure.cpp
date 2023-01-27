//! @file test_data_structure.cpp
//! @author ryftchen
//! @brief The definitions (test_data_structure) in the test module.
//! @version 0.1
//! @copyright Copyright (c) 2022
#include <gtest/gtest.h>
#include "data_structure/include/linear.hpp"
#include "data_structure/include/tree.hpp"

//! @brief Title of printing for data structure task tests.
#define TST_DS_PRINT_TASK_TITLE(taskType, taskState)                                                   \
    std::cout << "TEST DATA STRUCTURE: " << std::setiosflags(std::ios_base::left) << std::setfill('.') \
              << std::setw(50) << taskType << taskState << std::resetiosflags(std::ios_base::left)     \
              << std::setfill(' ') << std::endl;

//! @brief Data-structure-testing-related functions in the test module.
namespace test::tst_ds
{
//! @brief Test base of linear.
class LinearTestBase : public ::testing::Test
{
public:
    //! @brief Construct a new LinearTestBase object.
    LinearTestBase() = default;
    //! @brief Destroy the LinearTestBase object.
    ~LinearTestBase() override = default;

    //! @brief Set up the test case.
    static void SetUpTestCase()
    {
        TST_DS_PRINT_TASK_TITLE("LINEAR", "BEGIN");
        structure = std::make_shared<date_structure::linear::LinearStructure>();
    };
    //! @brief Tear down the test case.
    static void TearDownTestCase()
    {
        TST_DS_PRINT_TASK_TITLE("LINEAR", "END");
        structure.reset();
    }
    //! @brief Set up.
    void SetUp() override{};
    //! @brief Tear down.
    void TearDown() override{};

    //! @brief Structure Instance.
    static std::shared_ptr<date_structure::linear::LinearStructure> structure;
};

std::shared_ptr<date_structure::linear::LinearStructure> LinearTestBase::structure = nullptr;

//! @brief Test for the linked list instance in the structure of linear.
TEST_F(LinearTestBase, linkedListInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(structure->linkedListInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

//! @brief Test for the stack instance in the structure of linear.
TEST_F(LinearTestBase, stackInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(structure->stackInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

//! @brief Test for the queue instance in the structure of linear.
TEST_F(LinearTestBase, queueInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(structure->queueInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

//! @brief Test base of tree.
class TreeTestBase : public ::testing::Test
{
public:
    //! @brief Construct a new TreeTestBase object.
    TreeTestBase() = default;
    //! @brief Destroy the TreeTestBase object.
    ~TreeTestBase() override = default;

    //! @brief Set up the test case.
    static void SetUpTestCase()
    {
        TST_DS_PRINT_TASK_TITLE("TREE", "BEGIN");
        structure = std::make_shared<date_structure::tree::TreeStructure>();
    };
    //! @brief Tear down the test case.
    static void TearDownTestCase()
    {
        TST_DS_PRINT_TASK_TITLE("TREE", "END");
        structure.reset();
    }
    //! @brief Set up.
    void SetUp() override{};
    //! @brief Tear down.
    void TearDown() override{};

    //! @brief Structure Instance.
    static std::shared_ptr<date_structure::tree::TreeStructure> structure;
};

std::shared_ptr<date_structure::tree::TreeStructure> TreeTestBase::structure = nullptr;

//! @brief Test for the binary search instance in the structure of tree.
TEST_F(TreeTestBase, bsInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(structure->bsInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

//! @brief Test for the Adelson-Velsky-Landis instance in the structure of tree.
TEST_F(TreeTestBase, avlInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(structure->avlInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

//! @brief Test for the splay instance in the structure of tree.
TEST_F(TreeTestBase, splayInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(structure->splayInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}
} // namespace test::tst_ds
