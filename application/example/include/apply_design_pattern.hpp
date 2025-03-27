//! @file apply_design_pattern.hpp
//! @author ryftchen
//! @brief The declarations (apply_design_pattern) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#ifndef __PRECOMPILED_HEADER
#include <bitset>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

#include "design_pattern/include/behavioral.hpp"
#include "design_pattern/include/creational.hpp"
#include "design_pattern/include/structural.hpp"

//! @brief The application module.
namespace application // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Design-pattern-applying-related functions in the application module.
namespace app_dp
{
//! @brief Represent the maximum value of an enum.
//! @tparam T - type of specific enum
template <typename T>
struct Bottom;

//! @brief Enumerate specific behavioral instances.
enum BehavioralInstance : std::uint8_t
{
    //! @brief Chain of responsibility.
    chainOfResponsibility,
    //! @brief Command.
    command,
    //! @brief Interpreter.
    interpreter,
    //! @brief Iterator.
    iterator,
    //! @brief Mediator.
    mediator,
    //! @brief Memento.
    memento,
    //! @brief Observer.
    observer,
    //! @brief State.
    state,
    //! @brief Strategy.
    strategy,
    //! @brief Template method.
    templateMethod,
    //! @brief Visitor.
    visitor
};
//! @brief Store the maximum value of the BehavioralInstance enum.
template <>
struct Bottom<BehavioralInstance>
{
    //! @brief Maximum value of the BehavioralInstance enum.
    static constexpr std::uint8_t value{11};
};

//! @brief Enumerate specific creational instances.
enum CreationalInstance : std::uint8_t
{
    //! @brief Abstract factory.
    abstractFactory,
    //! @brief Builder.
    builder,
    //! @brief Factory method.
    factoryMethod,
    //! @brief Prototype.
    prototype,
    //! @brief Singleton.
    singleton
};
//! @brief Store the maximum value of the CreationalInstance enum.
template <>
struct Bottom<CreationalInstance>
{
    //! @brief Maximum value of the CreationalInstance enum.
    static constexpr std::uint8_t value{5};
};

//! @brief Enumerate specific structural instances.
enum StructuralInstance : std::uint8_t
{
    //! @brief Adapter.
    adapter,
    //! @brief Bridge.
    bridge,
    //! @brief Composite.
    composite,
    //! @brief Decorator.
    decorator,
    //! @brief Facade.
    facade,
    //! @brief Flyweight.
    flyweight,
    //! @brief Proxy.
    proxy
};
//! @brief Store the maximum value of the StructuralInstance enum.
template <>
struct Bottom<StructuralInstance>
{
    //! @brief Maximum value of the StructuralInstance enum.
    static constexpr std::uint8_t value{7};
};

//! @brief Manage design pattern choices.
class ApplyDesignPattern
{
public:
    //! @brief Enumerate specific design pattern choices.
    enum Category : std::uint8_t
    {
        //! @brief Behavioral.
        behavioral,
        //! @brief Creational.
        creational,
        //! @brief Structural.
        structural
    };

    //! @brief Bit flags for managing behavioral instances.
    std::bitset<Bottom<BehavioralInstance>::value> behavioralOpts{};
    //! @brief Bit flags for managing creational instances.
    std::bitset<Bottom<CreationalInstance>::value> creationalOpts{};
    //! @brief Bit flags for managing structural instances.
    std::bitset<Bottom<StructuralInstance>::value> structuralOpts{};

    //! @brief Check whether any design pattern choices do not exist.
    //! @return any design pattern choices do not exist or exist
    [[nodiscard]] inline bool empty() const
    {
        return behavioralOpts.none() && creationalOpts.none() && structuralOpts.none();
    }
    //! @brief Reset bit flags that manage design pattern choices.
    inline void reset()
    {
        behavioralOpts.reset();
        creationalOpts.reset();
        structuralOpts.reset();
    }

protected:
    //! @brief The operator (<<) overloading of the Category enum.
    //! @param os - output stream object
    //! @param cat - the specific value of Category enum
    //! @return reference of the output stream object
    friend std::ostream& operator<<(std::ostream& os, const Category cat)
    {
        switch (cat)
        {
            case Category::behavioral:
                os << "BEHAVIORAL";
                break;
            case Category::creational:
                os << "CREATIONAL";
                break;
            case Category::structural:
                os << "STRUCTURAL";
                break;
            default:
                os << "UNKNOWN (" << static_cast<std::underlying_type_t<Category>>(cat) << ')';
                break;
        }

        return os;
    }
};
extern ApplyDesignPattern& manager();

//! @brief Update choice.
//! @tparam T - type of target instance
//! @param target - target instance
template <typename T>
void updateChoice(const std::string_view target);
//! @brief Run choices.
//! @tparam T - type of target instance
//! @param candidates - container for the candidate target instances
template <typename T>
void runChoices(const std::vector<std::string>& candidates);

//! @brief Apply behavioral.
namespace behavioral
{
//! @brief The version used to apply.
const char* const version = design_pattern::behavioral::version();
//! @brief Behavioral instances.
class Behavioral
{
public:
    //! @brief Destroy the Behavioral object.
    virtual ~Behavioral() = default;

    // NOLINTBEGIN(google-build-using-namespace)
    //! @brief Chain of responsibility.
    //! @return procedure output
    static std::ostringstream chainOfResponsibility()
    {
        using namespace design_pattern::behavioral::chain_of_responsibility;
        const std::shared_ptr<ConcreteHandler1> handler1 = std::make_shared<ConcreteHandler1>();
        const std::shared_ptr<ConcreteHandler2> handler2 = std::make_shared<ConcreteHandler2>();

        handler1->setHandler(handler2);
        handler1->handleRequest();

        return transferOutputs(output());
    }
    //! @brief Command.
    //! @return procedure output
    static std::ostringstream command()
    {
        using namespace design_pattern::behavioral::command;
        const std::shared_ptr<ConcreteCommand> commands =
            std::make_shared<ConcreteCommand>(std::make_shared<Receiver>());

        Invoker invoker{};
        invoker.set(commands);
        invoker.confirm();

        return transferOutputs(output());
    }
    //! @brief Interpreter.
    //! @return procedure output
    static std::ostringstream interpreter()
    {
        using namespace design_pattern::behavioral::interpreter;
        const std::shared_ptr<AbstractExpression> a = std::make_shared<TerminalExpression>("A"),
                                                  b = std::make_shared<TerminalExpression>("B"),
                                                  exp = std::make_shared<NonTerminalExpression>(a, b);

        const std::shared_ptr<Context> context = std::make_shared<Context>();
        context->set("A", true);
        context->set("B", false);

        output() << context->get("A") << " AND " << context->get("B");
        output() << " = " << exp->interpret(context) << '\n';

        return transferOutputs(output());
    }
    //! @brief Iterator.
    //! @return procedure output
    static std::ostringstream iterator()
    {
        using namespace design_pattern::behavioral::iterator;
        constexpr std::uint32_t size = 5;
        const std::shared_ptr<ConcreteAggregate> list = std::make_shared<ConcreteAggregate>(size);

        for (std::shared_ptr<Iterator> iter = list->createIterator(); !iter->isDone(); iter->next())
        {
            output() << "item value: " << iter->currentItem() << '\n';
        }

        return transferOutputs(output());
    }
    //! @brief Mediator.
    //! @return procedure output
    static std::ostringstream mediator()
    {
        using namespace design_pattern::behavioral::mediator;
        constexpr std::uint32_t id1 = 1, id2 = 2, id3 = 3;
        const std::shared_ptr<Mediator> mediators = std::make_shared<ConcreteMediator>();
        const std::shared_ptr<Colleague> c1 = std::make_shared<ConcreteColleague>(mediators, id1),
                                         c2 = std::make_shared<ConcreteColleague>(mediators, id2),
                                         c3 = std::make_shared<ConcreteColleague>(mediators, id3);

        mediators->add(c1);
        mediators->add(c2);
        mediators->add(c3);
        c1->send("hi!");
        c3->send("hello!");

        return transferOutputs(output());
    }
    //! @brief Memento.
    //! @return procedure output
    static std::ostringstream memento()
    {
        using namespace design_pattern::behavioral::memento;
        constexpr int state1 = 1, state2 = 2, state3 = 3;
        const std::shared_ptr<Originator> originator = std::make_shared<Originator>();
        const std::shared_ptr<CareTaker> caretaker = std::make_shared<CareTaker>(originator);

        originator->setState(state1);
        caretaker->save();
        originator->setState(state2);
        caretaker->save();
        originator->setState(state3);
        caretaker->undo();

        output() << "actual state is " << originator->getState() << '\n';

        return transferOutputs(output());
    }
    //! @brief Observer.
    //! @return procedure output
    static std::ostringstream observer()
    {
        using namespace design_pattern::behavioral::observer;
        constexpr int state1 = 1, state2 = 2, state3 = 3;
        const std::shared_ptr<ConcreteObserver> observer1 = std::make_shared<ConcreteObserver>(state1),
                                                observer2 = std::make_shared<ConcreteObserver>(state2);

        output() << "observer1 state: " << observer1->getState() << '\n';
        output() << "observer2 state: " << observer2->getState() << '\n';

        const std::shared_ptr<Subject> subject = std::make_shared<ConcreteSubject>();
        subject->attach(observer1);
        subject->attach(observer2);
        subject->setState(state3);
        subject->notify();

        output() << "observer1 state: " << observer1->getState() << '\n';
        output() << "observer2 state: " << observer2->getState() << '\n';

        return transferOutputs(output());
    }
    //! @brief State.
    //! @return procedure output
    static std::ostringstream state()
    {
        using namespace design_pattern::behavioral::state;
        const std::shared_ptr<Context> context = std::make_shared<Context>();

        context->setState(std::make_unique<ConcreteStateA>());
        context->request();

        context->setState(std::make_unique<ConcreteStateB>());
        context->request();

        return transferOutputs(output());
    }
    //! @brief Strategy.
    //! @return procedure output
    static std::ostringstream strategy()
    {
        using namespace design_pattern::behavioral::strategy;
        Context contextA(std::make_unique<ConcreteStrategyA>());
        contextA.contextInterface();

        Context contextB(std::make_unique<ConcreteStrategyB>());
        contextB.contextInterface();

        return transferOutputs(output());
    }
    //! @brief Template method.
    //! @return procedure output
    static std::ostringstream templateMethod()
    {
        using namespace design_pattern::behavioral::template_method;
        const std::shared_ptr<AbstractClass> tm = std::make_shared<ConcreteClass>();

        tm->templateMethod();

        return transferOutputs(output());
    }
    //! @brief Visitor.
    //! @return procedure output
    static std::ostringstream visitor()
    {
        using namespace design_pattern::behavioral::visitor;
        const std::shared_ptr<ConcreteElementA> elementA = std::make_shared<ConcreteElementA>();
        const std::shared_ptr<ConcreteElementB> elementB = std::make_shared<ConcreteElementB>();

        ConcreteVisitor1 visitor1{};
        ConcreteVisitor2 visitor2{};
        elementA->accept(visitor1);
        elementA->accept(visitor2);
        elementB->accept(visitor1);
        elementB->accept(visitor2);

        return transferOutputs(output());
    }
    // NOLINTEND(google-build-using-namespace)

private:
    //! @brief Transfer full outputs.
    //! @param outputs - original outputs
    //! @return full outputs
    static std::ostringstream transferOutputs(std::ostringstream& outputs)
    {
        std::ostringstream process(outputs.str());
        outputs.str("");
        outputs.clear();

        return process;
    }
};

//! @brief Pattern of behavioral.
class BehavioralPattern
{
public:
    //! @brief Destroy the BehavioralPattern object.
    virtual ~BehavioralPattern() = default;

    //! @brief The chain of responsibility instance.
    static void chainOfResponsibilityInstance();
    //! @brief The command instance.
    static void commandInstance();
    //! @brief The interpreter instance.
    static void interpreterInstance();
    //! @brief The iterator instance.
    static void iteratorInstance();
    //! @brief The mediator instance.
    static void mediatorInstance();
    //! @brief The memento instance.
    static void mementoInstance();
    //! @brief The observer instance.
    static void observerInstance();
    //! @brief The state instance.
    static void stateInstance();
    //! @brief The strategy instance.
    static void strategyInstance();
    //! @brief The template method instance.
    static void templateMethodInstance();
    //! @brief The visitor instance.
    static void visitorInstance();
};
} // namespace behavioral
template <>
void updateChoice<BehavioralInstance>(const std::string_view target);
template <>
void runChoices<BehavioralInstance>(const std::vector<std::string>& candidates);

//! @brief Apply creational.
namespace creational
{
//! @brief The version used to apply.
const char* const version = design_pattern::creational::version();
//! @brief Creational instances.
class Creational
{
public:
    //! @brief Destroy the Creational object.
    virtual ~Creational() = default;

    // NOLINTBEGIN(google-build-using-namespace)
    //! @brief Abstract factory.
    //! @return procedure output
    static std::ostringstream abstractFactory()
    {
        using namespace design_pattern::creational::abstract_factory;
        const std::shared_ptr<ConcreteFactoryX> factoryX = std::make_shared<ConcreteFactoryX>();
        const std::shared_ptr<ConcreteFactoryY> factoryY = std::make_shared<ConcreteFactoryY>();

        const std::unique_ptr<ProductA> p1 = factoryX->createProductA();
        output() << "product: " << p1->getName() << '\n';

        const std::unique_ptr<ProductA> p2 = factoryY->createProductA();
        output() << "product: " << p2->getName() << '\n';

        const std::unique_ptr<ProductB> p3 = factoryX->createProductB();
        output() << "product: " << p3->getName() << '\n';

        const std::unique_ptr<ProductB> p4 = factoryY->createProductB();
        output() << "product: " << p4->getName() << '\n';

        return transferOutputs(output());
    }
    //! @brief Builder.
    //! @return procedure output
    static std::ostringstream builder()
    {
        using namespace design_pattern::creational::builder;
        Director director{};
        director.set(std::make_unique<ConcreteBuilderX>());
        director.construct();
        Product product1 = director.get();
        output() << "1st product parts: " << product1.get() << '\n';

        director.set(std::make_unique<ConcreteBuilderY>());
        director.construct();
        Product product2 = director.get();
        output() << "2nd product parts: " << product2.get() << '\n';

        return transferOutputs(output());
    }
    //! @brief Factory method.
    //! @return procedure output
    static std::ostringstream factoryMethod()
    {
        using namespace design_pattern::creational::factory_method;
        const std::shared_ptr<Creator> creator = std::make_shared<ConcreteCreator>();

        std::unique_ptr<Product> p1 = creator->createProductA();
        output() << "product: " << p1->getName() << '\n';
        creator->removeProduct(p1);

        std::unique_ptr<Product> p2 = creator->createProductB();
        output() << "product: " << p2->getName() << '\n';
        creator->removeProduct(p2);

        return transferOutputs(output());
    }
    //! @brief Prototype.
    //! @return procedure output
    static std::ostringstream prototype()
    {
        using namespace design_pattern::creational::prototype;
        Client::init();

        const std::unique_ptr<Prototype> prototype1 = Client::make(0);
        output() << "prototype: " << prototype1->type() << '\n';

        const std::unique_ptr<Prototype> prototype2 = Client::make(1);
        output() << "prototype: " << prototype2->type() << '\n';

        Client::remove();

        return transferOutputs(output());
    }
    //! @brief Singleton.
    //! @return procedure output
    static std::ostringstream singleton()
    {
        using namespace design_pattern::creational::singleton;
        Singleton::get()->tell();

        Singleton::restart();

        return transferOutputs(output());
    }
    // NOLINTEND(google-build-using-namespace)

private:
    //! @brief Transfer full outputs.
    //! @param outputs - original outputs
    //! @return full outputs
    static std::ostringstream transferOutputs(std::ostringstream& outputs)
    {
        std::ostringstream process(outputs.str());
        outputs.str("");
        outputs.clear();

        return process;
    }
};

//! @brief Pattern of creational.
class CreationalPattern
{
public:
    //! @brief Destroy the CreationalPattern object.
    virtual ~CreationalPattern() = default;

    //! @brief The abstract factory instance.
    static void abstractFactoryInstance();
    //! @brief The builder instance.
    static void builderInstance();
    //! @brief The factory method instance.
    static void factoryMethodInstance();
    //! @brief The prototype instance.
    static void prototypeInstance();
    //! @brief The singleton instance.
    static void singletonInstance();
};
} // namespace creational
template <>
void updateChoice<CreationalInstance>(const std::string_view target);
template <>
void runChoices<CreationalInstance>(const std::vector<std::string>& candidates);

//! @brief Apply structural.
namespace structural
{
//! @brief The version used to apply.
const char* const version = design_pattern::structural::version();
//! @brief Structural instances.
class Structural
{
public:
    //! @brief Destroy the Structural object.
    virtual ~Structural() = default;

    // NOLINTBEGIN(google-build-using-namespace)
    //! @brief Adapter.
    //! @return procedure output
    static std::ostringstream adapter()
    {
        using namespace design_pattern::structural::adapter;
        const std::shared_ptr<Target> t = std::make_shared<Adapter>();

        t->request();

        return transferOutputs(output());
    }
    //! @brief Bridge.
    //! @return procedure output
    static std::ostringstream bridge()
    {
        using namespace design_pattern::structural::bridge;
        const std::unique_ptr<Abstraction> abstract1 =
            std::make_unique<RefinedAbstraction>(std::make_unique<ConcreteImplementorA>());
        abstract1->operation();

        const std::unique_ptr<Abstraction> abstract2 =
            std::make_unique<RefinedAbstraction>(std::make_unique<ConcreteImplementorB>());
        abstract2->operation();

        return transferOutputs(output());
    }
    //! @brief Composite.
    //! @return procedure output
    static std::ostringstream composite()
    {
        using namespace design_pattern::structural::composite;
        constexpr std::uint32_t count = 5;
        Composite composites{};

        for (std::uint32_t i = 0; i < count; ++i)
        {
            composites.add(std::make_shared<Leaf>(i));
        }
        composites.remove(0);
        composites.operation();

        return transferOutputs(output());
    }
    //! @brief Decorator.
    //! @return procedure output
    static std::ostringstream decorator()
    {
        using namespace design_pattern::structural::decorator;
        const std::shared_ptr<ConcreteComponent> cc = std::make_shared<ConcreteComponent>();
        const std::shared_ptr<ConcreteDecoratorA> da = std::make_shared<ConcreteDecoratorA>(cc);
        const std::shared_ptr<ConcreteDecoratorB> db = std::make_shared<ConcreteDecoratorB>(da);

        const std::shared_ptr<Component> component = db;
        component->operation();

        return transferOutputs(output());
    }
    //! @brief Facade.
    //! @return procedure output
    static std::ostringstream facade()
    {
        using namespace design_pattern::structural::facade;
        const std::shared_ptr<Facade> facades = std::make_shared<Facade>();

        facades->operation1();
        facades->operation2();

        return transferOutputs(output());
    }
    //! @brief Flyweight.
    //! @return procedure output
    static std::ostringstream flyweight()
    {
        using namespace design_pattern::structural::flyweight;
        const std::shared_ptr<FlyweightFactory> factory = std::make_shared<FlyweightFactory>();

        factory->getFlyweight(1)->operation();
        factory->getFlyweight(2)->operation();

        return transferOutputs(output());
    }
    //! @brief Proxy.
    //! @return procedure output
    static std::ostringstream proxy()
    {
        using namespace design_pattern::structural::proxy;
        const std::shared_ptr<Proxy> proxies = std::make_shared<Proxy>();

        proxies->request();

        return transferOutputs(output());
    }

private:
    //! @brief Transfer full outputs.
    //! @param outputs - original outputs
    //! @return full outputs
    static std::ostringstream transferOutputs(std::ostringstream& outputs)
    {
        std::ostringstream process(outputs.str());
        outputs.str("");
        outputs.clear();

        return process;
    }
    // NOLINTEND(google-build-using-namespace)
};

//! @brief Pattern of structural.
class StructuralPattern
{
public:
    //! @brief Destroy the StructuralPattern object.
    virtual ~StructuralPattern() = default;

    //! @brief The adapter instance.
    static void adapterInstance();
    //! @brief The bridge instance.
    static void bridgeInstance();
    //! @brief The composite instance.
    static void compositeInstance();
    //! @brief The decorator instance.
    static void decoratorInstance();
    //! @brief The facade instance.
    static void facadeInstance();
    //! @brief The flyweight instance.
    static void flyweightInstance();
    //! @brief The proxy instance.
    static void proxyInstance();
};
} // namespace structural
template <>
void updateChoice<StructuralInstance>(const std::string_view target);
template <>
void runChoices<StructuralInstance>(const std::vector<std::string>& candidates);
} // namespace app_dp
} // namespace application
