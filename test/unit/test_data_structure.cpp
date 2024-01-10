//! @file test_data_structure.cpp
//! @author ryftchen
//! @brief The definitions (test_data_structure) in the test module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#include <gtest/gtest.h>
#include <syncstream>

#include "application/example/include/apply_data_structure.hpp"

//! @brief Title of printing for data structure task tests.
#define TST_DS_PRINT_TASK_TITLE(category, state)                                                    \
    std::osyncstream(std::cout) << "TEST DATA STRUCTURE: " << std::setiosflags(std::ios_base::left) \
                                << std::setfill('.') << std::setw(50) << category << state          \
                                << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl;

//! @brief The test module.
namespace test // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Data-structure-testing-related functions in the test module.
namespace tst_ds
{
using namespace application::app_ds; // NOLINT(google-build-using-namespace)
//! @brief Alias for the enumeration of data structure tasks.
using Category = DataStructureTask::Category;

//! @brief Test base of linear.
class LinearTestBase : public ::testing::Test
{
public:
    //! @brief Construct a new LinearTestBase object.
    LinearTestBase() = default;
    //! @brief Destroy the LinearTestBase object.
    ~LinearTestBase() override = default;

    //! @brief Set up the test case.
    static void SetUpTestCase() { TST_DS_PRINT_TASK_TITLE(Category::linear, "BEGIN"); }
    //! @brief Tear down the test case.
    static void TearDownTestCase() { TST_DS_PRINT_TASK_TITLE(Category::linear, "END"); }
    //! @brief Set up.
    void SetUp() override{};
    //! @brief Tear down.
    void TearDown() override{};
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

    //! @brief Set up the test case.
    static void SetUpTestCase() { TST_DS_PRINT_TASK_TITLE(Category::tree, "BEGIN"); }
    //! @brief Tear down the test case.
    static void TearDownTestCase() { TST_DS_PRINT_TASK_TITLE(Category::tree, "END"); }
    //! @brief Set up.
    void SetUp() override{};
    //! @brief Tear down.
    void TearDown() override{};
};

//! @brief Test for the binary search instance in the structure of tree.
TEST_F(TreeTestBase, bsInstance)
{
    ASSERT_NO_THROW(tree::Tree::bs());
}

//! @brief Test for the Adelson-Velsky-Landis instance in the structure of tree.
TEST_F(TreeTestBase, avlInstance)
{
    ASSERT_NO_THROW(tree::Tree::avl());
}

//! @brief Test for the splay instance in the structure of tree.
TEST_F(TreeTestBase, splayInstance)
{
    ASSERT_NO_THROW(tree::Tree::splay());
}
} // namespace tst_ds
} // namespace test
