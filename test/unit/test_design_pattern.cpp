#include <gtest/gtest.h>
#include "design_pattern/include/behavioral.hpp"
#include "design_pattern/include/creational.hpp"
#include "design_pattern/include/structural.hpp"

namespace test::tst_dp
{
constexpr uint32_t titleWidthForPrintTest = 50;

class BehavioralTestBase : public ::testing::Test
{
public:
    BehavioralTestBase() = default;
    ~BehavioralTestBase() override = default;

    static void SetUpTestCase()
    {
        std::cout << "TEST DESIGN PATTERN: " << std::setiosflags(std::ios_base::left) << std::setfill('.')
                  << std::setw(titleWidthForPrintTest) << "BEHAVIORAL"
                  << "BEGIN" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl;
        behavioral = std::make_shared<design_pattern::behavioral::BehavioralPattern>();
    };
    static void TearDownTestCase()
    {
        std::cout << "TEST DESIGN PATTERN: " << std::setiosflags(std::ios_base::left) << std::setfill('.')
                  << std::setw(titleWidthForPrintTest) << "BEHAVIORAL"
                  << "END" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl;
        behavioral.reset();
    }
    void SetUp() override{};
    void TearDown() override{};

    static std::shared_ptr<design_pattern::behavioral::BehavioralPattern> behavioral;
};

std::shared_ptr<design_pattern::behavioral::BehavioralPattern> BehavioralTestBase::behavioral = nullptr;

TEST_F(BehavioralTestBase, chainOfResponsibilityInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW( // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
        behavioral->chainOfResponsibilityInstance());
}

TEST_F(BehavioralTestBase, commandInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(behavioral->commandInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

TEST_F(BehavioralTestBase, interpreterInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(behavioral->interpreterInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

TEST_F(BehavioralTestBase, iteratorInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(behavioral->iteratorInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

TEST_F(BehavioralTestBase, mediatorInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(behavioral->mediatorInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

TEST_F(BehavioralTestBase, mementoInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(behavioral->mementoInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

TEST_F(BehavioralTestBase, observerInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(behavioral->observerInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

TEST_F(BehavioralTestBase, stateInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(behavioral->stateInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

TEST_F(BehavioralTestBase, strategyInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(behavioral->strategyInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

TEST_F(BehavioralTestBase, templateMethodInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(behavioral->templateMethodInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

TEST_F(BehavioralTestBase, visitorInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(behavioral->visitorInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

class CreationalTestBase : public ::testing::Test
{
public:
    CreationalTestBase() = default;
    ~CreationalTestBase() override = default;

    static void SetUpTestCase()
    {
        std::cout << "TEST DESIGN PATTERN: " << std::setiosflags(std::ios_base::left) << std::setfill('.')
                  << std::setw(titleWidthForPrintTest) << "CREATIONAL"
                  << "BEGIN" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl;
        creational = std::make_shared<design_pattern::creational::CreationalPattern>();
    };
    static void TearDownTestCase()
    {
        std::cout << "TEST DESIGN PATTERN: " << std::setiosflags(std::ios_base::left) << std::setfill('.')
                  << std::setw(titleWidthForPrintTest) << "CREATIONAL"
                  << "END" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl;
        creational.reset();
    }
    void SetUp() override{};
    void TearDown() override{};

    static std::shared_ptr<design_pattern::creational::CreationalPattern> creational;
};

std::shared_ptr<design_pattern::creational::CreationalPattern> CreationalTestBase::creational = nullptr;

TEST_F(CreationalTestBase, abstractFactoryInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(creational->abstractFactoryInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

TEST_F(CreationalTestBase, builderInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(creational->builderInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

TEST_F(CreationalTestBase, factoryMethodInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(creational->factoryMethodInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

TEST_F(CreationalTestBase, prototypeInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(creational->prototypeInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

TEST_F(CreationalTestBase, singletonInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(creational->singletonInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

class StructuralTestBase : public ::testing::Test
{
public:
    StructuralTestBase() = default;
    ~StructuralTestBase() override = default;

    static void SetUpTestCase()
    {
        std::cout << "TEST DESIGN PATTERN: " << std::setiosflags(std::ios_base::left) << std::setfill('.')
                  << std::setw(titleWidthForPrintTest) << "STRUCTURAL"
                  << "BEGIN" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl;
        structural = std::make_shared<design_pattern::structural::StructuralPattern>();
    };
    static void TearDownTestCase()
    {
        std::cout << "TEST DESIGN PATTERN: " << std::setiosflags(std::ios_base::left) << std::setfill('.')
                  << std::setw(titleWidthForPrintTest) << "STRUCTURAL"
                  << "END" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl;
        structural.reset();
    }
    void SetUp() override{};
    void TearDown() override{};

    static std::shared_ptr<design_pattern::structural::StructuralPattern> structural;
};

std::shared_ptr<design_pattern::structural::StructuralPattern> StructuralTestBase::structural = nullptr;

TEST_F(StructuralTestBase, adapterInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(structural->adapterInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

TEST_F(StructuralTestBase, bridgeInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(structural->bridgeInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

TEST_F(StructuralTestBase, compositeInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(structural->compositeInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

TEST_F(StructuralTestBase, decoratorInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(structural->decoratorInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

TEST_F(StructuralTestBase, facadeInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(structural->facadeInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

TEST_F(StructuralTestBase, flyweightInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(structural->flyweightInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}

TEST_F(StructuralTestBase, proxyInstance) // NOLINT(cert-err58-cpp)
{
    ASSERT_NO_THROW(structural->proxyInstance()); // NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
}
} // namespace test::tst_dp
