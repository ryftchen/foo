//! @file structural.hpp
//! @author ryftchen
//! @brief The declarations (structural) in the design pattern module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen.

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
    static void specificRequest();
};

//! @brief Delegate the call to an adaptee when getting a method call.
class Adapter : public Target
{
public:
    //! @brief Construct a new Adapter object.
    Adapter() : adaptee(std::make_unique<Adaptee>()) {}
    //! @brief Destroy the Adapter object.
    ~Adapter() override;

    //! @brief Generate a request.
    void request() override;

private:
    //! @brief The adaptee.
    std::unique_ptr<Adaptee> adaptee;
};

extern std::ostringstream& output();
} // namespace adapter

//! @brief The bridge pattern.
namespace bridge
{
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
    void action() override;
};

//! @brief The concrete implementor.
class ConcreteImplementorB : public Implementor
{
public:
    //! @brief Destroy the ConcreteImplementorB object.
    ~ConcreteImplementorB() override = default;

    //! @brief Implemented action.
    void action() override;
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
    void operation() override;

private:
    //! @brief The implementor.
    std::unique_ptr<Implementor> implementor;
};

extern std::ostringstream& output();
} // namespace bridge

//! @brief The composite pattern.
namespace composite
{
//! @brief Both the composite and the leaf nodes are in the composition.
class Component
{
public:
    //! @brief Destroy the Component object.
    virtual ~Component() = default;

    //! @brief Get the child component by index.
    //! @param index - child component index
    virtual std::shared_ptr<Component> getChild(const std::uint32_t index);
    //! @brief Add the child component.
    //! @param component - child component to be added
    virtual void add(const std::shared_ptr<Component>& component);
    //! @brief Remove the child component by index.
    //! @param index - child component index
    virtual void remove(const std::uint32_t index);
    //! @brief Execute all child components' operations.
    virtual void operation() = 0;
};

//! @brief The behavior of the components having children.
class Composite : public Component
{
public:
    //! @brief Destroy the Composite object.
    ~Composite() override;

    //! @brief Get the child component by index.
    //! @param index - child component index
    //! @return child component
    std::shared_ptr<Component> getChild(const std::uint32_t index) override;
    //! @brief Add the child component.
    //! @param component - child component to be added
    void add(const std::shared_ptr<Component>& component) override;
    //! @brief Remove the child component by index.
    //! @param index - child component index
    void remove(const std::uint32_t index) override;
    //! @brief Execute operation.
    void operation() override;

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
    void operation() override;

private:
    //! @brief Leaf node id.
    int id;
};

extern std::ostringstream& output();
} // namespace composite

//! @brief The decorator pattern.
namespace decorator
{
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
    void operation() override;
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
    void operation() override;

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
    void operation() override;
};

//! @brief The concrete decorator.
class ConcreteDecoratorB : public Decorator
{
public:
    //! @brief Construct a new ConcreteDecoratorB object.
    //! @param decorator - target decorator
    explicit ConcreteDecoratorB(std::shared_ptr<Component> decorator) : Decorator(std::move(decorator)) {}

    //! @brief The related operation.
    void operation() override;
};

extern std::ostringstream& output();
} // namespace decorator

//! @brief The facade pattern.
namespace facade
{
//! @brief Implement complex subsystem functionality.
class SubsystemA
{
public:
    //! @brief The sub-operation.
    static void suboperation();
};

//! @brief Implement complex subsystem functionality.
class SubsystemB
{
public:
    //! @brief The sub-operation.
    static void suboperation();
};

//! @brief Implement complex subsystem functionality.
class SubsystemC
{
public:
    //! @brief The sub-operation.
    static void suboperation();
};

//! @brief Delegate client requests to appropriate subsystem and unified interface.
class Facade
{
public:
    //! @brief Construct a new Facade object.
    Facade() : subsystemA(), subsystemB(), subsystemC() {}

    //! @brief The operation 1.
    void operation1();
    //! @brief The operation 2.
    void operation2();

private:
    //! @brief Subsystem A.
    std::shared_ptr<SubsystemA> subsystemA;
    //! @brief Subsystem B.
    std::shared_ptr<SubsystemB> subsystemB;
    //! @brief Subsystem C.
    std::shared_ptr<SubsystemC> subsystemC;
};

extern std::ostringstream& output();
} // namespace facade

//! @brief The flyweight pattern.
namespace flyweight
{
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
    void operation() override;

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
    void operation() override;

private:
    //! @brief Intrinsic state.
    int state;
};

//! @brief Manage flyweights and ensure that flyweights are appropriately shared.
class FlyweightFactory
{
public:
    //! @brief Destroy the FlyweightFactory object.
    ~FlyweightFactory();

    //! @brief Get the flyweight by key value.
    //! @param key - key value
    //! @return flyweight
    std::unique_ptr<Flyweight>& getFlyweight(const int key);

private:
    //! @brief The flies.
    std::map<int, std::unique_ptr<Flyweight>> flies;
};

extern std::ostringstream& output();
} // namespace flyweight

//! @brief The proxy pattern.
namespace proxy
{
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
    void request() override;
};

//! @brief Let the proxy access the real subject.
class Proxy : public Subject
{
public:
    //! @brief Destroy the Proxy object.
    ~Proxy() override;

    //! @brief The specific request.
    void request() override;

private:
    //! @brief Real subject.
    std::unique_ptr<RealSubject> subject;

protected:
    //! @brief Get the real subject.
    //! @return real subject
    RealSubject& realSubject();
};

extern std::ostringstream& output();
} // namespace proxy
} // namespace design_pattern::structural
