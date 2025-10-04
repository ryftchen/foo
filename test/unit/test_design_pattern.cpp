//! @file test_design_pattern.cpp
//! @author ryftchen
//! @brief The definitions (test_design_pattern) in the test module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include <gtest/gtest.h>
#include <syncstream>

#include "application/example/include/apply_design_pattern.hpp"

//! @brief Title of printing for design pattern task tests.
#define TST_DP_PRINT_TASK_TITLE(title, state)                                                       \
    std::osyncstream(std::cout) << "TEST DESIGN PATTERN: " << std::setiosflags(std::ios_base::left) \
                                << std::setfill('.') << std::setw(50) << (title) << (state)         \
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
protected:
    //! @brief Set up the test case.
    static void SetUpTestSuite() { TST_DP_PRINT_TASK_TITLE(title, "BEGIN"); }
    //! @brief Tear down the test case.
    static void TearDownTestSuite() { TST_DP_PRINT_TASK_TITLE(title, "END"); }

    //! @brief Test title.
    static const std::string_view title;
    //! @brief System under test.
    [[no_unique_address]] const behavioral::Showcase sut{};
    // clang-format off
    //! @brief Expected result 1.
    static constexpr std::string_view expRes1
    {
        "cannot be handled by handler 1\n"
        "handled by handler 2\n"
    };
    //! @brief Expected result 2.
    static constexpr std::string_view expRes2
    {
        "receiver: execute action\n"
    };
    //! @brief Expected result 3.
    static constexpr std::string_view expRes3
    {
        "1 AND 0 = 0\n"
    };
    //! @brief Expected result 4.
    static constexpr std::string_view expRes4
    {
        "item value: 1\n"
        "item value: 1\n"
        "item value: 1\n"
        "item value: 1\n"
        "item value: 1\n"
    };
    //! @brief Expected result 5.
    static constexpr std::string_view expRes5
    {
        "message \"hi!\" sent by colleague 1\n"
        "message \"hi!\" received by colleague 2\n"
        "message \"hi!\" received by colleague 3\n"
        "message \"hello!\" sent by colleague 3\n"
        "message \"hello!\" received by colleague 1\n"
        "message \"hello!\" received by colleague 2\n"
    };
    //! @brief Expected result 6.
    static constexpr std::string_view expRes6
    {
        "set state to 1\n"
        "save state\n"
        "set state to 2\n"
        "save state\n"
        "set state to 3\n"
        "undo state\n"
        "actual state is 2\n"
    };
    //! @brief Expected result 7.
    static constexpr std::string_view expRes7
    {
        "observer1 state: 1\n"
        "observer2 state: 2\n"
        "observer state updated\n"
        "observer state updated\n"
        "observer1 state: 3\n"
        "observer2 state: 3\n"
    };
    //! @brief Expected result 8.
    static constexpr std::string_view expRes8
    {
        "state A handled\n"
        "state B handled\n"
    };
    //! @brief Expected result 9.
    static constexpr std::string_view expRes9
    {
        "concrete strategy A\n"
        "concrete strategy B\n"
    };
    //! @brief Expected result 10.
    static constexpr std::string_view expRes10
    {
        "primitive operation 1\n"
        "primitive operation 2\n"
    };
    //! @brief Expected result 11.
    static constexpr std::string_view expRes11
    {
        "concrete visitor 1: element A visited\n"
        "concrete visitor 2: element A visited\n"
        "concrete visitor 1: element B visited\n"
        "concrete visitor 2: element B visited\n"
    };
    // clang-format on
};
const std::string_view BehavioralTestBase::title = design_pattern::behavioral::name();

//! @brief Test for the chain of responsibility instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, ChainOfResponsibilityInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.chainOfResponsibility());
    ASSERT_EQ(expRes1, result.str());
}

//! @brief Test for the command instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, CommandInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.command());
    ASSERT_EQ(expRes2, result.str());
}

//! @brief Test for the interpreter instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, InterpreterInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.interpreter());
    ASSERT_EQ(expRes3, result.str());
}

//! @brief Test for the iterator instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, IteratorInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.iterator());
    ASSERT_EQ(expRes4, result.str());
}

//! @brief Test for the mediator instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, MediatorInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.mediator());
    ASSERT_EQ(expRes5, result.str());
}

//! @brief Test for the memento instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, MementoInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.memento());
    ASSERT_EQ(expRes6, result.str());
}

//! @brief Test for the observer instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, ObserverInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.observer());
    ASSERT_EQ(expRes7, result.str());
}

//! @brief Test for the state instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, StateInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.state());
    ASSERT_EQ(expRes8, result.str());
}

//! @brief Test for the strategy instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, StrategyInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.strategy());
    ASSERT_EQ(expRes9, result.str());
}

//! @brief Test for the template method instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, TemplateMethodInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.templateMethod());
    ASSERT_EQ(expRes10, result.str());
}

//! @brief Test for the visitor instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, VisitorInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.visitor());
    ASSERT_EQ(expRes11, result.str());
}

//! @brief Test base of creational.
class CreationalTestBase : public ::testing::Test
{
protected:
    //! @brief Set up the test case.
    static void SetUpTestSuite() { TST_DP_PRINT_TASK_TITLE(title, "BEGIN"); }
    //! @brief Tear down the test case.
    static void TearDownTestSuite() { TST_DP_PRINT_TASK_TITLE(title, "END"); }

    //! @brief Test title.
    static const std::string_view title;
    //! @brief System under test.
    [[no_unique_address]] const creational::Showcase sut{};
    // clang-format off
    //! @brief Expected result 1.
    static constexpr std::string_view expRes1
    {
        "product: A-X\n"
        "product: A-Y\n"
        "product: B-X\n"
        "product: B-Y\n"
    };
    //! @brief Expected result 2.
    static constexpr std::string_view expRes2
    {
        "1st product parts: A-X B-X C-X\n"
        "2nd product parts: A-Y B-Y C-Y\n"
    };
    //! @brief Expected result 3.
    static constexpr std::string_view expRes3
    {
        "product: type A\n"
        "product: type B\n"
    };
    //! @brief Expected result 4.
    static constexpr std::string_view expRes4
    {
        "prototype: type A\n"
        "prototype: type B\n"
    };
    //! @brief Expected result 5.
    static constexpr std::string_view expRes5
    {
        "this is singleton\n"
    };
    // clang-format on
};
const std::string_view CreationalTestBase::title = design_pattern::creational::name();

//! @brief Test for the abstract factory instance in the pattern of creational.
TEST_F(CreationalTestBase, AbstractFactoryInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.abstractFactory());
    ASSERT_EQ(expRes1, result.str());
}

//! @brief Test for the builder instance in the pattern of creational.
TEST_F(CreationalTestBase, BuilderInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.builder());
    ASSERT_EQ(expRes2, result.str());
}

//! @brief Test for the factory method instance in the pattern of creational.
TEST_F(CreationalTestBase, FactoryMethodInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.factoryMethod());
    ASSERT_EQ(expRes3, result.str());
}

//! @brief Test for the prototype instance in the pattern of creational.
TEST_F(CreationalTestBase, PrototypeInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.prototype());
    ASSERT_EQ(expRes4, result.str());
}

//! @brief Test for the singleton instance in the pattern of creational.
TEST_F(CreationalTestBase, SingletonInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.singleton());
    ASSERT_EQ(expRes5, result.str());
}

//! @brief Test base of structural.
class StructuralTestBase : public ::testing::Test
{
protected:
    //! @brief Set up the test case.
    static void SetUpTestSuite() { TST_DP_PRINT_TASK_TITLE(title, "BEGIN"); }
    //! @brief Tear down the test case.
    static void TearDownTestSuite() { TST_DP_PRINT_TASK_TITLE(title, "END"); }

    //! @brief Test title.
    static const std::string_view title;
    //! @brief System under test.
    [[no_unique_address]] const structural::Showcase sut{};
    // clang-format off
    //! @brief Expected result 1.
    static constexpr std::string_view expRes1
    {
        "specific request\n"
    };
    //! @brief Expected result 2.
    static constexpr std::string_view expRes2
    {
        "concrete implementor A\n"
        "concrete implementor B\n"
    };
    //! @brief Expected result 3.
    static constexpr std::string_view expRes3
    {
        "leaf 1 operation\n"
        "leaf 2 operation\n"
        "leaf 3 operation\n"
        "leaf 4 operation\n"
    };
    //! @brief Expected result 4.
    static constexpr std::string_view expRes4
    {
        "concrete component operation\n"
        "decorator A\n"
        "decorator B\n"
    };
    //! @brief Expected result 5.
    static constexpr std::string_view expRes5
    {
        "subsystem A method\n"
        "subsystem B method\n"
        "subsystem C method\n"
    };
    //! @brief Expected result 6.
    static constexpr std::string_view expRes6
    {
        "concrete flyweight with state 1\n"
        "concrete flyweight with state 2\n"
    };
    //! @brief Expected result 7.
    static constexpr std::string_view expRes7
    {
        "real subject request\n"
    };
    // clang-format on
};
const std::string_view StructuralTestBase::title = design_pattern::structural::name();

//! @brief Test for the adapter instance in the pattern of structural.
TEST_F(StructuralTestBase, AdapterInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.adapter());
    ASSERT_EQ(expRes1, result.str());
}

//! @brief Test for the bridge instance in the pattern of structural.
TEST_F(StructuralTestBase, BridgeInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.bridge());
    ASSERT_EQ(expRes2, result.str());
}

//! @brief Test for the composite instance in the pattern of structural.
TEST_F(StructuralTestBase, CompositeInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.composite());
    ASSERT_EQ(expRes3, result.str());
}

//! @brief Test for the decorator instance in the pattern of structural.
TEST_F(StructuralTestBase, DecoratorInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.decorator());
    ASSERT_EQ(expRes4, result.str());
}

//! @brief Test for the facade instance in the pattern of structural.
TEST_F(StructuralTestBase, FacadeInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.facade());
    ASSERT_EQ(expRes5, result.str());
}

//! @brief Test for the flyweight instance in the pattern of structural.
TEST_F(StructuralTestBase, FlyweightInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.flyweight());
    ASSERT_EQ(expRes6, result.str());
}

//! @brief Test for the proxy instance in the pattern of structural.
TEST_F(StructuralTestBase, ProxyInstance)
{
    std::ostringstream result{};
    ASSERT_NO_THROW(result = sut.proxy());
    ASSERT_EQ(expRes7, result.str());
}
} // namespace tst_dp
} // namespace test

#undef TST_DP_PRINT_TASK_TITLE
