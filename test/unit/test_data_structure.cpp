//! @file test_data_structure.cpp
//! @author ryftchen
//! @brief The definitions (test_data_structure) in the test module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include <gtest/gtest.h>
#include <syncstream>

#include "application/example/include/apply_data_structure.hpp"

//! @brief Title of printing for data structure task tests.
#define TST_DS_PRINT_TASK_TITLE(category, state)                                                    \
    std::osyncstream(std::cout) << "TEST DATA STRUCTURE: " << std::setiosflags(std::ios_base::left) \
                                << std::setfill('.') << std::setw(50) << (category) << (state)      \
                                << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl;

//! @brief The test module.
namespace test // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Data-structure-testing-related functions in the test module.
namespace tst_ds
{
using namespace application::app_ds; // NOLINT(google-build-using-namespace)

//! @brief Test base of linear.
class LinearTestBase : public ::testing::Test
{
public:
    //! @brief Construct a new LinearTestBase object.
    LinearTestBase() = default;
    //! @brief Destroy the LinearTestBase object.
    ~LinearTestBase() override = default;

protected:
    //! @brief Set up the test case.
    static void SetUpTestSuite() { TST_DS_PRINT_TASK_TITLE("LINEAR", "BEGIN"); }
    //! @brief Tear down the test case.
    static void TearDownTestSuite() { TST_DS_PRINT_TASK_TITLE("LINEAR", "END"); }
    //! @brief Set up.
    void SetUp() override {}
    //! @brief Tear down.
    void TearDown() override {}
};

//! @brief Test for the linked list instance in the structure of linear.
TEST_F(LinearTestBase, linkedListInstance)
{
    ASSERT_NO_THROW(linear::Linear::linkedList());
}

//! @brief Test for the stack instance in the structure of linear.
TEST_F(LinearTestBase, stackInstance)
{
    ASSERT_NO_THROW(linear::Linear::stack());
}

//! @brief Test for the queue instance in the structure of linear.
TEST_F(LinearTestBase, queueInstance)
{
    ASSERT_NO_THROW(linear::Linear::queue());
}

//! @brief Test base of tree.
class TreeTestBase : public ::testing::Test
{
public:
    //! @brief Construct a new TreeTestBase object.
    TreeTestBase() = default;
    //! @brief Destroy the TreeTestBase object.
    ~TreeTestBase() override = default;

protected:
    //! @brief Set up the test case.
    static void SetUpTestSuite() { TST_DS_PRINT_TASK_TITLE("TREE", "BEGIN"); }
    //! @brief Tear down the test case.
    static void TearDownTestSuite() { TST_DS_PRINT_TASK_TITLE("TREE", "END"); }
    //! @brief Set up.
    void SetUp() override {}
    //! @brief Tear down.
    void TearDown() override {}
};

//! @brief Test for the binary search instance in the structure of tree.
TEST_F(TreeTestBase, bsInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = tree::Tree::bs());
    ASSERT_TRUE(!result.str().empty());
}

//! @brief Test for the Adelson-Velsky-Landis instance in the structure of tree.
TEST_F(TreeTestBase, avlInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = tree::Tree::avl());
    ASSERT_TRUE(!result.str().empty());
}

//! @brief Test for the splay instance in the structure of tree.
TEST_F(TreeTestBase, splayInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = tree::Tree::splay());
    ASSERT_TRUE(!result.str().empty());
}
} // namespace tst_ds
} // namespace test
