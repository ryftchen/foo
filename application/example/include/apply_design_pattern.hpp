//! @file apply_design_pattern.hpp
//! @author ryftchen
//! @brief The declarations (apply_design_pattern) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

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
namespace application // NOLINT (modernize-concat-nested-namespaces)
{
//! @brief Design-pattern-applying-related functions in the application module.
namespace app_dp
{
//! @brief Represent the maximum value of an enum.
//! @tparam T - type of specific enum
template <class T>
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
template <class T>
void updateChoice(const std::string& target);
//! @brief Run choices.
//! @tparam T - type of target instance
//! @param candidates - container for the candidate target instances
template <class T>
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

    //! @brief Chain of responsibility.
    //! @return procedure output
    static std::ostringstream chainOfResponsibility()
    {
        namespace chain_of_responsibility = design_pattern::behavioral::chain_of_responsibility;
        using chain_of_responsibility::ConcreteHandler1, chain_of_responsibility::ConcreteHandler2;

        std::shared_ptr<ConcreteHandler1> handler1 = std::make_shared<ConcreteHandler1>();
        std::shared_ptr<ConcreteHandler2> handler2 = std::make_shared<ConcreteHandler2>();

        handler1->setHandler(handler2);
        handler1->handleRequest();

        return transferOutputs(chain_of_responsibility::output());
    }
    //! @brief Command.
    //! @return procedure output
    static std::ostringstream command()
    {
        namespace command = design_pattern::behavioral::command;
        using command::Command, command::ConcreteCommand, command::Invoker, command::Receiver;

        std::shared_ptr<ConcreteCommand> commands = std::make_shared<ConcreteCommand>(std::make_shared<Receiver>());

        Invoker invoker{};
        invoker.set(commands);
        invoker.confirm();

        return transferOutputs(command::output());
    }
    //! @brief Interpreter.
    //! @return procedure output
    static std::ostringstream interpreter()
    {
        namespace interpreter = design_pattern::behavioral::interpreter;
        using interpreter::AbstractExpression, interpreter::Context, interpreter::NonTerminalExpression,
            interpreter::TerminalExpression;

        std::shared_ptr<AbstractExpression> a = std::make_shared<TerminalExpression>("A"),
                                            b = std::make_shared<TerminalExpression>("B"),
                                            exp = std::make_shared<NonTerminalExpression>(a, b);

        std::shared_ptr<Context> context = std::make_shared<Context>();
        context->set("A", true);
        context->set("B", false);

        interpreter::output() << context->get("A") << " AND " << context->get("B");
        interpreter::output() << " = " << exp->interpret(context) << '\n';

        return transferOutputs(interpreter::output());
    }
    //! @brief Iterator.
    //! @return procedure output
    static std::ostringstream iterator()
    {
        namespace iterator = design_pattern::behavioral::iterator;
        using iterator::ConcreteAggregate, iterator::Iterator;

        constexpr std::uint32_t size = 5;
        std::shared_ptr<ConcreteAggregate> list = std::make_shared<ConcreteAggregate>(size);
        std::shared_ptr<Iterator> iter = list->createIterator();

        for (; !iter->isDone(); iter->next())
        {
            iterator::output() << "item value: " << iter->currentItem() << '\n';
        }

        return transferOutputs(iterator::output());
    }
    //! @brief Mediator.
    //! @return procedure output
    static std::ostringstream mediator()
    {
        namespace mediator = design_pattern::behavioral::mediator;
        using mediator::Colleague, mediator::ConcreteColleague, mediator::ConcreteMediator, mediator::Mediator;

        constexpr std::uint32_t id1 = 1, id2 = 2, id3 = 3;
        std::shared_ptr<Mediator> mediators = std::make_shared<ConcreteMediator>();
        std::shared_ptr<Colleague> c1 = std::make_shared<ConcreteColleague>(mediators, id1),
                                   c2 = std::make_shared<ConcreteColleague>(mediators, id2),
                                   c3 = std::make_shared<ConcreteColleague>(mediators, id3);

        mediators->add(c1);
        mediators->add(c2);
        mediators->add(c3);
        c1->send("hi!");
        c3->send("hello!");

        return transferOutputs(mediator::output());
    }
    //! @brief Memento.
    //! @return procedure output
    static std::ostringstream memento()
    {
        namespace memento = design_pattern::behavioral::memento;
        using memento::CareTaker, memento::Originator;

        constexpr int state1 = 1, state2 = 2, state3 = 3;
        std::shared_ptr<Originator> originator = std::make_shared<Originator>();
        std::shared_ptr<CareTaker> caretaker = std::make_shared<CareTaker>(originator);

        originator->setState(state1);
        caretaker->save();
        originator->setState(state2);
        caretaker->save();
        originator->setState(state3);
        caretaker->undo();

        memento::output() << "actual state is " << originator->getState() << '\n';

        return transferOutputs(memento::output());
    }
    //! @brief Observer.
    //! @return procedure output
    static std::ostringstream observer()
    {
        namespace observer = design_pattern::behavioral::observer;
        using observer::ConcreteObserver, observer::ConcreteSubject, observer::Subject;

        constexpr int state1 = 1, state2 = 2, state3 = 3;
        std::shared_ptr<ConcreteObserver> observer1 = std::make_shared<ConcreteObserver>(state1),
                                          observer2 = std::make_shared<ConcreteObserver>(state2);

        observer::output() << "observer1 state: " << observer1->getState() << '\n';
        observer::output() << "observer2 state: " << observer2->getState() << '\n';

        std::shared_ptr<Subject> subject = std::make_shared<ConcreteSubject>();
        subject->attach(observer1);
        subject->attach(observer2);
        subject->setState(state3);
        subject->notify();

        observer::output() << "observer1 state: " << observer1->getState() << '\n';
        observer::output() << "observer2 state: " << observer2->getState() << '\n';

        return transferOutputs(observer::output());
    }
    //! @brief State.
    //! @return procedure output
    static std::ostringstream state()
    {
        namespace state = design_pattern::behavioral::state;
        using state::ConcreteStateA, state::ConcreteStateB, state::Context;

        std::shared_ptr<Context> context = std::make_shared<Context>();

        context->setState(std::make_unique<ConcreteStateA>());
        context->request();

        context->setState(std::make_unique<ConcreteStateB>());
        context->request();

        return transferOutputs(state::output());
    }
    //! @brief Strategy.
    //! @return procedure output
    static std::ostringstream strategy()
    {
        namespace strategy = design_pattern::behavioral::strategy;
        using strategy::ConcreteStrategyA, strategy::ConcreteStrategyB, strategy::Context;

        Context contextA(std::make_unique<ConcreteStrategyA>());
        contextA.contextInterface();

        Context contextB(std::make_unique<ConcreteStrategyB>());
        contextB.contextInterface();

        return transferOutputs(strategy::output());
    }
    //! @brief Template method.
    //! @return procedure output
    static std::ostringstream templateMethod()
    {
        namespace template_method = design_pattern::behavioral::template_method;
        using template_method::AbstractClass, template_method::ConcreteClass;

        std::shared_ptr<AbstractClass> tm = std::make_shared<ConcreteClass>();
        tm->templateMethod();

        return transferOutputs(template_method::output());
    }
    //! @brief Visitor.
    //! @return procedure output
    static std::ostringstream visitor()
    {
        namespace visitor = design_pattern::behavioral::visitor;
        using visitor::ConcreteElementA, visitor::ConcreteElementB, visitor::ConcreteVisitor1,
            visitor::ConcreteVisitor2;

        std::shared_ptr<ConcreteElementA> elementA = std::make_shared<ConcreteElementA>();
        std::shared_ptr<ConcreteElementB> elementB = std::make_shared<ConcreteElementB>();
        ConcreteVisitor1 visitor1{};
        ConcreteVisitor2 visitor2{};

        elementA->accept(visitor1);
        elementA->accept(visitor2);
        elementB->accept(visitor1);
        elementB->accept(visitor2);

        return transferOutputs(visitor::output());
    }

private:
    //! @brief Transfer full outputs.
    //! @param outputs - original outputs
    //! @return full outputs
    static std::ostringstream transferOutputs(std::ostringstream& outputs)
    {
        std::ostringstream os(outputs.str());
        outputs.str("");
        outputs.clear();
        return os;
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
void updateChoice<BehavioralInstance>(const std::string& target);
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

    //! @brief Abstract factory.
    //! @return procedure output
    static std::ostringstream abstractFactory()
    {
        namespace abstract_factory = design_pattern::creational::abstract_factory;
        using abstract_factory::ConcreteFactoryX, abstract_factory::ConcreteFactoryY, abstract_factory::ProductA,
            abstract_factory::ProductB;

        std::shared_ptr<ConcreteFactoryX> factoryX = std::make_shared<ConcreteFactoryX>();
        std::shared_ptr<ConcreteFactoryY> factoryY = std::make_shared<ConcreteFactoryY>();

        std::unique_ptr<ProductA> p1 = factoryX->createProductA();
        abstract_factory::output() << "product: " << p1->getName() << '\n';

        std::unique_ptr<ProductA> p2 = factoryY->createProductA();
        abstract_factory::output() << "product: " << p2->getName() << '\n';

        std::unique_ptr<ProductB> p3 = factoryX->createProductB();
        abstract_factory::output() << "product: " << p3->getName() << '\n';

        std::unique_ptr<ProductB> p4 = factoryY->createProductB();
        abstract_factory::output() << "product: " << p4->getName() << '\n';

        return transferOutputs(abstract_factory::output());
    }
    //! @brief Builder.
    //! @return procedure output
    static std::ostringstream builder()
    {
        namespace builder = design_pattern::creational::builder;
        using builder::ConcreteBuilderX, builder::ConcreteBuilderY, builder::Director, builder::Product;

        Director director{};
        director.set(std::make_unique<ConcreteBuilderX>());
        director.construct();
        Product product1 = director.get();
        builder::output() << "1st product parts: " << product1.get() << '\n';

        director.set(std::make_unique<ConcreteBuilderY>());
        director.construct();
        Product product2 = director.get();
        builder::output() << "2nd product parts: " << product2.get() << '\n';

        return transferOutputs(builder::output());
    }
    //! @brief Factory method.
    //! @return procedure output
    static std::ostringstream factoryMethod()
    {
        namespace factory_method = design_pattern::creational::factory_method;
        using factory_method::ConcreteCreator, factory_method::Creator, factory_method::Product;

        std::shared_ptr<Creator> creator = std::make_shared<ConcreteCreator>();

        std::unique_ptr<Product> p1 = creator->createProductA();
        factory_method::output() << "product: " << p1->getName() << '\n';
        creator->removeProduct(p1);

        std::unique_ptr<Product> p2 = creator->createProductB();
        factory_method::output() << "product: " << p2->getName() << '\n';
        creator->removeProduct(p2);

        return transferOutputs(factory_method::output());
    }
    //! @brief Prototype.
    //! @return procedure output
    static std::ostringstream prototype()
    {
        namespace prototype = design_pattern::creational::prototype;
        using prototype::Client, prototype::Prototype;

        Client::init();

        std::unique_ptr<Prototype> prototype1 = Client::make(0);
        prototype::output() << "prototype: " << prototype1->type() << '\n';

        std::unique_ptr<Prototype> prototype2 = Client::make(1);
        prototype::output() << "prototype: " << prototype2->type() << '\n';

        Client::remove();

        return transferOutputs(prototype::output());
    }
    //! @brief Singleton.
    //! @return procedure output
    static std::ostringstream singleton()
    {
        namespace singleton = design_pattern::creational::singleton;
        using singleton::Singleton;

        Singleton::get()->tell();
        Singleton::restart();

        return transferOutputs(singleton::output());
    }

private:
    //! @brief Transfer full outputs.
    //! @param outputs - original outputs
    //! @return full outputs
    static std::ostringstream transferOutputs(std::ostringstream& outputs)
    {
        std::ostringstream os(outputs.str());
        outputs.str("");
        outputs.clear();
        return os;
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
void updateChoice<CreationalInstance>(const std::string& target);
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

    //! @brief Adapter.
    //! @return procedure output
    static std::ostringstream adapter()
    {
        namespace adapter = design_pattern::structural::adapter;
        using adapter::Adapter, adapter::Target;

        std::shared_ptr<Target> t = std::make_shared<Adapter>();
        t->request();

        return transferOutputs(adapter::output());
    }
    //! @brief Bridge.
    //! @return procedure output
    static std::ostringstream bridge()
    {
        namespace bridge = design_pattern::structural::bridge;
        using bridge::Abstraction, bridge::ConcreteImplementorA, bridge::ConcreteImplementorB, bridge::Implementor,
            bridge::RefinedAbstraction;

        std::unique_ptr<Abstraction> abstract1 =
            std::make_unique<RefinedAbstraction>(std::make_unique<ConcreteImplementorA>());
        abstract1->operation();

        std::unique_ptr<Abstraction> abstract2 =
            std::make_unique<RefinedAbstraction>(std::make_unique<ConcreteImplementorB>());
        abstract2->operation();

        return transferOutputs(bridge::output());
    }
    //! @brief Composite.
    //! @return procedure output
    static std::ostringstream composite()
    {
        namespace composite = design_pattern::structural::composite;
        using composite::Composite, composite::Leaf;

        constexpr std::uint32_t count = 5;
        Composite composites{};

        for (std::uint32_t i = 0; i < count; ++i)
        {
            composites.add(std::make_shared<Leaf>(i));
        }
        composites.remove(0);
        composites.operation();

        return transferOutputs(composite::output());
    }
    //! @brief Decorator.
    //! @return procedure output
    static std::ostringstream decorator()
    {
        namespace decorator = design_pattern::structural::decorator;
        using decorator::Component, decorator::ConcreteComponent, decorator::ConcreteDecoratorA,
            decorator::ConcreteDecoratorB;

        std::shared_ptr<ConcreteComponent> cc = std::make_shared<ConcreteComponent>();
        std::shared_ptr<ConcreteDecoratorA> da = std::make_shared<ConcreteDecoratorA>(cc);
        std::shared_ptr<ConcreteDecoratorB> db = std::make_shared<ConcreteDecoratorB>(da);

        std::shared_ptr<Component> component = db;
        component->operation();

        return transferOutputs(decorator::output());
    }
    //! @brief Facade.
    //! @return procedure output
    static std::ostringstream facade()
    {
        namespace facade = design_pattern::structural::facade;
        using facade::Facade;

        std::shared_ptr<Facade> facades = std::make_shared<Facade>();
        facades->operation1();
        facades->operation2();

        return transferOutputs(facade::output());
    }
    //! @brief Flyweight.
    //! @return procedure output
    static std::ostringstream flyweight()
    {
        namespace flyweight = design_pattern::structural::flyweight;
        using flyweight::FlyweightFactory;

        std::shared_ptr<FlyweightFactory> factory = std::make_shared<FlyweightFactory>();
        factory->getFlyweight(1)->operation();
        factory->getFlyweight(2)->operation();

        return transferOutputs(flyweight::output());
    }
    //! @brief Proxy.
    //! @return procedure output
    static std::ostringstream proxy()
    {
        namespace proxy = design_pattern::structural::proxy;
        using proxy::Proxy;

        std::shared_ptr<Proxy> proxies = std::make_shared<Proxy>();
        proxies->request();

        return transferOutputs(proxy::output());
    }

private:
    //! @brief Transfer full outputs.
    //! @param outputs - original outputs
    //! @return full outputs
    static std::ostringstream transferOutputs(std::ostringstream& outputs)
    {
        std::ostringstream os(outputs.str());
        outputs.str("");
        outputs.clear();
        return os;
    }
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
void updateChoice<StructuralInstance>(const std::string& target);
template <>
void runChoices<StructuralInstance>(const std::vector<std::string>& candidates);
} // namespace app_dp
} // namespace application
