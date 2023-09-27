//! @file apply_design_pattern.hpp
//! @author ryftchen
//! @brief The declarations (apply_design_pattern) in the application module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen. All rights reserved.

#pragma once

#ifndef __PRECOMPILED_HEADER
#include <bitset>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER
#include "design_pattern/include/behavioral.hpp"
#include "design_pattern/include/creational.hpp"
#include "design_pattern/include/structural.hpp"

//! @brief Design-pattern-applying-related functions in the application module.
namespace application::app_dp
{
//! @brief Manage design pattern tasks.
class DesignPatternTask
{
public:
    //! @brief Represent the maximum value of an enum.
    //! @tparam T - type of specific enum
    template <class T>
    struct Bottom;

    //! @brief Enumerate specific design pattern tasks.
    enum Category : std::uint8_t
    {
        //! @brief Behavioral.
        behavioral,
        //! @brief Creational.
        creational,
        //! @brief Structural.
        structural
    };

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

    //! @brief Bit flags for managing behavioral instances.
    std::bitset<Bottom<BehavioralInstance>::value> behavioralBit;
    //! @brief Bit flags for managing creational instances.
    std::bitset<Bottom<CreationalInstance>::value> creationalBit;
    //! @brief Bit flags for managing structural instances.
    std::bitset<Bottom<StructuralInstance>::value> structuralBit;

    //! @brief Check whether any design pattern tasks do not exist.
    //! @return any design pattern tasks do not exist or exist
    [[nodiscard]] inline bool empty() const
    {
        return (behavioralBit.none() && creationalBit.none() && structuralBit.none());
    }
    //! @brief Reset bit flags that manage design pattern tasks.
    inline void reset()
    {
        behavioralBit.reset();
        creationalBit.reset();
        structuralBit.reset();
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
                os << "UNKNOWN: " << static_cast<std::underlying_type_t<Category>>(cat);
        }
        return os;
    }
};
extern DesignPatternTask& getTask();

//! @brief Get the bit flags of the instance in design pattern tasks.
//! @tparam T - type of instance
//! @return reference of the instance bit flags
template <typename T>
auto& getBit()
{
    if constexpr (std::is_same_v<T, DesignPatternTask::BehavioralInstance>)
    {
        return getTask().behavioralBit;
    }
    else if constexpr (std::is_same_v<T, DesignPatternTask::CreationalInstance>)
    {
        return getTask().creationalBit;
    }
    else if constexpr (std::is_same_v<T, DesignPatternTask::StructuralInstance>)
    {
        return getTask().structuralBit;
    }
}

//! @brief Set the bit flags of the instance in design pattern tasks.
//! @tparam T - type of instance
//! @param index - instance index
template <typename T>
void setBit(const int index)
{
    if constexpr (std::is_same_v<T, DesignPatternTask::BehavioralInstance>)
    {
        getTask().behavioralBit.set(DesignPatternTask::BehavioralInstance(index));
    }
    else if constexpr (std::is_same_v<T, DesignPatternTask::CreationalInstance>)
    {
        getTask().creationalBit.set(DesignPatternTask::CreationalInstance(index));
    }
    else if constexpr (std::is_same_v<T, DesignPatternTask::StructuralInstance>)
    {
        getTask().structuralBit.set(DesignPatternTask::StructuralInstance(index));
    }
}

//! @brief Apply behavioral.
namespace behavioral
{
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
        using chain_of_responsibility::ConcreteHandler1;
        using chain_of_responsibility::ConcreteHandler2;

        std::shared_ptr<ConcreteHandler1> handler1 = std::make_shared<ConcreteHandler1>();
        std::shared_ptr<ConcreteHandler2> handler2 = std::make_shared<ConcreteHandler2>();

        handler1->setHandler(handler2);
        handler1->handleRequest();

        std::ostringstream os = std::ostringstream(chain_of_responsibility::output().str());
        chain_of_responsibility::output().str("");
        chain_of_responsibility::output().clear();
        return os;
    }
    //! @brief Command.
    //! @return procedure output
    static std::ostringstream command()
    {
        namespace command = design_pattern::behavioral::command;
        using command::Command;
        using command::ConcreteCommand;
        using command::Invoker;
        using command::Receiver;

        std::shared_ptr<ConcreteCommand> commands = std::make_shared<ConcreteCommand>((std::make_shared<Receiver>()));

        Invoker invoker;
        invoker.set(commands);
        invoker.confirm();

        std::ostringstream os = std::ostringstream(command::output().str());
        command::output().str("");
        command::output().clear();
        return os;
    }
    //! @brief Interpreter.
    //! @return procedure output
    static std::ostringstream interpreter()
    {
        namespace interpreter = design_pattern::behavioral::interpreter;
        using interpreter::AbstractExpression;
        using interpreter::Context;
        using interpreter::NonTerminalExpression;
        using interpreter::TerminalExpression;

        std::shared_ptr<AbstractExpression> a = std::make_shared<TerminalExpression>("A");
        std::shared_ptr<AbstractExpression> b = std::make_shared<TerminalExpression>("B");
        std::shared_ptr<AbstractExpression> exp = std::make_shared<NonTerminalExpression>(a, b);

        std::shared_ptr<Context> context = std::make_shared<Context>();
        context->set("A", true);
        context->set("B", false);

        interpreter::output() << context->get("A") << " AND " << context->get("B");
        interpreter::output() << " = " << exp->interpret(context) << '\n';

        std::ostringstream os = std::ostringstream(interpreter::output().str());
        interpreter::output().str("");
        interpreter::output().clear();
        return os;
    }
    //! @brief Iterator.
    //! @return procedure output
    static std::ostringstream iterator()
    {
        namespace iterator = design_pattern::behavioral::iterator;
        using iterator::ConcreteAggregate;
        using iterator::Iterator;

        constexpr std::uint32_t size = 5;
        std::shared_ptr<ConcreteAggregate> list = std::make_shared<ConcreteAggregate>(size);
        std::shared_ptr<Iterator> iter = list->createIterator();

        for (; !iter->isDone(); iter->next())
        {
            iterator::output() << "item value: " << iter->currentItem() << '\n';
        }

        std::ostringstream os = std::ostringstream(iterator::output().str());
        iterator::output().str("");
        iterator::output().clear();
        return os;
    }
    //! @brief Mediator.
    //! @return procedure output
    static std::ostringstream mediator()
    {
        namespace mediator = design_pattern::behavioral::mediator;
        using mediator::Colleague;
        using mediator::ConcreteColleague;
        using mediator::ConcreteMediator;
        using mediator::Mediator;

        constexpr std::uint32_t id1 = 1, id2 = 2, id3 = 3;
        std::shared_ptr<Mediator> mediators = std::make_shared<ConcreteMediator>();
        std::shared_ptr<Colleague> c1 = std::make_shared<ConcreteColleague>(mediators, id1);
        std::shared_ptr<Colleague> c2 = std::make_shared<ConcreteColleague>(mediators, id2);
        std::shared_ptr<Colleague> c3 = std::make_shared<ConcreteColleague>(mediators, id3);

        mediators->add(c1);
        mediators->add(c2);
        mediators->add(c3);
        c1->send("hi!");
        c3->send("hello!");

        std::ostringstream os = std::ostringstream(mediator::output().str());
        mediator::output().str("");
        mediator::output().clear();
        return os;
    }
    //! @brief Memento.
    //! @return procedure output
    static std::ostringstream memento()
    {
        namespace memento = design_pattern::behavioral::memento;
        using memento::CareTaker;
        using memento::Originator;

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

        std::ostringstream os = std::ostringstream(memento::output().str());
        memento::output().str("");
        memento::output().clear();
        return os;
    }
    //! @brief Observer.
    //! @return procedure output
    static std::ostringstream observer()
    {
        namespace observer = design_pattern::behavioral::observer;
        using observer::ConcreteObserver;
        using observer::ConcreteSubject;
        using observer::Subject;

        constexpr int state1 = 1, state2 = 2, state3 = 3;
        std::shared_ptr<ConcreteObserver> observer1 = std::make_shared<ConcreteObserver>(state1);
        std::shared_ptr<ConcreteObserver> observer2 = std::make_shared<ConcreteObserver>(state2);

        observer::output() << "observer1 state: " << observer1->getState() << '\n';
        observer::output() << "observer2 state: " << observer2->getState() << '\n';

        std::shared_ptr<Subject> subject = std::make_shared<ConcreteSubject>();
        subject->attach(observer1);
        subject->attach(observer2);
        subject->setState(state3);
        subject->notify();

        observer::output() << "observer1 state: " << observer1->getState() << '\n';
        observer::output() << "observer2 state: " << observer2->getState() << '\n';

        std::ostringstream os = std::ostringstream(observer::output().str());
        observer::output().str("");
        observer::output().clear();
        return os;
    }
    //! @brief State.
    //! @return procedure output
    static std::ostringstream state()
    {
        namespace state = design_pattern::behavioral::state;
        using state::ConcreteStateA;
        using state::ConcreteStateB;
        using state::Context;

        std::shared_ptr<Context> context = std::make_shared<Context>();

        context->setState(std::make_unique<ConcreteStateA>());
        context->request();

        context->setState(std::make_unique<ConcreteStateB>());
        context->request();

        std::ostringstream os = std::ostringstream(state::output().str());
        state::output().str("");
        state::output().clear();
        return os;
    }
    //! @brief Strategy.
    //! @return procedure output
    static std::ostringstream strategy()
    {
        namespace strategy = design_pattern::behavioral::strategy;
        using strategy::ConcreteStrategyA;
        using strategy::ConcreteStrategyB;
        using strategy::Context;

        Context contextA(std::make_unique<ConcreteStrategyA>());
        contextA.contextInterface();

        Context contextB(std::make_unique<ConcreteStrategyB>());
        contextB.contextInterface();

        std::ostringstream os = std::ostringstream(strategy::output().str());
        strategy::output().str("");
        strategy::output().clear();
        return os;
    }
    //! @brief Template method.
    //! @return procedure output
    static std::ostringstream templateMethod()
    {
        namespace template_method = design_pattern::behavioral::template_method;
        using template_method::AbstractClass;
        using template_method::ConcreteClass;

        std::shared_ptr<AbstractClass> tm = std::make_shared<ConcreteClass>();
        tm->templateMethod();

        std::ostringstream os = std::ostringstream(template_method::output().str());
        template_method::output().str("");
        template_method::output().clear();
        return os;
    }
    //! @brief Visitor.
    //! @return procedure output
    static std::ostringstream visitor()
    {
        namespace visitor = design_pattern::behavioral::visitor;
        using visitor::ConcreteElementA;
        using visitor::ConcreteElementB;
        using visitor::ConcreteVisitor1;
        using visitor::ConcreteVisitor2;

        std::shared_ptr<ConcreteElementA> elementA = std::make_shared<ConcreteElementA>();
        std::shared_ptr<ConcreteElementB> elementB = std::make_shared<ConcreteElementB>();
        ConcreteVisitor1 visitor1;
        ConcreteVisitor2 visitor2;

        elementA->accept(visitor1);
        elementA->accept(visitor2);
        elementB->accept(visitor1);
        elementB->accept(visitor2);

        std::ostringstream os = std::ostringstream(visitor::output().str());
        visitor::output().str("");
        visitor::output().clear();
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
extern void runBehavioralTasks(const std::vector<std::string>& targets);
extern void updateBehavioralTask(const std::string& target);

//! @brief Apply creational.
namespace creational
{
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
        using abstract_factory::ConcreteFactoryX;
        using abstract_factory::ConcreteFactoryY;
        using abstract_factory::ProductA;
        using abstract_factory::ProductB;

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

        std::ostringstream os = std::ostringstream(abstract_factory::output().str());
        abstract_factory::output().str("");
        abstract_factory::output().clear();
        return os;
    }
    //! @brief Builder.
    //! @return procedure output
    static std::ostringstream builder()
    {
        namespace builder = design_pattern::creational::builder;
        using builder::ConcreteBuilderX;
        using builder::ConcreteBuilderY;
        using builder::Director;
        using builder::Product;

        Director director;
        director.set(std::make_unique<ConcreteBuilderX>());
        director.construct();
        Product product1 = director.get();
        builder::output() << "1st product parts: " << product1.get() << '\n';

        director.set(std::make_unique<ConcreteBuilderY>());
        director.construct();
        Product product2 = director.get();
        builder::output() << "2nd product parts: " << product2.get() << '\n';

        std::ostringstream os = std::ostringstream(builder::output().str());
        builder::output().str("");
        builder::output().clear();
        return os;
    }
    //! @brief Factory method.
    //! @return procedure output
    static std::ostringstream factoryMethod()
    {
        namespace factory_method = design_pattern::creational::factory_method;
        using factory_method::ConcreteCreator;
        using factory_method::Creator;
        using factory_method::Product;

        std::shared_ptr<Creator> creator = std::make_shared<ConcreteCreator>();

        std::unique_ptr<Product> p1 = creator->createProductA();
        factory_method::output() << "product: " << p1->getName() << '\n';
        creator->removeProduct(p1);

        std::unique_ptr<Product> p2 = creator->createProductB();
        factory_method::output() << "product: " << p2->getName() << '\n';
        creator->removeProduct(p2);

        std::ostringstream os = std::ostringstream(factory_method::output().str());
        factory_method::output().str("");
        factory_method::output().clear();
        return os;
    }
    //! @brief Prototype.
    //! @return procedure output
    static std::ostringstream prototype()
    {
        namespace prototype = design_pattern::creational::prototype;
        using prototype::Client;
        using prototype::Prototype;

        Client::init();

        std::unique_ptr<Prototype> prototype1 = Client::make(0);
        prototype::output() << "prototype: " << prototype1->type() << '\n';

        std::unique_ptr<Prototype> prototype2 = Client::make(1);
        prototype::output() << "prototype: " << prototype2->type() << '\n';

        Client::remove();

        std::ostringstream os = std::ostringstream(prototype::output().str());
        prototype::output().str("");
        prototype::output().clear();
        return os;
    }
    //! @brief Singleton.
    //! @return procedure output
    static std::ostringstream singleton()
    {
        namespace singleton = design_pattern::creational::singleton;
        using singleton::Singleton;

        Singleton::get()->tell();
        Singleton::restart();

        std::ostringstream os = std::ostringstream(singleton::output().str());
        singleton::output().str("");
        singleton::output().clear();
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
extern void runCreationalTasks(const std::vector<std::string>& targets);
extern void updateCreationalTask(const std::string& target);

//! @brief Apply structural.
namespace structural
{
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
        using adapter::Adapter;
        using adapter::Target;

        std::shared_ptr<Target> t = std::make_shared<Adapter>();
        t->request();

        std::ostringstream os = std::ostringstream(adapter::output().str());
        adapter::output().str("");
        adapter::output().clear();
        return os;
    }
    //! @brief Bridge.
    //! @return procedure output
    static std::ostringstream bridge()
    {
        namespace bridge = design_pattern::structural::bridge;
        using bridge::Abstraction;
        using bridge::ConcreteImplementorA;
        using bridge::ConcreteImplementorB;
        using bridge::Implementor;
        using bridge::RefinedAbstraction;

        std::unique_ptr<Abstraction> abstract1 =
            std::make_unique<RefinedAbstraction>(std::make_unique<ConcreteImplementorA>());
        abstract1->operation();

        std::unique_ptr<Abstraction> abstract2 =
            std::make_unique<RefinedAbstraction>(std::make_unique<ConcreteImplementorB>());
        abstract2->operation();

        std::ostringstream os = std::ostringstream(bridge::output().str());
        bridge::output().str("");
        bridge::output().clear();
        return os;
    }
    //! @brief Composite.
    //! @return procedure output
    static std::ostringstream composite()
    {
        namespace composite = design_pattern::structural::composite;
        using composite::Composite;
        using composite::Leaf;

        constexpr std::uint32_t count = 5;
        Composite composites;

        for (std::uint32_t i = 0; i < count; ++i)
        {
            composites.add(std::make_shared<Leaf>(i));
        }
        composites.remove(0);
        composites.operation();

        std::ostringstream os = std::ostringstream(composite::output().str());
        composite::output().str("");
        composite::output().clear();
        return os;
    }
    //! @brief Decorator.
    //! @return procedure output
    static std::ostringstream decorator()
    {
        namespace decorator = design_pattern::structural::decorator;
        using decorator::Component;
        using decorator::ConcreteComponent;
        using decorator::ConcreteDecoratorA;
        using decorator::ConcreteDecoratorB;

        std::shared_ptr<ConcreteComponent> cc = std::make_shared<ConcreteComponent>();
        std::shared_ptr<ConcreteDecoratorA> da = std::make_shared<ConcreteDecoratorA>(cc);
        std::shared_ptr<ConcreteDecoratorB> db = std::make_shared<ConcreteDecoratorB>(da);

        std::shared_ptr<Component> component = db;
        component->operation();

        std::ostringstream os = std::ostringstream(decorator::output().str());
        decorator::output().str("");
        decorator::output().clear();
        return os;
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

        std::ostringstream os = std::ostringstream(facade::output().str());
        facade::output().str("");
        facade::output().clear();
        return os;
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

        std::ostringstream os = std::ostringstream(flyweight::output().str());
        flyweight::output().str("");
        flyweight::output().clear();
        return os;
    }
    //! @brief Proxy.
    //! @return procedure output
    static std::ostringstream proxy()
    {
        namespace proxy = design_pattern::structural::proxy;
        using proxy::Proxy;

        std::shared_ptr<Proxy> proxies = std::make_shared<Proxy>();
        proxies->request();

        std::ostringstream os = std::ostringstream(proxy::output().str());
        proxy::output().str("");
        proxy::output().clear();
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
extern void runStructuralTasks(const std::vector<std::string>& targets);
extern void updateStructuralTask(const std::string& target);
} // namespace application::app_dp
