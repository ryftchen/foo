//! @file test_design_pattern.cpp
//! @author ryftchen
//! @brief The definitions (test_design_pattern) in the test module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#include <gtest/gtest.h>
#include "design_pattern/include/behavioral.hpp"
#include "design_pattern/include/creational.hpp"
#include "design_pattern/include/structural.hpp"

//! @brief Title of printing for design pattern task tests.
#define TST_DP_PRINT_TASK_TITLE(taskType, taskState)                                                   \
    std::cout << "TEST DESIGN PATTERN: " << std::setiosflags(std::ios_base::left) << std::setfill('.') \
              << std::setw(50) << taskType << taskState << std::resetiosflags(std::ios_base::left)     \
              << std::setfill(' ') << std::endl;

//! @brief Design-pattern-testing-related functions in the test module.
namespace test::tst_dp
{
//! @brief Test base of behavioral.
class BehavioralTestBase : public ::testing::Test
{
public:
    //! @brief Construct a new BehavioralTestBase object.
    BehavioralTestBase() = default;
    //! @brief Destroy the BehavioralTestBase object.
    ~BehavioralTestBase() override = default;

    //! @brief Set up the test case.
    static void SetUpTestCase()
    {
        TST_DP_PRINT_TASK_TITLE("BEHAVIORAL", "BEGIN");
        pattern = std::make_shared<design_pattern::behavioral::BehavioralPattern>();
    };
    //! @brief Tear down the test case.
    static void TearDownTestCase()
    {
        TST_DP_PRINT_TASK_TITLE("BEHAVIORAL", "END");
        pattern.reset();
    }
    //! @brief Set up.
    void SetUp() override{};
    //! @brief Tear down.
    void TearDown() override{};

    //! @brief Pattern Instance.
    static std::shared_ptr<design_pattern::behavioral::BehavioralPattern> pattern;
};

std::shared_ptr<design_pattern::behavioral::BehavioralPattern> BehavioralTestBase::pattern = nullptr;

//! @brief Test for the chain of responsibility instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, chainOfResponsibilityInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW( // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
        pattern->chainOfResponsibilityInstance());
}

//! @brief Test for the command instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, commandInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(pattern->commandInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

//! @brief Test for the interpreter instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, interpreterInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(pattern->interpreterInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

//! @brief Test for the iterator instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, iteratorInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(pattern->iteratorInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

//! @brief Test for the mediator instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, mediatorInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(pattern->mediatorInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

//! @brief Test for the memento instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, mementoInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(pattern->mementoInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

//! @brief Test for the observer instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, observerInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(pattern->observerInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

//! @brief Test for the state instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, stateInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(pattern->stateInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

//! @brief Test for the strategy instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, strategyInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(pattern->strategyInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

//! @brief Test for the template method instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, templateMethodInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(pattern->templateMethodInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

//! @brief Test for the visitor instance in the pattern of behavioral.
TEST_F(BehavioralTestBase, visitorInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(pattern->visitorInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
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
    static void SetUpTestCase()
    {
        TST_DP_PRINT_TASK_TITLE("CREATIONAL", "BEGIN");
        pattern = std::make_shared<design_pattern::creational::CreationalPattern>();
    };
    //! @brief Tear down the test case.
    static void TearDownTestCase()
    {
        TST_DP_PRINT_TASK_TITLE("CREATIONAL", "END");
        pattern.reset();
    }
    //! @brief Set up.
    void SetUp() override{};
    //! @brief Tear down.
    void TearDown() override{};

    //! @brief Pattern Instance.
    static std::shared_ptr<design_pattern::creational::CreationalPattern> pattern;
};

std::shared_ptr<design_pattern::creational::CreationalPattern> CreationalTestBase::pattern = nullptr;

//! @brief Test for the abstract factory instance in the pattern of creational.
TEST_F(CreationalTestBase, abstractFactoryInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(pattern->abstractFactoryInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

//! @brief Test for the builder instance in the pattern of creational.
TEST_F(CreationalTestBase, builderInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(pattern->builderInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

//! @brief Test for the factory method instance in the pattern of creational.
TEST_F(CreationalTestBase, factoryMethodInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(pattern->factoryMethodInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

//! @brief Test for the prototype instance in the pattern of creational.
TEST_F(CreationalTestBase, prototypeInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(pattern->prototypeInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

//! @brief Test for the singleton instance in the pattern of creational.
TEST_F(CreationalTestBase, singletonInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(pattern->singletonInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
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
    static void SetUpTestCase()
    {
        TST_DP_PRINT_TASK_TITLE("STRUCTURAL", "BEGIN");
        pattern = std::make_shared<design_pattern::structural::StructuralPattern>();
    };
    //! @brief Tear down the test case.
    static void TearDownTestCase()
    {
        TST_DP_PRINT_TASK_TITLE("STRUCTURAL", "END");
        pattern.reset();
    }
    //! @brief Set up.
    void SetUp() override{};
    //! @brief Tear down.
    void TearDown() override{};

    //! @brief Pattern Instance.
    static std::shared_ptr<design_pattern::structural::StructuralPattern> pattern;
};

std::shared_ptr<design_pattern::structural::StructuralPattern> StructuralTestBase::pattern = nullptr;

//! @brief Test for the adapter instance in the pattern of structural.
TEST_F(StructuralTestBase, adapterInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(pattern->adapterInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

//! @brief Test for the bridge instance in the pattern of structural.
TEST_F(StructuralTestBase, bridgeInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(pattern->bridgeInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

//! @brief Test for the composite instance in the pattern of structural.
TEST_F(StructuralTestBase, compositeInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(pattern->compositeInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

//! @brief Test for the decorator instance in the pattern of structural.
TEST_F(StructuralTestBase, decoratorInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(pattern->decoratorInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

//! @brief Test for the facade instance in the pattern of structural.
TEST_F(StructuralTestBase, facadeInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(pattern->facadeInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

//! @brief Test for the flyweight instance in the pattern of structural.
TEST_F(StructuralTestBase, flyweightInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(pattern->flyweightInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

//! @brief Test for the proxy instance in the pattern of structural.
TEST_F(StructuralTestBase, proxyInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(pattern->proxyInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}
} // namespace test::tst_dp
