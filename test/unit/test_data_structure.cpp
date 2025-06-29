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

    //! @brief System under test.
    const linear::Showcase sut{};
    // clang-format off
    //! @brief Expected result 1.
    static constexpr std::string_view expRes1
    {
        "insert (0): {65, foo}\n"
        "insert (0): {66, bar}\n"
        "insert (1): {67, baz}\n"
        "remove (2)\n"
        "insert first: {65, foo}\n"
        "insert last: {68, qux}\n"
        "get first: {65, foo}\n"
        "get last: {68, qux}\n"
        "remove first\n"
        "remove last\n"
        "insert (1): {65, foo}\n"
        "whether it is empty: false\n"
        "size: 3\n"
        "linear details: HEAD -> {66, bar} <-> {65, foo} <-> {67, baz} -> NULL\n"
    };
    //! @brief Expected result 2.
    static constexpr std::string_view expRes2
    {
        "push: {65, foo}\n"
        "push: {66, bar}\n"
        "push: {67, baz}\n"
        "push: {68, qux}\n"
        "pop: {68, qux}\n"
        "top: {67, baz}\n"
        "push: {68, qux}\n"
        "whether it is empty: false\n"
        "size: 4\n"
        "linear details: TOP [{68, qux}, {67, baz}, {66, bar}, {65, foo}] BOTTOM\n"
    };
    //! @brief Expected result 3.
    static constexpr std::string_view expRes3
    {
        "push: {65, foo}\n"
        "push: {66, bar}\n"
        "push: {67, baz}\n"
        "push: {68, qux}\n"
        "pop: {65, foo}\n"
        "front: {66, bar}\n"
        "push: {65, foo}\n"
        "whether it is empty: false\n"
        "size: 4\n"
        "linear details: FRONT [{66, bar}, {67, baz}, {68, qux}, {65, foo}] REAR\n"
    };
    // clang-format on
};

//! @brief Test for the linked list instance in the structure of linear.
TEST_F(LinearTestBase, linkedListInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.linkedList());
    ASSERT_EQ(expRes1, result.str());
}

//! @brief Test for the stack instance in the structure of linear.
TEST_F(LinearTestBase, stackInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.stack());
    ASSERT_EQ(expRes2, result.str());
}

//! @brief Test for the queue instance in the structure of linear.
TEST_F(LinearTestBase, queueInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.queue());
    ASSERT_EQ(expRes3, result.str());
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

    //! @brief System under test.
    const tree::Showcase sut{};
    // clang-format off
    //! @brief Expected result 1.
    static constexpr std::string_view expRes1
    {
        "insert: 1, 5, 4, 3, 2, 6\n"
        "pre-order traversal: 1 ... 5 ... 4 ... 3 ... 2 ... 6 ... \n"
        "in-order traversal: 1 ... 2 ... 3 ... 4 ... 5 ... 6 ... \n"
        "post-order traversal: 2 ... 3 ... 4 ... 6 ... 5 ... 1 ... \n"
        "minimum: 1\n"
        "maximum: 6\n"
        "tree details:\n"
        "+ 1 -> root\n"
        "+   5 -> 1's right child\n"
        "+     4 -> 5's left child\n"
        "+       3 -> 4's left child\n"
        "+         2 -> 3's left child\n"
        "+     6 -> 5's right child\n"
        "delete root node: 3\n"
        "in-order traversal: 1 ... 2 ... 4 ... 5 ... 6 ... \n"
        "tree details:\n"
        "+ 1 -> root\n"
        "+   5 -> 1's right child\n"
        "+     4 -> 5's left child\n"
        "+       2 -> 4's left child\n"
        "+     6 -> 5's right child\n"
    };
    //! @brief Expected result 2.
    static constexpr std::string_view expRes2
    {
        "height: 0\n"
        "insert: 3, 2, 1, 4, 5, 6, 7, 16, 15, 14, 13, 12, 11, 10, 8, 9\n"
        "pre-order traversal: 7 ... 4 ... 2 ... 1 ... 3 ... 6 ... 5 ... 13 ... 11 ... 9 ... 8 ... 10 ... 12 ... 15 ... 14 ... 16 ... \n"
        "in-order traversal: 1 ... 2 ... 3 ... 4 ... 5 ... 6 ... 7 ... 8 ... 9 ... 10 ... 11 ... 12 ... 13 ... 14 ... 15 ... 16 ... \n"
        "post-order traversal: 1 ... 3 ... 2 ... 5 ... 6 ... 4 ... 8 ... 10 ... 9 ... 12 ... 11 ... 14 ... 16 ... 15 ... 13 ... 7 ... \n"
        "height: 5\n"
        "minimum: 1\n"
        "maximum: 16\n"
        "tree details:\n"
        "+ 7 -> root\n"
        "+   4 -> 7's left child\n"
        "+     2 -> 4's left child\n"
        "+       1 -> 2's left child\n"
        "+       3 -> 2's right child\n"
        "+     6 -> 4's right child\n"
        "+       5 -> 6's left child\n"
        "+   13 -> 7's right child\n"
        "+     11 -> 13's left child\n"
        "+       9 -> 11's left child\n"
        "+         8 -> 9's left child\n"
        "+         10 -> 9's right child\n"
        "+       12 -> 11's right child\n"
        "+     15 -> 13's right child\n"
        "+       14 -> 15's left child\n"
        "+       16 -> 15's right child\n"
        "delete root node: 8\n"
        "height: 5\n"
        "in-order traversal: 1 ... 2 ... 3 ... 4 ... 5 ... 6 ... 7 ... 9 ... 10 ... 11 ... 12 ... 13 ... 14 ... 15 ... 16 ... \n"
        "tree details:\n"
        "+ 7 -> root\n"
        "+   4 -> 7's left child\n"
        "+     2 -> 4's left child\n"
        "+       1 -> 2's left child\n"
        "+       3 -> 2's right child\n"
        "+     6 -> 4's right child\n"
        "+       5 -> 6's left child\n"
        "+   13 -> 7's right child\n"
        "+     11 -> 13's left child\n"
        "+       9 -> 11's left child\n"
        "+         10 -> 9's right child\n"
        "+       12 -> 11's right child\n"
        "+     15 -> 13's right child\n"
        "+       14 -> 15's left child\n"
        "+       16 -> 15's right child\n"
    };
    //! @brief Expected result 3.
    static constexpr std::string_view expRes3
    {
        "insert: 10, 50, 40, 70, 30, 20, 60\n"
        "pre-order traversal: 60 ... 30 ... 20 ... 10 ... 50 ... 40 ... 70 ... \n"
        "in-order traversal: 10 ... 20 ... 30 ... 40 ... 50 ... 60 ... 70 ... \n"
        "post-order traversal: 10 ... 20 ... 40 ... 50 ... 30 ... 70 ... 60 ... \n"
        "minimum: 10\n"
        "maximum: 70\n"
        "tree details:\n"
        "+ 60 -> root\n"
        "+   30 -> 60's left child\n"
        "+     20 -> 30's left child\n"
        "+       10 -> 20's left child\n"
        "+     50 -> 30's right child\n"
        "+       40 -> 50's left child\n"
        "+   70 -> 60's right child\n"
        "delete root node: 70\n"
        "in-order traversal: 10 ... 20 ... 30 ... 40 ... 50 ... 60 ... \n"
        "tree details:\n"
        "+ 60 -> root\n"
        "+   30 -> 60's left child\n"
        "+     20 -> 30's left child\n"
        "+       10 -> 20's left child\n"
        "+     50 -> 30's right child\n"
        "+       40 -> 50's left child\n"
        "splay node as root node: 30\n"
        "tree details:\n"
        "+ 30 -> root\n"
        "+   20 -> 30's left child\n"
        "+     10 -> 20's left child\n"
        "+   60 -> 30's right child\n"
        "+     50 -> 60's left child\n"
        "+       40 -> 50's left child\n"
    };
    // clang-format on
};

//! @brief Test for the binary search instance in the structure of tree.
TEST_F(TreeTestBase, bsInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.bs());
    ASSERT_EQ(expRes1, result.str());
}

//! @brief Test for the Adelson-Velsky-Landis instance in the structure of tree.
TEST_F(TreeTestBase, avlInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.avl());
    ASSERT_EQ(expRes2, result.str());
}

//! @brief Test for the splay instance in the structure of tree.
TEST_F(TreeTestBase, splayInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.splay());
    ASSERT_EQ(expRes3, result.str());
}
} // namespace tst_ds
} // namespace test

#undef TST_DS_PRINT_TASK_TITLE
