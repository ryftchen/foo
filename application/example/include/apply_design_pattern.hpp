//! @file apply_design_pattern.hpp
//! @author ryftchen
//! @brief The declarations (apply_design_pattern) in the application module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#pragma once

#include <bitset>
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
    enum Type : uint8_t
    {
        behavioral,
        creational,
        structural
    };

    //! @brief Enumerate specific behavioral instances.
    enum BehavioralInstance : uint8_t
    {
        chainOfResponsibility,
        command,
        interpreter,
        iterator,
        mediator,
        memento,
        observer,
        state,
        strategy,
        templateMethod,
        visitor
    };
    //! @brief Store the maximum value of the BehavioralInstance enum.
    template <>
    struct Bottom<BehavioralInstance>
    {
        //! @brief Maximum value of the BehavioralInstance enum.
        static constexpr uint8_t value = 11;
    };

    //! @brief Enumerate specific creational instances.
    enum CreationalInstance : uint8_t
    {
        abstractFactory,
        builder,
        factoryMethod,
        prototype,
        singleton
    };
    //! @brief Store the maximum value of the CreationalInstance enum.
    template <>
    struct Bottom<CreationalInstance>
    {
        //! @brief Maximum value of the CreationalInstance enum.
        static constexpr uint8_t value = 5;
    };

    //! @brief Enumerate specific structural instances.
    enum StructuralInstance : uint8_t
    {
        adapter,
        bridge,
        composite,
        decorator,
        facade,
        flyweight,
        proxy
    };
    //! @brief Store the maximum value of the StructuralInstance enum.
    template <>
    struct Bottom<StructuralInstance>
    {
        //! @brief Maximum value of the StructuralInstance enum.
        static constexpr uint8_t value = 7;
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
    //! @brief The operator (<<) overloading of the Type enum.
    //! @param os - output stream object
    //! @param type - the specific value of Type enum
    //! @return reference of output stream object
    friend std::ostream& operator<<(std::ostream& os, const Type& type)
    {
        switch (type)
        {
            case Type::behavioral:
                os << "BEHAVIORAL";
                break;
            case Type::creational:
                os << "CREATIONAL";
                break;
            case Type::structural:
                os << "STRUCTURAL";
                break;
            default:
                os << "UNKNOWN: " << static_cast<std::underlying_type_t<Type>>(type);
        }
        return os;
    }
};

extern DesignPatternTask& getTask();

//! @brief Get the bit flags of the instance in design pattern tasks.
//! @tparam T - type of the instance
//! @return bit flags of the instance
template <typename T>
auto getBit()
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
//! @tparam T - type of the instance
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
        using design_pattern::behavioral::chain_of_responsibility::ConcreteHandler1;
        using design_pattern::behavioral::chain_of_responsibility::ConcreteHandler2;
        using design_pattern::behavioral::chain_of_responsibility::output;

        std::shared_ptr<ConcreteHandler1> handler1 = std::make_shared<ConcreteHandler1>();
        std::shared_ptr<ConcreteHandler2> handler2 = std::make_shared<ConcreteHandler2>();

        handler1->setHandler(handler2);
        handler1->handleRequest();

        std::ostringstream ret = std::ostringstream(output().str());
        output().str("");
        output().clear();
        return ret;
    }
    //! @brief Command.
    //! @return procedure output
    static std::ostringstream command()
    {
        using design_pattern::behavioral::command::Command;
        using design_pattern::behavioral::command::ConcreteCommand;
        using design_pattern::behavioral::command::Invoker;
        using design_pattern::behavioral::command::output;
        using design_pattern::behavioral::command::Receiver;

        std::shared_ptr<ConcreteCommand> command = std::make_shared<ConcreteCommand>((std::make_shared<Receiver>()));

        Invoker invoker;
        invoker.set(command);
        invoker.confirm();
        std::ostringstream ret = std::ostringstream(output().str());
        output().str("");
        output().clear();
        return ret;
    }
    //! @brief Interpreter.
    //! @return procedure output
    static std::ostringstream interpreter()
    {
        using design_pattern::behavioral::interpreter::AbstractExpression;
        using design_pattern::behavioral::interpreter::Context;
        using design_pattern::behavioral::interpreter::NonTerminalExpression;
        using design_pattern::behavioral::interpreter::output;
        using design_pattern::behavioral::interpreter::TerminalExpression;

        std::shared_ptr<AbstractExpression> a = std::make_shared<TerminalExpression>("A");
        std::shared_ptr<AbstractExpression> b = std::make_shared<TerminalExpression>("B");
        std::shared_ptr<AbstractExpression> exp = std::make_shared<NonTerminalExpression>(a, b);

        std::shared_ptr<Context> context = std::make_shared<Context>();
        context->set("A", true);
        context->set("B", false);

        output() << context->get("A") << " AND " << context->get("B");
        output() << " = " << exp->interpret(context) << std::endl;
        std::ostringstream ret = std::ostringstream(output().str());
        output().str("");
        output().clear();
        return ret;
    }
    //! @brief Iterator.
    //! @return procedure output
    static std::ostringstream iterator()
    {
        using design_pattern::behavioral::iterator::ConcreteAggregate;
        using design_pattern::behavioral::iterator::Iterator;
        using design_pattern::behavioral::iterator::output;

        constexpr uint32_t size = 5;
        std::shared_ptr<ConcreteAggregate> list = std::make_shared<ConcreteAggregate>(size);
        std::shared_ptr<Iterator> iter = list->createIterator();

        for (; !iter->isDone(); iter->next())
        {
            output() << "item value: " << iter->currentItem() << std::endl;
        }

        std::ostringstream ret = std::ostringstream(output().str());
        output().str("");
        output().clear();
        return ret;
    }
    //! @brief Mediator.
    //! @return procedure output
    static std::ostringstream mediator()
    {
        using design_pattern::behavioral::mediator::Colleague;
        using design_pattern::behavioral::mediator::ConcreteColleague;
        using design_pattern::behavioral::mediator::ConcreteMediator;
        using design_pattern::behavioral::mediator::Mediator;
        using design_pattern::behavioral::mediator::output;

        constexpr uint32_t id1 = 1, id2 = 2, id3 = 3;
        std::shared_ptr<Mediator> mediator = std::make_shared<ConcreteMediator>();
        std::shared_ptr<Colleague> c1 = std::make_shared<ConcreteColleague>(mediator, id1);
        std::shared_ptr<Colleague> c2 = std::make_shared<ConcreteColleague>(mediator, id2);
        std::shared_ptr<Colleague> c3 = std::make_shared<ConcreteColleague>(mediator, id3);

        mediator->add(c1);
        mediator->add(c2);
        mediator->add(c3);
        c1->send("Hi!");
        c3->send("Hello!");

        std::ostringstream ret = std::ostringstream(output().str());
        output().str("");
        output().clear();
        return ret;
    }
    //! @brief Memento.
    //! @return procedure output
    static std::ostringstream memento()
    {
        using design_pattern::behavioral::memento::CareTaker;
        using design_pattern::behavioral::memento::Originator;
        using design_pattern::behavioral::memento::output;

        constexpr int state1 = 1, state2 = 2, state3 = 3;
        std::shared_ptr<Originator> originator = std::make_shared<Originator>();
        std::shared_ptr<CareTaker> caretaker = std::make_shared<CareTaker>(originator);

        originator->setState(state1);
        caretaker->save();
        originator->setState(state2);
        caretaker->save();
        originator->setState(state3);
        caretaker->undo();

        output() << "actual state is " << originator->getState() << std::endl;

        std::ostringstream ret = std::ostringstream(output().str());
        output().str("");
        output().clear();
        return ret;
    }
    //! @brief Observer.
    //! @return procedure output
    static std::ostringstream observer()
    {
        using design_pattern::behavioral::observer::ConcreteObserver;
        using design_pattern::behavioral::observer::ConcreteSubject;
        using design_pattern::behavioral::observer::output;
        using design_pattern::behavioral::observer::Subject;

        constexpr int state1 = 1, state2 = 2, state3 = 3;
        std::shared_ptr<ConcreteObserver> observer1 = std::make_shared<ConcreteObserver>(state1);
        std::shared_ptr<ConcreteObserver> observer2 = std::make_shared<ConcreteObserver>(state2);

        output() << "observer1 state: " << observer1->getState() << std::endl;
        output() << "observer2 state: " << observer2->getState() << std::endl;

        std::shared_ptr<Subject> subject = std::make_shared<ConcreteSubject>();
        subject->attach(observer1);
        subject->attach(observer2);
        subject->setState(state3);
        subject->notify();

        output() << "observer1 state: " << observer1->getState() << std::endl;
        output() << "observer2 state: " << observer2->getState() << std::endl;

        std::ostringstream ret = std::ostringstream(output().str());
        output().str("");
        output().clear();
        return ret;
    }
    //! @brief State.
    //! @return procedure output
    static std::ostringstream state()
    {
        using design_pattern::behavioral::state::ConcreteStateA;
        using design_pattern::behavioral::state::ConcreteStateB;
        using design_pattern::behavioral::state::Context;
        using design_pattern::behavioral::state::output;

        std::shared_ptr<Context> context = std::make_shared<Context>();

        context->setState(std::make_unique<ConcreteStateA>());
        context->request();

        context->setState(std::make_unique<ConcreteStateB>());
        context->request();

        std::ostringstream ret = std::ostringstream(output().str());
        output().str("");
        output().clear();
        return ret;
    }
    //! @brief Strategy.
    //! @return procedure output
    static std::ostringstream strategy()
    {
        using design_pattern::behavioral::strategy::ConcreteStrategyA;
        using design_pattern::behavioral::strategy::ConcreteStrategyB;
        using design_pattern::behavioral::strategy::Context;
        using design_pattern::behavioral::strategy::output;

        Context contextA(std::make_unique<ConcreteStrategyA>());
        contextA.contextInterface();

        Context contextB(std::make_unique<ConcreteStrategyB>());
        contextB.contextInterface();

        std::ostringstream ret = std::ostringstream(output().str());
        output().str("");
        output().clear();
        return ret;
    }
    //! @brief Template method.
    //! @return procedure output
    static std::ostringstream templateMethod()
    {
        using design_pattern::behavioral::template_method::AbstractClass;
        using design_pattern::behavioral::template_method::ConcreteClass;
        using design_pattern::behavioral::template_method::output;

        std::shared_ptr<AbstractClass> tm = std::make_shared<ConcreteClass>();
        tm->templateMethod();

        std::ostringstream ret = std::ostringstream(output().str());
        output().str("");
        output().clear();
        return ret;
    }
    //! @brief Visitor.
    //! @return procedure output
    static std::ostringstream visitor()
    {
        using design_pattern::behavioral::visitor::ConcreteElementA;
        using design_pattern::behavioral::visitor::ConcreteElementB;
        using design_pattern::behavioral::visitor::ConcreteVisitor1;
        using design_pattern::behavioral::visitor::ConcreteVisitor2;
        using design_pattern::behavioral::visitor::output;

        std::shared_ptr<ConcreteElementA> elementA = std::make_shared<ConcreteElementA>();
        std::shared_ptr<ConcreteElementB> elementB = std::make_shared<ConcreteElementB>();
        ConcreteVisitor1 visitor1;
        ConcreteVisitor2 visitor2;

        elementA->accept(visitor1);
        elementA->accept(visitor2);
        elementB->accept(visitor1);
        elementB->accept(visitor2);

        std::ostringstream ret = std::ostringstream(output().str());
        output().str("");
        output().clear();
        return ret;
    }
};

//! @brief Pattern of behavioral.
class BehavioralPattern
{
public:
    //! @brief Construct a new BehavioralPattern object.
    BehavioralPattern();
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
extern void runBehavioral(const std::vector<std::string>& targets);
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
        using design_pattern::creational::abstract_factory::ConcreteFactoryX;
        using design_pattern::creational::abstract_factory::ConcreteFactoryY;
        using design_pattern::creational::abstract_factory::output;
        using design_pattern::creational::abstract_factory::ProductA;
        using design_pattern::creational::abstract_factory::ProductB;

        std::shared_ptr<ConcreteFactoryX> factoryX = std::make_shared<ConcreteFactoryX>();
        std::shared_ptr<ConcreteFactoryY> factoryY = std::make_shared<ConcreteFactoryY>();

        std::unique_ptr<ProductA> p1 = factoryX->createProductA();
        output() << "product: " << p1->getName() << std::endl;

        std::unique_ptr<ProductA> p2 = factoryY->createProductA();
        output() << "product: " << p2->getName() << std::endl;

        std::unique_ptr<ProductB> p3 = factoryX->createProductB();
        output() << "product: " << p3->getName() << std::endl;

        std::unique_ptr<ProductB> p4 = factoryY->createProductB();
        output() << "product: " << p4->getName() << std::endl;

        std::ostringstream ret = std::ostringstream(output().str());
        output().str("");
        output().clear();
        return ret;
    }
    //! @brief Builder.
    //! @return procedure output
    static std::ostringstream builder()
    {
        using design_pattern::creational::builder::ConcreteBuilderX;
        using design_pattern::creational::builder::ConcreteBuilderY;
        using design_pattern::creational::builder::Director;
        using design_pattern::creational::builder::output;
        using design_pattern::creational::builder::Product;

        Director director;

        director.set(std::make_unique<ConcreteBuilderX>());
        director.construct();
        Product product1 = director.get();
        output() << "1st product parts: " << product1.get() << std::endl;

        director.set(std::make_unique<ConcreteBuilderY>());
        director.construct();
        Product product2 = director.get();
        output() << "2nd product parts: " << product2.get() << std::endl;

        std::ostringstream ret = std::ostringstream(output().str());
        output().str("");
        output().clear();
        return ret;
    }
    //! @brief Factory method.
    //! @return procedure output
    static std::ostringstream factoryMethod()
    {
        using design_pattern::creational::factory_method::ConcreteCreator;
        using design_pattern::creational::factory_method::Creator;
        using design_pattern::creational::factory_method::output;
        using design_pattern::creational::factory_method::Product;

        std::shared_ptr<Creator> creator = std::make_shared<ConcreteCreator>();

        std::unique_ptr<Product> p1 = creator->createProductA();
        output() << "product: " << p1->getName() << std::endl;
        creator->removeProduct(p1);

        std::unique_ptr<Product> p2 = creator->createProductB();
        output() << "product: " << p2->getName() << std::endl;
        creator->removeProduct(p2);

        std::ostringstream ret = std::ostringstream(output().str());
        output().str("");
        output().clear();
        return ret;
    }
    //! @brief Prototype.
    //! @return procedure output
    static std::ostringstream prototype()
    {
        using design_pattern::creational::prototype::Client;
        using design_pattern::creational::prototype::output;
        using design_pattern::creational::prototype::Prototype;

        Client::init();

        std::unique_ptr<Prototype> prototype1 = Client::make(0);
        output() << "prototype: " << prototype1->type() << std::endl;

        std::unique_ptr<Prototype> prototype2 = Client::make(1);
        output() << "prototype: " << prototype2->type() << std::endl;

        Client::remove();

        std::ostringstream ret = std::ostringstream(output().str());
        output().str("");
        output().clear();
        return ret;
    }
    //! @brief Singleton.
    //! @return procedure output
    static std::ostringstream singleton()
    {
        using design_pattern::creational::singleton::output;
        using design_pattern::creational::singleton::Singleton;

        Singleton::get()->tell();
        Singleton::restart();

        std::ostringstream ret = std::ostringstream(output().str());
        output().str("");
        output().clear();
        return ret;
    }
};

//! @brief Pattern of creational.
class CreationalPattern
{
public:
    //! @brief Construct a new CreationalPattern object.
    CreationalPattern();
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
extern void runCreational(const std::vector<std::string>& targets);
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
        using design_pattern::structural::adapter::Adapter;
        using design_pattern::structural::adapter::output;
        using design_pattern::structural::adapter::Target;

        std::shared_ptr<Target> t = std::make_shared<Adapter>();
        t->request();

        std::ostringstream ret = std::ostringstream(output().str());
        output().str("");
        output().clear();
        return ret;
    }
    //! @brief Bridge.
    //! @return procedure output
    static std::ostringstream bridge()
    {
        using design_pattern::structural::bridge::Abstraction;
        using design_pattern::structural::bridge::ConcreteImplementorA;
        using design_pattern::structural::bridge::ConcreteImplementorB;
        using design_pattern::structural::bridge::Implementor;
        using design_pattern::structural::bridge::output;
        using design_pattern::structural::bridge::RefinedAbstraction;

        std::unique_ptr<Abstraction> abstract1 =
            std::make_unique<RefinedAbstraction>(std::make_unique<ConcreteImplementorA>());
        abstract1->operation();

        std::unique_ptr<Abstraction> abstract2 =
            std::make_unique<RefinedAbstraction>(std::make_unique<ConcreteImplementorB>());
        abstract2->operation();

        std::ostringstream ret = std::ostringstream(output().str());
        output().str("");
        output().clear();
        return ret;
    }
    //! @brief Composite.
    //! @return procedure output
    static std::ostringstream composite()
    {
        using design_pattern::structural::composite::Composite;
        using design_pattern::structural::composite::Leaf;
        using design_pattern::structural::composite::output;

        constexpr uint32_t count = 5;
        Composite composite;

        for (uint32_t i = 0; i < count; ++i)
        {
            composite.add(std::make_shared<Leaf>(i));
        }
        composite.remove(0);
        composite.operation();

        std::ostringstream ret = std::ostringstream(output().str());
        output().str("");
        output().clear();
        return ret;
    }
    //! @brief Decorator.
    //! @return procedure output
    static std::ostringstream decorator()
    {
        using design_pattern::structural::decorator::Component;
        using design_pattern::structural::decorator::ConcreteComponent;
        using design_pattern::structural::decorator::ConcreteDecoratorA;
        using design_pattern::structural::decorator::ConcreteDecoratorB;
        using design_pattern::structural::decorator::output;

        std::shared_ptr<ConcreteComponent> cc = std::make_shared<ConcreteComponent>();
        std::shared_ptr<ConcreteDecoratorA> da = std::make_shared<ConcreteDecoratorA>(cc);
        std::shared_ptr<ConcreteDecoratorB> db = std::make_shared<ConcreteDecoratorB>(da);

        std::shared_ptr<Component> component = db;
        component->operation();

        std::ostringstream ret = std::ostringstream(output().str());
        output().str("");
        output().clear();
        return ret;
    }
    //! @brief Facade.
    //! @return procedure output
    static std::ostringstream facade()
    {
        using design_pattern::structural::facade::Facade;
        using design_pattern::structural::facade::output;

        std::shared_ptr<Facade> facade = std::make_shared<Facade>();
        facade->operation1();
        facade->operation2();

        std::ostringstream ret = std::ostringstream(output().str());
        output().str("");
        output().clear();
        return ret;
    }
    //! @brief Flyweight.
    //! @return procedure output
    static std::ostringstream flyweight()
    {
        using design_pattern::structural::flyweight::FlyweightFactory;
        using design_pattern::structural::flyweight::output;

        std::shared_ptr<FlyweightFactory> factory = std::make_shared<FlyweightFactory>();
        factory->getFlyweight(1)->operation();
        factory->getFlyweight(2)->operation();

        std::ostringstream ret = std::ostringstream(output().str());
        output().str("");
        output().clear();
        return ret;
    }
    //! @brief Proxy.
    //! @return procedure output
    static std::ostringstream proxy()
    {
        using design_pattern::structural::proxy::output;
        using design_pattern::structural::proxy::Proxy;

        std::shared_ptr<Proxy> proxy = std::make_shared<Proxy>();
        proxy->request();

        std::ostringstream ret = std::ostringstream(output().str());
        output().str("");
        output().clear();
        return ret;
    }
};

//! @brief Pattern of structural.
class StructuralPattern
{
public:
    //! @brief Construct a new StructuralPattern object.
    StructuralPattern();
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
extern void runStructural(const std::vector<std::string>& targets);
extern void updateStructuralTask(const std::string& target);
} // namespace application::app_dp
