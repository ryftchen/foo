//! @file test_data_structure.cpp
//! @author ryftchen
//! @brief The definitions (test_data_structure) in the test module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen. All rights reserved.

#include <gtest/gtest.h>
#include "application/example/include/apply_data_structure.hpp"
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
    static void SetUpTestCase() { TST_DS_PRINT_TASK_TITLE("LINEAR", "BEGIN"); };
    //! @brief Tear down the test case.
    static void TearDownTestCase() { TST_DS_PRINT_TASK_TITLE("LINEAR", "END"); }
    //! @brief Set up.
    void SetUp() override{};
    //! @brief Tear down.
    void TearDown() override{};
};

//! @brief Test for the linked list instance in the structure of linear.
TEST_F(LinearTestBase, linkedListInstance) // NOLINT(cert-err58-cpp)
{
    // NOLINTNEXTLINE(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
    ASSERT_NO_THROW(application::app_ds::linear::Linear::linkedList());
}

//! @brief Test for the stack instance in the structure of linear.
TEST_F(LinearTestBase, stackInstance) // NOLINT(cert-err58-cpp)
{
    // NOLINTNEXTLINE(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
    ASSERT_NO_THROW(application::app_ds::linear::Linear::stack());
}

//! @brief Test for the queue instance in the structure of linear.
TEST_F(LinearTestBase, queueInstance) // NOLINT(cert-err58-cpp)
{
    // NOLINTNEXTLINE(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
    ASSERT_NO_THROW(application::app_ds::linear::Linear::queue());
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
    static void SetUpTestCase() { TST_DS_PRINT_TASK_TITLE("TREE", "BEGIN"); };
    //! @brief Tear down the test case.
    static void TearDownTestCase() { TST_DS_PRINT_TASK_TITLE("TREE", "END"); }
    //! @brief Set up.
    void SetUp() override{};
    //! @brief Tear down.
    void TearDown() override{};
};

//! @brief Test for the binary search instance in the structure of tree.
TEST_F(TreeTestBase, bsInstance) // NOLINT(cert-err58-cpp)
{
    // NOLINTNEXTLINE(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
    ASSERT_NO_THROW(application::app_ds::tree::Tree::bs());
}

//! @brief Test for the Adelson-Velsky-Landis instance in the structure of tree.
TEST_F(TreeTestBase, avlInstance) // NOLINT(cert-err58-cpp)
{
    // NOLINTNEXTLINE(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
    ASSERT_NO_THROW(application::app_ds::tree::Tree::avl());
}

//! @brief Test for the splay instance in the structure of tree.
TEST_F(TreeTestBase, splayInstance) // NOLINT(cert-err58-cpp)
{
    // NOLINTNEXTLINE(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
    ASSERT_NO_THROW(application::app_ds::tree::Tree::splay());
}
} // namespace test::tst_ds
