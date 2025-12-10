//! @file test_data_structure.cpp
//! @author ryftchen
//! @brief The definitions (test_data_structure) in the test module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include <gtest/gtest.h>
#include <syncstream>

#include "application/example/include/apply_data_structure.hpp"

//! @brief The test module.
namespace test // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Data-structure-testing-related functions in the test module.
namespace tst_ds
{
//! @brief Print the progress of the data structure task tests.
//! @param title - task title
//! @param state - task state
//! @param align - alignment width
static void printTaskProgress(const std::string_view title, const std::string_view state, const std::uint8_t align = 50)
{
    std::osyncstream(std::cout) << "TEST DATA STRUCTURE: " << std::setiosflags(std::ios_base::left) << std::setfill('.')
                                << std::setw(align) << title << state << std::resetiosflags(std::ios_base::left)
                                << std::setfill(' ') << std::endl;
}

//! @brief Test base of cache.
class CacheTestBase : public ::testing::Test
{
protected:
    //! @brief Alias for the showcase.
    using Showcase = application::app_ds::cache::Showcase;
    //! @brief Set up the test case.
    static void SetUpTestSuite() { printTaskProgress(title, "BEGIN"); }
    //! @brief Tear down the test case.
    static void TearDownTestSuite() { printTaskProgress(title, "END"); }

    //! @brief Test title.
    inline static const std::string_view title{data_structure::cache::description()};
    //! @brief System under test.
    [[no_unique_address]] const Showcase sut{};
    // clang-format off
    //! @brief Expected result 1.
    static constexpr std::string_view expRes1
    {
        "insert {A: foo}\n"
        "insert {B: bar}\n"
        "insert {C: baz}\n"
        "find A: true\n"
        "find B: true\n"
        "find B: true\n"
        "find C: true\n"
        "find A: true\n"
        "erase D: false\n"
        "insert {D: qux}\n"
        "find range {A, B, C, D}: {A: ---}, {B: bar}, {C: baz}, {D: qux}\n"
        "erase range {B, C}: 2\n"
        "resolve range {A: ---}, {B: ---}, {C: ---}, {D: ---}: {A: ---}, {B: ---}, {C: ---}, {D: qux}\n"
        "insert range {A: foo}, {B: bar}, {C: baz}, {D: qux}\n"
        "whether it is empty: false\n"
        "size: 3\n"
        "capacity: 3\n"
        "current status: {A: ---}, {B: bar}, {C: baz}, {D: qux}\n"
    };
    //! @brief Expected result 2.
    static constexpr std::string_view expRes2
    {
        "insert {A: foo}\n"
        "insert {B: bar}\n"
        "insert {C: baz}\n"
        "find A: true\n"
        "find B: true\n"
        "find B: true\n"
        "find C: true\n"
        "find A: true\n"
        "erase D: false\n"
        "insert {D: qux}\n"
        "find range {A, B, C, D}: {A: foo}, {B: bar}, {C: ---}, {D: qux}\n"
        "erase range {B, C}: 1\n"
        "resolve range {A: ---}, {B: ---}, {C: ---}, {D: ---}: {A: foo}, {B: ---}, {C: ---}, {D: qux}\n"
        "insert range {A: foo}, {B: bar}, {C: baz}, {D: qux}\n"
        "whether it is empty: false\n"
        "size: 3\n"
        "capacity: 3\n"
        "current status: {A: foo}, {B: ---}, {C: baz}, {D: qux}\n"
    };
    //! @brief Expected result 3.
    static constexpr std::string_view expRes3
    {
        "insert {A: foo}\n"
        "insert {B: bar}\n"
        "insert {C: baz}\n"
        "find A: true\n"
        "find B: true\n"
        "find B: true\n"
        "find C: true\n"
        "find A: true\n"
        "erase D: false\n"
        "insert {D: qux}\n"
        "find range {A, B, C, D}: {A: foo}, {B: ---}, {C: baz}, {D: qux}\n"
        "erase range {B, C}: 1\n"
        "resolve range {A: ---}, {B: ---}, {C: ---}, {D: ---}: {A: foo}, {B: ---}, {C: ---}, {D: qux}\n"
        "insert range {A: foo}, {B: bar}, {C: baz}, {D: qux}\n"
        "whether it is empty: false\n"
        "size: 3\n"
        "capacity: 3\n"
        "current status: {A: ---}, {B: bar}, {C: baz}, {D: qux}\n"
    };
    // clang-format on
};

//! @brief Test for the first in first out instance in the structure of cache.
TEST_F(CacheTestBase, FIFOInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.fifo());
    ASSERT_EQ(expRes1, result.str());
}

//! @brief Test for the least frequently used instance in the structure of cache.
TEST_F(CacheTestBase, LFUInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.lfu());
    ASSERT_EQ(expRes2, result.str());
}

//! @brief Test for the least recently used instance in the structure of cache.
TEST_F(CacheTestBase, LRUInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.lru());
    ASSERT_EQ(expRes3, result.str());
}

//! @brief Test base of filter.
class FilterTestBase : public ::testing::Test
{
protected:
    //! @brief Alias for the showcase.
    using Showcase = application::app_ds::filter::Showcase;
    //! @brief Set up the test case.
    static void SetUpTestSuite() { printTaskProgress(title, "BEGIN"); }
    //! @brief Tear down the test case.
    static void TearDownTestSuite() { printTaskProgress(title, "END"); }

    //! @brief Test title.
    inline static const std::string_view title{data_structure::filter::description()};
    //! @brief System under test.
    [[no_unique_address]] const Showcase sut{};
    // clang-format off
    //! @brief Expected result 1.
    static constexpr std::string_view expRes1
    {
        "insert {foo://bar/0/baz.qux ... foo://bar/999/baz.qux}: 1000\n"
        "may contain {foo://bar/0/baz.qux ... foo://bar/999/baz.qux}: 1000\n"
        "may not contain {foo://bar/1000/baz.qux ... foo://bar/1999/baz.qux}: 1000\n"
    };
    //! @brief Expected result 2.
    static constexpr std::string_view expRes2
    {
        "A insert {foo://bar/0/baz.qux ... foo://bar/499/baz.qux}: 500\n"
        "B insert {foo://bar/500/baz.qux ... foo://bar/999/baz.qux}: 500\n"
        "C merge A and B: true\n"
        "C remove {foo://bar/500/baz.qux ... foo://bar/999/baz.qux}: 500\n"
        "C may contain {foo://bar/0/baz.qux ... foo://bar/499/baz.qux}: 500\n"
        "C may not contain {foo://bar/500/baz.qux ... foo://bar/999/baz.qux}: 500\n"
    };
    // clang-format on
};

//! @brief Test for the Bloom instance in the structure of filter.
TEST_F(FilterTestBase, BloomInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.bloom());
    ASSERT_EQ(expRes1, result.str());
}

//! @brief Test for the quotient instance in the structure of filter.
TEST_F(FilterTestBase, QuotientInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.quotient());
    ASSERT_EQ(expRes2, result.str());
}

//! @brief Test base of graph.
class GraphTestBase : public ::testing::Test
{
protected:
    //! @brief Alias for the showcase.
    using Showcase = application::app_ds::graph::Showcase;
    //! @brief Set up the test case.
    static void SetUpTestSuite() { printTaskProgress(title, "BEGIN"); }
    //! @brief Tear down the test case.
    static void TearDownTestSuite() { printTaskProgress(title, "END"); }

    //! @brief Test title.
    inline static const std::string_view title{data_structure::graph::description()};
    //! @brief System under test.
    [[no_unique_address]] const Showcase sut{};
    // clang-format off
    //! @brief Expected result 1.
    static constexpr std::string_view expRes1
    {
        "add vertex A: true\n"
        "add vertex B: true\n"
        "add vertex C: true\n"
        "add vertex D: true\n"
        "add vertex E: true\n"
        "add vertex F: true\n"
        "add vertex G: true\n"
        "add edge A-C: true\n"
        "add edge A-D: true\n"
        "add edge A-F: true\n"
        "add edge B-C: true\n"
        "add edge C-D: true\n"
        "add edge E-G: true\n"
        "add edge F-G: true\n"
        "DFS traversal from A: A C B D F G E \n"
        "BFS traversal from A: A C D F B G E \n"
        "delete edge A-D: true\n"
        "delete vertex A: true\n"
        "DFS traversal from B: B C D \n"
        "BFS traversal from B: B C D \n"
        "DFS traversal from F: F G E \n"
        "BFS traversal from F: F G E \n"
    };
    //! @brief Expected result 2.
    static constexpr std::string_view expRes2
    {
        "add vertex A: true\n"
        "add vertex B: true\n"
        "add vertex C: true\n"
        "add vertex D: true\n"
        "add vertex E: true\n"
        "add vertex F: true\n"
        "add vertex G: true\n"
        "add arc A-B: true\n"
        "add arc B-C: true\n"
        "add arc B-E: true\n"
        "add arc B-F: true\n"
        "add arc C-E: true\n"
        "add arc D-C: true\n"
        "add arc E-B: true\n"
        "add arc E-D: true\n"
        "add arc F-G: true\n"
        "DFS traversal from A: A B C E D F G \n"
        "BFS traversal from A: A B C E F D G \n"
        "delete arc E-B: true\n"
        "delete vertex B: true\n"
        "DFS traversal from A: A \n"
        "BFS traversal from A: A \n"
        "DFS traversal from C: C E D \n"
        "BFS traversal from C: C E D \n"
        "DFS traversal from F: F G \n"
        "BFS traversal from F: F G \n"
    };
    // clang-format on
};

//! @brief Test for the undirected instance in the structure of graph.
TEST_F(GraphTestBase, UndirectedInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.undirected());
    ASSERT_EQ(expRes1, result.str());
}

//! @brief Test for the directed instance in the structure of graph.
TEST_F(GraphTestBase, DirectedInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.directed());
    ASSERT_EQ(expRes2, result.str());
}

//! @brief Test base of heap.
class HeapTestBase : public ::testing::Test
{
protected:
    //! @brief Alias for the showcase.
    using Showcase = application::app_ds::heap::Showcase;
    //! @brief Set up the test case.
    static void SetUpTestSuite() { printTaskProgress(title, "BEGIN"); }
    //! @brief Tear down the test case.
    static void TearDownTestSuite() { printTaskProgress(title, "END"); }

    //! @brief Test title.
    inline static const std::string_view title{data_structure::heap::description()};
    //! @brief System under test.
    [[no_unique_address]] const Showcase sut{};
    // clang-format off
    //! @brief Expected result 1.
    static constexpr std::string_view expRes1
    {
        "insert 80, 40, 30, 60, 90, 70, 10, 50, 20\n"
        "traversal: 10 ... 20 ... 30 ... 50 ... 90 ... 70 ... 40 ... 80 ... 60 ... \n"
        "insert 15: true\n"
        "traversal: 10 ... 15 ... 30 ... 50 ... 20 ... 70 ... 40 ... 80 ... 60 ... 90 ... \n"
        "remove 10: true\n"
        "traversal: 15 ... 20 ... 30 ... 50 ... 90 ... 70 ... 40 ... 80 ... 60 ... \n"
    };
    //! @brief Expected result 2.
    static constexpr std::string_view expRes2
    {
        "A insert 10, 40, 24, 30, 36, 20, 12, 16\n"
        "A all details:\n"
        "+ 10(2) -> root\n"
        "+   24(1) -> 10's left child\n"
        "+     30(0) -> 24's left child\n"
        "+     36(0) -> 24's right child\n"
        "+   12(1) -> 10's right child\n"
        "+     20(0) -> 12's left child\n"
        "+       40(0) -> 20's left child\n"
        "+     16(0) -> 12's right child\n"
        "B insert 17, 13, 11, 15, 19, 21, 23\n"
        "B all details:\n"
        "+ 11(2) -> root\n"
        "+   15(1) -> 11's left child\n"
        "+     19(0) -> 15's left child\n"
        "+     21(0) -> 15's right child\n"
        "+   13(1) -> 11's right child\n"
        "+     17(0) -> 13's left child\n"
        "+     23(0) -> 13's right child\n"
        "A merge B\n"
        "A pre-order traversal: 10 ... 11 ... 15 ... 19 ... 21 ... 12 ... 13 ... 17 ... 16 ... 23 ... 20 ... 40 ... 24 ... 30 ... 36 ... \n"
        "A in-order traversal: 19 ... 15 ... 21 ... 11 ... 17 ... 13 ... 23 ... 16 ... 12 ... 40 ... 20 ... 10 ... 30 ... 24 ... 36 ... \n"
        "A post-order traversal: 19 ... 21 ... 15 ... 17 ... 23 ... 16 ... 13 ... 40 ... 20 ... 12 ... 11 ... 30 ... 36 ... 24 ... 10 ... \n"
        "A minimum: 10\n"
        "A remove\n"
        "A all details:\n"
        "+ 11(2) -> root\n"
        "+   12(2) -> 11's left child\n"
        "+     13(1) -> 12's left child\n"
        "+       17(0) -> 13's left child\n"
        "+       16(0) -> 13's right child\n"
        "+         23(0) -> 16's left child\n"
        "+     20(1) -> 12's right child\n"
        "+       24(1) -> 20's left child\n"
        "+         30(0) -> 24's left child\n"
        "+         36(0) -> 24's right child\n"
        "+       40(0) -> 20's right child\n"
        "+   15(1) -> 11's right child\n"
        "+     19(0) -> 15's left child\n"
        "+     21(0) -> 15's right child\n"
    };
    //! @brief Expected result 3.
    static constexpr std::string_view expRes3
    {
        "A insert 10, 40, 24, 30, 36, 20, 12, 16\n"
        "A all details:\n"
        "+ 10 -> root\n"
        "+   16 -> 10's left child\n"
        "+     20 -> 16's left child\n"
        "+       30 -> 20's left child\n"
        "+         40 -> 30's left child\n"
        "+   12 -> 10's right child\n"
        "+     24 -> 12's left child\n"
        "+       36 -> 24's left child\n"
        "B insert 17, 13, 11, 15, 19, 21, 23\n"
        "B all details:\n"
        "+ 11 -> root\n"
        "+   13 -> 11's left child\n"
        "+     17 -> 13's left child\n"
        "+       23 -> 17's left child\n"
        "+     19 -> 13's right child\n"
        "+   15 -> 11's right child\n"
        "+     21 -> 15's left child\n"
        "A merge B\n"
        "A pre-order traversal: 10 ... 11 ... 12 ... 15 ... 21 ... 24 ... 36 ... 13 ... 17 ... 23 ... 19 ... 16 ... 20 ... 30 ... 40 ... \n"
        "A in-order traversal: 21 ... 15 ... 12 ... 36 ... 24 ... 11 ... 23 ... 17 ... 13 ... 19 ... 10 ... 40 ... 30 ... 20 ... 16 ... \n"
        "A post-order traversal: 21 ... 15 ... 36 ... 24 ... 12 ... 23 ... 17 ... 19 ... 13 ... 11 ... 40 ... 30 ... 20 ... 16 ... 10 ... \n"
        "A minimum: 10\n"
        "A remove\n"
        "A all details:\n"
        "+ 11 -> root\n"
        "+   13 -> 11's left child\n"
        "+     16 -> 13's left child\n"
        "+       19 -> 16's left child\n"
        "+       20 -> 16's right child\n"
        "+         30 -> 20's left child\n"
        "+           40 -> 30's left child\n"
        "+     17 -> 13's right child\n"
        "+       23 -> 17's left child\n"
        "+   12 -> 11's right child\n"
        "+     15 -> 12's left child\n"
        "+       21 -> 15's left child\n"
        "+     24 -> 12's right child\n"
        "+       36 -> 24's left child\n"
    };
    // clang-format on
};

//! @brief Test for the binary instance in the structure of heap.
TEST_F(HeapTestBase, BinaryInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.binary());
    ASSERT_EQ(expRes1, result.str());
}

//! @brief Test for the leftist instance in the structure of heap.
TEST_F(HeapTestBase, LeftistInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.leftist());
    ASSERT_EQ(expRes2, result.str());
}

//! @brief Test for the skew instance in the structure of heap.
TEST_F(HeapTestBase, SkewInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.skew());
    ASSERT_EQ(expRes3, result.str());
}

//! @brief Test base of linear.
class LinearTestBase : public ::testing::Test
{
protected:
    //! @brief Alias for the showcase.
    using Showcase = application::app_ds::linear::Showcase;
    //! @brief Set up the test case.
    static void SetUpTestSuite() { printTaskProgress(title, "BEGIN"); }
    //! @brief Tear down the test case.
    static void TearDownTestSuite() { printTaskProgress(title, "END"); }

    //! @brief Test title.
    inline static const std::string_view title{data_structure::linear::description()};
    //! @brief System under test.
    [[no_unique_address]] const Showcase sut{};
    // clang-format off
    //! @brief Expected result 1.
    static constexpr std::string_view expRes1
    {
        "insert (0) 97: true\n"
        "insert (0) 98: true\n"
        "insert (0) 99: true\n"
        "insert (0) 100: true\n"
        "traversal: 100 ... 99 ... 98 ... 97 ... \n"
        "remove (1): true\n"
        "insert (2) 99: true\n"
        "insert first 97: true\n"
        "insert last 100: true\n"
        "get first: 97\n"
        "get last: 100\n"
        "remove first: true\n"
        "remove last: true\n"
        "whether it is empty: false\n"
        "size: 4\n"
        "all details: HEAD -> 100 <-> 98 <-> 99 <-> 97 <- TAIL\n"
    };
    //! @brief Expected result 2.
    static constexpr std::string_view expRes2
    {
        "push 97: true\n"
        "push 98: true\n"
        "push 99: true\n"
        "push 100: true\n"
        "traversal: 100 ... 99 ... 98 ... 97 ... \n"
        "pop: 100\n"
        "top: 99\n"
        "push 100: true\n"
        "whether it is empty: false\n"
        "size: 4\n"
        "all details: TOP [ 100 | 99 | 98 | 97 ] BOTTOM\n"
    };
    //! @brief Expected result 3.
    static constexpr std::string_view expRes3
    {
        "push 97: true\n"
        "push 98: true\n"
        "push 99: true\n"
        "push 100: true\n"
        "traversal: 97 ... 98 ... 99 ... 100 ... \n"
        "pop: 97\n"
        "front: 98\n"
        "push 97: true\n"
        "whether it is empty: false\n"
        "size: 4\n"
        "all details: FRONT [ 98 | 99 | 100 | 97 ] REAR\n"
    };
    // clang-format on
};

//! @brief Test for the doubly linked list instance in the structure of linear.
TEST_F(LinearTestBase, DLLInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.dll());
    ASSERT_EQ(expRes1, result.str());
}

//! @brief Test for the stack instance in the structure of linear.
TEST_F(LinearTestBase, StackInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.stack());
    ASSERT_EQ(expRes2, result.str());
}

//! @brief Test for the queue instance in the structure of linear.
TEST_F(LinearTestBase, QueueInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.queue());
    ASSERT_EQ(expRes3, result.str());
}

//! @brief Test base of tree.
class TreeTestBase : public ::testing::Test
{
protected:
    //! @brief Alias for the showcase.
    using Showcase = application::app_ds::tree::Showcase;
    //! @brief Set up the test case.
    static void SetUpTestSuite() { printTaskProgress(title, "BEGIN"); }
    //! @brief Tear down the test case.
    static void TearDownTestSuite() { printTaskProgress(title, "END"); }

    //! @brief Test title.
    inline static const std::string_view title{data_structure::tree::description()};
    //! @brief System under test.
    [[no_unique_address]] const Showcase sut{};
    // clang-format off
    //! @brief Expected result 1.
    static constexpr std::string_view expRes1
    {
        "insert 1, 5, 4, 3, 2, 6\n"
        "pre-order traversal: 1 ... 5 ... 4 ... 3 ... 2 ... 6 ... \n"
        "in-order traversal: 1 ... 2 ... 3 ... 4 ... 5 ... 6 ... \n"
        "post-order traversal: 2 ... 3 ... 4 ... 6 ... 5 ... 1 ... \n"
        "minimum: 1\n"
        "maximum: 6\n"
        "all details:\n"
        "+ 1 -> root\n"
        "+   5 -> 1's right child\n"
        "+     4 -> 5's left child\n"
        "+       3 -> 4's left child\n"
        "+         2 -> 3's left child\n"
        "+     6 -> 5's right child\n"
        "search 3: true\n"
        "predecessor of 3: 2\n"
        "successor of 3: 4\n"
        "remove 3\n"
        "in-order traversal: 1 ... 2 ... 4 ... 5 ... 6 ... \n"
        "all details:\n"
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
        "insert 3, 2, 1, 4, 5, 6, 7, 16, 15, 14, 13, 12, 11, 10, 8, 9\n"
        "pre-order traversal: 7 ... 4 ... 2 ... 1 ... 3 ... 6 ... 5 ... 13 ... 11 ... 9 ... 8 ... 10 ... 12 ... 15 ... 14 ... 16 ... \n"
        "in-order traversal: 1 ... 2 ... 3 ... 4 ... 5 ... 6 ... 7 ... 8 ... 9 ... 10 ... 11 ... 12 ... 13 ... 14 ... 15 ... 16 ... \n"
        "post-order traversal: 1 ... 3 ... 2 ... 5 ... 6 ... 4 ... 8 ... 10 ... 9 ... 12 ... 11 ... 14 ... 16 ... 15 ... 13 ... 7 ... \n"
        "height: 5\n"
        "minimum: 1\n"
        "maximum: 16\n"
        "all details:\n"
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
        "search 13: true\n"
        "remove 13\n"
        "height: 5\n"
        "in-order traversal: 1 ... 2 ... 3 ... 4 ... 5 ... 6 ... 7 ... 8 ... 9 ... 10 ... 11 ... 12 ... 14 ... 15 ... 16 ... \n"
        "all details:\n"
        "+ 7 -> root\n"
        "+   4 -> 7's left child\n"
        "+     2 -> 4's left child\n"
        "+       1 -> 2's left child\n"
        "+       3 -> 2's right child\n"
        "+     6 -> 4's right child\n"
        "+       5 -> 6's left child\n"
        "+   12 -> 7's right child\n"
        "+     9 -> 12's left child\n"
        "+       8 -> 9's left child\n"
        "+       11 -> 9's right child\n"
        "+         10 -> 11's left child\n"
        "+     15 -> 12's right child\n"
        "+       14 -> 15's left child\n"
        "+       16 -> 15's right child\n"
    };
    //! @brief Expected result 3.
    static constexpr std::string_view expRes3
    {
        "insert 10, 50, 40, 70, 30, 20, 60\n"
        "pre-order traversal: 60 ... 30 ... 20 ... 10 ... 50 ... 40 ... 70 ... \n"
        "in-order traversal: 10 ... 20 ... 30 ... 40 ... 50 ... 60 ... 70 ... \n"
        "post-order traversal: 10 ... 20 ... 40 ... 50 ... 30 ... 70 ... 60 ... \n"
        "minimum: 10\n"
        "maximum: 70\n"
        "all details:\n"
        "+ 60 -> root\n"
        "+   30 -> 60's left child\n"
        "+     20 -> 30's left child\n"
        "+       10 -> 20's left child\n"
        "+     50 -> 30's right child\n"
        "+       40 -> 50's left child\n"
        "+   70 -> 60's right child\n"
        "search 70: true\n"
        "remove 70\n"
        "in-order traversal: 10 ... 20 ... 30 ... 40 ... 50 ... 60 ... \n"
        "all details:\n"
        "+ 60 -> root\n"
        "+   30 -> 60's left child\n"
        "+     20 -> 30's left child\n"
        "+       10 -> 20's left child\n"
        "+     50 -> 30's right child\n"
        "+       40 -> 50's left child\n"
        "splay 30\n"
        "all details:\n"
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
TEST_F(TreeTestBase, BSInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.bs());
    ASSERT_EQ(expRes1, result.str());
}

//! @brief Test for the Adelson-Velsky-Landis instance in the structure of tree.
TEST_F(TreeTestBase, AVLInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.avl());
    ASSERT_EQ(expRes2, result.str());
}

//! @brief Test for the splay instance in the structure of tree.
TEST_F(TreeTestBase, SplayInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.splay());
    ASSERT_EQ(expRes3, result.str());
}
} // namespace tst_ds
} // namespace test
