//! @file structural.hpp
//! @author ryftchen
//! @brief The declarations (structural) in the design pattern module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023

#pragma once

#include <map>
#include <memory>
#include <sstream>
#include <vector>

//! @brief Structural-related functions in the design pattern module.
namespace design_pattern::structural
{
//! @brief The adapter pattern.
namespace adapter
{
extern std::ostringstream& output();

//! @brief The specific interface that the client uses.
class Target
{
public:
    //! @brief Destroy the Target object.
    virtual ~Target() = default;

    //! @brief Generate a request.
    virtual void request() = 0;
};

//! @brief Get calls that client makes on the target because of the adapter.
class Adaptee
{
public:
    //! @brief The specific request.
    static void specificRequest() { output() << "specific request\n"; }
};

//! @brief Delegate the call to an adaptee when getting a method call.
class Adapter : public Target
{
public:
    //! @brief Construct a new Adapter object.
    Adapter() : adaptee(std::make_unique<Adaptee>()) {}
    //! @brief Destroy the Adapter object.
    ~Adapter() override { adaptee.reset(); }

    //! @brief Generate a request.
    void request() override { adaptee->specificRequest(); }

private:
    //! @brief
    std::unique_ptr<Adaptee> adaptee;
};

} // namespace adapter

//! @brief The bridge pattern.
namespace bridge
{
extern std::ostringstream& output();

//! @brief The interface for implementation.
class Implementor
{
public:
    //! @brief Destroy the Implementor object.
    virtual ~Implementor() = default;

    //! @brief Implemented action.
    virtual void action() = 0;
};

//! @brief The concrete implementor.
class ConcreteImplementorA : public Implementor
{
public:
    //! @brief Destroy the ConcreteImplementorA object.
    ~ConcreteImplementorA() override = default;

    //! @brief Implemented action.
    void action() override { output() << "concrete implementor A\n"; }
};

//! @brief The concrete implementor.
class ConcreteImplementorB : public Implementor
{
public:
    //! @brief Destroy the ConcreteImplementorB object.
    ~ConcreteImplementorB() override = default;

    //! @brief Implemented action.
    void action() override { output() << "concrete implementor B\n"; }
};

//! @brief The interface for abstraction.
class Abstraction
{
public:
    //! @brief Destroy the Abstraction object.
    virtual ~Abstraction() = default;

    //! @brief Abstracted operation.
    virtual void operation() = 0;
};

//! @brief Extend abstraction’s interface.
class RefinedAbstraction : public Abstraction
{
public:
    //! @brief Construct a new RefinedAbstraction object
    //! @param implementor - target implementor
    explicit RefinedAbstraction(std::unique_ptr<Implementor> implementor) : implementor(std::move(implementor)) {}
    //! @brief Destroy the RefinedAbstraction object.
    ~RefinedAbstraction() override = default;

    //! @brief Abstracted operation.
    void operation() override { return implementor->action(); }

private:
    //! @brief The implementor.
    std::unique_ptr<Implementor> implementor;
};
} // namespace bridge

//! @brief The composite pattern.
namespace composite
{
extern std::ostringstream& output();

//! @brief Both the composite and the leaf nodes are in the composition.
class Component
{
public:
    //! @brief Destroy the Component object.
    virtual ~Component() = default;

    //! @brief Get the child component by index.
    virtual std::shared_ptr<Component> getChild(const std::uint32_t /*unused*/) { return nullptr; }
    //! @brief Add the child component.
    virtual void add(const std::shared_ptr<Component>& /*unused*/) {}
    //! @brief Remove the child component by index.
    virtual void remove(const std::uint32_t /*unused*/) {}
    //! @brief Execute all child components' operations.
    virtual void operation() = 0;
};

//! @brief The behavior of the components having children.
class Composite : public Component
{
public:
    //! @brief Destroy the Composite object.
    ~Composite() override
    {
        std::for_each(
            children.begin(),
            children.end(),
            [](auto& component)
            {
                component.reset();
            });
    }

    //! @brief Get the child component by index.
    //! @param index - child component index
    //! @return child component
    std::shared_ptr<Component> getChild(const std::uint32_t index) override { return children[index]; }
    //! @brief Add the child component.
    //! @param component - child component to be added
    void add(const std::shared_ptr<Component>& component) override { children.emplace_back(component); }
    //! @brief Remove the child component by index.
    //! @param index - child component index
    void remove(const std::uint32_t index) override
    {
        std::shared_ptr<Component> child = children[index];
        children.erase(children.begin() + index);
        child.reset();
    }
    //! @brief Execute operation.
    void operation() override
    {
        std::for_each(
            children.cbegin(),
            children.cend(),
            [](const auto& component)
            {
                component->operation();
            });
    }

private:
    //! @brief Child components.
    std::vector<std::shared_ptr<Component>> children;
};

//! @brief The behavior of the element that has no child in the composition.
class Leaf : public Component
{
public:
    //! @brief Construct a new Leaf object.
    //! @param id - target leaf node id
    explicit Leaf(const int id) : id(id) {}
    //! @brief Destroy the Leaf object.
    ~Leaf() override = default;

    //! @brief Execute operation.
    void operation() override { output() << "leaf " << id << " operation\n"; }

private:
    //! @brief Leaf node id.
    int id;
};
} // namespace composite

//! @brief The decorator pattern.
namespace decorator
{
extern std::ostringstream& output();

//! @brief The component to which additional responsibilities can be attached.
class Component
{
public:
    //! @brief Destroy the Component object.
    virtual ~Component() = default;

    //! @brief The related operation.
    virtual void operation() = 0;
};

//! @brief The concrete component.
class ConcreteComponent : public Component
{
public:
    //! @brief Destroy the ConcreteComponent object.
    ~ConcreteComponent() override = default;

    //! @brief The related operation.
    void operation() override { output() << "concrete component operation\n"; }
};

//! @brief Reference to the component. Add responsibilities to the component.
class Decorator : public Component
{
public:
    //! @brief Construct a new Decorator object.
    //! @param component - target component
    explicit Decorator(std::shared_ptr<Component> component) : component(std::move(component)) {}
    //! @brief Destroy the Decorator object.
    ~Decorator() override = default;

    //! @brief The related operation.
    void operation() override { component->operation(); }

private:
    //! @brief The component.
    std::shared_ptr<Component> component;
};

//! @brief The concrete decorator.
class ConcreteDecoratorA : public Decorator
{
public:
    //! @brief Construct a new ConcreteDecoratorA object.
    //! @param decorator - target decorator
    explicit ConcreteDecoratorA(std::shared_ptr<Component> decorator) : Decorator(std::move(decorator)) {}

    //! @brief The related operation.
    void operation() override
    {
        Decorator::operation();
        output() << "decorator A\n";
    }
};

//! @brief The concrete decorator.
class ConcreteDecoratorB : public Decorator
{
public:
    //! @brief Construct a new ConcreteDecoratorB object.
    //! @param decorator - target decorator
    explicit ConcreteDecoratorB(std::shared_ptr<Component> decorator) : Decorator(std::move(decorator)) {}

    //! @brief The related operation.
    void operation() override
    {
        Decorator::operation();
        output() << "decorator B\n";
    }
};
} // namespace decorator

//! @brief The facade pattern.
namespace facade
{
extern std::ostringstream& output();

//! @brief Implement complex subsystem functionality.
class SubsystemA
{
public:
    //! @brief The sub-operation.
    static void suboperation() { output() << "subsystem A method\n"; }
};

//! @brief Implement complex subsystem functionality.
class SubsystemB
{
public:
    //! @brief The sub-operation.
    static void suboperation() { output() << "subsystem B method\n"; }
};

//! @brief Implement complex subsystem functionality.
class SubsystemC
{
public:
    //! @brief The sub-operation.
    static void suboperation() { output() << "subsystem C method\n"; }
};

//! @brief Delegate client requests to appropriate subsystem and unified interface.
class Facade
{
public:
    //! @brief Construct a new Facade object.
    Facade() : subsystemA(), subsystemB(), subsystemC() {}

    //! @brief The operation 1.
    void operation1()
    {
        subsystemA->suboperation();
        subsystemB->suboperation();
    }
    //! @brief The operation 2.
    void operation2() { subsystemC->suboperation(); }

private:
    //! @brief Subsystem A.
    std::shared_ptr<SubsystemA> subsystemA;
    //! @brief Subsystem B.
    std::shared_ptr<SubsystemB> subsystemB;
    //! @brief Subsystem C.
    std::shared_ptr<SubsystemC> subsystemC;
};
} // namespace facade

//! @brief The flyweight pattern.
namespace flyweight
{
extern std::ostringstream& output();

//! @brief Receive and act on the extrinsic state through which flyweights.
class Flyweight
{
public:
    //! @brief Destroy the Flyweight object.
    virtual ~Flyweight() = default;

    //! @brief The related operation.
    virtual void operation() = 0;
};

//! @brief The unshared concrete flyweight.
class UnsharedConcreteFlyweight : public Flyweight
{
public:
    //! @brief Construct a new UnsharedConcreteFlyweight object.
    //! @param intrinsicState - target intrinsic state
    explicit UnsharedConcreteFlyweight(const int intrinsicState) : state(intrinsicState) {}
    //! @brief Destroy the UnsharedConcreteFlyweight object.
    ~UnsharedConcreteFlyweight() override = default;

    //! @brief The related operation.
    void operation() override { output() << "unshared flyweight with state " << state << '\n'; }

private:
    //! @brief Intrinsic state.
    int state;
};

//! @brief The concrete flyweight.
class ConcreteFlyweight : public Flyweight
{
public:
    //! @brief Construct a new ConcreteFlyweight object.
    //! @param intrinsicState - target intrinsic state
    explicit ConcreteFlyweight(const int intrinsicState) : state(intrinsicState) {}
    //! @brief Destroy the ConcreteFlyweight object.
    ~ConcreteFlyweight() override = default;

    //! @brief The related operation.
    void operation() override { output() << "concrete flyweight with state " << state << '\n'; }

private:
    //! @brief Intrinsic state.
    int state;
};

//! @brief Manage flyweights and ensure that flyweights are appropriately shared.
class FlyweightFactory
{
public:
    //! @brief Destroy the FlyweightFactory object.
    ~FlyweightFactory()
    {
        std::for_each(
            flies.begin(),
            flies.end(),
            [](auto& file)
            {
                file.second.reset();
            });
        flies.clear();
    }

    //! @brief Get the flyweight by key value.
    //! @param key - key value
    //! @return flyweight
    std::unique_ptr<Flyweight>& getFlyweight(const int key)
    {
        if (flies.find(key) != flies.cend())
        {
            return flies[key];
        }
        std::unique_ptr<Flyweight> fly = std::make_unique<ConcreteFlyweight>(key);
        flies.insert(std::pair<int, std::unique_ptr<Flyweight>>(key, std::move(fly)));
        return flies[key];
    }

private:
    //! @brief The flies.
    std::map<int, std::unique_ptr<Flyweight>> flies;
};
} // namespace flyweight

//! @brief The proxy pattern.
namespace proxy
{
extern std::ostringstream& output();

//! @brief Make the proxy can be used anywhere a real subject is expected.
class Subject
{
public:
    //! @brief Destroy the Subject object.
    virtual ~Subject() = default;

    //! @brief The specific request.
    virtual void request() = 0;
};

//! @brief The real object that the proxy represents.
class RealSubject : public Subject
{
public:
    //! @brief The specific request.
    void request() override { output() << "real subject request\n"; }
};

//! @brief Let the proxy access the real subject.
class Proxy : public Subject
{
public:
    //! @brief Destroy the Proxy object.
    ~Proxy() override
    {
        if (subject)
        {
            subject.reset();
        }
    }

    //! @brief The specific request.
    void request() override { realSubject().request(); }

private:
    //! @brief Real subject.
    std::unique_ptr<RealSubject> subject;

protected:
    //! @brief Get the real subject.
    //! @return real subject
    RealSubject& realSubject()
    {
        if (!subject)
        {
            subject = std::make_unique<RealSubject>();
        }
        return *subject;
    }
};
} // namespace proxy
} // namespace design_pattern::structural
