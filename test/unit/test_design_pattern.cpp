//! @file test_design_pattern.cpp
//! @author ryftchen
//! @brief The definitions (test_design_pattern) in the test module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include <gtest/gtest.h>
#include <syncstream>

#include "application/example/include/apply_design_pattern.hpp"

//! @brief Title of printing for design pattern task tests.
#define TST_DP_PRINT_TASK_TITLE(category, state)                                                    \
    std::osyncstream(std::cout) << "TEST DESIGN PATTERN: " << std::setiosflags(std::ios_base::left) \
                                << std::setfill('.') << std::setw(50) << category << state          \
                                << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl;

//! @brief The test module.
namespace test // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Design-pattern-testing-related functions in the test module.
namespace tst_dp
{
using namespace application::app_dp; // NOLINT(google-build-using-namespace)

//! @brief Test base of behavioral.
class BehavioralTestBase : public ::testing::Test
{
public:
    //! @brief Construct a new BehavioralTestBase object.
    BehavioralTestBase() = default;
    //! @brief Destroy the BehavioralTestBase object.
    ~BehavioralTestBase() override = default;

    //! @brief Set up the test case.
    static void SetUpTestCase() { TST_DP_PRINT_TASK_TITLE("BEHAVIORAL", "BEGIN"); }
    //! @brief Tear down the test case.
    static void TearDownTestCase() { TST_DP_PRINT_TASK_TITLE("BEHAVIORAL", "END"); }
    //! @brief Set up.
    void SetUp() override {}
    //! @brief Tear down.
    void TearDown() override {}
};

//! @brief Test for the chain of responsibility instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, chainOfResponsibilityInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = behavioral::Behavioral::chainOfResponsibility());
    ASSERT_TRUE(!result.str().empty());
}

//! @brief Test for the command instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, commandInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = behavioral::Behavioral::command());
    ASSERT_TRUE(!result.str().empty());
}

//! @brief Test for the interpreter instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, interpreterInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = behavioral::Behavioral::interpreter());
    ASSERT_TRUE(!result.str().empty());
}

//! @brief Test for the iterator instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, iteratorInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = behavioral::Behavioral::iterator());
    ASSERT_TRUE(!result.str().empty());
}

//! @brief Test for the mediator instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, mediatorInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = behavioral::Behavioral::mediator());
    ASSERT_TRUE(!result.str().empty());
}

//! @brief Test for the memento instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, mementoInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = behavioral::Behavioral::memento());
    ASSERT_TRUE(!result.str().empty());
}

//! @brief Test for the observer instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, observerInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = behavioral::Behavioral::observer());
    ASSERT_TRUE(!result.str().empty());
}

//! @brief Test for the state instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, stateInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = behavioral::Behavioral::state());
    ASSERT_TRUE(!result.str().empty());
}

//! @brief Test for the strategy instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, strategyInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = behavioral::Behavioral::strategy());
    ASSERT_TRUE(!result.str().empty());
}

//! @brief Test for the template method instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, templateMethodInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = behavioral::Behavioral::templateMethod());
    ASSERT_TRUE(!result.str().empty());
}

//! @brief Test for the visitor instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, visitorInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = behavioral::Behavioral::visitor());
    ASSERT_TRUE(!result.str().empty());
}

//! @brief Test base of creational.
class CreationalTestBase : public ::testing::Test
{
public:
    //! @brief Construct a new CreationalTestBase object.
    CreationalTestBase() = default;
    //! @brief Destroy the CreationalTestBase object.
    ~CreationalTestBase() override = default;

    //! @brief Set up the test case.
    static void SetUpTestCase() { TST_DP_PRINT_TASK_TITLE("CREATIONAL", "BEGIN"); }
    //! @brief Tear down the test case.
    static void TearDownTestCase() { TST_DP_PRINT_TASK_TITLE("CREATIONAL", "END"); }
    //! @brief Set up.
    void SetUp() override {}
    //! @brief Tear down.
    void TearDown() override {}
};

//! @brief Test for the abstract factory instance in the pattern of creational.
TEST_F(CreationalTestBase, abstractFactoryInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = creational::Creational::abstractFactory());
    ASSERT_TRUE(!result.str().empty());
}

//! @brief Test for the builder instance in the pattern of creational.
TEST_F(CreationalTestBase, builderInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = creational::Creational::builder());
    ASSERT_TRUE(!result.str().empty());
}

//! @brief Test for the factory method instance in the pattern of creational.
TEST_F(CreationalTestBase, factoryMethodInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = creational::Creational::factoryMethod());
    ASSERT_TRUE(!result.str().empty());
}

//! @brief Test for the prototype instance in the pattern of creational.
TEST_F(CreationalTestBase, prototypeInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = creational::Creational::prototype());
    ASSERT_TRUE(!result.str().empty());
}

//! @brief Test for the singleton instance in the pattern of creational.
TEST_F(CreationalTestBase, singletonInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = creational::Creational::singleton());
    ASSERT_TRUE(!result.str().empty());
}

//! @brief Test base of structural.
class StructuralTestBase : public ::testing::Test
{
public:
    //! @brief Construct a new StructuralTestBase object.
    StructuralTestBase() = default;
    //! @brief Destroy the StructuralTestBase object.
    ~StructuralTestBase() override = default;

    //! @brief Set up the test case.
    static void SetUpTestCase() { TST_DP_PRINT_TASK_TITLE("STRUCTURAL", "BEGIN"); }
    //! @brief Tear down the test case.
    static void TearDownTestCase() { TST_DP_PRINT_TASK_TITLE("STRUCTURAL", "END"); }
    //! @brief Set up.
    void SetUp() override {}
    //! @brief Tear down.
    void TearDown() override {}
};

//! @brief Test for the adapter instance in the pattern of structural.
TEST_F(StructuralTestBase, adapterInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = structural::Structural::adapter());
    ASSERT_TRUE(!result.str().empty());
}

//! @brief Test for the bridge instance in the pattern of structural.
TEST_F(StructuralTestBase, bridgeInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = structural::Structural::bridge());
    ASSERT_TRUE(!result.str().empty());
}

//! @brief Test for the composite instance in the pattern of structural.
TEST_F(StructuralTestBase, compositeInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = structural::Structural::composite());
    ASSERT_TRUE(!result.str().empty());
}

//! @brief Test for the decorator instance in the pattern of structural.
TEST_F(StructuralTestBase, decoratorInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = structural::Structural::decorator());
    ASSERT_TRUE(!result.str().empty());
}

//! @brief Test for the facade instance in the pattern of structural.
TEST_F(StructuralTestBase, facadeInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = structural::Structural::facade());
    ASSERT_TRUE(!result.str().empty());
}

//! @brief Test for the flyweight instance in the pattern of structural.
TEST_F(StructuralTestBase, flyweightInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = structural::Structural::flyweight());
    ASSERT_TRUE(!result.str().empty());
}

//! @brief Test for the proxy instance in the pattern of structural.
TEST_F(StructuralTestBase, proxyInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = structural::Structural::proxy());
    ASSERT_TRUE(!result.str().empty());
}
} // namespace tst_dp
} // namespace test
