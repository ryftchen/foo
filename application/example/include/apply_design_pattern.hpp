//! @file apply_design_pattern.hpp
//! @author ryftchen
//! @brief The declarations (apply_design_pattern) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include "design_pattern/include/behavioral.hpp"
#include "design_pattern/include/creational.hpp"
#include "design_pattern/include/structural.hpp"

//! @brief The application module.
namespace application // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Design-pattern-applying-related functions in the application module.
namespace app_dp
{
//! @brief Apply behavioral.
namespace behavioral
{
//! @brief The version used to apply.
const char* const version = design_pattern::behavioral::version();

//! @brief Showcase for behavioral instances.
class Showcase
{
public:
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
        const std::shared_ptr<AbstractExpression> a = std::make_shared<TerminalExpression>("A");
        const std::shared_ptr<AbstractExpression> b = std::make_shared<TerminalExpression>("B");
        const std::shared_ptr<AbstractExpression> exp = std::make_shared<NonTerminalExpression>(a, b);

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

        for (const std::shared_ptr<Iterator> iter = list->createIterator(); !iter->isDone(); iter->next())
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
        constexpr std::uint32_t id1 = 1;
        constexpr std::uint32_t id2 = 2;
        constexpr std::uint32_t id3 = 3;
        const std::shared_ptr<Mediator> mediators = std::make_shared<ConcreteMediator>();
        const std::shared_ptr<Colleague> c1 = std::make_shared<ConcreteColleague>(mediators, id1);
        const std::shared_ptr<Colleague> c2 = std::make_shared<ConcreteColleague>(mediators, id2);
        const std::shared_ptr<Colleague> c3 = std::make_shared<ConcreteColleague>(mediators, id3);

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
        constexpr int state1 = 1;
        constexpr int state2 = 2;
        constexpr int state3 = 3;
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
        constexpr int state1 = 1;
        constexpr int state2 = 2;
        constexpr int state3 = 3;
        const std::shared_ptr<ConcreteObserver> observer1 = std::make_shared<ConcreteObserver>(state1);
        const std::shared_ptr<ConcreteObserver> observer2 = std::make_shared<ConcreteObserver>(state2);

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
} // namespace behavioral
extern void applyingBehavioral(const std::vector<std::string>& candidates);

//! @brief Apply creational.
namespace creational
{
//! @brief The version used to apply.
const char* const version = design_pattern::creational::version();

//! @brief Showcase for creational instances.
class Showcase
{
public:
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
        const Product product1 = director.get();
        output() << "1st product parts: " << product1.get() << '\n';

        director.set(std::make_unique<ConcreteBuilderY>());
        director.construct();
        const Product product2 = director.get();
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
} // namespace creational
extern void applyingCreational(const std::vector<std::string>& candidates);

//! @brief Apply structural.
namespace structural
{
//! @brief The version used to apply.
const char* const version = design_pattern::structural::version();

//! @brief Showcase for structural instances.
class Showcase
{
public:
    // NOLINTBEGIN(google-build-using-namespace)
    //! @brief Adapter.
    //! @return procedure output
    static std::ostringstream adapter()
    {
        using namespace design_pattern::structural::adapter;
        const std::shared_ptr<Target> adp = std::make_shared<Adapter>();

        adp->request();
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
} // namespace structural
extern void applyingStructural(const std::vector<std::string>& candidates);
} // namespace app_dp
} // namespace application
