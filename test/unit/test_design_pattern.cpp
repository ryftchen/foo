//! @file test_design_pattern.cpp
//! @author ryftchen
//! @brief The definitions (test_design_pattern) in the test module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

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
//! @brief Alias for the enumeration of design pattern tasks.
using Category = application::app_dp::DesignPatternTask::Category;

//! @brief Alias for the namespace of applying behavioral.
namespace behavioral = application::app_dp::behavioral;

//! @brief Test base of behavioral.
class BehavioralTestBase : public ::testing::Test
{
public:
    //! @brief Construct a new BehavioralTestBase object.
    BehavioralTestBase() = default;
    //! @brief Destroy the BehavioralTestBase object.
    ~BehavioralTestBase() override = default;

    //! @brief Set up the test case.
    static void SetUpTestCase() { TST_DP_PRINT_TASK_TITLE(Category::behavioral, "BEGIN"); }
    //! @brief Tear down the test case.
    static void TearDownTestCase() { TST_DP_PRINT_TASK_TITLE(Category::behavioral, "END"); }
    //! @brief Set up.
    void SetUp() override{};
    //! @brief Tear down.
    void TearDown() override{};
};

//! @brief Test for the chain of responsibility instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, chainOfResponsibilityInstance)
{
    ASSERT_NO_THROW(behavioral::Behavioral::chainOfResponsibility());
}

//! @brief Test for the command instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, commandInstance)
{
    ASSERT_NO_THROW(behavioral::Behavioral::command());
}

//! @brief Test for the interpreter instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, interpreterInstance)
{
    ASSERT_NO_THROW(behavioral::Behavioral::interpreter());
}

//! @brief Test for the iterator instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, iteratorInstance)
{
    ASSERT_NO_THROW(behavioral::Behavioral::iterator());
}

//! @brief Test for the mediator instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, mediatorInstance)
{
    ASSERT_NO_THROW(behavioral::Behavioral::mediator());
}

//! @brief Test for the memento instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, mementoInstance)
{
    ASSERT_NO_THROW(behavioral::Behavioral::memento());
}

//! @brief Test for the observer instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, observerInstance)
{
    ASSERT_NO_THROW(behavioral::Behavioral::observer());
}

//! @brief Test for the state instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, stateInstance)
{
    ASSERT_NO_THROW(behavioral::Behavioral::state());
}

//! @brief Test for the strategy instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, strategyInstance)
{
    ASSERT_NO_THROW(behavioral::Behavioral::strategy());
}

//! @brief Test for the template method instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, templateMethodInstance)
{
    ASSERT_NO_THROW(behavioral::Behavioral::templateMethod());
}

//! @brief Test for the visitor instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, visitorInstance)
{
    ASSERT_NO_THROW(behavioral::Behavioral::visitor());
}

//! @brief Alias for the namespace of applying creational.
namespace creational = application::app_dp::creational;

//! @brief Test base of creational.
class CreationalTestBase : public ::testing::Test
{
public:
    //! @brief Construct a new CreationalTestBase object.
    CreationalTestBase() = default;
    //! @brief Destroy the CreationalTestBase object.
    ~CreationalTestBase() override = default;

    //! @brief Set up the test case.
    static void SetUpTestCase() { TST_DP_PRINT_TASK_TITLE(Category::creational, "BEGIN"); }
    //! @brief Tear down the test case.
    static void TearDownTestCase() { TST_DP_PRINT_TASK_TITLE(Category::creational, "END"); }
    //! @brief Set up.
    void SetUp() override{};
    //! @brief Tear down.
    void TearDown() override{};
};

//! @brief Test for the abstract factory instance in the pattern of creational.
TEST_F(CreationalTestBase, abstractFactoryInstance)
{
    ASSERT_NO_THROW(creational::Creational::abstractFactory());
}

//! @brief Test for the builder instance in the pattern of creational.
TEST_F(CreationalTestBase, builderInstance)
{
    ASSERT_NO_THROW(creational::Creational::builder());
}

//! @brief Test for the factory method instance in the pattern of creational.
TEST_F(CreationalTestBase, factoryMethodInstance)
{
    ASSERT_NO_THROW(creational::Creational::factoryMethod());
}

//! @brief Test for the prototype instance in the pattern of creational.
TEST_F(CreationalTestBase, prototypeInstance)
{
    ASSERT_NO_THROW(creational::Creational::prototype());
}

//! @brief Test for the singleton instance in the pattern of creational.
TEST_F(CreationalTestBase, singletonInstance)
{
    ASSERT_NO_THROW(creational::Creational::singleton());
}

//! @brief Alias for the namespace of applying structural.
namespace structural = application::app_dp::structural;

//! @brief Test base of structural.
class StructuralTestBase : public ::testing::Test
{
public:
    //! @brief Construct a new StructuralTestBase object.
    StructuralTestBase() = default;
    //! @brief Destroy the StructuralTestBase object.
    ~StructuralTestBase() override = default;

    //! @brief Set up the test case.
    static void SetUpTestCase() { TST_DP_PRINT_TASK_TITLE(Category::structural, "BEGIN"); }
    //! @brief Tear down the test case.
    static void TearDownTestCase() { TST_DP_PRINT_TASK_TITLE(Category::structural, "END"); }
    //! @brief Set up.
    void SetUp() override{};
    //! @brief Tear down.
    void TearDown() override{};
};

//! @brief Test for the adapter instance in the pattern of structural.
TEST_F(StructuralTestBase, adapterInstance)
{
    ASSERT_NO_THROW(structural::Structural::adapter());
}

//! @brief Test for the bridge instance in the pattern of structural.
TEST_F(StructuralTestBase, bridgeInstance)
{
    ASSERT_NO_THROW(structural::Structural::bridge());
}

//! @brief Test for the composite instance in the pattern of structural.
TEST_F(StructuralTestBase, compositeInstance)
{
    ASSERT_NO_THROW(structural::Structural::composite());
}

//! @brief Test for the decorator instance in the pattern of structural.
TEST_F(StructuralTestBase, decoratorInstance)
{
    ASSERT_NO_THROW(structural::Structural::decorator());
}

//! @brief Test for the facade instance in the pattern of structural.
TEST_F(StructuralTestBase, facadeInstance)
{
    ASSERT_NO_THROW(structural::Structural::facade());
}

//! @brief Test for the flyweight instance in the pattern of structural.
TEST_F(StructuralTestBase, flyweightInstance)
{
    ASSERT_NO_THROW(structural::Structural::flyweight());
}

//! @brief Test for the proxy instance in the pattern of structural.
TEST_F(StructuralTestBase, proxyInstance)
{
    ASSERT_NO_THROW(structural::Structural::proxy());
}
} // namespace tst_dp
} // namespace test
